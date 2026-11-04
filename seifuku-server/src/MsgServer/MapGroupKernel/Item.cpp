#include "Item.h"
#include "ItemType.h"
#include "MapGroup.h"
#include "I_mydb.h"
#include "User.h"
#include "netmsg.h"
#include "AllMsg.h"

static ITEMSCORE_SET	s_setGem1Score;
static ITEMSCORE_SET	s_setGem2Score;
static ITEMSCORE_SET	s_setAdditionScore;
static ITEMSCORE_SET	s_setTypeScore;
static ITEMSCORE_SET	s_setReduceDmgScore;
static int				s_nEmoneyRate;

MYHEAP_IMPLEMENTATION(CItem,s_heap)
CItem::CItem()
{

	SetObjType(OBJ_ITEM);
}

CItem::~CItem()
{
}

bool CItem::CreateItemInfo(ItemInfoStruct& info, DWORD dwValue, CNpcType* pType, int nQuality )
{
	if (!pType)
		return false;

	DWORD dwItemQuality = 0;

	if (nQuality == 0)
	{
		int nRand = ::RandGet(100);
		if (nRand >= 0 && nRand < 30)
			dwItemQuality = 5;
		else if (nRand >= 30 && nRand <70)
			dwItemQuality = 4;
		else
			dwItemQuality = 3;
	}
	else
		dwItemQuality = nQuality;

	DWORD dwItemSort	= 0;
	DWORD dwItemColor	= 0;
	DWORD dwItemLev		= 0;

	int nRand = ::RandGet(1200);
	if (nRand >= 0 && nRand < 20)
	{
		dwItemSort	= 160;

		dwItemLev	= pType->GetInt(NPCTYPEDATA_DROP_SHOES);
	}
	else if (nRand >= 20 && nRand < 50)
	{
		DWORD dwSort[2] = {120, 121};
		dwItemSort = dwSort[::RandGet(2)];

		dwItemLev	= pType->GetInt(NPCTYPEDATA_DROP_NECKLACE);
	}
	else if (nRand >= 50 && nRand < 100)
	{
		DWORD dwSort[3] = {150, 151, 152};
		dwItemSort = dwSort[::RandGet(3)];

		dwItemLev	= pType->GetInt(NPCTYPEDATA_DROP_RING);
	}
	else if (nRand >= 100 && nRand < 400)
	{
		DWORD dwSort[5] = {111, 113, 114, 117, 118};
		dwItemSort = dwSort[::RandGet(5)];

		dwItemColor = ::RandGet(7)+3;

		dwItemLev	= pType->GetInt(NPCTYPEDATA_DROP_ARMET);
	}
	else if (nRand >= 400 && nRand < 700)
	{
		DWORD dwSort[5] = {130, 131, 132, 133, 134};
		dwItemSort	= dwSort[::RandGet(5)];
		dwItemColor = ::RandGet(7)+3;
		dwItemLev	= pType->GetInt(NPCTYPEDATA_DROP_ARMOR);
	}
	else
	{
		int nRate = ::RandGet(100);
		if (nRate >= 0 && nRate < 20)
		{
			dwItemSort = 421;
		}
		else if (nRate >= 20 && nRate < 40)
		{
			return false;
		}
		else if (nRate >= 40 && nRate < 80)
		{
			DWORD dwSort[11] = {410, 420, 421, 430, 440, 450, 460, 480, 481, 490, 500};
			dwItemSort = dwSort[::RandGet(11)];
		}
		else
		{
			DWORD dwSort[6] = {510, 530, 560, 561, 580, 900};
			dwItemSort = dwSort[::RandGet(6)];

			if (900 == dwItemSort)
				dwItemColor = ::RandGet(7)+3;
		}

		if (900 == dwItemSort)
		{
			dwItemColor = ::RandGet(7)+3;
			dwItemLev	= pType->GetInt(NPCTYPEDATA_DROP_SHIELD);
		}
		else
		{
			dwItemLev	= pType->GetInt(NPCTYPEDATA_DROP_WEAPON);
		}
	}

	if (dwItemLev == 99)
		return false;

	{
		int nRate = ::RandGet(100);
		if (nRate < 50)
		{
			DWORD dwLev = dwItemLev;
			dwItemLev = ::RandGet(dwLev/2)+dwLev/3;

			if (dwItemLev >= 1)
				dwItemLev--;
		}
		else if (nRate >= 80)
		{
			if ((dwItemSort >= 110 && dwItemSort <= 114) ||
					(dwItemSort >= 130 && dwItemSort <= 134) ||
						(dwItemSort >= 900 && dwItemSort <= 999))
			{
				dwItemLev = __min(dwItemLev+1, 9);
			}
			else
			{
				dwItemLev = __min(dwItemLev+1, 23);
			}
		}

	}

	OBJID idItemType = dwItemSort*1000+dwItemColor*100+dwItemLev*10+dwItemQuality;

	CItemTypeData* pItemType = ItemType()->QueryItemType(idItemType);
	if (!pItemType)
		return false;

	memset(&info, 0L, sizeof(info));

	info.id			= ID_NONE;
	info.idOwner	= ID_NONE;
	info.idPlayer	= ID_NONE;
	info.idType		= idItemType;

	info.nAmountLimit	= pItemType->GetInt(ITEMTYPEDATA_AMOUNT_LIMIT)*::RandomRateGet(0.3);
	if (info.nAmountLimit < 1)
		info.nAmountLimit = 1;

	if (dwItemQuality > 5)
		info.nAmount = info.nAmountLimit*(50+::RandGet(50))/100;
	else
	{
		DWORD dwPrice = pItemType->GetInt(ITEMTYPEDATA_PRICE);
		if (dwPrice <= 0)
			dwPrice = 1;

		info.nAmount = 3*info.nAmountLimit*dwValue/dwPrice;
		if (info.nAmount >= info.nAmountLimit)
			info.nAmount = info.nAmountLimit;

		if (info.nAmount < 1)
		{
			info.nAmount = 1;
		}
	}

	if (idItemType >= 400000 && idItemType < 599999)
	{
		int nRate = ::RandGet(100);
		if (nRate < 5)
		{
			info.nGem1 = 0xff;
			info.nGem2 = 0xff;
		}
		else if (nRate < 20)
		{
			info.nGem1 = 0xff;
		}

	}

	if (::RandGet(300) == 123)
		info.nMagic3	= 1;
	if (info.nGem1 != GEM_HOLE && info.nGem2 != GEM_HOLE)
	{
		int nChance = 300;
		if (CItem::IsType44Weapon(idItemType))
			nChance = 60;
		if (::RandGet(nChance) == 13)
		{
			if (::RandGet(100) < 70)
				info.nReduceDmg	= 3;
			else
				info.nReduceDmg	= 5;
		}
	}
	info.usPlunder		= 0;
	info.usSpecialFlag	= 0;
	info.nColor			= (USHORT)dwItemColor;

	return true;
}

