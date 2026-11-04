#include "LotteryMgr.h"
#include "MsgAction.h"
#include "MsgItemInfo.h"
#include "Cn_DDRes.h"
#include "common.h"
#include "BaseFunc.h"
#include <mmsystem.h>

static CLotteryMgr*	s_pLotteryMgr	= NULL;
CLotteryMgr* CLotteryMgr::GetInstance()
{
	if (!s_pLotteryMgr)
	{
		static CLotteryMgr	s_objLotteryMgr;
		s_pLotteryMgr = &s_objLotteryMgr;
	}
	return s_pLotteryMgr;
}

CLotteryMgr::~CLotteryMgr()
{
	for (LOTTERY_SET::iterator it = m_setLottery.begin(); it != m_setLottery.end(); it++)
	{
		CLottery* pLottery = it->second;
		IF_OK(pLottery)
		SAFE_RELEASE(pLottery);
	}
	m_setLottery.clear();
}

bool CLotteryMgr::Create(IDatabase* pDb)
{
	if (!s_pLotteryMgr)
		s_pLotteryMgr = this;

	IF_NOT(pDb)
		return false;

	char szSQL[1024];
	sprintf(szSQL, "SELECT * FROM %s", _TBL_LOTTERY);
	IRecordset* pRes = pDb->CreateNewRecordset(szSQL, true);
	IF_NOT(pRes)
		return false;

	for (int i = 0; i < (int)pRes->RecordCount(); i++)
	{
		ST_LOTTERY	info;
		info.id				= pRes->GetInt(0);
		info.nType			= pRes->GetInt(1);
		info.nRank			= pRes->GetInt(2);
		info.nChance		= pRes->GetInt(3);
		LPCTSTR pszName = pRes->GetStr(4);		if (pszName)	info.strPrizeName = pszName;
		info.idPrizeItem	= pRes->GetInt(5);
		info.nColor			= pRes->GetInt(6);
		info.nHoleNum		= pRes->GetInt(7);
		info.nAdditionLev	= pRes->GetInt(8);

		CLottery* pLottery = CLottery::CreateNew(&info);
		IF_NOT(pLottery)
			return false;
		m_setLottery[pLottery->GetID()] = pLottery;

		pRes->MoveNext();
	}
	pRes->Release();
	return true;
}

CLottery* CLotteryMgr::QueryLottery(OBJID idLottery)
{
	if (idLottery != ID_NONE)
	{
		LOTTERY_SET::iterator it = m_setLottery.find(idLottery);
		if (it != m_setLottery.end())
		{
			CLottery* pLottery = it->second;
			IF_NOT(pLottery)
				;
		}
	}
	return NULL;
}

int CLotteryMgr::SumChance(int nLotteryType, int nRank)
{
	if (m_setLottery.size() == 0)
		return 0;

	int nSum = 0;
	for (LOTTERY_SET::iterator it = m_setLottery.begin(); it != m_setLottery.end(); it++)
	{
		CLottery* pLottery = it->second;
		IF_NOT(pLottery)
			return 0;
		if (pLottery->GetType() == nLotteryType && pLottery->GetRank() == nRank)
			nSum += pLottery->GetChance();
	}
	return nSum;
}

CLottery* CLotteryMgr::GetLotteryByRoll(int nLotteryType, int nRank, int nRoll)
{
	if (m_setLottery.size() == 0)
		return NULL;

	int nSum = 0;
	for (LOTTERY_SET::iterator it = m_setLottery.begin(); it != m_setLottery.end(); it++)
	{
		CLottery* pLottery = it->second;
		IF_NOT(pLottery)
			return NULL;
		if (pLottery->GetType() == nLotteryType && pLottery->GetRank() == nRank)
		{
			nSum += pLottery->GetChance();
			if (nRoll < nSum)
				return pLottery;
		}
	}
	return NULL;
}

bool CLotteryMgr::Lottery(int nLotteryType, int nColor, ILotteryActor* pActor, bool bTest)
{
	IF_NOT(pActor)
		return false;

	int nRank		= 0;
	int nBigRank	= 0;
	this->GetLotteryRank(nLotteryType, pActor, &nRank, &nBigRank);
	CLottery* pLottery = this->GetLottery(nLotteryType, nRank, nColor);
	if (!pLottery)
		return false;

	if (bTest)
	{
		::MyLogSave("gmlog/lotterytest", "%u\t%u\t%s\t", pLottery->GetID(), nRank, pLottery->GetPrizeName());
		return true;
	}

	if (!this->AwardPrize(pLottery, pActor))
		return false;

	if (nRank < nBigRank)
	{
		char szMsg[1024];
		::wsprintf(szMsg, STR_LOTTERY_BIG, pActor->GetName(), pLottery->GetPrizeName());
		pActor->BroadcastMsg(szMsg);
	}

	{
		pActor->SendSysMsg(STR_LOTTERY_GENERAL, pLottery->GetPrizeName());
	}

	CMsgAction	msgAction;
	IF_OK(msgAction.Create(pActor->GetID(), 0, 0, nLotteryType, actionLottery, nRank))
		pActor->BroadcastRoomMsg((PLUGIN_MSG*)&msgAction);

	::MyLogSave("gmlog/lottery", "lottery user [id:%u] [name:%s] | award [rank:%d] [prize_name:%s]",
				pActor->GetID(), pActor->GetName(), nRank, pLottery->GetPrizeName());
	return true;
}

