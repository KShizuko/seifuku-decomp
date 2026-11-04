#include "BaseFunc.h"
#include "Item.h"
#include "3DRoleData.h"
#include "3DGameMap.h"
#include "Hero.h"

CItemData CItem::s_ItemData;

CItem::CItem()
{
	m_nMaxAttackModify		=0;
	m_nMinAttackModify		=0;
	m_nDefenceModify		=0;
	m_nDexterityModify		=0;
	m_nDodgeModify			=0;
	m_nMagicAttackModify	=0;
	m_nMagicDefenceModify	=0;
	memset(&m_infoAddition, 0, sizeof(ItemAdditionInfo));
	m_dwBless				=0;
	m_dwEnchant				=0;
	m_dwRestrain			=0;
	m_dwSellPrice			=0;
	m_dwSellEmoneyPrice		=0;
}
CItem::~CItem()
{
}

BOOL CItem::Create(OBJID idItem, OBJID idItemType)
{
	if(idItem == ID_NONE || idItemType == ID_NONE)
		return false;

	if(!s_ItemData.GetItemTypeInfo(idItemType, m_infoType))
		return false;
	ItemTypeInfo infoNormal;
	if(s_ItemData.GetItemTypeInfo(idItemType/10*10 + 5, infoNormal))
	{
		m_nMaxAttackModify		= m_infoType.usMaxAttack - infoNormal.usMaxAttack;
		m_nMinAttackModify		= m_infoType.usMinAttack - infoNormal.usMinAttack;
		m_nDefenceModify		= m_infoType.usDefense - infoNormal.usDefense;
		m_nDexterityModify		= m_infoType.usDexterity - infoNormal.usDexterity;
		m_nDodgeModify			= m_infoType.usDodge - infoNormal.usDodge;
		m_nMagicAttackModify	= m_infoType.usMagicAttack - infoNormal.usMagicAttack;
		m_nMagicDefenceModify	= m_infoType.usDefense - infoNormal.usDefense;
	}
	else
	{
		m_nMagicDefenceModify	= 0;
		m_nMagicAttackModify	= 0;
		m_nDexterityModify		= 0;
		m_nDefenceModify		= 0;
		m_nMinAttackModify		= 0;
		m_nMaxAttackModify		= 0;
	}
	m_idItem = 	idItem;
	if(idItemType/10000 == 105)
	{
		m_nMagicDefenceModify	= 0;
		m_nMagicAttackModify	= 0;
		m_nDexterityModify		= 0;
		m_nDefenceModify		= 0;
		m_nMinAttackModify		= 0;
		m_nMaxAttackModify		= 0;
	}
	m_dwBless				=0;
	m_dwEnchant				=0;
	m_dwRestrain			=0;
	return true;
}
void CItem::UpdateAdditionInfo()
{
	OBJID idType = this->GetTypeID();
	if(this->IsBacksword() || this->IsBow())
	{
		idType = idType/10*10;
		s_ItemData.GetItemAdditionInfo(idType, this->GetAddition(), m_infoAddition);
		return;
	}
	if(this->IsSingleHandWeapon())
		idType = (idType/100000)*100000 + idType%1000 + 44000;
	else if(this->IsDoubleHandWeapon())
		idType = (idType/100000)*100000 + idType%1000 + 55000;
	else if(this->IsHelmet() || this->IsArmor() || this->IsShield())
		idType = (idType/1000)*1000 + idType%100;
	idType = idType/10*10;
	s_ItemData.GetItemAdditionInfo(idType, this->GetAddition(), m_infoAddition);
}
int	CItem::GetSort()
{
	return CItem::ItemGetSort(m_infoType.uID);
}
int	CItem::GetSubType()
{
	return CItem::ItemGetSubType(m_infoType.uID);
}
DWORD CItem::GetSellPrice()
{
	if(this->GetAmountLimit() == 0)
		return 0;
	return (this->GetAmount() * this->GetPrice() / this->GetAmountLimit()) / 3;
}

DWORD CItem::GetRepairCost()
{
	int nFactor = 10;
	switch(m_infoType.uID % 10)
	{
	case 6:
		nFactor = 11;
		break;
	case 7:
		nFactor = 12;
		break;
	case 8:
		nFactor = 13;
		break;
	case 9:
		nFactor = 15;
		break;
	}
	if(this->GetAmountLimit() == 0)
		return 0;
	int nRecoverDurability = __max(0,  this->GetAmountLimit() - this->GetAmount());
	int nRepairCost	= this->GetPrice() * nRecoverDurability * 3 / (this->GetAmountLimit() * 4);
	return nRepairCost * nFactor / 10;
}

