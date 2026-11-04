#include "windows.h"
#include "NpcWorld.h"
#include "NpcGenerator.h"
#include "NpcType.h"
#include "Agent.h"
#include "IniFile.h"
CNpcManager::CNpcManager()
{
	m_pNpcSet		= CNpcBigSet::CreateNew(true);
	m_pNpcGenSet	= CNpcGenSet::CreateNew(true);
	m_pNpcTypeSet	= CNpcTypeSet::CreateNew(true);

	m_idLastNewNpc	= MONSTERID_FIRST-1;
	{ CIniFile ini("config.ini", "GameServer"); MAXNPC_PER_ONTIMER = ini.GetInt("MaxNpcPerTimer"); }
	if(MAXNPC_PER_ONTIMER <= 0)
		MAXNPC_PER_ONTIMER = 20;
}

CNpcManager::~CNpcManager()
{
	if(m_pNpcSet)
		m_pNpcSet->Release();
	if(m_pNpcGenSet)
		m_pNpcGenSet->Release();
	if(m_pNpcTypeSet)
		m_pNpcTypeSet->Release();
}

bool CNpcManager::Create()
{
	CHECKF(m_pNpcSet);
	CHECKF(m_pNpcGenSet);
	CHECKF(m_pNpcTypeSet);
	IRecordset* pRes = Database()->CreateNewRecordset("SELECT * FROM " _TBL_GENERATOR, false);
	if(!pRes)
		return false;
	for(int i = 0; i < (int)pRes->RecordCount(); i++,pRes->MoveNext())
	{
		CGenPtr pGen = CNpcGenerator::CreateNew();
		if(!pGen)
		{
			pRes->Release();
			return false;
		}
		if(!pGen->Create(pRes))
		{
			pGen->ReleaseByOwner();
			pRes->Release();
			return false;
		}
		QueryGenSet()->AddObj(pGen);
	}
	pRes->Release();
	pRes = Database()->CreateNewRecordset("SELECT * FROM " _TBL_MONSTERTYPE, false);
	if(!pRes)
		return false;
	for(int i = 0; i < (int)pRes->RecordCount(); i++,pRes->MoveNext())
	{
		CNpcTypePtr pType = CNpcType::CreateNew();
		if(!pType)
		{
			pRes->Release();
			return false;
		}
		if(!pType->Create(pRes))
		{
			pRes->Release();
			pType->ReleaseByOwner();
			return false;
		}
		QueryNpcTypeSet()->AddObj(pType);
	}
	pRes->Release();
	pRes = Database()->CreateNewRecordset("SELECT id,time_of_life FROM " _TBL_USER " WHERE account_id=0", false);
	if(!pRes)
		return false;
	for(int i = 0; i < (int)pRes->RecordCount(); i++,pRes->MoveNext())
	{
		CAgent* pAgent = CAgent::CreateNew();
		if(!pAgent)
		{
			pRes->Release();
			return false;
		}
		if(!pAgent->Create(pRes->LoadDWord("id"), pRes->LoadDWord("time_of_life")))
		{
			pRes->Release();
			pAgent->ReleaseByOwner();
			return false;
		}
		m_setAgentGen.push(pAgent);
	}
	pRes->Release();

	return true;
}

OBJID CNpcManager::SpawnNewNpcID()
{
	if(m_idLastNewNpc < MONSTERID_LAST)
	{
		return ++m_idLastNewNpc;
	}
	else if(m_setRecycle.size())
	{
		return m_setRecycle.pop();
	}
	else
	{
		m_idLastNewNpc = MONSTERID_FIRST;
		::LogSave("CNpcManager::SpawnNewNpcID() MONSTER ID space wrapped at %s, %d", __FILE__, __LINE__);
		return m_idLastNewNpc;
	}
}

int	MAXNPC_PER_ONTIMER	= 20;

void CNpcManager::OnTimer(DWORD nCurr)
{
	int	nMaxSpawn	= MAXNPC_PER_ONTIMER;
	for(int i = 0; i < QueryGenSet()->GetAmount() && nMaxSpawn > 0; i++)
	{
		DEBUG_TRY
		CNpcGenerator* pNpcGenerator = QueryGenSet()->GetObjByIndex(i);
		MYASSERT(pNpcGenerator);
		int nSpawn = pNpcGenerator->GetAmount() - pNpcGenerator->GetGenAmount() + nMaxSpawn;
		if(nSpawn > 0)
			nMaxSpawn -= pNpcGenerator->Generator(__min(nSpawn, nMaxSpawn));
		DEBUG_CATCH("CNpcManager::OnTimer(1)")
	}

	for(CNpcBigSet::Iter iter = QuerySet()->Begin(); iter != QuerySet()->End(); iter++)
	{
		DEBUG_TRY
		CNpcPtr pNpc = QuerySet()->GetObjByIter(iter);
		CNpc* pMonster;
		if(pNpc && !(Cast(&pMonster, pNpc) && pMonster->IsDormancy()))
		{
			pNpc->OnTimer();
		}
		DEBUG_CATCH("CNpcManager::OnTimer(1)")
	}

	if(!m_tAgentDelay.IsActive())
	{
		m_tAgentDelay.Startup(AGENT_GEN_DELAY_SECS);
	}
	else if(m_tAgentDelay.IsTimeOut(AGENT_GEN_DELAY_SECS))
	{
		if(m_setAgentGen.size())
		{
			CAgent* pAgent = m_setAgentGen.pop();
			IF_OK(pAgent)
			{
				IF_OK(pAgent->Login())
					m_pNpcSet->AddObj(Cast<INpc>(pAgent));
				else
					pAgent->ReleaseByOwner();
			}
		}
	}
}

int CNpcManager::CountActiveNpc()
{
	int nCount = 0;
	for(CNpcBigSet::Iter iter = QuerySet()->Begin(); iter != QuerySet()->End(); iter++)
	{
		CNpcPtr pNpc = QuerySet()->GetObjByIter(iter);
		CNpc* pMonster;
		if(pNpc && !(Cast(&pMonster, pNpc) && pMonster->IsDormancy()))
		{
			nCount++;
		}
	}
	return nCount;
}

CAgent* CNpcManager::QueryAgent(LPCTSTR szName)
{
	for(CNpcBigSet::Iter iter = QuerySet()->Begin(); iter != QuerySet()->End(); iter++)
	{
		CNpcPtr pNpc = QuerySet()->GetObjByIter(iter);
		CAgent* pAgent;
		if(pNpc && Cast(&pAgent, pNpc))
		{
			if(strcmp(pAgent->GetName(), szName) == 0)
				return pAgent;
		}
	}
	return NULL;
}
