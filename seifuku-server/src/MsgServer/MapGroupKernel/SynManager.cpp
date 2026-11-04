#include "define.h"
#include "AllMsg.h"
#include "Syndicate.h"
#include "SynManager.h"
#include "MapGroup.h"

const int	MINI_SYN_SIZE			= 0;

MYHEAP_IMPLEMENTATION(CSynManager,s_heap)
CSynManager::CSynManager(PROCESS_ID idProcess) : m_idProcess(idProcess)
{
	m_setSyn = CSynSet::CreateNew(true);
	m_obj.m_pOwner=this;
	m_idWhiteSyn	= ID_NONE;
	m_idBlackSyn	= ID_NONE;
}

CSynManager::~CSynManager()
{
	if(m_setSyn)
		m_setSyn->Release();
}

bool CSynManager::Create()
{
	IRecordset* pRes = Database()->CreateNewRecordset("SELECT * FROM cq_syndicate ORDER BY amount DESC", false);
	CHECKF(pRes);
	for(int i = 0; i < (int)pRes->RecordCount(); i++)
	{
		CSynPtr pSyn = CSyndicate::CreateNew(m_idProcess);
		IF_OK(pSyn)
		{
			if(pSyn->Create(pRes))
				m_setSyn->AddObj(pSyn);
			else
				pSyn->ReleaseByOwner();
		}
		pRes->MoveNext();
	}
	pRes->Release();
	return true;
}

OBJID CSynManager::CreateSyndicate(const CreateSyndicateInfo* pInfo)
{
	CSynPtr pSyn = CSyndicate::CreateNew(m_idProcess);
	IF_OK(pSyn)
	{
		IF_OK(pSyn->Create(pInfo, false))
		{
			m_setSyn->AddObj(pSyn);

			SQLBUF	szSQL;
			sprintf(szSQL, STR_SYNDICATE_CREATE_ss, pInfo->szLeader, pInfo->szName);
			RoleManager()->BroadcastMapGroupMsg(szSQL);
			if(strlen(pInfo->szTenet))
				RoleManager()->BroadcastMapGroupMsg(pInfo->szTenet);

			CUser* pNewUser = UserManager()->GetUser(pInfo->idLeader);
			if(pNewUser)
			{
				pNewUser->QuerySynAttr()->SetIdRank(pSyn->GetID(), RANK_LEADER, true);
			}

			return pInfo->idSyn;
		}
		else
			pSyn->ReleaseByOwner();
	}

	return ID_NONE;
}

bool CSynManager::DestroySyndicate(OBJID idSyn)
{
	CSynPtr pSyn = QuerySyndicate(idSyn);
	if(pSyn)
	{
		pSyn->ClearAllRelation();

		SQLBUF	szSQL = "";
		sprintf(szSQL, STR_SYNDICATE_DESTROY_s, pSyn->GetStr(SYNDATA_NAME));
		RoleManager()->BroadcastMapGroupMsg(szSQL);

		CMsgSyndicate	msg;
		IF_OK(msg.Create(DESTROY_SYN, idSyn))
			RoleManager()->BroadcastMapGroupMsg(&msg);

		m_setSyn->DelObj(idSyn);

		CUserManager::Iterator pUser = UserManager()->NewEnum();
		while(pUser.Next())
		{
			if(pUser && pUser->QuerySynAttr()->GetSynID() == idSyn)
				pUser->QuerySynAttr()->LeaveSyn(false, false);
		}
	}
	return true;
}

bool CSynManager::CombineSyndicate(OBJID idSyn, OBJID idTarget)
{
	CSynPtr pSyn	= QuerySyndicate(idSyn);
	CSynPtr pTarget	= QuerySyndicate(idTarget);
	if(pSyn && pTarget)
	{
		if(pSyn->GetInt(SYNDATA_FEALTY) == ID_NONE)
		{
			SQLBUF	szSQL;
			sprintf(szSQL, STR_SYNDICATE_COMBINESUB_ss, pSyn->GetStr(SYNDATA_NAME), pTarget->GetStr(SYNDATA_NAME));
			RoleManager()->BroadcastMapGroupMsg(szSQL);
		}

		CMsgAction	msg;
		IF_OK(msg.Create(idSyn, 0, 0, 0, actionCombineSubSyn, idTarget))
			RoleManager()->BroadcastMapGroupMsg(&msg);

		CUserManager::Iterator pUser = UserManager()->NewEnum();
		while(pUser.Next())
		{
			if(pUser && pUser->QuerySynAttr()->GetSynID() == idSyn)
				pUser->QuerySynAttr()->SetIdRank(idTarget, RANK_NEWBIE, SYNCHRO_FALSE);
		}

		m_setSyn->DelObj(idSyn);
	}
	return true;
}

