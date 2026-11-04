#define FD_SETSIZE      2048
#include "ClientSocket.h"

CClientSocket::CClientSocket()
{
	m_sock		= INVALID_SOCKET;
	m_nLen		= 0;
	m_nSendLen	= 0;
	m_nState	= STATE_CLOSED;

	m_szIP[0]	= 0;
}

bool CClientSocket::Open(const char* szName, int nPort, int nBlockSecs , int nSndBuf , int nRcvBuf )
{
	ASSERT(m_nState == STATE_CLOSED);

	SafeCopy(m_szIP, szName, IPSTR_SIZE);
	m_nPort		= nPort;
	m_nSndSize	= nSndBuf;
	m_nRcvSize	= nRcvBuf;

	if(IsOpen())
		return true;

	if(m_nState == STATE_CLOSED || m_sock == INVALID_SOCKET)
	{
#ifdef	ENCRYPT_ENABLE
		m_cEncryptSend.Init();
		m_cEncryptRecv.Init();
#endif
		ASSERT(m_sock == INVALID_SOCKET);
		m_sock = socket(PF_INET, SOCK_STREAM, 0);
		if(m_sock == INVALID_SOCKET)
		{
			LOGERROR("Init socket() failed.");
			return false;
		}

		m_nLen		= 0;

		int		optval=1;
		if(setsockopt(m_sock, SOL_SOCKET, SO_KEEPALIVE, (char *) &optval, sizeof(optval)))
		{
			Close();
			LOGERROR("setsockopt() set SO_KEEPALIVE failed.");
			return false;
		}

				optval = m_nSndSize;
		if(m_nSndSize && setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, (char *) &optval, sizeof(optval)))
		{
			int err = WSAGetLastError();
			LOGERROR("setsockopt() set SO_SNDBUF failed[%d].", err);
			Close();
			return false;
		}

				optval = m_nRcvSize;
		if(m_nRcvSize && setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char *) &optval, sizeof(optval)))
		{
			int err = WSAGetLastError();
			LOGERROR("setsockopt() set SO_RCVBUF failed[%d].", err);
			Close();
			return false;
		}

		unsigned long	i = 1;
		if(ioctlsocket(m_sock, FIONBIO, &i))
		{
			LOGERROR("Init ioctlsocket() failed.");
			Close();
			return false;
		}

		UINT	nAddr = inet_addr(m_szIP);
		if(nAddr == INADDR_NONE)
		{
			hostent	* hp;
			hp = gethostbyname(m_szIP);
			if(hp == 0)
			{
				LOGERROR("Bad account server host name.");
				Close();
				return false;
			}
			nAddr = *(UINT*)(hp->h_addr_list[0]);
		}

		sockaddr_in	addr_in;
		memset((void *)&addr_in, sizeof(addr_in), 0);
		addr_in.sin_family = AF_INET;
		addr_in.sin_port = htons(m_nPort);
		addr_in.sin_addr.s_addr = nAddr;
		int ret = connect(m_sock, (sockaddr *)&addr_in, sizeof(addr_in));
		if(ret == -1)
		{
			int err = WSAGetLastError();
			if(err != WSAEWOULDBLOCK)
			{
				Close();
				return false;
			}
		}

		m_nState = STATE_CONNECT;
	}
	if(m_sock == INVALID_SOCKET)
	{
		Close();
		return false;
	}

	ASSERT(m_nState == STATE_CONNECT);
	timeval timeout;
	timeout.tv_sec	= nBlockSecs;
	timeout.tv_usec	= 0;
	fd_set writeset, exceptset;
	FD_ZERO(&writeset);
	FD_ZERO(&exceptset);
	FD_SET(m_sock, &writeset);
	FD_SET(m_sock, &exceptset);

	int ret = select(FD_SETSIZE, NULL, &writeset, &exceptset, &timeout);
	if(ret == SOCKET_ERROR)
	{
		Close();
		return false;
	}
	if (ret > 0 && FD_ISSET(m_sock, &exceptset))
	{
		Close();
		return false;
	}
	else if(ret > 0 && FD_ISSET(m_sock, &writeset))
	{
		m_nState	= STATE_OPEN;
		return true;
	}

	return false;
}

