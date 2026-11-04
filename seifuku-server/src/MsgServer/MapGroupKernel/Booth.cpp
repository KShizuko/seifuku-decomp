#include "windows.h"
#include "Booth.h"
#include "User.h"
#include "MapGroup.h"
#include "AllMsg.h"

const int	BOOTH_SIZE		= 20;
const int	BOOTH_LOOK		= 407;

MYHEAP_IMPLEMENTATION(CBooth,s_heap)
CBooth::CBooth()
{
	m_id			= ID_NONE;
	m_pMap			= NULL;
	m_bEnterMap		= false;
	m_szCryOut[0]	= 0;
}

CBooth::~CBooth()
{
	if(m_pMap)
	{
		LeaveMap();
	}

	if(m_id != ID_NONE)
		MapManager()->RecycleDynaNpcID(m_id);
}

CBooth*	CBooth::CreateNew(PROCESS_ID idProcess, CUser* pUser, int nPosX, int nPosY, int nDir)
{
	CBooth* pBooth = new CBooth;
	if(pBooth)
	{
		if(pBooth->Create(idProcess, pUser, nPosX, nPosY, nDir))
			return pBooth;
		else
			delete pBooth;
	}

	return NULL;
}

bool CBooth::Create(PROCESS_ID idProcess, CUser* pUser, int nPosX, int nPosY, int nDir)
{
	m_idProcess	= idProcess;
	m_pUser		= pUser;
	CHECKF(pUser);

	m_pMap		= m_pUser->GetMap();
	CHECKF(m_pMap);
	m_id		= MapManager()->SpawnNewNpcID();

	EnterMap(nPosX, nPosY, nDir);

	for(SYSBUY_SET::iterator it = s_setSysBuy.begin(); it != s_setSysBuy.end(); it++)
		m_setSysBuyTime.push_back(0);

	return true;
}

void CBooth::SendShow(IRole* pRole)
{
	OBJID idType0 = ID_NONE;
	OBJID idType1 = ID_NONE;
	if(m_setGoods.size() > 0)
		idType0 = m_setGoods[0].nType;
	if(m_setGoods.size() > 1)
		idType1 = m_setGoods[1].nType;

	CMsgNpcInfoEx	msg;
	IF_OK(msg.Create(GetID(), idType0, idType1, _BOOTH_NPC, NPCSORT_NONE, (BOOTH_LOOK/10)*10 + m_nDir, GetPosX(), GetPosY(), m_pUser->GetName()))
		pRole->SendMsg(&msg);

	if(strlen(m_szCryOut))
	{
		CMsgTalk msg;
		BOOL ret = msg.Create(m_pUser->GetName(), "ALL", m_szCryOut, NULL, 0xffffff, _TXTATR_CRYOUT);
		IF_OK_ (ret)
			pRole->SendMsg(&msg);
	}
}

bool CBooth::AddItem(OBJID idItem, int nMoney, int nEmoney)
{
	CHECKF(idItem != ID_NONE && (nMoney > 0 || nEmoney > 0));
	if(m_setGoods.size() >= BOOTH_SIZE)
		return false;

	CItem* pItem = m_pUser->GetItem(idItem);
	if(!pItem)
		return false;

	if (!pItem->IsExchangeEnable() || pItem->IsPlunder() || pItem->TestSpecialFlag(2) || pItem->TestSpecialFlag(1))
		return false;

	if (m_pUser->IsItemProtected(pItem) && !m_pUser->IsBusinessman())
		return false;
	{

		int nMinEmoney = pItem->GetScore() / 5;
		if (nEmoney < nMinEmoney && nMoney / __max(1, CItem::GetEmoneyRate()) < nMinEmoney)
		{
			m_pUser->SendSysMsg(STR_BOOTH_LOW_PRICE, nMinEmoney);
			return false;
		}
	}

	GOODS_ST	info;
	info.idItem		= idItem;
	info.nType		= pItem->GetInt(ITEMDATA_TYPE);
	info.nMoney		= nMoney;
	info.nEmoney	= nEmoney;

	for(int i = 0; i < (int)m_setGoods.size(); i++)
	{
		if(m_setGoods[i].idItem == idItem)
			return false;
	}

	m_setGoods.push_back(info);

	return true;
}

