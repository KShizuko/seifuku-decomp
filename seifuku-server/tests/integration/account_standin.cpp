#include "account_standin.h"
#include <windows.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

namespace
{
	void Put16(std::vector<unsigned char>& v, size_t off, unsigned short x)	{ memcpy(&v[off], &x, 2); }
	void Put32(std::vector<unsigned char>& v, size_t off, unsigned long x)	{ memcpy(&v[off], &x, 4); }
	void PutStr(std::vector<unsigned char>& v, size_t off, const char* p, size_t n)
	{
		size_t len = p ? strlen(p) : 0;
		if (len >= n) len = n - 1;
		if (len) memcpy(&v[off], p, len);
	}
	std::vector<unsigned char> Header(unsigned short usSize, unsigned short usType)
	{
		std::vector<unsigned char> v(usSize, 0);
		Put16(v, 0, usSize);
		Put16(v, 2, usType);
		return v;
	}
}

CAccountStandIn::Cipher::Cipher() : p1(0), p2(0)
{
	int x = 0x7E;
	for (int i = 0; i < 256; i++) { t1[i] = (unsigned char)x; x = (x * (0x12 * x + 0x97) + 0x19) % 256; }
	x = 0xD3;
	for (int i = 0; i < 256; i++) { t2[i] = (unsigned char)x; x = (x * (0x48 * x + 0x51) + 0x1B) % 256; }
}

unsigned char CAccountStandIn::Cipher::Next()
{
	unsigned char t = (unsigned char)(t1[p1] ^ t2[p2]);
	if (++p1 >= 256) { p1 = 0; if (++p2 >= 256) p2 = 0; }
	return t;
}

void CAccountStandIn::Cipher::Inverse(unsigned char* buf, int nLen)
{
	for (int i = 0; i < nLen; i++)
	{
		unsigned char b = (unsigned char)(buf[i] ^ 0xAB);
		b = (unsigned char)(((b & 0x0F) << 4) + (b >> 4));
		buf[i] = (unsigned char)(b ^ Next());
	}
}

CAccountStandIn::CAccountStandIn() : m_sockListen(INVALID_SOCKET), m_sock(INVALID_SOCKET), m_nPort(0)
{
}

CAccountStandIn::~CAccountStandIn()
{
	Close();
}

bool CAccountStandIn::Listen(int nPort)
{
	m_sockListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sockListen == INVALID_SOCKET)
		return false;
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= htonl(INADDR_LOOPBACK);
	addr.sin_port			= htons((unsigned short)nPort);
	if (bind(m_sockListen, (sockaddr*)&addr, sizeof(addr)) != 0 || listen(m_sockListen, 1) != 0)
		return false;
	int len = sizeof(addr);
	if (getsockname(m_sockListen, (sockaddr*)&addr, &len) != 0)
		return false;
	m_nPort = ntohs(addr.sin_port);
	return true;
}

bool CAccountStandIn::Accept(int nMilliSecs)
{
	fd_set set;
	FD_ZERO(&set);
	FD_SET(m_sockListen, &set);
	timeval tv = { nMilliSecs / 1000, (nMilliSecs % 1000) * 1000 };
	if (select(0, &set, NULL, NULL, &tv) != 1)
		return false;
	m_sock = accept(m_sockListen, NULL, NULL);
	return m_sock != INVALID_SOCKET;
}

bool CAccountStandIn::Recv(Message& msg, int nMilliSecs)
{
	DWORD tEnd = GetTickCount() + nMilliSecs;
	while (true)
	{
		if (m_plain.size() >= 4)
		{
			unsigned short usSize;
			memcpy(&usSize, &m_plain[0], 2);
			if (usSize < 4)
				return false;
			if (m_plain.size() >= usSize)
			{
				msg.buf.assign(m_plain.begin(), m_plain.begin() + usSize);
				memcpy(&msg.usType, &m_plain[2], 2);
				m_plain.erase(m_plain.begin(), m_plain.begin() + usSize);
				return true;
			}
		}
		if (m_sock == INVALID_SOCKET)
			return false;
		long nLeft = (long)(tEnd - GetTickCount());
		if (nLeft <= 0)
			return false;
		fd_set set;
		FD_ZERO(&set);
		FD_SET(m_sock, &set);
		timeval tv = { nLeft / 1000, (nLeft % 1000) * 1000 };
		if (select(0, &set, NULL, NULL, &tv) != 1)
			return false;
		unsigned char buf[4096];
		int n = recv(m_sock, (char*)buf, sizeof(buf), 0);
		if (n <= 0)
			return false;
		m_wireIn.insert(m_wireIn.end(), buf, buf + n);
		m_cipherIn.Inverse(buf, n);
		m_plain.insert(m_plain.end(), buf, buf + n);
	}
}

bool CAccountStandIn::SendRaw(const void* pBuf, int nLen)
{
	if (m_sock == INVALID_SOCKET || nLen <= 0)
		return false;
	std::vector<unsigned char> wire((const unsigned char*)pBuf, (const unsigned char*)pBuf + nLen);
	m_cipherOut.Inverse(&wire[0], nLen);
	m_wireOut.insert(m_wireOut.end(), wire.begin(), wire.end());
	int nSent = 0;
	while (nSent < nLen)
	{
		int n = send(m_sock, (const char*)&wire[nSent], nLen - nSent, 0);
		if (n <= 0)
			return false;
		nSent += n;
	}
	return true;
}

bool CAccountStandIn::SendLoginAnswer(unsigned long idServer, unsigned long dwCheckData, unsigned short usVersion, const char* pszName)
{
	std::vector<unsigned char> v = Header(SIZE_LOGIN, MSG_LOGIN);
	Put32(v, 4, idServer);
	Put32(v, 8, dwCheckData);
	Put16(v, 0x0C, usVersion);
	PutStr(v, 0x0E, pszName, 16);
	return SendRaw(&v[0], (int)v.size());
}

bool CAccountStandIn::SendConnect(unsigned long idAccount, unsigned long dwData, const char* pszInfo)
{
	std::vector<unsigned char> v = Header(SIZE_CONNECT, MSG_CONNECT);
	Put32(v, 4, idAccount);
	Put32(v, 8, dwData);
	PutStr(v, 0x0C, pszInfo, 16);
	return SendRaw(&v[0], (int)v.size());
}

bool CAccountStandIn::SendFee(unsigned long idAccount, unsigned char ucType)
{
	std::vector<unsigned char> v = Header(SIZE_FEE, MSG_FEE);
	Put32(v, 4, idAccount);
	v[8] = ucType;
	return SendRaw(&v[0], (int)v.size());
}

bool CAccountStandIn::SendQueryFee(unsigned long idAccount, unsigned long dwData, int nTime, unsigned char ucType)
{
	std::vector<unsigned char> v = Header(SIZE_QUERYFEE, MSG_QUERYFEE);
	Put32(v, 4, idAccount);
	Put32(v, 8, dwData);
	Put32(v, 0x0C, (unsigned long)nTime);
	v[0x10] = ucType;
	return SendRaw(&v[0], (int)v.size());
}

void CAccountStandIn::Close()
{
	if (m_sock != INVALID_SOCKET)		{ closesocket(m_sock); m_sock = INVALID_SOCKET; }
	if (m_sockListen != INVALID_SOCKET)	{ closesocket(m_sockListen); m_sockListen = INVALID_SOCKET; }
}
