#if !defined(AFX_NPCWORLD_H__D741877F_A9D8_4DD2_8187_364C7AC912D0__INCLUDED_)
#define AFX_NPCWORLD_H__D741877F_A9D8_4DD2_8187_364C7AC912D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "define.h"
#include "I_Shell.h"
#include "NpcManager.h"
#include "MapManager.h"
#include "UserManager.h"
#include "SynManager.h"
#include "Timeout.h"
#include "ItemType.h"

const int	UPDATESTATEWIN_SECS			= 10;

#ifdef	_DEBUG
const int	LOGIN_DELAY_SECS			= 1;
#else
const int	LOGIN_DELAY_SECS			= 10;
#endif

class CNetMsg;
class IDatabase;
class CNpcWorld : public IKernel
{
public:
	CNpcWorld();
	virtual ~CNpcWorld();

public:
	virtual	bool	Create(IShell* pShell);
	virtual void	SetDisconnect()		{ SetLoginState(STATE_BREAK); }
	virtual	ULONG	Release()			{ delete this; return 0; }
	virtual	void	ProcessMsg(OBJID idPacket, const char* pBuf, int nMsgSize, OBJID idNpc);
	virtual	void	OnTimer(DWORD nCurr);
	virtual void	ShellCommond(LPCTSTR);

public:
	CMapManager*	QueryMapManager()	{ return m_pMapManager; }
	CUserManager*	QueryUserManager()	{ return m_pUserManager; }
	CNpcManager*	QueryNpcManager()	{ return m_pNpcManager; }
	CSynManager*	QuerySynManager()	{ return m_pSynManager; }

	CItemType*		GetItemType()		{ return m_pItemType; }
	IDatabase*		GetDatabase()		{ return m_pDatabase; }

public:
	bool			SendMsg(OBJID idNpc, CNetMsg* pMsg);
	void			PrintText(LPCTSTR szMsg)	{ m_pShell->PrintText(szMsg); }
	void			CloseShell()				{ m_pShell->CloseAll(); }

public:
	void			SetLoginState(int nState)	{ m_nLoginState = nState; }
	void			LoginOK();

public:
	enum { STATE_NONE, STATE_LOGINING, STATE_NORMAL, STATE_BREAK };
protected:
	int 			m_nLoginState;
	CTimeOut		m_timeoutLogin;
	CTimeOut		m_tUpdateStateWin;

protected:
	IShell*			m_pShell;

protected: 
	CMapManager*	m_pMapManager;
	CUserManager*	m_pUserManager;
	CNpcManager*	m_pNpcManager;
	CSynManager*	m_pSynManager;

	CJavaObj<CItemType>		m_pItemType;
	IDatabase*		m_pDatabase;

protected:
	static CNpcWorld*	m_pNpcWorld;
	friend CNpcWorld*	NpcWorld();
};

inline CNpcWorld*		NpcWorld()				{ return CNpcWorld::m_pNpcWorld; }
inline CUserManager*	UserManager()			{ return NpcWorld()->QueryUserManager(); }
inline CNpcManager*		NpcManager()			{ return NpcWorld()->QueryNpcManager(); }
inline CMapManager*		MapManager()			{ return NpcWorld()->QueryMapManager(); }
inline CSynManager*		SynManager()			{ return NpcWorld()->QuerySynManager(); }
inline IDatabase*		Database()				{ return NpcWorld()->GetDatabase(); }

inline bool	IsNpcID(OBJID idRole)		{ return (idRole >= NPCSERVERID_FIRST && idRole <= NPCSERVERID_LAST); }
inline bool	IsMonsterID(OBJID idRole)	{ return (idRole >= MONSTERID_FIRST && idRole <= MONSTERID_LAST); }
inline bool IsSceneID(OBJID idRole)		{ return (idRole >= SCENEID_FIRST && idRole <= SCENEID_LAST); }
inline bool IsTrapID(OBJID idRole)		{ return (idRole >= TRAPID_FIRST && idRole <= TRAPID_LAST); }
inline bool IsGmName(LPCTSTR szName)	{ return strstr(szName, "[GM]") != NULL || strstr(szName, "[PM]") != NULL; }

#endif
