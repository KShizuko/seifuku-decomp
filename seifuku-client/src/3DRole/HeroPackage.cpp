#include "Hero.h"
#include "GameMsg.h"
#include "AllMsg.h"
#include "GamePlayerSet.h"
#include "3DGameMap.h"
void CHero::OpenPackage(OBJID idPackage, int nType)
{
	const int _MAX_ITEM_AMOUNT = 20;
	int nMaxAmount = _MAX_ITEM_AMOUNT;
	if(g_objGameMap.GetType() & MAPTYPE_BOOTH_ENABLE)
		nMaxAmount *= 2;
	if(nType == PACKAGE_CHEST)
	{
		CItem* pItem = g_objHero.GetItem(idPackage, true);
		if(pItem)
			nMaxAmount = pItem->GetTypeID() % 10;
		else
			nMaxAmount = 0;
		m_objPackage.SetType(PACKAGE_CHEST);
	}
	else
	{
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(idPackage);
	if(pPlayer)
	{
		CMyPos posPlayer;
		pPlayer->GetPos(posPlayer.x, posPlayer.y);
		CMyPos posHero;
		this->GetPos(posHero.x, posHero.y);
		int nDir = g_objGameMap.GetDirection(posPlayer.x, posPlayer.y, posHero.x, posHero.y);
		pPlayer->m_pIRoleView->SetVariable(_VAR_DIRECT_DIR, nDir);
		if(pPlayer->GetRoleType() == _ROLE_STORAGE_NPC)
			m_objPackage.SetType(PACKAGE_STORAGE);
		else
			m_objPackage.SetType(PACKAGE_TRUNK);
	}
	}
	m_objPackage.Reset();
	m_objPackage.SetId(idPackage);
	m_objPackage.SetMaxItemAmount(nMaxAmount);
	g_objHero.QueryPackage();
}
int	CHero::GetPackageMaxItemAmount()
{
	return m_objPackage.GetMaxItemAmount();
}
void CHero::ClosePackage()
{
	m_objPackage.Reset();
}
int CHero::GetPackageItemAmount()
{
	return m_objPackage.GetItemAmount();
}
CItem* CHero::GetPackageItemByIndex(int nIndex)
{
	return m_objPackage.GetItemByIndex(nIndex);
}
void CHero::AddPackageItem(CItem* pItem)
{
	if(pItem)
		m_objPackage.AddItem(pItem);
}
int	CHero::GetPackageType()
{
	return m_objPackage.GetType();
}
void CHero::QueryPackage()
{

	CMsgPackage msg;
	if(msg.Create(MSGPACKAGE_QUERYLIST, m_objPackage.GetType(), m_objPackage.GetId(), ID_NONE))
		msg.Send();
}
void CHero::CheckInItem(OBJID idItem)
{
	CItem* pItem = this->GetItem(idItem);
	if(!pItem)
		return;
	if(pItem->IsMonopolyItem())
		return;
	CMsgPackage msg;
	if(msg.Create(MSGPACKAGE_CHECKIN, m_objPackage.GetType(), m_objPackage.GetId(), idItem))
		msg.Send();

	::PostCmd(CMD_FLASHPACKAGEITEM);
}
void CHero::CheckOutItem(OBJID idItem)
{
	CItem* pItem = m_objPackage.GetItem(idItem);
	if(!pItem)
		return;

	CMsgPackage msg;
	if(msg.Create(MSGPACKAGE_CHECKOUT, m_objPackage.GetType(), m_objPackage.GetId(), idItem))
		msg.Send();

	::PostCmd(CMD_FLASHPACKAGEITEM);
}
void CHero::ResetPackage()
{
	m_objPackage.Reset();
}
void CHero::DelPackageItem(OBJID idItem)
{
	m_objPackage.DelItem(idItem);
}
