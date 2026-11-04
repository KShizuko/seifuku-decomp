#if !defined(AFX_SYNSHELL_H__8147FFBF_85B9_43E4_9D3A_B847BF31F088__INCLUDED_2)
#define AFX_SYNSHELL_H__8147FFBF_85B9_43E4_9D3A_B847BF31F088__INCLUDED_2

#if _MSC_VER > 1000
#pragma once
#endif

#include "define.h"
#include "GameData.h"
#include "protocol.h"
#include "AutoPtr.h"
#include "MessageBoard.h"
#include <vector>
#include <string>
using namespace std;
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
extern char	szID[];
extern char	szSynTable[];
typedef	CGameData<SYNDATA, szSynTable, szID>	CSynWorldBase;
class ISynModify
{
public:
	virtual bool	Demise(OBJID idOldLeader, OBJID idNewLeader, LPCTSTR szNewLeader, OBJID idBackSyn, bool bSave)		PURE_VIRTUAL_FUNCTION_0
	virtual bool	SetTenet(LPCTSTR szTenet, bool bSave)													PURE_VIRTUAL_FUNCTION_0
	virtual bool	SetLeaderTitle(LPCTSTR szLeaderTitle, bool bSave)										PURE_VIRTUAL_FUNCTION_0
	virtual bool	SetMemberTitle(LPCTSTR szMemberTitle, bool bSave)										PURE_VIRTUAL_FUNCTION_0
	virtual bool	AddData(SYNDATA idx, int nData, bool bSave)												PURE_VIRTUAL_FUNCTION_0
	virtual bool	SetData(SYNDATA idx, int nData, bool bSave)												PURE_VIRTUAL_FUNCTION_0
	virtual bool	PresentMoney(OBJID idSyn, int nMoney, bool bSave)										PURE_VIRTUAL_FUNCTION_0
};
struct CPlayer;
class CSyndicateWorld : ISynModify
{
protected:
	CSyndicateWorld();
	virtual ~CSyndicateWorld();
public:
	static CSyndicateWorld* CreateNew()	{ return new CSyndicateWorld; }
	virtual ULONG	ReleaseByOwner()				{ delete this; return 0; }
	bool	Create(IRecordset* pRes);
	bool	Create(IRecordset* pRes, const CreateSyndicateInfo* pInfo, bool bSave);
	ISynModify*		QueryModify()					{ return (ISynModify*)this; }
	CMessageBoard*	QueryMessageBoard()				{ ASSERT(m_ptrMsgBd); return m_ptrMsgBd; }
	bool			ChangeSyndicate(const SynFuncInfo0* pFuncInfo);

public:
	OBJID	GetID()									{ return m_pData->GetKey(); }
	int		GetInt(SYNDATA idx)						{ return m_pData->GetInt(idx); }
	LPCTSTR	GetStr(SYNDATA idx)						{ return m_pData->GetStr(idx); }

protected:
	struct	MemberInfo{
		MemberInfo(LPCTSTR str, int nUserRank) 
		{
			szName	= str;
			nRank	= nUserRank;
		}
		string	szName;	
		int		nRank;
	};
	typedef	vector<MemberInfo>	MEMBER_SET;
public:
	bool	AddMember(LPCTSTR szName, int nUserRank);
	bool	DelMember(LPCTSTR szName, int* pLevel = NULL);
	bool	SetMemberLevel(LPCTSTR szName, int nLevel);
	int		GetMemberAmount()				{ return m_setMember.size(); }
	void	SendMemberList(CPlayer* pUser, int idx);
	void	CombineMemberList(const MEMBER_SET& setMember);
	MEMBER_SET&	QueryMemberList()						{ return m_setMember; }
protected:
	int		AppendOnlineMember(class CMsgName& msg, int idxFirst);
	void	AppendOfflineMember(class CMsgName& msg, int idxFirst, int nCurrCount=0);
protected:
	MEMBER_SET	m_setMember;

protected:
	virtual bool	Demise(OBJID idOldLeader, OBJID idNewLeader, LPCTSTR szNewLeader, OBJID idBackSyn, bool bSave);
	virtual bool	SetTenet(LPCTSTR szTenet, bool bSave);
	virtual bool	SetLeaderTitle(LPCTSTR szLeaderTitle, bool bSave);
	virtual bool	SetMemberTitle(LPCTSTR szMemberTitle, bool bSave);
	virtual bool	AddData(SYNDATA idx, int nData, bool bSave);
	virtual bool	SetData(SYNDATA idx, int nData, bool bSave);
	virtual bool	PresentMoney(OBJID idSyn, int nMoney, bool bSave);

private:
	CSynWorldBase*	m_pData;

protected:
	typedef	CAutoPtr<CMessageBoard>	MSGBD_PTR;
	MSGBD_PTR		m_ptrMsgBd;

protected:
	MYHEAP_DECLARATION(s_heap)
};

#endif
