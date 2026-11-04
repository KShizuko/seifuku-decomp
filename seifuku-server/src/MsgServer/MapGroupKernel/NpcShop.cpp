#include "windows.h"
#include "define.h"
#include "NpcShop.h"
#include "MapGroup.h"
#include "npc.h"

const int	SYNSHOP_REBATE_PERCENT				= 90;

MYHEAP_IMPLEMENTATION(CNpcShop,s_heap)
CNpcShop::CNpcShop()
{
	m_setGoods	= NULL;
}

CNpcShop::~CNpcShop()
{
	if(m_setGoods)
		m_setGoods->Release();
}

bool CNpcShop::Create(PROCESS_ID idProcess, CNpc* pOwner, OBJID idNpc)
{
	m_idProcess	= idProcess;
	m_pOwner	= pOwner;

	m_setGoods	= CGoodsSet::CreateNew(false);

	enum SHOPDATA { SHOPDATA_OWNERID=1, SHOPDATA_ITEMTYPE, SHOPDATA_MONEYTYPE, };
	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT * FROM cq_goods WHERE ownerid=%u", idNpc);
	IRecordset* pRes = Database()->CreateNewRecordset(szSQL, false);
	if(!pRes)
		return true;

	for(int i = 0; i < pRes->RecordCount(); i++)
	{
		OBJID	idType = pRes->GetInt(SHOPDATA_ITEMTYPE);
		CItemTypeData* pType = ItemType()->QueryItemType(idType);
		if(!pType || m_setGoods->GetObj(pType->GetID()))
		{
			LOGWARNING("Shop [%u] has an illegal or duplicate itemtype [%u]", idNpc, idType);

			pRes->MoveNext();
			continue;
		}

		m_setGoods->AddObj(pType);
		m_setMoneyType[idType] = pRes->GetInt(SHOPDATA_MONEYTYPE);
		pRes->MoveNext();
	}
	pRes->Release();

	return true;
}

bool CNpcShop::IsEmoneyGoods(OBJID idType)
{
	MONEYTYPE_SET::iterator iter = m_setMoneyType.find(idType);
	if(iter == m_setMoneyType.end())
		return false;
	return iter->second != 0;
}

bool CNpcShop::IsOpen()	{ return !(m_pOwner->IsSynNpc() && m_pOwner->GetMap()->GetSynID() != ID_NONE); }

int CNpcShop::Rebate(int nPrice, OBJID idSyn, int nRankShow)
{
	OBJID idOwnerSyn = m_pOwner->GetSynID();
	if(idOwnerSyn != ID_NONE && idSyn == idOwnerSyn)
	{
		return MulDiv(nPrice, SYNSHOP_REBATE_PERCENT, 100);
	}

	return nPrice;
}