bool CLotteryMgr::GetLotteryRank(int nLotteryType, ILotteryActor* pActor, int* pRank, int* pBigRank)
{
	int nBigFails	= pActor->GetBigPrizeFails();
	int nSmallFails	= pActor->GetSmallPrizeFails();

	CLotteryConfigSet* pLotteryConfigSet = CConfigMgr::GetInstance()->GetConfigSet(nLotteryType);
	IF_NOT(pLotteryConfigSet)
		return false;

	DWORD setInfo[3];
	if (!pLotteryConfigSet->GetGuarantee(nLotteryType, setInfo))
		return false;
	ST_LOTTERYCONFIG* pBig		= pLotteryConfigSet->GetConfig(setInfo[0]);
	ST_LOTTERYCONFIG* pSmall	= pLotteryConfigSet->GetConfig(setInfo[1]);
	if (!pBig || !pSmall)
		return false;

	if (nBigFails < pBig->nData3)
	{
		if (nSmallFails < pSmall->nData3)
		{
			::srand(::timeGetTime() + nSmallFails + nBigFails + nLotteryType);
			int nRoll = ::RandGet(setInfo[2]);
			*pRank = pLotteryConfigSet->RollType(nLotteryType, nRoll) % 10;
			if (*pRank <= pBig->nData2)
			{
				pActor->SetBigPrizeFails(0, false);
				pActor->SetSmallPrizeFails(0, true);
			}
			else if (*pRank <= pSmall->nData2)
			{
				pActor->SetBigPrizeFails(nBigFails+1, false);
				pActor->SetSmallPrizeFails(0, true);
			}
			else
			{
				pActor->SetBigPrizeFails(nBigFails+1, false);
				pActor->SetSmallPrizeFails(nSmallFails+1, true);
			}
		}
		else
		{
			*pRank = pSmall->nData2;
			pActor->SetBigPrizeFails(nBigFails+1, false);
			pActor->SetSmallPrizeFails(0, true);
		}
	}
	else
	{
		*pRank = pBig->nData2;
		pActor->SetBigPrizeFails(0, false);
		pActor->SetSmallPrizeFails(0, true);
	}
	*pBigRank = pSmall->nData2;
	return true;
}

CLottery* CLotteryMgr::GetLottery(int nLotteryType, int nRank, int nColor)
{
	int nSum = this->SumChance(nLotteryType, nRank);
	if (!nSum)
		return NULL;

	::srand(::timeGetTime() + nLotteryType + nRank + nColor);
	CLottery* pLottery = this->GetLotteryByRoll(nLotteryType, nRank, ::RandGet(nSum));
	if (!pLottery)
		return NULL;
	if (nColor > 0 && nColor != pLottery->GetColor())
		pLottery = this->GetLotteryByRoll(nLotteryType, nRank, ::RandGet(nSum));
	return pLottery;
}

bool CLotteryMgr::AwardPrize(CLottery* pLottery, ILotteryActor* pActor)
{
	IF_NOT(pLottery && pActor)
		return false;

	ILotteryPrize* pPrize = pActor->AwardItem(pLottery->GetPrizeItem());
	IF_NOT(pPrize)
		return false;

	if (pPrize->IsEquip())
	{
		pPrize->SetInt(0, pLottery->GetHoleNum());
		pPrize->SetInt(1, pLottery->GetAdditionLev());
	}

	CMsgItemInfo	msg;
	IF_OK(msg.Create(pPrize, ITEMINFO_UPDATE))
		pActor->SendMsg((PLUGIN_MSG*)&msg);
	return true;
}

bool ModuleCreate(IRegister* pRegister, IDatabase* pDb)
{
	IF_NOT(pDb && pRegister)
		return false;

	if (!CConfigMgr::GetInstance()->Create(pDb))
		return false;
	if (!CLotteryMgr::GetInstance()->Create(pDb))
		return false;

	return pRegister->Register(ADAPTER_MODULE_LOTTERY, CLotteryMgr::GetInstance());
}
