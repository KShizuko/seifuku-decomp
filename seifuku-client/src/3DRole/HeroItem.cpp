#include "BaseFunc.h"
#include "Hero.h"
#include "GameMsg.h"
#include "AllMsg.h"
#include "GamePlayerSet.h"
#include "Shop.h"
#include "3DGameMap.h"
#include "GameDataSet.h"
#include "3DRoleData.h"
#include "NDsound.h"

CItem* CHero::GetItem(OBJID id, BOOL bExcludeEquipment)
{
	int nAmount = m_dequeItem.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CItem* pItem = m_dequeItem[i];
		if(pItem && pItem->GetID() == id)
			return pItem;
	}

	if(!bExcludeEquipment)
	{
		for (int i=0; i<_MAX_EQUIPMENT; i++)
		{
			if(m_pEquipment[i] && m_pEquipment[i]->GetID() == id)
				return m_pEquipment[i];
		}
	}
	return NULL;
}
BOOL CHero::AddItem(CItem* pItem)
{
	if(!pItem)
		return false;
	if(m_dequeItem.size() >= _MAX_HEROITEM)
		return false;
	if(this->GetItem(pItem->GetID()))
		return false;

	if(pItem->IsArrow() && pItem->GetAmount() < pItem->GetAmountLimit())
	{
		OBJID idTarget = ID_NONE;
		int nMinAmount = 999999999;
		int nAmount = m_dequeItem.size();
		for(int i = 0; i < nAmount; i ++)
		{
			CItem* pOldItem = this->GetItemByIndex(i);
			if(pOldItem && pOldItem->GetTypeID() == pItem->GetTypeID()
				&& pOldItem->GetAmount() < nMinAmount
				&& pOldItem->GetAmount() < pOldItem->GetAmountLimit())
			{
				idTarget = pOldItem->GetID();
				nMinAmount = pOldItem->GetAmount();
			}
		}
		if(idTarget != ID_NONE)
			g_objHero.CombinItem(pItem->GetID(), idTarget);
	}

	m_dequeItem.push_back(pItem);
	return true;
}
CItem* CHero::GetItemByIndex(UINT uIndex)
{
	if(uIndex >= m_dequeItem.size())
		return NULL;
	return m_dequeItem[uIndex];
}

BOOL CHero::DelItem(OBJID id, BOOL bDelete)
{
	int nAmount = m_dequeItem.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CItem* pItem = m_dequeItem[i];
		if(pItem && pItem->GetID() == id)
		{
			m_dequeItem.erase(m_dequeItem.begin() + i);
			if(bDelete)
				SAFE_DELETE(pItem);
			return true;
		}
	}

	int nItemCount = g_objHero.m_objBoothManager.GetItemAmount();
	for(int i = 0; i < nItemCount; i ++)
	{
		CItem* pItem = g_objHero.m_objBoothManager.GetItemByIndex(i);
		if(pItem && pItem->GetID() == id)
		{
			if(bDelete)
				SAFE_DELETE(pItem);
			g_objHero.m_objBoothManager.m_dequeItem.erase(g_objHero.m_objBoothManager.m_dequeItem.begin() + i);
		}
	}
	return false;
}

void CHero::ClearItem(void)
{
	int nAmount = m_dequeItem.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CItem* pItem = m_dequeItem[i];
		SAFE_DELETE(pItem);
	}
	m_dequeItem.clear();

	for (int i=0; i<_MAX_EQUIPMENT; i++)
	{
		SAFE_DELETE(m_pEquipment[i]);
	}
}

