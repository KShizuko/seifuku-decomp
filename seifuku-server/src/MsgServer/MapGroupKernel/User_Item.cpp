#include "AllMsg.h"
#include "User.h"
#include "MapGroup.h"
#include "netmsg.h"
#include "ItemType.h"
#include "MapItem.h"
#include "Package.h"
#include "FuseItem.h"
const int	MINI_ADDLIFE_DRAG				= 100;
const int	STORAGE_NPC_ID					= 1005;
const int	_MINE_RANGE						= 1;
const int	_MAX_USERITEMSIZE				= 40;
bool CUser::CreateAllItem(IDatabase* pDb)
{
	SQLBUF		szSQL;
	_snprintf(szSQL, sizeof(szSQL), "SELECT * FROM GhostContract WHERE owner_id=%u", GetID());
	IRecordset*	pResLockDate = pDb->CreateNewRecordset(szSQL);
	if(pResLockDate)
	{
		for(int i = 0; i < (int)pResLockDate->RecordCount(); i++)
		{
			SetItemLockDate(pResLockDate->LoadDWord("Item_id"), pResLockDate->LoadDWord("timeout"));

			pResLockDate->MoveNext();
		}
		SAFE_RELEASE(pResLockDate);
	}

	_snprintf(szSQL, sizeof(szSQL), "SELECT * FROM cq_item WHERE player_id=%u order by position desc", GetID());
	IRecordset*	pRes = pDb->CreateNewRecordset(szSQL);
	if(pRes)
	{
		for(int i = 0; i < (int)pRes->RecordCount(); i++)
		{
			CItemPtr	pItem = CItem::CreateNew();
			if(pItem)
			{
				if(pItem->Create(pRes))
				{
					if(!pItem->IsChkSumOK())
					{
						::MyLogSave("gmlog/invalid_item", "invalid item:[%u] found in user:[%s][%u]", pItem->GetID(), GetName(), GetID());
					}
					else
					{
						int nPosition = pItem->GetInt(ITEMDATA_POSITION);
						if(nPosition >= ITEMPOSITION_EQUIPBEGIN && nPosition < ITEMPOSITION_EQUIPEND)
						{
							CItemPtr& refpEquip = GetEquipItemRef(nPosition);
							if(!refpEquip)
							{
								refpEquip = pItem;
								EquipMagicItem(pItem, nPosition);
								pItem = NULL;
							}
							else
							{
								::LogSave("Found same position of user equip.");
							}
						}

						if(pItem)
						{
							if(m_setItem.size() < _MAX_USERITEMSIZE && pItem->GetInt(ITEMDATA_POSITION) <= ITEMPOSITION_USER_LIMIT)
							{
								if(pItem->GetInt(ITEMDATA_POSITION) != ITEMPOSITION_BACKPACK)
									pItem->SetInt(ITEMDATA_POSITION, ITEMPOSITION_BACKPACK);
								m_setItem.push_back(pItem);
							}
							else if(pItem->TestSpecialFlag(2))
							{
								m_setUnkItem.push_back(pItem);
							}
							else
							{
								pItem->ReleaseByOwner();
							}
						}
					}
				}
				else
				{
					LOGERROR("Player [%s] cannot load item [%u]", GetName(), pItem->GetID());
					pItem->ReleaseByOwner();
				}
			}

			pRes->MoveNext();
		}
		pRes->Release();
	}

	CItemPtr& refpWeaponL = GetEquipItemRef(ITEMPOSITION_WEAPONL);
	if(refpWeaponL && refpWeaponL->IsWeapon2())
	{
		refpWeaponL->SetInt(ITEMDATA_POSITION, ITEMPOSITION_BACKPACK);
		m_setItem.push_back(refpWeaponL);
		refpWeaponL = NULL;
	}
	CItemPtr& refpRight	= GetEquipItemRef(ITEMPOSITION_WEAPONR);
	CItemPtr& refpLeft	= GetEquipItemRef(ITEMPOSITION_WEAPONL);
	if(refpRight && refpRight->IsWeapon2() && refpLeft)
	{
		if(!refpRight->IsBowSort() || !refpLeft->IsArrowSort())
		{
			refpLeft->SetInt(ITEMDATA_POSITION, ITEMPOSITION_BACKPACK);
			m_setItem.push_back(refpLeft);
			refpLeft = NULL;
		}
	}

	UpdateWeight();
	return true;
}

void CUser::SaveItemInfo()
{
	if(m_pHelmet)
		m_pHelmet->SaveInfo();
	if(m_pNecklace)
		m_pNecklace->SaveInfo();
	if(m_pArmor)
		m_pArmor->SaveInfo();
	if(m_pWeaponR)
		m_pWeaponR->SaveInfo();
	if(m_pWeaponL)
		m_pWeaponL->SaveInfo();
	if(m_pRingR)
		m_pRingR->SaveInfo();
	if(m_pRingL)
		m_pRingL->SaveInfo();
	if(m_pShoes)
		m_pShoes->SaveInfo();
	if(m_pOvercoat)
		m_pOvercoat->SaveInfo();

	for(int i = 0; i < (int)m_setItem.size(); i++)
	{
		if(m_setItem[i])
			m_setItem[i]->SaveInfo();
	}
}

void CUser::DeleteAllItem()
{
	SAFE_RELEASEBYOWNER(m_pHelmet);
	SAFE_RELEASEBYOWNER(m_pNecklace);
	SAFE_RELEASEBYOWNER(m_pArmor);
	SAFE_RELEASEBYOWNER(m_pWeaponR);
	SAFE_RELEASEBYOWNER(m_pWeaponL);
	SAFE_RELEASEBYOWNER(m_pRingR);
	SAFE_RELEASEBYOWNER(m_pRingL);
	SAFE_RELEASEBYOWNER(m_pShoes);
	SAFE_RELEASEBYOWNER(m_pOvercoat);
	for(int i = 0; i < (int)m_setItem.size(); i++)
		SAFE_RELEASEBYOWNER(m_setItem[i]);
	m_setItem.clear();

	UpdateWeight();
}

void CUser::SendAllItemInfo()
{
	if(m_pHelmet)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pHelmet, ITEMINFO_ADDITEM))
			this->SendMsg(&msg);
	}
	if(m_pNecklace)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pNecklace, ITEMINFO_ADDITEM))
			this->SendMsg(&msg);
	}
	if(m_pArmor)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pArmor, ITEMINFO_ADDITEM))
			this->SendMsg(&msg);
	}
	if(m_pWeaponR)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pWeaponR, ITEMINFO_ADDITEM))
			this->SendMsg(&msg);
	}
	if(m_pWeaponL)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pWeaponL, ITEMINFO_ADDITEM))
			this->SendMsg(&msg);
	}
	if(m_pRingR)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pRingR, ITEMINFO_ADDITEM))
			this->SendMsg(&msg);
	}
	if(m_pRingL)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pRingL, ITEMINFO_ADDITEM))
			this->SendMsg(&msg);
	}
	if(m_pShoes)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pShoes, ITEMINFO_ADDITEM))
			this->SendMsg(&msg);
	}
	if(m_pOvercoat)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pOvercoat, ITEMINFO_ADDITEM))
			this->SendMsg(&msg);
	}

	for(int i = 0; i < (int)m_setItem.size(); i++)
	{
		CItemPtr pItem = m_setItem[i];
		if(pItem)
		{
			CMsgItemInfo	msg;
			if(msg.Create(pItem, ITEMINFO_ADDITEM))
				this->SendMsg(&msg);
		}
	}

	for(vector<CItem*>::iterator it = m_setUnkItem.begin(); it != m_setUnkItem.end(); it++)
	{
		CItemPtr pItem = *it;
		if(pItem)
		{
			CMsgItemInfo	msg;
			if(msg.Create(pItem, ITEMINFO_AUCTION))
				this->SendMsg(&msg);
		}
	}
}

void CUser::SendAllEquipInfoTo(CUser* pRecv)
{
	CHECK(pRecv);

	if(m_pHelmet)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pHelmet, ITEMINFO_OTHERPLAYER_EQUIPMENT, GetID()))
			pRecv->SendMsg(&msg);
	}
	if(m_pNecklace)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pNecklace, ITEMINFO_OTHERPLAYER_EQUIPMENT, GetID()))
			pRecv->SendMsg(&msg);
	}
	if(m_pArmor)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pArmor, ITEMINFO_OTHERPLAYER_EQUIPMENT, GetID()))
			pRecv->SendMsg(&msg);
	}
	if(m_pWeaponR)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pWeaponR, ITEMINFO_OTHERPLAYER_EQUIPMENT, GetID()))
			pRecv->SendMsg(&msg);
	}
	if(m_pWeaponL)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pWeaponL, ITEMINFO_OTHERPLAYER_EQUIPMENT, GetID()))
			pRecv->SendMsg(&msg);
	}
	if(m_pRingR)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pRingR, ITEMINFO_OTHERPLAYER_EQUIPMENT, GetID()))
			pRecv->SendMsg(&msg);
	}
	if(m_pRingL)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pRingL, ITEMINFO_OTHERPLAYER_EQUIPMENT, GetID()))
			pRecv->SendMsg(&msg);
	}
	if(m_pShoes)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pShoes, ITEMINFO_OTHERPLAYER_EQUIPMENT, GetID()))
			pRecv->SendMsg(&msg);
	}
	if(m_pOvercoat)
	{
		CMsgItemInfo	msg;
		if(msg.Create(m_pOvercoat, ITEMINFO_OTHERPLAYER_EQUIPMENT, GetID()))
			pRecv->SendMsg(&msg);
	}
}

CItem* CUser::GetItem(OBJID idItem)
{
	for(int i = m_setItem.size() - 1; i >= 0; i--)
	{
		CItemPtr pItem = m_setItem[i];
		if(pItem && pItem->GetID() == idItem)
			return pItem;
	}
	return NULL;
}

CItem* CUser::GetUnkItem(OBJID idItem)
{
	for(vector<CItem*>::iterator it = m_setUnkItem.begin(); it != m_setUnkItem.end(); it++)
	{
		CItemPtr pItem = *it;
		if(pItem && pItem->GetID() == idItem)
			return pItem;
	}
	return NULL;
}

void CUser::ClearUnkItemSet()
{
	for(vector<CItem*>::iterator it = m_setUnkItem.begin(); it != m_setUnkItem.end(); it++)
	{
		if(*it)
			(*it)->ReleaseByOwner();
	}
	m_setUnkItem.clear();
}

CItem* CUser::GetStorageItem(OBJID idItem)
{
	if(m_pStorage)
		return m_pStorage->GetItemByID(idItem);
	return NULL;
}

vector<CItem*>& CUser::GetItemSet()
{
	return m_setItem;
}

CItem* CUser::GetTaskItemByType(OBJID idType)
{
	for(int i = m_setItem.size() - 1; i >= 0; i--)
	{
		CItemPtr pItem = m_setItem[i];
		if(pItem && pItem->GetInt(ITEMDATA_TYPE) == idType)
			return pItem;
	}
	return NULL;
}

CItem* CUser::GetTaskItem(LPCTSTR szItemName)
{
	for(int i = m_setItem.size() - 1; i >= 0; i--)
	{
		CItemPtr pItem = m_setItem[i];
		if(pItem && strcmp(pItem->GetStr(ITEMDATA_NAME), szItemName) == 0)
			return pItem;
	}
	return NULL;
}

CItem* CUser::FindSpaceTransSpell()
{
	for(int i = 0; i < (int)m_setItem.size(); i++)
	{
	}
	return NULL;
}

CItem* CUser::GetEquipItem(OBJID idItem)
{
	if(m_pHelmet && m_pHelmet->GetID() == idItem)
		return m_pHelmet;
	else if(m_pNecklace && m_pNecklace->GetID() == idItem)
		return m_pNecklace;
	else if(m_pArmor && m_pArmor->GetID() == idItem)
		return m_pArmor;
	else if(m_pWeaponR && m_pWeaponR->GetID() == idItem)
		return m_pWeaponR;
	else if(m_pWeaponL && m_pWeaponL->GetID() == idItem)
		return m_pWeaponL;
	else if(m_pRingR && m_pRingR->GetID() == idItem)
		return m_pRingR;
	else if(m_pRingL && m_pRingL->GetID() == idItem)
		return m_pRingL;
	else if(m_pShoes && m_pShoes->GetID() == idItem)
		return m_pShoes;
	else if(m_pOvercoat && m_pOvercoat->GetID() == idItem)
		return m_pOvercoat;

	return NULL;
}

CItem* CUser::GetEquipItemByPos(int nPosition)
{
	CItemPtr* ppEquip = this->GetEquipItemPtr(nPosition);
	if (!ppEquip)
		return NULL;

	return *ppEquip;
}

CItemPtr* CUser::GetEquipItemPtr(int nPosition)
{
	switch(nPosition)
	{
	case	ITEMPOSITION_HELMET:
		return &m_pHelmet;
	case	ITEMPOSITION_NECKLACE:
		return &m_pNecklace;
	case	ITEMPOSITION_ARMOR:
		return &m_pArmor;
	case	ITEMPOSITION_WEAPONR:
		return &m_pWeaponR;
	case	ITEMPOSITION_WEAPONL:
		return &m_pWeaponL;
	case	ITEMPOSITION_RINGR:
		return &m_pRingR;
	case	ITEMPOSITION_RINGL:
		return &m_pRingL;
	case	ITEMPOSITION_SHOES:
		return &m_pShoes;
	case	ITEMPOSITION_MOUNT:
		return &m_pOvercoat;
	}

	return NULL;
}

bool CUser::AddItem (CItem* pItem, bool bSynchro, bool bUpdate )
{
	CHECKF(pItem);

	if(m_setItem.size() >= _MAX_USERITEMSIZE)
		return false;

	PushItem(pItem, bSynchro, bUpdate);
	return true;
}

void CUser::PushItem(CItem* pItem, bool bSynchro, bool bUpdate)
{
	CHECK(pItem);

	bool bChanged = false;
	if (pItem->GetInt(ITEMDATA_OWNERID) != GetID())
	{
		pItem->SetInt(ITEMDATA_OWNERID, GetID());
		bChanged = true;
	}
	if (pItem->GetInt(ITEMDATA_PLAYERID) != GetID())
	{
		pItem->SetInt(ITEMDATA_PLAYERID, GetID());
		bChanged = true;
	}
	if (pItem->GetInt(ITEMDATA_POSITION) != ITEMPOSITION_BACKPACK)
	{
		pItem->SetInt(ITEMDATA_POSITION, ITEMPOSITION_BACKPACK);
		bChanged = true;
	}

	if(m_setItem.size() < _MAX_USERITEMSIZE)
	{
		if(bUpdate && bChanged)
			pItem->SaveInfo();
		m_setItem.push_back(pItem);

		if(bSynchro)
		{
			CMsgItemInfo msg;
			if(msg.Create(pItem, ITEMINFO_ADDITEM))
				this->SendMsg(&msg);
		}
	}

	UpdateWeight();
}

bool CUser::DelItemPt (OBJID idItem)
{
	for(int i = m_setItem.size() - 1; i >= 0; i--)
	{
		CItemPtr pItem = m_setItem[i];
		if(pItem && pItem->GetID() == idItem)
		{
			m_setItem.erase(m_setItem.begin() + i);
			if(m_pBooth)
				m_pBooth->DelItem(idItem);

			UpdateWeight();
			return true;
		}
	}
	return false;
}

CItem* CUser::PopItem (OBJID idItem, bool bSynchro, bool bUpdate)
{
	for(int i = m_setItem.size() - 1; i >= 0; i--)
	{
		CItemPtr pItem = m_setItem[i];
		if(pItem && pItem->GetID() == idItem)
		{
			pItem->SetInt(ITEMDATA_PLAYERID, ID_NONE, UPDATE_FALSE);
			pItem->SetInt(ITEMDATA_OWNERID, ID_NONE, bUpdate);
			m_setItem.erase(m_setItem.begin() + i);
			if(m_pBooth)
				m_pBooth->DelItem(idItem);

			if(bSynchro)
			{
				CMsgItem msg;
				if(msg.Create(idItem, ITEMACT_DROP))
					this->SendMsg(&msg);
			}

			UpdateWeight();
			return pItem;
		}
	}
	return NULL;
}

CItem* CUser::AwardItem(int nItemType, bool bSynchro)
{
	if(m_setItem.size() >= _MAX_USERITEMSIZE)
	{
		this->SendSysMsg(STR_FULLBAG_NO_MORE);
		return NULL;
	}

	return AwardNewItem(nItemType, bSynchro);
}

CItem* CUser::AwardNewItem(int nItemType, bool bSynchro)
{
	CItemPtr pItem = CItem::CreateNew();
	CHECKF(pItem);

	if(!pItem->Create(GameDataDefault()->GetGameItemData(), nItemType, GetID(), ITEMPOSITION_BACKPACK))
	{
		pItem->ReleaseByOwner();
		return NULL;
	}

	PushItem(pItem, bSynchro, true);

	if(pItem->GetInt(ITEMDATA_TYPE) == 1088000)
	{
		SQLBUF	szSQL = "";
		_snprintf(szSQL, sizeof(szSQL), "INSERT INTO item_log SET item_id=%u,item_type=%u,owner_id=%u,time=%u,event=%u", pItem->GetID(), pItem->GetInt(ITEMDATA_TYPE), GetID(), ::TimeGetMinute(), 22);
		Database()->ExecuteAsyncSQL(szSQL);
	}
	else if(pItem->IsGem() && (pItem->GetInt(ITEMDATA_TYPE) & 0x0F) == 3)
	{
		SQLBUF	szSQL = "";
		_snprintf(szSQL, sizeof(szSQL), "INSERT INTO item_log SET item_id=%u,item_type=%u,owner_id=%u,time=%u,event=%u", pItem->GetID(), pItem->GetInt(ITEMDATA_TYPE), GetID(), ::TimeGetMinute(), 2);
		Database()->ExecuteAsyncSQL(szSQL);
	}
	else if(pItem->IsEquipment() && (pItem->GetInt(ITEMDATA_GEM1) != 0 || pItem->GetInt(ITEMDATA_GEM2) != 0))
	{
		SQLBUF	szSQL = "";
		_snprintf(szSQL, sizeof(szSQL), "INSERT INTO item_log SET item_id=%u,item_type=%u,owner_id=%u,time=%u,event=%u", pItem->GetID(), pItem->GetInt(ITEMDATA_TYPE), GetID(), ::TimeGetMinute(), 12);
		Database()->ExecuteAsyncSQL(szSQL);
	}

	return pItem;
}

bool CUser::EraseItem(OBJID idItem, bool bSynchro)
{
	for(int i = 0; i < (int)m_setItem.size(); i++)
	{
		CItemPtr pItem = m_setItem[i];
		if(pItem && pItem->GetID() == idItem && !pItem->IsPlunder() && !pItem->TestSpecialFlag(2) && !pItem->TestSpecialFlag(1)
				&& !IsItemProtected(pItem))
		{
			m_setItem.erase(m_setItem.begin() + i);
			if(m_pBooth)
				m_pBooth->DelItem(idItem);

			pItem->DeleteRecord();
			pItem->ReleaseByOwner();

			if(bSynchro)
			{
				CMsgItem msg;
				if(msg.Create(idItem, ITEMACT_DROP))
					this->SendMsg(&msg);
			}

			UpdateWeight();
			m_dwUnk5B0	= idItem;
			return true;
		}
	}
	return false;
}

bool CUser::EraseEquip(int nPosition, bool bSynchro)
{
	CItemPtr& refpEquip = GetEquipItemRef(nPosition);
	CHECKF(refpEquip);

	OBJID idItem = refpEquip->GetID();
	refpEquip->DeleteRecord();
	refpEquip->ReleaseByOwner();
	refpEquip = NULL;

	if(bSynchro)
	{
		CMsgItem msg;
		if(msg.Create(idItem, ITEMACT_DROPEQUIPMENT, nPosition))
			this->SendMsg(&msg);

		if(nPosition == ITEMPOSITION_HELMET
				|| nPosition == ITEMPOSITION_ARMOR
				|| nPosition == ITEMPOSITION_WEAPONR
				|| nPosition == ITEMPOSITION_WEAPONL )
		{
			CMsgPlayer msgPlayer;
			if (msgPlayer.Create(this->QueryRole()))
				this->BroadcastRoomMsg(&msgPlayer, EXCLUDE_SELF);
		}
	}

	UpdateWeight();
	return true;
}

