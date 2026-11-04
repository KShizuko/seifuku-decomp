#include "ItemAddition.h"
#include "i_mydb.h"
#include "Item.h"

MYHEAP_IMPLEMENTATION(CItemAddition,s_heap)
char szItemAdditionTable[] = _TBL_ITEMADDITION;

CItemAddition::CItemAddition()
{
	m_setAddition = NULL;
}

CItemAddition::~CItemAddition()
{
	if (m_setAddition)
		m_setAddition->Release();
}

bool CItemAddition::Create(IDatabase* pDb)
{
	m_setAddition	= CItemAdditionSet64::CreateNew(true);
	CHECKF(m_setAddition);

	bool ret = m_setAddition->Create("SELECT * FROM " _TBL_ITEMADDITION, pDb);
	IF_NOT_(ret)
		return false;

	return true;
}

CItemAdditionData* CItemAddition::QueryItemAddition(OBJID idType, int nLevel)
{
	OBJID idKey = idType;

	if (CItem::IsMSword(idType) || CItem::IsBow(idType))
	{
	}
	else if (CItem::IsWeapon1(idType))
	{
		idKey = (idType/100000)*100000 + idType % 1000 + 44000;
	}
	else if (CItem::IsWeapon2(idType))
	{
		idKey = (idType/100000)*100000 + idType % 1000 + 55000;
	}
	else if (CItem::GetItemSort(idType) == ITEMSORT_HELMET || CItem::GetItemSort(idType) == ITEMSORT_ARMOR || CItem::IsShield(idType))
	{
		idKey = (idType/1000)*1000 + idType % 100;
	}

	_int64	key = (idKey / 10) * 10;
	key = (key << 32) + nLevel;
	return m_setAddition->GetObj(key);
}
