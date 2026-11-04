#include "AllMsg.h"
#include "BetOutside.h"
#include "MsgBetOutside.h"
#include "logfile.h"
#include "GameMap.h"
#include "MapGroup.h"

const int		MAXBOMONEY		= 30000;
const int		MINBOMONEY		= 100;
const int		BOCOUNTDOWNSECS	= 50;
const int		BOOPENDELAYSECS	= 2;
const int		BOPLAYDICESECS	= 8;

const int	PAYFOR_SIZE		= 22;
const int	g_aPayFor[PAYFOR_SIZE] = { 2, 2, 
						216, 216, 216, 216, 216, 216, 
						72, 36, 21, 14, 
						10, 8, 8, 8, 8, 10, 
						14, 21, 36, 72 };

MYHEAP_IMPLEMENTATION(CBet,s_heap)
MYHEAP_IMPLEMENTATION(CNpcBet,s_heap)
CNpcBet::CNpcBet()
{
	m_nState		= c_betNone;
	m_nCountDown	= 0;

	m_nSysGain		= 0;
	m_nSysGainAll	= 0;
	m_tGain			= timeGetTime() + BOSTAT_SECS*1000;

	ASSERT(PAYFOR_SIZE == CBet::DICE_VALUE_END);

	m_setBet		= CBetSet::CreateNew(true);
}

CNpcBet::~CNpcBet()
{
}

CNpcBet* CNpcBet::CreateNew(PROCESS_ID idProcess, CNpc* pNpc)
{
	CNpcBet* pBet = new CNpcBet;
	IF_OK(pBet)
	{
		pBet->m_idProcess	= idProcess;
		pBet->m_pOwner		= pNpc;
		return pBet;
	}
	return NULL;
}

bool	CNpcBet::JoinBet(CUser * pUser)
{
	CHECKF(pUser);

	if(m_setPlayer.GetObj(pUser->GetID()))
		m_setPlayer.DelObj(pUser->GetID());

	CMsgName	msg;
	IF_OK(msg.Create(NAMEACT_ADDDICE_PLAYER, pUser->GetName(), m_pOwner->GetID()))
		pUser->SendMsg(&msg);

	switch(m_nState)
	{
	case	c_betNone:
		m_nState		= c_betWait;
		m_nCountDown	= BOCOUNTDOWNSECS;
		m_setBet->Clear();
	case	c_betWait:
		{
			CMsgBODice	cMsg;
			IF_OK(cMsg.Create(_ACTION_BEGINCHIP, m_pOwner->GetID(), m_nCountDown))
				pUser->SendMsg(&cMsg);
		}
		break;
	case	c_betRun:
		{
			CMsgBODice	cMsg;
			IF_OK(cMsg.Create(_ACTION_ENDCHIP, m_pOwner->GetID()))
				pUser->SendMsg(&cMsg);
		}
		break;
	default:
		LOGERROR("CNpcBet::JoinBet() exception");
	}

	PLAYER_SET::Iterator pPlayer = m_setPlayer.NewEnum();
	while(pPlayer.Next())
	{
		IF_OK(pPlayer)
		{
			CMsgName	cMsg;
			if(cMsg.Create(NAMEACT_ADDDICE_PLAYER, pPlayer->GetName(), m_pOwner->GetID()))
				pUser->SendMsg(&cMsg);
		}
		CMsgBODice	cMsg;
		CHECKF(cMsg.Create(_ACTION_CHIPIN, m_pOwner->GetID()));
		bool	bOK = false;
		for(int i = 0; i < m_setBet->GetAmount(); i++)
		{
			CBet*	pBet = m_setBet->GetObjByIndex(i);
			MYASSERT(pBet);
			if(pBet && pBet->GetID() == pPlayer->GetID())
			{
				if(cMsg.Append(pBet->GetObjType(), pBet->GetMoney()))
					bOK = true;
			}
		}
		if(bOK)
			pUser->SendMsg(&cMsg);
	}

	m_setPlayer.AddObj(pUser->QueryLink());

	return true;
}