bool CUser::EquipItem(CItem* pItem, int nPosition, bool bSynchro)
{
	CHECKF(pItem);

	QueryMagic()->AbortMagic(false);

	OBJID idItem = pItem->GetID();
	if (!this->TryItem(idItem, nPosition))
		return false;

	if (this->IsWing() && !pItem->IsArrowSort() && !pItem->IsBowSort())
		return false;

	if(nPosition == ITEMPOSITION_BACKPACK)
	{
		if(pItem->GetItemSort() == ITEMSORT_HELMET)
		{
			nPosition	= ITEMPOSITION_HELMET;
		}
		else if(pItem->GetItemSort() == ITEMSORT_NECKLACE)
		{
			nPosition	= ITEMPOSITION_NECKLACE;
		}
		else if(pItem->GetItemSort() == ITEMSORT_ARMOR)
		{
			nPosition	= ITEMPOSITION_ARMOR;
		}
		else if(pItem->IsHoldEnable())
		{
			if(pItem->IsArrowSort())
			{
				nPosition	= ITEMPOSITION_WEAPONL;
			}
			else if(pItem->IsWeapon2())
			{
				nPosition	= ITEMPOSITION_WEAPONR;
			}
			else if(pItem->IsWeapon1() && (!m_pWeaponR || m_pWeaponL))
			{
				nPosition	= ITEMPOSITION_WEAPONR;
			}
			else
			{
				nPosition	= ITEMPOSITION_WEAPONL;
			}
		}
		else if(pItem->GetItemSort() == ITEMSORT_RING)
		{
			if(!m_pRingR)
				nPosition	= ITEMPOSITION_RINGR;
		}
		else if (pItem->GetItemSort() == ITEMSORT_EXTEND)
		{
			nPosition	= ITEMPOSITION_RINGL;
		}
		else if(pItem->GetItemSort() == ITEMSORT_SHOES)
		{
			nPosition	= ITEMPOSITION_SHOES;
		}
		else if(pItem->GetItemSort() == ITEMSORT_MANTLE)
		{
			nPosition	= ITEMPOSITION_MOUNT;
		}
	}

	if(m_setItem.size() > _MAX_USERITEMSIZE)
	{
		CItem* pEquip = GetEquipItemByPos(nPosition);
		if(pEquip)
			return false;
	}

	if(nPosition == ITEMPOSITION_WEAPONR && !GetWeaponR() && GetWeaponL() && GetWeaponL()->IsArrowSort() && !pItem->IsBowSort())
		return false;

	CHECKF(DelItemPt(idItem));
	bool	bRet	= false;
	switch(nPosition)
	{
	case	ITEMPOSITION_HELMET:
		{
			if(pItem->GetItemSort() == ITEMSORT_HELMET)
			{
				UnEquipOnly(nPosition);
				m_pHelmet	= pItem;
				pItem->SetInt(ITEMDATA_POSITION, ITEMPOSITION_HELMET);
				bRet	= true;
			}
		}
		break;
	case	ITEMPOSITION_NECKLACE:
		{
			if(pItem->GetItemSort() == ITEMSORT_NECKLACE)
			{
				UnEquipOnly(nPosition);
				m_pNecklace	= pItem;
				pItem->SetInt(ITEMDATA_POSITION, ITEMPOSITION_NECKLACE);
				bRet	= true;
			}
		}
		break;
	case	ITEMPOSITION_ARMOR:
		{
			if(pItem->GetItemSort() == ITEMSORT_ARMOR)
			{
				UnEquipOnly(nPosition);
				m_pArmor	= pItem;
				pItem->SetInt(ITEMDATA_POSITION, ITEMPOSITION_ARMOR);
				bRet	= true;
			}
		}
		break;
	case	ITEMPOSITION_WEAPONR:
		{
			if(pItem->IsHoldEnable())
			{
				if(pItem->IsWeapon2())
				{
					if(m_pWeaponR && m_pWeaponL && m_setItem.size() + 2 > _MAX_USERITEMSIZE)
						break;
					UnEquipOnly(ITEMPOSITION_WEAPONR);
					if(m_pWeaponL && (!m_pWeaponL->IsArrowSort() || !pItem->IsBowSort()))
						UnEquipOnly(ITEMPOSITION_WEAPONL);
					m_pWeaponR	= pItem;
					pItem->SetInt(ITEMDATA_POSITION, ITEMPOSITION_WEAPONR);
					bRet	= true;
				}
				else if(pItem->IsWeapon1())
				{
					if(m_pWeaponL && m_pWeaponL->IsArrowSort() && m_setItem.size() + 2 > _MAX_USERITEMSIZE)
						break;
					UnEquipOnly(ITEMPOSITION_WEAPONR);
					if(m_pWeaponL && m_pWeaponL->IsArrowSort())
						UnEquipOnly(ITEMPOSITION_WEAPONL);
					m_pWeaponR	= pItem;
					pItem->SetInt(ITEMDATA_POSITION, ITEMPOSITION_WEAPONR);
					bRet	= true;
				}
			}
		}
		break;
	case	ITEMPOSITION_WEAPONL:
		{
			if(pItem->IsHoldEnable() && m_pWeaponR)
			{
				bool	bEnable	= false;
				if(m_pWeaponR->IsWeapon1() && (pItem->IsWeapon1() || pItem->GetItemSort() == ITEMSORT_SHIELD))
					bEnable	= true;
				else if(m_pWeaponR->IsBowSort() && pItem->IsArrowSort())
					bEnable	= true;

				if(bEnable && (IsBowman() || ((DWORD)GetLev() >= 40 && GetProfessionSort() < 10)))
				{
					UnEquipOnly(nPosition);
					m_pWeaponL	= pItem;
					pItem->SetInt(ITEMDATA_POSITION, ITEMPOSITION_WEAPONL);
					UnequipMagicItem(ITEMPOSITION_WEAPONL);
					bRet	= true;
				}
			}
		}
		break;
	case	ITEMPOSITION_RINGR:
		{
			if(pItem->GetItemSort() == ITEMSORT_RING)
			{
				UnEquipOnly(nPosition);
				m_pRingR	= pItem;
				pItem->SetInt(ITEMDATA_POSITION, ITEMPOSITION_RINGR);
				bRet	= true;
			}
		}
		break;
	case	ITEMPOSITION_RINGL:
		{
			if(pItem->GetItemSort() == ITEMSORT_EXTEND)
			{
				UnEquipOnly(nPosition);
				m_pRingL	= pItem;
				pItem->SetInt(ITEMDATA_POSITION, ITEMPOSITION_RINGL);
				bRet	= true;
			}
		}
		break;
	case	ITEMPOSITION_SHOES:
		{
			if(pItem->GetItemSort() == ITEMSORT_SHOES)
			{
				UnEquipOnly(nPosition);
				m_pShoes	= pItem;
				pItem->SetInt(ITEMDATA_POSITION, ITEMPOSITION_SHOES);
				bRet	= true;
			}
		}
		break;
	case	ITEMPOSITION_MOUNT:
		{
			if(pItem->GetItemSort() == ITEMSORT_MANTLE)
			{
				UnEquipOnly(nPosition);
				m_pOvercoat	= pItem;
				pItem->SetInt(ITEMDATA_POSITION, ITEMPOSITION_MOUNT);
				bRet	= true;
			}
		}
		break;
	default:
		MYASSERT(!"switch");
		break;
	}

	if(!bRet)
	{
		AddItem(pItem, SYNCHRO_FALSE, true);
		return false;
	}

	if(bSynchro)
	{
		CMsgItem msg;
		if(msg.Create(idItem, ITEMACT_EQUIP, nPosition))
			this->SendMsg(&msg);

		if(nPosition == ITEMPOSITION_HELMET
				|| nPosition == ITEMPOSITION_ARMOR
				|| nPosition == ITEMPOSITION_WEAPONR
				|| nPosition == ITEMPOSITION_WEAPONL
				|| nPosition == ITEMPOSITION_MOUNT )
		{
			CMsgPlayer msgPlayer;
			if (msgPlayer.Create(this->QueryRole()))
				this->BroadcastRoomMsg(&msgPlayer, EXCLUDE_SELF);
		}
	}

	UpdateWeight();
	CalcFightRate();
	EquipMagicItem(pItem, nPosition);
	UpdateBattlePower();
	return true;
}

bool CUser::TryItem(OBJID idItem, int nPosition)
{
	CItemPtr pItem = GetItem(idItem);
	if(!pItem)
		return false;

	if (pItem->IsTaskItem())
		return false;

	if (pItem->GetInt(ITEMDATA_REQ_LEVEL) && pItem->GetInt(ITEMDATA_REQ_LEVEL) > (int)GetLev())
		return false;

	if (pItem->IsNeedIdent())
		return false;

	if (pItem->GetInt(ITEMDATA_REQ_SEX) && pItem->GetInt(ITEMDATA_REQ_SEX) != GetSex())
		return false;

	if (pItem->GetItemSort() == ITEMSORT_SHIELD && this->GetProfessionSort() >= 10)
		return false;

	if(this->GetMetempsychosis() && GetLev() >= 70)
	{
		if (pItem->GetInt(ITEMDATA_REQ_LEVEL) <= 70)
			return true;
	}

	if (pItem->GetInt(ITEMDATA_REQ_PROF) != 0)
	{
		int nRequireProfSort	= (pItem->GetInt(ITEMDATA_REQ_PROF)%1000)/10;
		int nRequireProfLevel	= pItem->GetInt(ITEMDATA_REQ_PROF)%10;

		int nProfSort  = this->GetProfessionSort();
		int nProfLevel = this->GetProfessionLevel();
		if (nRequireProfSort == 19)
		{
			if (nProfSort < 10)
				return false;
			else
			{
				if (ITEMPOSITION_WEAPONL == nPosition)
					return false;
			}
		}
		else
		{
			if (nRequireProfSort != nProfSort)
				return false;
		}

		if (nProfLevel < nRequireProfLevel)
			return false;
	}

	if (pItem->GetInt(ITEMDATA_REQ_SKILL) && pItem->IsWeapon() &&
				pItem->GetInt(ITEMDATA_REQ_SKILL) > (int)GetWeaponSkillLev(pItem->GetInt(ITEMDATA_TYPE)))
		return false;

	if ((pItem->GetInt(ITEMDATA_REQ_FORCE) && pItem->GetInt(ITEMDATA_REQ_FORCE) > (int)GetForce())
		|| (pItem->GetInt(ITEMDATA_REQ_SPEED) && pItem->GetInt(ITEMDATA_REQ_SPEED) > (int)GetSpeed())
		|| (pItem->GetInt(ITEMDATA_REQ_HEALTH) && pItem->GetInt(ITEMDATA_REQ_HEALTH) > (int)GetHealth())
		|| (pItem->GetInt(ITEMDATA_REQ_SOUL) && pItem->GetInt(ITEMDATA_REQ_SOUL) > (int)GetSoul()))
	{
		return false;
	}

	return true;
}

bool CUser::UseExpBall(OBJID idItem)
{
	CItemPtr pItem = GetItem(idItem);
	if(!pItem)
		return false;

	if(!this->IsAlive())
		return false;

	if((int)this->GetLev() >= g_nMaxLevel)
		return false;

	if(this->IsIncomeLimited() && !this->GetIncomePercent())
		return false;

	int		nAddTime	= 600;
	int		nToday		= ::DateStamp() % 10000;
	DWORD	dwUsage		= m_data.m_Info.dwExpBallUsage;
	if(nToday != (int)(dwUsage / 10))
	{
		m_data.SetExpBallUsage(nToday*10, true);
	}
	else
	{
		int nCount = dwUsage % 10;
		if(nCount >= 9)
			return false;
		if(nCount >= 4)
			nAddTime = 300;
		m_data.SetExpBallUsage(nToday*10 + nCount + 1, true);
	}

	CMsgUserAttrib	msg;
	if(msg.Create(GetID(), 128, m_data.m_Info.dwExpBallUsage))
		this->SendMsg(&msg);

	IF_NOT(EraseItem(pItem->GetID(), SYNCHRO_TRUE))
		return false;

	UpdateWeight();

	int nLev = this->GetLev();
	CLevupexpData* pLevupexp = LevupexpSet()->GetObj(nLev);
	if(!pLevupexp)
		return false;

	__int64	i64Need	= __max(pLevupexp->GetInt64(LEVUPEXPDATA_EXP), (__int64)1);
	double	dUpTime	= pLevupexp->GetInt(LEVUPEXPDATA_UP_LEV_TIME);
	int		nTime	= (int)(this->GetExp() * dUpTime / i64Need);
	if(this->IsIncomeLimited())
		nAddTime	= this->GetIncomePercent() * nAddTime / 100;
	int		nTotal	= nTime + nAddTime;

	int nAddLev = 0;
	while(nLev < g_nMaxLevel)
	{
		CLevupexpData* pData = LevupexpSet()->GetObj(nLev);
		if(!pData)
			break;
		pLevupexp	= pData;
		if(nTotal < pLevupexp->GetInt(LEVUPEXPDATA_UP_LEV_TIME))
			break;
		nTotal	-= pLevupexp->GetInt(LEVUPEXPDATA_UP_LEV_TIME);
		nAddLev++;
		nLev++;
	}

	bool bSatori = false;
	if(nAddLev > 0)
	{
		this->IncLev(nAddLev, &bSatori);
		if(this->IsAutoAllot())
			this->AllotPoint();
		else
			this->AddAttrib(_USERATTRIB_ADDPOINT, nAddLev * 3, SYNCHRO_TRUE, NULL);
		this->SetAttrib(_USERATTRIB_LIFE, this->GetMaxLife(), SYNCHRO_TRUE);
		if(this->GetTeam())
			this->BroadcastTeamLife(true);
	}

	int		nLevTime	= __max(pLevupexp->GetInt(LEVUPEXPDATA_UP_LEV_TIME), 1);
	__int64	i64NewExp	= (__int64)(pLevupexp->GetInt64(LEVUPEXPDATA_EXP) * (double)nTotal / nLevTime);
	if(!bSatori || this->GetExp() < i64NewExp)
		this->SetAttrib(_USERATTRIB_EXP, i64NewExp, SYNCHRO_TRUE);

	this->AddUnk8E8(nAddTime);
	this->ContributeExp(true);
	return true;
}

void CUser::AddContributeExp(__int64 i64Exp)
{
	m_i64ContributeExp	+= i64Exp;
}

void CUser::AddUnk8E8(int nData)
{
	m_dwUnk8E8	+= nData;
}

void CUser::GuideNotify(int nData)
{
}

void CUser::GuideAddTime(int nHours)
{
}

bool CUser::UseItem(OBJID idItem, int nPosition, bool bSynchro)
{
	if (!this->TryItem(idItem, nPosition))
		return false;

	CItemPtr pItem = GetItem(idItem);
	if(!pItem)
		return false;

	if (pItem->IsPlunder())
		return false;

	if (pItem->GetInt(ITEMDATA_TYPE) == 723700)
		return UseExpBall(idItem);

	if(pItem->IsActionItem())
	{
		switch(pItem->GetInt(ITEMDATA_TYPE))
		{
		case 1200000:	GuideAddTime(72);	break;
		case 1200001:	GuideAddTime(168);	break;
		case 1200002:	GuideAddTime(720);	break;
		}

		SetTaskItemID(pItem->GetID());
		GameAction()->ProcessAction(pItem->GetInt(ITEMDATA_ACTION), this, NULL, pItem);
		return true;
	}
	else if (pItem->IsSpell())
	{
		int nAmount = pItem->GetInt(ITEMDATA_AMOUNT);
		if (nAmount <= 0)
		{
			IF_NOT (EraseItem(pItem->GetID(), SYNCHRO_TRUE))
				return false;
		}

		OBJID idAction = pItem->GetInt(ITEMDATA_ACTION);

		pItem->SetInt(ITEMDATA_AMOUNT, --nAmount);
		if (nAmount <= 0)
		{
			MYASSERT (EraseItem(pItem->GetID(), SYNCHRO_TRUE));
			pItem	= NULL;
		}
		else
		{
			pItem->SaveInfo();

			CMsgItemInfo msg;
			IF_OK (msg.Create(pItem, ITEMINFO_ADDITEM))
				this->SendMsg(&msg);
		}

		if (idAction != ID_NONE)
		{
			SetTaskItemID(ID_NONE);
			GameAction()->ProcessAction(idAction, this, NULL, NULL);
		}
		return true;
	}
	else if(pItem->IsEquipEnable())
		return EquipItem(pItem, nPosition, bSynchro);
	else if(pItem->IsEatEnable())
	{
		int	nAddLife	= pItem->GetInt(ITEMDATA_AMOUNT) ? pItem->GetInt(ITEMDATA_LIFE) : 0;
		int	nAddMana	= pItem->GetInt(ITEMDATA_MANA);

		if(nAddLife > 0 && this->GetLife() >= this->GetMaxLife())
			return false;
		if(nAddMana > 0 && this->GetMana() >= this->GetMaxMana())
			return false;

		DEBUG_TRY
		IF_NOT(EraseItem(pItem->GetID(), SYNCHRO_TRUE))
			return false;

		if(nAddLife > 0)
		{
			AddAttrib(_USERATTRIB_LIFE, nAddLife, SYNCHRO_TRUE, NULL);
			this->BroadcastTeamLife();

			{
				if((IsCheater(_TYPE_WS)||IsCheater(_TYPE_FY)) && IsCheater(_TYPE_USE_LIFE) && ::RandGet(3)==0)
					KickoutCheat(_TYPE_USE_LIFE);
			}
		}
		else if (nAddLife < 0)
		{
			AddAttrib(_USERATTRIB_LIFE, nAddLife, SYNCHRO_TRUE, NULL);
			this->BroadcastTeamLife();
		}

		if(nAddMana != 0)
		{
			AddAttrib(_USERATTRIB_MANA, nAddMana, SYNCHRO_TRUE, NULL);

			{
				if((IsCheater(_TYPE_WS)||IsCheater(_TYPE_FY)) && IsCheater(_TYPE_USE_MANA) && ::RandGet(3)==0)
					KickoutCheat(_TYPE_USE_MANA);
			}
		}
		DEBUG_CATCH("UseItem")

		UpdateWeight();
		return true;
	}

	return false;
}

bool CUser::UnEquipItem(int nPosition, bool bSynchro)
{
	if(!( nPosition >= ITEMPOSITION_EQUIPBEGIN && nPosition < ITEMPOSITION_EQUIPEND ))
		return false;

	QueryMagic()->AbortMagic(false);

	CItem* pEquip = GetEquipItemByPos(nPosition);
	if(!pEquip)
		return false;

	if(nPosition == ITEMPOSITION_WEAPONR && m_pWeaponL && (m_pWeaponL->IsShield() || m_pWeaponL->IsArrowSort()))
	{
		if(m_setItem.size() + 2 > _MAX_USERITEMSIZE)
			return false;
	}
	else
	{
		if(m_setItem.size() + 1 > _MAX_USERITEMSIZE)
			return false;
	}

	CItemPtr pItem = UnEquipOnly(nPosition);
	CHECKF(pItem);

	if(!m_pWeaponR && m_pWeaponL)
	{
		if(m_pWeaponL->IsWeapon1())
		{
			m_pWeaponR = m_pWeaponL;
			m_pWeaponL = NULL;
		}
		else if(m_pWeaponL->IsShield() || m_pWeaponL->IsArrowSort())
		{
			UnEquipOnly(ITEMPOSITION_WEAPONL);
		}
	}

	if(bSynchro)
	{
		CMsgItem msg;
		if(msg.Create(pItem->GetID(), ITEMACT_UNEQUIP, nPosition))
			this->SendMsg(&msg);

		if(nPosition == ITEMPOSITION_HELMET
				|| nPosition == ITEMPOSITION_ARMOR
				|| nPosition == ITEMPOSITION_WEAPONR
				|| nPosition == ITEMPOSITION_WEAPONL )
		{
			CMsgPlayer msgPlayer;
			if (msgPlayer.Create(this->QueryRole()))
				this->BroadcastRoomMsg(&msgPlayer, EXCLUDE_SELF);
		}
	}

	UpdateWeight();
	CalcFightRate();
	UpdateBattlePower();
	return true;
}

