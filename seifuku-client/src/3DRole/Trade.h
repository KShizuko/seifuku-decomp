#if !defined(AFX_TRADE_H__C2A01AF1_0393_47A2_AC4B_08FF3DA86864__INCLUDED_)
#define AFX_TRADE_H__C2A01AF1_0393_47A2_AC4B_08FF3DA86864__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif


#include "Item.h"

const int _MAX_TRADEITEM = 20;

class CTrade
{
	friend struct TradeProbe;
public:
	CTrade();
	virtual ~CTrade();

private:
	DEQUE_ITEM	m_setHeroItem;
	DEQUE_ITEM	m_setPlayerItem;
	DWORD		m_dwHeroMoney;
	DWORD		m_dwHeroEmoney;
	DWORD		m_dwPlayerMoney;
	DWORD		m_dwPlayerEmoney;
	BOOL		m_bHeroAgree;
	BOOL		m_bPlayerAgree;
	BOOL		m_bActive;
	OBJID		m_idPlayer;
	char		m_szPlayerName[_MAX_NAMESIZE];
	DWORD		m_dwPlayerLevel;
	OBJID		m_idLastApplyPlayer;
	char		m_szLastApplyPlayerName[_MAX_NAMESIZE];
public:
	void	Destroy();

	void	SetPlayerMoney(DWORD dwMoney);
	void	SetPlayerEmoney(DWORD dwMoney);
	void	AddHeroMoney(DWORD dwMoney);
	void	AddHeroEmoney(DWORD dwEmoney);
	void	SetHeroMoney(DWORD dwMoney);
	void	SetHeroEmoney(DWORD dwMoney);
	void	DelHeroItem(OBJID idItem, BOOL bDel);
	void	AddHeroItem(CItem* pItem);
	void	AddPlayerItem(CItem* pItem);
	void	Close(BOOL	bSuc);
	void	Open(OBJID idPlayer);
	BOOL	IsHeroAgree(){return m_bHeroAgree;}
	BOOL	IsPlayerAgree(){return m_bPlayerAgree;}
	void	HeroAgree(){m_bHeroAgree = true;}
	void	PlayerAgree(){m_bPlayerAgree = true;}

	BOOL	IsActive(){return m_bActive;}

	OBJID	GetLastApplyPlayerId(){return m_idLastApplyPlayer;}
	void	SetLastApplyPlayerId(OBJID idplayer);

	char*	GetLastApplyPlayerName(){return m_szLastApplyPlayerName;}
	char*	GetPlayerName(){return m_szPlayerName;}
	DWORD	GetPlayerLevel(){return m_dwPlayerLevel;}
	void	SetPlayerLevel(DWORD dwLevel){m_dwPlayerLevel = dwLevel;}
	OBJID	GetPlayerId(){return m_idPlayer;}

	int		GetHeroItemAmount(){return m_setHeroItem.size();}
	int		GetPlayerItemAmount(){return m_setPlayerItem.size();}

	DWORD	GetHeroMoney(){return m_dwHeroMoney;}
	DWORD	GetHeroEmoney(){return m_dwHeroEmoney;}
	DWORD	GetPlayerMoney(){return m_dwPlayerMoney;}
	DWORD	GetPlayerEmoney(){return m_dwPlayerEmoney;}

	CItem*	GetHeroItemByIndex(int nIndex);
	CItem*	GetPlayerItemByIndex(int nIndex);

	void	AddItemFalse(OBJID idItem);
};

#endif
