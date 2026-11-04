#include "MsgTeam.h"
#include "Hero.h"
#include "GameMsg.h"
#include "GameDataSet.h"

CMsgTeam::CMsgTeam()
{
	Init();
	m_pInfo	=(MSG_Info* )m_bufMsg;
}

CMsgTeam::~CMsgTeam()
{

}

BOOL CMsgTeam::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if(!pbufMsg)
		return false;
	if((unsigned short)dwMsgSize != GetSize(pbufMsg, dwMsgSize))
		return false;
	if(_MSG_TEAM != GetType(pbufMsg, dwMsgSize))
		return false;

	memcpy(this->m_bufMsg, pbufMsg, dwMsgSize);
	return true;
}

BOOL CMsgTeam::Create(OBJID idPlayer, int nAction)
{
	this->Init();

	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_TEAM;

	m_pInfo->idPlayer = idPlayer;
	m_pInfo->unAction = nAction;
	return true;
}

void CMsgTeam::Process(void* pInfo)
{
		switch(m_pInfo->unAction)
		{
		case _MSGTEAM_CREATE:
			{
				g_objHero.ClearTeamMember();
				TeamMemberInfo* pInfo = CTeam::CreateNewMemberInfo((char*)g_objHero.GetName(),
										g_objHero.GetID(),
										g_objHero.GetFace(),
										g_objHero.GetData(CPlayer::_PLAYER_LIFE),
										g_objHero.GetMaxLife());
				g_objHero.AddTeamMember(pInfo);
				::PostCmd(CMD_FLASHTEAM);
			}
			break;
		case _MSGTEAM_APPLYJOIN:
			{
				if(g_objHero.SetTeamLastApply(m_pInfo->idPlayer))
					::PostCmd(CMD_TEAMAPPLY);
			}
			break;
		case _MSGTEAM_LEAVE:
			{
				if(g_objHero.GetID() == m_pInfo->idPlayer)
				{
					g_objHero.ClearTeamMember();
					OBJID idLeaveTeam = 100107;
					g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idLeaveTeam));
				}
				else
				{
					g_objHero.DelTeamMember(m_pInfo->idPlayer);
					TeamMemberInfo* pInfo = g_objHero.GetTeamMemberInfoByID(m_pInfo->idPlayer);
					if(pInfo)
					{
						char szMsg[128];
						OBJID idLeaveTeam = 100108;
						sprintf(szMsg, GameDataSetQuery()->GetStr(idLeaveTeam), pInfo->szName);
						g_objGameMsg.AddMsg(szMsg);
					}
				}
				::PostCmd(CMD_FLASHTEAM);
			}
			break;
		case _MSGTEAM_ACCEPTINVITE:
			break;
		case _MSGTEAM_INVITE:
			{
				if(g_objHero.SetTeamLastApply(m_pInfo->idPlayer))
					::PostCmd(CMD_TEAMINVITE);
			}
			break;
		case _MSGTEAM_AGREEJOIN:
			break;
		case _MSGTEAM_DISMISS:
			{
				g_objHero.ClearTeamMember();
				const OBJID idDismiss = 100109;
				g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idDismiss));
				::PostCmd(CMD_FLASHTEAM);
			}
			break;
		case _MSGTEAM_KICKOUT:
			{
				if(g_objHero.GetID() == m_pInfo->idPlayer)
				{
					const OBJID idKickOut = 100110;
					g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idKickOut));
					g_objHero.ClearTeamMember();
				}
				else
				{
					TeamMemberInfo* pInfo = g_objHero.GetTeamMemberInfoByID(m_pInfo->idPlayer);
					if(pInfo)
					{
						const OBJID idKickOut = 100111;
						char szMsg[128];
						sprintf(szMsg, GameDataSetQuery()->GetStr(idKickOut), pInfo->szName);
						g_objGameMsg.AddMsg(szMsg);
					}
					g_objHero.DelTeamMember(m_pInfo->idPlayer);
				}
				::PostCmd(CMD_FLASHTEAM);
			}
			break;
		case _MSGTEAM_CLOSETEAM:
			{
				g_objHero.CloseTeam();
				const OBJID idClose = 100112;
				g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idClose));
				::PostCmd(CMD_FLASHTEAM);
			}
			break;
		case _MSGTEAM_OPENTEAM:
			{
				g_objHero.OpenTeam();
				const OBJID idOpen = 100113;
				g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idOpen));
				::PostCmd(CMD_FLASHTEAM);
			}
			break;
		}
}
