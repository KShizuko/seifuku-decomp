#include "AllMsg.h"
#include "Item.h"
#include "Hero.h"

CMsgItemInfo::CMsgItemInfo()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgItemInfo::~CMsgItemInfo()
{

}

BOOL CMsgItemInfo::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_ITEMINFO != this->GetType())
		return false;

	return true;
}

void CMsgItemInfo::Process(void* pInfo)
{
#ifdef _MSGDEBUG
	::LogSave("Process CMsgItemInfo, ItemID:%u, ItemType:%u, Action:%u",
						m_pInfo->id,
						m_pInfo->dwType,
						m_pInfo->ucAction);

#endif

	switch(m_pInfo->ucAction)
	{
	case ITEMINFO_OTHERPLAYER_EQUIPMENT:
		{
			if(m_pInfo->ucPosition == ITEMPOSITION_BACKPACK)
				return;
			CItem* pItem	=new CItem;
			if (!pItem || !pItem->Create(m_pInfo->id, m_pInfo->dwType))
				return;

			pItem->SetAmount(m_pInfo->usAmount);
			pItem->SetAmountLimit(m_pInfo->usAmountLimit);
			pItem->SetStatus(m_pInfo->ucStatus);
			pItem->SetGem1(m_pInfo->ucGem1);
			pItem->SetGem2(m_pInfo->ucGem2);
			pItem->SetMagic1(m_pInfo->ucMagic1);
			pItem->SetMagic2(m_pInfo->ucMagic2);
			pItem->SetMagic3(m_pInfo->ucMagic3);
			pItem->SetBless(m_pInfo->ucBless);
			pItem->SetEnchant(m_pInfo->ucEnchant);
			pItem->SetRestrain(m_pInfo->dwRestrain);

			if(!g_objHero.m_objPlayerDummy.SetEquipment(m_pInfo->ucPosition, pItem))
				SAFE_DELETE(pItem);
			::PostCmd(CMD_FLASH_OTHERPLAYER_EQUIPMENT);
		}
		break;
	case ITEMINFO_ADDITEM:
		{
			CItem* pItem	=new CItem;
			if (!pItem || !pItem->Create(m_pInfo->id, m_pInfo->dwType))
				return;

			pItem->SetAmount(m_pInfo->usAmount);
			pItem->SetAmountLimit(m_pInfo->usAmountLimit);
			pItem->SetStatus(m_pInfo->ucStatus);
			pItem->SetGem1(m_pInfo->ucGem1);
			pItem->SetGem2(m_pInfo->ucGem2);
			pItem->SetMagic1(m_pInfo->ucMagic1);
			pItem->SetMagic2(m_pInfo->ucMagic2);
			pItem->SetMagic3(m_pInfo->ucMagic3);
			pItem->SetBless(m_pInfo->ucBless);
			pItem->SetEnchant(m_pInfo->ucEnchant);
			pItem->SetRestrain(m_pInfo->dwRestrain);

			switch(m_pInfo->ucPosition)
			{
			case ITEMPOSITION_BACKPACK:
				{
					if(!g_objHero.AddItem(pItem))
					{
						SAFE_DELETE(pItem);
					}
					else
					{
						::PostCmd(CMD_FLASHITEM);
					}
				}
				break;
			default:
				g_objHero.SetEquipment(m_pInfo->ucPosition, pItem);
				break;
			}
		}
		break;

	case ITEMINFO_TRADE:
		{
			CItem* pItem	=new CItem;
			if (!pItem || !pItem->Create(m_pInfo->id, m_pInfo->dwType))
				return;

			pItem->SetAmount(m_pInfo->usAmount);
			pItem->SetAmountLimit(m_pInfo->usAmountLimit);
			pItem->SetStatus(m_pInfo->ucStatus);
			pItem->SetGem1(m_pInfo->ucGem1);
			pItem->SetGem2(m_pInfo->ucGem2);
			pItem->SetMagic1(m_pInfo->ucMagic1);
			pItem->SetMagic2(m_pInfo->ucMagic2);
			pItem->SetMagic3(m_pInfo->ucMagic3);
			pItem->SetBless(m_pInfo->ucBless);
			pItem->SetEnchant(m_pInfo->ucEnchant);
			pItem->SetRestrain(m_pInfo->dwRestrain);

			g_objHero.AddPlayerTradeItem(pItem);
		}
		break;

	case ITEMINFO_UPDATE:
		{
			CItem* pItem = g_objHero.GetItem(m_pInfo->id);
			if (!pItem)
			{
				pItem = g_objHero.SearchEquipment(m_pInfo->id);

				if (!pItem)
					return;
			}

			if (pItem->GetTypeID() != m_pInfo->dwType)
			{
				if (!pItem->Create(m_pInfo->id, m_pInfo->dwType))
					return;
			}

			pItem->SetAmount(m_pInfo->usAmount);
			pItem->SetAmountLimit(m_pInfo->usAmountLimit);
			pItem->SetStatus(m_pInfo->ucStatus);
			pItem->SetGem1(m_pInfo->ucGem1);
			pItem->SetGem2(m_pInfo->ucGem2);
			pItem->SetMagic1(m_pInfo->ucMagic1);
			pItem->SetMagic2(m_pInfo->ucMagic2);
			pItem->SetMagic3(m_pInfo->ucMagic3);
			pItem->SetBless(m_pInfo->ucBless);
			pItem->SetEnchant(m_pInfo->ucEnchant);
			pItem->SetRestrain(m_pInfo->dwRestrain);

			if (g_objHero.GetItem(m_pInfo->id))
				::PostCmd(CMD_FLASHITEM);
			else
			{
				g_objHero.SetEquipment(m_pInfo->ucPosition, pItem);
				::PostCmd(CMD_FLASHEQUIPMENT);
			}
		}
		break;
	default:
		break;
	}
}