DWORD CItem::GetWeaponSkillType ()
{
	if (ITEMSORT_WEAPON_SINGLE_HAND != this->GetSort() 
			&& ITEMSORT_WEAPON_DOUBLE_HAND != this->GetSort()
			&& ITEMSORT_SHIELD != this->GetSort())
		return 0;

	return (this->GetTypeID()%1000000)/1000;
}

int CItem::ItemGetSort (OBJID idType)
{
	int nType = ITEMSORT_INVALID;
	switch((idType%10000000)/100000)
	{
	case 10:
		nType = ITEMSORT_EXPEND;
		break;

	case 1:
		{
			switch((idType%1000000)/10000)
			{
			case 11:
				nType = ITEMSORT_HELMET;
				break;
			case 12:
				nType = ITEMSORT_NECKLACE;
				break;
			case 13:
			case 14:
				nType = ITEMSORT_ARMOR;
				break;
			case 15:
				nType = ITEMSORT_RING;
				break;
			case 16:
				nType = ITEMSORT_SHOES;
				break;
			case 17:
				nType = ITEMSORT_HELMET;
				break;
			case 18:
			case 19:
				nType = ITEMSORT_SPRITE;
				break;
			}
		}
		break;

	case 4:
		nType = ITEMSORT_WEAPON_SINGLE_HAND;
		break;

	case 5:
		nType = ITEMSORT_WEAPON_DOUBLE_HAND;
		break;
	case 7:
		nType = ITEMSORT_OTHER;
		break;

	case 9:
		nType = ITEMSORT_SHIELD;
		break;
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
		nType = ITEMSORT_MOUNT;
		break;
	}

	return nType;
}

int CItem::ItemGetSubType(OBJID idType)
{
	int nType = EXPEND_INVALID;
	switch((idType%10000000)/100000)
	{
	case 7:
	case 10:
		nType = ((idType%100000)/10000)*10000;
		break;

	case 4:
	case 5:
		nType = ((idType%100000)/1000)*1000;
		break;
	}

	return nType;
}

unsigned short CItem::GetMagicDefence()
{
	return m_infoType.usMagicDefence;
}
char* CItem::GetDesc()
{
	return m_infoType.szDesc;
}
void	CItem::SetBoothSellPrice(DWORD dwPrice)
{
	m_dwSellPrice = dwPrice;
}
DWORD	CItem::GetBoothSellPrice()
{
	return m_dwSellPrice;
}
void	CItem::SetBoothSellEmoneyPrice(DWORD dwPrice)
{
	m_dwSellEmoneyPrice = dwPrice;
}
void	CItem::SetOwnerID(OBJID idOwner)
{
	m_idOwner = idOwner;
}
BOOL	CItem::IsNonsuchItem(OBJID idItemType)
{
	if ((idItemType / 100000 ) < 10 && !( idItemType >= 700000 && idItemType <= 800000) && idItemType%10 == 9)
		return true;
	ItemTypeInfo info;
	if (s_ItemData.GetItemTypeInfo(idItemType, info))
		return (info.usTypeMask & 1) != 0;
	return false;
}
DWORD CItem::GetFuseInfo()
{
	DWORD dwKey;
	if(this->GetSort() >= ITEMSORT_HELMET && this->GetSort() <= ITEMSORT_SHOES)
	{
		dwKey = (m_infoType.uID/1000 + 10000000)*100;
		DWORD dwQuality;
		if(this->GetSort() == ITEMSORT_WEAPON_SINGLE_HAND || this->GetSort() == ITEMSORT_WEAPON_DOUBLE_HAND)
			dwQuality = m_infoType.uID % 1000;
		else
			dwQuality = m_infoType.uID % 100;
		dwKey += (dwQuality/10/6)*10 + m_infoType.uID % 10;
	}
	else
		dwKey = (m_infoType.uID + 20000000)*100;
	return s_ItemData.GetFuseInfo(dwKey);
}
BOOL CItem::IsAdditionStone()
{
	return (m_infoType.uID / 10000) % 100 == 73;
}
int CItem::ItemGetBaseClass(OBJID idType)
{
	int nClass = (idType / 1000) % 1000;
	switch (nClass)
	{
	case 112:
		switch ((idType / 10) % 10)
		{
		case 8:	return 118;
		case 1:	return 111;
		case 3:	return 113;
		case 4:	return 114;
		}
		return nClass;
	case 130:
	case 135:
	case 140:
		return 130;
	case 131:
	case 136:
	case 141:
		return 131;
	case 133:
	case 138:
	case 143:
		return 133;
	case 134:
	case 139:
	case 144:
		return 134;
	case 111:
	case 171:
		return 111;
	case 113:
	case 173:
		return 113;
	case 114:
	case 174:
		return 114;
	case 117:
	case 177:
		return 117;
	case 118:
	case 178:
		return 118;
	}
	return nClass;
}