CItem* CUser::UnEquipOnly(int nPosition)
{
	CHECKF(m_setItem.size() < _MAX_USERITEMSIZE);

	CItem* pItem = NULL;
	CItemPtr& refpEquip = GetEquipItemRef(nPosition);

	if(nPosition == ITEMPOSITION_WEAPONR)
		QueryStatusSet()->DelObj(STATUS_KEEPBOW);

	if(refpEquip)
	{
		pItem = refpEquip;
		pItem->SetInt(ITEMDATA_POSITION, ITEMPOSITION_BACKPACK);
		m_setItem.push_back(pItem);

		refpEquip = NULL;
	}

	UnequipMagicItem(nPosition);
	return pItem;
}

void CUser::BuyItem (OBJID idNpc, OBJID idType, int nCount)
{
	CNpc* pNpc = NULL;
	bool bRemote = false;
	if(!GetMap()->QueryObj(GetPosX(), GetPosY(), OBJ_NPC, idNpc, IPP_OF(pNpc)))
	{
		IRole* pRole = RoleManager()->QueryRole(idNpc);
		if(!pRole)
			return;
		pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc));
		if(!pNpc)
			return;
		bRemote = true;
	}
	if(!pNpc->IsShopNpc())
		return;
	if(!pNpc->QueryShop()->IsOpen())
		return;

	CItemTypeData* pType = pNpc->QueryShop()->QueryItemType(idType);
	if(!pType)
		return;

	if(!pNpc->QueryShop()->IsEmoneyGoods(idType))
	{
		if(bRemote)
			return;

		int nPrice = pType->GetInt(ITEMTYPEDATA_PRICE);
		nPrice = pNpc->QueryShop()->Rebate(nPrice, GetSynID(), GetSynRankShow());

		if((int)this->GetMoney() < nPrice)
		{
			this->SendSysMsg(STR_NOT_SO_MUCH_MONEY);
			return;
		}

		CItem* pItem = AwardItem(idType, SYNCHRO_TRUE);
		if(!pItem)
		{
			return;
		}

		MYASSERT(SpendMoney(pItem->GetInt(ITEMDATA_PRICE), SYNCHRO_TRUE));
		UpdateWeight();
		return;
	}

	int nPrice = pType->GetInt(ITEMTYPEDATA_EMONEY_PRICE);
	if(nPrice <= 0)
		return;

	ASSERT(nCount >= 1);
	if(m_setItem.size() + nCount > _MAX_USERITEMSIZE)
	{
		this->SendSysMsg(STR_FULLBAG_NO_MORE);
		return;
	}

	if((int)this->GetEmoney() < nPrice * nCount)
	{
		this->SendSysMsg(STR_NOT_SO_MUCH_MONEY);
		return;
	}

	int nBought = 0;
	for(int i = 0; i < nCount; i++)
	{
		if((int)this->GetEmoney() < nPrice || m_setItem.size() >= _MAX_USERITEMSIZE)
			break;
		if(!AwardItem(idType, SYNCHRO_TRUE))
			continue;

		CHECK(AddAttrib(_USERATTRIB_EMONEY, -nPrice, SYNCHRO_TRUE));
		UpdateWeight();
		::MyLogSave("gmlog/emoney_buy", "%u\t%s\t%u\t%u\t%u\t%u\t", GetID(), GetName(), GetAccountID(), pNpc->GetID(), idType, nPrice);
		SaveEmoneyLog(10, GetID(), ID_NONE, nPrice);
		nBought++;
	}
	if(nBought > 0)
		this->SendSysMsg(STR_BUY_ITEM, nBought, pType->GetStr(ITEMTYPEDATA_NAME));
}

void CUser::SellItem(OBJID idNpc, OBJID idItem, DWORD dwData)
{
	CNpc* pNpc;
	if(!GetMap()->QueryObj(GetPosX(), GetPosY(), OBJ_NPC, idNpc, IPP_OF(pNpc)))
		return ;
	if(!pNpc->IsShopNpc())
		return;

	CItemPtr pItem = this->GetItem(idItem);
	if(!pItem)
	{
		this->SendSysMsg(STR_ITEM_NOT_FOUND);
		return;
	}

	if (!pItem->IsSellEnable() || IsItemProtected(pItem))
	{
		this->SendSysMsg(STR_NOT_SALEABLE);
		return;
	}

	if (pItem->IsNonsuchItem())
	{
		::MyLogSave("gmlog/sell_item", "%s(%u) sell item:[id=%u, type=%u], dur=%d, max_dur=%d, price=%d",
				this->GetName(),
				this->GetID(),
				pItem->GetID(),
				pItem->GetInt(ITEMDATA_TYPE),
				pItem->GetInt(ITEMDATA_AMOUNT),
				pItem->GetInt(ITEMDATA_AMOUNTLIMIT),
				pItem->GetSellPrice());
	}

	if(pItem->GetInt(ITEMDATA_TYPE) == 1088000)
	{
		SQLBUF	szSQL = "";
		sprintf(szSQL, "INSERT INTO item_log SET item_id=%u,item_type=%u,owner_id=%u,time=%u,event=%u", pItem->GetID(), pItem->GetInt(ITEMDATA_TYPE), GetID(), ::TimeGetMinute(), 24);
		Database()->ExecuteAsyncSQL(szSQL);
	}
	else if(pItem->IsGem() && (pItem->GetInt(ITEMDATA_TYPE) & 0x0F) == 3)
	{
		SQLBUF	szSQL = "";
		sprintf(szSQL, "INSERT INTO item_log SET item_id=%u,item_type=%u,owner_id=%u,time=%u,event=%u", pItem->GetID(), pItem->GetInt(ITEMDATA_TYPE), GetID(), ::TimeGetMinute(), 4);
		Database()->ExecuteAsyncSQL(szSQL);
	}
	else if(pItem->IsEquipment() && (pItem->GetInt(ITEMDATA_GEM1) || pItem->GetInt(ITEMDATA_GEM2)))
	{
		SQLBUF	szSQL = "";
		sprintf(szSQL, "INSERT INTO item_log SET item_id=%u,item_type=%u,owner_id=%u,time=%u,event=%u", pItem->GetID(), pItem->GetInt(ITEMDATA_TYPE), GetID(), ::TimeGetMinute(), 15);
		Database()->ExecuteAsyncSQL(szSQL);
	}

	DEBUG_TRY
	DWORD	dwMoney = pItem->GetSellPrice();
	if(pItem->IsEquipment() && pItem->GetInt(ITEMDATA_AMOUNT) == 0)
		dwMoney = 0;
	if(!this->EraseItem(idItem, SYNCHRO_TRUE))
	{
		this->SendSysMsg(STR_ITEM_INEXIST);
		return;
	}

	if(!this->GainMoney(dwMoney, SYNCHRO_TRUE))
		this->SendSysMsg(STR_MONEYBAG_FULL);
	DEBUG_CATCH("SellItem")

	UpdateWeight();

	if(!m_pAnalyser->AddAction(dwData))
	{
		SetCheatMark(_TYPE_ACTION_MODE, 0);
		if(IsWeaponR562())
			SetCheatKickout(::RandGet(60) + 600);
	}
}

bool CUser::DropItem(OBJID idItem, int x, int y)
{
	if(!GetMap()->IsLayItemEnable(x, y))
		return false;
	void* pObj = NULL;
	if(GetMap()->QueryObjInPos(x, y, OBJ_MAPITEM, &pObj))
		return false;

	ItemInfoStruct info;
	CItemPtr pItem = NULL;
	bool	bDropItem = false;
	int		nItemType = 0;
	bool	bLog1088000 = false, bLogGemEquip = false, bLogGem = false;
	int i;
	for(i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItemPtr& refpEquip = GetEquipItemRef(i);

		if(refpEquip && refpEquip->GetID() == idItem && !refpEquip->IsPlunder() && !refpEquip->TestSpecialFlag(2)
				&& !refpEquip->TestSpecialFlag(1) && !IsItemProtected(refpEquip))
		{
			CMsgItem msg;
			if (msg.Create(refpEquip->GetID(), ITEMACT_DROPEQUIPMENT, i))
				this->SendMsg(&msg);

			pItem	= refpEquip;
			refpEquip	= NULL;

			pItem->GetInfo(&info);
			bDropItem	= pItem->IsExchangeEnable();

			if (pItem->IsNonsuchItem())
			{
				::MyLogSave("gmlog/drop_item", "%s(%u) drop equip:[id=%u, type=%u], dur=%d, max_dur=%d",
						this->GetName(),
						this->GetID(),
						pItem->GetID(),
						pItem->GetInt(ITEMDATA_TYPE),
						pItem->GetInt(ITEMDATA_AMOUNT),
						pItem->GetInt(ITEMDATA_AMOUNTLIMIT));
			}

			nItemType = pItem->GetInt(ITEMDATA_TYPE);
			if(nItemType == 1088000)
				bLog1088000 = true;
			else if(pItem->IsEquipment() && (pItem->GetInt(ITEMDATA_GEM1) || pItem->GetInt(ITEMDATA_GEM2)))
				bLogGemEquip = true;
			else if(pItem->IsGem() && (pItem->GetInt(ITEMDATA_TYPE) & 0x0F) == 3)
				bLogGem = true;

			pItem->DeleteRecord();
			pItem->ReleaseByOwner();
			pItem = NULL;
			UpdateBattlePower();
			break;
		}
	}
	if(i >= ITEMPOSITION_EQUIPEND)
	{
		pItem = GetItem(idItem);
		if (pItem)
		{
			if (pItem->IsNonsuchItem() && !pItem->IsPlunder() && !pItem->TestSpecialFlag(2) && !pItem->TestSpecialFlag(1)
					&& !IsItemProtected(pItem))
			{
				::MyLogSave("gmlog/drop_item", "%s(%u) drop item:[id=%u, type=%u], dur=%d, max_dur=%d",
						this->GetName(),
						this->GetID(),
						pItem->GetID(),
						pItem->GetInt(ITEMDATA_TYPE),
						pItem->GetInt(ITEMDATA_AMOUNT),
						pItem->GetInt(ITEMDATA_AMOUNTLIMIT));
			}

			if (pItem->IsSellEnable() && !IsItemProtected(pItem))
			{

				pItem->GetInfo(&info);
				bDropItem	= pItem->IsExchangeEnable();
				nItemType = pItem->GetInt(ITEMDATA_TYPE);
				if(nItemType == 1088000)
					bLog1088000 = true;
				else if(pItem->IsEquipment() && (pItem->GetInt(ITEMDATA_GEM1) || pItem->GetInt(ITEMDATA_GEM2)))
					bLogGemEquip = true;
				else if(pItem->IsGem() && (pItem->GetInt(ITEMDATA_TYPE) & 0x0F) == 3)
					bLogGem = true;
				EraseItem(idItem, SYNCHRO_TRUE);
				pItem = NULL;
			}
			else
			{
				this->SendSysMsg(STR_NOT_DISCARDABLE);
				return false;
			}
		}
	}

	if(bDropItem)
	{
		POINT	pos;
		pos.x	= x;
		pos.y	= y;
		CMapItem* pMapItem = CMapItem::CreateNew();
		if(pMapItem)
		{
			IF_OK(pMapItem->Create(MapManager()->SpawnMapItemID(), GetMap(), pos, &info))
			{
				MapManager()->QueryMapItemSet()->AddObj(pMapItem);
			}
			else
			{
				this->SendSysMsg(STR_MAKE_ITEM_FAILED, pMapItem->GetInfo()->idType);
				pMapItem->ReleaseByOwner();
			}
		}
	}

	if(nItemType != 0)
	{
		if(bLog1088000)
		{
			SQLBUF	szSQL = "";
			sprintf(szSQL, "INSERT INTO item_log SET item_id=%u,item_type=%u,owner_id=%u,time=%u,event=%u", idItem, nItemType, GetID(), ::TimeGetMinute(), 23);
			Database()->ExecuteAsyncSQL(szSQL);
		}
		else if(bLogGem)
		{
			SQLBUF	szSQL = "";
			sprintf(szSQL, "INSERT INTO item_log SET item_id=%u,item_type=%u,owner_id=%u,time=%u,event=%u", idItem, nItemType, GetID(), ::TimeGetMinute(), 3);
			Database()->ExecuteAsyncSQL(szSQL);
		}
		else if(bLogGemEquip)
		{
			SQLBUF	szSQL = "";
			sprintf(szSQL, "INSERT INTO item_log SET item_id=%u,item_type=%u,owner_id=%u,time=%u,event=%u", idItem, nItemType, GetID(), ::TimeGetMinute(), 14);
			Database()->ExecuteAsyncSQL(szSQL);
		}
	}

	UpdateWeight();
	return true;
}

bool CUser::DropMoney (int nMoney, int x, int y)
{
	CHECKF(nMoney > 0);
	if(!GetMap()->IsLayItemEnable(x, y))
		return false;
	if((int)this->GetMoney() < nMoney)
		return false;

	POINT	pos;
	pos.x	= x;
	pos.y	= y;
	CMapItem* pMapItem = CMapItem::CreateNew();
	if(pMapItem)
	{
		if(pMapItem->CreateMoney(MapManager()->SpawnMapItemID(), GetMap(), pos, nMoney))
		{
			this->SpendMoney(nMoney, SYNCHRO_TRUE);
			MapManager()->QueryMapItemSet()->AddObj(pMapItem);
			if(nMoney >= 100000)
				::MyLogSave("gmlog/drop_item", "%s(%u) drop money: %d", GetName(), GetID(), nMoney);
		}
		else
		{
			this->SendSysMsg(STR_MAKE_MONEY_FAILED, nMoney);
			pMapItem->ReleaseByOwner();
		}
	}

	return true;
}

bool CUser::SplitItem		(OBJID idItem, int nNum)
{
	if(IsBackPackFull())
		return false;

	CItemPtr pItem = GetItem(idItem);
	if(!pItem || !pItem->IsPileEnable() || nNum >= pItem->GetInt(ITEMDATA_AMOUNT))
		return false;

	CItemPtr pOther = pItem->Split(nNum);
	CHECKF(pOther);
	AddItem(pOther, SYNCHRO_FALSE, true);

	return true;
}

bool CUser::CombineItem		(OBJID idItem, OBJID idOther)
{
	CItemPtr pItem = GetItem(idItem);
	CItemPtr pOther = GetItem(idOther);
	if(!pItem || !pOther || !pItem->IsPileEnable() || pItem->GetInt(ITEMDATA_TYPE) != pOther->GetInt(ITEMDATA_TYPE)
			|| pOther->IsPlunder())
		return false;

	int nNewNum		= pItem->GetInt(ITEMDATA_AMOUNT) + pOther->GetInt(ITEMDATA_AMOUNT);
	if(nNewNum > pItem->GetInt(ITEMDATA_AMOUNTLIMIT))
	{
		pOther->SetInt(ITEMDATA_AMOUNT, nNewNum - pItem->GetInt(ITEMDATA_AMOUNTLIMIT));
		{
			CMsgItem	msg;
			IF_OK(msg.Create(pOther->GetID(), ITEMACT_SYNCHRO_AMOUNT, pOther->GetInt(ITEMDATA_AMOUNT)))
				SendMsg(&msg);
		}
		pItem->SetInt(ITEMDATA_AMOUNT, pItem->GetInt(ITEMDATA_AMOUNTLIMIT));
		{
			CMsgItem	msg;
			IF_OK(msg.Create(pItem->GetID(), ITEMACT_SYNCHRO_AMOUNT, pItem->GetInt(ITEMDATA_AMOUNT)))
				SendMsg(&msg);
		}
	}
	else
	{
		EraseItem(idOther, SYNCHRO_TRUE);
		pItem->SetInt(ITEMDATA_AMOUNT, nNewNum);
		{
			CMsgItem	msg;
			IF_OK(msg.Create(pItem->GetID(), ITEMACT_SYNCHRO_AMOUNT, pItem->GetInt(ITEMDATA_AMOUNT)))
				SendMsg(&msg);
		}
	}

	return true;
}

bool CUser::SpendArrow()
{
	CItem* pArrow = GetEquipItemByPos(ITEMPOSITION_WEAPONL);
	if(!pArrow)
		return false;
	CHECKF(pArrow->IsArrow());

	if(pArrow->GetInt(ITEMDATA_AMOUNT) > 1)
	{
		pArrow->SetInt(ITEMDATA_AMOUNT, pArrow->GetInt(ITEMDATA_AMOUNT) - 1);
		{
			CMsgItem	msg;
			IF_OK(msg.Create(pArrow->GetID(), ITEMACT_SYNCHRO_AMOUNT, pArrow->GetInt(ITEMDATA_AMOUNT)))
				SendMsg(&msg);
		}
	}
	else
		EraseEquip(ITEMPOSITION_WEAPONL, SYNCHRO_TRUE);

	return true;
}

bool CUser::SpendEquipItem(int nSubType, int nNum, bool bSynchro)
{
	CHECKF(nNum > 0);

	CItem* pItem = NULL;
	if (m_pWeaponR && m_pWeaponR->GetItemSubType() == nSubType && m_pWeaponR->GetInt(ITEMDATA_AMOUNT) >= nNum)
		pItem = m_pWeaponR;
	else if (m_pWeaponL && m_pWeaponL->GetItemSubType() == nSubType && m_pWeaponL->GetInt(ITEMDATA_AMOUNT) >= nNum)
		pItem = m_pWeaponL;
	IF_NOT(pItem)
		return false;

	MYASSERT(pItem->IsExpend() && pItem->GetInt(ITEMDATA_AMOUNT) >= nNum);

	if(pItem->IsExpend() && pItem->GetInt(ITEMDATA_AMOUNT) > nNum)
	{
		pItem->SetInt(ITEMDATA_AMOUNT, pItem->GetInt(ITEMDATA_AMOUNT) - nNum);

		if(bSynchro)
		{
			CMsgItem	msg;
			IF_OK(msg.Create(pItem->GetID(), ITEMACT_SYNCHRO_AMOUNT, pItem->GetInt(ITEMDATA_AMOUNT)))
				SendMsg(&msg);
		}
	}
	else
	{
		if (pItem->IsNonsuchItem())
		{
			::MyLogSave("gmlog/SpendEquipItem", "%s(%u) Spend item:[id=%u, type=%u], dur=%d, max_dur=%d",
					this->GetName(),
					this->GetID(),
					pItem->GetID(),
					pItem->GetInt(ITEMDATA_TYPE),
					pItem->GetInt(ITEMDATA_AMOUNT),
					pItem->GetInt(ITEMDATA_AMOUNTLIMIT));
		}

		this->EraseEquip(pItem->GetInt(ITEMDATA_POSITION), bSynchro);
	}

	return true;
}

const char*	g_szItemUpLog	= "gmlog/ItemUp.log";

