#include "winsock2.h"
#include "ItemType.h"
#include "I_mydb.h"
#include "ItemData.h"

MYHEAP_IMPLEMENTATION(CItemType,s_heap)
char	szItemTypeTable[] = _TBL_ITEMTYPE;
CItemType::CItemType()
{
	m_setType	= NULL;
}

CItemType::~CItemType()
{
	if(m_setType)
		m_setType->Release();
}

bool CItemType::Create(IDatabase* pDb)
{
	m_setType	= CItemTypeSet::CreateNew(true);
	CHECKF(m_setType);
	bool ret = m_setType->Create("SELECT * FROM " _TBL_ITEMTYPE, pDb);
	IF_NOT_(ret)
		return false;

	return true;
}

CItemTypeData* CItemType::QueryItemType(OBJID idType)
{
	return m_setType->GetObj(idType);
}
