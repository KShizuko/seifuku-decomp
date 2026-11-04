#if !defined(AFX_SYNDICATE_H__78C1D98F_7DA8_482D_9819_22AD24402CB6__INCLUDED_)
#define AFX_SYNDICATE_H__78C1D98F_7DA8_482D_9819_22AD24402CB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "define.h"
#include "NetWorkDef.h"
#include "protocol.h"
#include "GameData.h"
#include "GameObj.h"
#include <vector>
using namespace std;

const int	SYN_MONEY_BASELINE		= 100000;

enum {
		SYNFUNC_DEMISE=1,
		SYNFUNC_SETTENET,
		SYNFUNC_SETLEADERTITLE,
		SYNFUNC_SETMEMBERTITLE,
		SYNFUNC_ADDDATA,
		SYNFUNC_SETDATA,
		SYNFUNC_PRESENT,
		SYNFUNC_ADD_MEMBER,
		SYNFUNC_DEL_MEMBER,
		SYNFUNC_UP_LEVEL,
};

const int	MAX_SYNENEMYSIZE		= 5;
const int	MAX_SYNALLYSIZE			= 5;
enum SYNDATA{
		SYNDATA_ID=0,

		SYNDATA_NAME=1,
		SYNDATA_TENET,
		SYNDATA_ANNOUNCE,
		SYNDATA_MEMBERTITLE,
		SYNDATA_LEADERID,
		SYNDATA_LEADERNAME,
		SYNDATA_MONEY,
		SYNDATA_FEALTY,
		SYNDATA_DEL_FLAG,
		SYNDATA_AMOUNT,
		SYNDATA_ENEMY0,
		SYNDATA_ENEMY1,
		SYNDATA_ENEMY2,
		SYNDATA_ENEMY3,
		SYNDATA_ENEMY4,
		SYNDATA_ALLY0,
		SYNDATA_ALLY1,
		SYNDATA_ALLY2,
		SYNDATA_ALLY3,
		SYNDATA_ALLY4,
};
extern char	szSynTable[];
typedef	CGameData<SYNDATA, szSynTable, szID>	CSynBase;

class ISynModify
{
public:
	virtual bool	Demise(OBJID idOldLeader, OBJID idNewLeader, LPCTSTR szNewLeader, OBJID idBackSyn, bool bSave=true)		PURE_VIRTUAL_FUNCTION_0
	virtual bool	SetTenet(LPCTSTR szTenet, bool bSave=true)													PURE_VIRTUAL_FUNCTION_0
	virtual bool	SetLeaderTitle(LPCTSTR szLeaderTitle, bool bSave=true)										PURE_VIRTUAL_FUNCTION_0
	virtual bool	SetMemberTitle(LPCTSTR szMemberTitle, bool bSave=true)										PURE_VIRTUAL_FUNCTION_0
	virtual bool	AddData(SYNDATA idx, int nData, bool bSave=true)											PURE_VIRTUAL_FUNCTION_0
	virtual bool	SetData(SYNDATA idx, int nData, bool bSave=true)											PURE_VIRTUAL_FUNCTION_0
	virtual bool	PresentMoney(OBJID idSyn, int nMoney, bool bSave=true)										PURE_VIRTUAL_FUNCTION_0
};