bool CUser::RepairItem		(OBJID idItem, bool bSynchro, bool bEquip)
{
	CItemPtr pItem = GetItem(idItem);
	if(!pItem)
	{
		if(!bEquip)
			return false;
		pItem = GetEquipItem(idItem);
		if(!pItem)
			return false;
	}

	if(!GetMap()->QueryNpcByType(GetPosX(), GetPosY(), 1, 20))
		return false;
	if(pItem->GetInt(ITEMDATA_REPAIR_MODE) != 0)
		return false;

	if(!( pItem->IsRepairEnable() && pItem->GetInt(ITEMDATA_AMOUNTLIMIT) > pItem->GetInt(ITEMDATA_AMOUNT) ))
	{
		return false;
	}

	int nMoney		= pItem->CalcRepairMoney();
	if(pItem->GetInt(ITEMDATA_AMOUNT) == 0)
	{
		if(!MultiDelItem(1088001, 1088001, 5))
		{
			this->SendSysMsg(STR_NOSSTAR_REPAIR);
			return false;
		}
	}
	else
	{
		int	nRepair		= pItem->GetInt(ITEMDATA_AMOUNTLIMIT) - pItem->GetInt(ITEMDATA_AMOUNT);
		if(nMoney <= 0 || nRepair <= 0)
			return true;

		if (!this->SpendMoney(nMoney, SYNCHRO_TRUE))
			return false;
	}

	switch(CItem::GetItemSort(pItem->GetInt(ITEMDATA_TYPE)))
	{
	case 1:
	case 2:
	case 3:
	case 6:
	case 8:
		if(pItem->GetInt(ITEMDATA_AMOUNT) < 150)
		{
			int nDec	= pItem->GetInt(ITEMDATA_AMOUNTLIMIT) * (pItem->GetInt(ITEMDATA_AMOUNT)*2/3) / 150;
			nDec		= __min(nDec, pItem->GetInt(ITEMDATA_AMOUNTLIMIT)/5);
			int nLimit	= pItem->GetInt(ITEMDATA_AMOUNTLIMIT) - nDec;
			if(nLimit < 100)
				nLimit	= 100;
			pItem->SetInt(ITEMDATA_AMOUNTLIMIT, nLimit);
		}
		break;
	case 4:
	case 5:
	case 7:
		if(pItem->GetInt(ITEMDATA_AMOUNT) < 190)
		{
			int nDec	= pItem->GetInt(ITEMDATA_AMOUNTLIMIT) * (pItem->GetInt(ITEMDATA_AMOUNT)*2/3) / 190;
			nDec		= __min(nDec, pItem->GetInt(ITEMDATA_AMOUNTLIMIT)/5);
			int nLimit	= pItem->GetInt(ITEMDATA_AMOUNTLIMIT) - nDec;
			if(nLimit < 100)
				nLimit	= 100;
			pItem->SetInt(ITEMDATA_AMOUNTLIMIT, nLimit);
		}
		break;
	default:
		MYASSERT(!"invalid item sort!");
		return false;
	}

	pItem->SetInt(ITEMDATA_AMOUNT, pItem->GetInt(ITEMDATA_AMOUNTLIMIT), true);

	if(m_dwUnk638 > 0 && pItem->IsEquipment() && pItem->GetInt(ITEMDATA_TYPE)%10 <= 6)
	{
		int nChance = 0;
		if(pItem->GetInt(ITEMDATA_TYPE)%10 <= 5)
			nChance	= MulDiv(nMoney, 1000, 300000);
		else
			nChance	= __max(1, MulDiv(nMoney, 1000, 800000));

		if(::RandGet(1000) < nChance)
		{
			OBJID idOldType	= pItem->GetInt(ITEMDATA_TYPE);
			OBJID idNewType	= ChkUpEqQuality(pItem, false);
			if(idNewType)
			{
				IF_OK(pItem->ChangeType(idNewType))
				{
					::MyLogSave(g_szItemUpLog, "User[%u] up quality Item[%u] from [%u] to [%u] while repair", GetID(), pItem->GetID(), idOldType, idNewType);
					pItem->SaveInfo();

					this->SendSysMsg(_TXTATR_SYSTEM, STR_LUCK_REPAIR_UPQUALITY);

					CMsgName msg;
					IF_OK(msg.Create(NAMEACT_ROLEEFFECT, STR_LUCKY_EFFECT, this->GetID()))
						SendMsg(&msg);
				}
			}
		}
	}

	if (bSynchro)
	{
		CMsgItemInfo msg;
		CHECKF(msg.Create(pItem, ITEMINFO_UPDATE));
		SendMsg(&msg);
	}

	return true;
}

bool CUser::RepairAll		(bool bSynchro)
{

	return false;
}

bool CUser::IdentItem		(OBJID idItem, bool bSynchro)
{
	CHECKF(idItem);
	CItemPtr pItem = GetEquipItem(idItem);
	if(!pItem)
		pItem = GetItem(idItem);
	if(!( pItem && pItem->IsNeedIdent() ))
		return false;

	int	nMoney	= MulDiv(pItem->GetInt(ITEMDATA_PRICE), ITEMIDENT_PERCENT, 100);
	if(nMoney && (int)GetMoney() < nMoney)
		return false;
	if(nMoney)
		this->SpendMoney(nMoney, SYNCHRO_TRUE);

	pItem->SetIdent(0, UPDATE_FALSE);

	if(bSynchro)
	{
		CMsgItemInfo	msg;
		CHECKF(msg.Create(pItem));
		SendMsg(&msg);
	}
	return true;
}

void CUser::EmbedGem (OBJID idItem, OBJID idGem, int nPos)
{
	if(!GetMap()->QueryNpcByType(GetPosX(), GetPosY(), 7, 20))
		return;

	CItem* pItem = this->GetItem(idItem);
	if (!pItem)
		return;

	CItem* pGem	= this->GetItem(idGem);
	if (!pGem || !pGem->IsGem() || pGem->IsPlunder())
		return;

	nPos = __min(2, __max(1, nPos));

	ItemInfoStruct info;
	pItem->GetInfo(&info);
	int nOldBattlePower = GetEquipBattlePower(pItem);

	switch(nPos)
	{
	case 1:
		{
			IF_OK (GEM_HOLE == info.nGem1)
			{
				pItem->SetInt(ITEMDATA_GEM1, pGem->GetInt(ITEMDATA_TYPE)%1000);

				int nDurEffect = pItem->GetGemDurEffect(pGem->GetInt(ITEMDATA_TYPE));
				if (nDurEffect != 0)
				{
					int nDur = pItem->GetInt(ITEMDATA_AMOUNTLIMIT);
					pItem->SetInt(ITEMDATA_AMOUNTLIMIT, nDur*(100+nDurEffect)/100);
				}

				if (pItem->IsNonsuchItem())
				{
					::MyLogSave("gmlog/embed_gem", "user[%u] embed gem[id:%u, type:%u] on socket 1 of item[id:%u, type[%u]",
							GetID(), idGem, pGem->GetInt(ITEMDATA_TYPE), idItem, pItem->GetInt(ITEMDATA_TYPE));
				}

				DEBUG_TRY
				pItem->SaveInfo();
				MYASSERT(EraseItem(idGem, SYNCHRO_TRUE));
				DEBUG_CATCH("EmbedGem2")
			}
		}
		break;

	case 2:
		{
			IF_OK (GEM_HOLE == info.nGem2)
			{
				pItem->SetInt(ITEMDATA_GEM2, pGem->GetInt(ITEMDATA_TYPE)%1000);

				int nDurEffect = pItem->GetGemDurEffect(pGem->GetInt(ITEMDATA_TYPE));
				if (nDurEffect != 0)
				{
					int nDur = pItem->GetInt(ITEMDATA_AMOUNTLIMIT);
					pItem->SetInt(ITEMDATA_AMOUNTLIMIT, nDur*(100+nDurEffect)/100);
				}

				if (pItem->IsNonsuchItem())
				{
					::MyLogSave("gmlog/embed_gem", "user[%u] embed gem[id:%u, type:%u] on socket 2 of item[id:%u, type[%u]",
							GetID(), idGem, pGem->GetInt(ITEMDATA_TYPE), idItem, pItem->GetInt(ITEMDATA_TYPE));
				}

				DEBUG_TRY
				pItem->SaveInfo();
				MYASSERT(EraseItem(idGem, SYNCHRO_TRUE));
				DEBUG_CATCH("EmbedGem2")
			}
		}
		break;

	default:
		{
			::LogSave("invalid pos:%u in gem embed.", nPos);
			return;
		}
		break;
	}

	UpdateBattlePower();

	CMsgItemInfo msg;
	IF_OK (msg.Create(pItem, ITEMINFO_UPDATE))
		this->SendMsg(&msg);
}

void CUser::TakeOutGem (OBJID idItem, int nPos)
{
	if(!GetMap()->QueryNpcByType(GetPosX(), GetPosY(), 7, 20))
		return;

	CItem* pItem = this->GetItem(idItem);
	if (!pItem || pItem->IsPlunder())
		return;

	nPos = __min(2, __max(1, nPos));

	ItemInfoStruct info;
	pItem->GetInfo(&info);
	int nOldBattlePower = GetEquipBattlePower(pItem);

	switch(nPos)
	{
	case 1:
		{
			if (GEM_NONE != info.nGem1 && GEM_HOLE != info.nGem1)
			{
				pItem->SetInt(ITEMDATA_GEM1, GEM_HOLE);

				OBJID idGemType = 700000+info.nGem1;
				int nDurEffect = pItem->GetGemDurEffect(idGemType);
				if (nDurEffect != 0)
				{
					int nDur = pItem->GetInt(ITEMDATA_AMOUNTLIMIT);
					pItem->SetInt(ITEMDATA_AMOUNTLIMIT, __max(0, nDur*100/(100+nDurEffect)));

					if (pItem->GetInt(ITEMDATA_AMOUNT) > pItem->GetInt(ITEMDATA_AMOUNTLIMIT))
						pItem->SetInt(ITEMDATA_AMOUNT, pItem->GetInt(ITEMDATA_AMOUNTLIMIT));
				}

				pItem->SaveInfo();
			}
		}
		break;

	case 2:
		{
			if (GEM_NONE != info.nGem2 && GEM_HOLE != info.nGem2)
			{
				pItem->SetInt(ITEMDATA_GEM2, GEM_HOLE);

				OBJID idGemType = 700000+info.nGem2;
				int nDurEffect = pItem->GetGemDurEffect(idGemType);
				if (nDurEffect != 0)
				{
					int nDur = pItem->GetInt(ITEMDATA_AMOUNTLIMIT);
					pItem->SetInt(ITEMDATA_AMOUNTLIMIT, __max(0, nDur*100/(100+nDurEffect)));

					if (pItem->GetInt(ITEMDATA_AMOUNT) > pItem->GetInt(ITEMDATA_AMOUNTLIMIT))
						pItem->SetInt(ITEMDATA_AMOUNT, pItem->GetInt(ITEMDATA_AMOUNTLIMIT));
				}

				pItem->SaveInfo();
			}
		}
		break;

	default:
		{
			::LogSave("invalid pos:%u in gem take out.", nPos);
			return;
		}
		break;
	}

	UpdateBattlePower();

	CMsgItemInfo msg;
	IF_OK (msg.Create(pItem, ITEMINFO_UPDATE))
		this->SendMsg(&msg);
}

void CUser::UpdateEquipmentDurability(void)
{
	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if (pEquip && pEquip->GetInt(ITEMDATA_FRAY_MODE) == 1)
			this->AddEquipmentDurability(i, -1);
	}
}

void CUser::AddEquipmentDurability(int nPosition, int nAddValue)
{
	if (nPosition == ITEMPOSITION_MOUNT)
		return;

	CItemPtr& refpEquip = this->GetEquipItemRef(nPosition);
	if (!refpEquip || refpEquip->IsExpend())
		return;

	int nOldDur	= refpEquip->GetInt(ITEMDATA_AMOUNT);
	int nDurability = __max(0, refpEquip->GetInt(ITEMDATA_AMOUNT)+nAddValue);
	refpEquip->SetInt(ITEMDATA_AMOUNT, nDurability);

	if (refpEquip->GetInt(ITEMDATA_REPAIR_MODE) == 0 && nDurability < 200)
	{
		if (nDurability < 100)
		{
			if ((nDurability%10) == 0 && nDurability > 0)
				this->SendSysMsg(_TXTATR_GM, STR_DAMAGED_REPAIR, refpEquip->GetStr(ITEMDATA_NAME));
		}
		else
		{
			if ((nDurability%20) == 0)
				this->SendSysMsg(_TXTATR_GM, STR_DURABILITY_REPAIR, refpEquip->GetStr(ITEMDATA_NAME));
		}
	}

	if (nOldDur/100 != nDurability/100
			|| nDurability <= 0)
	{
		CMsgItem msg;
		if (msg.Create(nPosition, ITEMACT_DURABILITY, nDurability))
			this->SendMsg(&msg);

		if (nDurability == 0)
		{
			if (nOldDur > 0)
			{
				UpdateBattlePower();
				::MyLogSave("gmlog/wareout_item", "%s(%u) wear out item:[id=%u, type=%u], dur=%d, max_dur=%d",
						this->GetName(),
						this->GetID(),
						refpEquip->GetID(),
						refpEquip->GetInt(ITEMDATA_TYPE),
						refpEquip->GetInt(ITEMDATA_AMOUNT),
						refpEquip->GetInt(ITEMDATA_AMOUNTLIMIT));
			}

			CMsgPlayer msgPlayer;
			if (msgPlayer.Create(this->QueryRole()))
				this->BroadcastRoomMsg(&msgPlayer, EXCLUDE_SELF);
		}
		else
			refpEquip->SaveInfo();
	}
}

int CUser::GetWeight()
{
	if(m_bUpdateWeight)
	{
		m_nAllWeight = 0;
		if(m_pHelmet)
			m_nAllWeight	+= m_pHelmet->GetWeight();
		if(m_pNecklace)
			m_nAllWeight	+= m_pNecklace->GetWeight();
		if(m_pArmor)
			m_nAllWeight	+= m_pArmor->GetWeight();
		if(m_pWeaponR)
			m_nAllWeight	+= m_pWeaponR->GetWeight();
		if(m_pWeaponL)
			m_nAllWeight	+= m_pWeaponL->GetWeight();
		if(m_pRingR)
			m_nAllWeight	+= m_pRingR->GetWeight();
		if(m_pRingL)
			m_nAllWeight	+= m_pRingL->GetWeight();
		if(m_pShoes)
			m_nAllWeight	+= m_pShoes->GetWeight();
		if(m_pOvercoat)
			m_nAllWeight	+= m_pOvercoat->GetWeight();

		for(int i = 0; i < (int)m_setItem.size(); i++)
		{
			if(m_setItem[i])
				m_nAllWeight	+= m_setItem[i]->GetWeight();
		}

		m_bUpdateWeight = false;
	}

	return m_nAllWeight;
}

void CUser::SetItemLockDate(OBJID idItem, DWORD dwTimeout)
{
	m_mapItemLockDate[idItem] = dwTimeout;
}

void CUser::UpdateGhostContract(CItem* pItem, OBJID idOwner)
{
	if(pItem)
	{
		char szSQL[1024] = "";
		_snprintf(szSQL, 1024, "UPDATE GhostContract SET owner_id = %u WHERE Item_id = %u LIMIT 1", idOwner, pItem->GetID());
		Database()->ExecuteSQL(szSQL);
	}
}

bool CUser::PickMapItem(OBJID idItem)
{
	CMapItem* pMapItem = MapManager()->QueryMapItem(idItem);
	if (!pMapItem)
		return false;

	if (this->GetDistance(pMapItem->QueryMapThing()) > PICKMAPITEMDIST_LIMIT)
	{
		this->SendSysMsg(STR_FAR_CANNOT_PICK);
		return false;
	}

	if (this->IsBackPackFull() && !pMapItem->IsMoney())
	{
		this->SendSysMsg(STR_FULL_CANNOT_PICK);
		return false;
	}

	OBJID idOwner = pMapItem->GetPlayerID();
	CUser* pOwner = UserManager()->GetUser(idOwner);
	if (pMapItem->IsPriv() && idOwner != GetID())
	{
		if (!(pOwner && this->IsMate(pOwner)))
		{
			CTeam* pTeam = this->GetTeam();
			if (pTeam && pTeam->IsTeamMember(idOwner)
					&& (pMapItem->IsMoney() && !pTeam->IsCloseMoney() || !pMapItem->IsMoney() && !pTeam->IsCloseItem())
					&& !(pMapItem->GetType() == TYPE_DRAGONBALL || pMapItem->GetType() == TYPE_SHOOTINGSTAR) )
			{

			}
			else
			{
				SendSysMsg(STR_OTHERS_ITEM);
				return false;
			}
		}
	}

	if (pMapItem->IsMoney())
	{
		int	nMoney = pMapItem->GetAmount();
		MapManager()->QueryMapItemSet()->DelObj(pMapItem->GetID());

		if (this->GetLev() < 70 && GetID() != idOwner && pOwner)
		{
			CTeam* pTeam = this->GetTeam();
			if (pTeam && pTeam->IsTeamMember(idOwner) && pOwner->GetLev() >= 70 && pOwner->GetLev() >= this->GetLev() + 20)
			{
				int nShareMoney = nMoney * 20 / 100;
				if (nShareMoney > 0)
				{
					pOwner->GainMoney(nShareMoney, SYNCHRO_TRUE);
					CMsgTeamAward msg;
					IF_OK(msg.Create(0, nShareMoney, 0))
						pOwner->SendMsg(&msg);

					char szMsg[1024];
					sprintf(szMsg, STR_AWARD_TEAM_MONEY, nShareMoney);
					CMsgTalk msg2;
					IF_OK(msg2.Create(_TXTATR_SYSTEM, szMsg))
						pTeam->BroadcastTeamMsg(&msg2, NULL);

					nMoney -= nShareMoney;
				}
			}
		}

		GainMoney(nMoney, SYNCHRO_TRUE);

		CMsgTeamAward msg;
		IF_OK(msg.Create(0, nMoney, 0))
			this->SendMsg(&msg);

		if(nMoney >= PICK_MORE_MONEY)
		{
			CMsgAction	msg;
			IF_OK(msg.Create(GetID(), GetPosX(), GetPosY(), GetDir(), actionGetMoney, nMoney))
				SendMsg(&msg);
		}

		this->SendSysMsg(STR_PICK_MONEY, nMoney);
		if(nMoney >= 100000)
			::MyLogSave("gmlog/pick_item", "%s(%u) pick up money: %d", this->GetName(), this->GetID(), nMoney);
	}
	else
	{
		CItemPtr pItem = CItem::CreateNew();
		CHECKF(pItem);

		ItemInfoStruct info;
		memcpy(&info, pMapItem->GetInfo(), sizeof(info));
		info.idOwner	= GetID();
		info.idPlayer	= GetID();
		info.nPosition	= ITEMPOSITION_BACKPACK;
		if(!pItem->Create(GameDataDefault()->GetGameItemData(), &info, true, info.id))
		{
			pItem->ReleaseByOwner();
			MYASSERT(!"Create");
			this->SendSysMsg(STR_FAILED_GENERATE_ITEM);
			return false;
		}

		MapManager()->QueryMapItemSet()->DelObj(pMapItem->GetID());
		this->AddItem(pItem, SYNCHRO_TRUE, true);

		if (pItem->IsNonsuchItem())
		{
			::MyLogSave("gmlog/pick_item", "%s(%u) pick item:[id=%u, type=%u], dur=%d, max_dur=%d",
					this->GetName(),
					this->GetID(),
					pItem->GetID(),
					pItem->GetInt(ITEMDATA_TYPE),
					pItem->GetInt(ITEMDATA_AMOUNT),
					pItem->GetInt(ITEMDATA_AMOUNTLIMIT));
		}

		this->SendSysMsg(STR_GOT_ITEM, pItem->GetStr(ITEMDATA_NAME));

		OBJID	setPickActionType[] = { 790001 };
		for(int i = 0; i < (int)(sizeof(setPickActionType) / sizeof(OBJID)); i++)
		{
			if(setPickActionType[i] == (OBJID)pItem->GetInt(ITEMDATA_TYPE))
			{
				GameAction()->ProcessAction(80000002, this, this, pItem);
				break;
			}
		}

		if(pItem->GetInt(ITEMDATA_TYPE) == 1088000)
		{
			SQLBUF	szSQL = "";
			sprintf(szSQL, "INSERT INTO item_log SET item_id=%u,item_type=%u,owner_id=%u,time=%u,event=%u", pItem->GetID(), pItem->GetInt(ITEMDATA_TYPE), GetID(), ::TimeGetMinute(), 21);
			Database()->ExecuteAsyncSQL(szSQL);
		}
		else if(pItem->IsGem() && (pItem->GetInt(ITEMDATA_TYPE) & 0x0F) == 3)
		{
			SQLBUF	szSQL = "";
			sprintf(szSQL, "INSERT INTO item_log SET item_id=%u,item_type=%u,owner_id=%u,time=%u,event=%u", pItem->GetID(), pItem->GetInt(ITEMDATA_TYPE), GetID(), ::TimeGetMinute(), 1);
			Database()->ExecuteAsyncSQL(szSQL);
		}
		else if(pItem->IsEquipment() && (pItem->GetInt(ITEMDATA_GEM1) != 0 || pItem->GetInt(ITEMDATA_GEM2) != 0))
		{
			SQLBUF	szSQL = "";
			sprintf(szSQL, "INSERT INTO item_log SET item_id=%u,item_type=%u,owner_id=%u,time=%u,event=%u", pItem->GetID(), pItem->GetInt(ITEMDATA_TYPE), GetID(), ::TimeGetMinute(), 11);
			Database()->ExecuteAsyncSQL(szSQL);
		}
	}

	CMsgMapItem msg;
	IF_OK (msg.Create(this->GetID(), this->GetPosX(), this->GetPosY(), MSGMAPITEM_PICK))
		this->BroadcastRoomMsg(&msg, EXCLUDE_SELF);

	return true;
}