int	CItem::CalcRepairMoney()
{
	int nQualityRate = 10;
	switch(GetInt(ITEMDATA_TYPE) % 10)
	{
	case 6:		nQualityRate = 11;		break;
	case 7:		nQualityRate = 12;		break;
	case 8:		nQualityRate = 13;		break;
	case 9:		nQualityRate = 15;		break;
	}

	int nRecoverDurability = __max(0, GetInt(ITEMDATA_AMOUNTLIMIT) - GetInt(ITEMDATA_AMOUNT));

	int nRepairCost	= ::MulDiv(GetInt(ITEMDATA_PRICE), nRecoverDurability*3, GetInt(ITEMDATA_AMOUNTLIMIT)*4);
	return ::MulDiv(nRepairCost, nQualityRate, 10);
}

DWORD CItem::GetSellPrice()
{
	if(GetInt(ITEMDATA_AMOUNTLIMIT) == 0)
		return 0;

	CHECKF(GetInt(ITEMDATA_AMOUNTLIMIT) > 0);

	int nAmount = this->GetInt(ITEMDATA_AMOUNT);
	if (nAmount > this->GetInt(ITEMDATA_AMOUNTLIMIT))
		nAmount = this->GetInt(ITEMDATA_AMOUNTLIMIT);
	int nPrice = GetInt(ITEMDATA_PRICE);
	return ::MulDiv(nPrice/3, nAmount, GetInt(ITEMDATA_AMOUNTLIMIT));
}

