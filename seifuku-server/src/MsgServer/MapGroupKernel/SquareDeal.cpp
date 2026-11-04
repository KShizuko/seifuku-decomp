#include "AllMsg.h"
#include "windows.h"
#include "SquareDeal.h"
#include "Item.h"
#include "User.h"
#include "TradeBuddy.h"
#include <algorithm>
using namespace std;

MYHEAP_IMPLEMENTATION(CSquareDeal,s_heap)
const int	MAX_SQUAREDEALITEMS		= 20;
const int	MAX_SQUAREDEALMONEY		= 100000000;

CSquareDeal::CSquareDeal()
{
	m_iface1.m_pParent	= this;
	m_iface2.m_pParent	= this;

	m_pUser1			= NULL;
	m_pUser2			= NULL;
	m_nAllWeight1		= 0;
	m_nAllWeight2		= 0;
	m_nMoney1			= 0;
	m_nMoney2			= 0;
	m_nEmoney1			= 0;
	m_nEmoney2			= 0;
	m_bClickOK1			= false;
	m_bClickOK2			= false;
	m_bUnfairOK1		= false;
	m_bUnfairOK2		= false;
	m_bTradeSuccess		= false;
}

CSquareDeal::~CSquareDeal()
{
	if(!m_bTradeSuccess)
	{
		CUser*	pUser1	= m_pUser1->QueryUser();
		CUser*	pUser2	= m_pUser2->QueryUser();
		IF_OK(pUser1 && pUser2)
		{
			CMsgTrade	msg;
			IF_OK(msg.Create(_TRADE_FALSE, 0))
			{
				pUser1->SendMsg(&msg);
				pUser2->SendMsg(&msg);
			}
			pUser1->SendSysMsg(STR_TRADE_FAIL);
			pUser2->SendSysMsg(STR_TRADE_FAIL);
		}
	}

	if(m_pUser1)
		m_pUser1->Release();
	if(m_pUser2)
		m_pUser2->Release();
}

