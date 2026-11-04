#if !defined(AFX_TEAM_H__096C275F_D6F9_40F1_8B39_471D69EBA7CC__INCLUDED_)
#define AFX_TEAM_H__096C275F_D6F9_40F1_8B39_471D69EBA7CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "define.h"
#include <windows.h>
#include "Myheap.h"

#include <vector>
#include <algorithm>
using namespace std;

#define		_MAX_TEAMAMOUNT			5
const int	_RANGE_EXPSHARE			= 32;
const int	_RANGE_TEAM_STATUS		= 12;
const int	MAX_TEAM_EXP_TIMES		= 360;

enum {
	CHANGETEAM_ADD=1,
	CHANGETEAM_DEL,
	CHANGETEAM_DISMISS,
	CHANGETEAM_INFO,
	CHANGETEAM_FLY,
};

struct TeamInfoStruct{
	OBJID	id;
	OBJID	idLeader;
	OBJID	setMember[_MAX_TEAMAMOUNT];
	BOOL	bCloseMoney;
	BOOL	bCloseItem;
	bool	bUnk24;
};

class CNetMsg;
class IRole;
class CTeam
{
public:
	CTeam(PROCESS_ID idProcess);
	virtual ~CTeam();

public:
	static	CTeam*		CreateNew		(PROCESS_ID idProcess, OBJID m_idTeam, OBJID idLeader);
	static	CTeam*		CreateNew		(PROCESS_ID idProcess, TeamInfoStruct* pInfo);
	ULONG	ReleaseByOwner()	{ delete this; return 0; }
	OBJID	GetID()				{ return m_id; }
	void	GetInfo(TeamInfoStruct* pInfo);

public:
	BOOL	AddMember			(OBJID idMember);
	BOOL	DelMember			(OBJID idMember);
	void	Dismiss				(OBJID idLeader);

	void	Open				();
	void	Close				();
	BOOL	IsClosed			();
	void	OpenMoney			();
	void	CloseMoney			();
	BOOL	IsCloseMoney		()			{ return m_bCloseMoney; }
	void	SetCloseMoney		(bool flag)	{ m_bCloseMoney = flag; }
	void	OpenItem			();
	void	CloseItem			();
	BOOL	IsCloseItem			()			{ return m_bCloseItem; }
	void	SetCloseItem		(bool flag)	{ m_bCloseItem = flag; }
	bool	IsUnk28				();

	OBJID	GetMemberByIndex	(int nIndex);
	int		GetMemberAmount		();

	BOOL	IsTeamMember		(OBJID idMember);
	BOOL	IsTeamWithNewbie	(OBJID idKiller, IRole* pTarget);
	bool	IsAwardVirtueEnable	(CUser* pQueryUser);

	OBJID	GetLeader			()					{ return m_idLeader; }

	void	AwardMemberExp		(OBJID idKiller, IRole* pTarget, int nAddExp);
	void	AddKillStatistic	(OBJID idKiller, IRole* pTarget, int idEvent, int idType);
	void	SendAllKillStatistic();

	void	BroadcastTeamMsg	(CNetMsg* pMsg, CUser* pSender=NULL);
	void	BroadcastMemberLife	(CUser* pMember, bool bMaxLife=false);

public:
	void	AddMemberOnly		(OBJID idMember);
	void	DelMemberOnly		(OBJID idMember);

private:
	BOOL	Create				(OBJID idLeader);
	void	Init				();
	CTeam*	CloneNew();

protected:
	typedef	vector<OBJID>		USERIDSET;
	USERIDSET					m_setIdUser;

private:
	OBJID						m_id;
	OBJID						m_idLeader;
	PROCESS_ID					m_idProcess;
	BOOL						m_fClosed;
	BOOL						m_bCloseMoney;
	BOOL						m_bCloseItem;
	bool						m_bUnk28;

protected:
	MYHEAP_DECLARATION(s_heap)
};

#endif
