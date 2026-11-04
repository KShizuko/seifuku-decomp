#include "Trade.h"
#include "Hero.h"
#include "GamePlayerSet.h"

CTrade::CTrade()
{
	m_bHeroAgree		=	false;
	m_bPlayerAgree		=	false;
	m_bActive			=	false;

	m_dwHeroMoney		=	0;
	m_dwHeroEmoney		=	0;
	m_dwPlayerMoney		=	0;
	m_dwPlayerEmoney	=	0;
	m_dwPlayerLevel		=	0;

	m_idPlayer			=	ID_NONE;

	m_setHeroItem.clear();
	m_setPlayerItem.clear();
}

CTrade::~CTrade()
{
	this->Destroy();
}

void CTrade::Destroy()
{
	m_bHeroAgree		=	false;
	m_bPlayerAgree		=	false;
	m_bActive			=	false;

	m_idPlayer			=	ID_NONE;

	m_dwHeroEmoney		=	0;
	m_dwHeroMoney		=	0;
	m_dwPlayerEmoney	=	0;
	m_dwPlayerMoney		=	0;

	int nAmount, i;

	nAmount = m_setHeroItem.size();
	for(i = 0; i < nAmount; i ++)
	{
		CItem* pItem = m_setHeroItem[i];
		SAFE_DELETE(pItem);
	}
	m_setHeroItem.clear();

	nAmount = m_setPlayerItem.size();
	for(i = 0; i < nAmount; i ++)
	{
		CItem* pItem = m_setPlayerItem[i];
		SAFE_DELETE(pItem);
	}
	m_setPlayerItem.clear();
}

void CTrade::Close(BOOL	bSuc)
{
	int i, nAmount;
	if(bSuc)
	{
		nAmount = m_setHeroItem.size();
		for(i = 0; i < nAmount; i ++)
		{
			CItem* pItem = m_setHeroItem[i];
			SAFE_DELETE(pItem);
		}
		m_setHeroItem.clear();

		nAmount = m_setPlayerItem.size();
		for(i = 0; i < nAmount; i ++)
		{
			CItem* pItem = m_setPlayerItem[i];
			g_objHero.AddItem(pItem);
		}
		m_setPlayerItem.clear();

		g_objHero.SetMoney(g_objHero.GetMoney() + m_dwPlayerMoney);
	}
	else
	{
		nAmount = m_setHeroItem.size();
		for(i = 0; i < nAmount; i ++)
		{
			CItem* pItem = m_setHeroItem[i];
			g_objHero.AddItem(pItem);
		}
		m_setHeroItem.clear();

		g_objHero.SetMoney(g_objHero.GetMoney() + m_dwHeroMoney);
		g_objHero.SetEmoney(g_objHero.GetEmoney() + m_dwHeroEmoney);

		nAmount = m_setPlayerItem.size();
		for(i = 0; i < nAmount; i ++)
		{
			CItem* pItem = m_setPlayerItem[i];
			SAFE_DELETE(pItem);
		}
		m_setPlayerItem.clear();
	}

	m_bHeroAgree		=	false;
	m_bPlayerAgree		=	false;
	m_bActive			=	false;
	m_dwHeroEmoney		=	0;
	m_dwHeroMoney		=	0;
	m_dwPlayerEmoney	=	0;
	m_dwPlayerMoney		=	0;

}

CItem*	CTrade::GetHeroItemByIndex(int nIndex)
{
	int nAmount = this->GetHeroItemAmount();
	if((nIndex < 0) || (nIndex >= nAmount))
		return NULL;
	return m_setHeroItem[nIndex];
}
CItem*	CTrade::GetPlayerItemByIndex(int nIndex)
{
	int nAmount = this->GetPlayerItemAmount();
	if((nIndex < 0) || (nIndex >= nAmount))
		return NULL;
	return m_setPlayerItem[nIndex];
}
void CTrade::SetPlayerMoney(DWORD dwMoney)
{
	m_dwPlayerMoney += dwMoney;
	::PostCmd(CMD_TRADEPLAYERITEM);
}
void CTrade::SetPlayerEmoney(DWORD dwMoney)
{
	m_dwPlayerEmoney += dwMoney;
	::PostCmd(CMD_TRADEPLAYERITEM);
}
void CTrade::AddHeroMoney(DWORD dwMoney)
{
	m_dwHeroMoney += dwMoney;
}
void CTrade::AddHeroEmoney(DWORD dwEmoney)
{
	m_dwHeroEmoney += dwEmoney;
}
void CTrade::AddHeroItem(CItem* pItem)
{
	if(!pItem)
		return;
	m_setHeroItem.push_back(pItem);
}
void CTrade::DelHeroItem(OBJID idItem, BOOL bDel)
{
	int nAmount = this->GetPlayerItemAmount();
	for(int i = 0; i < nAmount; i ++)
	{
		CItem* pItem = m_setHeroItem[i];
		if(pItem && pItem->GetID() == idItem)
		{
			if(bDel)
				SAFE_DELETE(pItem);
			m_setHeroItem.erase(m_setHeroItem.begin()+i);
			return;
		}
	}
}

void CTrade::AddPlayerItem(CItem* pItem)
{
	if(pItem)
	{
		m_setPlayerItem.push_back(pItem);
		::PostCmd(CMD_TRADEPLAYERITEM);
	}
}
void CTrade::Open(OBJID idPlayer)
{
	m_idPlayer = idPlayer;
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(idPlayer);
	if(pPlayer)
	{
		strcpy(m_szPlayerName, pPlayer->GetName());
		m_dwPlayerLevel = pPlayer->GetLev();
	}
	else
	{
		strcpy(m_szPlayerName, 0);
		m_dwPlayerLevel = 0;
	}

	m_bActive = true;
	this->Destroy();
}
void CTrade::SetLastApplyPlayerId(OBJID idplayer)
{
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(idplayer);
	MYASSERT(pPlayer);
	m_idLastApplyPlayer = idplayer;
	strcpy(m_szLastApplyPlayerName, pPlayer->GetName());
	::PostCmd(CMD_TRADEPLAYERAPPLY);
}
void CTrade::SetHeroMoney(DWORD dwMoney)
{
	m_dwHeroMoney = +dwMoney;
}
void CTrade::SetHeroEmoney(DWORD dwMoney)
{
	m_dwHeroEmoney = dwMoney;
}
void CTrade::AddItemFalse(OBJID idItem)
{
	int nAmount = m_setHeroItem.size();
	CItem* pItem = NULL;
	for(int i = 0; i < nAmount; i ++)
	{
		pItem = m_setHeroItem[i];
		if(pItem && pItem->GetID() == idItem)
		{
			m_setHeroItem.erase(m_setHeroItem.begin()+i);
			break;
		}
	}
	if(pItem)
		g_objHero.AddItem(pItem);
}