bool CClientSocket::Open(int nBlockSecs )
{
	ASSERT(m_szIP[0]);

	if(IsOpen())
		return true;

	if(m_nState == STATE_CLOSED || m_sock == INVALID_SOCKET)
	{
#ifdef	ENCRYPT_ENABLE
		m_cEncryptSend.Init();
		m_cEncryptRecv.Init();
#endif
		ASSERT(m_sock == INVALID_SOCKET);
		m_sock = socket(PF_INET, SOCK_STREAM, 0);
		if(m_sock == INVALID_SOCKET)
		{
			LOGERROR("Init socket() failed.");
			return false;
		}

		m_nLen		= 0;

		int		optval=1;
		if(setsockopt(m_sock, SOL_SOCKET, SO_KEEPALIVE, (char *) &optval, sizeof(optval)))
		{
			Close();
			LOGERROR("setsockopt() set SO_KEEPALIVE failed.");
			return false;
		}

				optval = m_nSndSize;
		if(m_nSndSize && setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, (char *) &optval, sizeof(optval)))
		{
			int err = WSAGetLastError();
			LOGERROR("setsockopt() set SO_SNDBUF failed[%d].", err);
			Close();
			return false;
		}

				optval = m_nRcvSize;
		if(m_nRcvSize && setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char *) &optval, sizeof(optval)))
		{
			int err = WSAGetLastError();
			LOGERROR("setsockopt() set SO_RCVBUF failed[%d].", err);
			Close();
			return false;
		}

		unsigned long	i = 1;
		if(ioctlsocket(m_sock, FIONBIO, &i))
		{
			LOGERROR("Init ioctlsocket() failed.");
			Close();
			return false;
		}

		UINT	nAddr = inet_addr(m_szIP);
		if(nAddr == INADDR_NONE)
		{
			hostent	* hp;
			hp = gethostbyname(m_szIP);
			if(hp == 0)
			{
				LOGERROR("Bad account server host name.");
				Close();
				return false;
			}
			nAddr = *(UINT*)(hp->h_addr_list[0]);
		}

		sockaddr_in	addr_in;
		memset((void *)&addr_in, sizeof(addr_in), 0);
		addr_in.sin_family = AF_INET;
		addr_in.sin_port = htons(m_nPort);
		addr_in.sin_addr.s_addr = nAddr;
		int ret = connect(m_sock, (sockaddr *)&addr_in, sizeof(addr_in));
		if(ret == -1)
		{
			int err = WSAGetLastError();
			if(err != WSAEWOULDBLOCK)
			{
				Close();
				return false;
			}
		}

		m_nState = STATE_CONNECT;
	}
	if(m_sock == INVALID_SOCKET)
	{
		Close();
		return false;
	}

	ASSERT(m_nState == STATE_CONNECT);
	timeval timeout;
	timeout.tv_sec	= nBlockSecs;
	timeout.tv_usec	= 0;
	fd_set writeset, exceptset;
	FD_ZERO(&writeset);
	FD_ZERO(&exceptset);
	FD_SET(m_sock, &writeset);
	FD_SET(m_sock, &exceptset);

	int ret = select(FD_SETSIZE, NULL, &writeset, &exceptset, &timeout);
	if(ret == SOCKET_ERROR)
	{
		Close();
		return false;
	}
	if (ret > 0 && FD_ISSET(m_sock, &exceptset))
	{
		Close();
		return false;
	}
	else if(ret > 0 && FD_ISSET(m_sock, &writeset))
	{
		m_nState	= STATE_OPEN;
		return true;
	}

	return false;
}

const char* CClientSocket::GetPacket(int* pBufSize, bool bFromNet )
{
	CHECKF(pBufSize);

	*pBufSize = 0;

	if(m_nState != STATE_OPEN)
		return NULL;

	if(m_sock == INVALID_SOCKET)
	{
		Close();
		return NULL;
	}

	if(bFromNet && m_nLen < RECV_BUFFER_SIZE)
	{
		int ret = recv(m_sock, m_bufMsg + m_nLen, RECV_BUFFER_SIZE - m_nLen, 0);
		if(ret > 0)
		{
#ifdef	ENCRYPT_ENABLE
				m_cEncryptRecv.Encrypt((unsigned char *)m_bufMsg + m_nLen, ret);
#endif
			m_nLen += ret;
		}
		else if(ret == 0)
		{
			Close();
		}
		else if(ret < 0)
		{
			int err = WSAGetLastError();
			if(err != WSAEWOULDBLOCK)
				Close();
		}
	}

	*pBufSize	= m_nLen;
	return m_bufMsg;
}

bool CClientSocket::ClearPacket	(int nLen)
{
	ASSERT(nLen <= m_nLen);

	if(m_nState != STATE_OPEN)
		return false;

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

void CClientSocket::Close()
{
	if(m_sock != INVALID_SOCKET)
		closesocket(m_sock);
	m_sock		= INVALID_SOCKET;
	m_nState	= STATE_CLOSED;
	m_nLen		= 0;
	m_nSendLen	= 0;
}

bool CClientSocket::SendPacket(const char* buf, int nLen, bool bFlush )
{
	if(m_nState != STATE_OPEN)
		return false;

	if(m_sock == INVALID_SOCKET)
	{
		Close();
		return false;
	}
	if(nLen + m_nSendLen > SEND_BUFFER_SIZE)
		Flush();

	if(nLen + m_nSendLen > SEND_BUFFER_SIZE)
	{
		LOGERROR("SOCKET overflow!");
		Close();
		return false;
	}

	memcpy(m_bufSendMsg + m_nSendLen, buf, nLen);
	m_nSendLen	+= nLen;
#ifdef	ENCRYPT_ENABLE
		m_cEncryptSend.Encrypt((unsigned char*)m_bufSendMsg + m_nSendLen - nLen, nLen);
#endif

	if(bFlush)
		return Flush();

	return true;
}

bool CClientSocket::Flush()
{
	if(m_nState != STATE_OPEN)
		return false;

	if(!m_nSendLen)
		return true;

	int ret = send(m_sock, m_bufSendMsg, m_nSendLen, 0);
	if(ret > 0)
	{
		if(m_nSendLen - ret > 0)
			memcpy(m_bufSendMsg, m_bufSendMsg + ret, m_nSendLen - ret);
		m_nSendLen	-= ret;
		ASSERT(m_nSendLen >= 0);

		return !m_nSendLen;
	}
	else
	{
		Close();
		return false;
	}
}

bool CClientSocket::IsOpen()
{
	return (m_nState == STATE_OPEN && m_sock != INVALID_SOCKET);
}