CItem*	CItem::Split(int nNum)
{
	CHECKF(IsPileEnable());

	CItemPtr pItem = Clone();
	CHECKF(pItem);

	int nCurrNum = GetInt(ITEMDATA_AMOUNT);
	if (nCurrNum <= nNum)
		return NULL;

	pItem->SetInt(ITEMDATA_AMOUNT, nNum);
	this->SetInt(ITEMDATA_AMOUNT, nCurrNum - nNum);

	return pItem;
}

CItem*	CItem::Clone()
{
	CItemPtr pItem = CreateNew();
	IF_NOT(pItem)
		return NULL;

	ItemInfoStruct	info;
	GetInfo(&info);
	info.id		= ID_NONE;
	if(pItem->Create(m_pData->GetRecord(), &info, true))
		return pItem;

	pItem->ReleaseByOwner();
	return NULL;
}

int	CItem::GetItemType(OBJID idType)
{
	int nSort = ((int)idType % 10000000) / 100000;
	if (nSort >= 4 && nSort <= 5)
		return  (((int)idType%100000)/1000)*1000;
	if (nSort == 7 || nSort == 10)
		return  (((int)idType%100000)/10000)*10000;
	return idType;
}
int	CItem::GetItemSubType()
{
	return (GetInt(ITEMDATA_TYPE)%1000000)/1000;
}

int	CItem::GetLevel(void)
{
	if (this->IsShield() || this->IsArmor() || this->IsHelmet())
		return (GetInt(ITEMDATA_TYPE)%100)/10;
	else
		return (GetInt(ITEMDATA_TYPE)%1000)/10;
}

bool CItem::ChangeType	(OBJID idNewType)
{
	CItemTypeData *pType = ItemType()->QueryItemType(idNewType);
	if(!pType)
	{
		LOGERROR("ChangeType: bad type [%d]!", idNewType);

		return false;
	}

	m_pType = pType;
	this->SetInt(ITEMDATA_TYPE, idNewType);

	m_pAddition = ItemAddition()->QueryItemAddition(idNewType, GetInt(ITEMDATA_ADDITION));

	return true;
}

int	CItem::GetGemDmgEffect()
{
	if (GetInt(ITEMDATA_AMOUNT) == 0)
		return 0;

	int nEffect = 0;
	int nGem[2] = { this->GetInt(ITEMDATA_GEM1), this->GetInt(ITEMDATA_GEM2) };

	for (int i=0; i<2; i++)
	{
		if (nGem[i] != GEM_NONE && nGem[i] != GEM_HOLE)
		{
			OBJID idGemType = 700000 + nGem[i];
			switch(idGemType)
			{
			case GEM_DMG_LOW:
				nEffect += 5;
				break;

			case GEM_DMG_MID:
				nEffect += 10;
				break;

			case GEM_DMG_HGT:
				nEffect += 15;
				break;
			}
		}
	}

	return nEffect;
}

int	CItem::GetGemEffect700071()
{
	if (GetInt(ITEMDATA_AMOUNT) == 0)
		return 0;

	int nEffect = 0;
	int nGem[2] = { this->GetInt(ITEMDATA_GEM1), this->GetInt(ITEMDATA_GEM2) };

	for (int i=0; i<2; i++)
	{
		if (nGem[i] != GEM_NONE && nGem[i] != GEM_HOLE)
		{
			OBJID idGemType = 700000 + nGem[i];
			switch(idGemType)
			{
			case GEM_700071_LOW:
				nEffect += 4;
				break;

			case GEM_700071_MID:
				nEffect += 8;
				break;

			case GEM_700071_HGT:
				nEffect += 12;
				break;
			}
		}
	}

	return nEffect;
}

