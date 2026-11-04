#include "AllMsg.h"
#include "npcworld.h"
#include "BaseFunc.h"
#include "agent.h"

CMsgAction::CMsgAction()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgAction::~CMsgAction()
{

}

BOOL CMsgAction::Create(OBJID idPlayer, int nPosX, int nPosY, USHORT usDir, USHORT usAction, DWORD dwData)
{
	if (idPlayer == ID_NONE)
		return false;

	this->Init();

	m_unMsgSize	=sizeof(MSG_Info);
	m_unMsgType	=_MSG_ACTION;
	m_pInfo->dwTimeStamp=::TimeGet();	

	m_pInfo->idUser		=idPlayer;
	m_pInfo->unPosX		=(USHORT)nPosX;
	m_pInfo->unPosY		=(USHORT)nPosY;
	m_pInfo->unDir		=usDir;
	m_pInfo->dwData		=dwData;
	m_pInfo->usAction	=usAction;

	return true;
}

BOOL CMsgAction::Create(OBJID idPlayer, int nPosX, int nPosY, USHORT usDir, USHORT usAction, USHORT usTargetPosX, USHORT usTargetPosY)
{
	if (idPlayer == ID_NONE)
		return false;

	this->Init();

	m_unMsgSize	=sizeof(MSG_Info);
	m_unMsgType	=_MSG_ACTION;
	m_pInfo->dwTimeStamp=::TimeGet();	

	m_pInfo->idUser		=idPlayer;
	m_pInfo->unPosX		=(USHORT)nPosX;
	m_pInfo->unPosY		=(USHORT)nPosY;
	m_pInfo->unDir		=usDir;
	m_pInfo->usAction	=usAction;

	m_pInfo->usTargetPosX	=usTargetPosX;
	m_pInfo->usTargetPosY	=usTargetPosY;

	return true;
}

BOOL CMsgAction::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_ACTION != this->GetType())
		return false;

	return true;
}