BOOL CHero::DropItem(OBJID id, int x, int y)
{
	if(g_objGameMap.GetType() & MAPTYPE_FAMILY)
	{
		OBJID idCanNotDrop = 100146;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idCanNotDrop));
		return false;
	}
	if (!this->GetItem(id))
	{
		OBJID idItemError = 100044;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idItemError));
		return false;
	}

	CMyPos posCell;
	g_objGameMap.Screen2Cell(x, y, posCell.x, posCell.y);

	CMyPos posHero;
	g_objHero.GetPos(posHero);
	int nDir = g_objGameMap.GetDirection(posHero.x, posHero.y, posCell.x, posCell.y);

	posCell.x = posHero.x+_DELTA_X[nDir];
	posCell.y = posHero.y+_DELTA_Y[nDir];

	CellInfo* pInfo = g_objGameMap.GetCell(posCell.x, posCell.y);
	if(!pInfo || g_objGameMap.GetMapItem(posCell) || g_objGameMap.GetGroundMask(pInfo) != 0)
	{
		int nX[9]={0, 1, -1, 0, 1, 1, -1, -1, 0};
		int nY[9]={0, 1, -1, 1, 0, -1, 1, 0, -1};
		int i;
		for(i = 0; i < 9; i ++)
		{
			CMyPos posItem;
			posItem.x = posHero.x + nX[i];
			posItem.y = posHero.y + nY[i];
			CellInfo* pInfo = g_objGameMap.GetCell(posItem.x, posItem.y);
			if(pInfo)
			{
				if(!g_objGameMap.GetMapItem(posItem) && g_objGameMap.GetGroundMask(pInfo) == 0 && !g_objPlayerSet.GetPlayer(posItem.x, posItem.y))
				{
					posCell = posItem;
					break;
				}
			}
		}

		if (i >= 9)
			return false;
	}

	CMsgItem msg;
	if(msg.Create(id, posCell.x, posCell.y, ITEMACT_DROP))
		msg.Send();

	return true;
}

void CHero::PickUpItem(OBJID id, int x, int y)
{
	if (id == ID_NONE)
		return;

	if(m_objBoothManager.IsActive())
		return;

	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(x, y);
	if (pPlayer && !pPlayer->IsDead())
		return;

	CCommand cmd;
	cmd.iType		=_COMMAND_PICKUP;
	cmd.iStatus		=_CMDSTATUS_BEGIN;
	cmd.idTarget	=id;
	cmd.posTarget.x =x;
	cmd.posTarget.y =y;

	this->SetCommand(&cmd);
}

void CHero::BuyItem(OBJID idType)
{
	if (g_objHero.GetItemAmount() >= _MAX_HEROITEM)
	{
		OBJID idPackageFull = 100045 ;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idPackageFull), _TXTATR_SHOP);
		return;
	}
	CMsgItem msg;
	if (msg.Create(g_objShop.GetShopID(), idType, ITEMACT_BUY))
		msg.Send();
}

void CHero::SellItem(OBJID id)
{
	if (!this->GetItem(id))
	{
		OBJID idItemError = 100044;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idItemError), _TXTATR_SHOP);
		return;
	}

	CMsgItem msg;
	if(msg.Create(g_objShop.GetShopID(), id, ITEMACT_SELL))
		msg.Send();
}

void CHero::RepairItem(OBJID id)
{
	if (id == ID_NONE)
		return;

	CMsgItem msg;
	if(msg.Create(id, 0, ITEMACT_REPAIR))
		msg.Send();
}

