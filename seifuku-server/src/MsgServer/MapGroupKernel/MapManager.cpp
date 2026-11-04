#include "GameObj.h"
#include "MapManager.h"
#include "MapGroup.h"
#include "MapItem.h"
#include "Npc.h"

MYHEAP_IMPLEMENTATION(CMapManager,s_heap)
CMapManager::CMapManager(PROCESS_ID idProcess) : m_idProcess(idProcess)
{
	m_pMapSet		= CMapSet::CreateNew(true);
	m_setPortal		= CPortalSet::CreateNew(true);
	m_setPassway	= CPasswaySet::CreateNew(true);
	m_nMaxMapItem	= 0;
	m_tMapItemLog	= 0;

	m_pMapItemSet	= CMapItemSet::CreateNew(true);
	m_idLastMapItem	= MAPITEM_FIRSTID - 1;
	m_tMapItem.SetInterval(MAPITEM_ONTIMER_SECS);
	m_tMapItem.Update();

	m_idLastPet		= (PETID_FIRST-1) + MapGroup(PID)->GetID();
	m_idLastDynaNpc		= (DYNANPCID_FIRST-1) + MapGroup(PID)->GetID();
	m_idLastTrap		= (MAGICTRAPID_FIRST-1) + MapGroup(PID)->GetID();
	m_idLastCallPet		= (CALLPETID_FIRST-1) + MapGroup(PID)->GetID();
}

CMapManager::~CMapManager()
{
	if(m_pMapItemSet)
		m_pMapItemSet->Release();
	if(m_setPortal)
		m_setPortal->Release();
	if(m_setPassway)
		m_setPassway->Release();
	if(m_pMapSet)
		m_pMapSet->Release();
}

void	CMapManager::Release()
{
	delete this;
}

void	CMapManager::OnTimer(time_t tCurr)
{
	for(int i = 0; i < m_pMapSet->GetAmount(); i++)
	{
		CGameMap* pMap = m_pMapSet->GetObjByIndex(i);
		if(pMap)
			pMap->OnTimer(tCurr);
	}

	if(m_tMapItem.ToNextTime())
	{
		int nAmount = QueryMapItemSet()->GetAmount();
		for(int i = nAmount-1; i >= 0; i--)
		{
			CMapItem* pMapItem = QueryMapItemSet()->GetObjByIndex(i);
			if(pMapItem)
			{
				if(!pMapItem->IsAlive())
				{
					QueryMapItemSet()->DelObj(pMapItem->GetID());
				}
			}
		}
		if(m_nMaxMapItem < nAmount)
			m_nMaxMapItem = nAmount;
		if(nAmount > 5000)
		{
			time_t tNow = time(NULL);
			if(abs(tNow - m_tMapItemLog) >= 1200)
			{
				LOGMSG("MapManager[%u] item count %u / %u at %s, %d", m_idProcess, nAmount, m_nMaxMapItem, __FILE__, __LINE__);
				m_tMapItemLog = tNow;
			}
		}
	}
}

bool CMapManager::Create()
{
	CHECKF(m_pMapSet);
	CHECKF(m_setPortal);
	CHECKF(m_setPassway);
	CHECKF(m_pMapItemSet);

	SQLBUF	szSQL;
#ifdef _DEBUGx
	sprintf(szSQL, "SELECT * FROM %s WHERE (idxserver=-1 || idxserver=%u) && mapgroup=%u limit 1", _TBL_MAP, GetServerID(), MapGroup(PID)->GetMapGroup());
#else
	sprintf(szSQL, "SELECT * FROM cq_map WHERE (idxserver=-1 || idxserver=%u) && mapgroup=%u", GetServerID(), MapGroup(PID)->GetMapGroup());
#endif
	IRecordset* pRes = Database()->CreateNewRecordset(szSQL, false);
	if(!pRes)
		return false;
	int nCount = 0;
	for(int i = 0; i < (int)pRes->RecordCount(); i++, pRes->MoveNext())
	{
		CMapPtr pMap = CGameMap::CreateNew();
		if(!pMap)
		{
			pRes->Release();
			return false;
		}
#ifdef	PALED_DEBUG
		if(pRes->GetInt(0) == 1041 || pRes->GetInt(0)>=1060 && pRes->GetInt(0)<=1062)
		{
			pMap->ReleaseByOwner();
			continue;
		}
#endif
		if(!pMap->Create(m_idProcess, pRes))
		{
			pMap->ReleaseByOwner();
#ifdef	LOCAL_DEBUG
			MapGroup(PID)->QueryIntraMsg()->PrintText("Can't init a map data!");
			continue;
#endif
			pRes->Release();
			return false;
		}
		if(pMap->GetID() == DEFAULT_LOGIN_MAPID)
			pMap->m_bNewbieMap	= true;

		m_pMapSet->AddObj(pMap);
		nCount++;
		DebugSave("load map :%u", pMap->GetID());
	}
	pRes->Release();
	LOGMSG("Load Map %d in mapgroup %d.", nCount, MapGroup(PID)->GetMapGroup());

	bool ret = m_setPortal->Create("SELECT * from cq_portal", Database());
	IF_NOT_(ret)
		return false;
	ret = m_setPassway->Create("SELECT * from cq_passway", Database());
	IF_NOT_(ret)
		return false;

	return true;
}

