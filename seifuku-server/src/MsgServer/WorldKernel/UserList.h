#if !defined(AFX_USERLIST_H__419FFB53_E120_437C_9BF8_B8839D75C9E1__INCLUDED_)
#define AFX_USERLIST_H__419FFB53_E120_437C_9BF8_B8839D75C9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#pragma warning(disable:4786)
#include "AllMsg.h"
#include "define.h"
#include "windows.h"
#include "NetworkDef.h"
#include "TimeOut.h"
#include "ClientSocket.h"
#include "AutoPtr.h"
#include "GameObjSet.h"
#include <time.h>
#include <vector>
#include <map>
using namespace std;
using namespace world_kernel;

class CSupermanList;
struct CPlayer
{
	OBJID		idUser;
	NAMESTR		szName;
	PROCESS_ID	idProcess;

	NAMESTR		szAccount;
	SOCKET_ID	idSocket;
	int			m_nReserved;

	CPlayer() { idUser = ID_NONE; m_tPoint = 0; idSocket = SOCKET_NONE; m_nReserved = 0; }
	OBJID	GetID()					{ return idUser; }
	void	ReleaseByOwner()		{ delete this; }
	OBJID	GetProcessID()			{ return idProcess; }
	bool	SendMsg(world_kernel::CNetMsg* pMsg);
	bool	IsAgent()				{ return idSocket == SOCKET_NONE; }
	bool	IsGM()					{ return (strstr(szName, "PM") || strstr(szName, "GM")); }
	bool	IsPM()					{ return (strstr(szName, "[PM]") != NULL); }

public:
	void	OnTimer(time_t tCurr);
public:
	int		m_tPoint;
	CTimeOut	m_tTick;
	OBJID	m_idAccount;
	int 	m_nLevel;

	NAMESTR		szNotify;
};

enum { LOGIN_OK, LOGIN_NEW, LOGIN_BAN,
		LOGIN_GMIP };

struct ACCOUNT_LOCK
{
	DWORD	nCount;
	DWORD	tLast;
	DWORD	tFirst;
	DWORD	nLoginMinute;
	unsigned char	bufMac[6];
	DWORD	dwIP;
	DWORD	dwResData;
};

class IDatabase;
class CUserList
{
public:
	CUserList();
	virtual ~CUserList();
	static CUserList*	CreateNew()		{ return new CUserList; }
	bool	Create(world_kernel::ISocket* pPort, IDatabase* pDb);
	ULONG	Release()			{ delete this; return 0; }

public:
	void DelAllAgent();
	int		LoginUser(OBJID idAccount, SOCKET_ID idSocket, LPCTSTR szInfo);
	bool	LoginAgent(OBJID idAgent);
	bool	LogoutUser(SOCKET_ID idSocket);
	bool	KickOutAccount(OBJID idAccount, LPCTSTR szText);
	bool	KickOutSocket(SOCKET_ID idSocket);
	void	OnTimer(time_t tCurr);
	void	LogoutAllUser();
	bool	CreateUser(PROCESS_ID idProcess, OBJID idUser, LPCTSTR szName, OBJID idAccount, SOCKET_ID idSocket, int nLevel, LPCTSTR szNotify);
	bool	CreateAgent(PROCESS_ID idProcess, OBJID idAgent, LPCTSTR szName);
	CPlayer*	GetPlayer(LPCTSTR szName);
	CPlayer*	GetPlayer(OBJID idUser);
	CPlayer*	GetPlayerBySocket(SOCKET_ID idSocket)	{ if(idSocket>=0 && idSocket<(int)m_setUser.size()) return m_setUser[idSocket]; return NULL; }
	CPlayer*	GetPlayerByAccountID(OBJID idAccount);
	bool	CreateNewPlayer(LPCTSTR szAccount, LPCTSTR szName, LPCTSTR szPassword,
							int unLook, int unData, OBJID idAccount);
	void	ChangeProcessID(SOCKET_ID idSocket, PROCESS_ID idProcess);
	bool	IsLoginMapGroup(SOCKET_ID idSocket)			{ return (idSocket>=0 && idSocket<(int)m_setUser.size() && m_setUser[idSocket] && m_setUser[idSocket]->idProcess >= MSGPORT_MAPGROUP_FIRST); }
	void	LoginToMapGroup(CPlayer* pPlayer);
	int		GetUserAmount()								{ return m_nPlayerAmount; }
	void	SendTimeToSocket(SOCKET_ID idSocket);	void	SetFeeData(OBJID idUser, DWORD dwData);
	CSupermanList*	QuerySupermanList()					{ MYASSERT(m_pSupermanSet); return m_pSupermanSet; }

	void	BroadcastMsg(world_kernel::CNetMsg* pMsg, CPlayer* pExcludeSender=NULL);
	void	BroadcastMsg(LPCTSTR szMsg, CPlayer* pExcludeSender=NULL, LPCTSTR pszEmotion=NULL, DWORD dwColor=0xffffff, int nAttribute=_TXTATR_NORMAL);

public:
	bool	CheckAccountLock(OBJID idAccount, const char* pMac, DWORD dwIP, DWORD dwResData);
	void	LockAccount(OBJID idAccount);
	void	UnlockAccount(OBJID idAccount);
	void	LoadGmIP();
	void	SetClientVersion(OBJID idAccount, int nVersion);
	int		GetClientVersion(OBJID idAccount);
	void	DelClientVersion(OBJID idAccount);
	void	SetMinorFlag(OBJID idAccount, int nMinor);
	int		GetMinorFlag(OBJID idAccount);
	void	DelMinorFlag(OBJID idAccount);

protected:
	int m_nHour;
	CTimeOut m_tHour;
	typedef vector<CPlayer*>	USERLIST_SET;
	USERLIST_SET	m_setUser;
	typedef	CGameObjSet<CPlayer>	AGENT_SET;
	CAutoPtr<AGENT_SET>		m_setAgent;

	world_kernel::ISocket*		m_pSocket;

	CAutoPtr<CSupermanList>		m_pSupermanSet;
	int				m_nPlayerAmount;
	int				m_nMaxPlayerAmount;

	typedef map<OBJID, ACCOUNT_LOCK>	ACCOUNTLOCK_SET;
	ACCOUNTLOCK_SET		m_setAccountLock;
	map<OBJID, int>		m_setClientVersion;
	map<OBJID, int>		m_setMinorFlag;
	vector<DWORD>		m_setGmIP;

public:
	MYHEAP_DECLARATION(s_heap)
};

#endif