void CHero::UseItem(OBJID id)
{
	if (this->IsDead())
		return;


	CItem* pItem = this->GetItem(id);
	if (!pItem)
	{
		OBJID idItemError = 100044;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idItemError));
		return;
	}

	if (!this->TryItem(pItem))
		return;

	int nPosition = 0;
	switch(pItem->GetSort())
	{
	case ITEMSORT_EXPEND:
 		if(pItem->GetLife() > 0)
		{
			if (this->GetData(_PLAYER_LIFE) >= this->GetMaxLife())
			{
				g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(10462));
				::DXPlaySound("Sound/error.wav");
				return;
			}
			m_objCheck.AddLife();
		}
		else if (pItem->GetMana() > 0)
		{
			if (this->GetMana() >= this->GetMaxMana())
			{
				g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(10463));
				::DXPlaySound("Sound/error.wav");
				return;
			}
			m_objCheck.AddMana();
		}
		break;
	case ITEMSORT_HELMET:
		nPosition = ITEMPOSITION_HELMET;
		break;

	case ITEMSORT_NECKLACE:
		nPosition = ITEMPOSITION_NECKLACE;
		break;

	case ITEMSORT_ARMOR:
		nPosition = ITEMPOSITION_ARMOR;
		break;

	case ITEMSORT_WEAPON_SINGLE_HAND:
		if (m_Info.iActType >= _ACTION_DANCE1 && m_Info.iActType <= _ACTION_DANCE8)
		{
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(100046));
			return;
		}
		nPosition = ITEMPOSITION_WEAPONR;
		break;

	case ITEMSORT_WEAPON_DOUBLE_HAND:
		if (m_Info.iActType >= _ACTION_DANCE1 && m_Info.iActType <= _ACTION_DANCE8)
		{
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(100046));
			return;
		}
		if (this->GetEquipment(ITEMPOSITION_WEAPONL) && !this->HaveBow() && pItem->IsBow())
		{
			const OBJID idEqumipError = 100047;
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idEqumipError));
			return;
		}
		nPosition = ITEMPOSITION_WEAPONR;
		break;
	case ITEMSORT_SHIELD:
		if (m_Info.iActType >= _ACTION_DANCE1 && m_Info.iActType <= _ACTION_DANCE8)
		{
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(100046));
			return;
		}
		{
			CItem* pItem = this->GetEquipment(ITEMPOSITION_WEAPONR);
			if (pItem && ITEMSORT_WEAPON_DOUBLE_HAND == pItem->GetSort())
			{
				const OBJID idEqumipError = 100048;
				g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idEqumipError));
				return;
			}
		}

		nPosition = ITEMPOSITION_WEAPONL;
		break;

	case ITEMSORT_RING:
		nPosition = ITEMPOSITION_RINGR;
		break;

	case ITEMSORT_SHOES:
		nPosition = ITEMPOSITION_SHOES;
		break;

	case 10:
		nPosition = ITEMPOSITION_TREASURE;
		break;
	case 11:
		nPosition = 9;
		break;

	default:
		break;
	}

	const CMediaEffectInfo* pMedia = RoleDataQuery()->GetMediaEffectInfo(pItem->GetTypeID());
	if (pMedia)
	{
		if (pMedia->strEffect.length() > 0 && m_objEffect)
			m_objEffect->Add((char*)pMedia->strEffect.c_str());
		if (pMedia->strSound.length() > 0)
		{
			int nWorldX = 0, nWorldY = 0;
			this->GetWorldPos(nWorldX, nWorldY);
			::DXPlaySound((char*)pMedia->strSound.c_str(), nWorldX, nWorldY, 2000, 0, 100);
		}
	}

	CMsgItem msg;
	if(msg.Create(id, nPosition, ITEMACT_USE))
		msg.Send();
}
BOOL CHero::TryItem(CItem* pItem)
{
	if (!pItem)
		return false;

	if (pItem->IsTaskItem())
		return false;

	if (this->GetMetempsychosis() != 0 && pItem->GetLevelRequired() <= 70 && this->GetLev() >= 70)
		return true;

	if (pItem->GetLevelRequired() != 0 &&
			this->GetLev() < pItem->GetLevelRequired())
		return false;

	if (pItem->GetForceRequired() != 0 &&
			this->GetForce() < pItem->GetForceRequired())
		return false;

	if (pItem->GetSpeedRequired() != 0 &&
			this->GetSpeed() < pItem->GetSpeedRequired())
		return false;

	if (pItem->GetHealthRequired() != 0 &&
			this->GetHealth() < pItem->GetHealthRequired())
		return false;

	if (pItem->GetSoulRequired() != 0 &&
			this->GetSoul() < pItem->GetSoulRequired())
		return false;

	if (pItem->GetSexRequried() != 0 &&
			this->GetSex() != (int)pItem->GetSexRequried())
		return false;

	if (pItem->GetWeaponSkillRequired() != 0)
	{
		CWeaponSkill* pSkill = this->GetWeaponSkill((int)pItem->GetWeaponSkillType());
		if (!pSkill)
			return false;
		if (pSkill->GetLevel() < pItem->GetWeaponSkillRequired())
			return false;
	}
	int nReqProfession = pItem->GetProfessionRequired ();
	int nProfession = this->GetProfession();
	if ( nReqProfession != 0 )
	{
		int nRequireProfSort	= nReqProfession/10;
		int nRequireProfLevel	= nReqProfession%10;

		if (nRequireProfSort == 19)
		{
			if ((DWORD)(nProfession%1000)/10 < 10)
				return false;
		}
		else if (nRequireProfSort != (nProfession%1000)/10)
			return false;
		if (nProfession%10 < nRequireProfLevel)
			return false;
	}

	if (this->TestStatus(USERSTATUS_FLY))
	{
		if (pItem->GetSort() == ITEMSORT_WEAPON_SINGLE_HAND
			|| pItem->GetSort() == ITEMSORT_WEAPON_DOUBLE_HAND
			|| pItem->GetSort() == ITEMSORT_SHIELD)
		{
			if (!pItem->IsBow() && !pItem->IsArrow())
				return false;
		}
	}

	return true;
}
void CHero::SetEquipment (int nPosition, CItem* pItem)
{
	MYASSERT(nPosition-1>=0 && nPosition-1<_MAX_EQUIPMENT);

	int nType = nPosition-1;
	switch(nPosition)
	{
	case ITEMPOSITION_HELMET:
		m_pEquipment[nType] = pItem;
		if (pItem)
			this->SetArmet(pItem->GetTypeID());
		else
			this->SetArmet(ID_NONE);
		break;

	case ITEMPOSITION_NECKLACE:
		m_pEquipment[nType] = pItem;
		break;

	case ITEMPOSITION_ARMOR:
		m_pEquipment[nType] = pItem;
		if (pItem)
			this->SetArmor(pItem->GetTypeID());
		else
			this->SetArmor(ID_NONE);
		this->SetMetempsychosis(this->GetMetempsychosis());
		break;

	case ITEMPOSITION_WEAPONR:
		m_pEquipment[nType] = pItem;
		if (pItem)
		{
			if(pItem->GetSort() == ITEMSORT_WEAPON_DOUBLE_HAND)
			{
				CItem* pLItem = g_objHero.GetEquipment(ITEMPOSITION_WEAPONL);
				if(pItem->IsBow() && pLItem && pLItem->IsArrow())
				{
				}
				else
				{
					this->AddItem(m_pEquipment[nPosition]);
					m_pEquipment[nPosition] = NULL;
					this->SetLWeapon(ID_NONE);
					this->SetWeaponRestrain(-1, 0);
				}
			}
			this->SetRWeapon(pItem->GetTypeID());
		}
		else
		{
			this->SetRWeapon(ID_NONE);
		}
		this->SetWeaponRestrain(pItem ? pItem->GetRestrain() : 0, -1);
		break;

	case ITEMPOSITION_WEAPONL:
		m_pEquipment[nType] = pItem;
		if (pItem)
			this->SetLWeapon(pItem->GetTypeID());
		else
			this->SetLWeapon(ID_NONE);
		this->SetWeaponRestrain(-1, pItem ? pItem->GetRestrain() : 0);
		break;

	case ITEMPOSITION_RINGR:
		m_pEquipment[nType] = pItem;
		break;

	case ITEMPOSITION_TREASURE:
		m_pEquipment[nType] = pItem;
		break;

	case ITEMPOSITION_SHOES:
		m_pEquipment[nType] = pItem;
		break;

	case 9:
		m_pEquipment[nType] = pItem;
		if (pItem)
			this->SetMantle(pItem->GetTypeID());
		else
			this->SetMantle(ID_NONE);
		break;
	}
}

