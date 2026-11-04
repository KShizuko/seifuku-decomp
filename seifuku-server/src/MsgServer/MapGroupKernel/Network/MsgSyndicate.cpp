#include "AllMsg.h"
#include "basefunc.h"
#include "MapGroup.h"
#include "SynManager.h"

CMsgSyndicate::CMsgSyndicate()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgSyndicate::~CMsgSyndicate()
{
}

BOOL	CMsgSyndicate::Create(unsigned short usAction, OBJID idTarget)
{
	if(usAction == ACTION_NONE)
	{
		MYASSERT(!"CMsgSyndicate::Create()");
		return false;
	}

	this->Init();

	m_pInfo->usAction	=usAction;
	m_pInfo->idTarget	=idTarget;
	m_unMsgSize	=sizeof(MSG_Info);
	m_unMsgType	=_MSG_SYNDICATE;

	return true;
}

BOOL	CMsgSyndicate::Create		(char* pMsgBuf, DWORD dwSize)
{
	if (!CNetMsg::Create(pMsgBuf, dwSize))
		return false;

	if(_MSG_SYNDICATE != this->GetType())
		return false;

	return true;
}

void	CMsgSyndicate::Process		(void* pSocket)
{
	MYASSERT(pSocket);
	if(!pSocket)
		return;

	CUserPtr pUser = UserManager()->GetUser(this);
	IF_NOT(pUser)
		return;
	OBJID idUser = pUser->GetID();
	IF_NOT(idUser != ID_NONE)
		return;

	if(!pUser->IsAlive())
	{
		pUser->SendSysMsg(STR_DIE);
		return ;
	}

	switch(m_pInfo->usAction)
	{
	case	APPLY_JOIN:
		{
			if(m_pInfo->idTarget == ID_NONE || m_pInfo->idTarget == idUser)
				return;

			CUserPtr pTarget = UserManager()->GetUser(m_pInfo->idTarget);
			if(!pTarget)
				return ;

			OBJID	idSyn	= pUser->GetSynID();
			int		nRank	= pUser->GetSynRank();
			OBJID	idTargetSyn = pTarget->GetSynID();
			int		nTargetRank	= pTarget->GetSynRank();

			if(idSyn != ID_NONE || idTargetSyn == ID_NONE || nTargetRank < 90)
			{
				pUser->SendSysMsg(STR_JOIN_SYN_FAILED);
				return;
			}

			CSyndicate* pTargetSyn = SynManager()->QuerySyndicate(idTargetSyn);
			if(pTargetSyn && pTargetSyn->GetInt(SYNDATA_MONEY) < SYN_MONEY_BASELINE)
			{
				pUser->SendSysMsg(STR_TOO_LOWER_SYN_MONEY);
				return;
			}

			if(pTarget->QuerySynAttr()->SumSynMember() > SYN_MEMBER_LIMIT)
			{
				pUser->SendSysMsg(STR_SYN_FULL);
				return;
			}

			if(!pTarget->FetchApply(CUser::APPLY_INVITE_JOIN_SYN, idUser))
			{
				pUser->SetApply(CUser::APPLY_JOIN_SYN, m_pInfo->idTarget);
				IF_OK_(Create(APPLY_JOIN, idUser))
					pTarget->SendMsg(this);
				return;
			}

			pUser->QuerySynAttr()->JoinSyn(idTargetSyn);
		}
		break;
	case	INVITE_JOIN:
		{
			if(m_pInfo->idTarget == ID_NONE || m_pInfo->idTarget == idUser)
				return;

			CUserPtr pTarget = UserManager()->GetUser(m_pInfo->idTarget);
			if(!pTarget)
				return ;

			OBJID	idSyn	= pUser->GetSynID();
			int		nRank	= pUser->GetSynRank();
			OBJID	idTargetSyn = pTarget->GetSynID();
			int		nTargetRank	= pTarget->GetSynRank();
			if(idSyn == ID_NONE || nRank < 90 || idTargetSyn != ID_NONE)
			{
				pUser->SendSysMsg(STR_JOIN_SYN_FAILED);
				return;
			}

			CSyndicate* pSyn = SynManager()->QuerySyndicate(idSyn);
			if(pSyn && pSyn->GetInt(SYNDATA_MONEY) < SYN_MONEY_BASELINE)
			{
				pUser->SendSysMsg(STR_TOO_LOWER_SYN_MONEY);
				return;
			}

			if(pUser->QuerySynAttr()->SumSynMember() > SYN_MEMBER_LIMIT)
			{
				pUser->SendSysMsg(STR_SYN_FULL);
				return;
			}

			if(!pTarget->FetchApply(CUser::APPLY_JOIN_SYN, idUser))
			{
				pUser->SetApply(CUser::APPLY_INVITE_JOIN_SYN, m_pInfo->idTarget);
				IF_OK_(Create(INVITE_JOIN, idUser))
					pTarget->SendMsg(this);
				return;
			}

			pTarget->QuerySynAttr()->JoinSyn(idSyn);
		}
		break;

	case	LEAVE_SYN:
		{
			OBJID idSyn = pUser->GetSynID();
			if(idSyn == ID_NONE)
				return;

			int	nRank = pUser->GetSynRank();
			if(nRank == RANK_LEADER)
			{
				pUser->SendSysMsg(STR_NO_DISBAND);
				return;
			}

			if(pUser->QuerySynAttr()->GetInt(SYNATTRDATA_PROFFER) < 20000)
				return;

			pUser->QuerySynAttr()->LeaveSyn();
		}
		break;
	case	KICKOUT_MEMBER:
		{
			if(m_pInfo->idTarget == ID_NONE || m_pInfo->idTarget == idUser)
				return;

			CUserPtr pTarget = UserManager()->GetUser(m_pInfo->idTarget);
			if(!pTarget)
				return ;

			OBJID idSyn = pUser->GetSynID();
			int nRank = pUser->GetSynRank();
			OBJID idTargetSyn = pTarget->GetSynID();
			if(idSyn == ID_NONE || nRank != RANK_LEADER || idTargetSyn == ID_NONE || idTargetSyn != idSyn)
			{
				pUser->SendSysMsg(STR_NOT_AUTHORIZED);
				return;
			}
			CSyndicate* pSyn = SynManager()->QuerySyndicate(idSyn);

			if(pSyn && pSyn->GetInt(SYNDATA_MONEY) < SYN_MONEY_BASELINE)
			{
				pUser->SendSysMsg(STR_TOO_LOWER_SYN_MONEY);
				return;
			}

			pTarget->QuerySynAttr()->LeaveSyn();
			pTarget->SendSysMsg(STR_KICKOUT_ME_s, pUser->GetName());

			MSGBUF	szMsg;
			sprintf(szMsg, STR_KICKOUT_MEMBER_s, pTarget->GetName());
			IF_OK(pSyn)
				pSyn->BroadcastSubSynMsg(GetSocketID(), GetNpcID(), szMsg);
		}
		break;
	case	QUERY_SYN_NAME:
		{
			OBJID idTargetSyn	= m_pInfo->idTarget;
			CSynPtr pTargetSyn = SynManager()->QuerySyndicate(idTargetSyn);
			if(!pTargetSyn)
			{
				return;
			}

			CSynPtr pTargetMasterSyn = pTargetSyn->GetMasterSyn();
			CHECK(pTargetMasterSyn);

			CSyndicate* pSyn = pUser->GetSyndicate();
			CMsgName	msg;
			int nAction = NAMEACT_SYNDICATE;
			if(pSyn && pSyn->IsFriendly(idTargetSyn))
				nAction = NAMEACT_SYNDICATE_ALLY;
			else if(pSyn && pSyn->IsHostile(idTargetSyn))
				nAction = NAMEACT_SYNDICATE_ENEMY;
			IF_OK(msg.Create(nAction, pTargetMasterSyn->GetStr(SYNDATA_NAME), idTargetSyn))
			{
				if(pTargetSyn->GetID() != pTargetMasterSyn->GetID())
				{
					MYASSERT(msg.AppendStr(pTargetSyn->GetStr(SYNDATA_NAME)));
				}
				pUser->SendMsg(&msg);
			}
		}
		break;
	case	DONATE_MONEY:
		{
			if(pUser->GetSynID() == ID_NONE)
				return ;

			int nMoney = m_pInfo->idTarget;
			if(nMoney <= 0 || nMoney > (int)pUser->GetMoney())
				return;

			pUser->QuerySynAttr()->DonateMoney(nMoney);
		}
		break;
	case	QUERY_SYNATTR:
		{
			pUser->QuerySynAttr()->SendInfoToClient();
		}
		break;
	default:
		MYASSERT(!"switch MsgSyndicate");
		return;
	}
}
