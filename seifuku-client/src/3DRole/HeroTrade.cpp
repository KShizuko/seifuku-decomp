#include "Hero.h"
#include "GameMsg.h"
#include "AllMsg.h"
#include "GamePlayerSet.h"

void CHero::CloseTrade(BOOL	bSuc)
{
	m_objTrade.Close(bSuc);
	if(bSuc)
		::PostCmd(CMD_TRADEOK);
	else
		::PostCmd(CMD_TRADEFALSE);
}	
void CHero::OpenTrade(OBJID idPlayer)
{
	if(m_objTrade.IsActive())
		m_objTrade.Close(false);
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(idPlayer);
	if(!pPlayer)
	{
		g_objHero.QuitTrade();
		return;
	}
	m_objTrade.Open(idPlayer);
	::PostCmd(CMD_OPENTRADE);
}
BOOL CHero::IsTradeActive()
{
	return m_objTrade.IsActive();
}
DWORD CHero::GetHeroTradeMoney()
{
	return m_objTrade.GetHeroMoney();
}
DWORD CHero::GetHeroTradeEmoney()
{
	return m_objTrade.GetHeroEmoney();
}
DWORD CHero::GetPlayerTradeMoney()
{
	return m_objTrade.GetPlayerMoney();
}
CItem* CHero::GetHeroTradeItemByIndex(int nIndex)
{
	return m_objTrade.GetHeroItemByIndex(nIndex);
}
CItem* CHero::GetPlayerTradeItemByIndex(int nIndex)
{
	return m_objTrade.GetPlayerItemByIndex(nIndex);
}
char* CHero::GetPlayerTradeName()
{
	return m_objTrade.GetPlayerName();
}
DWORD CHero::GetPlayerTradeLevel()
{
	return m_objTrade.GetPlayerLevel();
}
void CHero::SetPlayerTradeLevel(DWORD dwLevel)
{
	m_objTrade.SetPlayerLevel(dwLevel);
}
OBJID CHero::GetPlayerTradeID()
{
	return m_objTrade.GetPlayerId();
}
void CHero::SetPlayerTradeMoney(DWORD dwMoney)
{
	if(m_objTrade.GetPlayerMoney() == dwMoney)
		return;
	m_objTrade.SetPlayerMoney(dwMoney);
	::PostCmd(CMD_TRADPLAYERMONEY);
}

void CHero::SetHeroTradeMoney(DWORD dwMoney)
{
	if(m_objTrade.GetHeroMoney() == dwMoney)
		return;
	m_objTrade.SetHeroMoney(dwMoney);
	::PostCmd(CMD_TRADHEROEMONEY);
}
void CHero::SetPlayerTradeEmoney(DWORD dwEmoney)
{
	if(m_objTrade.GetPlayerEmoney() == dwEmoney)
		return;
	m_objTrade.SetPlayerEmoney(dwEmoney);
	::PostCmd(CMD_TRADPLAYERMONEY);
}
void CHero::SetHeroTradeEmoney(DWORD dwEmoney)
{
	if(m_objTrade.GetHeroEmoney() == dwEmoney)
		return;
	m_objTrade.SetHeroEmoney(dwEmoney);
	::PostCmd(CMD_TRADHEROEMONEY);
}
DWORD CHero::GetPlayerTradeEmoney()
{
	return m_objTrade.GetPlayerEmoney();
}
void CHero::AddHeroTradeMoney(DWORD dwMoney)
{
	if(dwMoney == 0 || this->GetMoney() < dwMoney)
		return;
	CMsgTrade msg;
	if(msg.Create(dwMoney, _TRADE_ADDMONEY))
	{
		msg.Send();
		g_objHero.SetMoney(g_objHero.GetMoney()-dwMoney);
		m_objTrade.AddHeroMoney(dwMoney);
	}
	::PostCmd(CMD_TRADHEROEMONEY);
	::PostCmd(CMD_FLASHMONEY);
}
void CHero::AddHeroTradeEmoney(DWORD dwEmoney)
{
	if(dwEmoney == 0 || this->GetEmoney() < dwEmoney)
		return;
	CMsgTrade msg;
	if(msg.Create(dwEmoney, _TRADE_ADDEMONEY))
	{
		msg.Send();
		g_objHero.SetEmoney(g_objHero.GetEmoney()-dwEmoney);
		m_objTrade.AddHeroEmoney(dwEmoney);
	}
	::PostCmd(CMD_TRADHEROEMONEY);
	::PostCmd(CMD_FLASHMONEY);
}
void CHero::AddHeroTradeItem(CItem* pItem)
{
	if(pItem && !pItem->IsMonopolyItem())
	{
		CMsgTrade msg;
		if(msg.Create(pItem->GetID(), _TRADE_ADDITEM))
		{
			m_objTrade.AddHeroItem(pItem);
			g_objHero.DelItem(pItem->GetID(), false);
			msg.Send();
		}
		::PostCmd(CMD_FLASHITEM);
		::PostCmd(CMD_TRADEHEROITEM);
	}
}
void CHero::AddPlayerTradeItem(CItem* pItem)
{
	if(pItem)
	{
		m_objTrade.AddPlayerItem(pItem);
		::PostCmd(CMD_TRADEPLAYERITEM);
	}
}
void CHero::ApplyTrade(CPlayer* pPlayer)
{
	if(!pPlayer)
		return;
	CMsgTrade msg;
	if(msg.Create(pPlayer->GetID(), _TRADE_APPLY))
		msg.Send();
}
void CHero::AcceptTrade()
{
	CMsgTrade msg;
	if(msg.Create(m_objTrade.GetLastApplyPlayerId(), _TRADE_APPLY))
		msg.Send();
}
void CHero::AgreeTrade()
{
	CMsgTrade msg;
	if(msg.Create(g_objHero.GetID(), _TRADE_OK))
		msg.Send();
	m_objTrade.HeroAgree();
}
void CHero::AddApply(OBJID idPlayer)
{
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(idPlayer);
	if(!pPlayer)
		return;
	m_objTrade.SetLastApplyPlayerId(idPlayer);
}
void CHero::QuitTrade()
{
	CMsgTrade msg;
	if(msg.Create(g_objHero.GetID(), _TRADE_QUIT))
		msg.Send();
}
void CHero::TradePlayerAgree()
{
	m_objTrade.PlayerAgree();
	::PostCmd(CMD_TRADEPLAYERAGREE);	
}
void CHero::AddItemFalse(OBJID idItem)
{
	m_objTrade.AddItemFalse(idItem);
	::PostCmd(CMD_FLASHITEM);
	::PostCmd(CMD_TRADEHEROITEM);
}
char* CHero::GetApplyTradeName()
{
	return m_objTrade.GetLastApplyPlayerName();
}
int	CHero::GetHeroTradeItemAmount()
{
	return m_objTrade.GetHeroItemAmount();
}

int CHero::GetPlayerTradeItemAmount()
{
	return m_objTrade.GetPlayerItemAmount();
}