void CMsgAction::Process(void *pInfo)
{

	switch(m_pInfo->usAction)
	{
	case actionChgDir:
		{
			CUser* pUser	= UserManager()->QueryUser(m_pInfo->idUser);
			if (pUser)
				pUser->SetDir(m_pInfo->unDir);
		}
		break;
	case actionEmotion:
		{
			CUser* pUser	= UserManager()->QueryUser(m_pInfo->idUser);
			if (pUser)
				pUser->SetPose(m_pInfo->dwData);
		}
		break;
	case actionDivorce:
		{
			MYASSERT(!"actionDivorce");
		}
		break;
	case actionSelfUnfreeze:
		{
			MYASSERT(!"actionSelfUnfreeze");
		}
		break;
	case actionChgMap:
		{
			MYASSERT(!"actionChgMap");
		}
		break;
	case actionFlyMap:
		{
			CUser* pUser	= UserManager()->QueryUser(m_pInfo->idUser);
			if (pUser)
			{
				OBJID idMap = m_pInfo->idTarget;
				pUser->FlyMap(idMap, m_pInfo->unPosX, m_pInfo->unPosY);
				return ;
			}

			CAgent* pAgent = NpcManager()->QueryAgent(m_pInfo->idUser);
			if(pAgent)
			{
				OBJID idMap = m_pInfo->idTarget;
				Cast<CRoleMove>(pAgent)->FlyMap(idMap, m_pInfo->unPosX, m_pInfo->unPosY);
				return ;
			}
		}
		break;
	case actionDie:
		{
			if(IsSceneID(m_pInfo->idUser))
			{
				;
			}
			else
			{
				INpc* pNpc = NpcManager()->QueryNpc(m_pInfo->idUser);
				IF_OK(pNpc)
					pNpc->SetDie();
			}
		}
		break;
	case actionQuitSyn:
		{
			MYASSERT(!"actionQuitSyn");
		}
		break;
	case actionEnterMap:
		{
			CAgent* pAgent = NpcManager()->QueryAgent(GetNpcID());
			if(pAgent)
			{
				OBJID idMap = m_pInfo->idUser;
				Cast<CRoleMove>(pAgent)->EnterMap(idMap, m_pInfo->unPosX, m_pInfo->unPosY);

				CMsgAction	msg;
				IF_OK(msg.Create(pAgent->GetID(), pAgent->GetPosX(), pAgent->GetPosY(), pAgent->GetDir(), actionGetItemSet))
					SendMsg(&msg);
			}
		}
		break;
	case actionGetItemSet:
		{
			CAgent* pAgent = NpcManager()->QueryAgent(GetNpcID());
			if(pAgent)
			{
				CMsgAction	msg;
				IF_OK(msg.Create(pAgent->GetID(), pAgent->GetPosX(), pAgent->GetPosY(), pAgent->GetDir(), actionGetGoodFriend))
					SendMsg(&msg);
			}
		}
		break;
	case actionGetGoodFriend:
		{
			CAgent* pAgent = NpcManager()->QueryAgent(GetNpcID());
			if(pAgent)
			{
				CMsgAction	msg;
				IF_OK(msg.Create(pAgent->GetID(), pAgent->GetPosX(), pAgent->GetPosY(), pAgent->GetDir(), actionGetWeaponSkillSet))
					SendMsg(&msg);
			}
		}
		break;
	case actionGetWeaponSkillSet:
		{
			CAgent* pAgent = NpcManager()->QueryAgent(GetNpcID());
			if(pAgent)
			{
				CMsgAction	msg;
				IF_OK(msg.Create(pAgent->GetID(), pAgent->GetPosX(), pAgent->GetPosY(), pAgent->GetDir(), actionGetMagicSet))
					SendMsg(&msg);
			}
		}
		break;
	case actionGetMagicSet:
		{
			CAgent* pAgent = NpcManager()->QueryAgent(GetNpcID());
			if(pAgent)
			{
				CMsgAction	msg;
				IF_OK(msg.Create(pAgent->GetID(), pAgent->GetPosX(), pAgent->GetPosY(), pAgent->GetDir(), actionGetSynAttr))
					SendMsg(&msg);
			}
		}
		break;
	case actionGetSynAttr:
		{
			CAgent* pAgent = NpcManager()->QueryAgent(GetNpcID());
			if(pAgent)
				pAgent->LoginOK();
		}
		break;
	case actionJump:
		{
			CUser* pUser	= UserManager()->QueryUser(m_pInfo->idUser);
			if (pUser)
				pUser->JumpPos(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY, m_pInfo->unDir);
		}
		break;
	case actionLeaveMap:
		{
			if(IsSceneID(m_pInfo->idUser))
			{
				;
			}
			else if(IsNpcID(m_pInfo->idUser))
			{
				NpcManager()->QuerySet()->DelObj(m_pInfo->idUser);
			}
			else
			{
				UserManager()->QuerySet()->DelObj(m_pInfo->idUser);
			}
		}
		break;
	case actionSynchro:
		{
			CUser* pUser	= UserManager()->QueryUser(m_pInfo->idUser);
			if(pUser)
			{
				CGameMap* pMap = pUser->GetMap();
				if(!pMap)
					{ LOGERROR("UserID=%u has no map at %s, %d", pUser->GetID(), __FILE__, __LINE__); return ; }
				if(!pMap->IsValidPoint(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY))
				{
					LOGERROR("UserID=%u MapID=%d (%u, %u) invalid point at %s, %d",
								pUser->GetID(), pMap->GetID(),
								m_pInfo->usTargetPosX, m_pInfo->usTargetPosY,
								__FILE__, __LINE__);
					return ;
				}

				pUser->SetPos(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY);
			}
			else if(GetNpcID() == m_pInfo->idUser)
			{
				INpc* pNpc = NpcManager()->QueryNpc(m_pInfo->idUser);
				if(pNpc)
				{
					CGameMap* pMap = pNpc->GetMap();
					if(!pMap)
					{
						LOGERROR("NPC idUser=%u has no map at %s, %d",
									m_pInfo->idUser,
									__FILE__, __LINE__);
						return ;
					}
					if(!pMap->IsValidPoint(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY))
					{
						LOGERROR("NPC idUser=%u MapID=%d (%u, %u) invalid point at %s, %d",
									m_pInfo->idUser, pMap->GetID(), m_pInfo->usTargetPosX, m_pInfo->usTargetPosY, __FILE__, __LINE__);
						return ;
					}

					pNpc->SynchroPos(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY, m_pInfo->usTargetPosX, m_pInfo->usTargetPosY);
				}
			}
		}
		break;
	case	actionKickBack:
		{
			INpc* pNpc = NpcManager()->QueryNpc(m_pInfo->idUser);
			if(pNpc)
			{
				CGameMap* pMap = pNpc->GetMap();
				if(!pMap)
				{
					LOGERROR("NPC idUser=%u has no map at %s, %d",
								m_pInfo->idUser,
								__FILE__, __LINE__);
					return ;
				}
				if(!pMap->IsValidPoint(m_pInfo->unPosX, m_pInfo->unPosY))
				{
					LOGERROR("NPC idUser=%u MapID=%d (%u, %u) invalid point at %s, %d",
								m_pInfo->idUser, pMap->GetID(), m_pInfo->unPosX, m_pInfo->unPosY, __FILE__, __LINE__);
					return ;
				}

				pNpc->KickBack(m_pInfo->unPosX, m_pInfo->unPosY);
			}
		}
		break;
	case	actionMapStatus:
		{
			OBJID idMap = m_pInfo->idUser;
			int nStatus = m_pInfo->iData;
			CGameMap* pMap = MapManager()->QueryMap(idMap);
			IF_OK(pMap)
				pMap->SetStatus(nStatus);
		}
		break;
	case	actionMoveStop:
		{
			INpc* pNpc = NpcManager()->QueryNpc(GetNpcID());
			if(pNpc && Cast<CNpc>(pNpc))
			{
				Cast<CNpc>(pNpc)->LockMove(m_pInfo->dwData);
			}
		}
		break;
	case	actionChangeMapDoc:
		{
			OBJID idMap = m_pInfo->idUser;
			OBJID idDoc = m_pInfo->dwData;
			CGameMap* pMap = MapManager()->QueryMap(idMap);
			IF_OK(pMap)
			{
				pMap->ChangeMapDoc(idDoc);
			}
		}
		break;
	case	actionAddTerrainObj:
		{
			OBJID idMap = m_pInfo->idUser;
			OBJID idOwner = m_pInfo->dwData;
			OBJID idTerrainObj = GetNpcID();
			CGameMap* pMap = MapManager()->QueryMap(idMap);
			IF_OK(pMap)
			{
				pMap->AddTerrainObj(idOwner, m_pInfo->unPosX, m_pInfo->unPosY, idTerrainObj);
			}
		}
		break;
	case	actionDelTerrainObj:
		{
			OBJID idMap = m_pInfo->idUser;
			OBJID idOwner = m_pInfo->dwData;
			CGameMap* pMap = MapManager()->QueryMap(idMap);
			IF_OK(pMap)
			{
				pMap->DelTerrainObj(idOwner);
			}
		}
		break;
	default:
		break;
	}
}
