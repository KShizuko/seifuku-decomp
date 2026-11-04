#include "MessagePort.h"
#include "AllMsg.h"
#include "protocol.h"
#include "MapGroup.h"
#include "MapTrap.h"

MYHEAP_IMPLEMENTATION(CNpcManager,s_heap)
CNpcManager::CNpcManager(PROCESS_ID idProcess) : m_idProcess(idProcess)
{
}

CNpcManager::~CNpcManager()
{
}

bool CNpcManager::Create()
{
	SQLBUF szSQL;
	sprintf(szSQL, "SELECT * FROM %s WHERE (idxserver=-1 || idxserver=%u)", _TBL_NPC, GetServerID());
	IRecordset* pRes = Database()->CreateNewRecordset(szSQL, false);
	if(!pRes)
		return false;
	int i;
	for(i = 0; i < (int)pRes->RecordCount(); i++)
	{
		CNpcPtr pNpc = CNpc::CreateNew();
		if(!pNpc)
		{
			pRes->Release();
			return false;
		}

		if(!pNpc->Create(m_idProcess, pRes, false))
			pNpc->ReleaseByOwner();
		else
			RoleManager()->QuerySet()->AddObj(pNpc->QueryRole());

		pRes->MoveNext();
	}
	pRes->Release();

	sprintf(szSQL, "SELECT * FROM %s WHERE (idxserver=-1 || idxserver=%u)", _TBL_DYNANPC, GetServerID());
	 pRes = Database()->CreateNewRecordset(szSQL, false);
	if(!pRes)
		return false;
	for( i = 0; i < (int)pRes->RecordCount(); i++)
	{
		CNpcPtr pNpc = CNpc::CreateNew();
		if(!pNpc)
		{
			pRes->Release();
			return false;
		}

		if(!pNpc->Create(m_idProcess, pRes, true))
			pNpc->ReleaseByOwner();
		else
			RoleManager()->QuerySet()->AddObj(pNpc->QueryRole());

		pRes->MoveNext();
	}
	pRes->Release();

	sprintf(szSQL, "SELECT * FROM %s", _TBL_TRAP);
	 pRes = Database()->CreateNewRecordset(szSQL, false);
	if(!pRes)
		return false;
	for( i = 0; i < (int)pRes->RecordCount(); i++, pRes->MoveNext())
	{
		CMapTrap* pTrap = CMapTrap::CreateNew(m_idProcess, pRes);
		if(pTrap)
			RoleManager()->QuerySet()->AddObj(pTrap->QueryRole());
	}
	pRes->Release();

	return true;
}

bool CNpcManager::DelAllMonster()
{
	DEBUG_TRY
	LOGMSG("DelAllMonster() start...");
	vector<OBJID>	setDel;
	IRoleSet::Iterator pRole = RoleManager()->QuerySet()->NewEnum();
	while(pRole.Next())
	{
		CNpc* pNpc;
		CUser* pUser;
		if(pRole && (pRole->QueryObj(OBJ_MONSTER, IPP_OF(pNpc)) || pRole->QueryObj(OBJ_USER, IPP_OF(pUser)) && pUser->IsAgent()))
		{
			OBJID idRole = pRole->GetID();
			if(idRole)
				setDel.push_back(idRole);
		}
	}
	for(vector<OBJID>::iterator it = setDel.begin(); it != setDel.end(); it++)
		RoleManager()->QuerySet()->DelObj(*it);
	LOGMSG("DelAllMonster() ok.");
	DEBUG_CATCH("delete all npc")

	return true;
}

bool CNpcManager::CreateMonster(const ST_CREATENEWNPC* pInfo)
{
	CMonsterPtr pNpc = CMonster::CreateNew();
	if(pNpc)
	{
		CNpcType*	pType = MonsterType()->GetObj(pInfo->usType);
		if(pType)
		{
			ST_CREATENEWNPC	info;
			memcpy(&info, pInfo, sizeof(ST_CREATENEWNPC));
			info.idOwner	= ID_NONE;
			info.ucOwnerType= OWNER_NONE;
			info.usAction	= MSGAINPCINFO_CREATENEW;

			CMapPtr pMap = MapManager()->GetGameMap(pInfo->idMap);
			if(pMap && pNpc->Create(m_idProcess, pType, &info))
			{
				RoleManager()->QuerySet()->AddObj(pNpc->QueryRole());
				pNpc->EnterMapGroup();

				return true;
			}
		}
		pNpc->ReleaseByOwner();
	}
	return false;
}

bool CNpcManager::CreateSynPet(const ST_CREATENEWNPC* pInfo, LPCTSTR pszName )
{
	CHECKF(pInfo);

	CMonsterPtr pPet = CMonster::CreateNew();
	if(pPet)
	{
		CNpcType*	pType = MonsterType()->GetObj(pInfo->usType);
		if(pType)
		{
			ST_CREATENEWNPC	info;
			memcpy(&info, pInfo, sizeof(ST_CREATENEWNPC));
			info.id				= MapManager()->SpawnNewPetID();
			info.usAction		= MSGAINPCINFO_CREATENEW;

			CMapPtr pMap = MapManager()->QueryMap(pInfo->idMap);
			if(pMap && pPet->Create(m_idProcess, pType, &info, pszName))
			{
				RoleManager()->QuerySet()->AddObj(pPet->QueryRole());
				pPet->EnterMapGroup();

				return true;
			}
		}
		pPet->ReleaseByOwner();
	}
	return false;
}

CMonster* CNpcManager::CreateCallPet(CUser* pUser, OBJID idType, int x, int y)
{
	CHECKF(pUser);

	CMonsterPtr pPet = CMonster::CreateNew();
	if(pPet)
	{
		CNpcType*	pType = MonsterType()->GetObj(idType);
		if(pType)
		{
			ST_CREATENEWNPC	info;
			memset(&info, 0, sizeof(ST_CREATENEWNPC));
			info.id				= MapManager()->SpawnNewCallPetID();
			info.usAction		= MSGAINPCINFO_CREATENEW;
			info.usType			= idType;
			info.ucOwnerType	= OWNER_USER;
			info.idOwner		= pUser->GetID();
			info.idMap			= pUser->GetMapID();
			info.usPosX			= x;
			info.usPosY			= y;
			info.idData			= 0;
			info.nData			= 0;

			CMapPtr pMap = pUser->GetMap();
			if(pMap && pPet->Create(m_idProcess, pType, &info, pUser))
			{
				RoleManager()->QuerySet()->AddObj(pPet->QueryRole());
				pPet->EnterMapGroup();

				return pPet;
			}
		}
		pPet->ReleaseByOwner();
	}
	return NULL;
}