bool	CNpcBet::LeaveBet(CUser * pUser)
{
	CHECKF(pUser);

	if(!m_setPlayer.GetObj(pUser->GetID()))
		return false;

	switch(m_nState)
	{
	case	c_betWait:
		{
			for(int i = m_setBet->GetAmount() - 1; i >= 0 ; i--)
			{
				CBet*	pBet = m_setBet->GetObjByIndex(i);
				MYASSERT(pBet);
				if(pBet && pBet->GetID() == pUser->GetID())
				{
					CMsgBODice	cMsg;
					if(cMsg.Create(_ACTION_CANCELCHIP, m_pOwner->GetID()) && cMsg.Append(pBet->GetObjType(), pBet->GetMoney()))
					{
						BroadcastListMsg(&cMsg, pUser);
						
					}

					int	nMoney = pBet->GetMoney();
					MYASSERT(nMoney > 0);

					DEBUG_TRY
					m_setBet->DelObj(pUser->GetID(), pBet->GetObjType());
					pUser->GainMoney(nMoney, SYNCHRO_TRUE);
					DEBUG_CATCH("leave bet")

					m_nSysGain  -= nMoney;
				}
			}
		}
		break;
	case	c_betRun:
		break;
	default:
		LOGERROR("CNpcBet::LeaveBet() internal exception");
	}

	m_setPlayer.DelObj(pUser->GetID());
	if(m_setPlayer.Size() == 0)
		m_nState = c_betNone;

	PLAYER_SET::Iterator pPLayer = m_setPlayer.NewEnum();
	while(pPLayer.Next())
	{
		IF_OK(pPLayer)
		{
			CMsgName	cMsg;
			if(cMsg.Create(NAMEACT_DELDICE_PLAYER, pUser->GetName(), m_pOwner->GetID()))
				pPLayer->SendMsg(&cMsg);
		}
	}

	return true;
}

bool	CNpcBet::ChipIn(CUser * pUser,int nType, int nMoney)
{
	CHECKF(pUser && m_setPlayer.GetObj(pUser->GetID()));
	CHECKF(m_nState != c_betNone);
	CHECKF(nMoney == 0 && nType == CBet::DICE_VALUE_ALL || nMoney > 0 && nType>=0 && nType < CBet::DICE_VALUE_END);
	CHECKF(nMoney == 0 || nMoney == 100 || nMoney == 1000 || nMoney == 10000);

	char bufText[1024];
	if(m_nState != c_betWait)
	{
		sprintf(bufText, STR_MISS_BET);
		CMsgTalk msg;
		if(msg.Create(m_pOwner->GetName(), pUser->GetName(), bufText))
			pUser->SendMsg(&msg);

		return false;
	}

	int		nAllMoney	= 0;
	int		nThisMoney	= 0;
	for(int i = 0; i < m_setBet->GetAmount(); i++)
	{
		CBet*	pBet = m_setBet->GetObjByIndex(i);
		MYASSERT(pBet);
		if(pBet && pBet->GetID() == pUser->GetID())
		{
			nAllMoney += pBet->GetMoney();
			if(pBet->GetObjType() == nType)
				nThisMoney = pBet->GetMoney();
		}
	}
	MYASSERT(nAllMoney >= 0);
	MYASSERT(nThisMoney >= 0);

	if( nMoney > 0 && (int)pUser->GetMoney() < nMoney
					|| nAllMoney + nMoney > MAXBOMONEY )
		return false;

	MYASSERT(nThisMoney + nMoney >= 0);
	if(nMoney > 0)
	{
		CBet* pBet = new CBet(pUser->GetID(), nType, nThisMoney + nMoney);
		MYASSERT(pBet);
		if(!pBet)
			return false;
		m_setBet->DelObj(pUser->GetID(), nType);

		DEBUG_TRY
		pUser->SpendMoney(nMoney, SYNCHRO_TRUE);
		m_setBet->AddObj(pBet);
		DEBUG_CATCH("add bet")

		m_nSysGain		+= nMoney;
	}
	else
	{
		if(nType == CBet::DICE_VALUE_ALL)
		{
			for(int i = m_setBet->GetAmount() - 1; i >= 0 ; i--)
			{
				CBet*	pBet = m_setBet->GetObjByIndex(i);
				MYASSERT(pBet);
				if(pBet && pBet->GetID() == pUser->GetID())
				{
					CMsgBODice	cMsg;
					if(cMsg.Create(_ACTION_CANCELCHIP, m_pOwner->GetID()) && cMsg.Append(pBet->GetObjType(), pBet->GetMoney()))
						BroadcastListMsg(&cMsg, pUser);
					if(cMsg.Create(_ACTION_CANCELCHIP_CONFIRM, m_pOwner->GetID()) && cMsg.Append(pBet->GetObjType(), pBet->GetMoney()))
						pUser->SendMsg(&cMsg);

					int	nMoney = pBet->GetMoney();
					MYASSERT(nMoney > 0);

					DEBUG_TRY
					m_setBet->DelObj(pUser->GetID(), pBet->GetObjType());
					pUser->GainMoney(nMoney, SYNCHRO_TRUE);
					DEBUG_CATCH("del bet")

					m_nSysGain		-= nMoney;
				}
			}					
		}
		else
			m_setBet->DelObj(pUser->GetID(), nType);
	}

	if(nMoney > 0)
	{
		CMsgBODice	cMsg;
		if(cMsg.Create(_ACTION_CHIPIN_CONFIRM, m_pOwner->GetID()) && cMsg.Append(nType, nMoney))
			pUser->SendMsg(&cMsg);
		if(cMsg.Create(_ACTION_CHIPIN, m_pOwner->GetID()) && cMsg.Append(nType, nMoney))
			BroadcastListMsg(&cMsg, pUser);
	}
	else
	{
		CMsgBODice	cMsg;
		if(cMsg.Create(_ACTION_CANCELCHIP_CONFIRM, m_pOwner->GetID()) && cMsg.Append(nType, nMoney))
			pUser->SendMsg(&cMsg);
		if(cMsg.Create(_ACTION_CANCELCHIP, m_pOwner->GetID()) && cMsg.Append(nType, nThisMoney))
			BroadcastListMsg(&cMsg, pUser);
	}

	return true;
}

