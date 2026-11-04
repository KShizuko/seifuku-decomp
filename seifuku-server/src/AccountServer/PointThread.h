#if !defined(AFX_POINTTHREAD_H__6188DE47_48CA_459B_9207_A425EB6C6A0F__INCLUDED_)
#define AFX_POINTTHREAD_H__6188DE47_48CA_459B_9207_A425EB6C6A0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Define.h"
#include "T_Index.h"
#include "ThreadBase.h"

struct	CServerAccount
{
	char	m_szServerName[SERVERNAMESIZE];
	char	m_szLoginName[SERVERNAMESIZE];
	char	m_szPassword[SERVERNAMESIZE];
	int		m_bFree;
	int		m_b91U;
	CServerAccount() { 
		m_szServerName[0] = m_szLoginName[0] = m_szPassword[0] = m_bFree = m_b91U = 0;
	}
};
class CPointThread : public CThreadBase  
{
public:
	CPointThread(u_short nPort, int nSndSize = 0);
	virtual ~CPointThread();

public:
	enum { ERR_NONE=0, ERR_NO_SERVER, ERR_BUSY, ERR_FULL };
	int		NewLogin(OBJID idAccount, DWORD nAuthenID, const char * szClientIP, const char * pGameServer);
	bool	GetServerIP(char * bufIP, const char * pServerName);
	int		GetServerCount();
	int		GetServerState(LPCTSTR szServerName);
	bool	CheckHeartbeatAll();
	void	LogCount();
	void	LogSerialCount();
	void	LogServer();
	bool	Kickout(OBJID idAccount);

protected:
	virtual void	OnInit();
	virtual bool	OnProcess();
	virtual void	OnDestroy();

public:
	enum { STATE_OFFLINE=0, STATE_NORMAL, STATE_BUSY, STATE_FULL, STATE_NORMAL91U, };
protected:
	int 	m_aState[MAXGAMESERVERS];
	CServerSocket<POINT_KEY1, POINT_KEY2>	m_aServerSocket[MAXGAMESERVERS];
	char	m_aServerName[MAXGAMESERVERS][SERVERNAMESIZE];
	CListenSocket	m_cListenSocket;
	time_t	m_aHeartbeat[MAXGAMESERVERS];
	time_t	m_aHeartbeatLast[MAXGAMESERVERS];
	time_t	m_aServerDelay[MAXGAMESERVERS];

private:
	bool	Kickout_0(OBJID idAccount, LPCTSTR szServerName);
	bool	ProcessMsg(int nServerIndex,  char * pBuf, int nLen);
	OBJID	GetServerIndex_0(const char * pServerName);
	bool	CheckHeartbeat(int nIndex);
	void	Clear(int nIndex, bool flag = true);
	void	SetServerBusy(int nServerIndex);
	void	SetServerFull(int nServerIndex);
};

#endif