BOOL CHero::EquipItem (int nPosition, OBJID id)
{
	if (this->IsDead())
		return false;

	CItem* pItem	=this->GetItem(id);
	if (!pItem)
		return false;

	if (!this->TryItem(pItem))
		return false;

	switch(pItem->GetSort())
	{
	case ITEMSORT_HELMET:
		if (nPosition != ITEMPOSITION_HELMET)
			return false;
		break;

	case ITEMSORT_NECKLACE:
		if (nPosition != ITEMPOSITION_NECKLACE)
			return false;
		break;

	case ITEMSORT_ARMOR:
		if (nPosition != ITEMPOSITION_ARMOR)
			return false;
		break;

	case ITEMSORT_WEAPON_SINGLE_HAND:
		{
			if (m_Info.iActType >= _ACTION_DANCE1 && m_Info.iActType <= _ACTION_DANCE8)
			{
				g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(100030));
				return false;
			}
			if (nPosition != ITEMPOSITION_WEAPONR)
			{
				CItem* pEquipment = this->GetEquipment(ITEMPOSITION_WEAPONR);
				if (!pEquipment || pEquipment->GetSort() != ITEMSORT_WEAPON_SINGLE_HAND
					|| nPosition != ITEMPOSITION_WEAPONL
					|| (this->GetProfession()%1000)/10 != 1
					|| this->GetProfession()%10 < 2)
				{
					const OBJID idEqumipError = 100049;
					g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idEqumipError));
					return false;
				}
			}
			if (m_Info.iActType == _ACTION_DANCE1)
			{
				g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(100046));
				return false;
			}
		}
		break;

	case ITEMSORT_WEAPON_DOUBLE_HAND:
		{
			if (m_Info.iActType >= _ACTION_DANCE1 && m_Info.iActType <= _ACTION_DANCE8)
			{
				g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(100030));
				return false;
			}
			if (nPosition != ITEMPOSITION_WEAPONR)
			{
				const OBJID idEqumipError = 100049;
				g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idEqumipError));
				return false;
			}
			if (m_Info.iActType == _ACTION_DANCE1)
			{
				g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(100046));
				return false;
			}

			if (this->GetEquipment(ITEMPOSITION_WEAPONL) && !this->HaveBow() && pItem->IsBow())
			{
				const OBJID idEqumipError = 100047;
				g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idEqumipError));
				return false;
			}
		}
		break;

	case ITEMSORT_SHIELD:
		{
			if (m_Info.iActType >= _ACTION_DANCE1 && m_Info.iActType <= _ACTION_DANCE8)
			{
				g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(100030));
				return false;
			}
			if (nPosition != ITEMPOSITION_WEAPONL)
				return false;

			CItem* pEquipment = this->GetEquipment(ITEMPOSITION_WEAPONR);
			if (pEquipment && ITEMSORT_WEAPON_DOUBLE_HAND == pEquipment->GetSort())
			{
				const OBJID idEqumipError = 100048;
				g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idEqumipError));
				return false;
			}
		}
		break;

	case ITEMSORT_RING:
		if (nPosition != ITEMPOSITION_RINGR)
			return false;
		break;

	case ITEMSORT_SHOES:
		if (nPosition != ITEMPOSITION_SHOES)
			return false;
		break;

	case 10:
		if (nPosition != ITEMPOSITION_TREASURE)
			return false;
		break;
	case 11:
		if (nPosition != 9)
			return false;
		break;
	}

	CMsgItem msg;
	if (msg.Create(id, nPosition, ITEMACT_USE))
		msg.Send();

	return true;
}