bool CUser::CreateSquareDeal(CUser* pTarget)
{
	IF_NOT(pTarget)
		return false;

	ISquareDeal* pSquareDeal = CSquareDeal::CreateNew();
	CHECKF(pSquareDeal);

	CMsgUserAttrib	msgTarget;
	CMsgUserAttrib	msgUser;
	msgTarget.Create(GetID(), _USERATTRIB_LEV, GetLev());
	pTarget->SendMsg(&msgTarget);
	msgUser.Create(pTarget->GetID(), _USERATTRIB_LEV, pTarget->GetLev());
	this->SendMsg(&msgUser);

	if (pSquareDeal->Create(&m_ifaceSquareDeal, &pTarget->m_ifaceSquareDeal))
		return true;

	pSquareDeal->Release();
	return false;
}

void CUser::XUserSquareDeal::CheckDistance()
{
	if(!m_pSquareDeal)
		return ;

	if(m_pSquareDeal->GetTarget()->GetMapID() == This()->GetMapID()
			&& m_pSquareDeal->GetTarget()->GetDistance(This()->GetPosX(), This()->GetPosY()) <= CELLS_PER_VIEW)
		return ;

	m_pSquareDeal->Release();
	return ;
}

bool CUser::XUserSquareDeal::Create(ISquareDeal* pSquareDeal)
{
	if(m_pSquareDeal)
		return false;

	m_pSquareDeal = pSquareDeal;

	CMsgTrade	msg;
	CHECKF(msg.Create(_TRADE_OPEN, m_pSquareDeal->GetTarget()->GetID()));
	This()->SendMsg(&msg);

	return true;
}

ULONG CUser::XUserSquareDeal::Release()
{
	CHECKF(m_pSquareDeal);

	m_pSquareDeal = NULL;
	return 1;
}

bool CUser::XUserSquareDeal::AddItem(CItem* pItem, int nAmount, int nAllWeight)
{
	CHECKF(m_pSquareDeal);

	if(!This()->IsBackPackSpare(nAmount, nAllWeight))
		return false;
	if(!pItem->IsExchangeEnable())
		return false;

	CMsgItemInfo	msg;
	CHECKF(msg.Create(pItem, ITEMINFO_TRADE));
	This()->SendMsg(&msg);

	return true;
}

bool CUser::XUserSquareDeal::AddMoney(int nAllMoney)
{
	CHECKF(m_pSquareDeal);

	if(This()->GetMoney() + nAllMoney > _MAX_MONEYLIMIT)
		return false;

	CMsgTrade	msg;
	CHECKF(msg.Create(_TRADE_MONEYALL, nAllMoney));
	This()->SendMsg(&msg);

	return true;
}

bool CUser::XUserSquareDeal::AddEmoney(int nAllEmoney)
{
	CHECKF(m_pSquareDeal);

	if(This()->GetEmoney() + nAllEmoney > _MAX_MONEYLIMIT)
		return false;

	CMsgTrade	msg;
	CHECKF(msg.Create(_TRADE_EMONEYALL, nAllEmoney));
	This()->SendMsg(&msg);

	return true;
}

void CUser::XUserSquareDeal::ClickOK()
{
	CHECK(m_pSquareDeal);

	CMsgTrade	msg;
	CHECK(msg.Create(_TRADE_OK, 0));
	This()->SendMsg(&msg);

	return ;
}

OBJID CUser::XUserSquareDeal::GetAccountID()		{ return This()->m_data.m_Info.idAccount; }
DWORD CUser::XUserSquareDeal::GetEmoney()			{ return This()->GetEmoney(); }
DWORD CUser::XUserSquareDeal::GetMoney()			{ return This()->GetMoney(); }
LPCTSTR CUser::XUserSquareDeal::GetName()			{ return This()->GetName(); }

bool CUser::SendStorageInfo(OBJID idNpc)
{
	if(!m_pStorage)
		m_pStorage	= CPackage::CreateNew();
	CHECKF(m_pStorage);
	CHECKF(m_pStorage->Create(ITEMPOSITION_STORAGE, idNpc, GetID(), Database()));
	return m_pStorage->SendInfo(this);
}

bool CUser::CheckIn(OBJID idNpc, CItem* pItem, bool bUpdate )
{
	if(!m_pStorage)
		m_pStorage	= CPackage::CreateNew();
	CHECKF(m_pStorage);
	CHECKF(m_pStorage->Create(ITEMPOSITION_STORAGE, idNpc, GetID(), Database()));
	return m_pStorage->CheckIn(this, pItem, bUpdate);
}

CItem* CUser::CheckOut(OBJID idNpc, OBJID idItem, bool bUpdate)
{
	CHECKF(m_pStorage);
	CHECKF(m_pStorage->Create(ITEMPOSITION_STORAGE, idNpc, GetID(), Database()));
	return m_pStorage->CheckOut(this, idItem, bUpdate);
}

bool CUser::IsChestFull(CItem* pItemPack)
{
	IF_NOT(pItemPack && pItemPack->IsTreasurePack())
		return true;

	if(!m_pStorage)
		m_pStorage	= CPackage::CreateNew();
	IF_NOT(m_pStorage)
		return true;

	IF_NOT(m_pStorage->Create(ITEMPOSITION_CHEST, pItemPack->GetID(), this->GetID(), Database()))
		return true;

	int nMaxAmount = 3;
	switch(pItemPack->GetInt(ITEMDATA_TYPE))
	{
	case 1100006:	nMaxAmount = 6;		break;
	case 1100009:	nMaxAmount = 9;		break;
	}
	return m_pStorage->GetAmount() >= nMaxAmount;
}

void CUser::SendChestInfo(OBJID idPack)
{
	CItem* pPack = GetItem(idPack);
	if(!pPack)
		return;
	IF_NOT(pPack->IsTreasurePack())
		return;

	if(!m_pStorage)
		m_pStorage	= CPackage::CreateNew();
	IF_NOT(m_pStorage)
		return;

	IF_NOT(m_pStorage->Create(ITEMPOSITION_CHEST, idPack, this->GetID(), Database()))
		return;

	m_pStorage->SendInfo(this);
}

void CUser::CheckInChest(OBJID idPack, OBJID idItem)
{
	CItem* pPack = GetItem(idPack);
	if(!pPack)
		return;
	IF_NOT(pPack->IsTreasurePack())
		return;

	CItem* pItem = GetItem(idItem);
	if(!pItem)
		return;

	if(pItem->IsStorageEnable() && !pItem->IsTreasurePack() && pItem->IsExchangeEnable())
	{
		if(!m_pStorage)
			m_pStorage	= CPackage::CreateNew();
		IF_NOT(m_pStorage)
			return;

		IF_NOT(m_pStorage->Create(ITEMPOSITION_CHEST, idPack, this->GetID(), Database()))
			return;

		if(IsChestFull(pPack))
		{
			SendSysMsg("Too many item.");
			return;
		}

		PopItem(idItem, SYNCHRO_TRUE, true);
		m_pStorage->CheckIn(this, pItem, true);
	}
	else
		SendSysMsg(STR_CANT_STORAGE);
}

void CUser::CheckOutChest(OBJID idPack, OBJID idItem)
{
	if(m_setItem.size() + 1 > 40)
		return;

	CItem* pPack = GetItem(idPack);
	if(!pPack)
		return;
	IF_NOT(pPack->IsTreasurePack())
		return;

	if(!m_pStorage)
		m_pStorage	= CPackage::CreateNew();
	IF_NOT(m_pStorage)
		return;

	IF_NOT(m_pStorage->Create(ITEMPOSITION_CHEST, idPack, this->GetID(), Database()))
		return;

	CItem* pItem = m_pStorage->CheckOut(this, idItem, true);
	if(pItem)
		AddItem(pItem, SYNCHRO_TRUE, true);
}

bool CUser::ProcessTargetRole(OBJID idNpc)
{
	void* pObj = NULL;
	if(GetMap()->QueryObj(GetPosX(), GetPosY(), OBJ_NPC, idNpc, &pObj))
		return QueryStorage()->SendStorageInfo(idNpc);
	return false;
}

OBJID CUser::ChkUpEqQuality	(CItem* pItem, bool bSendHint )
{
	if (!(pItem && pItem->IsEquipment()))
		return ID_NONE;

	if (pItem->GetInt(ITEMDATA_AMOUNT)/100 < pItem->GetInt(ITEMDATA_AMOUNTLIMIT)/100)
	{
		if (bSendHint)
			this->SendSysMsg(STR_REPAIR_THEN_IMPROVE);

		return ID_NONE;
	}

	int nQuality = pItem->GetQuality();
	if (nQuality < 3 || nQuality >= 9)
	{
		if (bSendHint)
			this->SendSysMsg(STR_SUPERIOR_ALREADY);

		return ID_NONE;
	}

	OBJID idType = pItem->GetInt(ITEMDATA_TYPE);
	if (nQuality < 5)
		idType		= (idType/10)*10+5;

	OBJID idNewType = idType+1;
	if (!ItemType()->QueryItemType(idNewType))
	{
		if (bSendHint)
			this->SendSysMsg(STR_CONNOT_IMPROVE);

		return ID_NONE;
	}

	return idNewType;
}

OBJID CUser::ChkUpEqLevel	(CItem* pItem, bool bSendHint )
{
	if (!(pItem && pItem->IsEquipment()))
		return ID_NONE;

	int nSubType = pItem->GetItemSubType();
	if (pItem->IsHeadOrBodyLev10() || nSubType == 112 || (nSubType >= 135 && nSubType <= 139))
	{
		if (bSendHint)
			this->SendSysMsg(STR_UPGRADE_NOMORE);

		return ID_NONE;
	}

	int nQuality = pItem->GetQuality();
	if (nQuality < 3)
		return ID_NONE;

	if (pItem->GetInt(ITEMDATA_AMOUNT)/100 < pItem->GetInt(ITEMDATA_AMOUNTLIMIT)/100)
	{
		if (bSendHint)
			this->SendSysMsg(STR_REPAIR_THEN_UPGRADE);

		return ID_NONE;
	}

	OBJID idType	= pItem->GetInt(ITEMDATA_TYPE);
	OBJID idNewType = idType;
	bool bColour = pItem->IsShield() || pItem->IsArmor() || pItem->IsHelmet();
	if (bColour && (idType/10)%10 >= 9)
	{
		if (pItem->IsArmor())
			idNewType += 5000;
		else if (pItem->IsHelmet())
		{
			int nThousand = (idType/1000)%10;
			idNewType = idNewType - nThousand*1000 + 2000 - ((idType/10)%10)*10 + nThousand*10;
		}
		if (idType != idNewType && ItemType()->QueryItemType(idNewType))
			return idNewType;

		if (bSendHint)
			this->SendSysMsg(STR_UPGRADE_NOMORE);
		return ID_NONE;
	}
	while (true)
	{
		idNewType += 10;

		if (ItemType()->QueryItemType(idNewType))
			break;

		if ((idNewType%1000)/10 >= 99 || (bColour && (idNewType/10)%10 >= 9))
		{
			if (bSendHint)
				this->SendSysMsg(STR_UPGRADE_NOMORE);

			return ID_NONE;
		}
	}

	return idNewType;
}

bool CUser::GetUpEpQualityInfo	(CItem* pItem, int& nChance, OBJID& idNewType, bool bSendHint )
{
	idNewType = this->ChkUpEqQuality(pItem, bSendHint);
	if (ID_NONE == idNewType)
		return false;

	nChance = 100;
	switch(pItem->GetQuality())
	{
	case 6:
		nChance = 50;
		break;

	case 7:
		nChance = 33;
		break;

	case 8:
		nChance = 20;
		break;

	default:
		nChance = 100;
		break;
	}

	int nFactor = pItem->GetInt(ITEMDATA_REQ_LEVEL);
	if (nFactor > 70 )
		nChance = nChance*(100-(nFactor-70)*1.0)*0.01;

	nChance = __max(1, nChance);
	return true;
}

bool CUser::GetUpEpLevelInfo	(CItem* pItem, int& nChance, OBJID& idNewType, bool bSendHint )
{
	idNewType = this->ChkUpEqLevel(pItem, bSendHint);
	if (ID_NONE == idNewType)
		return false;

	if (pItem->IsHeadOrBodyLev10())
		return false;

	OBJID idType = pItem->GetInt(ITEMDATA_TYPE);

	nChance = 100;
	if (pItem->IsShield() || pItem->IsArmor() || pItem->IsHelmet())
	{
		int nLev = (idType%100)/10;
		switch(nLev)
		{
		case 5:
			nChance = 50;
			break;

		case 6:
			nChance = 40;
			break;

		case 7:
			nChance = 30;
			break;

		case 8:
		case 9:
			nChance = 20;
			break;

		default:
			nChance = 500;
			break;
		}

		int nQuality = idType%10;
		switch(nQuality)
		{
		case 6:
			nChance = nChance*90/100;
			break;

		case 7:
			nChance = nChance*70/100;
			break;

		case 8:
			nChance = nChance*30/100;
			break;

		case 9:
			nChance = nChance*10/100;
			break;

		default:
			break;
		}
	}
	else
	{
		int nLev = (idType%1000)/10;
		switch(nLev)
		{
		case 11:
			nChance = 95;
			break;

		case 12:
			nChance = 90;
			break;

		case 13:
			nChance = 85;
			break;

		case 14:
			nChance = 80;
			break;

		case 15:
			nChance = 75;
			break;

		case 16:
			nChance = 70;
			break;

		case 17:
			nChance = 65;
			break;

		case 18:
			nChance = 60;
			break;

		case 19:
			nChance = 55;
			break;

		case 20:
			nChance = 50;
			break;

		case 21:
			nChance = 45;
			break;

		case 22:
			nChance = 40;
			break;

		default:
			nChance = 500;
			break;
		}

		int nQuality = idType%10;
		switch(nQuality)
		{
		case 6:
			nChance = nChance*90/100;
			break;

		case 7:
			nChance = nChance*70/100;
			break;

		case 8:
			nChance = nChance*30/100;
			break;

		case 9:
			nChance = nChance*10/100;
			break;

		default:
			break;
		}

		if (nLev >= 22)
			return false;
	}

	nChance = __min(100, __max(1, nChance));
	return true;
}

int	CUser::GetChanceUpEpQuality	(int nPosition, bool bSendHint )
{
	CItem* pItem = GetEquipItemByPos(nPosition);
	if (!pItem)
		return -1;

	OBJID idNewType = ID_NONE;
	int nChance = 0;

	if (this->GetUpEpQualityInfo(pItem, nChance, idNewType, bSendHint))
		return nChance;
	else
		return -1;
}

int CUser::GetChanceUpEpLevel	(int nPosition, bool bSendHint )
{
	CItem* pItem = GetEquipItemByPos(nPosition);
	if (!pItem)
		return -1;

	OBJID idNewType = ID_NONE;
	int nChance = 0;

	if (this->GetUpEpLevelInfo(pItem, nChance, idNewType, bSendHint))
		return nChance;
	else
		return -1;
}

bool CUser::UpEquipmentQuality		(int nPosition, bool bFree)
{
	CItem* pItem = GetEquipItemByPos(nPosition);
	if (!pItem)
		return false;

	if (pItem->IsNeedRepair())
		return false;

	OBJID idNewType = ID_NONE;
	int nChance = 0;

	if (!this->GetUpEpQualityInfo(pItem, nChance, idNewType, true))
		return false;
	int nGemCost = 100/nChance + 1;
	int nOldBattlePower = GetEquipBattlePower(pItem);
	if (!bFree && !this->MultiCheckItem(TYPE_DRAGONBALL, TYPE_DRAGONBALL, nGemCost))
		return false;

	{
		OBJID idOldType = pItem->GetInt(ITEMDATA_TYPE);
		IF_NOT (pItem->ChangeType(idNewType))
			return false;
		::MyLogSave(g_szItemUpLog, "User[%u] up quality Item[%u] from [%u] to [%u] at cost %d Treasure[%u]", GetID(), pItem->GetID(), idOldType, idNewType, nGemCost, TYPE_DRAGONBALL);

		if (!bFree)
			this->MultiDelItem(TYPE_DRAGONBALL, TYPE_DRAGONBALL, nGemCost);

		int nAmountLimit = pItem->GetInt(ITEMDATA_AMOUNTLIMIT_ORIGINAL);
		if (::RandGet(100) < 2)
			nAmountLimit += (::RandGet(3)+1)*100;

		nAmountLimit = nAmountLimit*(100+pItem->GetGemDurEffect())/100;

		pItem->SetInt(ITEMDATA_AMOUNTLIMIT, nAmountLimit, UPDATE_FALSE);
		pItem->SetInt(ITEMDATA_AMOUNT, nAmountLimit, UPDATE_FALSE);

		if (::RandGet(300) < 1 && !bFree)
		{
			if (GEM_NONE == pItem->GetInt(ITEMDATA_GEM1))
			{
				pItem->SetInt(ITEMDATA_GEM1, GEM_HOLE, UPDATE_FALSE);

				char szMsg[1024] = "";
				sprintf(szMsg, STR_UPLEV_HOLE1, this->GetName(), pItem->GetStr(ITEMDATA_NAME));
				this->SendSystemBBS(szMsg);
				::MyLogSave(g_szItemUpLog, "Item[%u] got No.1 GemHole.", pItem->GetID());
			}
			else if (GEM_NONE == pItem->GetInt(ITEMDATA_GEM2))
			{
				pItem->SetInt(ITEMDATA_GEM2, GEM_HOLE, UPDATE_FALSE);

				char szMsg[1024] = "";
				sprintf(szMsg, STR_UPLEV_HOLE2, this->GetName(), pItem->GetStr(ITEMDATA_NAME));
				this->SendSystemBBS(szMsg);
				::MyLogSave(g_szItemUpLog, "Item[%u] got No.2 GemHole.", pItem->GetID());
			}
		}

		pItem->SaveInfo();

		CMsgItemInfoEx msg;
		IF_OK (msg.Create(pItem, this->GetID(), 0, ITEMINFOEX_EQUIPMENT))
			this->BroadcastRoomMsg(&msg, true);
	}
	UpdateBattlePower();

	return true;
}

