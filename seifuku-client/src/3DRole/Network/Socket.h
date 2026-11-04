#ifndef _SOCKET_H
#define _SOCKET_H

#include <windows.h>
#include <winsock.h>
#include <assert.h>
#include "EncryptClient.h"

#define	ENCRYPT
#define ACCOUNT_KEY1
#define ACCOUNT_KEY2
#define LOGIN_KEY1
#define LOGIN_KEY2
#ifdef	LOGFILE
void LogSave (const char* fmt, ...);
#endif

#define BLOCKSECONDS	30
#define INBUFSIZE	(64*1024)
#define OUTBUFSIZE	(8*1024)
#define	SOCKETLOGFILE	"Socket.h"

template <unsigned char a1, unsigned char b1, unsigned char c1, unsigned char fst1, 
			unsigned char a2, unsigned char b2, unsigned char c2, unsigned char fst2>
class CGameSocket {
protected:
	char	m_bufOutput[OUTBUFSIZE];
	int		m_nOutbufLen;
	SOCKET	m_sockClient;
	BOOL	m_bState;
	char	m_bufInput[INBUFSIZE];
	int		m_nInbufLen;
	int		m_nInbufStart;
#ifdef	ENCRYPT
	typedef	CEncryptClient<a1, b1, c1, fst1, a2, b2, c2, fst2>	ENCRYPTCLIENT;
	ENCRYPTCLIENT	m_cSendEncrypt;
	ENCRYPTCLIENT	m_cRecvEncrypt;
#endif

#ifdef	_DEBUG_X
	int		pf_nInbufAverage;
	int		pf_nInbufMax;
	int		pf_nInbufCount;
	int		pf_nOutbufAverage;
	int		pf_nOutbufMax;
	int		pf_nOutbufCount;
#endif

public:
	CGameSocket(void);
	BOOL	Create(char* pszServerIP, int nServerPort, BOOL bKeepAlive = FALSE);
	BOOL	SendMsg(void* pBuf, int nSize);
	BOOL	ReceiveMsg(void* pBuf, int& nSize);
	BOOL	Flush(void);
	BOOL	Check(void);
	void	Destroy(void);
#ifdef	ENCRYPT
	void	ChangeCode(DWORD dwData) { m_cSendEncrypt.ChangeCode(dwData); }

#endif
private:
	BOOL	RecvFromSock(void);
};

#ifdef	_DEBUG_X
int		pf_nInbufAverage;
int		pf_nInbufMax;
int		pf_nInbufCount;
int		pf_nOutbufAverage;
int		pf_nOutbufMax;
int		pf_nOutbufCount;
#endif

template <unsigned char a1, unsigned char b1, unsigned char c1, unsigned char fst1, 
			unsigned char a2, unsigned char b2, unsigned char c2, unsigned char fst2>
CGameSocket<a1, b1, c1, fst1, a2, b2, c2, fst2>::CGameSocket()
{ 
	m_bState	= false;

#ifndef	TEST
	DeleteFile(SOCKETLOGFILE);
#endif

}

template <unsigned char a1, unsigned char b1, unsigned char c1, unsigned char fst1, 
			unsigned char a2, unsigned char b2, unsigned char c2, unsigned char fst2>