BOOL CHero::UnequipItem(int nPosition)
{
	if (this->IsDead())
		return false;

	if (m_dequeItem.size() >= _MAX_HEROITEM)
	{
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(100052));
		return false;
	}

	if (nPosition == ITEMPOSITION_WEAPONR)
	{
		if (this->GetEquipment(ITEMPOSITION_WEAPONL))
		{
			const OBJID idUnequipLeftWeaponFirst = 100053;
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idUnequipLeftWeaponFirst));
			return false;
		}
	}

	CItem* pItem = this->GetEquipment(nPosition);
	if (!pItem)
		return false;

	CMsgItem msg;
	if(msg.Create(pItem->GetID(), nPosition, ITEMACT_UNEQUIP))
		msg.Send();

	return true;
}

CItem* CHero::SearchEquipment(OBJID idItem)
{
	if (ID_NONE == idItem)
		return NULL;

	for (int i=0; i<_MAX_EQUIPMENT; i++)
	{
		CItem* pEquip = m_pEquipment[i];
		if (pEquip && pEquip->GetID() == idItem)
			return pEquip;
	}

	return NULL;
}

BOOL CHero::CanEquip(int nPosition, CItem* pItem) const
{
	return true;
}
DWORD CHero::GetCarryingCapacity()
{
	return 10000;
}
DWORD CHero::GetItemTotalWeight()
{
	int nAmount = this->GetItemAmount();
	DWORD dwWeight = 0;
	for(int i = 0; i < nAmount; i ++)
	{
		CItem* pItem = this->GetItemByIndex(i);
		if(pItem)
			dwWeight += pItem->GetWeight();
	}
	return dwWeight;
}
int CHero::GetAtkRange()
{
	if (this->IsTransform())
		return 1;

	int nRange = 1, nRangeR = 0, nRangeL = 0;
	CItem* pRWeapon = this->GetEquipment(ITEMPOSITION_WEAPONR);
	if (pRWeapon)
		nRangeR = pRWeapon->GetAtkRange();

	CItem* pLWeapon = this->GetEquipment(ITEMPOSITION_WEAPONL);
	if (pLWeapon)
		nRangeL = pLWeapon->GetAtkRange();

	if(nRangeR && nRangeL)
		nRange = (nRangeR + nRangeL) / 2;
	else if(nRangeR)
		nRange = nRangeR;
	else if(nRangeL)
		nRange = nRangeL;

	int nAmount = m_setMagic.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CMagic* pMagic = m_setMagic[i];
		if (pMagic && (MAGICSORT_ATKRANGE == pMagic->m_infoMagicType.dwActionSort))
		{
			CItem* pWeaponR = this->GetEquipment(ITEMPOSITION_WEAPONR);
			CItem* pWeaponL = this->GetEquipment(ITEMPOSITION_WEAPONL);

			if ((pWeaponR && pWeaponR->GetWeaponSkillType() == pMagic->GetWeaponRequired())
					|| (pWeaponL && pWeaponL->GetWeaponSkillType() == pMagic->GetWeaponRequired()))
			{
				nRange += pMagic->GetPower();
			}
		}
	}

	return nRange;
}