void CUser::UpEquipmentQuality	(OBJID idEquipment, OBJID idTreasure)
{
	if(!GetMap()->QueryNpcByType(GetPosX(), GetPosY(), 6, 20))
		return;

	CItem* pTreasure = this->GetItem(idTreasure);
	if (!pTreasure || pTreasure->IsPlunder())
		return;

	if (pTreasure->GetInt(ITEMDATA_TYPE) != TYPE_DRAGONBALL)
	{
		this->SendSysMsg(STR_DRAGONBALL);
		return;
	}

	CItem* pItem = this->GetItem(idEquipment);
	IF_NOT (pItem)
		return;

	if (pItem->IsNeedRepair())
		return;

	if (pItem->GetItemSort() == ITEMSORT_EXTEND)
		return;

	int nQuality = pItem->GetQuality();

	OBJID idNewType = ID_NONE;
	int nChance = 0;

	if (!this->GetUpEpQualityInfo(pItem, nChance, idNewType, true))
		return;

	this->EraseItem(idTreasure, SYNCHRO_TRUE);

	ItemInfoStruct infoItem;
	pItem->GetInfo(&infoItem);
	int nOldBattlePower = GetEquipBattlePower(pItem);

	if (::RandGet(100) < nChance)
	{
		OBJID idOldType = pItem->GetInt(ITEMDATA_TYPE);
		IF_NOT (pItem->ChangeType(idNewType))
		{
			::MsgLogSave(g_szItemUpLog, "item[%u] change type to [%d] failed when upquality, lost treasure: %u.", pItem->GetID(), idNewType, TYPE_DRAGONBALL);
			return;
		}
		::MyLogSave(g_szItemUpLog, "User[%u] up quality Item[%u] from [%u] to [%u] at cost Treasure[%u]", GetID(), pItem->GetID(), idOldType, idNewType, TYPE_DRAGONBALL);

		int nAmountLimit = pItem->GetInt(ITEMDATA_AMOUNTLIMIT_ORIGINAL);
		if (::RandGet(100) < 2)
			nAmountLimit += (::RandGet(3)+1)*100;

		nAmountLimit = nAmountLimit*(100+pItem->GetGemDurEffect())/100;

		pItem->SetInt(ITEMDATA_AMOUNTLIMIT, nAmountLimit, UPDATE_FALSE);
		pItem->SetInt(ITEMDATA_AMOUNT, nAmountLimit, UPDATE_FALSE);

		if (::RandGet(300) < 1)
		{
			if (GEM_NONE == pItem->GetInt(ITEMDATA_GEM1))
			{
				pItem->SetInt(ITEMDATA_GEM1, GEM_HOLE, UPDATE_FALSE);

				char szMsg[1024] = "";
				sprintf(szMsg, STR_UPLEV_HOLE1, this->GetName(), pItem->GetStr(ITEMDATA_NAME));
				this->SendSystemBBS(szMsg);
				::MyLogSave(g_szItemUpLog, "Item[%u] got No.1 GemHole.", pItem->GetID());
			}
			else if (GEM_NONE == pItem->GetInt(ITEMDATA_GEM2))
			{
				pItem->SetInt(ITEMDATA_GEM2, GEM_HOLE, UPDATE_FALSE);

				char szMsg[1024] = "";
				sprintf(szMsg, STR_UPLEV_HOLE1, this->GetName(), pItem->GetStr(ITEMDATA_NAME));
				this->SendSystemBBS(szMsg);
				::MyLogSave(g_szItemUpLog, "Item[%u] got No.2 GemHole.", pItem->GetID());
			}
		}

		pItem->SaveInfo();

		CMsgItemInfo msg;
		IF_OK (msg.Create(pItem, ITEMINFO_UPDATE))
			this->SendMsg(&msg);

		int anLuckChance[3] = { 5, 20, 50 };
		int nOldQuality = idOldType%10;
		if (nOldQuality >= 6 && m_dwUnk638 > 0 && ::RandGet(100) < anLuckChance[nOldQuality-6])
		{
			this->SendSysMsg(_TXTATR_SYSTEM, STR_LUCK_EQP_UPQUALITY);

			CMsgName msg;
			IF_OK(msg.Create(NAMEACT_ROLEEFFECT, STR_LUCKY_EFFECT, this->GetID()))
				SendMsg(&msg);
		}
	}
	else
	{
		if (8 == nQuality)
		{
			{
				int nAmountLimit = pItem->GetInt(ITEMDATA_AMOUNTLIMIT);
				if (::RandGet(100) < PERCENT_DECDUR)
				{
					if (nAmountLimit/100 > 1)
					{
						nAmountLimit -= 100;
						pItem->SetInt(ITEMDATA_AMOUNTLIMIT, nAmountLimit);
					}
				}

				int nAmountNew = nAmountLimit;
				int nOrgAmountLimit = pItem->GetInt(ITEMDATA_AMOUNTLIMIT_ORIGINAL);
				nAmountNew -= nOrgAmountLimit/2;

				if (nAmountNew < 100)
					nAmountNew = 100;

				pItem->SetInt(ITEMDATA_AMOUNT, nAmountNew);

				pItem->SaveInfo();

				CMsgItemInfo msg;
				IF_OK (msg.Create(pItem, ITEMINFO_UPDATE))
					this->SendMsg(&msg);
			}
		}
		else
		{

			int nAmountLimit = pItem->GetInt(ITEMDATA_AMOUNTLIMIT);
			if (::RandGet(100) < PERCENT_DECDUR)
			{
				if (nAmountLimit/100 > 1)
				{
					nAmountLimit -= 100;
					pItem->SetInt(ITEMDATA_AMOUNTLIMIT, nAmountLimit);
				}
			}

			int nAmountNew = nAmountLimit;
			int nOrgAmountLimit = pItem->GetInt(ITEMDATA_AMOUNTLIMIT_ORIGINAL);
			nAmountNew -= nOrgAmountLimit/2;

			if (nAmountNew < 100)
				nAmountNew = 100;

			pItem->SetInt(ITEMDATA_AMOUNT, nAmountNew);

			pItem->SaveInfo();

			CMsgItemInfo msg;
			IF_OK (msg.Create(pItem, ITEMINFO_UPDATE))
				this->SendMsg(&msg);
		}
	}
	UpdateBattlePower();
}

bool CUser::UpEquipmentLevel		(int nPosition, bool bFree)
{
	CItem* pItem = GetEquipItemByPos(nPosition);
	if (!pItem)
		return false;

	if (pItem->IsNeedRepair())
		return false;

	OBJID idNewType = ID_NONE;
	int nChance = 0;

	if (!this->GetUpEpLevelInfo(pItem, nChance, idNewType, true))
		return false;

	CItemTypeData* pType = ItemType()->QueryItemType(idNewType);
	if (!pType)
		return false;

	if (pType->GetInt(ITEMTYPEDATA_REQ_LEVEL) > this->GetLev())
	{
		this->SendSysMsg(_TXTATR_NORMAL, STR_NEXTEQP_OVERLEV);
		return false;
	}

	int nGemCost = (100/nChance + 1)*12/10;

	if (!bFree && !(this->IsMeteorEnough(nGemCost) && this->IsMeteorChangeSpare(nGemCost)))
		return false;

	{
		OBJID idOldType = pItem->GetInt(ITEMDATA_TYPE);

		IF_NOT (pItem->ChangeType(idNewType))
			return false;
		::MyLogSave(g_szItemUpLog, "User[%u] up lev Item[%u] from [%u] to [%u] at cost %d Treasure[%u]", GetID(), pItem->GetID(), idOldType, idNewType, nGemCost, TYPE_SHOOTINGSTAR);

		if (!bFree)
		{
			int nChange = this->SpendMeteor(nGemCost);
			for(int i = 0; i < nChange; i++)
				this->AwardNewItem(TYPE_SHOOTINGSTAR, SYNCHRO_TRUE);
		}

		int nAmountLimit = pItem->GetInt(ITEMDATA_AMOUNTLIMIT_ORIGINAL);
		if (::RandGet(100) < 2)
			nAmountLimit += (::RandGet(3)+1)*100;

		nAmountLimit = nAmountLimit*(100+pItem->GetGemDurEffect())/100;

		pItem->SetInt(ITEMDATA_AMOUNTLIMIT, nAmountLimit, UPDATE_FALSE);
		pItem->SetInt(ITEMDATA_AMOUNT, nAmountLimit, UPDATE_FALSE);

		if (::RandGet(1200) < 1 && !bFree)
		{
			int nOldBattlePower = GetEquipBattlePower(pItem);
			if (GEM_NONE == pItem->GetInt(ITEMDATA_GEM1))
			{
				pItem->SetInt(ITEMDATA_GEM1, GEM_HOLE, UPDATE_FALSE);
				UpdateBattlePower();

				char szMsg[1024] = "";
				sprintf(szMsg, STR_UPLEV_HOLE1, this->GetName(), pItem->GetStr(ITEMDATA_NAME));
				this->SendSystemBBS(szMsg);
				::MyLogSave(g_szItemUpLog, "Item[%u] got No.1 GemHole.", pItem->GetID());
			}
			else if (GEM_NONE == pItem->GetInt(ITEMDATA_GEM2))
			{
				pItem->SetInt(ITEMDATA_GEM2, GEM_HOLE, UPDATE_FALSE);
				UpdateBattlePower();

				char szMsg[1024] = "";
				sprintf(szMsg, STR_UPLEV_HOLE2, this->GetName(), pItem->GetStr(ITEMDATA_NAME));
				this->SendSystemBBS(szMsg);
				::MyLogSave(g_szItemUpLog, "Item[%u] got No.2 GemHole.", pItem->GetID());
			}
		}

		pItem->SaveInfo();

		CMsgItemInfoEx msg;
		IF_OK (msg.Create(pItem, this->GetID(), 0, ITEMINFOEX_EQUIPMENT))
			this->BroadcastRoomMsg(&msg, true);
	}

	return true;
}

bool CUser::UpEquipmentLevelUltra2		(int nPosition)
{
	CItem* pEquip = GetEquipItemByPos(nPosition);
	IF_NOT (pEquip)
		return false;

	OBJID idNewType = this->ChkUpEqLevel(pEquip, false);
	CItemTypeData* pType = ItemType()->QueryItemType(idNewType);
	if (!pType)
		return false;

	if (pType->GetInt(ITEMTYPEDATA_REQ_LEVEL) > this->GetLev())
	{
		this->SendSysMsg(_TXTATR_NORMAL, STR_NEXTEQP_OVERLEV);
		return false;
	}

	IF_NOT (pEquip->ChangeType(idNewType))
		return false;

	pEquip->SaveInfo();

	CMsgItemInfoEx msg;
	IF_OK (msg.Create(pEquip, this->GetID(), 0, ITEMINFOEX_EQUIPMENT))
		this->BroadcastRoomMsg(&msg, true);

	return true;
}

bool CUser::UpEquipmentLevelUltra		(int nPosition)
{
	CItem* pEquip = GetEquipItemByPos(nPosition);
	IF_NOT (pEquip)
		return false;

	if (!pEquip->IsWeapon())
		return false;

	UINT nLev = (pEquip->GetInt(ITEMDATA_TYPE)/10)%100;
	if (nLev < 22 || nLev >= 33)
		return false;

	OBJID idNewType = pEquip->GetInt(ITEMDATA_TYPE) + 10;
	CItemTypeData* pType = ItemType()->QueryItemType(idNewType);
	if (!pType)
		return false;

	if (pType->GetInt(ITEMTYPEDATA_REQ_LEVEL) > this->GetLev())
	{
		this->SendSysMsg(_TXTATR_NORMAL, STR_NEXTEQP_OVERLEV);
		return false;
	}

	IF_NOT (pEquip->ChangeType(idNewType))
		return false;

	pEquip->SaveInfo();

	CMsgItemInfoEx msg;
	IF_OK (msg.Create(pEquip, this->GetID(), 0, ITEMINFOEX_EQUIPMENT))
		this->BroadcastRoomMsg(&msg, true);

	return true;
}

bool CUser::DownEquipmentLevel		(int nPosition, int nUserLevel)
{
	CItem* pItem = GetEquipItemByPos(nPosition);
	IF_NOT (pItem)
		return false;

	OBJID idType = pItem->GetInt(ITEMDATA_TYPE);
	if (idType%10 == 0)
		return false;

	OBJID idNewType = ID_NONE;
	if (pItem->IsShield() || pItem->IsHelmet() || pItem->IsArmor())
		idNewType = idType - ((idType/10)%10)*10;
	else if (pItem->GetItemSort() == ITEMSORT_RING || pItem->GetItemSort() == ITEMSORT_SHOES)
		idNewType = idType - ((idType/10)%100)*10 + 10;
	else if (pItem->GetItemSort() == ITEMSORT_NECKLACE || pItem->IsWeapon())
		idNewType = idType - ((idType/10)%100)*10 + 20;
	else
		return false;

	IF_NOT (pItem->ChangeType(idNewType))
		return false;

	pItem->SaveInfo();

	CMsgItemInfoEx msg;
	IF_OK (msg.Create(pItem, this->GetID(), 0, ITEMINFOEX_EQUIPMENT))
		this->BroadcastRoomMsg(&msg, true);

	return true;
}

void CUser::UpEquipmentLevel (OBJID idEquipment, OBJID idTreasure)
{
	if(!GetMap()->QueryNpcByType(GetPosX(), GetPosY(), 6, 20))
		return;

	CItem* pTreasure = this->GetItem(idTreasure);
	if (!pTreasure || pTreasure->IsPlunder())
		return;

	OBJID idTreasureType = pTreasure->GetInt(ITEMDATA_TYPE);
	if (idTreasureType != TYPE_SHOOTINGSTAR
			&& idTreasureType != TYPE_DIVOICEITEM)
	{
		this->SendSysMsg(STR_METEOR);
		return;
	}

	CItem* pItem = this->GetItem(idEquipment);
	IF_NOT(pItem && pItem->IsEquipment())
		return;

	if (pItem->GetItemSort() == ITEMSORT_EXTEND)
		return;

	if (pItem->IsNeedRepair())
		return;

	OBJID idNewType = ID_NONE;
	int nChance = 0;

	if (!this->GetUpEpLevelInfo(pItem, nChance, idNewType, true))
		return;

	if (pTreasure->GetInt(ITEMDATA_TYPE) == TYPE_DIVOICEITEM)
		nChance = nChance*130/100;

	this->EraseItem(idTreasure, SYNCHRO_TRUE);

	ItemInfoStruct infoItem;
	pItem->GetInfo(&infoItem);

	if (::RandGet(100) < nChance)
	{
		OBJID idOldType = pItem->GetInt(ITEMDATA_TYPE);
		IF_NOT (pItem->ChangeType(idNewType))
		{
			::MsgLogSave(g_szItemUpLog, "item[%u] change type to [%d] failed when uplev, lost treasure: %u.", pItem->GetID(), idNewType, idTreasureType);
			return;
		}
		::MyLogSave(g_szItemUpLog, "User[%u] uplev Item[%u] from [%u] to [%u] at cost Treasure[%u]", GetID(), pItem->GetID(), idOldType, idNewType, idTreasureType);

		int nAmountLimit = pItem->GetInt(ITEMDATA_AMOUNTLIMIT_ORIGINAL);
		if (::RandGet(100) < 2)
			nAmountLimit += (::RandGet(3)+1)*100;

		nAmountLimit = nAmountLimit*(100+pItem->GetGemDurEffect())/100;

		pItem->SetInt(ITEMDATA_AMOUNTLIMIT, nAmountLimit, UPDATE_FALSE);
		pItem->SetInt(ITEMDATA_AMOUNT, nAmountLimit, UPDATE_FALSE);

		if (::RandGet(1200) == 0)
		{
			GetEquipBattlePower(pItem);

			bool bHole = false;
			if (GEM_NONE == pItem->GetInt(ITEMDATA_GEM1))
			{
				pItem->SetInt(ITEMDATA_GEM1, GEM_HOLE, UPDATE_FALSE);
				UpdateBattlePower();

				char szMsg[1024] = "";
				sprintf(szMsg, STR_UPLEV_HOLE1, this->GetName(), pItem->GetStr(ITEMDATA_NAME));
				this->SendSystemBBS(szMsg);
				::MyLogSave(g_szItemUpLog, "Item[%u] got No.1 GemHole.", pItem->GetID());
				bHole = true;
			}
			else if (GEM_NONE == pItem->GetInt(ITEMDATA_GEM2))
			{
				pItem->SetInt(ITEMDATA_GEM2, GEM_HOLE, UPDATE_FALSE);
				UpdateBattlePower();

				char szMsg[1024] = "";
				sprintf(szMsg, STR_UPLEV_HOLE2, this->GetName(), pItem->GetStr(ITEMDATA_NAME));
				this->SendSystemBBS(szMsg);
				::MyLogSave(g_szItemUpLog, "Item[%u] got No.2 GemHole.", pItem->GetID());
				bHole = true;
			}

			if (bHole && m_dwUnk638 > 0 && ::RandGet(100) < 50)
			{
				this->SendSysMsg(_TXTATR_SYSTEM, STR_LUCK_GEM_SOCKET);

				CMsgName msg;
				IF_OK(msg.Create(NAMEACT_ROLEEFFECT, STR_LUCKY_EFFECT, this->GetID()))
					SendMsg(&msg);
			}
		}

		pItem->SaveInfo();

		CMsgItemInfo msg;
		IF_OK (msg.Create(pItem, ITEMINFO_UPDATE))
			this->SendMsg(&msg);
	}
	else
	{
		if (m_dwUnk638 > 0 && ::RandGet(100) < 10)
		{
			this->SendSysMsg(_TXTATR_SYSTEM, STR_LUCK_EQUIP_DUR);

			CMsgName msg;
			IF_OK(msg.Create(NAMEACT_ROLEEFFECT, STR_LUCKY_EFFECT, this->GetID()))
				SendMsg(&msg);
			return;
		}

		int nAmountLimit = pItem->GetInt(ITEMDATA_AMOUNTLIMIT);
		if (::RandGet(100) < PERCENT_DECDUR)
		{
			if (nAmountLimit/100 > 1)
			{
				nAmountLimit -= 100;
				pItem->SetInt(ITEMDATA_AMOUNTLIMIT, nAmountLimit);
			}
		}

		int nOrgAmountLimit = pItem->GetInt(ITEMDATA_AMOUNTLIMIT_ORIGINAL);
		nAmountLimit -= nOrgAmountLimit/2;
		if (nAmountLimit < 100)
			nAmountLimit = 100;
		pItem->SetInt(ITEMDATA_AMOUNT, nAmountLimit);

		pItem->SaveInfo();

		CMsgItemInfo msg;
		IF_OK (msg.Create(pItem, ITEMINFO_UPDATE))
			this->SendMsg(&msg);
	}
}

bool CUser::RecoverEquipmentDur(int nPos)
{
	CItem* pItem = this->GetEquipItemByPos(nPos);
	if (!pItem)
		return false;

	int nRecoverCost = pItem->GetRecoverDurCost();
	IF_NOT (nRecoverCost > 0)
		return false;

	if (!this->SpendMoney(nRecoverCost, SYNCHRO_TRUE))
		return false;

	if (!pItem->RecoverDur())
		return false;

	pItem->SaveInfo();

	CMsgItemInfo msg;
	IF_OK (msg.Create(pItem, ITEMINFO_UPDATE))
		this->SendMsg(&msg);

	return true;
}

bool CUser::IsItemProtected(CItem* pItem)
{
	if (true)
		return false;

	if (!pItem || !pItem->IsEquipment())
		return false;

	if (0 == GetEquipBattlePower(pItem))
		return false;

	if (pItem->GetInt(ITEMDATA_TYPE)%10 < 8)
		return false;

	int nKind	= pItem->GetSubType();
	DWORD dwProf = GetProfessionSort();
	bool bSuit	= false;
	switch(nKind)
	{
	case 117:
	case 160:
		bSuit = true;
		break;
	case 118:
	case 130:
		bSuit = (dwProf == 1);
		break;
	case 111:
	case 131:
	case 141:
	case 900:
		bSuit = (dwProf == 2);
		break;
	case 113:
	case 133:
	case 142:
	case 500:
		bSuit = (dwProf == 4);
		break;
	case 114:
	case 121:
	case 134:
	case 152:
	case 421:
		bSuit = (dwProf == 10 || dwProf == 14 || dwProf == 13);
		break;
	case 120:
		bSuit = (dwProf != 10 && dwProf != 14);
		break;
	case 150:
	case 151:
		bSuit = (dwProf != 10 && dwProf != 14);
		nKind = 7;
		break;
	case 410:
	case 420:
	case 430:
	case 440:
	case 450:
	case 460:
	case 480:
	case 481:
	case 490:
		bSuit = (dwProf == 1 || dwProf == 2);
		nKind = 4;
		break;
	case 510:
	case 530:
	case 540:
	case 560:
	case 561:
	case 580:
		bSuit = (dwProf != 4);
		nKind = 5;
		break;
	default:
		break;
	}
	if (!bSuit)
		return false;

	int nCount = 0;
	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		if (!IsProtectedAgainst(pItem, GetEquipItemByPos(i), nKind, &nCount, true))
			return false;
	}

	for(vector<CItem*>::iterator it = m_setItem.begin(); it != m_setItem.end(); it++)
	{
		if (!IsProtectedAgainst(pItem, *it, nKind, &nCount, false))
			return false;
	}

	return true;
}