int	CItem::GetGemMgcAtkEffect()
{
	if (GetInt(ITEMDATA_AMOUNT) == 0)
		return 0;

	int nEffect = 0;
	int nGem[2] = { this->GetInt(ITEMDATA_GEM1), this->GetInt(ITEMDATA_GEM2) };

	for (int i=0; i<2; i++)
	{
		if (nGem[i] != GEM_NONE && nGem[i] != GEM_HOLE)
		{
			OBJID idGemType = 700000 + nGem[i];
			switch(idGemType)
			{
			case GEM_MATK_LOW:
				nEffect += 5;
				break;

			case GEM_MATK_MID:
				nEffect += 10;
				break;

			case GEM_MATK_HGT:
				nEffect += 15;
				break;
			}
		}
	}

	return nEffect;
}

int	CItem::GetGemHitRateEffect()
{
	if (GetInt(ITEMDATA_AMOUNT) == 0)
		return 0;

	int nEffect = 0;
	int nGem[2] = { this->GetInt(ITEMDATA_GEM1), this->GetInt(ITEMDATA_GEM2) };

	for (int i=0; i<2; i++)
	{
		if (nGem[i] != GEM_NONE && nGem[i] != GEM_HOLE)
		{
			OBJID idGemType = 700000 + nGem[i];
			switch(idGemType)
			{
			case GEM_HIT_LOW:
				nEffect += 5;
				break;

			case GEM_HIT_MID:
				nEffect += 10;
				break;

			case GEM_HIT_HGT:
				nEffect += 15;
				break;
			}
		}
	}

	return nEffect;
}

int	CItem::GetGemExpEffect()
{
	if (GetInt(ITEMDATA_AMOUNT) == 0)
		return 0;

	int nEffect = 0;
	int nGem[2] = { this->GetInt(ITEMDATA_GEM1), this->GetInt(ITEMDATA_GEM2) };

	for (int i=0; i<2; i++)
	{
		if (nGem[i] != GEM_NONE && nGem[i] != GEM_HOLE)
		{
			OBJID idGemType = 700000 + nGem[i];
			switch(idGemType)
			{
			case GEM_EXP_LOW:
				nEffect += 10;
				break;

			case GEM_EXP_MID:
				nEffect += 15;
				break;

			case GEM_EXP_HGT:
				nEffect += 25;
				break;
			}
		}
	}

	return nEffect;
}

int	CItem::GetGemWpnExpEffect()
{
	if (GetInt(ITEMDATA_AMOUNT) == 0)
		return 0;

	int nEffect = 0;
	int nGem[2] = { this->GetInt(ITEMDATA_GEM1), this->GetInt(ITEMDATA_GEM2) };

	for (int i=0; i<2; i++)
	{
		if (nGem[i] != GEM_NONE && nGem[i] != GEM_HOLE)
		{
			OBJID idGemType = 700000 + nGem[i];
			switch(idGemType)
			{
			case GEM_WPNEXP_LOW:
				nEffect += 30;
				break;

			case GEM_WPNEXP_MID:
				nEffect += 50;
				break;

			case GEM_WPNEXP_HGT:
				nEffect += 100;
				break;
			}
		}
	}

	return nEffect;
}

int	CItem::GetGemMgcExpEffect()
{
	if (GetInt(ITEMDATA_AMOUNT) == 0)
		return 0;

	int nEffect = 0;
	int nGem[2] = { this->GetInt(ITEMDATA_GEM1), this->GetInt(ITEMDATA_GEM2) };

	for (int i=0; i<2; i++)
	{
		if (nGem[i] != GEM_NONE && nGem[i] != GEM_HOLE)
		{
			OBJID idGemType = 700000 + nGem[i];
			switch(idGemType)
			{
			case GEM_MGCEXP_LOW:
				nEffect += 15;
				break;

			case GEM_MGCEXP_MID:
				nEffect += 30;
				break;

			case GEM_MGCEXP_HGT:
				nEffect += 50;
				break;
			}
		}
	}

	return nEffect;
}

