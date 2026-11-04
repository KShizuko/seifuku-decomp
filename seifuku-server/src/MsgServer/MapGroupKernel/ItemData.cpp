#include "ItemData.h"
#include "I_mydb.h"
#include "MapGroup.h"
#include "ItemAddition.h"

char	szItemTable[] = _TBL_ITEM;
CItemData::CItemData()
{
	m_pData		= NULL;
	m_pType		= NULL;
	m_pAddition	= NULL;
}

CItemData::~CItemData()
{
	if(m_pData)
		m_pData->Release();

}

DWORD CItemData::CalcChkSum(OBJID id, OBJID idPlayer, OBJID idType, int nGem1, int nGem2, int nMagic1, int nMagic2, int nMagic3,
							int nReduceDmg, int nAddLife, int nAntiMonster)
{
	DWORD dwSum = (nGem1 + nGem2 + 1) * 9527 + idType + nMagic1*3 + nMagic2*2 + nMagic3
					+ nReduceDmg + nAddLife + nAntiMonster;
	return dwSum ^ (id + 0xB00CF) ^ idPlayer;
}

DWORD CItemData::CalcChkSum()
{
	return CalcChkSum(GetID(), GetInt(ITEMDATA_PLAYERID), GetInt(ITEMDATA_TYPE),
					GetInt(ITEMDATA_GEM1), GetInt(ITEMDATA_GEM2),
					GetInt(ITEMDATA_MAGIC1), GetInt(ITEMDATA_MAGIC2), GetInt(ITEMDATA_MAGIC3),
					GetInt(ITEMDATA_REDUCE_DMG), GetInt(ITEMDATA_ADD_LIFE), GetInt(ITEMDATA_ANTI_MONSTER));
}

void CItemData::UpdateChkSum()
{
	SetInt(ITEMDATA_CHKSUM, CalcChkSum());
}

bool CItemData::Create(IRecord* pDefault, const ItemInfoStruct* pInfo, bool bInsert , OBJID idNew )
{
	CHECKF(!m_pData);
	CHECKF(!m_pType);
	CHECKF(pInfo);
	CHECKF(pDefault);

	OBJID idItem = pInfo->id;
	if(idNew)
		idItem = idNew;

	m_pData = CGameItemData::CreateNew();
	CHECKF(m_pData);
	IF_NOT(m_pData->Create(pDefault, idItem))
		return false;

	if(idNew)
		SetInt(ITEMDATA_ID_,		idNew);
	SetInt(ITEMDATA_TYPE,			pInfo->idType);
	SetInt(ITEMDATA_OWNERID,		pInfo->idOwner);
	SetInt(ITEMDATA_PLAYERID,		pInfo->idPlayer);
	SetInt(ITEMDATA_AMOUNT,		pInfo->nAmount);
	SetInt(ITEMDATA_AMOUNTLIMIT,	pInfo->nAmountLimit);
	SetInt(ITEMDATA_IDENT,			pInfo->nIdent);
	SetInt(ITEMDATA_POSITION,		pInfo->nPosition);
	SetInt(ITEMDATA_DATA,			pInfo->nData);
	SetInt(ITEMDATA_GEM1,			pInfo->nGem1);
	SetInt(ITEMDATA_GEM2,			pInfo->nGem2);
	SetInt(ITEMDATA_MAGIC1,		pInfo->nMagic1);
	SetInt(ITEMDATA_MAGIC2,		pInfo->nMagic2);
	SetInt(ITEMDATA_MAGIC3,		pInfo->nMagic3);
	SetInt(ITEMDATA_REDUCE_DMG,	pInfo->nReduceDmg);
	SetInt(ITEMDATA_ADD_LIFE,		pInfo->nAddLife);
	SetInt(ITEMDATA_ANTI_MONSTER,	pInfo->nAntiMonster);
	SetInt(ITEMDATA_PLUNDER,		pInfo->usPlunder);
	SetInt(ITEMDATA_SPECIALFLAG,	pInfo->usSpecialFlag);
	SetInt(ITEMDATA_COLOR,			(pInfo->nColor < 2) ? 3 : pInfo->nColor);

	if(bInsert)
	{
		IF_NOT(m_pData->InsertRecord())
			return false;
	}
	else
	{
		m_pData->ClearUpdateFlags();
	}

	m_pType	= ItemType()->QueryItemType(GetInt(ITEMDATA_TYPE));
	IF_NOT(m_pType)
		return false;

	m_pAddition = ItemAddition()->QueryItemAddition(pInfo->idType, pInfo->nMagic3);
	UpdateChkSum();
	SaveInfo();
	return true;
}

