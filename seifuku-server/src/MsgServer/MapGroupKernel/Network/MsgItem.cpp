#include "AllMsg.h"
#include "MsgItem.h"
#include "MapGroup.h"
#include "ItemType.h"

CMsgItem::CMsgItem()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgItem::~CMsgItem()
{

}

BOOL CMsgItem::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_ITEM != this->GetType())
		return false;

	return true;
}

BOOL CMsgItem::Create(OBJID id, int nAction, int nPosition )
{
	CHECKF(nAction);

	this->Init();
	m_pInfo->usAction	= nAction;
	m_pInfo->id			= id;
	m_pInfo->dwData		= nPosition;

	m_unMsgType	=_MSG_ITEM;
	m_unMsgSize	=sizeof(MSG_Info);

	return true;
}

void CMsgItem::Process(void *pInfo)
{
	CUserPtr pUser = UserManager()->GetUser(this);
	if(!pUser)
		return;

	if(!pUser->IsAlive())
	{
		pUser->SendSysMsg(STR_DIE);
		return ;
	}

	try {
	switch(m_pInfo->usAction)
	{
	case ITEMACT_BUY:
		{
			pUser->BuyItem(m_pInfo->id, m_pInfo->dwData, 1);
		}
		break;
	case ITEMACT_SELL:
		{
			pUser->SellItem(m_pInfo->id, m_pInfo->dwData, m_pInfo->idTarget ^ pUser->GetID());
		}
		break;
	case ITEMACT_DROP:
		{
			if(pUser->GetDistance(m_pInfo->usPosX, m_pInfo->usPosY) > 27)
			{
				pUser->SetCheatMark(9, 1);
				pUser->SetCheatKickout(RandGet(10));
				break;
			}

			CItem* pItem = pUser->GetItem(m_pInfo->id);
			if(pItem && pItem->IsPreciousItem())
			{
				pUser->SendSysMsg(STR_NOT_DISCARDABLE);
				break;
			}

			pUser->DropItem(m_pInfo->id, m_pInfo->usPosX, m_pInfo->usPosY);
		}
		break;
	case ITEMACT_DROPMONEY:
		{
			IF_NOT(pUser->GetDistance(m_pInfo->usPosX, m_pInfo->usPosY) <= USERDROPITEM_RANGE2)
				return ;

			if((int)m_pInfo->id < 100)
				break;

			pUser->DropMoney(m_pInfo->id, m_pInfo->usPosX, m_pInfo->usPosY);
		}
		break;
	case ITEMACT_USE:
		{
			if(!pUser->UseItem(m_pInfo->id, m_pInfo->dwData, SYNCHRO_TRUE))
				pUser->SendSysMsg(STR_UNABLE_USE_ITEM);
		}
		break;
	case ITEMACT_EQUIP:
		{
			ASSERT(!"ITEMACT_EQUIP");
		}
		break;
	case ITEMACT_UNEQUIP:
		{
			if(!pUser->UnEquipItem(m_pInfo->dwData, SYNCHRO_TRUE))
				pUser->SendSysMsg(STR_YOUR_BAG_FULL);
		}
		break;
	case ITEMACT_SPLITITEM:
		{
			if(!pUser->SplitItem(m_pInfo->id, m_pInfo->dwData))
				pUser->SendSysMsg(STR_YOUR_BAG_FULL);
		}
		break;
	case ITEMACT_COMBINEITEM:
		{
			pUser->CombineItem(m_pInfo->id, m_pInfo->dwData);
		}
		break;
	case ITEMACT_QUERYMONEYSAVED:
		{
			CNpc* pNpc;
			if(!pUser->GetMap()->QueryObj(pUser->GetPosX(), pUser->GetPosY(), OBJ_NPC, m_pInfo->id, IPP_OF(pNpc)))
				return ;
			if(!pNpc->IsStorageNpc() && !pNpc->IsTrunkNpc())
				return;

			m_pInfo->dwData		= pUser->GetMoneySaved();
			pUser->SendMsg(this);
		}
		break;
	case ITEMACT_SAVEMONEY:
		{
			CNpc* pNpc;
			if(!pUser->GetMap()->QueryObj(pUser->GetPosX(), pUser->GetPosY(), OBJ_NPC, m_pInfo->id, IPP_OF(pNpc)))
				return ;
			if(!pNpc->IsStorageNpc() && !pNpc->IsTrunkNpc())
				return;

			if(!pUser->SaveMoney(m_pInfo->dwData, SYNCHRO_TRUE))
				pUser->SendSysMsg(STR_NOT_SO_MUCH_MONEY);
		}
		break;
	case ITEMACT_DRAWMONEY:
		{
			CNpc* pNpc;
			if(!pUser->GetMap()->QueryObj(pUser->GetPosX(), pUser->GetPosY(), OBJ_NPC, m_pInfo->id, IPP_OF(pNpc)))
				return ;
			if(!pNpc->IsStorageNpc() && !pNpc->IsTrunkNpc())
				return;

			if(!pUser->DrawMoney(m_pInfo->dwData, SYNCHRO_TRUE))
				pUser->SendSysMsg(STR_NOT_SO_MUCH_MONEY);
		}
		break;
	case ITEMACT_SPENDMONEY:
		{
			ASSERT(!"ITEMACT_SPENDMONEY");
		}
		break;
	case ITEMACT_REPAIR:
		{
			if(!pUser->RepairItem(m_pInfo->id, SYNCHRO_TRUE, false))
				pUser->SendSysMsg(STR_REPAIR_FAILED);
		}
		break;
	case ITEMACT_IDENT:
		{
			pUser->IdentItem(m_pInfo->id, SYNCHRO_TRUE);
		}
		break;
	case ITEMACT_IMPROVE:
		{
			pUser->UpEquipmentQuality(m_pInfo->id, m_pInfo->dwData);
		}
		break;
	case ITEMACT_UPLEV:
		{
			pUser->UpEquipmentLevel(m_pInfo->id, m_pInfo->dwData);
		}
		break;
	case ITEMACT_BOOTH_QUERY:
		{
			CBooth* pBooth;
			if(!pUser->FindAroundObj(OBJ_BOOTH, m_pInfo->dwData, IPP_OF(pBooth)))
				return ;

			pBooth->SendGoods(pUser);
		}
		break;
	case ITEMACT_BOOTH_ADD:
		{
			if(pUser->QueryBooth())
			{
				if(pUser->QueryBooth()->AddItem(m_pInfo->id, m_pInfo->dwData, 0))
					pUser->SendMsg(this);
			}
		}
		break;
	case ITEMACT_BOOTH_ADD_EMONEY:
		{
			if(pUser->QueryBooth())
			{
				if(pUser->QueryBooth()->AddItem(m_pInfo->id, 0, m_pInfo->dwData))
					pUser->SendMsg(this);
			}
		}
		break;
	case ITEMACT_BOOTH_DEL:
		{
			if(pUser->QueryBooth())
				pUser->QueryBooth()->DelItem(m_pInfo->id);
		}
		break;
	case ITEMACT_BOOTH_BUY:
		{
			CBooth* pBooth;
			if(!pUser->FindAroundObj(OBJ_BOOTH, m_pInfo->dwData, IPP_OF(pBooth)))
				return ;

			pBooth->BuyItem(pUser, m_pInfo->id);
		}
		break;
	case ITEMACT_PING:
		{
			pUser->SendMsg(this);
		}
		break;
	case ITEMACT_ENHANCE:
		{
			pUser->EnhanceItem(m_pInfo->id, m_pInfo->dwData);
		}
		break;
	}
	}catch(...)
	{
		::LogSave("switch(MSGITEM) Action [%d], id [%u], data [%u] ", m_pInfo->usAction, m_pInfo->id, m_pInfo->dwData);
	}

#ifdef _MSGDEBUG
	::LogMsg("Process CMsgItem, id:%u", m_pInfo->id);
#endif
}
