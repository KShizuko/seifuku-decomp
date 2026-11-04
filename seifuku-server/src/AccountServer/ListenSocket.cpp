#include "allheads.h"
#include "ListenSocket.h"

CListenSocket::CListenSocket(u_short nPort, int nSndBuf )
		 : m_nPort(nPort), m_nSndBuf(nSndBuf)
{
	m_sockListen	= INVALID_SOCKET;
	m_bState		= false;
	m_tCloseTime	= 0;
}

CListenSocket::~CListenSocket()
{
	Close();
}

bool	CListenSocket::Open()
{
	if(m_bState)
		return true;

	WSADATA		wsaData;
	int	err;
	if((err = WSAStartup(0x0002, &wsaData)) != 0)
	{
		LOGERROR("Init WSAStartup() failed[%d].", err);
		return FALSE;
	}

	if(wsaData.wVersion != 0x0002)
	{
		LOGERROR("WSAStartup Version[0x%04X] not match 2.0", wsaData.wVersion);
		return FALSE;
	}
	m_sockListen=socket(AF_INET, SOCK_STREAM, 0);
	if(m_sockListen==INVALID_SOCKET)
	{
		int err = WSAGetLastError();
		LOGERROR("Init socket() failed[%d].", err);
		return FALSE;
	}
	else
	{
		InterlockedIncrement(&s_nSocketCount);
		m_bState = true;
	}

	int		optval = 1;
	if(setsockopt(m_sockListen, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof(optval)))
	{
		int err = WSAGetLastError();
		Close();
		LOGERROR("Init setsockopt() SO_REUSEADDR failed[%d].", err);
		return FALSE;
	}

	optval = 1;
	if(setsockopt(m_sockListen, SOL_SOCKET, SO_KEEPALIVE, (char *) &optval, sizeof(optval)))
	{
		int err = WSAGetLastError();
		Close();
		LOGERROR("Init setsockopt() SO_KEEPALIVE failed[%d].", err);
		return FALSE;
	}
	if(m_nSndBuf)
	{
		optval = m_nSndBuf;
		if(setsockopt(m_sockListen, SOL_SOCKET, SO_SNDBUF, (char *) &optval, sizeof(optval)))
		{
			int err = WSAGetLastError();
			Close();
			LOGERROR("Init setsockopt() SO_SNDBUF failed[%d].", err);
			return FALSE;
		}

		int ret = sizeof(optval);
		if(getsockopt(m_sockListen, SOL_SOCKET, SO_SNDBUF, (char *) &optval, &ret) == 0)
		{
			LOGMSG("Account SOCKET internal send buffer size is [%d]K.", optval/1024);
		}
	}
	SOCKADDR_IN		sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family		= AF_INET;
	sin.sin_addr.s_addr	= 0;
	sin.sin_port		= htons(m_nPort);
	if(bind(m_sockListen, (LPSOCKADDR) &sin, sizeof(sin)))
	{
		int err = WSAGetLastError();
		Close();
		LOGERROR("Init bind() failed[%d].", err);
		return FALSE;
	}

	unsigned long	i = 1;
	if(ioctlsocket(m_sockListen, FIONBIO, &i))
	{
		int err = WSAGetLastError();
		Close();
		LOGERROR("Init ioctlsocket() failed[%d].", err);
		return FALSE;
	}

	if(listen(m_sockListen, SOMAXCONN))
	{
		int err = WSAGetLastError();
		Close();
		LOGERROR("Init listen() failed[%d].", err);
		return FALSE;
	}

	return TRUE;
}

bool	CListenSocket::Rebuild()
{
	if(!m_bState)
	{
		if(clock() < m_tCloseTime + REBUILDLISTENDELAYSEC*CLOCKS_PER_SEC)
			return false;
		else
		{
			LOGMSG("Crash guard: rebuilding listen SOCKET [%d] . . .", m_nPort);
			if(Open())
			{
				LOGMSG(". . . listen SOCKET rebuilt.");
				PrintText("Listen socket %d failed, rebuilt; server still running...", m_nPort);
			}
			else
			{
				LOGERROR(". . . listen SOCKET rebuild failed. The server cannot accept new connections");
				PrintText("Listen socket %d error, rebuilding in %d seconds; server still running...", m_nPort, REBUILDLISTENDELAYSEC);
				return false;
			}
			m_tCloseTime = clock();
		}
	}

	return true;
}

