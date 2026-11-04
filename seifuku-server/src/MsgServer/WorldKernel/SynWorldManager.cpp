#define	_WINSOCKAPI_
#include "MessagePort.h"
#include "windows.h"
#include "define.h"
#include "MessageBoard.h"
#include "SynWorldShell.h"
#include "SynWorldManager.h"
#include "WorldKernel.h"

MYHEAP_IMPLEMENTATION(CSynWorldManager,s_heap)
CSynWorldManager::CSynWorldManager()
{
	m_setSyn = CSynWorldSet::CreateNew(true);
	m_obj.m_pOwner	= this;
	m_pDefault		= NULL;
}

CSynWorldManager::~CSynWorldManager()
{
	SAFE_RELEASE (m_setSyn);

	SAFE_RELEASE (m_pDefault);
}
bool CSynWorldManager::Create(IDatabase* pDb)
{
	IRecordset* pRes = pDb->CreateNewRecordset("SELECT * FROM cq_syndicate", false);
	CHECKF(pRes);
	for(int i = 0; i < (int)pRes->RecordCount(); i++)
	{
		CSyndicateWorld* pSyn = CSyndicateWorld::CreateNew();
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

	m_pDefault	= pDb->CreateNewRecordset("SELECT * FROM cq_syndicate WHERE id=0 LIMIT 1", false);
	CHECKF(m_pDefault);

	return true;
}

OBJID CSynWorldManager::CreateSyndicate(const CreateSyndicateInfo* pInfo)
{
	CSyndicateWorld* pSyn = CSyndicateWorld::CreateNew();
	IF_OK(pSyn)
	{
		IF_OK(pSyn->Create(m_pDefault, pInfo, false))
		{
			m_setSyn->AddObj(pSyn);
			return pInfo->idSyn;
		}
		else
			pSyn->ReleaseByOwner();
	}

	return ID_NONE;
}

bool CSynWorldManager::DestroySyndicate(OBJID idSyn)
{
	CSyndicateWorld* pSyn = QuerySyndicate(idSyn);
	if(pSyn)
	{
		m_setSyn->DelObj(idSyn);
	}
	return true;
}

bool CSynWorldManager::CombineSyndicate(OBJID idSyn, OBJID idTarget)
{
	CSyndicateWorld* pSyn = QuerySyndicate(idSyn);
	if(pSyn)
	{
		CSyndicateWorld* pTarget = QuerySyndicate(idTarget);
		if(pTarget)
			pTarget->CombineMemberList(pSyn->QueryMemberList());

		m_setSyn->DelObj(idSyn);
	}
	return true;
}

OBJID CSynWorldManager::XSynMngModify::CreateSyndicate(const CreateSyndicateInfo* pInfo)
{
	OBJID	ret = ID_NONE;
	CSyndicateWorld* pSyn = CSyndicateWorld::CreateNew();
	IF_OK(pSyn)
	{
		IF_OK(pSyn->Create(This()->m_pDefault, pInfo, false))
		{
			This()->m_setSyn->AddObj(pSyn);

			LOGMSG("[%s] created syndicate [%s]. Motto: ...", pInfo->szLeader, pInfo->szName);
			LOGMSG(pInfo->szTenet);
			ret = pInfo->idSyn;
		}
		else
			pSyn->ReleaseByOwner();
	}

	if(ret)
	{
		GameWorld()->QuerySynMessage()->CreateSyndicate(pInfo);
	}
	return ret;
}

bool CSynWorldManager::XSynMngModify::DestroySyndicate(OBJID idSyn)
{
	bool	ret = false;
	CSyndicateWorld* pSyn = This()->QuerySyndicate(idSyn);
	if(pSyn)
	{
		This()->m_setSyn->DelObj(idSyn);

		ret = true;
	}

	if(ret)
	{
		GameWorld()->QuerySynMessage()->DestroySyndicate(idSyn);
	}
	return ret;
}

bool CSynWorldManager::XSynMngModify::CombineSyndicate(OBJID idSyn, OBJID idTarget)
{
	bool	ret = false;
	CSyndicateWorld* pSyn = This()->QuerySyndicate(idSyn);
	if(pSyn)
	{
		CSyndicateWorld* pTarget = This()->QuerySyndicate(idTarget);
		if(pTarget)
			pTarget->CombineMemberList(pSyn->QueryMemberList());

		This()->m_setSyn->DelObj(idSyn);

		ret = true;
	}

	if(ret)
	{
		GameWorld()->QuerySynMessage()->CombineSyndicate(idSyn, idTarget);
	}
	return ret;
}