void	CNpcBet::OnTimer()
{
	if(!m_tClock.ToNextTime(1))
		return ;

	switch(m_nState)
	{
	case	c_betNone:
		break;
	case	c_betWait:
		--m_nCountDown;
		if(m_nCountDown <= 0)
		{
			ProcessRun();
			m_nState = c_betRun;
		}
		break;
	case	c_betRun:
		--m_nCountDown;
		if(m_nCountDown == -BOOPENDELAYSECS)
		{
			ProcessOpen();

			if(m_nSysGain && timeGetTime() > m_tGain)
			{
				m_nSysGainAll	+= m_nSysGain;
				LOGMSG("BET STAT: dice house total income [%d], half-hour income [%d]", m_nSysGainAll, m_nSysGain);
				m_nSysGain		= 0;
				m_tGain		= timeGetTime() + BOSTAT_SECS*1000;
			}
		}
		else if(m_nCountDown <= -(BOOPENDELAYSECS+BOPLAYDICESECS))
		{
			ProcessNew();

			m_nState		= c_betWait;
			m_nCountDown	= BOCOUNTDOWNSECS;
			m_setBet->Clear();
		}
		break;
	default:
		LOGERROR("CNpcBet::OnTimer() internal exception");
	}
}

void CNpcBet::BroadcastListMsg(CNetMsg *pMsg, CUser* pExcludeUser)
{
	PLAYER_SET::Iterator pUser = m_setPlayer.NewEnum();
	while(pUser.Next())
	{
		if(pUser && (pExcludeUser==NULL || pUser->GetID() != pExcludeUser->GetID()))
		{
			pUser->SendMsg(pMsg);
		}
	}
}

void CNpcBet::ProcessRun()
{
	CMsgBODice	cMsg;
	if(cMsg.Create(_ACTION_ENDCHIP, m_pOwner->GetID()))
		BroadcastListMsg(&cMsg, NULL);
}

