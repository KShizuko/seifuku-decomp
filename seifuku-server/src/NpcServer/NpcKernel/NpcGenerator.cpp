#include "NpcGenerator.h"
#include "BaseFunc.h"
#include "NpcWorld.h"
#include "AllMsg.h"

char	szGenerator[] = _TBL_GENERATOR;
const int	RANDOM_GENERATOR_SECS			= 600;

CNpcGenerator::CNpcGenerator()
{
	SetObjType(OBJ_NPCGEN);
	m_pData	= CNpcGenData::CreateNew();

	m_nAmount			= 0;
	m_nGenAmount		= 0;
	m_nCurrGen			= 0;
	m_nTotalGen			= 0;
	m_tBadGen			= 0;
}

CNpcGenerator::~CNpcGenerator()
{
	::MyLogSave("gmlog/npc_generator", "generator:%09u, monster:%09d", GetID(), m_nTotalGen);

	if(m_pData)
		m_pData->Release();
}

bool CNpcGenerator::Create(IRecordset* pRes)
{
	if(!m_pData->Create(pRes))
		return false;

	m_timeoutRest.SetInterval(GetInt(NPCGENDATA_RESTSECS) + ::RandGet(GetInt(NPCGENDATA_RESTSECS)));
	m_timeoutRest.Update();

	int nGrid = GetInt(NPCGENDATA_GRID);
	if(nGrid < 1)
		m_nMaxNpc = 1;
	else
		m_nMaxNpc		= (GetInt(NPCGENDATA_BOUNDCX) / nGrid) * (GetInt(NPCGENDATA_BOUNDCY) / nGrid);
	if(m_nMaxNpc < 1)
		m_nMaxNpc = 1;

	m_pMap			= MapManager()->QueryMap(GetInt(NPCGENDATA_MAPID));

	if (!m_pMap)
	{
		LOGERROR("Error: npc generator [%u] got invalid map [%u]!", this->GetID(), this->GetInt(NPCGENDATA_MAPID));
		return false;
	}

	return true;
}

POINT CNpcGenerator::FindGenPos()
{
	POINT pos;

	if(GetInt(NPCGENDATA_BORNX) > 0)
	{
		pos.x = GetInt(NPCGENDATA_BORNX);
		pos.y = GetInt(NPCGENDATA_BORNY);
		return pos;
	}

	pos.x = GetInt(NPCGENDATA_BOUNDX) + ::RandGet(GetInt(NPCGENDATA_BOUNDCX));
	pos.y = GetInt(NPCGENDATA_BOUNDY) + ::RandGet(GetInt(NPCGENDATA_BOUNDCY));
	return pos;
}

int CNpcGenerator::Generator(int nNum)
{
	CHECKF(nNum > 0);

	if(!m_pMap)
		return 0;

	if(GetInt(NPCGENDATA_BEGIN))
	{
		if(!::IsTimeRange(GetInt(NPCGENDATA_BEGIN), GetInt(NPCGENDATA_END)))
			return 0;
	}

	if(m_nGenAmount >= GetMaxNpc())
		return 0;
	if(nNum > GetMaxNpc() - m_nGenAmount)
		nNum = GetMaxNpc() - m_nGenAmount;

	if(m_timeoutRest.ToNextTime())
	{
		if(GetInt(NPCGENDATA_RESTSECS) >= RANDOM_GENERATOR_SECS)
			m_timeoutRest.Startup(GetInt(NPCGENDATA_RESTSECS) + ::RandGet(GetInt(NPCGENDATA_RESTSECS)));

		m_nCurrGen	= GetInt(NPCGENDATA_MAXPERGEN);
	}

	if(m_nCurrGen <= 0)
		return 0;
	if(nNum > m_nCurrGen)
		nNum = m_nCurrGen;

	int nMaxTry = GetInt(NPCGENDATA_BOUNDCX) * GetInt(NPCGENDATA_BOUNDCY);
	if(nMaxTry <= 0)
		nMaxTry = 1;

	int nGenNum = 0;
	while(nGenNum < nNum && m_nCurrGen > 0)
	{

		POINT pos;
		int nTry = 0;
		for(; nTry < nMaxTry; nTry++)
		{
			pos	= FindGenPos();
			if(m_pMap->IsStandEnable(pos.x, pos.y))
				break;
		}
		if(nTry < nMaxTry)
		{
			OBJID idNewNpc	= NpcManager()->SpawnNewNpcID();

			CMsgAiNpcInfo		msg;
			if(msg.Create(MSGAINPCINFO_CREATENEW, idNewNpc, GetID(), GetInt(NPCGENDATA_NPCTYPE), GetInt(NPCGENDATA_MAPID), pos.x, pos.y))
			{
				m_nCurrGen--;
				m_nGenAmount++;
				m_nTotalGen++;

				nGenNum++;
				NpcWorld()->SendMsg(ID_NONE, &msg);
			}
			continue;
		}

		if(nGenNum <= 0 && nMaxTry >= 9)
		{
			time_t	tNow = time(NULL);
			if(::abs(tNow - m_tBadGen) >= 1200)
			{
				m_tBadGen	= tNow;
				LOGERROR("Bad NPC generator %u at %s, %d", GetID(), __FILE__, __LINE__);
			}
		}
		break;
	}

	return nGenNum;
}

void CNpcGenerator::GeneratorOK(OBJID idNpc)
{
	CHECK(idNpc != ID_NONE);

	m_nAmount++;

	if(GetInt(NPCGENDATA_MAXPERGEN) > 0 && m_nGenAmount - m_nAmount > MAXNPC_PER_ONTIMER)
		LOGERROR("GeneratorID[%u]: m_nGenAmount[%d] > m_nAmount[%d]", GetID(), m_nGenAmount, m_nAmount);
}

void CNpcGenerator::DeleteNpc(OBJID idNpc)
{
	CHECK(m_nAmount > 0);

	m_nAmount--;

	if(GetInt(NPCGENDATA_MAXPERGEN) > 0)
	{
		m_nGenAmount--;
		if(m_nGenAmount < m_nAmount)
		{
			LOGERROR("GeneratorID[%u]: m_nGenAmount[%d] < m_nAmount[%d]", GetID(), m_nGenAmount, m_nAmount);
			m_nGenAmount = m_nAmount;
		}
	}
}

bool CNpcGenerator::IsTooFar(OBJID idMap, int x, int y, int nRange)
{
	if(m_pMap && m_pMap->GetID() == idMap)
	{
		if(x >= GetInt(NPCGENDATA_BOUNDX) - nRange
				&& x < GetInt(NPCGENDATA_BOUNDX) + GetInt(NPCGENDATA_BOUNDCX) + nRange
				&& y >= GetInt(NPCGENDATA_BOUNDY) - nRange
				&& y < GetInt(NPCGENDATA_BOUNDY) + GetInt(NPCGENDATA_BOUNDCY) + nRange)
			return false;
	}
	return true;
}
