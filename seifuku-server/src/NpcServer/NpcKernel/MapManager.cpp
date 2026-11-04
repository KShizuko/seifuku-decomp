#include "windows.h"
#include "MapManager.h"
#include "NpcWorld.h"
#include "GameMap.h"

CMapManager::CMapManager()
{
	m_pMapSet = CGameMapSet::CreateNew(true);
}
CMapManager::~CMapManager()
{
	if(m_pMapSet)
		m_pMapSet->Release();
}
bool CMapManager::Create()
{
	CHECKF(m_pMapSet);
	IRecordset* pRes = NpcWorld()->GetDatabase()->CreateNewRecordset("SELECT * FROM " _TBL_MAP " LIMIT 1234567890");
	if(!pRes)
		return false;
	for(int i = 0; i < (int)pRes->RecordCount(); i++, pRes->MoveNext())
	{
		CMapPtr pMap = CGameMap::CreateNew();
		if(!pMap)
		{
			pRes->Release();
			return false;
		}
		if(!pMap->Create(pRes))
		{
			pMap->ReleaseByOwner();
			pRes->Release();
			return false;
		}
		m_pMapSet->AddObj(pMap);
	}
	pRes->Release();
	return true;
}

CGameMap* CMapManager::QueryMap(OBJID idMap)
{
	CHECKF(idMap != ID_NONE);

	for(int i = 0; i < m_pMapSet->GetAmount(); i++)
	{
		CMapPtr pMap = m_pMapSet->GetObjByIndex(i);
		if(pMap && pMap->GetID() == idMap)
		{
			return pMap;
		}
	}

	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT * FROM %s WHERE id=%u && del_flag=0 LIMIT 1", _TBL_DYNAMAP, idMap);
	CAutoPtr<IRecordset> pRes = NpcWorld()->GetDatabase()->CreateNewRecordset(szSQL);
	if(pRes)
	{
		CMapPtr pMap = CGameMap::CreateNew();
		if(!pMap)
		{
			return NULL;
		}

		IF_NOT(pMap->Create(pRes))
		{
			pMap->ReleaseByOwner();
#ifdef	LOCAL_DEBUG
			NpcWorld()->PrintText("Can't init a dyna map data!");
#endif
			return NULL;
		}

		m_pMapSet->AddObj(pMap);
		return pMap;
	}

	return NULL;
}

void CMapManager::OnTimer(DWORD nCurr)
{

}