OBJID CMapManager::SpawnMapItemID()
{
	m_idLastMapItem++;

	if(m_idLastMapItem > MAPITEM_LASTID)
	{
		LOGERROR("CMapManager::SpawnMapItemID() MapItemID ID space wrapped at %s, %d", __FILE__, __LINE__);
		m_idLastMapItem	= MAPITEM_FIRSTID;
	}

	return m_idLastMapItem;
}

PROCESS_ID CMapManager::GetProcessID(OBJID idMap)
{
	extern IStaticMapSet* g_setStaticMap;
	for(int i = 0; i < g_setStaticMap->GetAmount(); i++)
	{
		CGameMapData* pData = g_setStaticMap->GetObjByIndex(i);
		if(pData && pData->GetID() == idMap)
			return pData->GetInt(GAMEMAPDATA_MAPGROUP) + MSGPORT_MAPGROUP_FIRST;
	}

	return PROCESS_NONE;
}

bool CMapManager::FindPortal(POINT* pPos, OBJID idMap, int idxPortal)
{
	CHECKF(m_setPortal);
	CHECKF(idMap != ID_NONE && pPos);
	for(int i = 0; i < m_setPortal->GetAmount(); i++)
	{
		CPortalData* pData = m_setPortal->GetObjByIndex(i);
		if(pData && pData->GetInt(PORTALDATA_MAPID) == idMap && pData->GetInt(PORTALDATA_INDEX) == idxPortal)
		{
			pPos->x	= pData->GetInt(PORTALDATA_X);
			pPos->y	= pData->GetInt(PORTALDATA_Y);
			return true;
		}
	}
	return false;
}

bool CMapManager::FindPassway(OBJID* pidTarget, int* pidxPortal, OBJID idMap, int idxPortal)
{
	CHECKF(m_setPassway);
	CHECKF(idMap != ID_NONE && pidTarget && pidxPortal);

	for(int i = 0; i < m_setPassway->GetAmount(); i++)
	{
		CPasswayData* pData = m_setPassway->GetObjByIndex(i);
		if(pData && pData->GetInt(PASSWAYDATA_MAPID) == idMap && pData->GetInt(PASSWAYDATA_INDEX) == idxPortal)
		{
			*pidTarget	= pData->GetInt(PASSWAYDATA_PROTALMAP);
			*pidxPortal	= pData->GetInt(PASSWAYDATA_PROTALIDX);
			return true;
		}
	}
	return false;
}

OBJID CMapManager::SpawnNewPetID()
{
	int nNextID = m_idLastPet + MapGroup(PID)->GetMapGroupAmount();
	if(nNextID < PETID_LAST)
	{
		m_idLastPet = nNextID;
		return nNextID;
	}
	else if(m_setPetRecycle.size())
	{
		return m_setPetRecycle.pop();
	}
	else
	{
		m_idLastPet = (PETID_FIRST-1) + MapGroup(PID)->GetID();
		LOGERROR("CMapManager::SpawnNewPetID() PETID ID space wrapped at %s, %d", __FILE__, __LINE__);
		return m_idLastPet;
	}
}

OBJID CMapManager::SpawnNewNpcID()
{
	int nNextID = m_idLastDynaNpc + MapGroup(PID)->GetMapGroupAmount();
	if(nNextID < DYNANPCID_LAST)
	{
		m_idLastDynaNpc = nNextID;
		return nNextID;
	}
	else if(m_setDynaNpcRecycle.size())
	{
		return m_setDynaNpcRecycle.pop();
	}
	else
	{
		m_idLastDynaNpc = (DYNANPCID_FIRST-1) + MapGroup(PID)->GetID();
		LOGERROR("CMapManager::SpawnNewNpcID() DYNANPC ID space wrapped at %s, %d", __FILE__, __LINE__);
		return m_idLastDynaNpc;
	}
}

