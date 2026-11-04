#include "AllMsg.h"
#include "mapgroup.h"
#include "transformation.h"
#include "Agent.h"
#include "User.h"
#include "TradeBuddy.h"
static void SetWatchUser(CUser* pUser);

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
	DEBUG_CREATEMSG("ACTION",idPlayer,usAction,"",usDir,0);

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
	DEBUG_CREATEMSG("ACTION",idPlayer,usAction,"",usDir,0);

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
#ifdef _MSGDEBUG
	::LogMsg("Process CMsgAction, idUser:%u, data:%u",
					m_pInfo->idUser,
					m_pInfo->dwData);
#endif

	IRole* pRole	= RoleManager()->QueryRole(this, m_pInfo->idUser);
	if (!pRole)
	{
		if(IsNpcMsg())
			return;

		switch(m_pInfo->usAction)
		{
		case actionQueryFriendInfo:
			{
				CUser* pTarget = UserManager()->GetUser(m_pInfo->idTarget);
				if(pTarget)
				{
					CMsgFriendInfo	msg;
					IF_OK(msg.Create(_FRIEND_INFO, pTarget->GetID(), pTarget->GetLookFace(), pTarget->GetLev(), pTarget->GetProfession(), pTarget->GetPk(), pTarget->GetSynID(), pTarget->GetSynRankShow(), pTarget->GetMate()))
						SendMsg(&msg);
				}
			}
			break;
		case actionQueryEnemyInfo:
			{
				CUser* pTarget = UserManager()->GetUser(m_pInfo->idTarget);
				if(pTarget)
				{
					CMsgFriendInfo	msg;
					IF_OK(msg.Create(_ENEMY_INFO, pTarget->GetID(), pTarget->GetLookFace(), pTarget->GetLev(), pTarget->GetProfession(), pTarget->GetPk(), pTarget->GetSynID(), pTarget->GetSynRankShow(), pTarget->GetMate()))
						SendMsg(&msg);
				}
			}
			break;
		}
		return;
	}

	CUser* pUser = NULL;
	pRole->QueryObj(OBJ_USER, IPP_OF(pUser));

	if(m_pInfo->usAction != actionXpCLear)
		m_pInfo->idUser	= pRole->GetID();

	switch(m_pInfo->usAction)
	{
	case actionQueryFriendInfo:
	case actionQueryEnemyInfo:
	case actionQueryLeaveWord:
	case actionQueryPlayer:
	case actionXpCLear:
	case actionQueryCryOut:
	case actionQueryTeamMember:
	case actionQueryEquipment:
	case actionVerifyCode:
	case actionCheatFound:
	case actionDetectMsg:
		break;

	default:
		pRole->ClrAttackTarget();
		break;
	}

	DEBUG_TRY
	switch(m_pInfo->usAction)
	{
	case actionQueryTeamMember:
		{
			if (pUser)
			{
				CTeam* pTeam = pUser->GetTeam();
				if (pTeam && pTeam->IsTeamMember(m_pInfo->idTarget))
				{
					CUser* pTarget = UserManager()->GetUser(m_pInfo->idTarget);
					if (pTarget && pTarget->GetMapID() == pUser->GetMapID())
					{
						CMsgAction msg;
						IF_OK (msg.Create(m_pInfo->idUser, pTarget->GetPosX(), pTarget->GetPosY(), 0, actionQueryTeamMember, m_pInfo->idTarget))
							pUser->SendMsg(&msg);
					}
				}
			}
		}
		break;

	case actionQueryPlayer:
		{
			if (pUser)
			{
				if (pUser->GetMap()->IsNewbieMap())
					break;

				IRole* pRole = pUser->FindAroundRole(m_pInfo->idTarget);
				if (pRole && pRole->IsAlive())
				{
					if (pRole->GetDistance(pUser->GetPosX(), pUser->GetPosY()) <= CELLS_PER_VIEW)
					{
						pRole->QueryMapThing()->SendShow(pUser->QueryRole());
					}
				}
				if(pUser->IsGM())
				{
					pUser->SendSysMsg("actionQueryPlayer: [%d]", m_pInfo->idTarget);
					if(pRole)
						pUser->SendSysMsg("target: [%s][%d], pos: [%d][%d]", pRole->GetName(), pRole->GetID(), pRole->GetPosX(), pRole->GetPosY());
#ifdef	PALED_DEBUG
					LOGERROR("actionQueryPlayer: [%d]", m_pInfo->idTarget);
					if(pRole)
					{
						LOGERROR("target: [%s][%d], pos: [%d][%d]", pRole->GetName(), pRole->GetID(), pRole->GetPosX(), pRole->GetPosY());
					}
#endif
				}
			}
		}
		break;

	case actionMine:
		{
			if (pUser)
			{
				if (!pUser->IsAlive())
				{
					pUser->SendSysMsg(STR_DIE);
					return;
				}

				if (!pUser->GetMap()->IsMineField())
				{
					pUser->SendSysMsg(STR_NO_MINE);
					return;
				}

				pUser->Mine();
			}
		}
		break;

	case actionChangeFace:
		{
			if (pUser)
			{
				if (!pUser->SpendMoney(500, true))
				{
					pUser->SendSysMsg(STR_NOT_SO_MUCH_MONEY);
					return;
				}

				bool bFaceOK = true;
				if (strcmp(CStringManager::Instance()->GetStr("Language"), "English") == 0)
					bFaceOK = (m_pInfo->dwData <= 400);
				if (!bFaceOK)
					break;

				pUser->SetFace(m_pInfo->dwData);

				pUser->BroadcastRoomMsg(this, true);

				CTeam* pTeam = pUser->GetTeam();
				if (pTeam)
					pTeam->BroadcastTeamMsg(this, pUser);
			}
		}
		break;
	case actionChgDir:
		{
			pRole->SetDir(m_pInfo->unDir);
			pRole->BroadcastRoomMsg(this, EXCLUDE_SELF);
		}
		break;
	case actionPosition:
		{
		}
		break;
	case actionEmotion:
		{
			DWORD dwEmotion = m_pInfo->dwData;
			if (!((dwEmotion >= 1 && dwEmotion <= 8)
					|| dwEmotion == _ACTION_LAUGH || dwEmotion == _ACTION_FURY || dwEmotion == _ACTION_SAD
					|| dwEmotion == _ACTION_SAYHELLO || dwEmotion == _ACTION_SALUTE || dwEmotion == _ACTION_GENUFLECT
					|| dwEmotion == _ACTION_KNEEL || dwEmotion == _ACTION_COOL || dwEmotion == _ACTION_SITDOWN
					|| dwEmotion == _ACTION_FAINT || dwEmotion == _ACTION_LIE))
				break;

			if (pUser)
			{
				m_pInfo->dwData	&= 0xFFFF;
				pRole->SetPose(m_pInfo->dwData);

				if (_ACTION_COOL == m_pInfo->dwData)
				{
					if (::TimeGet() > pUser->m_dwLastCoolShow + 3000)
					{
						if (pUser->IsAllNonsuchEquip())
							m_pInfo->dwData	|= (pUser->GetProfession()*0x00010000 + 0x01000000);
						else if ((pUser->GetArmorTypeID()%10) == 9)
							m_pInfo->dwData |= pUser->GetProfession()*0x010000;

						pUser->m_dwLastCoolShow = ::TimeGet();
					}
				}
#if	defined(PALED_DEBUG)
				if(m_pInfo->dwData >= 1 && m_pInfo->dwData <= 10)
					pUser->GetMap()->BroadcastDance(pUser, m_pInfo->dwData);
				else
#endif
				pRole->BroadcastRoomMsg(this, INCLUDE_SELF);
			}
		}
		break;
	case actionBroadcastPos:
		{
		}
		break;
	case actionDivorce:
		{
			ASSERT(!"仅下传此消息");
		}
		break;
	case actionChgMap:
		{
			if(!pUser)
				break;
			if (!pUser->IsAlive())
			{
					pUser->SendSysMsg(STR_DIE);
					return;
			}

			pRole->ProcessOnMove(MOVEMODE_CHGMAP);
			pUser->ChangeMap();
		}
		break;
	case actionFlyMap:
		{
		}
		break;
	case actionChgWeather:
		{
			ASSERT(!"仅下传此消息");
		}
		break;
	case actionFireworks:
		{
			pRole->BroadcastRoomMsg(this, EXCLUDE_SELF);
		}
		break;
	case actionDie:
		{
			CMonster* pMonster;
			if(pRole->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
			{
				if(!pMonster->IsDeleted())
				{
					pMonster->DelMonster();
				}
			}
		}
		break;
	case actionQuitSyn:
		{
		}
		break;
	case actionWalk:
	case actionRun:
		{
		}
		break;
	case actionEnterMap:
		{
			if(pUser && pUser->CheckActionCount(1))
			{
				OBJID idMapDoc = ID_NONE;
				CGameMap* pMap = MapManager()->QueryMap(pUser->GetMapID());
				IF_OK (pMap)
				{
					idMapDoc = pMap->GetDocID();

					CMsgAction	msg;
					if(msg.Create(pUser->GetMapID(), pUser->GetPosX(), pUser->GetPosY(), pUser->GetDir(), actionEnterMap, idMapDoc))
						SendMsg(&msg);

					pUser->EnterMap();
					if(!pUser->IsAgent())
						MapGroup(PID)->QueryIntraMsg()->QueryFee(pUser->GetAccountID());
					pUser->IncActionCount();
				}
				else
				{
					LOGERROR("Error: invalid map id[%u] of user:%u", pUser->GetMapID(), pUser->GetID());
				}
			}
		}
		break;
	case actionGetItemSet:
		{
			if(!pUser || !pUser->CheckActionCount(2))
				break;
			pUser->SendItemSet();

			pUser->SendBonusInfo();

			CMsgAction	msg;
			if(msg.Create(pUser->GetID(), 0, 0, 0, actionGetItemSet, 0, 0))
				SendMsg(&msg);
			pUser->IncActionCount();
		}
		break;
	case actionGetGoodFriend:
		{
			if(!pUser || !pUser->CheckActionCount(3))
				break;
			pUser->IncActionCount();
			CMsgAction	msg;
			if(msg.Create(pUser->GetID(), 0, 0, 0, actionGetGoodFriend, 0, 0))
				SendMsg(&msg);

			pUser->SendGoodFriend();
			pUser->SendTradeBuddyInfo();
			pUser->QueryEnemy()->SendToClient();

			LeaveWord()->ShowWords(pUser);
		}
		break;
	case actionGetWeaponSkillSet:
		{
			if(!pUser || !pUser->CheckActionCount(4))
				break;
			CMsgAction	msg;
			if(msg.Create(pUser->GetID(), 0, 0, 0, actionGetWeaponSkillSet, 0, 0))
				SendMsg(&msg);

			pUser->SendAllWeaponSkillInfo();
			pUser->IncActionCount();
		}
		break;
	case actionGetMagicSet:
		{
			if(!pUser || !pUser->CheckActionCount(5))
				break;
			CMsgAction	msg;
			if(msg.Create(pUser->GetID(), 0, 0, 0, actionGetMagicSet, 0, 0))
				SendMsg(&msg);

			pUser->SendAllMagicInfo();
			pUser->QuerySupermanSecs();
			pUser->IncActionCount();
		}
		break;
	case actionGetSynAttr:
		{
			if(!pUser || !pUser->CheckActionCount(6))
				break;
			CMsgAction	msg;
			if(msg.Create(pUser->GetID(), 0, 0, 0, actionGetSynAttr, 0, 0))
				SendMsg(&msg);

			pUser->QuerySynAttr()->SendInfoToClient();

			CSynPtr pSyn = SynManager()->QuerySyndicate(pUser->GetSynID());
			if(pSyn)
			{
				pSyn->SendInfoToClient(pUser);
			}

			if(pSyn && pSyn->GetInt(SYNDATA_MONEY) <= 0)
				pUser->SendSysMsg(_TXTATR_GM, STR_DESTROY_SYNDICATE_SOON);

			OBJID idSyn = _SynManager(PID)->GetMapSynID(WHITE_SYN_MAP_ID);
			if(idSyn != ID_NONE)
			{
				CMsgSyndicate	msg;
				IF_OK(msg.Create(SET_WHITE_SYN, idSyn))
					pUser->SendMsg(&msg);
			}
			 idSyn = _SynManager(PID)->GetMapSynID(BLACK_SYN_MAP_ID);
			if(idSyn != ID_NONE)
			{
				CMsgSyndicate	msg;
				IF_OK(msg.Create(SET_BLACK_SYN, idSyn))
					pUser->SendMsg(&msg);
			}
			pUser->IncActionCount();

			pUser->AddAttrib(29, 0, SYNCHRO_TRUE, NULL);
		}
		break;
	case actionJump:
		{
			if(!this->IsNpcMsg())
			{
				OBJID idUser = m_pInfo->idUser;
				if(!(idUser >= 700001 && idUser <= 799999) && !(idUser >= 1000000 && idUser <= 3999999999u))
				{
					IRole* pRole	= RoleManager()->QueryRole(this, idUser);
					CUser* pUser	= NULL;
					if(pRole && pRole->QueryObj(OBJ_USER, IPP_OF(pUser)))
					{
						pUser->SetCheatMark(7, 3);
						pUser->SetCheatKickout(RandGet(300) + 30);
					}
					break;
				}
			}

			if (pUser)
			{
				if (!pUser->CheckJumpTime(m_pInfo->dwTimeStamp))
				{
					pUser->SendSysMsg("invalid jump");
					UserManager()->KickOutSocket(m_idSocket, "invalid jump time stamp");
					return;
				}

				if (!pUser->IsAlive())
				{
					pUser->SendSysMsg(STR_DIE);
					return;
				}

				if (pUser->GetDistance(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY) > CELLS_PER_BLOCK)
				{
					pUser->SendSysMsg(STR_INVALID_MSG);
					UserManager()->KickOutSocket(m_idSocket, "Jump too far");
					return;
				}
				if (pUser->QueryTransformation() && !pUser->QueryTransformation()->IsJumpEnable())
				{
					UserManager()->KickOutSocket(m_idSocket, "Cannot jump!");
					return;
				}
			}

			{
				pRole->BroadcastRoomMsg(this, INCLUDE_SELF);

				if(pUser && !pUser->IsJumpPass(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY, pUser->IsWing() ? FLY_ALT : JUMP_ALT))
					pUser->KickBack();

				if(pRole->GetDistance(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY) > CELLS_PER_BLOCK)
				{
					if(pUser)
						ASSERT(!pUser);
					pRole->ProcessOnMove(MOVEMODE_TRANS);
					pRole->TransPos(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY);
				}
				else
				{
					pRole->ProcessOnMove(MOVEMODE_JUMP);
					pRole->JumpPos(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY);
				}
			}
		}
		break;
	case actionSynchro:
		{
			if (pUser)
			{
				if(pUser->IsSynchro())
				{
					UserManager()->KickOutSocket(pUser->GetSocketID(), "★Receive synchro msg when no flag!");
				}
				pUser->SetSynchro(true);
			}

			if (pRole->GetPosX() != m_pInfo->usTargetPosX || pRole->GetPosY() != m_pInfo->usTargetPosY)
			{
				pRole->BroadcastRoomMsg(this, EXCLUDE_SELF);

				if(pRole->GetDistance(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY) >= CELLS_PER_BLOCK)
				{
					pRole->ProcessOnMove(MOVEMODE_SYNCHRO);
					pRole->TransPos(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY);
				}
				else
				{
					pRole->ProcessOnMove(MOVEMODE_SYNCHRO);
					pRole->JumpPos(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY);
				}
			}
		}
		break;
	case actionXpCLear:
		{
			if(!pUser)
				break;

			if(m_pInfo->idUser == 809834)
			{
				OBJID	idCheatKind = m_pInfo->unDir;
				DWORD	dwCheatMark	= m_pInfo->dwData;
				pUser->SetCheatMark(idCheatKind, dwCheatMark);

				::MyLogSave("gmlog/cheater", "client cheater found: name=%s, type=%u", pUser->GetName(), m_pInfo->unDir);
				break;
			}

			pUser->ClsXpVal(false);
		}
		break;

	case actionReborn:
		{
			if(pUser && !pUser->IsAlive())
			{
				bool bChgPos = true;
				if (pUser->IsTimeCardOn() && m_pInfo->dwData == 1)
					bChgPos = false;
				pUser->Reborn(bChgPos, true);
			}
		}
		break;
	case actionGhost:
		{
			if(pUser && !pUser->IsAlive())
			{
				pUser->TransformGhost();
			}
		}
		break;

	case actionDelRole:
		{
			if (pUser && m_pInfo->dwData == pUser->GetAcceptKey())
			{
				pUser->SaveInfo();
				if(pUser->GetSynID())
					pUser->QuerySynAttr()->LeaveSyn(false, true);

				if (pUser->IsMarried())
				{
					pUser->UpdateMate(NOMATE_NAME, false);
					CUser* pMate = UserManager()->GetUser(pUser->GetMate());
					if (pMate)
					{
						pMate->UpdateMate(NOMATE_NAME, true);
					}
					else
					{
						SQLBUF szSQL = "";
						sprintf(szSQL, "UPDATE cq_user SET mate='%s' WHERE name='%s' LIMIT 1",
								NOMATE_NAME,
								pUser->GetMate());

						Database()->ExecuteSQL(szSQL);
					}
				}

				for (int i = 0; i < pUser->GetTradeBuddyAmount(); i++)
				{
					CTradeBuddy* pBuddy = pUser->GetTradeBuddyByIndex(i);
					if (!pBuddy)
						continue;
					CUser* pTarget = UserManager()->GetUser(pBuddy->GetBuddyID());
					if (pTarget)
					{
						pTarget->DelTradeBuddy(pUser->GetID());
						pTarget->SendSysMsg(STR_TRADE_BUDDY_DEL_ROLE, pUser->GetName());
					}
					else
					{
						char szWords[256];
						_snprintf(szWords, 256, STR_TRADE_BUDDY_DEL_ROLE, pUser->GetName());
						szWords[255] = 0;
						LeaveWord()->InsertWords(pUser->GetName(), pBuddy->GetName(), szWords);
					}
				}

				SQLBUF szSQL;

				_snprintf(szSQL, 1024, "delete from cq_business where userid=%u or business=%u", pUser->GetID(), pUser->GetID());
				szSQL[1023] = 0;
				Database()->ExecuteSQL(szSQL);

				_snprintf(szSQL, 1024, "INSERT INTO cq_deluser SELECT * FROM cq_user WHERE id=%u", pUser->GetID());
				if (Database()->ExecuteSQL(szSQL))
				{
					_snprintf(szSQL, 1024, "DELETE FROM cq_user WHERE id=%u", pUser->GetID());
					if (!Database()->ExecuteSQL(szSQL))
						LOGERROR("Del UID=%u from cq_user failed at %s, %d", pUser->GetID(), __FILE__, __LINE__);
				}
				else
				{
					LOGERROR("Move UID=%u to cq_deluser failed at %s, %d", pUser->GetID(), __FILE__, __LINE__);
				}
				UserManager()->KickOutSocket(m_idSocket, "Player deleted character");
			}
		}
		break;

	case actionSetPkMode:
		{
			if(pUser)
			{
				int	nMode = m_pInfo->dwData;
				const char* pMsg = NULL;
				switch(nMode)
				{
				case	PKMODE_FREE:
					pMsg = STR_FREE_PK_MODE;
					break;
				case	PKMODE_SAFE:
					pMsg = STR_SAFE_PK_MODE;
					break;
				case	PKMODE_TEAM:
					pMsg = STR_TEAM_PK_MODE;
					break;
				case	PKMODE_ARRESTMENT:
					pMsg = STR_ARRESTMENT_PK_MODE;
					break;
				default:
					ASSERT(!"switch pk mode");
					return ;
				}

				pUser->SetPkMode(nMode);
				pUser->SendMsg(this);
				pUser->SendSysMsg(pMsg);
			}
		}
		break;

	case actionQueryFriendInfo:
		{
			if(pUser)
			{
				CFriend* pFriend = pUser->GetFriend(m_pInfo->idTarget);
				if(pFriend)
				{
					CUser* pTarget = UserManager()->GetUser(m_pInfo->idTarget);
					if(pTarget)
					{
						CMsgFriendInfo	msg;
						IF_OK(msg.Create(_FRIEND_INFO, pTarget->GetID(), pTarget->GetLookFace(), pTarget->GetLev(), pTarget->GetProfession(), pTarget->GetPk(), pTarget->GetSynID(), pTarget->GetSynRankShow(), pTarget->GetMate()))
							pUser->SendMsg(&msg);
					}
					else
						MapGroup(m_idProcess)->QueryIntraMsg()->TransmitMsg(this, GetSocketID(), GetNpcID());
				}
			}
		}
		break;
	case actionQueryEnemyInfo:
		{
			if(pUser)
			{
				if(pUser->QueryEnemy()->GetName(m_pInfo->idTarget))
				{
					CUser* pTarget = UserManager()->GetUser(m_pInfo->idTarget);
					if(pTarget)
					{
						CMsgFriendInfo	msg;
						IF_OK(msg.Create(_ENEMY_INFO, pTarget->GetID(), pTarget->GetLookFace(), pTarget->GetLev(), pTarget->GetProfession(), pTarget->GetPk(), pTarget->GetSynID(), pTarget->GetSynRankShow(), pTarget->GetMate()))
							pUser->SendMsg(&msg);
					}
					else
						MapGroup(m_idProcess)->QueryIntraMsg()->TransmitMsg(this, GetSocketID(), GetNpcID());
				}
			}
		}
		break;
	case actionQueryLeaveWord:
		{
			if(pUser)
			{
				if(pUser->IsMoreLeaveWord())
				{
					LeaveWord()->ShowWords(pUser);
				}
			}
		}
		break;
	case actionCreateBooth:
		{
			if(pUser)
			{
				if(pUser->CreateBooth(m_pInfo->unPosX, m_pInfo->unPosY, m_pInfo->unDir))
				{
					m_pInfo->dwData = pUser->QueryBooth()->GetID();
					pUser->SendMsg(this);
				}
			}
		}
		break;
	case actionSuspendBooth:
		{
			if(pUser && pUser->QueryBooth())
				pUser->QueryBooth()->LeaveMap();
		}
		break;
	case actionResumeBooth:
		{
			if(pUser && pUser->QueryBooth())
			{
				pUser->QueryBooth()->EnterMap(m_pInfo->unPosX, m_pInfo->unPosY, m_pInfo->unDir);
				m_pInfo->dwData = pUser->QueryBooth()->GetID();
				pUser->SendMsg(this);
			}
		}
		break;
	case actionDestroyBooth:
		{
			if(pUser)
				pUser->DestroyBooth();
		}
		break;
	case actionQueryCryOut:
		{
			CUser* pTarget = UserManager()->GetUser(m_pInfo->idTarget);
			if(!pTarget)
				return ;

			if(pUser && pTarget->QueryBooth())
				pTarget->QueryBooth()->SendCryOut(pUser);
		}
		break;
	case actionQueryEquipment:
		{
			CUser* pTarget = UserManager()->GetUser(m_pInfo->idTarget);
			if(!pTarget)
				return ;

			pTarget->SendAllEquipInfoTo(pUser);
			if(pUser)
				pTarget->SendSysMsg(STR_QUERYEQUIPMENT_HINT, pUser->GetName());
		}
		break;
	case actionAbortTransform:
		{
			if(pUser && pUser->QueryTransformation())
				pUser->AbortTransform();
		}
		break;
	case actionTakeOff:
		{
			if(pUser)
				pUser->QueryStatusSet()->DelObj(STATUS_WING);
		}
		break;
	case actionCancelKeepBow:
		{
			if(pUser)
				pUser->QueryStatusSet()->DelObj(STATUS_KEEPBOW);
		}
		break;

	case actionSuperChgMap:
		{
			if(pUser && pUser->IsAgent())
			{
				pRole->ProcessOnMove(MOVEMODE_CHGMAP);
				OBJID idMap = m_pInfo->idTarget;
				pUser->FlyMap(idMap, m_pInfo->unPosX, m_pInfo->unPosY);
			}
		}
		break;
	case actionFullItem:
		{
			if(pUser && pUser->IsAgent())
			{
				CAgent* pAgent = pUser->QueryAgent();
				IF_OK(pAgent)
				{
					pAgent->FullItem(m_pInfo->dwData);
				}
			}
		}
		break;

	case actionVerifyCode:
		{
			if(pUser)
				pUser->CheckVerifyCode(m_pInfo->dwData & 0xFFFF);
		}
		break;
	case actionCheatFound:
		{
			if(pUser && !pUser->IsInCheatJail())
			{
				pUser->SetCheatMark(m_pInfo->dwData, 0);
				if(m_pInfo->dwData != 10)
					pUser->SetCheatKickout(300);
			}
		}
		break;
	case actionDetectMsg:
		{
			if(pUser)
				pUser->OnDetectMsg(1, m_pInfo->dwData);
		}
		break;
	case actionPetJump:
		{
			CMonster* pMonster;
			if(pRole->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)) && pMonster->IsCallPet())
			{
				pRole->BroadcastRoomMsg(this, INCLUDE_SELF);
				CUser* pOwner = pMonster->QueryOwnerUser();
				if(pOwner && pOwner->GetDistance(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY) <= CELLS_PER_VIEW)
				{
					pRole->ProcessOnMove(MOVEMODE_TRANS);
					pRole->TransPos(m_pInfo->usTargetPosX, m_pInfo->usTargetPosY);
				}
			}
		}
		break;
	case actionLoginComplete:
		{
			if(pUser)
				pUser->OnLoginComplete();
		}
		break;
	case actionCheckAccount:
		{
			DWORD dwCheck = m_pInfo->dwData ^ pUser->GetAccountID();	if(pUser && pUser->GetEffect(0) != dwCheck)
			{
				pUser->SetCheatMark(9, 0);
				pUser->SetCheatKickout(RandGet(60) + 30);
			}
		}
		break;
	case actionSetWatchUser:
		{
			if(pUser && pUser->IsGM())
				SetWatchUser(pUser);
		}
		break;
	case actionClearWatchUser:
		{
			if(pUser && pUser->IsGM())
				SetWatchUser(NULL);
		}
		break;
	case actionQueryPlayerInfo:
		{
			if(!pUser)
				break;
			CUser* pTarget = UserManager()->GetUser(m_pInfo->dwData);
			if(pTarget)
			{
				CMsgPlayer msg;
				msg.Create(pTarget->QueryRole(), true);
				pUser->SendMsg(&msg);
			}
		}
		break;
	case actionUnk134:
	case actionUnk160:
	case actionUnk171:
	case actionUnk174:
	case actionUnk230:
	case actionUnk231:
	case actionUnk307:
		break;
	case actionQueryTradeBuddyInfo:	break;
	default:
		ASSERT(!"switch");
		break;
	}
	DEBUG_CATCH2("switch(MSGACTION) [%d]", m_pInfo->usAction)
}

CAutoLink<CUser>	g_linkWatchUser;
static void SetWatchUser(CUser* pUser)
{
	if(!pUser)
		g_linkWatchUser.Clear();
	g_linkWatchUser = pUser;
}