bool CItemData::Create (IRecord* pDefault, OBJID idType, OBJID idUser, int nPosition)
{
	CHECKF(!m_pData);
	CHECKF(!m_pType);
	CHECKF(idType != ID_NONE);
	CHECKF(pDefault);

	m_pType	= ItemType()->QueryItemType(idType);
	IF_NOT(m_pType)
		return false;

	m_pData = CGameItemData::CreateNew();
	CHECKF(m_pData);
	IF_NOT(m_pData->Create(pDefault, ID_NONE))
		return false;

	SetInt(ITEMDATA_TYPE,			idType);
	SetInt(ITEMDATA_OWNERID,		idUser);
	SetInt(ITEMDATA_PLAYERID,		idUser);
	SetInt(ITEMDATA_AMOUNT,		GetInt(ITEMDATA_AMOUNT_ORIGINAL));
	SetInt(ITEMDATA_AMOUNTLIMIT,	GetInt(ITEMDATA_AMOUNTLIMIT_ORIGINAL));
	SetInt(ITEMDATA_IDENT,			GetInt(ITEMDATA_IDENT_ORIGINAL));
	SetInt(ITEMDATA_POSITION,		nPosition);
	SetInt(ITEMDATA_GEM1,			GetInt(ITEMDATA_GEM1_ORIGINAL));
	SetInt(ITEMDATA_GEM2,			GetInt(ITEMDATA_GEM2_ORIGINAL));
	SetInt(ITEMDATA_MAGIC1,		GetInt(ITEMDATA_MAGIC1_ORIGINAL));
	SetInt(ITEMDATA_MAGIC2,		GetInt(ITEMDATA_MAGIC2_ORIGINAL));
	SetInt(ITEMDATA_MAGIC3,		GetInt(ITEMDATA_MAGIC3_ORIGINAL));
	SetInt(ITEMDATA_COLOR,			3);

	IF_NOT(m_pData->InsertRecord())
		return false;

	UpdateChkSum();
	SaveInfo();
	m_pAddition = ItemAddition()->QueryItemAddition(idType, GetInt(ITEMDATA_ADDITION));

	return true;
}

bool CItemData::Create(IRecordset* pRes)
{
	CHECKF(!m_pData);
	CHECKF(!m_pType);
	CHECKF(pRes);

	m_pData = CGameItemData::CreateNew();
	IF_NOT(m_pData)
		return false;
	IF_NOT(m_pData->Create(pRes))
		return false;

	m_pType	= ItemType()->QueryItemType(GetInt(ITEMDATA_TYPE));
	if(!m_pType)
	{
		LOGERROR("Unknown item type [%d] in database", GetInt(ITEMDATA_TYPE));
		return false;
	}

	m_pAddition = ItemAddition()->QueryItemAddition(GetInt(ITEMDATA_TYPE), GetInt(ITEMDATA_ADDITION));

	int nPlunder = GetInt(ITEMDATA_PLUNDER);
	if(nPlunder > 0 && GetHourStamp() - nPlunder >= 72)
		SetInt(ITEMDATA_PLUNDER, 0, true);

	return true;
}

bool CItemData::GetInfo	(ItemInfoStruct* pInfo)
{
	CHECKF(pInfo);
	CHECKF(GetID() != ID_NONE);

	pInfo->id			= GetID();
	pInfo->idType		= GetInt(ITEMDATA_TYPE);
	pInfo->idOwner		= GetInt(ITEMDATA_OWNERID);
	pInfo->idPlayer		= GetInt(ITEMDATA_PLAYERID);
	pInfo->nAmount		= GetInt(ITEMDATA_AMOUNT);
	pInfo->nAmountLimit	= GetInt(ITEMDATA_AMOUNTLIMIT);
	pInfo->nIdent		= GetInt(ITEMDATA_IDENT);
	pInfo->nPosition	= GetInt(ITEMDATA_POSITION);
	pInfo->nData		= GetInt(ITEMDATA_DATA);
	pInfo->nGem1		= GetInt(ITEMDATA_GEM1);
	pInfo->nGem2		= GetInt(ITEMDATA_GEM2);
	pInfo->nMagic1		= GetInt(ITEMDATA_MAGIC1);
	pInfo->nMagic2		= GetInt(ITEMDATA_MAGIC2);
	pInfo->nMagic3		= GetInt(ITEMDATA_MAGIC3);
	pInfo->nAddLife		= GetInt(ITEMDATA_ADD_LIFE);
	pInfo->nReduceDmg	= GetInt(ITEMDATA_REDUCE_DMG);
	pInfo->nAntiMonster	= GetInt(ITEMDATA_ANTI_MONSTER);
	pInfo->usPlunder	= GetInt(ITEMDATA_PLUNDER);
	pInfo->usSpecialFlag	= GetInt(ITEMDATA_SPECIALFLAG);
	pInfo->nColor		= GetInt(ITEMDATA_COLOR);

	return true;
}

bool CItemData::SaveInfo(void)
{
	CHECKF(m_pData);
	CHECKF(m_pType);

	m_pData->Update();

	return true;
}

bool CItemData::DeleteRecord()
{
	return m_pData->DeleteRecord();
}

int CItemData::GetHourStamp()
{
	struct tm	tmBase;
	memset(&tmBase, 0, sizeof(tmBase));
	tmBase.tm_year	= 2007 - 1900;
	tmBase.tm_mon	= 0;
	tmBase.tm_mday	= 1;
	time_t	tBase	= mktime(&tmBase);
	return (int)(time(NULL) - tBase) / 3600;
}

void CItemData::SetInt(ITEMDATA idx, int nData, bool bUpdate )
{
	CHECK(idx < ITEMTYPEDATA_OFFSET);
	m_pData->SetInt(idx, nData);

	if((idx >= ITEMDATA_ID_ && idx <= ITEMDATA_TYPE) || idx == ITEMDATA_PLAYERID
			|| (idx >= ITEMDATA_GEM1 && idx <= ITEMDATA_MAGIC3) || (idx >= ITEMDATA_REDUCE_DMG && idx <= ITEMDATA_ANTI_MONSTER))
		UpdateChkSum();

	if(bUpdate)
		m_pData->Update();
}
