#include "windows.h"
#include "Body.h"
#include "Array.h"
#include "Vartype.h"
#include "AGent.h"
#include "AGentData.h"
#include "AllMsg.h"
#include "NpcWorld.h"

CBody::CBody()
{

}

CBody::~CBody()
{

}

bool CBody::Create(CAgent* pOwner)
{
	m_pOwner	= pOwner;

	return true;
}

void CBody::OnTimer()
{
	IAiCenter* pAi = Cast<IAiCenter>(m_pOwner);
	CHECK(pAi);
	int idxAction;
	VarTypeSet setParam;
	if(pAi->FetchAction(&idxAction, &setParam))
	{
		DEBUG_TRY
		ProcessAction(idxAction, setParam);
		DEBUG_CATCH2("CBody::OnTimer(): %d", idxAction)
	}
}

bool CBody::ProcessAction(int idxAction, VarTypeSetRef setParam)
{
	CHECKF(idxAction >= 0);

	switch(idxAction)
	{
	case	ACTION_REBORN:
		{
			CMsgAction	msg;
			IF_OK(msg.Create(m_pOwner->GetID(), m_pOwner->GetPosX(), m_pOwner->GetPosY(), m_pOwner->GetDir(), actionReborn))
				m_pOwner->SendMsg(&msg);
		}
		break;
	case	ACTION_TALK_TO_NAME:
		{
			StringRef	szName	= setParam[0];
			StringRef	szMsg	= setParam[1];
			CUser* pRole = UserManager()->QueryUser(szName);
			if(pRole)
			{
				CMsgTalk	msg;
				IF_OK(msg.Create(m_pOwner->GetName(), szName, szMsg))
					m_pOwner->SendMsg(&msg);
			}
		}
		break;
	case	ACTION_TALK_TO_ID:
		{
			OBJID		idRole	= setParam[0];
			StringRef	szMsg	= setParam[1];
			CUser* pRole = UserManager()->QueryUser(idRole);
			if(pRole)
			{
				CMsgTalk	msg;
				IF_OK(msg.Create(m_pOwner->GetName(), pRole->GetName(), szMsg))
					m_pOwner->SendMsg(&msg);
			}
		}
		break;
	case	ACTION_MOVE_CLOSE:
		{
			OBJID	idRole = setParam[0];
			IRole* pRole = UserManager()->QueryRole(idRole);
			CRoleMove* pMove;
			if(pRole && Cast(&pMove, m_pOwner))
			{
				if(::RandGet(3) == 0)
					pMove->JumpTo(pRole->GetPosX(), pRole->GetPosY(), m_pOwner->GetAttackRange(pRole->GetSizeAdd()));
				else
					pMove->RunTo(pRole->GetPosX(), pRole->GetPosY(), m_pOwner->GetAttackRange(pRole->GetSizeAdd()));
			}
		}
		break;
	case	ACTION_JUMP_CLOSE:
		{
			int		nPosX = setParam[0];
			int		nPosY = setParam[1];
			int		nDist = setParam[2];
			CRoleMove* pMove = Cast<CRoleMove>(m_pOwner);
			if(pMove)
			{
				pMove->JumpTo(nPosX, nPosY, nDist);
			}
		}
		break;
	case	ACTION_JUMP_TO_SAFE:
		{
			CRoleMove* pMove;
			if(Cast(&pMove, m_pOwner))
			{
				pMove->JumpSafePos();
			}
		}
		break;
	case	ACTION_CHGMAP:
		{
			OBJID	idMap = setParam[0];
			int		nPosX = setParam[1];
			int		nPosY = setParam[2];
			CMsgAction	msg;
			IF_OK(msg.Create(m_pOwner->GetID(), nPosX, nPosY, m_pOwner->GetDir(), actionSuperChgMap, idMap))
				m_pOwner->SendMsg(&msg);
		}
		break;
	case	ACTION_RANDOM_WALK:
		{
			CRoleMove* pMove;
			if(Cast(&pMove, m_pOwner))
			{
				POINT	pos = pMove->FindRandomPos();
				CGameMap* pMap = m_pOwner->GetMap(); if(pMap && pMap->IsJumpEnable(pos.x, pos.y))
					pMove->WalkTo(pos.x, pos.y);
			}
		}
		break;
	case	ACTION_CHANGE_MODE:
		{
			int	nMode1 = setParam[0];
			int	nMode2 = setParam[1];

			int		nPriority	= 0;
			String	strFact = "Mode";
			strFact	+= '(';
			strFact	+= String().FormatLong(nMode1);
			strFact	+= ',';
			strFact	+= String().FormatLong(nMode2);
			strFact	+= ')';

			IAiCenter* pAi = Cast<IAiCenter>(m_pOwner);
			pAi->AddFact(strFact, nPriority);

			LOGDUMP("ChangeMode to [%d,%d] OK! ===============================================", nMode1, nMode2);
		}
		break;
	case	ACTION_LEAVE_EVIL:
		{
			int	nDist = setParam[0];
			CRoleMove* pMove;
			if(Cast(&pMove, m_pOwner))
			{
				if(pMove->JumpSafePos(nDist))
					return true;

				POINT pos = pMove->FindRandomPos();
				CGameMap* pMap = m_pOwner->GetMap(); if(pMap && pMap->IsJumpEnable(pos.x, pos.y))
				{
					pMove->JumpTo(pos.x, pos.y);
					return true;
				}
			}
		}
		break;
	case	ACTION_ATTACK:
		{
			OBJID	idRole = setParam[0];
			IRole* pRole = UserManager()->QueryRole(idRole);
			CRoleFight* pFight;
			if(pRole && Cast(&pFight, m_pOwner))
			{
				pFight->Attack(pRole);
			}
		}
		break;
	case	ACTION_USE_ITEM:
		{
			OBJID	idType = setParam[0];
			CItemPack* pPack = Cast<CItemPack>(m_pOwner);
			CItem* pItem = pPack->FindItemByType(idType);
			if(pItem)
			{
				CMsgItem	msg;
				IF_OK(msg.Create(pItem->GetID(), ITEMACT_USE))
					m_pOwner->SendMsg(&msg);
			}
		}
		break;
	case	ACTION_FULL_MEDICINE:
		{
			OBJID	idItem = setParam[0];
			CMsgAction	msg;
			IF_OK(msg.Create(m_pOwner->GetID(), m_pOwner->GetPosX(), m_pOwner->GetPosY(), m_pOwner->GetDir(), actionFullItem, idItem))
				m_pOwner->SendMsg(&msg);
			LOGDUMP("ACTION_FULL_MEDICINE");
		}
		break;
	case	ACTION_ACCEPT_INVITE:
		{
			OBJID	idRole = setParam[0];
			CMsgTeam	msg;
			IF_OK(msg.Create(idRole, _MSGTEAM_ACCEPTINVEIT))
				m_pOwner->SendMsg(&msg);
		}
		break;
	case	ACTION_SEND_APPLY:
		{
			OBJID	idRole = setParam[0];
			CMsgTeam	msg;
			IF_OK(msg.Create(idRole, _MSGTEAM_APPLYJOIN))
				m_pOwner->SendMsg(&msg);
		}
		break;
	case	ACTION_LEAVE_TEAM:
		{
			CMsgTeam	msg;
			IF_OK(msg.Create(m_pOwner->GetID(), _MSGTEAM_LEAVE))
				m_pOwner->SendMsg(&msg);
		}
		break;
	default:
		MYASSERT(!"switch(idxAction)");
	}

	return false;
}
