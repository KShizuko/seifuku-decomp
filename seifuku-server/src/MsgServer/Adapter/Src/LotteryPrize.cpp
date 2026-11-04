#include "LotteryPrize.h"

ILotteryPrize* LotteryPrizeCreate(CItem* pItem, PROCESS_ID idProcess)
{
	if (!idProcess)
		return NULL;

	CLotteryPrize* pPrize = new CLotteryPrize(pItem, idProcess);
	IF_NOT(pPrize)
		return NULL;
	return pPrize;
}

CLotteryPrize::CLotteryPrize(CItem* pItem, PROCESS_ID idProcess)
{
	m_pItem		= pItem;
	m_idProcess	= idProcess;
}

bool CLotteryPrize::IsEquip()
{
	if (m_pItem->IsWeapon())
		return true;
	int nSort = CItem::GetItemSort(m_pItem->GetInt(ITEMDATA_TYPE));
	return nSort == 1 || nSort == 2 || nSort == 3 || nSort == 7 || nSort == 8 || nSort == 6;
}

bool CLotteryPrize::SetInt(int nIdx, int nData)
{
	if (nIdx == 0)
	{
		if (nData == 1)
		{
			m_pItem->SetInt(ITEMDATA_GEM1, GEM_HOLE, false);
			m_pItem->SetInt(ITEMDATA_GEM2, GEM_NONE, false);
		}
		else if (nData == 2)
		{
			m_pItem->SetInt(ITEMDATA_GEM1, GEM_HOLE, false);
			m_pItem->SetInt(ITEMDATA_GEM2, GEM_HOLE, false);
		}
		else if (nData == 0)
		{
			m_pItem->SetInt(ITEMDATA_GEM1, GEM_NONE, false);
			m_pItem->SetInt(ITEMDATA_GEM2, GEM_NONE, false);
		}
	}
	else if (nIdx == 1)
	{
		if (nData > 0)
			m_pItem->SetInt(ITEMDATA_MAGIC3, nData, false);
	}
	return true;
}

bool CLotteryPrize::GetInfo(PLUGIN_ITEMINFO* pInfo)
{
	ItemInfoStruct info;
	m_pItem->GetInfo(&info);

	pInfo->id				= info.id;
	pInfo->idType			= info.idType;
	pInfo->idOwner			= info.idOwner;
	pInfo->idPlayer			= info.idPlayer;
	pInfo->nAmount			= info.nAmount;
	pInfo->nAmountLimit		= info.nAmountLimit;
	pInfo->nIdent			= info.nIdent;
	pInfo->nPosition		= info.nPosition;
	pInfo->nData			= info.nData;
	pInfo->nGem1			= info.nGem1;
	pInfo->nGem2			= info.nGem2;
	pInfo->nMagic1			= info.nMagic1;
	pInfo->nMagic2			= info.nMagic2;
	pInfo->nMagic3			= info.nMagic3;
	pInfo->nReduceDmg		= (UCHAR)info.nReduceDmg;
	pInfo->nAddLife			= (UCHAR)info.nAddLife;
	pInfo->nAntiMonster		= info.nAntiMonster;
	pInfo->usPlunder		= info.usPlunder;
	pInfo->usSpecialFlag	= info.usSpecialFlag;
	return true;
}
