#define _WINSOCKAPI_

#define FD_SETSIZE      2048
#include <winsock2.h>
#include <windows.h>
#include <assert.h>
#include "EncryptServer.h"
#include "SSocket.h"
#include "define.h"

#define ENCRYPT

bool	ServerSocketInit()
{
	WSADATA		wsaData;
	int ret = WSAStartup(0x0002, &wsaData);
	if(ret!=0)
	{
		LOGERROR("ERROR: Init WSAStartup() failed.");
		return false;
	}

	if(LOBYTE(wsaData.wVersion) != 0x02 || HIBYTE(wsaData.wVersion) != 0x00)
	{
		LOGERROR("ERROR: WSAStartup Version not match 2.0");
		return false;
	}

	return true;
}

bool	ServerSocketFinal()
{
	WSACleanup();
	return true;
}

CListenSocket::CListenSocket()
{
	m_sock = INVALID_SOCKET;
}

bool		CListenSocket::Listen(int nPort, int nSndBuf , int nRcvBuf )
{
	if(m_sock != INVALID_SOCKET)
	{
		Close();
		m_sock = INVALID_SOCKET;
	}

	SOCKADDR_IN		sin;
	m_sock=socket(AF_INET, SOCK_STREAM, 0);
	if(m_sock==INVALID_SOCKET)
	{
		LOGERROR("ERROR: Init socket() failed.");
		return false;
	}

	int		optval=1;
	if(setsockopt(m_sock, SOL_SOCKET, SO_KEEPALIVE, (char *) &optval, sizeof(optval)))
	{
		closesocket(m_sock);
		LOGERROR("setsockopt() set SO_KEEPALIVE failed.");
		return false;
	}

			optval = nSndBuf;
	if(nSndBuf && setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, (char *) &optval, sizeof(optval)))
	{
		int err = WSAGetLastError();
		LOGERROR("setsockopt() set SO_SNDBUF failed[%d].", err);
		Close();
		return false;
	}

			optval = nRcvBuf;
	if(nRcvBuf && setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char *) &optval, sizeof(optval)))
	{
		int err = WSAGetLastError();
		LOGERROR("setsockopt() set SO_RCVBUF failed[%d].", err);
		Close();
		return false;
	}

	if(setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof(optval)))
	{
		closesocket(m_sock);
		LOGERROR("ERROR: Init setsockopt() failed.");
		return false;
	}

	sin.sin_family=AF_INET;
	sin.sin_addr.S_un.S_addr=0;
	sin.sin_port=htons(nPort);
	if(bind(m_sock, (LPSOCKADDR) &sin, sizeof(sin)))
	{
		closesocket(m_sock);
		LOGERROR("ERROR: Init bind() failed.");
		return false;
	}

	unsigned long	i = 1;
	if(ioctlsocket(m_sock, FIONBIO, &i))
	{
		closesocket(m_sock);
		LOGERROR("ERROR: Init ioctlsocket() failed.");
		return false;
	}

	if(listen(m_sock, 5))
	{
		closesocket(m_sock);
		LOGERROR("ERROR: Init listen() failed.");
		return false;
	}

	return true;
}

SOCKET		CListenSocket::Accept()
{
	fd_set	readmask;
	FD_ZERO(&readmask);
	FD_SET(m_sock, &readmask);

	struct timeval	timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	if(select(FD_SETSIZE, &readmask, (fd_set *) 0, (fd_set *) 0, &timeout) > 0)
	{
		if(FD_ISSET(m_sock, &readmask))
		{
			struct sockaddr_in	addr;
			int	   len = sizeof(addr);
			SOCKET newsock = accept(m_sock, (sockaddr *) &addr, (int *) &len);

			if(newsock == INVALID_SOCKET)
				LOGERROR("ERROR: accept() failed.");
			else
				return newsock;
		}
	}
	return INVALID_SOCKET;
}

