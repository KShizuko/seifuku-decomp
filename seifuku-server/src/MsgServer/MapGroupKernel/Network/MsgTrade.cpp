#include <windows.h>
#include "AllMsg.h"
#include "User.h"
#include "SquareDeal.h"
#include "MapGroup.h"

CMsgTrade::CMsgTrade()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgTrade::~CMsgTrade()
{

}

BOOL CMsgTrade::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_TRADE != this->GetType())
		return false;

	return true;
}

BOOL CMsgTrade::Create(unsigned short usAction, OBJID id)
{
	this->Init();
	
	m_unMsgType		= _MSG_TRADE;
	m_unMsgSize		= sizeof(MSG_Info);
	
	m_pInfo->usAction	= usAction;
	m_pInfo->id			= id;
	return true;
}

void CMsgTrade::Process(void* pInfo)
{
#ifdef _MYDEBUG
	::LogSave("Process CMsgTrade, id:%u, action:%u",
		m_pInfo->id,
		m_pInfo->usAction);
#endif

	CUserPtr pUser	= UserManager()->GetUser(this);
	if (!pUser)
		return;

	DEBUG_TRY
	switch(m_pInfo->usAction)
	{
	case _TRADE_APPLY:
		{
			if (m_pInfo->id == ID_NONE)
				return;

			OBJID	idTarget = m_pInfo->id;
			CUser* pTarget = NULL;
			if(!pUser->GetMap()->QueryObj(pUser->GetPosX(), pUser->GetPosY(), OBJ_USER, idTarget, IPP_OF(pTarget)))
			{
				pUser->SendSysMsg(STR_NO_TRADE_TARGET);
				return;
			}

			ISquareDeal* pSquareDeal = pUser->QuerySquareDeal();
			if(pSquareDeal)
			{
				LOGWARNING("Player trading twice!");
				pSquareDeal->Release();
				return ;
			}

			if(pTarget->IsIncomeLimited() && (pTarget->GetIncomePercent() == 0 || pTarget->GetIncomePercent() == 50))
			{
				pUser->SendSysMsg(STR_TRADE_WALLOW);
				return;
			}

			if(pTarget->QuerySquareDeal())
			{
				pUser->SendSysMsg(STR_TARGET_TRADING);
				return ;
			}

			if (!pTarget->FetchApply(CUser::APPLY_TRADE, pUser->GetID()))
			{
				pUser->SetApply(CUser::APPLY_TRADE, idTarget);
				IF_NOT(this->Create(_TRADE_APPLY, pUser->GetID()))
					return;
				pTarget->SendMsg(this);
				pUser->SendSysMsg(STR_TRADING_REQEST_SENT);
				return;
			}

			if(!pUser->CreateSquareDeal(pTarget))
			{
				pUser->SendSysMsg(STR_TARGET_TRADING);
				return;
			}
		}
		break;

	case _TRADE_QUIT:
		{
			ISquareDeal* pSquareDeal =	pUser->QuerySquareDeal();
			if(pSquareDeal)
			{
				CUserPtr pTarget	= pSquareDeal->GetTarget();
				if(pTarget)
				{
				}

				{
				}

				pSquareDeal->Release();
			}
		}
		break;

	case _TRADE_OPEN:
		{
			ASSERT(!"_TRADE_OPEN");
		}
		break;

	case _TRADE_SUCCESS:
		{
			ASSERT(!"_TRADE_SUCCESS");
		}
		break;

	case _TRADE_FALSE:
		{
			ASSERT(!"_TRADE_FALSE");
		}
		break;

	case _TRADE_ADDITEM:
		{
			ISquareDeal* pSquareDeal =	pUser->QuerySquareDeal();
			if(!pSquareDeal)
				return;

			CItemPtr pItem	= pUser->GetItem(m_pInfo->id);
			if (!pItem || !pItem->IsExchangeEnable())
			{
				pUser->SendSysMsg(STR_NOT_FOR_TRADE);

				IF_OK(this->Create(_TRADE_ADDITEMFAIL, m_pInfo->id))
					pUser->SendMsg(this);
				return;
			}
			if(!pSquareDeal->AddItem(pItem))
			{
				IF_OK(this->Create(_TRADE_ADDITEMFAIL, m_pInfo->id))
					pUser->SendMsg(this);
				return;
			}
		}
		break;

	case _TRADE_ADDMONEY:
		{
			ISquareDeal* pSquareDeal =	pUser->QuerySquareDeal();
			if(!pSquareDeal)
				return;

			int	nAllMoney = pSquareDeal->AddMoney(m_pInfo->nData);
			CHECK(this->Create(_TRADE_SELF_MONEYALL, nAllMoney));
			pUser->SendMsg(this);
		}
		break;

	case _TRADE_ADDEMONEY:
		{
			ISquareDeal* pSquareDeal =	pUser->QuerySquareDeal();
			if(!pSquareDeal)
				return;

			int	nAllEmoney = pSquareDeal->AddEmoney(m_pInfo->nData);
			CHECK(this->Create(_TRADE_SELF_EMONEYALL, nAllEmoney));
			pUser->SendMsg(this);
		}
		break;

	case _TRADE_OK:
		{
			ISquareDeal* pSquareDeal =	pUser->QuerySquareDeal();
			if(!pSquareDeal)
				return;

			if(pSquareDeal->ClickOK())
				pSquareDeal->Release();
		}
		break;

	case _TRADE_UNFAIR_OK:
		{
			ISquareDeal* pSquareDeal =	pUser->QuerySquareDeal();
			if(!pSquareDeal)
				return;

			if(pSquareDeal->ClickUnfairOK())
				pSquareDeal->Release();
		}
		break;

	default:
		break;
	}
	DEBUG_CATCH2("switch(MSGTRADE) [%d]", m_pInfo->usAction)
}
