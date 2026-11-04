#ifndef LISTENTHREAD_H
#define LISTENTHREAD_H

class	CListenSocket
{
public:
	CListenSocket(u_short nPort, int nSndSize = 0);
	~CListenSocket();
	bool	Open();
	SOCKET	Accept(u_long & nClientIP);
	void	Close();
	bool	IsOpen() { return m_bState; }
	bool	Rebuild();
public:
	SOCKET	Socket() { return m_sockListen; }
protected:
	const u_short	m_nPort;
	const int		m_nSndBuf;
	SOCKET	m_sockListen;
	bool	m_bState;
	time_t	m_tCloseTime;
};

#endif