void		CListenSocket::Close()
{
	if(m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
}

bool CServerSocket::Init()
{
	DWORD	dwIP	= GetPeerIP();
	m_bLocal	= false;
	if(dwIP == inet_addr("127.0.0.1"))
		m_bLocal	= true;
	if(m_bLocal)
		return true;

	return true;
}

const char* CServerSocket::GetPacket(int* pLen, BOOL bFromNet )
{
	CHECKF(pLen);

	*pLen = 0;

	if(bFromNet && m_sock != INVALID_SOCKET && m_nLen < RECV_BUFFER_SIZE)
	{
		int ret = recv(m_sock, m_bufMsg + m_nLen, RECV_BUFFER_SIZE - m_nLen, 0);
		if(ret > 0)
		{
#ifdef	ENCRYPT
			if(!m_bLocal)
				m_cEncryptRecv.Encrypt((unsigned char *)m_bufMsg + m_nLen, ret);
#endif
			m_nLen += ret;
		}
		else if(ret == 0)
		{
			LOGDEBUG("DEBUG: client closed the SOCKET.");
			Close();
		}
		else if(ret < 0)
		{
			int err = WSAGetLastError();
			if(err != WSAEWOULDBLOCK)
			{
				LOGDEBUG("DEBUG: network error, SOCKET failed.");
				Close(true);
			}
		}
	}

	*pLen	= m_nLen;
	return m_bufMsg;
}

bool CServerSocket::ClearPacket	(int nLen)
{
	ASSERT(nLen <= m_nLen);

	if(m_nLen - nLen > 0)
		memcpy(m_bufMsg, m_bufMsg + nLen, m_nLen - nLen);

	m_nLen	-= nLen;
	if(m_nLen < 0)
	{
		m_nLen = 0;
		return false;
	}
	return true;
}

bool CServerSocket::IsOpen()
{
	if(m_sock == INVALID_SOCKET)
		return false;

	return true;
}

void CServerSocket::Close(BOOL bNoLinger )
{
	if(m_sock != INVALID_SOCKET)
	{
		LOGDEBUG("DEBUG: SOCKET closing.");
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		m_nLen = 0;
		LOGDEBUG("DEBUG: SOCKET closed.");
	}
}

DWORD	CServerSocket::GetPeerIP()
{
	if(!IsOpen())
		return NULL;

	sockaddr_in	inAddr;
	memset(&inAddr, 0, sizeof(inAddr));
	int		nLen = sizeof(inAddr);
	if(getpeername(m_sock, (sockaddr *)&inAddr, &nLen))
	{
		int err = WSAGetLastError();
		LOGERROR("ERROR: getpeername() failed.");
		Close(true);
		return NULL;
	}

	return	inAddr.sin_addr.S_un.S_addr;
}

int	CServerSocket::SetBufSize(int nSndBuf, int nRcvBuf)
{
	if(!IsOpen())
		return false;

	int		optval = nSndBuf;
	if(nSndBuf && setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, (char *) &optval, sizeof(optval)))
	{
		int err = WSAGetLastError();
		LOGERROR("setsockopt() set SO_SNDBUF failed[%d].", err);
		Close();
		return false;
	}

			optval = nRcvBuf;
	if(nRcvBuf && setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char *) &optval, sizeof(optval)))
	{
		int err = WSAGetLastError();
		LOGERROR("setsockopt() set SO_RCVBUF failed[%d].", err);
		Close();
		return false;
	}
	return true;
}

CServerSocket* CServerSocket::CreateNew(SOCKET sock)
{
	CServerSocket* pSocket = ::new CServerSocket(sock);

#ifdef	ENCRYPT
		pSocket->m_cEncryptSend.Init();
		pSocket->m_cEncryptRecv.Init();
#endif
	if(pSocket && !pSocket->Init())
	{
		pSocket->Close();
		return NULL;
	}
	return	pSocket;
}

void CServerSocket::Destroy(CServerSocket* pSocket)
{
	::delete	pSocket;
}

bool CServerSocket::SendPacket(const char* pack, int nLen)
{
	if(!IsOpen())
		return false;
	if(nLen > MAX_PACKETSIZE)
		return false;
#ifdef	ENCRYPT
		char	buf[MAX_PACKETSIZE];
	if(!m_bLocal)
	{
		memcpy(buf, pack, nLen);
		pack = buf;
		m_cEncryptSend.Encrypt((unsigned char *)pack, nLen);
	}
#endif
	int ret = send(m_sock, pack, nLen, 0);
	if(ret == nLen)
	{
		return true;
	}
	else
	{
		Close(true);
		return false;
	}
}