void CNpcBet::ProcessOpen()
{
	srand(timeGetTime() + m_setPlayer.Size() + m_setBet->GetAmount());
	for(int i = 0; i < 4; i++)
	{
		m_aDice[i] = (rand() % 6) + 1;
	}
	CMsgBODice		cMsg;
	if(cMsg.Create(_ACTION_DICE, m_pOwner->GetID(), m_aDice))
	{
		m_pOwner->BroadcastRoomMsg(&cMsg);
	}
	
	for(int i = m_setBet->GetAmount() - 1; i >= 0 ; i--)
	{
		CBet*	pBet = m_setBet->GetObjByIndex(i);
		IF_NOT(pBet)
			continue;
		OBJID	idUser	= pBet->GetID();
		int		nType	= pBet->GetObjType();
		int		nMoney	= pBet->GetMoney();
		CUser*	pUser	= UserManager()->GetUser(idUser);
		MYASSERT(pUser && nMoney >= MINBOMONEY && nMoney <= MAXBOMONEY);
		if(!(pUser && nMoney >= MINBOMONEY && nMoney <= MAXBOMONEY))
		{
			LOGWARNING("Dice player offline or bad bet. User ID [%u], money [%d]", idUser, nMoney);
			m_setBet->DelObj(idUser, nType);
			continue;
		}
		int		nDiceSum = m_aDice[0] + m_aDice[1] + m_aDice[2];
		switch(nType)
		{
		case	CBet::DICE_VALUE_SMALL:
			if(nDiceSum <= 10)
				nMoney *= g_aPayFor[nType];
			else
				nMoney = 0;
			break;
		case	CBet::DICE_VALUE_BIG:
			if(nDiceSum >= 11)
				nMoney *= g_aPayFor[nType];
			else
				nMoney = 0;
			break;
		case	CBet::DICE_VALUE_111:
		case	CBet::DICE_VALUE_222:
		case	CBet::DICE_VALUE_333:
		case	CBet::DICE_VALUE_444:
		case	CBet::DICE_VALUE_555:
		case	CBet::DICE_VALUE_666:
			if(m_aDice[0] == m_aDice[1] 
				&& m_aDice[1] == m_aDice[2] 
				&& m_aDice[2] == nType - CBet::DICE_VALUE_111 + 1)
			{
				nMoney *= g_aPayFor[nType];
			}
			else
				nMoney = 0;
			break;
		case	CBet::DICE_VALUE_4:
		case	CBet::DICE_VALUE_5:
		case	CBet::DICE_VALUE_6:
		case	CBet::DICE_VALUE_7:
		case	CBet::DICE_VALUE_8:
		case	CBet::DICE_VALUE_9:
		case	CBet::DICE_VALUE_10:
		case	CBet::DICE_VALUE_11:
		case	CBet::DICE_VALUE_12:
		case	CBet::DICE_VALUE_13:
		case	CBet::DICE_VALUE_14:
		case	CBet::DICE_VALUE_15:
		case	CBet::DICE_VALUE_16:
		case	CBet::DICE_VALUE_17:
			if(nDiceSum == nType - CBet::DICE_VALUE_4 + 4)
				nMoney *= g_aPayFor[nType];
			else
				nMoney = 0;
			break;
		default:
			m_setBet->DelObj(idUser, nType);
			LOGERROR("CNpcBet::OnTimer() switch internal exception");
		}
		
		if(nMoney)
		{
			CBet* pBet = m_setBet->GetObj(idUser, nType);
			if(pBet)
				pBet->SetMoney(nMoney);
			MYASSERT(nMoney > 0);
			if(nMoney > 0)
			{
				pUser->GainMoney(nMoney, UPDATE_TRUE);
				m_nSysGain		-= nMoney;
			}
			
			CMsgName	cMsg;
			if(cMsg.Create(NAMEACT_DICE_BONUS, pUser->GetName(), nMoney))
				BroadcastListMsg(&cMsg, NULL);

			if(nMoney > 2*MAXBOMONEY)
			{
				char bufText[1024];
				sprintf(bufText, STR_WIN_BET_sd, pUser->GetName(), nMoney);
				pUser->BroadcastRoomMsg(bufText, INCLUDE_SELF);
			}

		}
		else
		{
			CBet* pBet = m_setBet->GetObj(idUser, nType);
			if(pBet && pBet->GetMoney() >= 10000)
				::MyLogSave("gmlog/gamble", "User:%s(%u) lost money %u in gamble.", pUser->GetName(), pUser->GetID(), pBet->GetMoney());
			m_setBet->DelObj(idUser, nType);
		}
	}
}

void CNpcBet::ProcessNew()
{
	CMsgBODice	cMsg;
	if(cMsg.Create(_ACTION_BEGINCHIP, m_pOwner->GetID(), BOCOUNTDOWNSECS))
		BroadcastListMsg(&cMsg, NULL);
}