bool CUser::IsProtectedAgainst(CItem* pItem, CItem* pOther, int nKind, int* pCount, bool bEquip)
{
	if (!pItem || !pOther || pOther == pItem || !pOther->IsEquipment())
		return true;

	int nOtherKind	= pOther->GetSubType();
	DWORD dwProf	= GetProfessionSort();
	int nPower		= GetEquipBattlePower(pItem);
	int nOtherPower	= GetEquipBattlePower(pOther);
	switch(nOtherKind)
	{
	case 150:
	case 151:
		nOtherKind = 7;
		break;
	case 410:
	case 420:
	case 430:
	case 440:
	case 450:
	case 460:
	case 480:
	case 481:
	case 490:
		nOtherKind = 4;
		break;
	case 510:
	case 530:
	case 540:
	case 560:
	case 561:
	case 580:
		nOtherKind = 5;
		break;
	default:
		break;
	}

	if (nOtherKind == nKind)
	{
		if (nOtherPower < nPower)
			return true;
		if (nOtherPower == nPower && !bEquip)
			return true;

		if (dwProf == 1 && nKind == 4 && *pCount < 1)
		{
			(*pCount)++;
			return true;
		}
		return false;
	}

	bool bRival = false;
	switch(dwProf)
	{
	case 1:
		bRival = (nOtherKind == 117 || nOtherKind == 118) && (nKind == 117 || nKind == 118)
				|| (nOtherKind == 4 || nOtherKind == 5) && (nKind == 4 || nKind == 5);
		break;
	case 2:
		bRival = (nOtherKind == 117 || nOtherKind == 111 || nOtherKind == 141) && (nKind == 117 || nKind == 111 || nKind == 141)
				|| (nOtherKind == 4 || nOtherKind == 5) && (nKind == 4 || nKind == 5);
		break;
	case 4:
		bRival = (nOtherKind == 117 || nOtherKind == 113 || nOtherKind == 142) && (nKind == 117 || nKind == 113 || nKind == 142);
		break;
	case 10:
	case 14:
		bRival = (nOtherKind == 117 || nOtherKind == 114) && (nKind == 117 || nKind == 114)
				|| (nOtherKind == 421 || nOtherKind == 5) && (nKind == 421 || nKind == 5);
		break;
	case 13:
		bRival = (nOtherKind == 117 || nOtherKind == 114) && (nKind == 117 || nKind == 114)
				|| (nOtherKind == 421 || nOtherKind == 5) && (nKind == 421 || nKind == 5)
				|| (nOtherKind == 120 || nOtherKind == 121) && (nKind == 120 || nKind == 121)
				|| (nOtherKind == 152 || nOtherKind == 7) && (nKind == 152 || nKind == 7);
		break;
	default:
		break;
	}

	if (bRival && nOtherPower - nPower >= 5)
		return false;
	return true;
}

bool CUser::IsWeaponR562()
{
	if (m_pWeaponR && (m_pWeaponR->GetInt(ITEMDATA_TYPE)/1000)%1000 == 562)
		return true;
	return false;
}

void CUser::Mine()
{
	if (!this->IsAlive())
		return;

	if (m_tMine.IsActive())
		return;

	if (m_pTransformation)
		return;

	if (!this->IsWeaponR562())
	{
		this->SendSysMsg(STR_MINE_WITH_PECKER);
		return;
	}

	CGameMap* pMap = MapManager()->GetGameMap(GetMapID());
	IF_NOT (pMap)
		return;

	if (!pMap->IsMineField())
		return;

	if (pMap->GetResLev() <= 0)
	{
		this->SendSysMsg(STR_NO_MINE);
		return;
	}

	m_tMine.Startup(3000);
	m_nMineCount = 0;
	m_dwMineFactor = (int)(::RandomRateGet(0.5)*100);

	DetachLuckyStatus();
	UpdateTimeUnk63C();
}

void CUser::StopMine(void)
{
	m_tMine.Clear();
	m_nMineCount = 0;
}

void CUser::ProcessMineTimer(void)
{
	if (!this->IsAlive())
	{
		this->StopMine();
		return;
	}

	CGameMap* pMap = MapManager()->GetGameMap(GetMapID());
	IF_NOT (pMap)
		return;

	if (!pMap->IsMineField())
	{
		this->StopMine();
		return;
	}

	if (!this->IsWeaponR562())
	{
		this->SendSysMsg(STR_NEED_PICK);
		return;
	}

	int nFactor = m_dwMineFactor*200 / (m_nMineCount+200);
	if (GetLev() <= 15)
		nFactor = nFactor*80/100;

	int nRand = (10000-1)-::RandGet(10000);

	switch(pMap->GetResLev())
	{
	case 1:
		if (nRand < nFactor*20/100)
		{
			this->AwardItem(1072050 + ::RandGet(3), SYNCHRO_TRUE);
			this->SendSysMsg(STR_GOLD_ORE);
		}
		else if (nRand < nFactor*70/100)
		{
			this->AwardItem(1072031, SYNCHRO_TRUE);
			this->SendSysMsg(STR_EUXENITE_ORE);
		}
		else if (nRand < nFactor*1070/100)
		{
			this->AwardItem(::RandGet(10) < 6 ? 1072010 + ::RandGet(5) : 1072015 + ::RandGet(5), SYNCHRO_TRUE);
			this->SendSysMsg(STR_IRON_ORE);
		}
		break;
	case 2:
		if (nRand < nFactor*17/100)
		{
			this->AwardItem(1072050 + ::RandGet(6), SYNCHRO_TRUE);
			this->SendSysMsg(STR_GOLD_ORE);
		}
		else if (nRand < nFactor*167/100)
		{
			this->AwardItem(1072010 + ::RandGet(10), SYNCHRO_TRUE);
			this->SendSysMsg(STR_IRON_ORE);
		}
		else if (nRand < nFactor*1017/100)
		{
			this->AwardItem(::RandGet(10) < 6 ? 1072020 + ::RandGet(5) : 1072025 + ::RandGet(5), SYNCHRO_TRUE);
			this->SendSysMsg(STR_COPPER_ORE);
		}
		break;
	case 3:
		if (nRand < nFactor*17/100)
		{
			this->AwardItem(1072050 + ::RandGet(10), SYNCHRO_TRUE);
			this->SendSysMsg(STR_GOLD_ORE);
		}
		else if (nRand < nFactor*167/100)
		{
			this->AwardItem(1072020 + ::RandGet(10), SYNCHRO_TRUE);
			this->SendSysMsg(STR_COPPER_ORE);
		}
		else if (nRand < nFactor*1017/100)
		{
			this->AwardItem(::RandGet(10) < 6 ? 1072040 + ::RandGet(10) : 1072045 + ::RandGet(10), SYNCHRO_TRUE);
			this->SendSysMsg(STR_SILVER_ORE);
		}
		break;
	case 4:
		if (::RandGet(10800) == 123)
			this->AwardItem(722312, SYNCHRO_TRUE);
		break;
	default:
		return;
	}

	m_nMineCount++;

	CMsgAction msg;
	IF_OK (msg.Create(this->GetID(), this->GetPosX(), this->GetPosY(), this->GetDir(), actionMine, nFactor))
		this->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	if (!(GetLev() <= 15))
	{
		OBJID idGem = ID_NONE;
		switch(pMap->GetResLev())
		{
		case 1:
			{
				int nGem = ::RandGet(20000000);
				if (nGem < 1)
				{
					if (stricmp(CStringManager::Instance()->GetStr("Language"), "Japanese") != 0 || ::RandGet(2) >= 1)
					{
						int nKind = ::RandGet(3);
						idGem = (nKind == 1) ? 700013 : (nKind == 2 ? 700003 : 700023);
					}
				}
				else if (nGem < 7)
				{
					int nKind = ::RandGet(3);
					idGem = (nKind == 1) ? 700012 : (nKind == 2 ? 700002 : 700022);
				}
				else if (nGem < 37)
				{
					int nKind = ::RandGet(3);
					idGem = (nKind == 1) ? 700011 : (nKind == 2 ? 700001 : 700021);
				}
				if (idGem != ID_NONE && this->AwardItem(idGem, SYNCHRO_TRUE))
					this->SendSysMsg(STR_GAIN_GEM);

				idGem = ID_NONE;
				nGem = ::RandGet(3000000);
				if (nGem < 1)
				{
					if (stricmp(CStringManager::Instance()->GetStr("Language"), "Japanese") != 0 || ::RandGet(2) >= 1)
						idGem = 700073;
				}
				else if (nGem < 7)
					idGem = 700072;
				else if (nGem < 37)
					idGem = 700071;
				if (idGem != ID_NONE && this->AwardItem(idGem, SYNCHRO_TRUE))
					this->SendSysMsg(STR_GAIN_GEM);
			}
			break;
		case 2:
			{
				int nGem = ::RandGet(12000000);
				int nBase = 0;
				if (nGem < 1)
				{
					if (stricmp(CStringManager::Instance()->GetStr("Language"), "Japanese") != 0 || ::RandGet(2) >= 1)
						nBase = 700003;
				}
				else if (nGem < 12)
					nBase = 700002;
				else if (nGem < 60)
					nBase = 700001;
				if (nBase)
				{
					int nKind = ::RandGet(4);
					idGem = (nKind == 1) ? nBase+30 : (nKind == 2 ? nBase+40 : (nKind == 3 ? nBase+10 : nBase));
					if (this->AwardItem(idGem, SYNCHRO_TRUE))
						this->SendSysMsg(STR_GAIN_GEM);
				}
			}
			break;
		case 3:
			{
				int nGem = ::RandGet(10000000);
				int nBase = 0;
				if (nGem < 1)
				{
					if (stricmp(CStringManager::Instance()->GetStr("Language"), "Japanese") != 0 || ::RandGet(2) >= 1)
						nBase = 700053;
				}
				else if (nGem < 12)
					nBase = 700052;
				else if (nGem < 60)
					nBase = 700051;
				if (nBase)
				{
					idGem = (::RandGet(2) == 1) ? nBase : nBase+10;
					if (this->AwardItem(idGem, SYNCHRO_TRUE))
						this->SendSysMsg(STR_GAIN_GEM);
				}
			}
			break;
		default:
			break;
		}
	}

	if (m_pWeaponR)
	{
		int nGem1 = m_pWeaponR->GetInt(ITEMDATA_GEM1);
		int nGem2 = m_pWeaponR->GetInt(ITEMDATA_GEM2);
		if ((nGem1 != GEM_NONE && nGem1 != GEM_HOLE || nGem2 != GEM_NONE && nGem2 != GEM_HOLE) && ::RandGet(10) == 3)
			this->SendGemEffect(true);
	}

	DetachLuckyStatus();
	UpdateTimeUnk63C();
}

bool CUser::MultiDelItem(OBJID idTypeFirst, OBJID idTypeLast, int nNum)
{
	int nItems	= m_setItem.size();
	if (nItems > 0)
	{
		int nCount = 0;
		OBJID* id = new OBJID[nItems];

		for(int i = nItems-1; i >= 0; i--)
		{
			CItem* pItem = m_setItem[i];
			if (!pItem || pItem->IsPlunder() || pItem->TestSpecialFlag(2) || pItem->TestSpecialFlag(1) || this->IsItemProtected(pItem))
				continue;
			if (pItem->GetInt(ITEMDATA_TYPE) < (int)idTypeFirst || pItem->GetInt(ITEMDATA_TYPE) > (int)idTypeLast)
				continue;

			id[nCount] = pItem->GetID();
			if (++nCount >= nNum)
				break;
		}

		if (nCount >= nNum)
		{
			for (int i=0; i<nCount; i++)
			{
				CItem* pItem = this->GetItem(id[i]);
				if (pItem && pItem->IsNonsuchItem())
					::MyLogSave(g_szItemUpLog, "User:%s[%u] consume nonsuch item %u[type=%u]", this->GetName(), this->GetID(), pItem->GetID(), pItem->GetInt(ITEMDATA_TYPE));
				this->EraseItem(id[i], SYNCHRO_TRUE);
			}

			delete [] id;
			return true;
		}
		else
			delete [] id;
	}
	return false;
}

bool CUser::MultiCheckItem(OBJID idTypeFirst, OBJID idTypeLast, int nNum)
{
	int nItems	= m_setItem.size();
	if (nItems <= 0)
		return false;

	int nCount = 0;
	for(int i = nItems-1; i >= 0; i--)
	{
		CItem* pItem = m_setItem[i];
		if (!pItem || pItem->IsPlunder() || pItem->TestSpecialFlag(2) || pItem->TestSpecialFlag(1) || this->IsItemProtected(pItem))
			continue;
		if (pItem->GetInt(ITEMDATA_TYPE) < (int)idTypeFirst || pItem->GetInt(ITEMDATA_TYPE) > (int)idTypeLast)
			continue;

		if (++nCount >= nNum)
			return true;
	}
	return false;
}

const int	_TYPE_METEOR_PACK5		= 723268;
const int	_TYPE_METEOR_PACK10		= 720027;

bool CUser::IsMeteorEnough(int nMeteors)
{
	int nItems	= m_setItem.size();
	if (nItems <= 0)
		return false;

	int nCount = 0;
	for(int i = nItems-1; i >= 0; i--)
	{
		CItem* pItem = m_setItem[i];
		if (!pItem)
			continue;

		int nType = pItem->GetInt(ITEMDATA_TYPE);
		if (nType == _TYPE_METEOR_PACK10)
			nCount += 10;
		else if (nType == _TYPE_METEOR_PACK5)
			nCount += 5;
		else if (nType > 1088000 && nType <= 1088002)
			nCount++;

		if (nCount >= nMeteors)
			return true;
	}
	return false;
}

bool CUser::IsMeteorChangeSpare(int nMeteors)
{
	int nItems	= m_setItem.size();
	int nSpare	= _MAX_USERITEMSIZE - nItems;
	int nOne = 0, nFive = 0, nTen = 0;
	if (nItems <= 0)
		return false;

	for(int i = nItems-1; i >= 0; i--)
	{
		CItem* pItem = m_setItem[i];
		if (!pItem)
			continue;

		int nType = pItem->GetInt(ITEMDATA_TYPE);
		if (nType == _TYPE_METEOR_PACK10)
			nTen++;
		else if (nType == _TYPE_METEOR_PACK5)
			nFive++;
		else if (nType > 1088000 && nType <= 1088002)
			nOne++;
	}

	int nRest = nMeteors - nOne;
	if (nRest <= 0)
		return true;

	nSpare += nOne;
	int nNeedFive	= nRest / 5;
	int nRestFive	= nRest % 5;
	if (nNeedFive <= nFive)
	{
		if (nRestFive == 0)
			return true;
		if (nNeedFive < nFive)
			return 5 - nRestFive <= nNeedFive + nSpare + 1;
	}

	nSpare	+= nFive;
	nRest	-= nFive*5;
	int nNeedTen	= nRest / 10;
	int nRestTen	= nRest % 10;
	if (nNeedTen > nTen)
		return false;
	if (nNeedTen == nTen && nRestTen > 0)
		return false;
	if (nRestTen == 0)
		return true;
	return 10 - nRestTen <= nNeedTen + nSpare + 1;
}

int CUser::SpendMeteor(int nMeteors)
{
	int nItems	= m_setItem.size();
	int nOne = 0, nFive = 0, nTen = 0;
	if (nItems <= 0)
		return 0;

	for(int i = nItems-1; i >= 0; i--)
	{
		CItem* pItem = m_setItem[i];
		if (!pItem)
			continue;

		int nType = pItem->GetInt(ITEMDATA_TYPE);
		if (nType == _TYPE_METEOR_PACK10)
			nTen++;
		else if (nType == _TYPE_METEOR_PACK5)
			nFive++;
		else if (nType > 1088000 && nType <= 1088002)
			nOne++;
	}

	if (nOne >= nMeteors)
	{
		this->MultiDelItem(TYPE_SHOOTINGSTAR, TYPE_DIVOICEITEM, nMeteors);
		return 0;
	}

	this->MultiDelItem(TYPE_SHOOTINGSTAR, TYPE_DIVOICEITEM, nOne);
	int nRest		= nMeteors - nOne;
	int nNeedFive	= nRest / 5;
	int nRestFive	= nRest % 5;
	if (nNeedFive > nFive)
	{
		this->MultiDelItem(_TYPE_METEOR_PACK5, _TYPE_METEOR_PACK5, nFive);
	}
	else if (nNeedFive < nFive)
	{
		if (nRestFive == 0)
		{
			this->MultiDelItem(_TYPE_METEOR_PACK5, _TYPE_METEOR_PACK5, nNeedFive);
			return 0;
		}
		this->MultiDelItem(_TYPE_METEOR_PACK5, _TYPE_METEOR_PACK5, nNeedFive+1);
		return 5 - nRestFive;
	}
	else
	{
		this->MultiDelItem(_TYPE_METEOR_PACK5, _TYPE_METEOR_PACK5, nNeedFive);
		if (nRestFive == 0)
			return 0;
	}

	nRest -= nFive*5;
	int nNeedTen	= nRest / 10;
	int nRestTen	= nRest % 10;
	if (nNeedTen >= nTen || nRestTen == 0)
	{
		this->MultiDelItem(_TYPE_METEOR_PACK10, _TYPE_METEOR_PACK10, nNeedTen);
		return 0;
	}
	this->MultiDelItem(_TYPE_METEOR_PACK10, _TYPE_METEOR_PACK10, nNeedTen+1);
	return 10 - nRestTen;
}

