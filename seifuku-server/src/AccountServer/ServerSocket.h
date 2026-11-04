#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#ifdef	ENCRYPT
	#include "EncryptServer.h"
#endif

template <unsigned long key1, unsigned long key2>
class	CServerSocket
{
public:
	CServerSocket();
	~CServerSocket();
public:
	bool	Open(SOCKET sockServer, u_long nClientIP);

	bool	Recv(char * pBuf, int &nLen);

	bool	Send(const char * pBuf, int nLen);

	bool	ShutDown();
	void	Close(bool bLinger0 = false);
	SOCKET	Socket() { return m_sockServer; }
	bool	IsOpen() { return m_bState != 0; }
	bool	IsShutDown() { return m_bState == c_stateShutDown; }
	char *	GetPeerIP();
protected:
	time_t m_tLastRecv;
	SOCKET	m_sockServer;
	enum	{ c_stateNone = 0, c_stateNormal, c_stateShutDown }	m_bState;
	char 	m_bufPeerIP[IPSTRSIZE];
#ifdef	ENCRYPT
	typedef		CEncryptServer<key1, key2>	POINTENCRYPT;
	POINTENCRYPT	m_cEncryptRecv;
	POINTENCRYPT	m_cEncryptSend;
#endif
};

#include "ServerSocket.hpp"

#endif