int CItem::GetGemDurEffect(OBJID idGemType)
{
	if (GetInt(ITEMDATA_AMOUNT) == 0)
		return 0;

	int nEffect = 0;

	if (ID_NONE == idGemType)
	{
		int nGem[2] = { this->GetInt(ITEMDATA_GEM1), this->GetInt(ITEMDATA_GEM2) };

		for (int i=0; i<2; i++)
		{
			if (nGem[i] != GEM_NONE && nGem[i] != GEM_HOLE)
			{
				OBJID idGemType = 700000 + nGem[i];
				switch(idGemType)
				{
				case GEM_DUR_LOW:
					nEffect += 50;
					break;

				case GEM_DUR_MID:
					nEffect += 100;
					break;

				case GEM_DUR_HGT:
					nEffect += 200;
					break;
				}
			}
		}
	}
	else
	{
		switch(idGemType)
		{
		case GEM_DUR_LOW:
			nEffect += 50;
			break;

		case GEM_DUR_MID:
			nEffect += 100;
			break;

		case GEM_DUR_HGT:
			nEffect += 200;
			break;
		}
	}

	return nEffect;
}

bool CItem::IsNonsuchItem(void)
{
	DWORD dwType = this->GetInt(ITEMDATA_TYPE);
	if ((dwType >= 720027 && dwType <= 720028) || (dwType >= TYPE_DRAGONBALL && dwType <= TYPE_DIVOICEITEM) || IsTreasurePack())
		return true;

	if (GetInt(ITEMDATA_TYPE_MASK) & 1)
		return true;

	if (GetInt(ITEMDATA_REDUCE_DMG) >= 3 || GetInt(ITEMDATA_ADD_LIFE) >= 128 || GetInt(ITEMDATA_ANTI_MONSTER) != 0)
		return true;

	if (this->IsGem() && (dwType%10) >= 2)
		return true;

	if (this->IsGameCard())
		return true;

	if (!(dwType <= 6000000 || (dwType >= 9000000 && dwType < 10000000)))
		return false;

	if (this->IsEquipment() && (int)dwType % 10 >= 8)
		return true;
	if (GetInt(ITEMDATA_ADDITION) >= 6)
		return true;

	int nGem1 = this->GetInt(ITEMDATA_GEM1)%10;
	int nGem2 = this->GetInt(ITEMDATA_GEM2)%10;

	bool bIsNonsuch = false;
	int nSort = CItem::GetItemSort(dwType);
	if(nSort >= ITEMSORT_WEAPON1 && nSort <= ITEMSORT_WEAPON2)
	{
		if ((nGem1 != GEM_HOLE && nGem1 >= 2)
				|| (nGem2 != GEM_HOLE && nGem2 >= 2))
			bIsNonsuch = true;
	}
	else if((nSort >= ITEMSORT_HELMET && nSort <= ITEMSORT_ARMOR) || (nSort >= ITEMSORT_SHIELD && nSort <= ITEMSORT_SHOES))
	{
		if (nGem1 != GEM_NONE || nGem2 != GEM_NONE)
			bIsNonsuch = true;
	}

	return bIsNonsuch;
}

bool CItem::ChangeColor(int nColor)
{
	int nSort = CItem::GetItemSort(GetInt(ITEMDATA_TYPE));
	if (!((nSort == ITEMSORT_ARMOR ? nColor >= 2 : nColor >= 3) && nColor <= 9))
		return false;

	if(nSort == ITEMSORT_HELMET || nSort == ITEMSORT_ARMOR || nSort == ITEMSORT_SHIELD)
	{
		DWORD idType = this->GetInt(ITEMDATA_TYPE);
		DWORD idNewType = idType-((idType/100)%10)*100+nColor*100;
		if (!this->ChangeType(idNewType))
			return false;

		this->SetColor(nColor);
		return true;
	}

	return false;
}