BOOL CGameSocket<a1, b1, c1, fst1, a2, b2, c2, fst2>::Create(char* pszServerIP, int nServerPort, BOOL bKeepAlive )
{
	try
	{
		if(m_bState)
			Destroy();

		try
		{
			if(pszServerIP == 0 || strlen(pszServerIP) > 15)
			{
#ifdef	LOGFILE
				LogSave("ERROR: IP addr too long.");
#endif
				exit(5);
			}
		}
		catch(...)
		{
#ifdef	LOGFILE
			LogSave("CATCH ERROR: Init() parameter [pszServerIP] is invalid point.");
#endif
			exit(5);
		}

		WSADATA		cData;
		if(WSAStartup(0x0101, &cData) != 0)
		{
#ifdef	LOGFILE
			LogSave("ERROR: WSAStartup() failed.");
#endif
			return false;
		}
		if(cData.wVersion != 0x0101)
		{
#ifdef	LOGFILE
			LogSave("ERROR: unmatched Socket version[0x%04X].", cData.wVersion);
#endif
			WSACleanup();
			return false;
		}

		m_sockClient = socket(PF_INET, SOCK_STREAM, 0);
		if(m_sockClient == INVALID_SOCKET)
		{
			int err = WSAGetLastError();
#ifdef	LOGFILE
			LogSave("ERROR %d: Init socket() failed.", err);
#endif
			WSACleanup();
			return FALSE;
		}

		if(bKeepAlive)
		{
			int		optval=1;
			if(setsockopt(m_sockClient, SOL_SOCKET, SO_KEEPALIVE, (char *) &optval, sizeof(optval)))
			{
				int err = WSAGetLastError();
#ifdef	LOGFILE
				LogSave("ERROR %d: Init setsockopt() failed.\n", err);
#endif
				closesocket(m_sockClient);
				WSACleanup();
				return FALSE;
			}
		}

		unsigned long	i = 1;
		if(ioctlsocket(m_sockClient, FIONBIO, &i))
		{
			int err = WSAGetLastError();
#ifdef	LOGFILE
			LogSave("ERROR %d: Init ioctlsocket() failed.", err);
#endif
			closesocket(m_sockClient);
			WSACleanup();
			return FALSE;
		}

		unsigned long serveraddr = inet_addr(pszServerIP);
		if(serveraddr == INADDR_NONE)
		{
#ifdef	LOGFILE
			LogSave("ERROR: IP地址[%s]格式错误.", pszServerIP);
#endif
			closesocket(m_sockClient);
			WSACleanup();
			return false;
		}

		sockaddr_in	addr_in;
		memset((void *)&addr_in, sizeof(addr_in), 0);
		addr_in.sin_family = AF_INET;
		addr_in.sin_port = htons(nServerPort);
		addr_in.sin_addr.s_addr = serveraddr;
		if(connect(m_sockClient, (sockaddr *)&addr_in, sizeof(addr_in)) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if(err != WSAEWOULDBLOCK)
			{
#ifdef	LOGFILE
				LogSave("ERROR %d: Connect() failed.", err);
#endif
				closesocket(m_sockClient);
				WSACleanup();
				return false;
			}
			else
			{
				timeval timeout;
				timeout.tv_sec	= BLOCKSECONDS;
				timeout.tv_usec	= 0;
				fd_set writeset, exceptset;
				FD_ZERO(&writeset);
				FD_ZERO(&exceptset);
				FD_SET(m_sockClient, &writeset);
				FD_SET(m_sockClient, &exceptset);

				int ret = select(FD_SETSIZE, NULL, &writeset, &exceptset, &timeout);
				if (ret == 0)
				{
#ifdef	LOGFILE
					LogSave("ERROR : select连接超时。");
#endif
					closesocket(m_sockClient);
					WSACleanup();
					return false;
				}
				else if(ret < 0)
				{
					int	err = WSAGetLastError();
#ifdef	LOGFILE
					LogSave("ERROR %d: select连接错误。", err);
#endif
					closesocket(m_sockClient);
					WSACleanup();
					return false;
				}
				else
				{
					if(FD_ISSET(m_sockClient, &exceptset))
					{
#ifdef	LOGFILE
						LogSave("ERROR : select连接例外。");
#endif
						closesocket(m_sockClient);
						WSACleanup();
						return false;
					}
				}
			}
		}

		m_nInbufLen		= 0;
		m_nInbufStart	= 0;
		m_nOutbufLen	= 0;
		m_bState	= true;
#ifdef	ENCRYPT
		m_cSendEncrypt.Init();
		m_cRecvEncrypt.Init();
#endif

#ifdef	_DEBUG_X
		pf_nInbufAverage	= 0;
		pf_nInbufMax		= 0;
		pf_nInbufCount		= 0;
		pf_nOutbufAverage	= 0;
		pf_nOutbufMax		= 0;
		pf_nOutbufCount		= 0;
#endif

		return true;
	}
	catch(...) { 
#ifdef	LOGFILE
		LogSave("CATCH ERROR: Init() failed."); 
#endif
		exit(5); 
	}
}

template <unsigned char a1, unsigned char b1, unsigned char c1, unsigned char fst1, 
			unsigned char a2, unsigned char b2, unsigned char c2, unsigned char fst2>