bool CSquareDeal::TradeOK()
{
	CHECKF(m_pUser1 && m_pUser2);

	if(m_bTradeSuccess)
		return true;

	CUser*	pUser1	= m_pUser1->QueryUser();
	CUser*	pUser2	= m_pUser2->QueryUser();
	IF_NOT(pUser1 && pUser2)
		return false;
	IF_NOT(pUser1->GetMapID() == pUser2->GetMapID())
		return false;
	IF_NOT(pUser1->GetDistance(pUser2->GetPosX(), pUser2->GetPosY()) <= CELLS_PER_VIEW)
		return false;
	if(!pUser1->IsAlive() || !pUser2->IsAlive())
		return false;

	bool	bUnfair	= false;
	int		nValue1	= 0;
	int		nValue2	= 0;
	vector<CItem*>	setPlunder;
	CTradeBuddy* pBuddy = pUser1->GetTradeBuddy(pUser2->GetID());
	if((!pBuddy || pBuddy->GetDate()) && !pUser1->IsGM() && !pUser2->IsGM())
	{
		vector<CItem*>	setValuable;
		for(ITEM_SET::iterator it = m_setItem1.begin(); it != m_setItem1.end(); it++)
		{
			CItem* pItem = pUser1->GetItem(*it);
			if(!pItem)
				continue;
			int nScore = pItem->GetScore();
			if(nScore >= 500)
				setValuable.push_back(pItem);
			nValue1	+= nScore;
		}
		nValue1	+= m_nEmoney1;
		nValue1	+= m_nMoney1 / __max(1, CItem::GetEmoneyRate());
		for(ITEM_SET::iterator it2 = m_setItem2.begin(); it2 != m_setItem2.end(); it2++)
		{
			CItem* pItem = pUser2->GetItem(*it2);
			if(!pItem)
				continue;
			int nScore = pItem->GetScore();
			if(nScore >= 500)
				setValuable.push_back(pItem);
			nValue2	+= nScore;
		}
		nValue2	+= m_nEmoney2;
		nValue2	+= m_nMoney2 / __max(1, CItem::GetEmoneyRate());

		if(setValuable.size() > 0)
		{
			if((nValue1 >= 1380 || nValue2 >= 1380) && (nValue1 >= nValue2*5 || nValue2 >= nValue1*5))
			{
				bUnfair	= true;
				if(!m_bUnfairOK1 || !m_bUnfairOK2)
				{
					CMsgTrade	msg;
					if(msg.Create(15, 0))
					{
						pUser1->SendMsg(&msg);
						pUser2->SendMsg(&msg);
					}
					return false;
				}

				for(vector<CItem*>::iterator it3 = setValuable.begin(); it3 != setValuable.end(); it3++)
				{
					CItem* pItem = *it3;
					if(pItem && pItem->GetInt(ITEMDATA_PLUNDER) == 0)
					{
						pItem->SetInt(ITEMDATA_PLUNDER, CItemData::GetHourStamp(), false);
						setPlunder.push_back(pItem);
					}
				}
			}
		}
	}

	bool	bRet = true;
	bool	bLog = false;
	if(	(m_nMoney1 && (m_nMoney1 > (int)pUser1->GetMoney() || m_nMoney1 + pUser2->GetMoney() > _MAX_MONEYLIMIT))
			|| (m_nMoney2 && (m_nMoney2 > (int)pUser2->GetMoney() || m_nMoney2 + pUser1->GetMoney() > _MAX_MONEYLIMIT)) )
		bRet = false;
	if(m_nMoney1 >= 10000 || m_nMoney2 >= 10000)
		bLog = true;

	if(	(m_nEmoney1 && (m_nEmoney1 > (int)pUser1->GetEmoney() || pUser2->GetEmoney() + m_nEmoney1 > _MAX_MONEYLIMIT))
			|| (m_nEmoney2 && (m_nEmoney2 > (int)pUser2->GetEmoney() || pUser1->GetEmoney() + m_nEmoney2 > _MAX_MONEYLIMIT)) )
		bRet = false;
	if(m_nEmoney1 || m_nEmoney2)
		bLog = true;

	int	nAmountCount	= 0;
	int	nAmountWeight	= 0;
	int	i;
	for(i = 0; i < (int)m_setItem1.size(); i++)
	{
		CItem* pItem = pUser1->GetItem(m_setItem1[i]);
		if(!pItem)
			return false;
		nAmountCount++;
		nAmountWeight	+= pItem->GetWeight();
		if(pItem->IsNonsuchItem())
			bLog = true;
	}
	if(!pUser2->IsBackPackSpare(nAmountCount, nAmountWeight))
		bRet = false;

		nAmountCount	= 0;
		nAmountWeight	= 0;
	for( i = 0; i < (int)m_setItem2.size(); i++)
	{
		CItem* pItem = pUser2->GetItem(m_setItem2[i]);
		if(!pItem)
			return false;
		nAmountCount++;
		nAmountWeight	+= pItem->GetWeight();
		if(pItem->IsNonsuchItem())
			bLog = true;
	}
	if(!pUser1->IsBackPackSpare(nAmountCount, nAmountWeight))
		bRet = false;

	if(!bRet)
		return true;

	DEBUG_TRY
	if(m_nMoney1)
	{
		IF_OK(pUser1->SpendMoney(m_nMoney1, SYNCHRO_FALSE))
		{
			MYASSERT(pUser2->GainMoney(m_nMoney1, SYNCHRO_FALSE));
		}
		else
			return false;
	}
	if(m_nEmoney1 > 0)
	{
		IF_OK(pUser1->AddAttrib(_USERATTRIB_EMONEY, -1 * m_nEmoney1, SYNCHRO_TRUE))
		{
			IF_OK(pUser2->AddAttrib(_USERATTRIB_EMONEY, m_nEmoney1, SYNCHRO_TRUE))
				pUser1->SaveEmoneyLog(2, pUser1->GetID(), pUser2->GetID(), m_nEmoney1);
			else
			{
				pUser1->SaveEmoneyLog(5, pUser1->GetID(), pUser2->GetID(), m_nEmoney1);
				return false;
			}
		}
		else
			return false;
	}
	if(m_nMoney2)
	{
		IF_OK(pUser2->SpendMoney(m_nMoney2, SYNCHRO_FALSE))
		{
			MYASSERT(pUser1->GainMoney(m_nMoney2, SYNCHRO_FALSE));
		}
		else
			return false;
	}
	if(m_nEmoney2 > 0)
	{
		IF_OK(pUser2->AddAttrib(_USERATTRIB_EMONEY, -1 * m_nEmoney2, SYNCHRO_TRUE))
		{
			IF_OK(pUser1->AddAttrib(_USERATTRIB_EMONEY, m_nEmoney2, SYNCHRO_TRUE))
				pUser2->SaveEmoneyLog(2, pUser2->GetID(), pUser1->GetID(), m_nEmoney2);
			else
			{
				pUser2->SaveEmoneyLog(5, pUser2->GetID(), pUser1->GetID(), m_nEmoney2);
				return false;
			}
		}
		else
			return false;
	}

	if(bLog)
	{
		::MyLogSave("gmlog/trade_item", "user1_id[%u] user1[%s] lev1[%u] money1[%u] emoney1[%u]",
				pUser1->GetID(), pUser1->GetName(), pUser1->GetLev(), m_nMoney1, m_nEmoney1);
		::MyLogSave("gmlog/trade_item", "user2_id[%u] user2[%s] lev2[%u] money2[%u] emoney2[%u] ",
				pUser2->GetID(), pUser2->GetName(), pUser2->GetLev(), m_nMoney2, m_nEmoney2);
		if(bUnfair)
		{
			::MyLogSave("gmlog/unfair_trade_item", "user1_id[%u] user1[%s] lev1[%u] money1[%u] emoney1[%u] value1[%u]",
					pUser1->GetID(), pUser1->GetName(), pUser1->GetLev(), m_nMoney1, m_nEmoney1, nValue1);
			::MyLogSave("gmlog/unfair_trade_item", "user2_id[%u] user2[%s] lev2[%u] money2[%u] emoney2[%u] value2[%u]",
					pUser2->GetID(), pUser2->GetName(), pUser2->GetLev(), m_nMoney2, m_nEmoney2, nValue2);
		}
	}

	for(i = 0; i < (int)m_setItem1.size(); i++)
	{
		CItem* pItem = pUser1->GetItem(m_setItem1[i]);
		MYASSERT(pItem);
		if(pItem)
		{
			MYASSERT(pUser1->PopItem(pItem->GetID(), SYNCHRO_FALSE, UPDATE_FALSE));
			MYASSERT(pUser2->AddItem(pItem, SYNCHRO_FALSE, UPDATE_TRUE));

			if(pItem->TestSpecialFlag(2) && pBuddy && !pBuddy->GetDate())
				pUser1->UpdateGhostContract(pItem, pUser2->GetID());
			if(bLog)
			{
				::MyLogSave("gmlog/trade_item", "user id[%u] name[%s] | item[%s][%u] type[%u] gem1[%d] gem2[%d] magic3[%d] dur[%d] max_dur[%d] antimonster[%u], dec_dmg[%u] add_life[%u]",
						pUser1->GetID(), pUser1->GetName(), pItem->GetStr(ITEMDATA_NAME), pItem->GetID(), pItem->GetInt(ITEMDATA_TYPE),
						pItem->GetInt(ITEMDATA_GEM1), pItem->GetInt(ITEMDATA_GEM2), pItem->GetInt(ITEMDATA_MAGIC3),
						pItem->GetInt(ITEMDATA_AMOUNT), pItem->GetInt(ITEMDATA_AMOUNTLIMIT),
						pItem->GetInt(ITEMDATA_ANTI_MONSTER), pItem->GetInt(ITEMDATA_REDUCE_DMG), pItem->GetInt(ITEMDATA_ADD_LIFE));
				if(bUnfair)
				{
					::MyLogSave("gmlog/unfair_trade_item", "user id[%u] name[%s] | item[%s][%u] type[%u] gem1[%d] gem2[%d] magic3[%d] dur[%d] max_dur[%d] antimonster[%u], dec_dmg[%u] add_life[%u]",
							pUser1->GetID(), pUser1->GetName(), pItem->GetStr(ITEMDATA_NAME), pItem->GetID(), pItem->GetInt(ITEMDATA_TYPE),
							pItem->GetInt(ITEMDATA_GEM1), pItem->GetInt(ITEMDATA_GEM2), pItem->GetInt(ITEMDATA_MAGIC3),
							pItem->GetInt(ITEMDATA_AMOUNT), pItem->GetInt(ITEMDATA_AMOUNTLIMIT),
							pItem->GetInt(ITEMDATA_ANTI_MONSTER), pItem->GetInt(ITEMDATA_REDUCE_DMG), pItem->GetInt(ITEMDATA_ADD_LIFE));
				}
			}
		}
	}
	for( i = 0; i < (int)m_setItem2.size(); i++)
	{
		CItem* pItem = pUser2->GetItem(m_setItem2[i]);
		MYASSERT(pItem);
		if(pItem)
		{
			MYASSERT(pUser2->PopItem(pItem->GetID(), SYNCHRO_FALSE, UPDATE_FALSE));
			MYASSERT(pUser1->AddItem(pItem, SYNCHRO_FALSE, UPDATE_TRUE));

			if(pItem->TestSpecialFlag(2) && pBuddy && !pBuddy->GetDate())
				pUser2->UpdateGhostContract(pItem, pUser1->GetID());
			if(bLog)
			{
				::MyLogSave("gmlog/trade_item", "user id[%u] name[%s] | item[%s][%u] type[%u] gem1[%d] gem2[%d] magic3[%d] dur[%d] max_dur[%d] antimonster[%u], dec_dmg[%u] add_life[%u]",
						pUser2->GetID(), pUser2->GetName(), pItem->GetStr(ITEMDATA_NAME), pItem->GetID(), pItem->GetInt(ITEMDATA_TYPE),
						pItem->GetInt(ITEMDATA_GEM1), pItem->GetInt(ITEMDATA_GEM2), pItem->GetInt(ITEMDATA_MAGIC3),
						pItem->GetInt(ITEMDATA_AMOUNT), pItem->GetInt(ITEMDATA_AMOUNTLIMIT),
						pItem->GetInt(ITEMDATA_ANTI_MONSTER), pItem->GetInt(ITEMDATA_REDUCE_DMG), pItem->GetInt(ITEMDATA_ADD_LIFE));
				if(bUnfair)
				{
					::MyLogSave("gmlog/unfair_trade_item", "user id[%u] name[%s] | item[%s][%u] type[%u] gem1[%d] gem2[%d] magic3[%d] dur[%d] max_dur[%d] antimonster[%u], dec_dmg[%u] add_life[%u]",
							pUser2->GetID(), pUser2->GetName(), pItem->GetStr(ITEMDATA_NAME), pItem->GetID(), pItem->GetInt(ITEMDATA_TYPE),
							pItem->GetInt(ITEMDATA_GEM1), pItem->GetInt(ITEMDATA_GEM2), pItem->GetInt(ITEMDATA_MAGIC3),
							pItem->GetInt(ITEMDATA_AMOUNT), pItem->GetInt(ITEMDATA_AMOUNTLIMIT),
							pItem->GetInt(ITEMDATA_ANTI_MONSTER), pItem->GetInt(ITEMDATA_REDUCE_DMG), pItem->GetInt(ITEMDATA_ADD_LIFE));
				}
			}
		}
	}
	pUser1->SendSysMsg(STR_TRADE_SUCCEED);
	pUser2->SendSysMsg(STR_TRADE_SUCCEED);
	DEBUG_CATCH("★Fair trade error★")

	CMsgTrade	msg;
	CHECKF(msg.Create(_TRADE_SUCCESS, 0));
	pUser1->SendMsg(&msg);
	pUser2->SendMsg(&msg);
	m_bTradeSuccess	= true;

	for(ITEM_SET::iterator it4 = m_setItem1.begin(); it4 != m_setItem1.end(); it4++)
	{
		CItem* pItem = pUser2->GetItem(*it4);
		if(pItem && find(setPlunder.begin(), setPlunder.end(), pItem) != setPlunder.end())
		{
			CMsgItemInfo	msg;
			if(msg.Create(pItem, ITEMINFO_UPDATE))
				pUser2->SendMsg(&msg);
		}
	}
	for(ITEM_SET::iterator it5 = m_setItem2.begin(); it5 != m_setItem2.end(); it5++)
	{
		CItem* pItem = pUser1->GetItem(*it5);
		if(pItem && find(setPlunder.begin(), setPlunder.end(), pItem) != setPlunder.end())
		{
			CMsgItemInfo	msg;
			if(msg.Create(pItem, ITEMINFO_UPDATE))
				pUser1->SendMsg(&msg);
		}
	}

	pUser1->SaveInfo();
	pUser2->SaveInfo();

	return true;
}