SOCKET	CListenSocket::Accept(u_long & nClientIP)
{
	if(!m_bState)
	{
		LOGERROR("ACCEPT internal exception a1");
		return INVALID_SOCKET;
	}

	struct sockaddr_in	addr;
	memset(&addr, 0, sizeof(addr));
	int	   len = sizeof(addr);
	SOCKET newsock = accept(m_sockListen, (sockaddr *) &addr, (int *) &len);
#ifdef	PROFILE_X
			const int	nTimes2 = ACCEPTPROFILESEC;
			static	clock_t tNextTime2 = clock() + nTimes2 * CLOCKS_PER_SEC;
			static long	nCount2	= 0;
			if(clock() >= tNextTime2)
			{
				LOGPROFILE("Every [%d] seconds Accept() ran [%d] times", nTimes2, InterlockedExchange(&nCount2, 0));
				tNextTime2 = clock() + nTimes2 * CLOCKS_PER_SEC;
			}
			else
			{
				InterlockedIncrement(&nCount2);
			}
#endif
	if(newsock == INVALID_SOCKET)
	{
		int err = WSAGetLastError();
		if(err != WSAEWOULDBLOCK)
		{
			PrintText("Listen socket %d failed, rebuilding in %s seconds.", m_nPort, REBUILDLISTENDELAYSEC);
			LOGERROR("Listen SOCKET [%d] failed [%d], rebuilding SOCKET in [%s] seconds.", m_nPort, err, REBUILDLISTENDELAYSEC);
			Close();
		}
		else
			LOGERROR("ACCEPT internal exception a2");
		return INVALID_SOCKET;
	}
	else
	{
		nClientIP = addr.sin_addr.S_un.S_addr;
		InterlockedIncrement(&s_nSocketCount);
	}

	fd_set	readmask;
	FD_ZERO(&readmask);
	FD_SET(newsock, &readmask);
	struct timeval	timeout = {0, 0};
	fd_set	exceptmask;
	FD_ZERO(&exceptmask);
	FD_SET(newsock, &exceptmask);
	int ret = select(FD_SETSIZE, &readmask, (fd_set *) 0, (fd_set *) &exceptmask, &timeout);
	if(ret < 0)
	{
		LOGERROR("message: ACCEPT: new SOCKET is invalid. can't read");
		closesocket(newsock);
		InterlockedDecrement(&s_nSocketCount);
		return INVALID_SOCKET;
	}
	else if(ret > 0)
	{
		if(FD_ISSET(newsock, &exceptmask))
		{
			LOGMSG("ACCEPT: new SOCKET is invalid. except");
			closesocket(newsock);
			InterlockedDecrement(&s_nSocketCount);
			return INVALID_SOCKET;
		}
		else if(FD_ISSET(newsock, &readmask))
		{
			char nTemp;
			if(recv(newsock, &nTemp, 1, MSG_PEEK) == 0)
			{
#ifdef	ALPHA_X
				LOGMSG("ACCEPT: new SOCKET is invalid. recv==0");
#endif
				closesocket(newsock);
				InterlockedDecrement(&s_nSocketCount);
				return INVALID_SOCKET;
			}
		}
	}
#ifdef	PROFILE_X
			const int	nTimes = ACCEPTPROFILESEC;
			static	clock_t tNextTime = clock() + nTimes * CLOCKS_PER_SEC;
			static long	nCount	= 0;
			if(clock() >= tNextTime)
			{
				LOGPROFILE("Port[%d]: every [%d] seconds Accept() succeeded [%d] times",
							m_nPort, nTimes, InterlockedExchange(&nCount, 0));
				tNextTime = clock() + nTimes * CLOCKS_PER_SEC;
			}
			else
			{
				InterlockedIncrement(&nCount);
			}
#endif

	return newsock;
}

void	CListenSocket::Close()
{
	if(m_bState)
	{
		{
			struct linger ling;
			ling.l_onoff = 1;
			ling.l_linger = 0;
			setsockopt(m_sockListen, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling));
		}
		m_bState = false;
		closesocket(m_sockListen);
		InterlockedDecrement(&s_nSocketCount);
		m_sockListen = INVALID_SOCKET;
		m_tCloseTime = clock();
	}
}
