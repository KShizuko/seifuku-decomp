#include "allmsg.h"
#include "mapgroup.h"

CMsgTeam::CMsgTeam()
{
	this->Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgTeam::~CMsgTeam()
{

}

BOOL CMsgTeam::Create(OBJID idPlayer, int nAction)
{
	this->Init();

	m_unMsgSize	=sizeof(MSG_Info);
	m_unMsgType	=_MSG_TEAM;

	m_pInfo->idPlayer	= idPlayer;
	m_pInfo->unAction	= nAction;
	return true;
}

BOOL CMsgTeam::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if(!pbufMsg)
		return false;

	memcpy(this->m_bufMsg, pbufMsg, dwMsgSize);
	return true;
}

void CMsgTeam::Process(void* pInfo)
{
	CUserPtr pUser	= UserManager()->GetUser(this);
	if (!pUser)
		return;

	if (pUser->IsGM() && !pUser->IsPM())
	{
		pUser->SendSysMsg("【GM】不能进行队伍操作。");
		return;
	}

	CGameMap* pMap = pUser->GetMap();
	if (pMap && pMap->IsTeamDisable())
		return;

	switch(m_pInfo->unAction)
	{
	case _MSGTEAM_CREATE:
		{
			DEBUG_TRY
			if (pUser->CreateTeam())
			{
				pUser->SendMsg(this);
			}
			DEBUG_CATCH("MsgTeam proc create team crash.")
		}
		break;
	case _MSGTEAM_APPLYJOIN:
		{
			if (m_pInfo->idPlayer == ID_NONE)
				return;

			if (pUser->GetTeam())
			{
				pUser->SendSysMsg(STR_INTEAM_NOJOIN);
				return;
			}

			OBJID	idTarget = m_pInfo->idPlayer;
			CUser* pTarget = NULL;

			DEBUG_TRY
			if(!pUser->GetMap()->QueryObj(pUser->GetPosX(), pUser->GetPosY(), OBJ_USER, idTarget, IPP_OF(pTarget)))
			{
				pUser->SendSysMsg(STR_NO_APPLICANT);
				return;
			}
			DEBUG_CATCH("MsgTeam apply join Queryobj by ID crash.");

			CTeam* pTeam	= pTarget->GetTeam();
			if (!pTeam)
			{
				pUser->SendSysMsg(STR_NO_TEAM);
				return;
			}

			if (pTeam->IsClosed())
			{
				pUser->SendSysMsg(STR_FORBIDDEN_JOIN);
				return;
			}

			if (pTeam->GetMemberAmount() >= _MAX_TEAMAMOUNT)
			{
				pUser->SendSysMsg(STR_TEAM_FULL);
				return;
			}

			DEBUG_TRY
			if (pTeam->GetLeader() != pTarget->GetID())
			{
				pUser->SendSysMsg(STR_NO_CAPTAIN_CLOSE);
				return;
			}
			DEBUG_CATCH("CMsgTeam apply join Queryobj by leader id crash.");

			pUser->SetApply(CUser::APPLY_TEAMAPPLY, idTarget);
			IF_NOT(this->Create(pUser->GetID(), _MSGTEAM_APPLYJOIN))
				return;
			
			pTarget->SendMsg(this);
			pUser->SendSysMsg(STR_JOIN_REQUEST_SENT);
		}
		break;
	case _MSGTEAM_AGREEJOIN:
		{
			if (m_pInfo->idPlayer == ID_NONE)
				return;

			CTeam* pTeam	= pUser->GetTeam();
			if (!pTeam)
			{
				pUser->SendSysMsg(STR_NO_TEAM_TO_INVITE);
				return;
			}
			if (pTeam->GetLeader() != pUser->GetID())
			{
				pUser->SendSysMsg(STR_NOT_CAPTAIN_INVITE);
				return;
			}
			if (pTeam->GetMemberAmount() >= _MAX_TEAMAMOUNT)
			{
				pUser->SendSysMsg(STR_YOUR_TEAM_FULL);
				return;
			}

			OBJID	idTarget = m_pInfo->idPlayer;
			CUser* pTarget = NULL;

			DEBUG_TRY
			if(!pUser->GetMap()->QueryObj(pUser->GetPosX(), pUser->GetPosY(), OBJ_USER, idTarget, IPP_OF(pTarget)))
			{
				pUser->SendSysMsg(STR_APPLICANT_NOT_FOUND);
				return;
			}
			DEBUG_CATCH("CMsgTeam proc accept join Queryobj by idtarget crash.")

			if (pTarget->GetTeam())
			{
				pTarget->SendSysMsg(STR_HAVE_JOIN_TEAM);
				return;
			}

			if (pTarget->FetchApply(CUser::APPLY_TEAMAPPLY, pUser->GetID()))
			{
				pTarget->SetApply(CUser::APPLY_NONE, ID_NONE);
				if (pUser->AcceptTeamMember(pTarget))
				{
				}
			}
			else
			{
				pUser->SendSysMsg(STR_NOT_TO_JOIN);
			}
		}
		break;
	case _MSGTEAM_INVITE:
		{
			if (m_pInfo->idPlayer == ID_NONE)
				return;

			CTeam* pTeam	= pUser->GetTeam();
			if (!pTeam)
			{
				pUser->SendSysMsg(STR_NOTEAM_TO_INVITE);
				return;
			}

			if (pTeam->GetLeader() != pUser->GetID())
			{
				pUser->SendSysMsg(STR_NOT_CAPTAIN_ACCEPT);
				return;
			}

			if (pTeam->GetMemberAmount() >= _MAX_TEAMAMOUNT)
			{
				pUser->SendSysMsg(STR_YOUR_TEAM_FULL);
				return;
			}

			OBJID	idTarget = m_pInfo->idPlayer;
			CUser* pTarget = NULL;

			DEBUG_TRY
			if(!pUser->GetMap()->QueryObj(pUser->GetPosX(), pUser->GetPosY(), OBJ_USER, idTarget, IPP_OF(pTarget)))
			{
				pUser->SendSysMsg(STR_INVITED_NOT_FOUND);
				return;
			}
			DEBUG_CATCH("CMsgTeam proc invite queryobj by id crash.");

			if (pTarget->GetTeam())
			{
				pUser->SendSysMsg(STR_HAS_IN_TEAM);
				return;
			}

			pUser->SetApply(CUser::APPLY_TEAMINVIT, pTarget->GetID());
			IF_NOT(this->Create(pUser->GetID(), _MSGTEAM_INVITE))
				return;
			
			pTarget->SendMsg(this);
			pUser->SendSysMsg(STR_INVITE_SENT);
		}
		break;
	case _MSGTEAM_ACCEPTINVITE:
		{
			if (m_pInfo->idPlayer == ID_NONE)
				return;

			if (pUser->GetTeam())
			{
				return;
			}

			OBJID	idTarget = m_pInfo->idPlayer;
			CUser* pTarget = NULL;

			DEBUG_TRY
			if(!pUser->GetMap()->QueryObj(pUser->GetPosX(), pUser->GetPosY(), OBJ_USER, idTarget, IPP_OF(pTarget)))
			{
				return;
			}
			DEBUG_CATCH("CMsgTeam proc accept invite queryobj by id crash.")

			CTeam* pTeam	= pTarget->GetTeam();
			if (!pTeam)
			{
				pUser->SendSysMsg(STR_NOT_CREATE_TEAM);
				return;
			}
			if (pTeam->GetMemberAmount() >= _MAX_TEAMAMOUNT)
			{
				pUser->SendSysMsg(STR_HIS_TEAM_FULL);
				return;
			}

			if (pTeam->GetLeader() != pTarget->GetID())
			{
				pUser->SendSysMsg(STR_NO_CAPTAIN_CLOSE);
				return;
			}

			if (pTarget->FetchApply(CUser::APPLY_TEAMINVIT, pUser->GetID()))
			{
				pTarget->SetApply(CUser::APPLY_NONE, ID_NONE);
				if (pUser->AcceptTeamInvit(pTarget))
				{
				}
			}
			else if (pTeam->IsUnk28())
			{
				pUser->AcceptTeamInvit(pTarget);
			}
			else
			{
				pUser->SendSysMsg(STR_NOT_BE_INVIITED);
			}
		}
		break;
	case _MSGTEAM_LEAVE:
		{
			DEBUG_TRY
			CTeam* pTeam	= pUser->GetTeam();
			if (!pTeam)
			{
				return;
			}

			m_pInfo->idPlayer	= pUser->GetID();
			pUser->QuitTeam();
			DEBUG_CATCH("CMsgTeam proc quitteam crash.")
		}
		break;
	case _MSGTEAM_DISMISS:
		{
			DEBUG_TRY
			CTeam* pTeam	= pUser->GetTeam();
			if (!pTeam)
			{
				return;
			}
			if (pTeam->GetLeader() != pUser->GetID())
			{
				return;
			}

			pUser->DismissTeam();
			DEBUG_CATCH("CMsgTeam proc dismiss crash.")
		}
		break;
	case _MSGTEAM_OPENTEAM:
		{
			CTeam* pTeam	= pUser->GetTeam();
			if (!pTeam)
			{
				return;
			}
			
			pTeam->Open();
		}
		break;
	case _MSGTEAM_CLOSETEAM:
		{
			CTeam* pTeam	= pUser->GetTeam();
			if (!pTeam)
			{
				return;
			}
			
			pTeam->Close();
		}
		break;
	case _MSGTEAM_KICKOUT:
		{
			CTeam* pTeam	= pUser->GetTeam();
			if (!pTeam)
			{
				return;
			}

			if(pUser->GetID() != m_pInfo->idPlayer)
				pUser->KickOut(m_pInfo->idPlayer);
		}
		break;
	case _MSGTEAM_CLOSEMONEYACCESS:
		{
			CTeam* pTeam	= pUser->GetTeam();
			if (!pTeam)
				return;

			pTeam->CloseMoney();
		}
		break;
	case _MSGTEAM_OPENMONEYACCESS:
		{
			CTeam* pTeam	= pUser->GetTeam();
			if (!pTeam)
				return;

			pTeam->OpenMoney();
		}
		break;
	case _MSGTEAM_CLOSEITEMACCESS:
		{
			CTeam* pTeam	= pUser->GetTeam();
			if (!pTeam)
				return;

			pTeam->CloseItem();
		}
		break;
	case _MSGTEAM_OPENITEMACCESS:
		{
			CTeam* pTeam	= pUser->GetTeam();
			if (!pTeam)
				return;

			pTeam->OpenItem();
		}
		break;
	}
}