bool CSquareDeal::XSquareDeal1::Create(IUserSquareDeal* pUser, IUserSquareDeal* pTarget)
{
	This()->m_pUser1		= pUser;
	This()->m_pUser2		= pTarget;

	if(!pUser->Create(&This()->m_iface1))
		return false;
	if(!pTarget->Create(&This()->m_iface2))
		return false;

	return true;
}

bool CSquareDeal::XSquareDeal1::AddItem(CItem* pItem)
{
	CHECKF(pItem && pItem->GetID() && This());

	CUser*	pUser1	= NULL;
	CUser*	pUser2	= NULL;
	bool	bGM		= false;
	if(This()->m_pUser1 && (pUser1 = This()->m_pUser1->QueryUser()) != NULL)
		bGM	= pUser1->IsGM();
	if(This()->m_pUser2 && (pUser2 = This()->m_pUser2->QueryUser()) != NULL && !bGM)
		bGM	= pUser2->IsGM();
	if(pItem->IsPlunder() && !bGM)
		return false;
	CTradeBuddy* pBuddy = NULL;
	if(pUser1 && pUser2)
		pBuddy = pUser1->GetTradeBuddy(pUser2->GetID());
	if(pItem->TestSpecialFlag(2) || pItem->TestSpecialFlag(1) || (pUser1 && pUser1->IsItemProtected(pItem) && !pUser1->IsBusinessman()))
	{
		if((!pBuddy || pBuddy->GetDate()) && !bGM)
			return false;
	}

	if(This()->m_bClickOK1 == true)
		return false;

	if(This()->m_setItem1.size() >= MAX_SQUAREDEALITEMS)
		return false;

	if(find(This()->m_setItem1.begin(), This()->m_setItem1.end(), pItem->GetID()) != This()->m_setItem1.end())
		return false;

	if(!This()->m_pUser2->AddItem(pItem, This()->m_setItem1.size()+1, This()->m_nAllWeight1 + pItem->GetWeight()))
		return false;

	This()->m_nAllWeight1	+= pItem->GetWeight();
	This()->m_setItem1.push_back(pItem->GetID());
	return true;
}