BOOL CGameSocket<a1, b1, c1, fst1, a2, b2, c2, fst2>::SendMsg(void* pBuf, int nSize)
{
	try
	{
		if(pBuf == 0 || nSize <= 0 || nSize > _MAX_MSGSIZE)
		{
#ifdef	LOGFILE
			LogSave("ERROR: SendMsg()参数错误.");
#endif
			exit(5);
		}

		if(!m_bState)
		{
#ifdef	LOGFILE
			LogSave("ERROR: 连接已经断开，无法SEND.");
#endif
			return false;
		}

		try
		{
			int packsize = *(unsigned short *)pBuf;
			if(packsize <= 0 || packsize > _MAX_MSGSIZE)
			{
#ifdef	LOGFILE
				LogSave("ERROR：想发送一个消息包尺寸超长[%d]的消息!!!", packsize);
#endif
				Destroy();
				exit(5);
			}
			if(packsize != nSize)
			{
#ifdef	LOGFILE
				LogSave("ERROR：CGameSocket::SendMsg()长度参数[%d]和消息头长度[%d]不相同!!!", nSize, packsize);
#endif
				Destroy();
				exit(5);
			}
		}
		catch(...)
		{
#ifdef	LOGFILE
			LogSave("CATCH ERROR: SendMsg() parameter pBuf is invalid point.");
#endif
			exit(5);
		}

		if(m_nOutbufLen + nSize > OUTBUFSIZE)
		{
			Flush();
			if(m_nOutbufLen + nSize > OUTBUFSIZE)
			{
#ifdef	LOGFILE
				LogSave("ERROR: OUTBUF溢出!!! 可能是 send() 阻塞");
#endif
				Destroy();
				return FALSE;
			}
		}
		memcpy(m_bufOutput + m_nOutbufLen, pBuf, nSize);
#ifdef	ENCRYPT
		if(a1 || b1 || c1 || fst1 || a2 || b2 || c2 || fst2)
			m_cSendEncrypt.Encrypt((unsigned char *)m_bufOutput + m_nOutbufLen, nSize);
#endif
		m_nOutbufLen += nSize;
		assert(m_nOutbufLen <= OUTBUFSIZE);

#ifdef	_DEBUG_X
		if(pf_nOutbufCount >= PF_OUTBUFCOUNT)
		{
#ifdef	LOGFILE
			LogSave("Message: outbuf len average is %d.", pf_nOutbufAverage / PF_OUTBUFCOUNT);
			LogSave("Message: outbuf len max is %d.", pf_nOutbufMax);
#endif
			pf_nOutbufAverage	= 0;
			pf_nOutbufMax		= 0;
			pf_nOutbufCount		= 0;
		}
		else
		{
			pf_nOutbufAverage = pf_nOutbufAverage + m_nOutbufLen;
			if(pf_nOutbufMax < m_nOutbufLen)
				pf_nOutbufMax = m_nOutbufLen;
			pf_nOutbufCount++;
		}
#endif

		return true;

	}catch(...){ 
#ifdef	LOGFILE
		LogSave("CATCH ERROR: SendMsg()."); 
#endif
		exit(5); 
	}
}

template <unsigned char a1, unsigned char b1, unsigned char c1, unsigned char fst1, 
			unsigned char a2, unsigned char b2, unsigned char c2, unsigned char fst2>
