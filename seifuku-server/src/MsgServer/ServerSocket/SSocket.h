#ifndef	SSOCKET_H
#define	SSOCKET_H

#include "Common.h"

#define	PURE_VIRTUAL_DECLARE		PURE_VIRTUAL_FUNCTION
#define	PURE_VIRTUAL_DECLARE_0		PURE_VIRTUAL_FUNCTION_0

#define LOGIN_KEY1				0xb29d3c84
#define LOGIN_KEY2				0xcc624ada

const int		RECV_BUFFER_SIZE		= 4096;

bool	ServerSocketInit();
bool	ServerSocketFinal();

typedef	char* CPacket;

class	IListenSocket
{
public:
	virtual bool Listen(int nPort, int nSndBuf = 0, int nRcvBuf = 0)	PURE_VIRTUAL_DECLARE_0

	virtual SOCKET		Accept()	PURE_VIRTUAL_DECLARE_0

	virtual void		Close()		PURE_VIRTUAL_DECLARE

	virtual SOCKET		Socket()	PURE_VIRTUAL_DECLARE_0
};

class	CListenSocket : IListenSocket
{
public:
	IListenSocket*	GetInterface() { return (IListenSocket*)this; }
public:
	CListenSocket();
	~CListenSocket() { Close(); }
protected:
	virtual bool		Listen(int nPort, int nSndBuf = 0, int nRcvBuf = 0);
	virtual SOCKET		Accept();
	virtual void		Close();
	virtual SOCKET		Socket() { return m_sock; }

protected:
	SOCKET		m_sock;
};

class	IServerSocket
{
public:
	virtual const char*	GetPacket	(int* nLen, BOOL bFromNet = true)				PURE_VIRTUAL_DECLARE_0
	virtual bool		ClearPacket	(int nLen)				PURE_VIRTUAL_DECLARE_0
	virtual bool		SendPacket	(const char* pack, int nLen)	PURE_VIRTUAL_DECLARE_0
	virtual bool		IsOpen		()						PURE_VIRTUAL_DECLARE_0
	virtual void		Close		(BOOL bNoLinger = false)	PURE_VIRTUAL_DECLARE		
	virtual DWORD		GetPeerIP	()						PURE_VIRTUAL_DECLARE_0

	virtual SOCKET		Socket()							PURE_VIRTUAL_DECLARE_0

	virtual int 		SetBufSize(int nSendBuf, int nRecvBuf=0)			PURE_VIRTUAL_DECLARE_0

	virtual void		ChangeCode(DWORD dwData)			PURE_VIRTUAL_DECLARE
	virtual bool		IsLocal		()						PURE_VIRTUAL_DECLARE_0
};
class	CServerSocket : IServerSocket
{
public:
	IServerSocket*	GetInterface() { return (IServerSocket*)this; }
public:
	static CServerSocket*	CreateNew(SOCKET sock);

	static void				Destroy(CServerSocket* pSocket);

protected:
	virtual const char*	GetPacket	(int* pLen, BOOL bFromNet );
	virtual bool		ClearPacket	(int nLen);
	virtual bool		SendPacket	(const char* pack, int nLen);
	virtual bool		IsOpen		();
	virtual	void		Close		(BOOL bNoLinger = false);
	virtual DWORD		GetPeerIP	();
	virtual SOCKET		Socket() { return m_sock; }
	virtual int			SetBufSize	(int nSendBuf, int nRecvBuf);
	virtual bool		IsLocal		() { return m_bLocal; }

public:
	CServerSocket(SOCKET sock) { m_sock = sock; m_nLen = 0; m_bLocal = false; }
	~CServerSocket() { Close(); }
protected:
	bool		Init();
protected:
	SOCKET		m_sock;
	char		m_bufMsg[RECV_BUFFER_SIZE];
	int			m_nLen;
	bool		m_bLocal;

public:
	void	ChangeCode(DWORD dwData) { m_cEncryptRecv.ChangeCode(dwData); }
protected:
	typedef	CEncryptServer<LOGIN_KEY1, LOGIN_KEY2>	EncryptServer;
	EncryptServer	m_cEncryptRecv, m_cEncryptSend;
};

#endif