int CSquareDeal::XSquareDeal1::AddMoney(int nMoney)
{
	if(This()->m_bClickOK1 || This()->m_nMoney1)
		return This()->m_nMoney1;

	IUserSquareDeal* pUser = This()->m_pUser1;
	if(nMoney < 0)
	{
		::GmLogSave("User[%s][%u] tried to trade with money < 0 at %s, %d", pUser->GetName(), pUser->GetAccountID(), __FILE__, __LINE__);
		return This()->m_nMoney1;
	}

	if(nMoney > (int)pUser->GetMoney())
	{
		::GmLogSave("User[%s][%u] tried to trade with money he hasn't at %s, %d", pUser->GetName(), pUser->GetAccountID(), __FILE__, __LINE__);
		return This()->m_nMoney1;
	}

	if(This()->m_nMoney1 >= nMoney || nMoney > _MAX_MONEYLIMIT)
		return This()->m_nMoney1;

	if(!This()->m_pUser2->AddMoney(nMoney))
		return This()->m_nMoney1;
	This()->m_nMoney1	= nMoney;
	return This()->m_nMoney1;
}

int CSquareDeal::XSquareDeal1::AddEmoney(int nEmoney)
{
	if(This()->m_bClickOK1 || This()->m_nEmoney1)
		return This()->m_nEmoney1;

	IUserSquareDeal* pUser = This()->m_pUser1;
	if(nEmoney < 0)
	{
		::GmLogSave("User[%s][%u] tried to trade with Emoney < 0 at %s, %d", pUser->GetName(), pUser->GetAccountID(), __FILE__, __LINE__);
		return This()->m_nEmoney1;
	}

	if(nEmoney > (int)pUser->GetEmoney())
	{
		::GmLogSave("User[%s][%u] tried to trade with Emoney he hasn't at %s, %d", pUser->GetName(), pUser->GetAccountID(), __FILE__, __LINE__);
		return This()->m_nEmoney1;
	}

	if(This()->m_nEmoney1 >= nEmoney || nEmoney > MAX_SQUAREDEALMONEY)
		return This()->m_nEmoney1;

	if(!This()->m_pUser2->AddEmoney(nEmoney))
		return This()->m_nEmoney1;
	This()->m_nEmoney1	= nEmoney;
	return This()->m_nEmoney1;
}

