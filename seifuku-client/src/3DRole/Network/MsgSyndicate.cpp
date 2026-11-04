#include "MsgSyndicate.h"
#include "GamePlayerSet.h"
#include "Hero.h"

CMsgSyndicate::CMsgSyndicate()
{
	Init();
	m_pInfo	=(MSG_Info* )m_bufMsg;
}

CMsgSyndicate::~CMsgSyndicate()
{

}

BOOL CMsgSyndicate::Create(char* pMsgBuf, DWORD dwSize)
{
	if (!CNetMsg::Create(pMsgBuf, dwSize))
		return false;
	if(_MSG_SYNDICATE != this->GetType())
		return false;

	return true;
}

BOOL CMsgSyndicate::Create(unsigned short usAction, OBJID idTarget)
{
	this->Init();
	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_SYNDICATE;
	m_pInfo->usAction = usAction;
	m_pInfo->idTarget = idTarget;

	return true;
}

void CMsgSyndicate::Process(void* pInfo)
{
	switch(m_pInfo->usAction)
	{
	case APPLY_JOIN:
		{
			::PostCmd(CMD_SYNDICATE_APPLY);
			g_objHero.SetLastSyndicateApplyID(m_pInfo->idTarget);
		}
		break;
	case INVITE_JOIN:
		{
			CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_pInfo->idTarget);
			if(pPlayer)
			{
				::PostCmd(CMD_SYNDICATE_INVITE);
				g_objHero.SetLastSyndicateApplyID(m_pInfo->idTarget);
				g_objHero.SetLastSyndicateID(pPlayer->GetSydicateID());
			}
		}
		break;
	case ALLY_APPLY:
		{
			g_objPlayerSet.GetSyndicateName(m_pInfo->idTarget);
			g_objPlayerSet.SetSyndicateStatus(m_pInfo->idTarget, SYNDICATE_ALLY);
		}
		break;
	case CLEAR_ALLY:
		{
			g_objPlayerSet.GetSyndicateName(m_pInfo->idTarget);
			g_objPlayerSet.SetSyndicateStatus(m_pInfo->idTarget, SYNDICATE_NORMAL);
		}
		break;
	case CLEAR_ANTAGONIZE:
		{
			g_objPlayerSet.GetSyndicateName(m_pInfo->idTarget);
			g_objPlayerSet.SetSyndicateStatus(m_pInfo->idTarget, SYNDICATE_NORMAL);
		}
		break;
	case ANTAGONIZE:
		{
			g_objPlayerSet.GetSyndicateName(m_pInfo->idTarget);
			g_objPlayerSet.SetSyndicateStatus(m_pInfo->idTarget, SYNDICATE_ANTAGONIZE);
		}
		break;
	case SET_MAINSYN:
	case SET_SYN:
		{
			g_objPlayerSet.GetSyndicateName(m_pInfo->idTarget);
			g_objPlayerSet.SetSyndicateStatus(m_pInfo->idTarget, SYNDICATE_SELF);
		}
		break;
	case UNITE_SUBSYN:
		{
			g_objHero.UniteSubSyn(m_pInfo->dwData, m_pInfo->dwData);
		}
		break;
	case UNITE_SYN:
		{
			g_objHero.UniteSyn(m_pInfo->dwData, m_pInfo->dwData);
		}
		break;
	case SET_WHITE_SYN:
		g_objPlayerSet.SetWhiteSynId(m_pInfo->idTarget);
		break;
	case SET_BLACK_SYN:
		g_objPlayerSet.SetBlackSynId(m_pInfo->idTarget);
		break;
	case DESTROY_SYN:
		g_objPlayerSet.DestroySyndicate(m_pInfo->idTarget);
		break;
	}
}
