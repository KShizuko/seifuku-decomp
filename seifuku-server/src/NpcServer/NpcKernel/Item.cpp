#include "windows.h"
#include "define.h"
#include "Item.h"
#include "ItemType.h"
#include "NpcType.h"
#include "NpcWorld.h"
#include "i_mydb.h"

MYHEAP_IMPLEMENTATION(CItem,s_heap)
CItem::CItem()
{
	SetObjType(OBJ_ITEM);
	m_pType	= NULL;
}

CItem::~CItem()
{
}

bool CItem::Create(ItemInfoStruct* pInfo, int nPrice )
{
	memcpy(&m_info, pInfo, sizeof(ItemInfoStruct));
	return true;
}

int	CItem::GetItemSort()
{
	int nType = ITEMSORT_INVALID;
	switch((GetInt(ITEMDATA_TYPE)%10000000)/100000)
	{
	case 10:
		nType = ITEMSORT_EXPEND;
		break;

	case 1:
		{
			switch((GetInt(ITEMDATA_TYPE)%1000000)/10000)
			{
			case 11:
				nType = ITEMSORT_HELMET;
				break;
			case 12:
				nType = ITEMSORT_NECKLACE;
				break;
			case 13:
				nType = ITEMSORT_ARMOR;
				break;
			case 15:
				nType = ITEMSORT_RING;
				break;
			case 16:
				nType = ITEMSORT_SHOES;
				break;
			}
		}
		break;

	case 4:
		nType = ITEMSORT_WEAPON1;
		break;

	case 5:
		nType = ITEMSORT_WEAPON2;
		break;

	case 7:
		nType = ITEMSORT_OTHER;
		break;

	case 9:
		nType = ITEMSORT_SHIELD;
		break;
	}

	return nType;
}

int	CItem::GetItemType()
{
	int nType = ITEMTYPE_INVALID;
	switch((GetInt(ITEMDATA_TYPE)%10000000)/100000)
	{
	case 7:
	case 10:
		nType = ((GetInt(ITEMDATA_TYPE)%100000)/10000)*10000;
		break;

	case 4:
	case 5:
		nType = ((GetInt(ITEMDATA_TYPE)%100000)/1000)*1000;
		break;

	default:
		break;
	}

	return nType;
}

bool CItem::UpdateInfo(ItemInfoStruct *pInfo)
{
	CHECKF(pInfo && (*pInfo)[ITEMDATA_ID_] == GetInt(ITEMDATA_ID_) && (*pInfo)[ITEMDATA_POSITION] == GetInt(ITEMDATA_POSITION));

	memcpy(&m_info, pInfo, sizeof(ItemInfoStruct));
	return true;
}

int CItem::GetTypeInt(ITEMDATA idx)
{
	if(!m_pType)
		m_pType = NpcWorld()->GetItemType()->QueryItemType(m_info[ITEMDATA_TYPE]);
	CHECKF(m_pType);

	return m_pType->GetInt((ITEMTYPEDATA)(idx - ITEMTYPEDATA_OFFSET));
}