bool CSquareDeal::XSquareDeal1::ClickOK()
{
	if(This()->m_bClickOK1)
		return false;

	This()->m_bClickOK1		= true;
	This()->m_pUser2->ClickOK();

	if(This()->m_bClickOK2 == true)
	{
		return This()->TradeOK();
	}

	return false;
}

bool CSquareDeal::XSquareDeal1::ClickUnfairOK()
{
	if(This()->m_bUnfairOK1)
		return false;

	This()->m_bUnfairOK1	= true;
	if(This()->m_bUnfairOK2)
		return This()->TradeOK();

	return false;
}

CUser* CSquareDeal::XSquareDeal1::GetTarget()
{
	return This()->m_pUser2->QueryUser();
}

bool CSquareDeal::XSquareDeal2::Create(IUserSquareDeal* pUser, IUserSquareDeal* pTarget)
{
	This()->m_pUser2		= pUser;
	This()->m_pUser1		= pTarget;

	if(!pUser->Create(&This()->m_iface2))
		return false;
	if(!pTarget->Create(&This()->m_iface1))
		return false;

	return true;
}

bool CSquareDeal::XSquareDeal2::AddItem(CItem* pItem)
{
	CHECKF(pItem && This());

	CUser*	pUser1	= NULL;
	CUser*	pUser2	= NULL;
	bool	bGM		= false;
	if(This()->m_pUser1 && (pUser1 = This()->m_pUser1->QueryUser()) != NULL)
		bGM	= pUser1->IsGM();
	if(This()->m_pUser2 && (pUser2 = This()->m_pUser2->QueryUser()) != NULL && !bGM)
		bGM	= pUser2->IsGM();
	if(pItem->IsPlunder() && !bGM)
		return false;
	CTradeBuddy* pBuddy = NULL;
	if(pUser1 && pUser2)
		pBuddy = pUser2->GetTradeBuddy(pUser1->GetID());
	if(pItem->TestSpecialFlag(2) || pItem->TestSpecialFlag(1) || (pUser2 && pUser2->IsItemProtected(pItem) && !pUser2->IsBusinessman()))
	{
		if((!pBuddy || pBuddy->GetDate()) && !bGM)
			return false;
	}

	if(This()->m_bClickOK2 == true)
		return false;

	if(This()->m_setItem2.size() >= MAX_SQUAREDEALITEMS)
		return false;

	if(find(This()->m_setItem2.begin(), This()->m_setItem2.end(), pItem->GetID()) != This()->m_setItem2.end())
		return false;

	if(!This()->m_pUser1->AddItem(pItem, This()->m_setItem2.size()+1, This()->m_nAllWeight2 + pItem->GetWeight()))
		return false;

	This()->m_nAllWeight2	+= pItem->GetWeight();
	This()->m_setItem2.push_back(pItem->GetID());
	return true;
}