BOOL CGameSocket<a1, b1, c1, fst1, a2, b2, c2, fst2>::ReceiveMsg(void* pBuf, int& nSize)
{
	try
	{
		if(pBuf == NULL)
		{
#ifdef	LOGFILE
			LogSave("ERROR: ReceiveMsg()参数错误.");
#endif
			exit(5);
		}

		if(nSize <= 0 || nSize >_MAX_MSGSIZE)
		{
#ifdef	ALPHA_X
#ifdef	LOGFILE
			LogSave("警告: ClientSocketReceiveMsg(void* pBuf, int& nSize[%d])参数错误, 忽略", nSize);
#endif
#endif
		}
		try
		{
			char temp = *((char *)pBuf);
		}
		catch(...)
		{
#ifdef	LOGFILE
			LogSave("CATCH ERROR: ReceiveMsg() parameter [pBuf] is invalid point.");
#endif
			exit(5);
		}

		if(m_nInbufLen < 2)
		{
			if(!RecvFromSock() || m_nInbufLen < 2)
				return false;
		}

		int packsize = (u_char)m_bufInput[m_nInbufStart] + 
					(u_char)m_bufInput[(m_nInbufStart + 1) % INBUFSIZE] * 256;

		if(packsize <= 0 || packsize > _MAX_MSGSIZE)
		{
#ifdef	LOGFILE
			LogSave("ERROR：接收到一个消息包尺寸超长[%d]的消息!!!", packsize);
#endif
			m_nInbufLen = 0;
			m_nInbufStart = 0;
			Destroy();
			exit(5);
		}

		if(packsize > m_nInbufLen)
		{
			if(!RecvFromSock() || packsize > m_nInbufLen)
				return false;
		}

		if(m_nInbufStart + packsize > INBUFSIZE)
		{
			int copylen = INBUFSIZE - m_nInbufStart;
			assert(copylen < packsize);
			assert(m_nInbufStart + copylen == INBUFSIZE);
			try{
				memcpy(pBuf, m_bufInput + m_nInbufStart, copylen);
			}catch(...){
#ifdef	LOGFILE
				LogSave("CATCH ERROR：memcpy(pBuf, m_bufInput + m_nInbufStart[%d], copylen[%d]);", m_nInbufStart, copylen);
#endif
				exit(5);
			}
			assert(packsize - copylen < INBUFSIZE);
			try{
				memcpy((unsigned char *)pBuf + copylen, m_bufInput, packsize - copylen);
			}catch(...){
#ifdef	LOGFILE
				LogSave("CATCH ERROR：memcpy(pBuf + copylen[%d], m_bufInput, packsize[%d] - copylen);", copylen, packsize);
#endif
				exit(5);
			}
			nSize = packsize;
		}
		else
		{
			assert(m_nInbufStart + packsize <= INBUFSIZE);
			try{
				memcpy(pBuf, m_bufInput + m_nInbufStart, packsize);
			}catch(...){
#ifdef	LOGFILE
				LogSave("CATCH ERROR：memcpy(pBuf, m_bufInput + m_nInbufStart[%d], packsize[%d]);", m_nInbufStart, packsize);
#endif
				exit(5);
			}
			nSize = packsize;
		}

		m_nInbufStart = (m_nInbufStart + packsize) % INBUFSIZE;
		m_nInbufLen -= packsize;
		assert(m_nInbufStart >= 0 && m_nInbufStart < INBUFSIZE);
		assert(m_nInbufLen >= 0);
		return	true;

	}catch(...){ 
#ifdef	LOGFILE
		LogSave("CATCH ERROR: ReceiveMsg()."); 
#endif
		exit(5); 
	}
}

template <unsigned char a1, unsigned char b1, unsigned char c1, unsigned char fst1, 
			unsigned char a2, unsigned char b2, unsigned char c2, unsigned char fst2>
BOOL CGameSocket<a1, b1, c1, fst1, a2, b2, c2, fst2>::RecvFromSock(void)
{
	if(m_bState && m_nInbufLen < INBUFSIZE)
	{
		int	savelen, savepos;
		if(m_nInbufStart + m_nInbufLen < INBUFSIZE)
			savelen = INBUFSIZE - (m_nInbufStart + m_nInbufLen);
		else
			savelen = INBUFSIZE - m_nInbufLen;
		savepos = (m_nInbufStart + m_nInbufLen) % INBUFSIZE;
		assert(savepos + savelen <= INBUFSIZE);
		int inlen = recv(m_sockClient, m_bufInput + savepos, savelen, 0);
		if(inlen > 0)
		{
#ifdef	ENCRYPT
			if(a1 || b1 || c1 || fst1 || a2 || b2 || c2 || fst2)
				m_cRecvEncrypt.Encrypt((unsigned char *)m_bufInput + savepos, inlen);
#endif
			m_nInbufLen += inlen;
			assert(m_nInbufLen <= INBUFSIZE);

			if(inlen == savelen && m_nInbufLen < INBUFSIZE)
			{
				int savelen = INBUFSIZE - m_nInbufLen;
				int savepos = (m_nInbufStart + m_nInbufLen) % INBUFSIZE;
				assert(savepos + savelen <= INBUFSIZE);
				int inlen = recv(m_sockClient, m_bufInput + savepos, savelen, 0);
				if(inlen > 0)
				{
#ifdef	ENCRYPT
					if(a1 || b1 || c1 || fst1 || a2 || b2 || c2 || fst2)
						m_cRecvEncrypt.Encrypt((unsigned char *)m_bufInput + savepos, inlen);
#endif	
					m_nInbufLen += inlen;
					assert(m_nInbufLen <= INBUFSIZE);
				}
				else if(inlen == 0)
				{
#ifdef	LOGFILE
					LogSave("ERROR: 服务器已经关闭!");
#endif
					Destroy();
					return false;
				}
				else
				{
					int err = WSAGetLastError();
					if(err != WSAEWOULDBLOCK)
					{
#ifdef	LOGFILE
						LogSave("ERROR %d: recv failed.", err);
#endif
						Destroy();
						return false;
					}
				}
			}
		}
		else if(inlen == 0)
		{
#ifdef	LOGFILE
			LogSave("ERROR: 服务器已经关闭!");
#endif
			Destroy();
			return false;
		}
		else
		{
			int err = WSAGetLastError();
			if(err != WSAEWOULDBLOCK)
			{
#ifdef	LOGFILE
				LogSave("ERROR %d: recv failed.", err);
#endif
				Destroy();
				return false;
			}
		}
	}

	return true;
}

