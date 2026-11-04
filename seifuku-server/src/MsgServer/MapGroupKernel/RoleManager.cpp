#include "windows.h"
#include "define.h"
#include "RoleManager.h"
#include "MapGroup.h"
#include "AllMsg.h"

MYHEAP_IMPLEMENTATION(CRoleManager,s_heap)
CRoleManager::CRoleManager(PROCESS_ID idProcess) : m_idProcess(idProcess)
{
	m_setRole	= CRoleSet::CreateNew(true);
}

CRoleManager::~CRoleManager()
{
	if(m_setRole)
		m_setRole->Release();
}

bool CRoleManager::Create()
{
	CHECKF(m_setRole);

	return true;
}

bool CRoleManager::LoadAllPet()
{
	CHECKF(m_setRole);
	IRecordset* pRes = Database()->CreateNewRecordset("SELECT * FROM cq_pet");
	if(pRes)
	{
		for(int i = 0; i < (int)pRes->RecordCount(); i++, pRes->MoveNext())
		{
			CMonster* pMonster = CMonster::CreateNew();
			IF_OK(pMonster)
			{
				if(pMonster->Create(m_idProcess, pRes))
				{
					m_setRole->AddObj(pMonster->QueryRole());
					pMonster->EnterMapGroup();
				}
				else
					pMonster->ReleaseByOwner();
			}
		}

		pRes->Release();
	}

	return true;
}

IRole* CRoleManager::QueryRole(CNetMsg* pMsg, OBJID idRole)
{
	if(!pMsg->IsNpcMsg())
	{
		CUserPtr pUser = UserManager()->GetUserBySocketID(pMsg->GetSocketID());
		if(pUser)
		{
			if (idRole && idRole == pUser->GetCallPetID())
				return pUser->QueryCallPet()->QueryRole();
			else
				return pUser->QueryRole();
		}
	}
	else
	{
		return  QuerySet()->GetObj(pMsg->GetNpcID());
	}
	return NULL;
}
void CRoleManager::OnTimer(time_t tCurr)
{
	vector<OBJID>	setDel;
	for(IRoleSet::Iter i = m_setRole->Begin(); i != m_setRole->End(); )
	{
		IRole* pRole = m_setRole->GetObjByIter(i);
		i++;
		if(pRole)
		{
			DEBUG_TRY
			pRole->OnTimer(tCurr);
			if(pRole->IsDelThis())
			{
				OBJID idRole = pRole->GetID();
				if(idRole)
					setDel.push_back(idRole);
			}
			DEBUG_CATCH("@CRoleManager::OnTimer@")
		}
	}
	for(vector<OBJID>::iterator it = setDel.begin(); it != setDel.end(); it++)
		m_setRole->DelObj(*it);
}

void CRoleManager::SaveAll()
{
	for(IRoleSet::Iter i = m_setRole->Begin(); i != m_setRole->End(); i++)
	{
		IRole* pRole = m_setRole->GetObjByIter(i);
		if(pRole)
			pRole->SaveInfo();
	}
}

void CRoleManager::BroadcastMsg(CNetMsg* pMsg, CUser* pExcludeSender)
{
	CHECK(pMsg);

	BroadcastMapGroupMsg(pMsg, pExcludeSender);

	MapGroup(PID)->QueryIntraMsg()->SendBroadcastMsg(pMsg);
}

void CRoleManager::BroadcastMsg(LPCTSTR szMsg, CUser* pExcludeSender)
{
	CHECK(szMsg);

	CMsgTalk	msg;
	if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, szMsg))
		BroadcastMsg(&msg, pExcludeSender);
}

void CRoleManager::BroadcastMapGroupMsg(CNetMsg* pMsg, CUser* pExcludeSender)
{
	MYASSERT(pMsg);

	UserManager()->GetForMapGroup()->BroadcastMapGroupMsg(pMsg, pExcludeSender);
}

void CRoleManager::BroadcastMapGroupMsg(LPCTSTR szMsg, CUser* pExcludeSender)
{
	CHECK(szMsg);

	CMsgTalk	msg;
	if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, szMsg))
		UserManager()->GetForMapGroup()->BroadcastMapGroupMsg(&msg, pExcludeSender);
}

int CRoleManager::CountAllNpc(OBJID idMap)
{
	int		nCount = 0;
	CNpc*	pNpc;
	for(IRoleSet::Iter i = m_setRole->Begin(); i != m_setRole->End(); i++)
	{
		IRole* pRole = m_setRole->GetObjByIter(i);
		if(pRole && pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->GetMapID() == idMap)
			nCount++;
	}
	return nCount;
}

int CRoleManager::CountFurnitureNpc(OBJID idMap)
{
	int		nCount = 0;
	CNpc*	pNpc;
	for(IRoleSet::Iter i = m_setRole->Begin(); i != m_setRole->End(); i++)
	{
		IRole* pRole = m_setRole->GetObjByIter(i);
		if(pRole && pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->GetMapID() == idMap && (pNpc->GetType()==_ROLE_FURNITURE_NPC || pNpc->GetType()==_ROLE_3DFURNITURE_NPC))
			nCount++;
	}
	return nCount;
}

int CRoleManager::CountNpcByName(OBJID idMap, LPCTSTR szName)
{
	int		nCount = 0;
	CNpc*	pNpc;
	for(IRoleSet::Iter i = m_setRole->Begin(); i != m_setRole->End(); i++)
	{
		IRole* pRole = m_setRole->GetObjByIter(i);
		if(pRole && pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->GetMapID() == idMap && strcmp(pNpc->GetName(), szName)==0)
			nCount++;
	}
	return nCount;
}

int CRoleManager::CountNpcByType(OBJID idMap, int nType)
{
	int		nCount = 0;
	CNpc*	pNpc;
	for(IRoleSet::Iter i = m_setRole->Begin(); i != m_setRole->End(); i++)
	{
		IRole* pRole = m_setRole->GetObjByIter(i);
		if(pRole && pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->GetMapID() == idMap && pNpc->GetType() ==  nType)
			nCount++;
	}
	return nCount;
}

int CRoleManager::CountMonsterByName(OBJID idMap, LPCTSTR szName)
{
	int		nCount = 0;
	CMonster*	pMonster;
	for(IRoleSet::Iter i = m_setRole->Begin(); i != m_setRole->End(); i++)
	{
		IRole* pRole = m_setRole->GetObjByIter(i);
		if(pRole && pRole->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)) && pMonster->GetMap()->GetID() == idMap && strcmp(pMonster->GetName(), szName)==0
				&& pMonster->IsAlive())
			nCount++;
	}
	return nCount;
}

int CRoleManager::CountMonsterByGenID(OBJID idMap, OBJID idGen)
{
	int		nCount = 0;
	CMonster*	pMonster;
	for(IRoleSet::Iter i = m_setRole->Begin(); i != m_setRole->End(); i++)
	{
		IRole* pRole = m_setRole->GetObjByIter(i);
		if(pRole && pRole->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)) && pMonster->GetMap()->GetID() == idMap && pMonster->GetGenID() ==  idGen
				&& pMonster->IsAlive())
			nCount++;
	}
	return nCount;
}

CNpc* CRoleManager::QueryDynNpcByRecordID(OBJID idNpcRecord)
{
	CNpc*	pNpc;
	for(IRoleSet::Iter i = m_setRole->Begin(); i != m_setRole->End(); i++)
	{
		IRole* pRole = m_setRole->GetObjByIter(i);
		if(pRole && pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsDynaNpc() && pNpc->GetRecordID() == idNpcRecord)
			return pNpc;
	}
	return NULL;
}