bool CBooth::GetCost(OBJID idItem, int* pnMoney, int* pnEmoney)
{
	for(int i = 0; i < (int)m_setGoods.size(); i++)
	{
		if(m_setGoods[i].idItem == idItem)
		{
			*pnMoney	= m_setGoods[i].nMoney;
			*pnEmoney	= m_setGoods[i].nEmoney;
			return true;
		}
	}

	*pnEmoney	= 0;
	*pnMoney	= 0;
	return false;
}

bool CBooth::DelItem(OBJID idItem)
{
	int i;
	for(i = 0; i < (int)m_setGoods.size(); i++)
	{
		if(m_setGoods[i].idItem == idItem)
			break;
	}
	if(i >= (int)m_setGoods.size())
		return false;

	m_setGoods.erase(m_setGoods.begin() + i);

	CMsgItem	msg;
	IF_OK(msg.Create(idItem, ITEMACT_BOOTH_DEL, GetID()))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	return true;
}

bool CBooth::BuyItem(CUser* pTarget, OBJID idItem)
{
	CHECKF(pTarget && idItem != ID_NONE);

	if(!m_pUser)
		return false;

	if(pTarget->GetID() == GetID())
		return false;

	int nMoney = 0;
	int nEmoney = 0;
	if(!GetCost(idItem, &nMoney, &nEmoney))
		return false;

	if(nEmoney > 0 && nEmoney > (int)pTarget->GetEmoney())
		return false;
	if(nMoney > 0 && nMoney > (int)pTarget->GetMoney())
		return false;

	CItem* pItem = m_pUser->GetItem(idItem);
	if(!pItem)
	{
		pTarget->SendSysMsg(STR_TRADE_FAIL);
		return false;
	}

	if(pTarget->IsBackPackFull())
	{
		pTarget->SendSysMsg(STR_BAG_FULL);
		return false;
	}

	if(m_pUser->GetMoney() + nMoney >= _MAX_MONEYLIMIT || m_pUser->GetEmoney() + nEmoney >= _MAX_MONEYLIMIT)
	{
		pTarget->SendSysMsg(STR_TRADE_FAIL);
		return false;
	}

	DEBUG_TRY
	MYASSERT(DelItem(idItem));
	pItem = m_pUser->PopItem(idItem, SYNCHRO_TRUE, UPDATE_TRUE);
	if(!pItem)
	{
		pTarget->SendSysMsg(STR_TRADE_FAIL);
		return false;
	}
	MYASSERT(pTarget->AddItem(pItem, SYNCHRO_TRUE, UPDATE_TRUE));
	if(nMoney > 0)
	{
		MYASSERT(pTarget->SpendMoney(nMoney, SYNCHRO_TRUE));
		MYASSERT(m_pUser->GainMoney(nMoney, SYNCHRO_TRUE));
	}
	if(nEmoney > 0)
	{
		MYASSERT(pTarget->AddAttrib(_USERATTRIB_EMONEY, -nEmoney, SYNCHRO_TRUE));
		MYASSERT(m_pUser->AddAttrib(_USERATTRIB_EMONEY, nEmoney, SYNCHRO_TRUE));
		m_pUser->SaveEmoneyLog(3, pTarget->GetID(), m_pUser->GetID(), nEmoney);
	}

	m_pUser->SaveInfo();
	pTarget->SaveInfo();

	if (pItem->IsNonsuchItem() || nEmoney > 0)
	{
		if (pItem->IsGameCard())
		{
			::MyLogSave("gmlog/booth_item", "%s(%u) sell gamecard(id:%u, type:%u) to %s(%u) cost money(%d), emoney(%d)",
					m_pUser->GetName(),
					m_pUser->GetID(),
					pItem->GetID(),
					pItem->GetInt(ITEMDATA_TYPE),
					pTarget->GetName(),
					pTarget->GetID(),
					nMoney,
					nEmoney);
		}
		else
		{
			::MyLogSave("gmlog/booth_item", "%s(%u) sell item:[id=%u, type=%u], dur=%d, max_dur=%d, gemhole(%d, %d), add_lev(%d), anti_monster(%d), dec_dmg(%d) to %s(%u) cost money(%d), emoney(%d)",
					m_pUser->GetName(),
					m_pUser->GetID(),
					pItem->GetID(),
					pItem->GetInt(ITEMDATA_TYPE),
					pItem->GetInt(ITEMDATA_AMOUNT),
					pItem->GetInt(ITEMDATA_AMOUNTLIMIT),
					pItem->GetInt(ITEMDATA_GEM1),
					pItem->GetInt(ITEMDATA_GEM2),
					pItem->GetInt(ITEMDATA_MAGIC3),
					pItem->GetInt(ITEMDATA_ANTI_MONSTER),
					pItem->GetInt(ITEMDATA_REDUCE_DMG),
					pTarget->GetName(),
					pTarget->GetID(),
					nMoney,
					nEmoney);
		}
	}

	if (nEmoney > 0)
		m_pUser->SendSysMsg(STR_BOOTH_BUY_EMONEY, pTarget->GetName(), nEmoney, pItem->GetStr(ITEMDATA_NAME));
	else
		m_pUser->SendSysMsg(STR_BOOTH_BUY_sds, pTarget->GetName(), nMoney, pItem->GetStr(ITEMDATA_NAME));
	DEBUG_CATCH("CBooth::BuyItem()")

	return true;
}