template <unsigned char a1, unsigned char b1, unsigned char c1, unsigned char fst1, 
			unsigned char a2, unsigned char b2, unsigned char c2, unsigned char fst2>
BOOL CGameSocket<a1, b1, c1, fst1, a2, b2, c2, fst2>::Flush(void)
{
	try{
		if(m_nOutbufLen > 0)
		{
			int	outsize;
			outsize = send(m_sockClient, m_bufOutput, m_nOutbufLen, 0);
			if(outsize > 0)
			{
				if(m_nOutbufLen - outsize > 0)
					memcpy(m_bufOutput, m_bufOutput + outsize, m_nOutbufLen - outsize);
				m_nOutbufLen -= outsize;
				assert(m_nOutbufLen >= 0);
			}
			else
			{
				int err = WSAGetLastError();
				if(err != WSAEWOULDBLOCK)
				{
#ifdef	LOGFILE
					LogSave("ERROR: 网络错误[%d]，无法发送", err);
#endif
					Destroy();
					return false;
				}
			}
		}

		return true;

	}catch(...){
#ifdef	LOGFILE
		LogSave("CATCH ERROR: Flush().");
#endif
		exit(5);
	}
}

template <unsigned char a1, unsigned char b1, unsigned char c1, unsigned char fst1, 
			unsigned char a2, unsigned char b2, unsigned char c2, unsigned char fst2>
BOOL CGameSocket<a1, b1, c1, fst1, a2, b2, c2, fst2>::Check(void)
{
	try{
		if(m_bState)
		{
			char	buf[1];
			int	ret = recv(m_sockClient, buf, 1, MSG_PEEK);
			if(ret == 0)
			{
#ifdef	LOGFILE
				LogSave("ERROR: 服务器已经关闭.");
#endif
				Destroy();
				return false;
			}
			else if(ret < 0)
			{
				int err = WSAGetLastError();
				if(err != WSAEWOULDBLOCK)
				{
#ifdef	LOGFILE
					LogSave("ERROR %d: 网络错误.", err);
#endif
					Destroy();
					return false;
				}
				else
				{
					return true;
				}
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}

	}catch(...){
#ifdef	LOGFILE
		LogSave("CATCH ERROR: Check().");
#endif
		exit(5);
	}
}

template <unsigned char a1, unsigned char b1, unsigned char c1, unsigned char fst1, 
			unsigned char a2, unsigned char b2, unsigned char c2, unsigned char fst2>
void CGameSocket<a1, b1, c1, fst1, a2, b2, c2, fst2>::Destroy(void)
{
	try
	{
		if(m_bState)
		{
			closesocket(m_sockClient);
			m_sockClient = INVALID_SOCKET;
			WSACleanup();
			m_nOutbufLen = 0;
			m_bState	= false;
		}

	}catch(...){
#ifdef	LOGFILE
		LogSave("CATCH ERROR: Destroy().");
#endif
		exit(5);
	}
}

typedef	CGameSocket<0xFA, 0x0F, 0x13, 0x9D, 0xA4, 0x79, 0x6D, 0x62>	CLoginSocket;


#endif
