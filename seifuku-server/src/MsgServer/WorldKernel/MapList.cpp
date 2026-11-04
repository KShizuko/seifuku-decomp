#include "MessagePort.h"
#include "common.h"
#include "NetMsg.h"
#include "I_mydb.h"
#include "protocol.h"
using namespace world_kernel;
#include "MapList.h"
#include "WorldKernel.h"
CMapList::CMapList()
{
}

CMapList::~CMapList()
{
	for(int i = 0; i < (int)m_setMap.size(); i++)
		delete m_setMap[i];
}
bool CMapList::Create(IDatabase* pDb)
{
	CHECKF(pDb);

	IRecordset* pRes = pDb->CreateNewRecordset("SELECT id,mapgroup,portal0_x,portal0_y FROM " _TBL_MAP " LIMIT 1234567890");
	CHECKF(pRes);
	int i;
	for(i = 0; i < (int)pRes->RecordCount(); i++)
	{
		CMapSt*	pMap = new CMapSt;
		if(pMap)
		{
			pMap->idMap		= pRes->LoadDWord("id");
			pMap->idxMapGroup	= pRes->LoadDWord("mapgroup");
			pMap->nPortalX		= pRes->LoadDWord("portal0_x");
			pMap->nPortalY		= pRes->LoadDWord("portal0_y");
			m_setMap.push_back(pMap);
		}
		pRes->MoveNext();
	}
	pRes->Release();

	pRes = pDb->CreateNewRecordset("SELECT id,mapgroup,portal0_x,portal0_y FROM " _TBL_DYNAMAP " LIMIT 1234567890");
	if(pRes)
	{
		for( i = 0; i < (int)pRes->RecordCount(); i++)
		{
			CMapSt*	pMap = new CMapSt;
			if(pMap)
			{
				pMap->idMap		= pRes->LoadDWord("id");
				pMap->idxMapGroup	= pRes->LoadDWord("mapgroup");
				pMap->nPortalX		= pRes->LoadDWord("portal0_x");
				pMap->nPortalY		= pRes->LoadDWord("portal0_y");
				m_setMap.push_back(pMap);
			}
			pRes->MoveNext();
		}
		pRes->Release();
	}

	return true;
}

PROCESS_ID CMapList::GetMapProcessID(OBJID idMap)
{
	for(int i = 0; i < (int)m_setMap.size(); i++)
	{
		if(m_setMap[i])
		{
			if(m_setMap[i]->idMap == idMap)
				return m_setMap[i]->idxMapGroup + MSGPORT_MAPGROUP_FIRST;
		}
	}

	int	nMapGroup	= MSGPORT_MAPGROUP_FIRST + DEFAULT_LOGIN_MAPGROUP;
	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT id,mapgroup,portal0_x,portal0_y FROM %s WHERE id=%u LIMIT 1", _TBL_DYNAMAP, idMap);
	IRecordset* pRes = GameWorld()->GetDatabase()->CreateNewRecordset(szSQL);
	if(pRes)
	{
		CMapSt*	pMap = new CMapSt;
		if(pMap)
		{
			pMap->idMap		= pRes->LoadDWord("id");
			pMap->idxMapGroup	= pRes->LoadDWord("mapgroup");
			pMap->nPortalX		= pRes->LoadDWord("portal0_x");
			pMap->nPortalY		= pRes->LoadDWord("portal0_y");
			m_setMap.push_back(pMap);

			nMapGroup	= pMap->idxMapGroup;
		}
		pRes->Release();
	}

	return nMapGroup;
}

CMapSt* CMapList::GetMap(OBJID idMap)
{
	for(int i = 0; i < (int)m_setMap.size(); i++)
	{
		if(m_setMap[i] && m_setMap[i]->idMap == idMap)
			return m_setMap[i];
	}

	return NULL;
}