int CSquareDeal::XSquareDeal2::AddMoney(int nMoney)
{
	if(This()->m_bClickOK2 || This()->m_nMoney2)
		return This()->m_nMoney2;

	IUserSquareDeal* pUser = This()->m_pUser2;
	if(nMoney < 0)
	{
		::GmLogSave("User[%s][%u] tried to trade with money < 0 at %s, %d", pUser->GetName(), pUser->GetAccountID(), __FILE__, __LINE__);
		return This()->m_nMoney2;
	}

	if(nMoney > (int)pUser->GetMoney())
	{
		::GmLogSave("User[%s][%u] tried to trade with money he hasn't at %s, %d", pUser->GetName(), pUser->GetAccountID(), __FILE__, __LINE__);
		return This()->m_nMoney2;
	}

	if(This()->m_nMoney2 >= nMoney || nMoney > _MAX_MONEYLIMIT)
		return This()->m_nMoney2;

	if(!This()->m_pUser1->AddMoney(nMoney))
		return This()->m_nMoney2;
	This()->m_nMoney2	= nMoney;
	return This()->m_nMoney2;
}

int CSquareDeal::XSquareDeal2::AddEmoney(int nEmoney)
{
	if(This()->m_bClickOK2 || This()->m_nEmoney2)
		return This()->m_nEmoney2;

	IUserSquareDeal* pUser = This()->m_pUser2;
	if(nEmoney < 0)
	{
		::GmLogSave("User[%s][%u] tried to trade with Emoney < 0 at %s, %d", pUser->GetName(), pUser->GetAccountID(), __FILE__, __LINE__);
		return This()->m_nEmoney2;
	}

	if(nEmoney > (int)pUser->GetEmoney())
	{
		::GmLogSave("User[%s][%u] tried to trade with Emoney he hasn't at %s, %d", pUser->GetName(), pUser->GetAccountID(), __FILE__, __LINE__);
		return This()->m_nEmoney2;
	}

	if(This()->m_nEmoney2 >= nEmoney || nEmoney > MAX_SQUAREDEALMONEY)
		return This()->m_nEmoney2;

	if(!This()->m_pUser1->AddEmoney(nEmoney))
		return This()->m_nEmoney2;
	This()->m_nEmoney2	= nEmoney;
	return This()->m_nEmoney2;
}

bool CSquareDeal::XSquareDeal2::ClickOK()
{
	if(This()->m_bClickOK2)
		return false;

	This()->m_bClickOK2		= true;
	This()->m_pUser1->ClickOK();

	if(This()->m_bClickOK1 == true)
	{
		return This()->TradeOK();
	}

	return false;
}

bool CSquareDeal::XSquareDeal2::ClickUnfairOK()
{
	if(This()->m_bUnfairOK2)
		return false;

	This()->m_bUnfairOK2	= true;
	if(This()->m_bUnfairOK1)
		return This()->TradeOK();

	return false;
}

CUser* CSquareDeal::XSquareDeal2::GetTarget()
{
	return This()->m_pUser1->QueryUser();
}