CSyndicate* CSynManager::QuerySynByName(LPCTSTR szSyn)
{
	CHECKF(szSyn);

	for(int i = 0; i < m_setSyn->GetAmount(); i++)
	{
		CSyndicate* pSyn = m_setSyn->GetObjByIndex(i);
		if(pSyn && strcmp(pSyn->GetStr(SYNDATA_NAME), szSyn) == 0)
			return pSyn;
	}

	return NULL;
}

bool CSynManager::FindNextSyn(int& nIter)
{
	OBJID	idLastSyn	= nIter;
	CSynPtr pTarget		= NULL;
	bool	bFindNext	= true;
	for(int i = 0; i < m_setSyn->GetAmount(); i++)
	{
		CSyndicate* pSyn = m_setSyn->GetObjByIndex(i);
		if(pSyn && pSyn->GetInt(SYNDATA_AMOUNT) > MINI_SYN_SIZE)
		{
			if(bFindNext)
			{
				pTarget = pSyn;
				bFindNext = false;
			}

			if(pSyn->GetID() == idLastSyn)
			{
				pTarget = NULL;
				bFindNext = true;
				continue;
			}
		}
	}

	if(pTarget)
	{
		nIter = pTarget->GetID();
		return true;
	}
	return false;
}

OBJID CSynManager::XSynMngModify::CreateSyndicate(const CreateSyndicateInfo* pInfo)
{
	SQLBUF	szSQL;
	sprintf(szSQL, "INSERT cq_syndicate SET name='%s',leader_id=%u,leader_name='%s',money=%d,fealty_syn=%d",
			pInfo->szName, pInfo->idLeader, pInfo->szLeader, pInfo->nMoney, pInfo->idFealty);
	LOGMSG(szSQL);
	if(!MapGroup(This()->m_idProcess)->GetDatabase()->ExecuteSQL(szSQL))
		return ID_NONE;
	CreateSyndicateInfo	info;
	memcpy(&info, pInfo, sizeof(info));
	info.idSyn = MapGroup(This()->m_idProcess)->GetDatabase()->GetInsertId();
	MapGroup(This()->m_idProcess)->QuerySynMessage()->CreateSyndicate(&info);
	return info.idSyn;
}

bool CSynManager::XSynMngModify::DestroySyndicate(OBJID idSyn)
{
	CHECKF (idSyn!=ID_NONE);

	SQLBUF	szSQL;
	sprintf(szSQL, "UPDATE cq_syndicate SET del_flag=1 WHERE id=%u LIMIT 1", idSyn);
	LOGMSG(szSQL);
	if(!MapGroup(This()->m_idProcess)->GetDatabase()->ExecuteSQL(szSQL))
		return false;

	OBJID idFealty = This()->QuerySyndicate(idSyn)->GetInt(SYNDATA_FEALTY);
	if(!MapGroup(This()->m_idProcess)->QuerySynMessage()->DestroySyndicate(idSyn))
		return false;

	if(idFealty == ID_NONE)
	{
		for(int i = 0; i < This()->QuerySynSet()->GetAmount(); i++)
		{
			CSyndicate* pSyn = This()->QuerySynSet()->GetObjByIndex(i);
			if(pSyn && pSyn->GetInt(SYNDATA_FEALTY) == idSyn)
				pSyn->QueryModify()->SetData(SYNDATA_FEALTY, ID_NONE, true);
		}
	}
	return true;
}

bool CSynManager::XSynMngModify::CombineSyndicate(OBJID idSyn, OBJID idTarget)
{
	CHECKF(idSyn);

	SQLBUF	szSQL;
	sprintf(szSQL, "UPDATE cq_syndicate SET del_flag=1 WHERE id=%u LIMIT 1", idSyn);
	LOGMSG(szSQL);
	if(!MapGroup(This()->m_idProcess)->GetDatabase()->ExecuteSQL(szSQL))
		return false;

	CSyndicate* pSyn	= This()->QuerySyndicate(idSyn);
	CSyndicate* pTarget = This()->QuerySyndicate(idTarget);
	CHECKF(pSyn && pTarget);
	int nCount = pSyn->GetInt(SYNDATA_AMOUNT);
	int nMoney = pSyn->GetInt(SYNDATA_MONEY);
	if(MapGroup(This()->m_idProcess)->QuerySynMessage()->CombineSyndicate(idSyn, idTarget))
	{
		if(nCount)
			pTarget->QueryModify()->AddData(SYNDATA_AMOUNT, nCount);
		if(nMoney)
			pTarget->QueryModify()->AddData(SYNDATA_MONEY, nMoney);

		return true;
	}
	else
		return false;
}

void CSynManager::XSynMngModify::SetMapSynID(OBJID idMap, OBJID idSyn)
{
	CHECK(idMap);

	This()->SetMapSynID(idMap, idSyn);
	MapGroup(This()->m_idProcess)->QuerySynMessage()->SetMapSynID(idMap, idSyn);
}
