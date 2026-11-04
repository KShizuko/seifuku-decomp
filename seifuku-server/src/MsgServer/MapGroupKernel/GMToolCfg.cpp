#include "windows.h"
#include "define.h"
#include "GMToolCfg.h"
#include "I_mydb.h"

CGMToolCfg*	g_pGMToolCfg	= NULL;
CGMToolCfg::CGMToolCfg()
{
}

CGMToolCfg::~CGMToolCfg()
{
}

CGMToolCfg* CGMToolCfg::CreateNew(IDatabase* pDb)
{
	CGMToolCfg* pCfg = new CGMToolCfg;
	if(!pCfg)
		return NULL;

	if(pCfg->Create(pDb))
		return pCfg;

	pCfg->Release();
	return NULL;
}

void CGMToolCfg::Release()
{
	delete this;
}

bool CGMToolCfg::Create(IDatabase* pDb)
{
	CHECKF(pDb);

	char szSQL[1024];
	sprintf(szSQL, "SELECT * FROM cq_config where type between 1020 and 1029");
	IRecordset* pRes = pDb->CreateNewRecordset(szSQL);
	if(pRes)
	{
		for(int i = 0; i < (int)pRes->RecordCount(); i++)
		{
			GMTOOL_REGION	region;
			region.idMap	= pRes->GetInt(GMTOOLCFGDATA_MAPID);
			::SetRect(&region.rc, pRes->GetInt(GMTOOLCFGDATA_X), pRes->GetInt(GMTOOLCFGDATA_Y),
						pRes->GetInt(GMTOOLCFGDATA_X) + pRes->GetInt(GMTOOLCFGDATA_CX),
						pRes->GetInt(GMTOOLCFGDATA_Y) + pRes->GetInt(GMTOOLCFGDATA_CY));
			m_setRegion.push_back(region);

			pRes->MoveNext();
		}
		pRes->Release();
	}
	return true;
}

bool CGMToolCfg::IsInRegion(OBJID idMap, POINT pos)
{
	for(std::vector<GMTOOL_REGION>::iterator iter = m_setRegion.begin(); iter != m_setRegion.end(); iter++)
	{
		if(iter->idMap == idMap && ::PtInRect(&iter->rc, pos))
			return true;
	}
	return false;
}