bool CUser::ComposeItem(OBJID idMain, OBJID idAccessory1, OBJID idAccessory2, OBJID idGem1, OBJID idGem2)
{
	if (idMain == ID_NONE || idAccessory1 == ID_NONE || idAccessory2 == ID_NONE)
		return false;

	CItem* pItem = GetItem(idMain);
	if (!pItem)
		return false;
	CItem* pItemAcc1 = GetItem(idAccessory1);
	CHECKF(pItemAcc1);
	CItem* pItemAcc2 = GetItem(idAccessory2);
	CHECKF(pItemAcc2);

	CItem* pGem1 = NULL;
	CItem* pGem2 = NULL;
	int nOldBattlePower = this->GetEquipBattlePower(pItem);

	if (pItemAcc1->IsPlunder() || pItemAcc2->IsPlunder()
			|| pItemAcc1->TestSpecialFlag(2) || pItemAcc1->TestSpecialFlag(1)
			|| pItemAcc2->TestSpecialFlag(2) || pItemAcc2->TestSpecialFlag(1)
			|| IsItemProtected(pItemAcc1) || IsItemProtected(pItemAcc2))
		return false;

	if (!pItem->IsEquipment() && !pItem->IsComposeStone())
		return false;
	if (!pItemAcc1->IsEquipment() && !pItemAcc1->IsComposeStone())
		return false;
	if (!pItemAcc2->IsEquipment() && !pItemAcc2->IsComposeStone())
		return false;

	if (CItem::GetItemSort(pItem->GetInt(ITEMDATA_TYPE)) != CItem::GetItemSort(pItemAcc1->GetInt(ITEMDATA_TYPE)) && !pItemAcc1->IsComposeStone())
		return false;
	if (CItem::GetItemSort(pItem->GetInt(ITEMDATA_TYPE)) != CItem::GetItemSort(pItemAcc2->GetInt(ITEMDATA_TYPE)) && !pItemAcc2->IsComposeStone())
		return false;

	if (pItem->IsComposeStone())
		return false;
	if (pItem->IsComposeStone())
	{
		if (!pItemAcc1->IsComposeStone())
			return false;
		if (!pItemAcc2->IsComposeStone())
			return false;
	}

	if (pItem->IsBowSort())
	{
		if (!pItemAcc1->IsBowSort() && !pItemAcc1->IsComposeStone())
			return false;
		if (!pItemAcc2->IsBowSort() && !pItemAcc2->IsComposeStone())
			return false;
	}
	if (CItem::IsMSword(pItem->GetInt(ITEMDATA_TYPE)))
	{
		if (!CItem::IsMSword(pItemAcc1->GetInt(ITEMDATA_TYPE)) && !pItemAcc1->IsComposeStone())
			return false;
		if (!CItem::IsMSword(pItemAcc2->GetInt(ITEMDATA_TYPE)) && !pItemAcc2->IsComposeStone())
			return false;
	}

	if (!pItem->IsWeapon())
	{
		if (pItemAcc1->GetSubType() != pItem->GetSubType() && !pItemAcc1->IsComposeStone())
			return false;
		if (pItemAcc2->GetSubType() != pItem->GetSubType() && !pItemAcc2->IsComposeStone())
			return false;
	}

	int nAddition = pItem->GetInt(ITEMDATA_MAGIC3);
	if (nAddition >= 9)
		return false;
	if (pItemAcc1->GetInt(ITEMDATA_MAGIC3) < 1 || pItemAcc1->GetInt(ITEMDATA_MAGIC3) < nAddition)
		return false;
	if (pItemAcc2->GetInt(ITEMDATA_MAGIC3) < 1 || pItemAcc2->GetInt(ITEMDATA_MAGIC3) < nAddition)
		return false;

	if (nAddition >= 5)
	{
		pGem1 = GetItem(idGem1);
		if (!pGem1 || !pGem1->IsGem() || pGem1->IsPlunder() || pGem1->TestSpecialFlag(2) || pGem1->TestSpecialFlag(1) || IsItemProtected(pGem1))
			return false;

		if (pItem->IsWeapon() || pItem->IsComposeStone())
		{
			pGem2 = GetItem(idGem2);
			if (!pGem2 || !pGem2->IsGem() || pGem2->IsPlunder() || pGem2->TestSpecialFlag(2) || pGem2->TestSpecialFlag(1) || IsItemProtected(pGem2))
				return false;
		}
	}

	try
	{
		if (pItemAcc1->IsNonsuchItem())
			::MyLogSave("gmlog/ComposeItem", "%s(%u) ComposeItem spend:[id=%u, type=%u], dur=%d, max_dur=%d",
						this->GetName(), this->GetID(), pItemAcc1->GetID(), pItemAcc1->GetInt(ITEMDATA_TYPE), pItemAcc1->GetInt(ITEMDATA_AMOUNT), pItemAcc1->GetInt(ITEMDATA_AMOUNTLIMIT));
		if (pItemAcc2->IsNonsuchItem())
			::MyLogSave("gmlog/ComposeItem", "%s(%u) ComposeItem spend:[id=%u, type=%u], dur=%d, max_dur=%d",
						this->GetName(), this->GetID(), pItemAcc2->GetID(), pItemAcc2->GetInt(ITEMDATA_TYPE), pItemAcc2->GetInt(ITEMDATA_AMOUNT), pItemAcc2->GetInt(ITEMDATA_AMOUNTLIMIT));
		if (pGem1 && pGem1->IsNonsuchItem())
			::MyLogSave("gmlog/ComposeItem", "%s(%u) ComposeItem spend:[id=%u, type=%u], dur=%d, max_dur=%d",
						this->GetName(), this->GetID(), pGem1->GetID(), pGem1->GetInt(ITEMDATA_TYPE), pGem1->GetInt(ITEMDATA_AMOUNT), pGem1->GetInt(ITEMDATA_AMOUNTLIMIT));
		if (pGem2 && pGem2->IsNonsuchItem())
			::MyLogSave("gmlog/ComposeItem", "%s(%u) ComposeItem spend:[id=%u, type=%u], dur=%d, max_dur=%d",
						this->GetName(), this->GetID(), pGem2->GetID(), pGem2->GetInt(ITEMDATA_TYPE), pGem2->GetInt(ITEMDATA_AMOUNT), pGem2->GetInt(ITEMDATA_AMOUNTLIMIT));

		bool bNonsuch = pItem->IsNonsuchItem();
		this->EraseItem(idAccessory1, SYNCHRO_TRUE);
		this->EraseItem(idAccessory2, SYNCHRO_TRUE);
		if (pGem1)
			this->EraseItem(idGem1, SYNCHRO_TRUE);
		if (pGem2)
			this->EraseItem(idGem2, SYNCHRO_TRUE);

		pItem->SetInt(ITEMDATA_MAGIC3, nAddition+1, UPDATE_TRUE);

		CMsgItemInfo msg;
		if (msg.Create(pItem, ITEMINFO_UPDATE))
			this->SendMsg(&msg);

		if (!bNonsuch && pItem->IsNonsuchItem())
			::MyLogSave("gmlog/ComposeItem", "%s(%u) ComposeItem make:[id=%u, type=%u], dur=%d, max_dur=%d",
						this->GetName(), this->GetID(), pItem->GetID(), pItem->GetInt(ITEMDATA_TYPE), pItem->GetInt(ITEMDATA_AMOUNT), pItem->GetInt(ITEMDATA_AMOUNTLIMIT));
	}
	catch(...)
	{
		::LogSave("catch error in CUser::ComposeItem() idMain [%u] idAccessory1 [%u] idAccessory2 [%u] idGem1 [%u] idGem2 [%u] ", idMain, idAccessory1, idAccessory2, idGem1, idGem2);
		return false;
	}

	this->UpdateBattlePower();
	return true;
}

int CUser::CountCard(void)
{
	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT id FROM cq_card WHERE account_id=%u", this->GetAccountID());
	return __max(0, Database()->CountRecord(szSQL));
}

bool CUser::CheckInCard(void)
{
	CItem* pItem = this->GetTaskItemByType(CARD_FEE_POINT);
	if (!pItem)
		return false;

	OBJID idCard = pItem->GetID();
	IF_NOT (this->EraseItem(idCard, SYNCHRO_TRUE))
		return false;

	::MyLogSave("gmlog/card_item", "[%s][id=%u] delete card item[id=%u]", this->GetName(), this->GetID(), idCard);

	IDatabase* pDb = Database();
	IF_NOT (pDb)
	{
		::MyLogSave("gmlog/card_item", "Error: [%s][id=%u] db failed.", this->GetName(), this->GetID());
		return false;
	}

	SQLBUF	szSQL;
	sprintf(szSQL, "INSERT INTO cq_card SET type=%u,account_id=%u", CARD_FEE_POINT, this->GetAccountID());
	IF_NOT (pDb->ExecuteSQL(szSQL))
	{
		::MyLogSave("gmlog/card_item", "Error: [%s][id=%u] exe insert sql[%s] failed.", this->GetName(), this->GetID(), szSQL);
		return false;
	}

	::MyLogSave("gmlog/card_item", "[%s][id=%u] suc insert card record[id=%u]",
				this->GetName(),
				this->GetID(),
				pDb->GetInsertId());

	return true;
}

bool CUser::CheckOutCard(void)
{
	if (m_setItem.size() >= _MAX_USERITEMSIZE)
	{
		this->SendSysMsg(STR_FULLBAG_NO_MORE);
		return false;
	}

	IDatabase* pDb = Database();
	IF_NOT (pDb)
	{
		::MyLogSave("gmlog/card_item", "Error: [%s][id=%u] db failed.", this->GetName(), this->GetID());
		return false;
	}

	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT id FROM cq_card WHERE account_id=%u", this->GetAccountID());

	int nCards = pDb->CountRecord(szSQL);
	if (-1 == nCards)
	{
		::MyLogSave("gmlog/card_item", "Error: [%s][id=%u] db failed when count.", this->GetName(), this->GetID());
		return false;
	}
	else if (0 == nCards)
	{
		::MyLogSave("gmlog/card_item", "Error: [%s][id=%u] no card exist.", this->GetName(), this->GetID());
		return false;
	}

	sprintf(szSQL, "DELETE FROM cq_card WHERE account_id=%u LIMIT 1", this->GetAccountID());
	IF_NOT (pDb->ExecuteSQL(szSQL))
	{
		::MyLogSave("gmlog/card_item", "Error: [%s][id=%u] exe delete sql[%s] failed.", this->GetName(), this->GetID(), szSQL);
		return false;
	}

	::MyLogSave("gmlog/card_item", "[%s][id=%u] delete a card record.", this->GetName(), this->GetID());

	CItem* pItem = this->AwardItem(CARD_FEE_POINT, SYNCHRO_TRUE);
	if (!pItem)
	{
		::MyLogSave("gmlog/card_item", "Error: [%s][id=%u] award card item [%u] failed.", this->GetName(), this->GetID(), CARD_FEE_POINT);
		return false;
	}

	::MyLogSave("gmlog/card_item", "[%s][id=%u] award card item [%u] [id=%u] succed.",
				this->GetName(),
				this->GetID(),
				CARD_FEE_POINT,
				pItem->GetID());

	return true;
}

int CUser::CountCard2(void)
{
	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT id FROM cq_card2 WHERE account_id=%u", this->GetAccountID());
	return __max(0, Database()->CountRecord(szSQL));
}

bool CUser::CheckInCard2(void)
{
	CItem* pItem = this->GetTaskItemByType(CARD_FEE_POINT2);
	if (!pItem)
		return false;

	OBJID idCard = pItem->GetID();
	IF_NOT (this->EraseItem(idCard, SYNCHRO_TRUE))
		return false;

	::MyLogSave("gmlog/card2_item", "[%s][id=%u] delete card item[id=%u]", this->GetName(), this->GetID(), idCard);

	IDatabase* pDb = Database();
	IF_NOT (pDb)
	{
		::MyLogSave("gmlog/card2_item", "Error: [%s][id=%u] db failed.", this->GetName(), this->GetID());
		return false;
	}

	SQLBUF	szSQL;
	sprintf(szSQL, "INSERT INTO cq_card2 SET type=%u,account_id=%u", CARD_FEE_POINT2, this->GetAccountID());
	IF_NOT (pDb->ExecuteSQL(szSQL))
	{
		::MyLogSave("gmlog/card2_item", "Error: [%s][id=%u] exe insert sql[%s] failed.", this->GetName(), this->GetID(), szSQL);
		return false;
	}

	::MyLogSave("gmlog/card2_item", "[%s][id=%u] suc insert card record[id=%u]",
				this->GetName(),
				this->GetID(),
				pDb->GetInsertId());

	return true;
}

bool CUser::CheckOutCard2(void)
{
	if (m_setItem.size() >= _MAX_USERITEMSIZE)
	{
		this->SendSysMsg(STR_FULLBAG_NO_MORE);
		return false;
	}

	IDatabase* pDb = Database();
	IF_NOT (pDb)
	{
		::MyLogSave("gmlog/card2_item", "Error: [%s][id=%u] db failed.", this->GetName(), this->GetID());
		return false;
	}

	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT id FROM cq_card2 WHERE account_id=%u", this->GetAccountID());

	int nCards = pDb->CountRecord(szSQL);
	if (-1 == nCards)
	{
		::MyLogSave("gmlog/card2_item", "Error: [%s][id=%u] db failed when count.", this->GetName(), this->GetID());
		return false;
	}
	else if (0 == nCards)
	{
		::MyLogSave("gmlog/card2_item", "Error: [%s][id=%u] no card exist.", this->GetName(), this->GetID());
		return false;
	}

	sprintf(szSQL, "DELETE FROM cq_card2 WHERE account_id=%u LIMIT 1", this->GetAccountID());
	IF_NOT (pDb->ExecuteSQL(szSQL))
	{
		::MyLogSave("gmlog/card2_item", "Error: [%s][id=%u] exe delete sql[%s] failed.", this->GetName(), this->GetID(), szSQL);
		return false;
	}

	::MyLogSave("gmlog/card2_item", "[%s][id=%u] delete a card record.", this->GetName(), this->GetID());

	CItem* pItem = this->AwardItem(CARD_FEE_POINT2, SYNCHRO_TRUE);
	if (!pItem)
	{
		::MyLogSave("gmlog/card2_item", "Error: [%s][id=%u] award card item [%u] failed.", this->GetName(), this->GetID(), CARD_FEE_POINT2);
		return false;
	}

	::MyLogSave("gmlog/card2_item", "[%s][id=%u] award card item [%u] [id=%u] succed.",
				this->GetName(),
				this->GetID(),
				CARD_FEE_POINT2,
				pItem->GetID());

	return true;
}

static LPCTSTR GetCardTableName(OBJID idCardType)
{
	switch(idCardType)
	{
	case CARD_FEE_POINT:		return "cq_card";
	case CARD_FEE_POINT2:		return "cq_card2";
	case 780002:				return "cq_card3";
	case 780003:				return "cq_card4";
	}
	return NULL;
}

int CUser::CountCardByType(OBJID idCardType)
{
	LPCTSTR pszTableName = ::GetCardTableName(idCardType);
	IF_NOT (pszTableName)
		return 0;

	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT id FROM %s WHERE account_id=%u", pszTableName, this->GetAccountID());
	return __max(0, Database()->CountRecord(szSQL));
}

bool CUser::FuseItem(OBJID* pidItem, int nSize, int nType)
{
	IF_NOT (pidItem || nSize <= 0)
		return false;

	if (nSize > 20)
		nSize = 20;

	int i = 0;
	for (i = 0; i < nSize; i++)
	{
		CItem* pItem = GetItem(pidItem[i]);
		if (!pItem)
			break;
		if (pItem->IsPlunder())
		{
			this->SendSysMsg(STR_FUSE_DISABLE);
			break;
		}
		if (pItem->TestSpecialFlag(2) || pItem->TestSpecialFlag(1))
		{
			this->SendSysMsg(STR_FUSE_LOCK_DISABLE);
			break;
		}
		if (IsItemProtected(pItem))
			break;
		if (pItem->IsEquipment() && pItem->GetQuality() <= 5
				&& pItem->GetInt(ITEMDATA_AMOUNT) > pItem->GetInt(ITEMDATA_AMOUNTLIMIT)/2)
			break;
		if (pItem->IsComposeStone())
			break;
		if (pItem->GetInt(ITEMDATA_MAGIC3) > 0)
			break;
	}
	if (i < nSize)
		return false;

	int nScore = 0;
	int nAddition = 0;
	bool bErased = true;
	for (i = 0; i < nSize; i++)
	{
		OBJID idItem = pidItem[i];
		CItem* pItem = GetItem(idItem);
		if (!pItem)
			continue;

		nScore += g_pFuseItem->GetItemScore(pItem);
		if (pItem->GetInt(ITEMDATA_MAGIC3) > nAddition)
			nAddition = pItem->GetInt(ITEMDATA_MAGIC3);
		if (nAddition > 9)
			nAddition = 9;

		bErased = bErased && this->EraseItem(idItem, SYNCHRO_TRUE);
	}

	int nChance = (int)(nScore / pow(3.0, (double)nAddition));
	if (bErased && ::RandGet(10000) <= nChance)
	{
		if (nType != 0)
			::LogSave("invalid fuse type:%d", nType);
		else
			this->AwardItem(730001 + nAddition, SYNCHRO_TRUE);
	}
	return true;
}

int CUser::FindEquipPos(int nPos, OBJID idxField, int nOpt, int nData)
{
	if (nPos == 0)
	{
		vector<int>	setPos;
		for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
		{
			CItem* pEquip = GetEquipItemByPos(i);
			if (!pEquip)
				continue;
			if (i == ITEMPOSITION_WEAPONL && CItem::GetItemSort(pEquip->GetInt(ITEMDATA_TYPE)) != ITEMSORT_SHIELD && !pEquip->IsWeapon())
				continue;

			int nField = __max(__min(idxField, (OBJID)20), (OBJID)1);
			bool bMatch;
			switch(nOpt)
			{
			case 1:		bMatch = pEquip->GetInt((ITEMDATA)nField) > nData;		break;
			case 2:		bMatch = pEquip->GetInt((ITEMDATA)nField) >= nData;		break;
			case 3:		bMatch = pEquip->GetInt((ITEMDATA)nField) < nData;		break;
			case 4:		bMatch = pEquip->GetInt((ITEMDATA)nField) <= nData;		break;
			default:	bMatch = pEquip->GetInt((ITEMDATA)nField) == nData;		break;
			}
			if (bMatch)
				setPos.push_back(i);
		}
		if (setPos.size() == 0)
			return -1;
		return setPos[::RandGet((int)setPos.size())];
	}

	CItem* pEquip = GetEquipItemByPos(nPos);
	if (!pEquip)
		return -1;

	int nField = __max(__min(idxField, (OBJID)20), (OBJID)1);
	bool bMatch;
	switch(nOpt)
	{
	case 1:		bMatch = pEquip->GetInt((ITEMDATA)nField) > nData;		break;
	case 2:		bMatch = pEquip->GetInt((ITEMDATA)nField) >= nData;		break;
	case 3:		bMatch = pEquip->GetInt((ITEMDATA)nField) < nData;		break;
	case 4:		bMatch = pEquip->GetInt((ITEMDATA)nField) <= nData;		break;
	default:	bMatch = pEquip->GetInt((ITEMDATA)nField) == nData;		break;
	}
	if (bMatch)
		return nPos;
	return -1;
}

bool CUser::EnhanceItem(OBJID idItem, OBJID idGem)
{
	CItem* pItem = GetItem(idItem);
	if (!pItem)
	{
		this->SendSysMsg("target item not found!");
		return false;
	}
	if (!pItem->IsEquipment())
	{
		this->SendSysMsg("target item is not equipment.");
		return false;
	}
	CItem* pGem = GetItem(idGem);
	if (!pGem)
	{
		this->SendSysMsg("assistance item not found!");
		return false;
	}
	if (!pGem->IsGem())
	{
		this->SendSysMsg("assistance item is not gem!");
		return false;
	}

	int nKind	= (pGem->GetInt(ITEMDATA_TYPE)/10) % 10;
	int nGrade	= pGem->GetInt(ITEMDATA_TYPE) % 10;
	int nLife	= 0;
	switch(nKind)
	{
	case 0:
		if (nGrade == 1)		nLife = ::RandGet(59) + 1;
		else if (nGrade == 2)	nLife = ::RandGet(50) + 60;
		else					nLife = ::RandGet(60) + 170;
		break;
	case 1:
		if (nGrade == 1)		nLife = ::RandGet(59) + 1;
		else if (nGrade == 2)	nLife = ::RandGet(60) + 100;
		else					nLife = ::RandGet(56) + 200;
		break;
	case 2:
		if (nGrade == 1)		nLife = ::RandGet(59) + 1;
		else if (nGrade == 2)	nLife = ::RandGet(50) + 40;
		else					nLife = ::RandGet(60) + 90;
		break;
	case 3:
		if (nGrade == 1)		nLife = ::RandGet(59) + 1;
		else if (nGrade == 2)	nLife = ::RandGet(50) + 80;
		else					nLife = ::RandGet(60) + 170;
		break;
	case 4:
		if (nGrade == 1)		nLife = ::RandGet(59) + 1;
		else if (nGrade == 2)	nLife = ::RandGet(50) + 40;
		else					nLife = ::RandGet(50) + 70;
		break;
	case 5:
	case 6:
		if (nGrade == 1)		nLife = ::RandGet(59) + 1;
		else if (nGrade == 2)	nLife = ::RandGet(50) + 60;
		else					nLife = ::RandGet(60) + 140;
		break;
	case 7:
		if (nGrade == 1)		nLife = ::RandGet(59) + 1;
		else if (nGrade == 2)	nLife = ::RandGet(50) + 40;
		else					nLife = ::RandGet(60) + 170;
		break;
	}

	if (!this->EraseItem(idGem, SYNCHRO_TRUE))
		return false;

	CMsgItem msg;
	if (msg.Create(idItem, ITEMACT_ENHANCE, nLife))
		this->SendMsg(&msg);

	int nOldLife = pItem->GetInt(ITEMDATA_ADD_LIFE);
	if (nLife > nOldLife)
	{
		pItem->SetInt(ITEMDATA_ADD_LIFE, nLife, UPDATE_FALSE);
		pItem->SaveInfo();

		CMsgItemInfo info;
		if (info.Create(pItem, ITEMINFO_UPDATE))
			this->SendMsg(&info);
	}

	::MyLogSave("gmlog/enhance.log", "%s(%u) enhance item(%u) cost gem(%u) get %u with org(%u)",
				this->GetName(), this->GetID(), idItem, idGem, nLife, nOldLife);
	return true;
}

bool CUser::IsBackPackSpare(int nNum, int nWeight)
{
	if (nWeight > 0 && GetWeight() + nWeight > GetWeightLimit())
		return false;
	if (nNum > 0 && m_setItem.size() + nNum > _MAX_USERITEMSIZE)
		return false;
	return true;
}
