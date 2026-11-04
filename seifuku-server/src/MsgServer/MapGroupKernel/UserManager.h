#if !defined(AFX_USERMANAGER_H__70A5EB1D_66D5_4DF0_BE98_F7E3498FFC72__INCLUDED_)
#define AFX_USERMANAGER_H__70A5EB1D_66D5_4DF0_BE98_F7E3498FFC72__INCLUDED_

#pragma once

#include "define.h"
#include "User.h"
#include "GameBlock.h"
#include "team.h"
#include "NetworkDef.h"
#include "roleManager.h"
#include "index.h"
#include <vector>
using namespace std;

class IForMapGroup
{
public:
	virtual void	BroadcastMapGroupMsg(CNetMsg* pMsg, CUser* pExcludeSender=NULL)				PURE_VIRTUAL_FUNCTION
public:
	virtual void	JoinMapGroup(SOCKET_ID idSocket, UserInfoStruct* pInfo)				PURE_VIRTUAL_FUNCTION
};

typedef	IGameObjSet<CTeam>		ITeamSet;
typedef	CGameObjSet<CTeam>		CTeamSet;

class CAgent;
class CUserManager;
class ISocket;
class IDatabase;
class CUserManager;
typedef CUserManager IUserManager;
class CUserManager : IForMapGroup
{
public:
	class Iterator
	{
	private:
		Iterator(IRoleSet* pSet) : m_pSet(pSet),m_pIter(pSet->NewEnum()),m_pUser(NULL) {}
		friend class CUserManager;
	public:
		bool	Next()							{ while(m_pIter.Next()){ if(m_pIter->QueryObj(OBJ_USER, IPP_OF(m_pUser))) return true; } m_pUser=NULL; return false; }
		operator CUser*()						{ return m_pUser; }
		CUser* operator->()						{ MYASSERT(m_pUser); return m_pUser; }
	protected:
		IRoleSet*			m_pSet;
		CRoleSet::Iterator	m_pIter;
		CUser*				m_pUser;
	};
	Iterator	NewEnum();
public:
	CUserManager(PROCESS_ID idProcess) : m_idProcess(idProcess), m_dwMaxUser(0) {}
	virtual ~CUserManager() {}
	IUserManager*	GetInterface() { return (IUserManager*)this; }
	virtual IForMapGroup*	GetForMapGroup() { return (IForMapGroup*)this; }

public:
	virtual bool	Create();
	virtual void	Release();
	virtual void	OnTimer(time_t tCurr);
public:
	virtual CUser*	CreateUser(SOCKET_ID idSocket, OBJID idUser);
	virtual bool	LoginUser(SOCKET_ID idSocket, CUser* pUser);
	virtual bool	LogoutUser(SOCKET_ID idSocket, BOOL bOffline=true);
	virtual CAgent*	CreateAgent(OBJID idAgent);
	virtual bool	LoginAgent(CAgent* pAgent);
	virtual bool	LogoutAgent(CUser* pAgent, BOOL bOffline=true);
	virtual bool	KickOutSocket(SOCKET_ID idSocket, LPCTSTR szReason = NULL);
	virtual void	LogoutAllUser();
	virtual bool	IsLoginMapGroup(SOCKET_ID idSocket)			{ return (idSocket>=0 && idSocket<(SOCKET_ID)m_setUser.size() && m_setUser[idSocket]); }

public:
	virtual CUser*	GetUser(OBJID idUser);
	virtual CUser*	GetUser(LPCTSTR szName);
	virtual CUser*	GetUser(CNetMsg* pMsg);
	virtual CUser*	GetUserBySocketID(SOCKET_ID idSocket)	{ if(idSocket>=0 && idSocket<(SOCKET_ID)m_setUser.size()) return m_setUser[idSocket]; return NULL; }
	virtual DWORD	GetMapUserAmount(OBJID idMap);
	virtual DWORD	GetUserAmount();
	virtual DWORD	GetMaxUser()	{return m_dwMaxUser;}
	DWORD	m_dwMaxUser;
	virtual bool	FindNextSynMember(OBJID idSyn, int& nIter);
	virtual CUser*	GetGM()	{ if (m_setGm.size() <= 0) return NULL; else return this->GetUser(m_setGm.front()); }

public:
	virtual void	BroadcastMsg(CNetMsg* pMsg, CUser* pExcludeSender=NULL);
	virtual void	BroadcastMsg(LPCTSTR szMsg, CUser* pExcludeSender=NULL, LPCTSTR pszEmotion=NULL, DWORD dwColor=0xffffff, int nAttribute=_TXTATR_NORMAL);
	virtual void	BroadcastMapMsg(OBJID idMap, CNetMsg* pMsg, CUser* pExcludeSender=NULL);
public:
	virtual void	BroadcastMapGroupMsg(CNetMsg* pMsg, CUser* pExcludeSender=NULL);
	virtual void	CheckAllClient(int nSecs);
	virtual void	JoinMapGroup(SOCKET_ID idSocket, UserInfoStruct* pInfo);

public:
	CTeam*			GetTeamByLeaderId		(OBJID idLeader);
	CTeam*			GetTeamByIndex			(int nIndex);
	int				GetTeamAmount			();
	BOOL			IsValidTeam				(CTeam* pTeam);
	BOOL			AddTeam					(CTeam* pTeam);
	void			DelTeam					(CTeam* pTeam);
	OBJID			SpawnNewTeamID			();

public:
	bool	AddCheat(OBJID idUser, int* pnAmount, int* pnTimes);
	int		GetCheatCount(OBJID idUser)				{ return m_setCheatIDTimes.IsIn(idUser) ? m_setCheatIDTimes.At(idUser) : 0; }
	int		GetCheatAmount()						{ return m_setCheatIDTimes.Size(); }
protected:
	Index<OBJID,int>	m_setCheatIDTimes;

protected:
	typedef	vector<CUser*> USER_SET;
	USER_SET	m_setUser;

	ITeamSet*	m_pTeamSet;
	OBJID		m_idLastTeam;

	vector<OBJID>	m_setGm;

	PROCESS_ID	m_idProcess;

public:
	MYHEAP_DECLARATION(s_heap)
};

#endif
