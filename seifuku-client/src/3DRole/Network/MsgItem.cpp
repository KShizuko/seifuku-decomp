#include "AllMsg.h"
#include "Hero.h"
#include "GameMsg.h"
#include "GameDataSet.h"
#include "SkyLayer.h"
#include "3DGameMap.h"
#include "GamePlayerSet.h"

CMsgItem::CMsgItem()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgItem::~CMsgItem()
{

}

BOOL CMsgItem::Create(OBJID id, DWORD dwData, int nAction)
{
	this->Init();

	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_ITEM;

	m_pInfo->id			=id;
	m_pInfo->dwData		=dwData;
	m_pInfo->usAction	=nAction;
	m_pInfo->dwTimeStamp	=::TimeGet() ^ g_objHero.GetID();

	return true;
}

BOOL CMsgItem::Create(OBJID id, USHORT usPosX, USHORT usPosY, int nAction)
{
	this->Init();

	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_ITEM;

	m_pInfo->id			=id;
	m_pInfo->usPosX		=usPosX;
	m_pInfo->usPosY		=usPosY;
	m_pInfo->usAction	=nAction;
	m_pInfo->dwTimeStamp	=::TimeGet() ^ g_objHero.GetID();

	return true;
}

BOOL CMsgItem::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;
	if(_MSG_ITEM != this->GetType())
		return false;

	return true;
}