void CHero::ImproveItem(OBJID idItem, OBJID idTreasure)
{
	CItem* pTreasure = this->GetItem(idTreasure);
	if (!pTreasure || !pTreasure->IsDragonBall())
		return;

	CItem* pItem = this->GetItem(idItem);
	if (!pItem || pItem->GetAmount() < pItem->GetAmountLimit())
		return;

	CMsgItem msg;
	IF_SUC (msg.Create(idItem, idTreasure, ITEMACT_IMPROVE))
		msg.Send();
}

void CHero::UpLevelItem(OBJID idItem, OBJID idTreasure)
{
	CItem* pTreasure = this->GetItem(idTreasure);
	if (!pTreasure || !(pTreasure->IsShootingStar() || pTreasure->GetTypeID() == CItem::TREASURE_SHOOTINGSTAR + 1))
		return;

	CItem* pItem = this->GetItem(idItem);
	if (!pItem || pItem->GetAmount() < pItem->GetAmountLimit())
		return;

	CMsgItem msg;
	IF_SUC (msg.Create(idItem, idTreasure, ITEMACT_UPLEV))
		msg.Send();
}

void CHero::EmbedGem	(OBJID idItem, OBJID idGem, int nPos)
{
	if (!this->GetItem(idItem))
		return;

	if (!this->GetItem(idGem))
		return;

	if (nPos != 1 && nPos != 2)
		return;

	CMsgGemEmbed msg;
	IF_OK (msg.Create(this->GetID(), idItem, idGem, nPos))
		msg.Send();
}