OBJID CMapManager::SpawnNewTrapID()
{
	int nNextID = m_idLastTrap + MapGroup(PID)->GetMapGroupAmount();
	if(nNextID < MAGICTRAPID_LAST)
	{
		m_idLastTrap = nNextID;
		return nNextID;
	}
	else if(m_setTrapRecycle.size())
	{
		return m_setTrapRecycle.pop();
	}
	else
	{
		m_idLastTrap = (MAGICTRAPID_FIRST-1) + MapGroup(PID)->GetID();
		LOGERROR("CMapManager::SpawnNewTrapID() MAGICTRAP ID space wrapped at %s, %d", __FILE__, __LINE__);
		return m_idLastTrap;
	}
}

OBJID CMapManager::SpawnNewCallPetID()
{
	int nNextID = m_idLastCallPet + MapGroup(PID)->GetMapGroupAmount();
	if(nNextID < CALLPETID_LAST)
	{
		m_idLastCallPet = nNextID;
		return nNextID;
	}
	else if(m_setCallPetRecycle.size())
	{
		return m_setCallPetRecycle.pop();
	}
	else
	{
		m_idLastCallPet = (CALLPETID_FIRST-1) + MapGroup(PID)->GetID();
		LOGERROR("CMapManager::SpawnNewCallPetID() CALLPETID ID space wrapped at %s, %d", __FILE__, __LINE__);
		return m_idLastCallPet;
	}
}

CGameMap* CMapManager::GetGameMap(OBJID idMap, BOOL bLoad )
{
	CGameMap* pMap = m_pMapSet->GetObj(idMap);
	if(pMap)
		return pMap;

	if(!bLoad)
		return NULL;

	if(!IsDynaMap(idMap))
		return NULL;

	LOGDUMP("Load dyna map %d", idMap);
	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT * FROM cq_dynamap WHERE id=%u && mapgroup=%u && del_flag=0 LIMIT 1", idMap, MapGroup(PID)->GetMapGroup());
	IRecordset* pRes = Database()->CreateNewRecordset(szSQL);
	if(!pRes)
	{
		return NULL;
	}

	{
		pMap = CGameMap::CreateNew();
		if(!pMap)
		{
			pRes->Release();
			return NULL;
		}

		if(!pMap->Create(m_idProcess, pRes))
		{
			pMap->ReleaseByOwner();
			pRes->Release();
			return NULL;
		}

		m_pMapSet->AddObj(pMap);
	}
	pRes->Release();
	LOGDUMP("Load dyna map %d OK!", idMap);

	sprintf(szSQL, "SELECT * FROM cq_dynanpc WHERE mapid=%u", idMap);
	 pRes = Database()->CreateNewRecordset(szSQL, false);
	CHECKF(pRes);
	for(int i = 0; i < (int)pRes->RecordCount(); i++, pRes->MoveNext())	{
		CNpcPtr pNpc = CNpc::CreateNew();
		IF_OK(pNpc)
		{
			IF_NOT(pNpc->Create(m_idProcess, pRes, true))
				pNpc->ReleaseByOwner();
			else
				RoleManager()->QuerySet()->AddObj(pNpc->QueryRole());
		}
	}
	pRes->Release();

	sprintf(szSQL, "SELECT * FROM cq_pet WHERE recordmap_id=%u", idMap);
	 pRes = Database()->CreateNewRecordset(szSQL, false);
	CHECKF(pRes);
	for(int i = 0; i < (int)pRes->RecordCount(); i++, pRes->MoveNext())	{
		CMonster* pMonster = CMonster::CreateNew();
		IF_OK(pMonster)
		{
			IF_OK(pMonster->Create(m_idProcess, pRes))
			{
				RoleManager()->QuerySet()->AddObj(pMonster->QueryRole());
				pMonster->EnterMapGroup();
			}
			else
				pMonster->ReleaseByOwner();
		}
	}
	pRes->Release();

	return pMap;
}

OBJID CMapManager::CreateDynaMap(const NewMapInfo* pInfo)
{
	CGameMap* pMap = CGameMap::CreateNew();
	CHECKF(pMap);

	OBJID	idMap = pMap->CreateDynaMap(m_idProcess, pInfo);
	if(idMap)
	{
		m_pMapSet->AddObj(pMap);
		return idMap;
	}
	else
		pMap->ReleaseByOwner();

	return ID_NONE;
}