void CMsgItem::Process(void* pInfo)
{
#ifdef _MSGDEBUG
	::LogSave("Process MsgItem, ItemID:%d, ItemAction:%d",
								m_pInfo->id,
								m_pInfo->nAction);
#endif
	switch(m_pInfo->usAction)
	{
	case ITEMACT_DROP:
		{
			g_objHero.DelItem(m_pInfo->id, true);
			::PostCmd(CMD_FLASHITEM);
		}
		break;
	case ITEMACT_QUERYMONEYSAVED:
		{
			g_objHero.SetSavedMoney(m_pInfo->dwData);
		}
		break;
	case ITEMACT_EQUIP:
		{
			CItem* pNewEquipment = g_objHero.GetItem(m_pInfo->id);
			if (pNewEquipment)
			{
				g_objHero.DelItem(pNewEquipment->GetID(), _DEL_PT);
				int nPosition = m_pInfo->dwData;
				CItem* pItem = g_objHero.GetEquipment(nPosition);
				if (pItem)
					g_objHero.AddItem(pItem);
				if(pItem && !pNewEquipment->IsBow() && pItem->IsBow() && m_pInfo->dwData == ITEMPOSITION_WEAPONR)
				{
					CItem* pLItem = g_objHero.GetEquipment(ITEMPOSITION_WEAPONL);
					if(pLItem && pLItem->IsArrow())
						g_objHero.SetEquipment(ITEMPOSITION_WEAPONL, NULL);
					g_objHero.AddItem(pLItem);
				}
				g_objHero.SetEquipment(nPosition, pNewEquipment);
				::PostCmd(CMD_FLASHITEM);
				::PostCmd(CMD_FLASHEQUIPMENT);
			}
		}
		break;
	case ITEMACT_UNEQUIP:
		{
			CItem* pItem = g_objHero.GetEquipment(m_pInfo->dwData);
			if (pItem)
			{
				if(pItem->IsBow() && m_pInfo->dwData == ITEMPOSITION_WEAPONR)
				{
					CItem* pLItem = g_objHero.GetEquipment(ITEMPOSITION_WEAPONL);
					if(pLItem && pLItem->IsArrow())
						g_objHero.SetEquipment(ITEMPOSITION_WEAPONL, NULL);
					g_objHero.AddItem(pLItem);
				}
				int nPosition = m_pInfo->dwData;
				g_objHero.SetEquipment(nPosition, NULL);
				g_objHero.AddItem(pItem);
				DWORD dwMaxLife = g_objHero.GetMaxLife();
				if (g_objHero.GetData(CPlayer::_PLAYER_LIFE) > dwMaxLife)
				{
					g_objHero.SetLife(dwMaxLife);
					g_objHero.SetData(CPlayer::_PLAYER_LIFE, dwMaxLife);
					g_objHero.SetData(CPlayer::_PLAYER_CURRENTLIFE, dwMaxLife);
				}
				::PostCmd(CMD_FLASHITEM);
				::PostCmd(CMD_FLASHEQUIPMENT);
			}
		}
		break;
	case ITEMACT_DURABILITY:
		{
			int nPosition = m_pInfo->id;
			CItem* pItem = g_objHero.GetEquipment(nPosition);
			if (pItem)
			{
				int nDurability = m_pInfo->dwData;
				if (nDurability <= 0)
				{
					const OBJID idAttaint = 100105;
					g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idAttaint), pItem->GetName());
					g_objHero.SetEquipment(nPosition, NULL);
					SAFE_DELETE (pItem);
				}
				else
				{
					const OBJID idAttaint = 100106;
					if (nDurability <= 100 && pItem->GetAmount() > nDurability)
					{
						g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idAttaint), pItem->GetName());
						g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idAttaint), pItem->GetName());
					}
					pItem->SetAmount(nDurability);
				}
				::PostCmd(CMD_FLASHEQUIPMENT);
			}
		}
		break;
	case ITEMACT_DROPEQUIPMENT:
		{
			int nPosition = m_pInfo->dwData;
			CItem* pItem = g_objHero.GetEquipment(nPosition);
			if (pItem)
			{
				MYASSERT (pItem->GetID() == m_pInfo->id);
				g_objHero.SetEquipment(nPosition, NULL);
				SAFE_DELETE (pItem);
				::PostCmd(CMD_FLASHEQUIPMENT);
			}
		}
		break;
	case ITEMACT_BOOTH_ADD:
		{
			g_objHero.m_objBoothManager.AddItem(m_pInfo->id, m_pInfo->dwData, false, false);
		}
		break;
	case ITEMACT_BOOTH_ADD_EMONEY:
		{
			g_objHero.m_objBoothManager.AddItem(m_pInfo->id, m_pInfo->dwData, false, true);
		}
		break;
	case ITEMACT_BOOTH_DEL:
		{
			if(m_pInfo->dwData == g_objHero.m_objBooth.GetID())
			{
				g_objHero.m_objBooth.DelItem(m_pInfo->id);
			}
			if(m_pInfo->dwData == g_objHero.m_objBoothManager.GetID())
			{
				g_objHero.m_objBoothManager.DelItem(m_pInfo->id, false);
			}
		}
		break;
	case ITEMACT_BOOTH_BUY:
		{
			if(m_pInfo->dwData == g_objHero.m_objBooth.GetID())
			{
				g_objHero.m_objBooth.DelItem(m_pInfo->id);
			}
		}
		break;
	case ITEMACT_FIREWORKS:
		{
				CSkyLayer *pSky = g_objGameMap.GetSkyLayer();
				pSky->CreateFireWork(rand()%200 + CMyBitmap::GetScreenWidth()/2 - 100 , CMyBitmap::GetScreenHeight()/2 -rand()%200 ,260+ rand()%160,rand()%5+1,0);
				pSky->CreateFireWork(rand()%200 + CMyBitmap::GetScreenWidth()/2 - 100 , CMyBitmap::GetScreenHeight()/2 -rand()%200 ,260+ rand()%120,rand()%5+1,700+rand()%300);
				pSky->CreateFireWork(rand()%200 + CMyBitmap::GetScreenWidth()/2 - 100 , CMyBitmap::GetScreenHeight()/2 -rand()%200 ,200+ rand()%300,rand()%5+1,1000+rand()%600);
		}
		break;
	case ITEMACT_SYNCHRO_AMOUNT:
		{
			CItem* pItem = g_objHero.GetItem(m_pInfo->id, false);
			if(pItem)
				pItem->SetAmount(m_pInfo->dwData);
			::PostCmd(CMD_FLASHITEM);
			::PostCmd(CMD_FLASHEQUIPMENT);
		}
		break;
	case ITEMACT_PING:
		{
			g_objHero.m_dwPing = ::TimeGet() - m_pInfo->dwTimeStamp;
		}
		break;
	case ITEMACT_ENCHANT:
		{
			g_bKoShowRecord = true;
			CItem* pItem = g_objHero.GetItem(m_pInfo->id);
			if (!pItem)
				break;
			OBJID idStr;
			if (m_pInfo->dwData > pItem->GetEnchant())
			{
				g_objHero.GetKillCounter()->SetZoomInPrefix("CountB");
				idStr = 10473;
			}
			else
			{
				g_objHero.GetKillCounter()->SetZoomInPrefix("CountK");
				idStr = 10474;
			}
			g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idStr), m_pInfo->dwData);
			char szNumber[256] = "";
			sprintf(szNumber, "%u", m_pInfo->dwData);
			g_objHero.GetKillCounter()->Flash(szNumber);
		}
		break;
	}
}
