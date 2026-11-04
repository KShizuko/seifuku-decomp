#include "AllMsg.h"
#include "GamePlayerSet.h"
#include "Hero.h"
#include "3DGameMap.h"
#include "GameMsg.h"
#include "Pet.h"
#include "NDsound.h"
#include "3DRoleData.h"

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

	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_ACTION;

	m_pInfo->dwTimeStamp	=::TimeGet();
	m_pInfo->idUser		=idPlayer;
	m_pInfo->unPosX		=(USHORT)nPosX;
	m_pInfo->unPosY		=(USHORT)nPosY;
	m_pInfo->unDir		=usDir;
	m_pInfo->dwData		=dwData;
	m_pInfo->usAction	=usAction;

	return true;
}

BOOL CMsgAction::Create(DWORD dwFirst, OBJID idUser, DWORD dwData, USHORT usAction)
{
	if (dwFirst == 0 || idUser == ID_NONE)
		return false;
	this->Init();

	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_ACTION;

	m_pInfo->dwTimeStamp	=dwFirst;
	m_pInfo->idUser		=idUser;
	m_pInfo->dwData		=dwData;
	m_pInfo->usAction	=usAction;

	return true;
}

BOOL CMsgAction::Create(OBJID idPlayer, int nPosX, int nPosY, USHORT usDir, USHORT usAction, USHORT usTargetPosX, USHORT usTargetPosY, DWORD dwTimeStamp)
{
	if (idPlayer == ID_NONE)
		return false;
	this->Init();

	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_ACTION;

	if (0 == dwTimeStamp)
		m_pInfo->dwTimeStamp	=::TimeGet();
	else
		m_pInfo->dwTimeStamp	=dwTimeStamp;
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

void CMsgAction::Process(void* pInfo)
{
#ifdef _MSGDEBUG
	::LogMsg("Process CMsgAction, idUser:%u, data:%u",
					m_pInfo->idUser,
					m_pInfo->dwData);
#endif
	if (m_pInfo->usAction == actionChangeFace)
		g_objHero.SetTeamMemberFace(m_pInfo->idUser, m_pInfo->dwData);
	if (m_pInfo->usAction == actionTeamLeaderPos)
	{
		if (!g_objHero.IsTeamLeader())
		{
			CMyPos pos = { m_pInfo->unPosX, m_pInfo->unPosY };
			g_objHero.SetTeamLeaderPos(pos);
		}
	}
	if(m_pInfo->usAction == actionQueryTeamMember)
	{
		CMyPos posTeamMember;
		posTeamMember.x = m_pInfo->unPosX;
		posTeamMember.y = m_pInfo->unPosY;
		g_objGameMap.SetFriendPos(posTeamMember);
	}
	if(m_pInfo->usAction == actionEnterMap)
	{
		m_pInfo->idUser = g_objHero.GetID();
	}
	if (m_pInfo->usAction == actionMapARGB)
	{
		if (m_pInfo->dwData == 0)
			m_pInfo->dwData = -1;
		g_objGameMap.SetARGB(m_pInfo->dwData | 0xFF000000);
	}
	if (m_pInfo->usAction == actionTableRequest)
	{
		g_objHero.m_dwTableParam[0] = m_pInfo->dwData;
		::PostCmd(CMD_NORMAL + 92, 0);
	}
	else if (m_pInfo->usAction == actionTableAct)
	{
		g_objHero.m_dwTableParam[1] = m_pInfo->unPosX;
		g_objHero.m_dwTableParam[2] = m_pInfo->unPosY;
		g_objHero.m_dwTableParam[0] = m_pInfo->dwData;
		::PostCmd(CMD_NORMAL + 92, 3);
	}
	else if (m_pInfo->usAction == actionTableClear)
	{
		g_objHero.ActTable(3, 0);
	}

	CPlayer* pPlayer	=g_objPlayerSet.GetPlayer(m_pInfo->idUser, true);
	if (!pPlayer)
	{
		switch(m_pInfo->usAction)
		{
		case actionJump:
		case actionMine:
		case actionChgDir:
		case actionEmotion:
		case actionAlignPos:
			{
				CMyPos posHero = g_objHero.GetAlignPos();
				{
					CMsgAction msg;
					if (msg.Create(g_objHero.GetID(), posHero.x, posHero.y, g_objHero.GetDir(), actionQueryPlayer, m_pInfo->idUser))
						msg.Send();
				}
			}
			break;
		default:
			break;
		}
		return;
	}

	switch(m_pInfo->usAction)
	{
	case actionPostCmd:
		::PostCmd(m_pInfo->dwData);
		break;
	case actionSynchro:
		{
			if (pPlayer->GetID() != g_objHero.GetID())
			{
				pPlayer->ResetStepDir();
				CMyPos posPlayer;
				pPlayer->GetPos(posPlayer);
				if (posPlayer.x != m_pInfo->usTargetPosX
						|| posPlayer.y != m_pInfo->usTargetPosY)
				{
					pPlayer->SetActionCmd(_ACTION_STANDBY);
					pPlayer->SetPos(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY);
				}
			}
			else
			{
				g_objHero.ResetActionData();
				CMyPos posHero;
				g_objHero.GetPos(posHero);
				if (posHero.x != m_pInfo->usTargetPosX
						|| posHero.y != m_pInfo->usTargetPosY)
				{
					m_pInfo->usTargetPosX = posHero.x;
					m_pInfo->usTargetPosY = posHero.y;
					this->Send();
				}
			}
		}
		break;
	case actionMine:
		{
			pPlayer->ResetStepDir();
			pPlayer->SetActionCmd(_ACTION_MINE);
			int nChance = m_pInfo->dwData/30;
			if (::RandGet(5) < nChance)
			{
				int nPosX, nPosY;
				pPlayer->GetPos(nPosX, nPosY);
				const int _DELTA_X[8]={0,-1,-1,-1,0,1,1,1};
				const int _DELTA_Y[8]={1,1,0,-1,-1,-1,0,1};
				nPosX += _DELTA_X[pPlayer->GetDir()];
				nPosY += _DELTA_Y[pPlayer->GetDir()];
				CMyPos posWorld;
				g_objGameMap.Cell2World(nPosX, nPosY, posWorld.x, posWorld.y);
				g_objGameMap.Add3DMapEffect(posWorld, "Mine");
			}
		}
		break;
	case actionChgDir:
		{
			pPlayer->SetDir(m_pInfo->unDir);
		}
		break;
	case actionEmotion:
		{
			struct {
				WORD	wEmotion;
				BYTE	btProf;
				BYTE	btFlagAllNosuch;
			} infoEmotion;
			memcpy(&infoEmotion, &m_pInfo->dwData, sizeof(DWORD));
			if (g_objHero.GetID() != m_pInfo->idUser)
			{
				CCommand cmdDemo;
				cmdDemo.iType		=_COMMAND_EMOTION;
				cmdDemo.iStatus		=_CMDSTATUS_BEGIN;
				cmdDemo.nData		=infoEmotion.wEmotion;
				cmdDemo.idTarget	=ID_NONE;
				cmdDemo.nDir		=m_pInfo->unDir;
				pPlayer->SetCommand(&cmdDemo);
				if (pPlayer->TestStatus(USERSTATUS_PRAY) && g_objHero.TestStatus(USERSTATUS_PRAYFOLLOW)
						&& g_objHero.GetDistance(pPlayer) <= 3
						&& (g_objHero.GetCommandType() == _COMMAND_STANDBY || g_objHero.GetCommandType() == _COMMAND_EMOTION))
					g_objHero.Emotion(infoEmotion.wEmotion, NULL, true);
			}
			if (infoEmotion.btProf)
				pPlayer->AddProfessionalCoolposEffect(infoEmotion.btProf/10, infoEmotion.btFlagAllNosuch);
		}
		break;
	case actionJump:
		{
			if (g_objHero.GetID() == m_pInfo->idUser)
			{
				g_objHero.SetAlignPos(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY);
			}
			else if(pPlayer->IsMyPet())
			{
				CPet* pPet = (CPet*)pPlayer;
				pPet->SetAlignPos(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY);
			}
			else
			{
				pPlayer->ResetStepDir();
				CCommand cmd;
				cmd.iType			=_COMMAND_JUMP;
				cmd.iStatus			=_CMDSTATUS_BEGIN;
				cmd.posTarget.x		=m_pInfo->usTargetPosX;
				cmd.posTarget.y		=m_pInfo->usTargetPosY;
				pPlayer->SetCommand(&cmd);
			}
		}
		break;
	case actionEnterMap:
		{
			extern unsigned int	g_uStatus;
			g_uStatus	=_STATUS_NORMAL;
			{
				MYASSERT (g_objGameMap.Create(m_pInfo->dwData));
				int nPosX	=m_pInfo->unPosX;
				int nPosY	=m_pInfo->unPosY;
				g_objHero.SetPos(nPosX, nPosY);
				g_objHero.SetAlignPos(nPosX, nPosY);
				g_objHero.SetDir(m_pInfo->unDir);
				CCommand cmd;
				cmd.iType	=_COMMAND_STANDBY;
				cmd.iStatus	=_CMDSTATUS_BEGIN;
				g_objHero.SetCommand(&cmd);
				CMsgAction msg;
				MYASSERT (msg.Create(g_objHero.GetID(), 0, 0, 0, actionGetItemSet));
				msg.Send();
				g_objHero.m_tmPing.Startup(10000);
			}
		}
		break;
	case actionFlyMap:
		{
			MYASSERT (m_pInfo->idUser == g_objHero.GetID());
			{
				extern UINT	g_uStatus;
				g_uStatus = _STATUS_WAITING;
				MYASSERT (g_objGameMap.Create(m_pInfo->dwData));
				g_objHero.SetPos(m_pInfo->unPosX, m_pInfo->unPosY);
				g_objHero.SetAlignPos(m_pInfo->unPosX, m_pInfo->unPosY);
				g_objHero.SetDir(m_pInfo->unDir);
				int nAmount = g_objPlayerSet.GetPlayerAmount();
				for(int i = 0; i < nAmount; i ++)
				{
					CPlayer* pPlayer = g_objPlayerSet.GetPlayerByIndex(i);
					if(pPlayer && pPlayer->IsMyPet())
					{
						pPlayer->ResetActionData();
						pPlayer->ResetStepDir();
						pPlayer->SetPos(m_pInfo->unPosX, m_pInfo->unPosY);
						CPet* pPet = (CPet*)pPlayer;
						pPet->SetAlignPos(m_pInfo->unPosX, m_pInfo->unPosY);
					}
				}
				g_objHero.ResetActionData();
				CMyPos posWorld;
				g_objHero.GetWorldPos(posWorld);
				posWorld.x	-=_SCR_WIDTH/2;
				posWorld.y	-=_SCR_HEIGHT/2;
				g_objGameMap.SetViewPos(posWorld);
				CCommand cmd;
				cmd.iType	=_COMMAND_STANDBY;
				cmd.iStatus	=_CMDSTATUS_BEGIN;
				g_objHero.SetCommand(&cmd);
				g_uStatus = _STATUS_NORMAL;
			}
		}
		break;
	case actionLeaveMap:
		{
			CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_pInfo->idUser);
			if (pPlayer)
			{
				if (pPlayer->GetID() <= 999999 && pPlayer->IsDead() && pPlayer->IsNpc())
				{
					CMyPos posWorld;
					pPlayer->GetWorldPos(posWorld);
					g_objGameMap.Add3DMapEffect(posWorld, "BodyDisapear");
				}
				BOOL bDelObj	=true;
				g_objPlayerSet.DelPlayer(m_pInfo->idUser, bDelObj);
			}
		}
		break;
	case actionGetItemSet:
		{
			CMsgAction msg;
			MYASSERT (msg.Create(g_objHero.GetID(), 0, 0, 0, actionGetGoodFriend));
			msg.Send();
			PostCmd(CMD_SETPKMODE);
		}
		break;
	case actionGetGoodFriend:
		{
			CMsgAction msg;
			MYASSERT (msg.Create(g_objHero.GetID(), 0, 0, 0, actionGetWeaponSkillSet));
			msg.Send();
		}
		break;
	case actionGetWeaponSkillSet:
		{
			CMsgAction msg;
			MYASSERT (msg.Create(g_objHero.GetID(), 0, 0, 0, actionGetMagicSet));
			msg.Send();
		}
		break;
	case actionGetMagicSet:
		{
			CMsgAction msg;
			MYASSERT (msg.Create(g_objHero.GetID(), 0, 0, 0, actionGetSynAttr));
			msg.Send();
		}
		break;
	case actionGetSynAttr:
		{
			CMsgAction msg;
			MYASSERT (msg.Create(g_objHero.GetID(), 0, 0, 0, actionGetStatistic));
			msg.Send();
		}
		break;
	case actionChgMap:
		{
		}
		break;
	case actionEquip:
		{
			pPlayer->SetRWeapon(m_pInfo->dwData);
		}
		break;
	case actionUnequip:
		{
			pPlayer->SetRWeapon(ID_NONE);
		}
		break;
	case actionUplev:
		{
			pPlayer->DoLevelUp();
		}
		break;
	case actionSetPkMode:
		{
			g_objHero.SetPkMode(m_pInfo->iData, false);
		}
		break;
	case actionChangeFace:
		{
			pPlayer->SetFace(m_pInfo->dwData);
			if (g_objHero.GetID() == m_pInfo->idUser)
				::PostCmd(CMD_FLASH_HERO_FACE);
		}
		break;
	case actionAbortMagic:
		{
			if(pPlayer->GetID() == g_objHero.GetID())
			{
				g_objHero.EndMagicBusy();
				g_objHero.StopChargeUp();
			}
		}
		break;
	case actionCreateBooth:
		{
			if(pPlayer->GetID() == g_objHero.GetID())
				g_objHero.m_objBoothManager.Open(m_pInfo->idTarget);
		}
		break;
	case actionResumeBooth:
		{
			if(pPlayer->GetID() == g_objHero.GetID())
				g_objHero.m_objBoothManager.Open(m_pInfo->idTarget);
		}
		break;
	case actionSetCheckKey:
		{
			((CCheckKeys*)&g_objHero)->SetKey(m_pInfo->idUser, m_pInfo->dwData);
		}
		break;
	case actionGetMoney:
		{
			if(pPlayer)
				pPlayer->AddMoneyEffect(m_pInfo->dwData);
		}
		break;
	case actionKickBack:
		{
			if(pPlayer->GetID() == g_objHero.GetID())
			{
				CMyPos posCell;
				g_objHero.GetPos(posCell);
				posCell.x = m_pInfo->unPosX;
				posCell.y = m_pInfo->unPosY;
				g_objHero.ResetActionData();
				g_objHero.ResetStepDir();
				g_objHero.SetPos(posCell);
				g_objHero.SetAlignPos(posCell.x, posCell.y);
				CMsgAction msg;
				if(msg.Create(g_objHero.GetID(), 0, 0, g_objHero.GetDir(), actionSynchro, posCell.x, posCell.y))
					msg.Send();
			}
		}
		break;
	case actionOpenDialog:
		{
			switch(m_pInfo->dwData)
			{
				case 1:
					{
						::PostCmd(CMD_NORMAL + 86);
					}
					break;
				case 2:
					{
						::PostCmd(CMD_NORMAL + 88);
					}
					break;
				case 3:
					{
						::PostCmd(CMD_NORMAL + 89);
					}
					break;
				case 4:
					{
						::PostCmd(CMD_NORMAL + 90);
						g_objHero.SetLastDialogPara(m_pInfo->dwData);
					}
					break;
				case 44:
					{
						g_objHero.ActTable(3, 0);
					}
					break;
			}
		}
		break;
	case actionSoundEffect:
		{
			if (GameDataSetQuery()->GetSound(m_pInfo->dwData))
				::DXPlaySound((char*)GameDataSetQuery()->GetSound(m_pInfo->dwData));
		}
		break;
	case actionTeamMemeberPos:
		{
			DWORD dwInterval = 0;
			switch (g_objHero.GetID() % 10)
			{
			case 2:
				dwInterval = 2000;
				break;
			case 5:
				dwInterval = 5000;
				break;
			case 8:
				dwInterval = 8000;
				break;
			}
			g_objHero.SetTeamMemberPosTimer(dwInterval, m_pInfo->dwData);
		}
		break;
	case actionDropMagic:
		{
			if (g_objHero.GetID() == m_pInfo->idUser)
				g_objHero.DelMagic(m_pInfo->dwData);
		}
		break;
	case actionDropSkill:
		{
			if (g_objHero.GetID() == m_pInfo->idUser)
				g_objHero.DelWeaponSkill(m_pInfo->dwData);
		}
		break;
	case actionCheckMotion:
		{
			I3DMotion* p3DMotion = GameDataSetQuery()->Get3DMotion("body", m_pInfo->dwData);
			if (p3DMotion)
			{
				DWORD dwKeyFrames = p3DMotion->GetKeyFrameAmount(0);
				D3DXMATRIX matrix;
				memcpy(&matrix, p3DMotion->GetKeyFrameMatrix(0, dwKeyFrames / 2), sizeof(matrix));
				m_pInfo->dwData = (int)matrix.m[3][m_pInfo->unDir % 4];
				this->Send();
			}
		}
		break;
	case actionAlignPos:
		{
			if (g_objHero.GetID() == m_pInfo->idUser)
			{
				g_objHero.SetAlignPos(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY);
			}
			else if(pPlayer->IsMyPet())
			{
				CPet* pPet = (CPet*)pPlayer;
				pPet->SetAlignPos(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY);
			}
			else
			{
				pPlayer->ResetStepDir();
				CMyPos posCell = { m_pInfo->usTargetPosX, m_pInfo->usTargetPosY };
				pPlayer->SetPos(posCell);
			}
		}
		break;
	case actionDisappear:
		{
			if (g_objHero.GetID() != m_pInfo->idUser)
			{
				const CMonsterInfo* pInfo = RoleDataQuery()->GetMonsterInfo(pPlayer->GetName());
				if (pInfo)
				{
					pPlayer->SetActionCmd(_ACTION_DISAPPEAR);
					if (pPlayer->m_objEffect)
						pPlayer->m_objEffect->Add((char*)pInfo->strBornEffect.c_str());
					CMyPos posCell, posWorld;
					pPlayer->GetPos(posCell);
					g_objGameMap.Cell2World(posCell.x, posCell.y, posWorld.x, posWorld.y);
					::DXPlaySound((char*)pInfo->strBornSound.c_str(), posWorld.x, posWorld.y, 2000, 0, 100);
				}
			}
		}
		break;
	case actionTaskKillCount:
		{
			if (m_pInfo->dwData != 0)
			{
				if (!g_objHero.IsTaskKillCount())
					g_objHero.BeginTaskKillCount();
			}
			else
			{
				if (g_objHero.IsTaskKillCount())
					g_objHero.EndTaskKillCount();
			}
		}
		break;
	case actionTaskKillCountAdd:
		{
			if (g_objHero.IsTaskKillCount())
				g_objHero.SetTaskKillCount(m_pInfo->dwData + g_objHero.GetTaskKillCount());
		}
		break;
	}
}