bool CItem::RecoverDur	(void)
{
	int nMaxDur = this->GetInt(ITEMDATA_AMOUNTLIMIT_ORIGINAL);
	nMaxDur += nMaxDur*this->GetGemDurEffect()/100;

	this->SetInt(ITEMDATA_AMOUNTLIMIT, nMaxDur, true);
	return true;
}

int CItem::GetRecoverDurCost	(void)
{
	int nRealMaxDur = this->GetInt(ITEMDATA_AMOUNTLIMIT)*100/(100+this->GetGemDurEffect());
	if (nRealMaxDur >= this->GetInt(ITEMDATA_AMOUNTLIMIT_ORIGINAL))
		return 0;

	int nRepairDur = this->GetInt(ITEMDATA_AMOUNTLIMIT_ORIGINAL)-nRealMaxDur;
	int nCost = 0;
	switch(this->GetQuality())
	{
	case 9:
		nCost = ::MulDiv(5000000, nRepairDur, this->GetInt(ITEMDATA_AMOUNTLIMIT_ORIGINAL));
		nCost = __max(nCost, 500000);
		break;

	case 8:
		nCost = ::MulDiv(3500000, nRepairDur, this->GetInt(ITEMDATA_AMOUNTLIMIT_ORIGINAL));
		nCost = __max(nCost, 350000);
		break;

	default:
		nCost = ::MulDiv(1500000, nRepairDur, this->GetInt(ITEMDATA_AMOUNTLIMIT_ORIGINAL));
		nCost = __max(nCost, 150000);
		break;
	}

	return __max(1, nCost);
}

bool CItem::TestSpecialFlag(DWORD dwMask)
{
	return ((DWORD)GetInt(ITEMDATA_SPECIALFLAG) & dwMask) == dwMask;
}

void CItem::SetSpecialFlag(DWORD dwFlag)
{
	SetInt(ITEMDATA_SPECIALFLAG, dwFlag, true);
}

void CItem::SetColor(int nColor)
{
	SetInt(ITEMDATA_COLOR, nColor, true);
}

int CItem::GetScore()
{
	int nGem1		= GetInt(ITEMDATA_GEM1);
	int nGem2		= GetInt(ITEMDATA_GEM2);
	int nAddition	= GetInt(ITEMDATA_ADDITION);
	int nType		= GetInt(ITEMDATA_TYPE);
	int nReduceDmg	= GetInt(ITEMDATA_REDUCE_DMG);

	int nScore = 0;
	if (s_setTypeScore.find(nType) != s_setTypeScore.end())
		nScore = s_setTypeScore[nType];
	if (!IsEquipEnable())
		return nScore;

	if (s_setGem1Score.find(nGem1) != s_setGem1Score.end())
		nScore += s_setGem1Score[nGem1];
	if (s_setGem2Score.find(nGem2) != s_setGem2Score.end())
		nScore += s_setGem2Score[nGem2];
	if (s_setAdditionScore.find(nAddition) != s_setAdditionScore.end())
		nScore += s_setAdditionScore[nAddition];
	if (s_setReduceDmgScore.find(nReduceDmg) != s_setReduceDmgScore.end())
		nScore += s_setReduceDmgScore[nReduceDmg];
	return nScore;
}

int CItem::GetItemSort(OBJID idType)
{
	int nSort = ITEMSORT_INVALID;
	switch(((int)idType % 10000000) / 100000)
	{
	case 1:
		switch(((int)idType % 1000000) / 10000)
		{
		case 11:	nSort = ITEMSORT_HELMET;		break;
		case 12:	nSort = ITEMSORT_NECKLACE;		break;
		case 13:	nSort = ITEMSORT_ARMOR;			break;
		case 14:	nSort = ITEMSORT_HELMET;		break;
		case 15:	nSort = ITEMSORT_RING;			break;
		case 16:	nSort = ITEMSORT_SHOES;			break;
		case 18:
		case 19:	nSort = ITEMSORT_MANTLE;		break;
		}
		break;
	case 4:		nSort = ITEMSORT_WEAPON1;		break;
	case 5:		nSort = ITEMSORT_WEAPON2;		break;
	case 7:		nSort = ITEMSORT_OTHER;			break;
	case 9:		nSort = ITEMSORT_SHIELD;		break;
	case 10:	nSort = ITEMSORT_EXPEND;		break;
	default:
		{
			int nCode = ((int)idType % 10000000) / 100000;
			if (nCode > 19 && nCode <= 29)
				nSort = ITEMSORT_EXTEND;
		}
		break;
	}
	return nSort;
}

