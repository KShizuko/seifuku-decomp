#include "windows.h"
#include "define.h"
#include "FuseItem.h"
#include "I_mydb.h"
#include "Item.h"

CFuseItem*	g_pFuseItem	= NULL;
CFuseItem::CFuseItem()
{
}

CFuseItem::~CFuseItem()
{
}
bool CFuseItem::Create(IDatabase* pDb)
{
	IF_NOT(pDb)
		return false;

	IRecordset* pRes = pDb->CreateNewRecordset("SELECT * FROM cq_fuse", true);
	IF_NOT(pRes)
		return false;

	for(int i = 0; i < (int)pRes->RecordCount(); i++)
	{
		int	nKey = pRes->GetInt(FUSEDATA_SORT) * FUSEKEY_SORT_BASE;
		if(pRes->GetInt(FUSEDATA_SORT) == FUSESORT_EQUIP)
		{
			nKey	+= pRes->GetInt(FUSEDATA_TYPE) * 1000;
			nKey	= nKey + pRes->GetInt(FUSEDATA_LEV) * 10;
			nKey	+= pRes->GetInt(FUSEDATA_QUALITY);
		}
		else
			nKey	+= pRes->GetInt(FUSEDATA_TYPE);
		int	nChance = pRes->GetInt(FUSEDATA_CHANCE);
		m_setFuse[nKey]	= nChance;

		pRes->MoveNext();
	}
	pRes->Release();
	return true;
}

int CFuseItem::GetItemScore(CItem* pItem)
{
	IF_NOT(pItem)
		return 0;

	int	nScore	= 0;
	int	nKey	= 2*FUSEKEY_SORT_BASE + pItem->GetInt(ITEMDATA_TYPE);
	FUSE_SET::iterator iter = m_setFuse.find(nKey);
	if(iter != m_setFuse.end())
		nScore	= iter->second;

	int nSort = CItem::GetItemSort(pItem->GetInt(ITEMDATA_TYPE));
	if(nSort >= 1 && nSort <= 8)
	{
		nKey	= FUSEKEY_SORT_BASE + pItem->GetItemSubType() * 1000;
		nKey	= nKey + pItem->GetLevel() * 10;
		nKey	+= pItem->GetQuality();
		iter	= m_setFuse.find(nKey);
		if(iter != m_setFuse.end() && nScore <= iter->second)
			nScore	= iter->second;
	}
	return nScore;
}
