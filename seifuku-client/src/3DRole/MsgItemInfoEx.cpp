#include "MsgItemInfoEx.h"
#include "GamePlayerSet.h"
#include "Hero.h"

CMsgItemInfoEx::CMsgItemInfoEx()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgItemInfoEx::~CMsgItemInfoEx()
{

}
BOOL CMsgItemInfoEx::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_ITEMINFOEX != this->GetType())
		return false;

	return true;
}

void CMsgItemInfoEx::Process(void* pInfo)
{
	switch(m_pInfo->ucAction)
	{
	case ITEMINFOEX_BOOTH:
		{
			if(g_objHero.m_objBooth.GetID() == m_pInfo->idOwner)
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
				pItem->SetOwnerID(m_pInfo->idOwner);
				pItem->SetBoothSellPrice(m_pInfo->dwPrice);
				pItem->SetBless(m_pInfo->ucBless);
				pItem->SetEnchant(m_pInfo->ucEnchant);
				pItem->SetRestrain(m_pInfo->dwRestrain);

				g_objHero.m_objBooth.AddItem(pItem);
			}
		}
		break;

	case ITEMINFOEX_BOOTH_EMONEY:
		{
			if(g_objHero.m_objBooth.GetID() == m_pInfo->idOwner)
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
				pItem->SetOwnerID(m_pInfo->idOwner);
				pItem->SetBoothSellEmoneyPrice(m_pInfo->dwPrice);
				pItem->SetBless(m_pInfo->ucBless);
				pItem->SetEnchant(m_pInfo->ucEnchant);
				pItem->SetRestrain(m_pInfo->dwRestrain);

				g_objHero.m_objBooth.AddItem(pItem);
			}
		}
		break;

	case ITEMINFOEX_EQUIPMENT:
		{
			CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_pInfo->idOwner, false);
			if (pPlayer)
			{
				switch(m_pInfo->ucPosition)
				{
				case ITEMPOSITION_HELMET:
					pPlayer->SetArmet(m_pInfo->dwType);
					break;

				case ITEMPOSITION_ARMOR:
					pPlayer->SetArmor(m_pInfo->dwType);
					break;

				case ITEMPOSITION_WEAPONR:
					pPlayer->SetRWeapon(m_pInfo->dwType);
					pPlayer->SetWeaponRestrain(m_pInfo->dwRestrain, -1);
					break;

				case ITEMPOSITION_WEAPONL:
					pPlayer->SetLWeapon(m_pInfo->dwType);
					pPlayer->SetWeaponRestrain(-1, m_pInfo->dwRestrain);
					break;
				}
			}
			else if (m_pInfo->idOwner == g_objHero.GetID())
			{
				CItem* pItem = g_objHero.SearchEquipment(m_pInfo->id);
				if (!pItem)
					return;

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

				g_objHero.SetEquipment(m_pInfo->ucPosition, pItem);
				::PostCmd(CMD_FLASHEQUIPMENT);
			}
		}
		break;
	}

}