void CHero::TakeOutGem	(OBJID idItem, int nPos)
{
	if (!this->GetItem(idItem))
		return;

	if (nPos != 1 && nPos != 2)
		return;

	CMsgGemEmbed msg;
	IF_OK (msg.Create(this->GetID(), idItem, ID_NONE, nPos, GEM_TAKEOUT))
		msg.Send();
}

BOOL CHero::IsMiner(void)
{
	CItem* pWeapon = this->GetEquipment(ITEMPOSITION_WEAPONR);
	if (!pWeapon)
		return false;

	OBJID idType = pWeapon->GetTypeID();
	if (ID_NONE == idType || 562 != (idType/1000)%1000)
		return false;

	return true;
}

void CHero::Mine(void)
{
	if(this->IsTransform())
	{
		return;
	}
	CItem* pWeapon = this->GetEquipment(ITEMPOSITION_WEAPONR);
	if (!pWeapon)
	{
		OBJID idMineToolError = 100054;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idMineToolError));
		return;
	}

	OBJID idType = pWeapon->GetTypeID();
	if (ID_NONE == idType || 562 != (idType/1000)%1000)
	{
		OBJID idMineToolError = 100054;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idMineToolError));
		return;
	}

	if (_COMMAND_MINE == this->GetCommandType())
		return;

	CCommand cmd;
	cmd.iType	= _COMMAND_MINE;
	cmd.iStatus	= _CMDSTATUS_BEGIN;

	this->SetCommand(&cmd);

	CMsgAction msg;
	IF_SUC (msg.Create(this->GetID(), 0, 0, this->GetDir(), actionMine))
		msg.Send();
}

BOOL CHero::IsAllEquipmentNonsuch()
{
	BOOL bReturn = true;
	for (int i=0; i<_MAX_EQUIPMENT; i++)
	{
		if(!m_pEquipment[i])
		{
			bReturn = false;
		}
		else if(!m_pEquipment[i]->IsArrow())
		{
			if(!CItem::IsNonsuchItem(m_pEquipment[i]->GetTypeID()))
			{
				bReturn = false;
			}
		}
	}
	return bReturn;
}
BOOL CHero::SplitItem	(OBJID idItem, int nAmount)
{
	CItem* pItem = g_objHero.GetItem(idItem);
	if(!pItem)
		return false;
	if(nAmount <= 0)
		return false;
	if(pItem->GetSort() != ITEMSORT_EXPEND)
		return false;
	if(pItem->GetAmount() <= nAmount)
		return false;
	CMsgItem msg;
	if(msg.Create(idItem, nAmount, ITEMACT_SPLITITEM))
		msg.Send();
	return true;
}
BOOL CHero::CombinItem	(OBJID idItemSource, OBJID idItemTarget)
{
	CMsgItem msg;
	if(msg.Create(idItemSource, idItemTarget, ITEMACT_COMBINEITEM))
		msg.Send();
	return true;
}
void CHero::SendDataArray(OBJID id0, OBJID id1, OBJID id2, OBJID id3, OBJID id4)
{
	CMsgDataArray msgDataArray;
	msgDataArray.Create(id0, id1, id2, id3, id4);
	msgDataArray.Send();
}
void CHero::FuseItem(OBJID idItem, FUSE_INFO infoFuse)
{
	if(infoFuse.setItem.size() == 0)
		return;
	list<OBJID>::iterator it = infoFuse.setItem.begin();
	CMsgFuse msg;
	msg.Create(idItem, 0, *it);
	for(++it; it != infoFuse.setItem.end(); ++it)
		msg.Append(*it);
	msg.Send();
}
void CHero::EnchantItem(OBJID idItem, OBJID idGem)
{
	if(!this->GetItem(idItem) || !this->GetItem(idGem))
		return;
	CMsgItem msg;
	if(msg.Create(idItem, idGem, ITEMACT_ENCHANT))
		msg.Send();
}
