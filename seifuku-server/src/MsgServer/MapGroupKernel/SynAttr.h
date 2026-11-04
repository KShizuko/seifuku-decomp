#if !defined(AFX_SYNATTR_H__7205A42E_F451_4A87_9BBD_37397D6578E2__INCLUDED_)
#define AFX_SYNATTR_H__7205A42E_F451_4A87_9BBD_37397D6578E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "define.h"
#include "GameData.h"
#include "GameObj.h"
#include "user.h"
#include "Myheap.h"
#include "AutoLink.h"
#include "Syndicate.h"

#define	DEFAULT_TENET				""
#define	DEFAULT_LEADERTITLE			STR_SYNDICATE_LEADER
#define	DEFAULT_MEMBERTITLE			STR_SYNDICATE_MEMBER
const int	DEFAULT_PROFFER			= 0;
const int	CHANGELEADER_DAYS		= 30;
const int	CHANGELEADER_MONEY		= 100000;
const int	SYNMONEY_LIMIT			= 2000000000;
const int	SYNPROFFER_LIMIT		= 1000000000;
const int	MAX_ASSISTANTSIZE		= 5;
const int	SYN_MEMBER_LIMIT		= 800;
struct SynAttrInfoStruct
{
	OBJID		id;
	OBJID		idSyn;
	UCHAR		ucRank;
	UCHAR		ucRankShow;
	USHORT		usReserved;
	int 		nProffer;
};

enum SYNATTRDATA
{
	SYNATTRDATA_ID=0,
	SYNATTRDATA_SYNID=1,
	SYNATTRDATA_RANK,
	SYNATTRDATA_PROFFER,
};
extern char	szSynAttrTable[];
typedef	CGameData<SYNATTRDATA, szSynAttrTable, szID>		CSynAttrData;

class CSynAttr
{
protected:
	CSynAttr();
	virtual ~CSynAttr();

public:
	static CSynAttr* CreateNew(PROCESS_ID idProcess, CUser* pUser);
	ULONG	ReleaseByOwner()						{ delete this; return 0; }

public:
	bool	Create();
	bool	SendInfoToClient();

public:
	bool	SaveInfo();
	bool	GetInfo(SynAttrInfoStruct* pInfo);
	bool	AppendSynAttr(SynAttrInfoStruct* pInfo);

public:
	OBJID	GetSynID()								{ if(m_pData) return m_pData->GetInt(SYNATTRDATA_SYNID); return ID_NONE; }
	int		GetInt(SYNATTRDATA idx)					{ CHECKF(m_pData); ASSERT(idx!=SYNATTRDATA_SYNID); return m_pData->GetInt(idx); }
	int		GetAssistantCount();
	DWORD	GetSynRankShow		();
	LPCTSTR	GetRankTitle();
	int		SumSynMember();
	OBJID	GetMasterSynID();
	int		GetProffer(void)	const				{ if(m_pData) return m_pData->GetInt(SYNATTRDATA_PROFFER); return 0; }

public:
	OBJID	CreateSyndicate(LPCTSTR szName, int nLevel, int nMoney, int nMoneyLeave);
	OBJID	CreateSubSyn(LPCTSTR szName, int nLevel, int nMoney, int nMoneyLeave);
	bool	JoinSyn(OBJID idSyn, int nRank=RANK_NEWBIE, int nProffer=0);
	bool	AddMemberList(OBJID idSyn, LPCTSTR szName, int nLevel);
	bool	LeaveSyn(bool bDeleteRecord=true, bool bSynchro=true);
	bool	DelMemberList(OBJID idOldSyn, LPCTSTR szName);
	bool	DonateMoney(int nMoney);
	bool	SetRank(int nRank, bool bLeader = false);
	bool	ChangeLeader(LPCTSTR szSyn, int nLevel);
	bool	SetIdRank(OBJID idSyn, int nRank, bool bSynchro=true);
	bool	AddProffer(int nData);
	void	UpLevel(int nLevel);

public:
	void	SynchroInfo(BOOL bAnnounce=false);
protected:
	bool	ChangeLeader();

public:
	bool	ChangeSyn(CSyndicate* pSyn, CSyndicate* pTarget);
	bool	KickoutMember(LPCTSTR szTarget);
	void	SetProffer(int nProffer)				{ if(m_pData) m_pData->SetInt(SYNATTRDATA_PROFFER, nProffer); }

protected:
	CSynAttrData*	m_pData;
protected:
	PROCESS_ID		m_idProcess;
	CUser*			m_pUser;
	CAutoLink<CSyndicate>	m_linkSyn;

protected:
	MYHEAP_DECLARATION(s_heap)
};

#endif