int CItem::GetAddition(ITEMADDITIONDATA idx)
{
	if (!m_pAddition)
		return 0;

	return m_pAddition->GetInt(idx);
}

int CItem::GetSubType()
{
	return GetItemSubType();
}

bool CItem::IsSellEnable()
{
	return !IsGameCard() && !IsPlunder() && !TestSpecialFlag(2) && !TestSpecialFlag(1);
}

bool CItem::IsPreciousItem()
{
	return GetScore() >= 1380;
}

bool CItem::IsHighValue()
{
	return GetScore() >= 500;
}

bool CItem::IsNeedRepair()
{
	if (!IsRepairEnable())
		return false;
	return GetInt(ITEMDATA_AMOUNT)/100 < GetInt(ITEMDATA_AMOUNTLIMIT)/100;
}

bool CItem::IsHeadOrBodyLev10()
{
	if (!IsEquipment())
		return false;

	bool bOK = false;
	int nSubType = GetItemSubType();
	if (nSubType == 111 || nSubType == 113 || nSubType == 114 || nSubType == 118
			|| nSubType == 130 || nSubType == 131 || nSubType == 133 || nSubType == 134)
	{
		if (GetLevel() >= 10)
			bOK = true;
	}
	return bOK;
}

bool CItem::IsPlunder()
{
	return GetInt(ITEMDATA_PLUNDER) != 0;
}

int CItem::GetEmoneyRate()
{
	return s_nEmoneyRate;
}

ITEMSCORE_SET* CItem::QueryGem1ScoreSet()			{ return &s_setGem1Score; }
ITEMSCORE_SET* CItem::QueryGem2ScoreSet()			{ return &s_setGem2Score; }
ITEMSCORE_SET* CItem::QueryAdditionScoreSet()		{ return &s_setAdditionScore; }
ITEMSCORE_SET* CItem::QueryTypeScoreSet()			{ return &s_setTypeScore; }
ITEMSCORE_SET* CItem::QueryReduceDmgScoreSet()		{ return &s_setReduceDmgScore; }

void CItem::SetEmoneyRate(int nRate)
{
	s_nEmoneyRate = nRate;
}

bool CItem::IsChkSumOK()
{
	return (DWORD)GetInt(ITEMDATA_CHKSUM) == CalcChkSum();
}

bool CItem::IsEquipEnable()
{
	int nSort = GetItemSort();
	if (nSort >= ITEMSORT_HELMET && nSort <= ITEMSORT_SHOES)
		return true;
	return IsArrowSort() || GetItemSort() == ITEMSORT_EXTEND || GetItemSort() == ITEMSORT_MANTLE;
}

int CItem::GetWeight(OBJID idType, int nAmount )
{
	CItemTypeData* pType = ItemType()->QueryItemType(idType);
	IF_NOT(pType)
		return ERROR_WEIGHT;

	if(!nAmount)
		nAmount	= pType->GetInt(ITEMTYPEDATA_AMOUNT);

	if(IsArrowSort(idType))
		return (pType->GetInt(ITEMTYPEDATA_AMOUNT)-1)*pType->GetInt(ITEMTYPEDATA_WEIGHT)/ARROW_PILE_WEIGHT + 1;

	if(CItem::IsExpend(idType))
		return pType->GetInt(ITEMTYPEDATA_WEIGHT) * nAmount;
	else
		return pType->GetInt(ITEMTYPEDATA_WEIGHT);
}