class CSyndicate : public CGameObj, ISynModify
{
protected:
	CSyndicate();
	virtual ~CSyndicate();
public:
	static CSyndicate* CreateNew() { ASSERT(!"CSyndicate::CreateNew()"); return NULL; }
	static CSyndicate* CreateNew(PROCESS_ID idProcess)	{ CSyndicate* ptr=new CSyndicate; if(ptr) ptr->m_idProcess=idProcess; return ptr; }
	virtual ULONG	ReleaseByOwner()				{ delete this; return 0; }
	bool	Create(IRecordset* pRes);
	bool	Create(const CreateSyndicateInfo* pInfo, bool bSave);
	ISynModify*		QueryModify()	{ return (ISynModify*)&m_obj; }
	bool			ChangeSyndicate(const SynFuncInfo0* pFuncInfo);
	bool			ReloadData();

public:
	OBJID	GetID()							{ return m_pData->GetKey(); }
	int		GetInt(SYNDATA idx);
	LPCTSTR	GetStr(SYNDATA idx)				{ return m_pData->GetStr(idx); }
	void	BroadcastSynMsg(CNetMsg* pMsg, CUser* pExcludeSender=NULL);
	void	BroadcastSubSynMsg(SOCKET_ID idSocket, OBJID idNpc, LPCTSTR szMsg, CUser* pExcludeSender=NULL, LPCTSTR pszEmotion=NULL, DWORD dwColor=0xffffff, int nAttribute=_TXTATR_SYNDICATE);
	LPCTSTR	GetFealtyName();
	bool	IsNearSyn(OBJID idSyn);
	OBJID	GetMasterSynID()				{ if(GetInt(SYNDATA_FEALTY) != ID_NONE) return GetInt(SYNDATA_FEALTY); return GetID(); }
	CSyndicate*		GetMasterSyn();
	bool	IsFriendly(OBJID idSyn, BOOL bNest=true);
	bool	IsHostile(OBJID idSyn, BOOL bNest=true);
	bool	IsWhiteSynMapOwner();
	bool	IsBlackSynMapOwner();
	int		GetMemberAmount();
protected:
	void	BroadcastLocalSynMsg(CNetMsg* pMsg, CUser* pExcludeSender=NULL);

public:
	void	SendInfoToClient(CUser* pUser);
	void	SetNewSubName(LPCTSTR szName);
	void	ClearAllRelation();
	void	ClearRelation(OBJID idSyn);

public:
	CAutoLink<CSyndicate>&	QueryLink	()					{ return m_link; }
protected:
	CAutoLink<CSyndicate>	m_link;

public:
	void	AddSynWarScore(OBJID idNpc, int nScore);
	int		GetSynWarScore(OBJID idNpc);
	void	ClearSynWarScore(OBJID idNpc);
protected:
	struct	SynWarInfo{
		OBJID	idNpc;
		int		nScore;
	};
	typedef	vector<SynWarInfo>	SYNWAR_SET;
	SYNWAR_SET		m_setSynWarScore;
protected:
	virtual bool	AddData(SYNDATA idx, int nData, bool bSave);
	virtual bool	Demise(OBJID idOldLeader, OBJID idNewLeader, LPCTSTR szNewLeader, OBJID idBackSyn, bool bSave);
	virtual bool	SetTenet(LPCTSTR szTenet, bool bSave);
	virtual bool	SetLeaderTitle(LPCTSTR szLeaderTitle, bool bSave);
	virtual bool	SetMemberTitle(LPCTSTR szMemberTitle, bool bSave);
	virtual bool	SetData(SYNDATA idx, int nData, bool bSave);
	virtual bool	PresentMoney(OBJID idSyn, int nMoney, bool bSave);

private:
	CSynBase*	m_pData;

protected:
	PROCESS_ID		m_idProcess;
	MYHEAP_DECLARATION(s_heap)
protected:
	class	XSynModify : public ISynModify
	{
	public:
		virtual bool	Demise(OBJID idOldLeader, OBJID idNewLeader, LPCTSTR szNewLeader, OBJID idBackSyn, bool bSave);
		virtual bool	SetTenet(LPCTSTR szTenet, bool bSave);
		virtual bool	SetLeaderTitle(LPCTSTR szLeaderTitle, bool bSave);
		virtual bool	SetMemberTitle(LPCTSTR szMemberTitle, bool bSave);
		virtual bool	AddData(SYNDATA idx, int nData, bool bSave);
		virtual bool	SetData(SYNDATA idx, int nData, bool bSave);
		virtual bool	PresentMoney(OBJID idSyn, int nMoney, bool bSave);
	protected:
		CSyndicate*		This()		{ return m_pOwner; }
		CSyndicate*		m_pOwner;
		friend class CSyndicate;
	} m_obj;
public:
	void	SynchroInfo(CUser* pExcludeSender=NULL);

	bool	IsMasterSyn()				{ return (GetInt(SYNDATA_FEALTY) == ID_NONE); }
	friend class CSyndicate::XSynModify;
};

#endif
