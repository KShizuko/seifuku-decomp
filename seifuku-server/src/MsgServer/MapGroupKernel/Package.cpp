#include "AllMsg.h"
#include "Package.h"
#include "User.h"
MYHEAP_IMPLEMENTATION(CPackage,s_heap)
CPackage::CPackage()
{
	m_nType		= _STORE_NONE;
	m_setItem	= NULL;
}

CPackage::~CPackage()
{
	if(m_setItem)
		m_setItem->Release();
}

bool CPackage::Create(int nType, OBJID idOwner, OBJID idPlayer, IDatabase* pDb)
{
	CHECKF(nType != _STORE_NONE);
	if(nType == m_nType && idOwner == m_idOwner && idPlayer == m_idPlayer)
		return true;

	m_nType		= nType;
	m_idOwner	= idOwner;
	m_idPlayer	= idPlayer;

	if(m_setItem)
		m_setItem->Release();
	m_setItem	= CItemSet::CreateNew(true);
	CHECKF(m_setItem);

	int	nPosition = m_nType;
	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT * FROM cq_item WHERE owner_id=%u && position=%d && player_id=%u LIMIT %d", idOwner, nPosition, idPlayer, PACKAGE_LIMIT);
	IRecordset* pRes = pDb->CreateNewRecordset(szSQL, false);
	if(!pRes)
		return true;
	for(int i = 0; i < pRes->RecordCount(); i++,pRes->MoveNext())
	{
		CItemPtr pItem = CItem::CreateNew();
		MYASSERT(pItem);
		if(!pItem)
			continue;

		if(pItem->Create(pRes))
		{
			if(pItem->IsChkSumOK())
				m_setItem->AddObj(pItem);
			else
			{
				OBJID idItem = pItem->GetID();
				pItem->ReleaseByOwner();
				::MyLogSave("gmlog/invalid_item", "invalid item:[%u] found in user:[%s][%u]", idItem, "unknow", idPlayer);
			}
		}
		else
			pItem->ReleaseByOwner();
	}
	pRes->Release();

	return true;
}

bool CPackage::CheckIn(CUser* pUser, CItem* pItem, bool bUpdate )
{
	CHECKF(m_nType != _STORE_NONE);
	CHECKF(pUser);
	CHECKF(pItem);

	if(m_nType == ITEMPOSITION_CHEST && !pItem->IsExchangeEnable())
	{
		return false;
	}

	int		nPosition = m_nType;
	OBJID	idPlayer = (IsPlayerPackage() ? pUser->GetID() : ID_NONE);
	pItem->SetInt(ITEMDATA_OWNERID, m_idOwner, UPDATE_FALSE);
	pItem->SetInt(ITEMDATA_PLAYERID, idPlayer, UPDATE_FALSE);
	pItem->SetInt(ITEMDATA_POSITION, nPosition, bUpdate);
	m_setItem->AddObj(pItem);

	MsgPackageItemInfo	info;
	info.idItem		= pItem->GetID();
	info.idItemType	= pItem->GetInt(ITEMDATA_TYPE);
	info.ucIdent	= pItem->GetInt(ITEMDATA_IDENT);
	info.ucGem1		= pItem->GetInt(ITEMDATA_GEM1);
	info.ucGem2		= pItem->GetInt(ITEMDATA_GEM2);
	info.ucMagic1	= pItem->GetInt(ITEMDATA_MAGIC1);
	info.ucMagic2	= pItem->GetInt(ITEMDATA_MAGIC2);
	info.ucMagic3	= pItem->GetInt(ITEMDATA_MAGIC3);
	info.ucReduceDmg	= pItem->GetInt(ITEMDATA_REDUCE_DMG);
	info.usAddLife		= pItem->GetInt(ITEMDATA_ADD_LIFE);
	info.usAntiMonster	= pItem->GetInt(ITEMDATA_ANTI_MONSTER);

	CMsgPackage msg;
	CHECKF(msg.Create(MSGPACKAGE_CHECKIN, m_idOwner, &info, 1));
	pUser->SendMsg(&msg);
	return true;
}

CItem* CPackage::CheckOut(CUser* pUser, OBJID idItem, bool bUpdate)
{
	CHECKF(m_nType != _STORE_NONE);

	CItemPtr pItem = m_setItem->GetObj(idItem);
	if(!pItem)
		return false;

	pItem->SetInt(ITEMDATA_OWNERID, pUser->GetID(), UPDATE_FALSE);
	pItem->SetInt(ITEMDATA_PLAYERID, pUser->GetID(), UPDATE_FALSE);
	pItem->SetInt(ITEMDATA_POSITION, ITEMPOSITION_BACKPACK, bUpdate);
	pItem->SetInt(ITEMDATA_POSITION, ITEMPOSITION_BACKPACK, UPDATE_TRUE);
	m_setItem->PopObj(idItem);

	CMsgPackage msg;
	CHECKF(msg.Create(MSGPACKAGE_CHECKOUT, m_idOwner, idItem));
	pUser->SendMsg(&msg);
	return pItem;
}
bool CPackage::SendInfo(CUser* pUser)
{
	CHECKF(m_nType != _STORE_NONE);
	CHECKF(pUser);

	MsgPackageItemInfo	buf[PACKAGE_LIMIT];
	int		nCount = 0;
	for(int i = 0; i < m_setItem->GetAmount(); i++)
	{
		CItemPtr pItem = m_setItem->GetObjByIndex(i);
		if(pItem && nCount < PACKAGE_LIMIT)
		{
			buf[nCount].ucIdent		= pItem->GetInt(ITEMDATA_IDENT);
			buf[nCount].ucGem1		= pItem->GetInt(ITEMDATA_GEM1);
			buf[nCount].ucGem2		= pItem->GetInt(ITEMDATA_GEM2);
			buf[nCount].ucMagic1	= pItem->GetInt(ITEMDATA_MAGIC1);
			buf[nCount].ucMagic2	= pItem->GetInt(ITEMDATA_MAGIC2);
			buf[nCount].ucMagic3	= pItem->GetInt(ITEMDATA_MAGIC3);
			buf[nCount].ucReduceDmg		= pItem->GetInt(ITEMDATA_REDUCE_DMG);
			buf[nCount].usAddLife		= pItem->GetInt(ITEMDATA_ADD_LIFE);
			buf[nCount].usAntiMonster	= pItem->GetInt(ITEMDATA_ANTI_MONSTER);

			buf[nCount].idItem			= pItem->GetID();
			buf[nCount].idItemType	= pItem->GetInt(ITEMDATA_TYPE);

			++nCount;
		}
		else
			break;
	}

	CMsgPackage msg;
	CHECKF(msg.Create(MSGPACKAGE_QUERYLIST, m_idOwner, buf, nCount));
	pUser->SendMsg(&msg);

	return true;
}

CItem* CPackage::GetItemByID(OBJID idItem)
{
	return m_setItem->GetObj(idItem);
}

int CPackage::GetItemType(int index)
{
	CHECKF(m_nType != _STORE_NONE);

	if(index >= 0 && index < m_setItem->GetAmount())
	{
		CItemPtr pItem = m_setItem->GetObjByIndex(index);
		if(pItem)
			return  pItem->GetInt(ITEMDATA_TYPE);
	}

	return ID_NONE;
}