void CBooth::SendGoods(CUser* pTarget)
{
	CHECK(pTarget);

	for(int i = m_setGoods.size()-1; i >= 0; i--)
	{
		OBJID	idItem	= m_setGoods[i].idItem;
		int		nMoney	= m_setGoods[i].nMoney;
		int		nEmoney	= m_setGoods[i].nEmoney;
		CItem* pItem = m_pUser->GetItem(idItem);
		IF_OK(pItem)
		{
			CMsgItemInfoEx	msg;
			if(nEmoney > 0)
			{
				IF_OK(msg.Create(pItem, GetID(), nEmoney, ITEMINFOEX_BOOTH_EMONEY))
					pTarget->SendMsg(&msg);
			}
			else
			{
				IF_OK(msg.Create(pItem, GetID(), nMoney, ITEMINFOEX_BOOTH))
					pTarget->SendMsg(&msg);
			}
		}
		else
		{
			DelItem(idItem);
		}
	}
}

void CBooth::EnterMap(int nPosX, int nPosY, int nDir)
{
	if(m_bEnterMap)
		return ;

	void* pTemp;
	if(m_pUser->GetMap()->QueryObjInPos(nPosX, nPosY, OBJ_BOOTH, IPP_OF(pTemp)))
		return ;

	m_nPosX		= nPosX;
	m_nPosY		= nPosY;
	m_nDir		= nDir;
	m_pMap->EnterRoom(QueryMapThing());

	CMsgNpcInfo	msg;
	IF_OK(msg.Create(GetID(), _BOOTH_NPC, NPCSORT_NONE, (BOOTH_LOOK/10)*10 + m_nDir, GetPosX(), GetPosY(), m_pUser->GetName()))
		m_pMap->BroadcastBlockMsg(QueryMapThing(), &msg);
	if(strlen(m_szCryOut))
	{
		CMsgTalk msg;
		IF_OK_ (msg.Create(m_pUser->GetName(), ALLUSERS_NAME, m_szCryOut, NULL, 0xffffff, _TXTATR_CRYOUT))
			m_pMap->BroadcastBlockMsg(QueryMapThing(), &msg);
	}

	m_bEnterMap	= true;
}

