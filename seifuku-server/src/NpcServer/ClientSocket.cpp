#include "ClientSocket.h"

CSendPacket::CSendPacket(const char* pack, int nLen)
{
	m_nSent	= 0;
	m_buf.reset(new char[nLen], std::default_delete<char[]>());
	if(m_buf.get())
	{
		memcpy(m_buf.get(), pack, nLen);
		m_nLen	= nLen;
	}
	else
		m_nLen	= 0;
}

CClientSocket::CClientSocket()
{
	m_sock		= INVALID_SOCKET;
	m_nLen		= 0;
	m_nState	= STATE_CLOSED;
	m_nBufferedSendBytes		= 0;
	m_nMaxBufferedSendBytes		= 0;
	m_nMaxBufferedSendPackets	= 0;

	m_szIP[0]	= 0;
}

CClientSocket::~CClientSocket()
{
	LOGMSG("To %s:%d MaxBufferedSendBytes=%d, MaxBufferedSendPackets=%d at %s, %d", m_szIP, m_nPort, m_nMaxBufferedSendBytes, m_nMaxBufferedSendPackets, __FILE__, __LINE__);
}

bool CClientSocket::Open(const char* szName, int nPort, int nBlockSecs , int nSndBuf , int nRcvBuf )
{
	MYASSERT(m_nState == STATE_CLOSED);

	SafeCopy(m_szIP, szName, IPSTR_SIZE);
	m_nPort		= nPort;
	m_nSndSize	= nSndBuf;
	m_nRcvSize	= nRcvBuf;

	if(m_nState == STATE_OPEN)
		return true;

	if(m_nState == STATE_CLOSED)
	{
		MYASSERT(m_sock == INVALID_SOCKET);
#ifdef	ENCRYPT
		m_cSendEncrypt.Init();
		m_cRecvEncrypt.Init();
#endif
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
			closesocket(m_sock);
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

	MYASSERT(m_nState == STATE_CONNECT);
	timeval timeout;
	timeout.tv_sec	= nBlockSecs;
	timeout.tv_usec	= 0;
	fd_set writeset, exceptset;
	FD_ZERO(&writeset);
	FD_ZERO(&exceptset);
	FD_SET(m_sock, &writeset);
	FD_SET(m_sock, &exceptset);

	int ret = select(FD_SETSIZE, NULL, &writeset, &exceptset, &timeout);
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
	MYASSERT(m_szIP[0]);

	if(m_nState == STATE_OPEN)
		return true;

	if(m_nState == STATE_CLOSED)
	{
		MYASSERT(m_sock == INVALID_SOCKET);
#ifdef	ENCRYPT
		m_cSendEncrypt.Init();
		m_cRecvEncrypt.Init();
#endif
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
			closesocket(m_sock);
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

	MYASSERT(m_nState == STATE_CONNECT);
	timeval timeout;
	timeout.tv_sec	= nBlockSecs;
	timeout.tv_usec	= 0;
	fd_set writeset, exceptset;
	FD_ZERO(&writeset);
	FD_ZERO(&exceptset);
	FD_SET(m_sock, &writeset);
	FD_SET(m_sock, &exceptset);

	int ret = select(FD_SETSIZE, NULL, &writeset, &exceptset, &timeout);
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

	if(bFromNet && m_sock != INVALID_SOCKET && m_nLen < RECV_BUFFER_SIZE)
	{
		int ret = recv(m_sock, m_bufMsg + m_nLen, RECV_BUFFER_SIZE - m_nLen, 0);
		if(ret > 0)
		{
#ifdef	ENCRYPT
				m_cRecvEncrypt.Encrypt((unsigned char *)m_bufMsg + m_nLen, ret);
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
	MYASSERT(nLen <= m_nLen);

	if (m_nState != STATE_OPEN)
		return false;

	if(m_nLen - nLen > 0)
		memcpy(m_bufMsg, m_bufMsg + nLen, m_nLen - nLen);

	m_nLen	-= nLen;
	if (m_nLen < 0)
	{
		m_nLen = 0;
		return false;
	}
	return true;
}

void CClientSocket::Close()
{
	if(m_nState != STATE_CLOSED)
	{
		if(m_sock != INVALID_SOCKET)
			closesocket(m_sock);
		m_sock		= INVALID_SOCKET;
		m_nState	= STATE_CLOSED;
		m_setSendPacket.clear();
		m_nBufferedSendBytes	= 0;
	}
}

bool CClientSocket::SendPacket(const char* buf, int nLen, bool bFlush )
{
	if(m_nState != STATE_OPEN)
		return false;

	CSendPacket	pkt(buf, nLen);
	if(!pkt.m_buf.get())
		return false;

	m_setSendPacket.push_back(pkt);
	int nAmount = m_setSendPacket.size();
	if(m_nMaxBufferedSendPackets < nAmount)
		m_nMaxBufferedSendPackets	= m_setSendPacket.size();
	m_nBufferedSendBytes	+= nLen;
	if(m_nMaxBufferedSendBytes < m_nBufferedSendBytes)
		m_nMaxBufferedSendBytes	= m_nBufferedSendBytes;

	if(bFlush)
		Flush();

	return true;
}

bool CClientSocket::Flush()
{
	if(m_nState != STATE_OPEN)
		return false;

	while(!m_setSendPacket.empty())
	{
		CSendPacket& pkt = m_setSendPacket.front();
		if(pkt.m_nLen > 0)
		{
			send(m_sock, pkt.m_buf.get() + pkt.m_nSent, pkt.m_nLen - pkt.m_nSent, 0);
			int nSize = pkt.m_nLen - pkt.m_nSent;
			m_nBufferedSendBytes	-= nSize;
			if(nSize == 0)
				break;
		}
		m_setSendPacket.pop_front();
	}
	return true;
}

bool CClientSocket::IsOpen()
{

	return (m_nState == STATE_OPEN);
}
