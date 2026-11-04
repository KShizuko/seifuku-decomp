#include "Lottery.h"
#include "common.h"

CLottery* CLottery::CreateNew(ST_LOTTERY* pInfo)
{
	CLottery* pLottery = new CLottery;
	IF_NOT(pLottery)
		return NULL;

	if (!pLottery->Create(pInfo))
	{
		SAFE_RELEASE(pLottery);
		return NULL;
	}
	return pLottery;
}

bool CLottery::Create(ST_LOTTERY* pInfo)
{
	m_info.id			= pInfo->id;
	m_info.nType		= pInfo->nType;
	m_info.nRank		= pInfo->nRank;
	m_info.nChance		= pInfo->nChance;
	m_info.strPrizeName	= pInfo->strPrizeName.c_str();
	m_info.idPrizeItem	= pInfo->idPrizeItem;
	m_info.nColor		= pInfo->nColor;
	m_info.nHoleNum		= pInfo->nHoleNum;
	m_info.nAdditionLev	= pInfo->nAdditionLev;
	return true;
}