void CBooth::LeaveMap()
{
	if(!m_bEnterMap)
		return ;

	CMsgNpc	msg;
	IF_OK(msg.Create(EVENT_LEAVEMAP, GetID()))
		m_pMap->BroadcastBlockMsg(QueryMapThing(), &msg);

	m_pMap->LeaveRoom(QueryMapThing());

	m_bEnterMap	= false;
}

void CBooth::SendCryOut(CUser* pUser)
{
	if(strlen(m_szCryOut))
	{
		CMsgTalk msg;
		BOOL ret = msg.Create(m_pUser->GetName(), "ALL", m_szCryOut, NULL, 0xffffff, _TXTATR_CRYOUT);
		IF_OK_ (ret)
			pUser->SendMsg(&msg);
	}
}

void CBooth::OnTimer()
{
	IF_NOT(m_pUser)
		return;

	int nAmount = s_setSysBuy.size();
	for(int i = 0; i < nAmount; i++)
	{
		SYSBUY_ST* pBuy = &s_setSysBuy[i];
		time_t tNow = time(NULL);
		if(tNow < m_setSysBuyTime[i] + pBuy->nInterval)
			continue;
		m_setSysBuyTime[i] = tNow;

		for(int j = m_setGoods.size()-1; j >= 0; j--)
		{
			GOODS_ST	info = m_setGoods[j];
			if(info.nType != pBuy->idType)
				continue;

			int nRate;
			if((DWORD)info.nMoney < pBuy->dwMinPrice)
				nRate = 100;
			else if((DWORD)info.nMoney >= pBuy->dwMaxPrice)
				nRate = 0;
			else
			{
				DWORD dwRange = pBuy->dwMaxPrice - pBuy->dwMinPrice;
				if(dwRange < 1)
					dwRange = 1;
				nRate = ::MulDiv(pBuy->dwMaxPrice - info.nMoney, 100, dwRange);
			}
			if(::RandGet(100) >= nRate)
				continue;

			m_setGoods.erase(m_setGoods.begin() + j);

			CMsgItem	msg;
			IF_OK(msg.Create(info.idItem, ITEMACT_BOOTH_DEL, GetID()))
				m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

			CItem* pItem = m_pUser->PopItem(info.idItem, SYNCHRO_TRUE, UPDATE_TRUE);
			IF_OK(pItem)
			{
				m_pUser->GainMoney(info.nMoney, SYNCHRO_TRUE);
				m_pUser->SaveInfo();
				pItem->DeleteRecord();
				pItem->ReleaseByOwner();
				::MyLogSave("gmlog/sysbuy", "system buy item:%u, type:%u at price:%u", info.idItem, info.nType, info.nMoney);
			}
		}
	}
}

CBooth::SYSBUY_SET	CBooth::s_setSysBuy;

bool CBooth::InitSysBuyInfo(IDatabase* pDb)
{
	IF_NOT(pDb)
		return false;

	IRecordset* pRes = pDb->CreateNewRecordset("SELECT * FROM cq_sysbuyer", true);
	IF_NOT(pRes)
		return false;

	for(int i = 0; i < (int)pRes->RecordCount(); i++)
	{
		SYSBUY_ST	info;
		info.idType		= pRes->GetInt(1);
		info.nInterval	= pRes->GetInt(2);
		info.dwMinPrice	= pRes->GetInt(3);
		info.dwMaxPrice	= pRes->GetInt(4);
		s_setSysBuy.push_back(info);
		::MyLogSave("gmlog/sys_buy_db", "item type:%d, buy interval:%u, price: %u-%u", info.idType, info.nInterval, info.dwMinPrice, info.dwMaxPrice);

		pRes->MoveNext();
	}
	pRes->Release();

	return true;
}
