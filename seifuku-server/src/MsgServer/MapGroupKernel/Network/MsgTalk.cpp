#include "AllMsg.h"
#ifdef	WORLD_KERNEL
#include "userlist.h"
#else
#pragma warning(disable:4786)
#include "mapgroup.h"
#endif
#include "GMToolCfg.h"
#include "user.h"
extern CAutoLink<CUser> g_linkWatchUser;	extern bool g_bCheckVerifyData;	static CItem* AwardItemWithOptions(CUser* pUser, const char* szWords);
CMsgTalk::CMsgTalk()
{
	Init();
	m_pInfo	=(MSG_Info* )m_bufMsg;

	m_StrPacker.SetBuf(m_pInfo->szBuf, _MAX_MSGSIZE-sizeof(MSG_Info)+1);
}

CMsgTalk::~CMsgTalk()
{

}

BOOL CMsgTalk::Create(LPCTSTR pszSpeaker, LPCTSTR pszHearer, LPCTSTR pszWords, 
					  LPCTSTR pszEmotion, DWORD dwColor, unsigned short unAttribute, 
					  unsigned short unStyle)
{
	if(!pszSpeaker || strlen(pszSpeaker) >= _MAX_NAMESIZE)
		return false;

	if(!pszHearer || strlen(pszHearer) >= _MAX_NAMESIZE)
		return false;

	if(!pszWords || strlen(pszWords) >= _MAX_WORDSSIZE)
		return false;

	if(pszEmotion && strlen(pszEmotion) >= _MAX_NAMESIZE)	
		return false;

	this->Init();

	m_pInfo->dwWordsColor	=dwColor;
	m_pInfo->unTxtAttribute	=unAttribute;
	m_pInfo->unTxtStyle		=unStyle;
	m_pInfo->dwTime			=::SysTimeGet();

	BOOL bSucMake	=true;
	bSucMake	&=m_StrPacker.AddString(pszSpeaker);
	bSucMake	&=m_StrPacker.AddString(pszHearer);
	bSucMake	&=m_StrPacker.AddString(pszEmotion);
	bSucMake	&=m_StrPacker.AddString(pszWords);

	m_unMsgType	=_MSG_TALK;
	m_unMsgSize	=sizeof(MSG_Info)-1+m_StrPacker.GetSize();

	return bSucMake;
}

void CMsgTalk::Process(void *pInfo)
{

#ifdef _MSGDEBUG
	::LogMsg("Process MsgTalk, Sender:%s, Receiver:%s, Words:%s", 
						szSender, 
						szReceiver, 
						szWords);
#endif

	char szSender[_MAX_NAMESIZE];
	char szReceiver[_MAX_NAMESIZE];
	char szEmotion[_MAX_NAMESIZE];
	char szWords[_MAX_WORDSSIZE];

	m_StrPacker.GetString(0, szSender, sizeof(szSender));
	m_StrPacker.GetString(1, szReceiver, sizeof(szReceiver));
	m_StrPacker.GetString(2, szEmotion, sizeof(szEmotion));
	m_StrPacker.GetString(3, szWords, sizeof(szWords));

	CHECK(strlen(szWords) <= 255);

	IRole* pRole = RoleManager()->QueryRole(this);
	if(!pRole)
	{
		if(IsNpcMsg())
			return;

		switch(m_pInfo->unTxtAttribute)
		{
		case	_TXTATR_SYNDICATE:
			{
				OBJID idSyn = m_pInfo->idTransmit;
				CSyndicate* pSyn = SynManager()->QuerySyndicate(idSyn);
				if (pSyn)
				{
					pSyn->BroadcastSynMsg(this, NULL);
				}
			}
			break;
		}

		return;
	}
	CUser* pUser = NULL;
	pRole->QueryObj(OBJ_USER, IPP_OF(pUser));
	if(pUser && strcmp(pUser->GetName(), szSender) != 0)
	{
		if (!pUser->IsGM())
		{
			::GmLogSave("Player [%s] tried to send a Talk message under another name [%s]; ban the account.", pUser->GetName(), szSender);
			return ;
		}
	}

	if(!pRole->IsTalkEnable())
	{
		pRole->SendSysMsg(STR_CAN_NOT_TALK);
		return;
	}

	if(pUser && pUser->GetCurrentLayoutType() == 1)
		return;

	if(pUser && pUser->IsGM())
	{
		::GmLogSave("-TALK %s->%s: %s", szSender, szReceiver, szWords);
	}

	if(false)
	{
		CUser* pWatchUser = g_linkWatchUser;
		if(pWatchUser)
			pWatchUser->SendMsg(this);
		return;
	}

	if(pUser && pUser->IsGM() && szWords[0] == '/')
	{
		char	szCmd[_MAX_WORDSSIZE]	= "NO_CMD";
		char	szParam[_MAX_WORDSSIZE] = "";
		char	szParam2[_MAX_WORDSSIZE] = "";
		DEBUG_TRY
		int nArgs = sscanf(szWords+1, "%s %s %s", szCmd, szParam, szParam2);
		if(stricmp(szCmd, "punish") == 0)
		{
			if(nArgs == 2 && pUser->IsGM())
			{
				if(szParam[0] == '1')
				{
					g_bCheckVerifyData	= true;
					pUser->SendSysMsg("cheater punish enable.\tYES");
				}
				else
				{
					g_bCheckVerifyData	= false;
					pUser->SendSysMsg("cheater punish disable. NO");
				}
			}
		}
		else if(stricmp(szCmd, "newbiemap") == 0)
		{
			if(pUser->IsGM() && nArgs == 3)
			{
				CGameMap* pMap = MapManager()->GetGameMap(atoi(szParam));
				if(pMap)
				{
					if(stricmp("on", szParam2) == 0)
					{
						pMap->m_bNewbieMap	= true;
						pUser->SendSysMsg("map set to newbie type now.");
					}
					else
					{
						pMap->m_bNewbieMap	= false;
						pUser->SendSysMsg("map set to unnewbie type now.");
					}
				}
			}
		}
		else if(stricmp(szCmd, "cmd") == 0)
		{
			if(pUser->IsGM())
			{
				LPCTSTR pszCmd = szWords + 1 + strlen(szCmd) + 1;
				if(strlen(pszCmd) > 0)
					MapGroup(PID)->QueryIntraMsg()->TransmitWorldTalk(pszCmd);
			}
		}
		else if(stricmp(szCmd, "lock") == 0)
		{
			if(nArgs == 2 && pUser->IsGM())
			{
				m_pInfo->unTxtAttribute	= _TXTATR_GM;
				MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(this);
			}
		}
		else if(stricmp(szCmd, "unlock") == 0)
		{
			m_pInfo->unTxtAttribute	= _TXTATR_GM;
			if(nArgs == 2 && pUser->IsGM())
				MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(this);
		}
		else if(stricmp(szCmd, "lockname") == 0)
		{
			if(nArgs == 2 && pUser->IsGM())
			{
				CUser* pTarget = UserManager()->GetUser(szParam);
				if(pTarget)
				{
					char szMsg[256] = "";
					sprintf(szMsg, "/lock %u", pTarget->GetAccountID());
					CMsgTalk msg;
					msg.Create(SYSTEM_NAME, ALLUSERS_NAME, szMsg, NULL, 0xFFFFFF, _TXTATR_GM);
					MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(&msg);
				}
				else
					pTarget->SendSysMsg("user (%s) not found.", szParam);
			}
		}
		else if(stricmp(szCmd, "unlockname") == 0)
		{
			if(nArgs == 2 && pUser->IsGM())
			{
				CUser* pTarget = UserManager()->GetUser(szParam);
				if(pTarget)
				{
					char szMsg[256] = "";
					sprintf(szMsg, "/unlock %u", pTarget->GetAccountID());
					CMsgTalk msg;
					msg.Create(SYSTEM_NAME, ALLUSERS_NAME, szMsg, NULL, 0xFFFFFF, _TXTATR_GM);
					MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(&msg);
				}
				else
					pTarget->SendSysMsg("user (%s) not found.", szParam);
			}
		}
		else if(stricmp(szCmd, "cheatwnd") == 0)
		{
			if(nArgs == 3 && pUser->IsGM())
			{
				char szMsg[1024] = "";
				sprintf(szMsg, "%s %s", szParam, szParam2);
				if(strlen(szMsg) < 256)
				{
					CMsgName msg;
					if(msg.Create(NAMEACT_CHEATWND, szMsg, 0))
						UserManager()->BroadcastMapGroupMsg(&msg);
				}
			}
		}
		else if(stricmp(szCmd, "逮捕") == 0 || stricmp(szCmd, "arrest") == 0)
		{
			if(nArgs == 2 && pUser->IsGM())
			{
				OBJID idTarget = atoi(szParam);
				CUser* pTarget = UserManager()->GetUser(idTarget);
				if(pTarget)
				{
					pTarget->GotoJail(6002, STR_GOTO_CHEATJAIL);
					pUser->SendSysMsg("arrest %s(%u) succeed.", pTarget->GetName(), idTarget);
				}
				else
					pUser->SendSysMsg("user (%u) not found.", idTarget);
			}
		}
		else if(stricmp(szCmd, "queryid") == 0)
		{
			if(pUser->IsPM() && nArgs == 2)
			{
				for(IRoleSet::Iter i = RoleManager()->QuerySet()->Begin(); i != RoleManager()->QuerySet()->End(); i++)
				{
					IRole* pTheRole = RoleManager()->QuerySet()->GetObjByIter(i);
					if(pTheRole && pTheRole->GetMap() && strcmp(pTheRole->GetName(), szParam) == 0 && pTheRole->GetMap() == pUser->GetMap())
					{
						char szMsg[1024] = "";
						sprintf(szMsg, "ID:%u, pos(%u, %u)", pTheRole->GetID(), pTheRole->GetPosX(), pTheRole->GetPosY());
						pUser->SendSysMsg(szMsg);
					}
				}
			}
		}
		else if(stricmp(szCmd, "QueryRole") == 0)
		{
			if(pUser->IsPM() && nArgs == 2)
			{
				OBJID idRole = atoi(szParam);
				if(idRole == ID_NONE)
					idRole = pUser->GetID();
				IRole* pTheRole = RoleManager()->QueryRole(idRole);
				if(pTheRole)
				{
					char szMsg[1024] = "";
					OBJID idOwner = pTheRole->QueryOwnerUser() ? pTheRole->QueryOwnerUser()->GetID() : ID_NONE;
					_snprintf(szMsg, 1024, "[QueryRole:%d] lookface:%d map:%u pos(%d:%d) effect:0x%x lev:%d life:%d maxlife:%d ownerid:%d sizeadd:%d",
						idRole, pTheRole->GetLookFace(), pTheRole->GetMap()->GetID(), pTheRole->GetPosX(), pTheRole->GetPosY(), pTheRole->GetEffect(0),
						pTheRole->GetLev(), pTheRole->GetLife(), pTheRole->GetMaxLife(), idOwner, pTheRole->GetSizeAdd());
					pUser->SendSysMsg(szMsg);
				}
			}
		}
		else if(stricmp(szCmd, "statuslist") == 0)
		{
			if(pUser->IsPM() && nArgs == 2)
			{
				OBJID idRole = atoi(szParam);
				if(idRole == ID_NONE)
					idRole = pUser->GetID();
				IStatusSet* pStatusSet = RoleManager()->QueryRole(idRole)->QueryStatusSet();
				if (pStatusSet)
				{
					pUser->SendSysMsg(_TXTATR_NORMAL, "================ Status info =================");
					for (int i=0; i<pStatusSet->GetAmount(); i++)
					{
						IStatus* pStatus = pStatusSet->GetObjByIndex(i);
						if (pStatus)
						{
							StatusInfoStruct info;
							pStatus->GetInfo(&info);
							pUser->SendSysMsg(_TXTATR_NORMAL, "Status=%d, power=%d, remain secs=%d, times=%d",
								info.nStatus, info.nPower, info.nSecs, info.nTimes);
						}
					}
					pUser->SendSysMsg(_TXTATR_NORMAL, "==============================================");
				}
			}
		}
		else if(stricmp(szCmd, "arrestname") == 0)
		{
			if(nArgs == 2 && pUser->IsGM())
			{
				CUser* pTarget = UserManager()->GetUser(szParam);
				if(pTarget)
				{
					pTarget->GotoJail(6002, STR_GOTO_CHEATJAIL);
					pUser->SendSysMsg("arrest %s succeed.", szParam);
				}
				else
					pUser->SendSysMsg("user (%s) not found.", szParam);
			}
		}
		else if(stricmp(szCmd, "client") == 0)
		{
			if(pUser->IsGM())
				UserManager()->CheckAllClient(180);
		}
		else if(stricmp(szCmd, "pro") == 0)
		{
			if(pUser->IsPM())
			{
				int	nData = atoi(szParam);
				pUser->SetAttrib(_USERATTRIB_PORFESSION, nData, SYNCHRO_TRUE);
			}
		}
		else if(stricmp(szCmd, STR_SETLOCK) == 0)
		{
			if(nArgs == 2)
			{
				if(pUser->SetLock(atoi(szParam)))
					pUser->SendSysMsg(STR_SETLOCK_OK);
				else
					pUser->SendSysMsg(STR_SETLOCK_FAIL);
			}
			else
				pUser->SendSysMsg(STR_SETLOCK_HELP);
		}
		else if(stricmp(szCmd, STR_UNLOCK) == 0)
		{
			if(nArgs == 2)
			{
				if(pUser->CheckLock(false))
				{
					if(pUser->OpenLock(atoi(szParam)))
						pUser->SendSysMsg(STR_UNLOCK_OK);
					else
						pUser->SendSysMsg(STR_UNLOCK_HELP);
				}
				else
					pUser->SendSysMsg(STR_NO_LOCKKEY);
			}
			else
				pUser->SendSysMsg(STR_UNLOCK_HELP);
		}
		else if(stricmp(szCmd, STR_CLEARLOCK) == 0)
		{
			if(nArgs == 2)
			{
				if(pUser->CheckLock(false))
				{
					if(pUser->ClearLock(atoi(szParam)))
						pUser->SendSysMsg(STR_CLEARLOCK_OK);
					else
						pUser->SendSysMsg(STR_CLEARLOCK_HELP);
				}
				else
					pUser->SendSysMsg(STR_NO_LOCKKEY);
			}
			else
				pUser->SendSysMsg(STR_CLEARLOCK_HELP);
		}
		else if (stricmp(szCmd, "焰火") == 0)
		{
			if(pUser->IsGM())
			{
				CMsgItem msg;
				IF_OK (msg.Create(pUser->GetID(), ITEMACT_FIREWORKS))
					pUser->BroadcastRoomMsg(&msg, true);
			}
		}
		else if (stricmp(szCmd, "焰火2") == 0)
		{
			if(pUser->IsGM())
			{
				int nLen = strlen(szParam);
				if (nLen > 0 && nLen < 9)
				{
					CMsgName msg;
					IF_OK (msg.Create(NAMEACT_FIREWORKS, szParam))
						pUser->BroadcastRoomMsg(&msg, true);
				}
			}
		}
		else if (stricmp(szCmd, "kickoutcheat") == 0)
		{
			if(pUser->IsPM())
			{
				int	nData = 0;
				if (2 == sscanf(szWords+1, "%s %d", szCmd, &nData))
				{
					extern long	g_sKickoutCheat;
					long nOld = InterlockedExchange(&g_sKickoutCheat, nData);
					pUser->SendSysMsg("set kickoutcheat OK! old value is %d", nOld);
				}
			}
		}
		else if (stricmp(szCmd, "action") == 0)
		{
			if(pUser->IsPM())
			{
				OBJID	idAction = ID_NONE;
				if (2 == sscanf(szWords+1, "%s %u", szCmd, &idAction))
					GameAction()->ProcessAction(idAction, pUser, NULL, NULL, NULL);
			}
		}
		else if(stricmp(szCmd, "sp") == 0)
		{
			if(pUser->IsPM())
			{
				int	nData = atoi(szParam);
				pUser->SetAttrib(_USERATTRIB_ENERGY, nData, SYNCHRO_TRUE);
			}
		}
		else if(stricmp(szCmd, "awardmoney") == 0)
		{
			if(pUser->IsPM())
			{
				int	nMoney = atoi(szParam);
				if(pUser->GainMoney(nMoney, SYNCHRO_TRUE))
					pUser->SendSysMsg("[Your money has increased.]");
			}
		}
		else if(stricmp(szCmd, "awarditem") == 0)
		{
			if(pUser->IsPM())
			{
				int	nItemType = atoi(szParam);
				if(nArgs > 2 ? AwardItemWithOptions(pUser, szWords) : pUser->AwardItem(nItemType, SYNCHRO_TRUE))
					pUser->SendSysMsg("[Your items have increased.]");
			}
		}
		else if(stricmp(szCmd, "kickout") == 0)
		{
			if(pUser->IsGM())
			{
				CUserPtr pTarget = UserManager()->GetUser(szParam);
				if (pTarget)
				{
					UserManager()->KickOutSocket(pTarget->GetSocketID(), "GM /kickout");
				}
			}
		}
		else if(stricmp(szCmd, "kickoutall") == 0)
		{
			if(pUser->IsGM())
			{
				LOGMSG("kickoutall process!");
				pUser->SendSysMsg(STR_KICKOUT_ALL);
				MapGroup(PID)->QueryIntraMsg()->PrintText("Server stop by GM, close server please!");
				MapGroup(PID)->QueryIntraMsg()->CloseMapGroup(GetSocketID());
			}
		}
		else if(stricmp(szCmd, "find") == 0)
		{
			if(pUser->IsGM())
			{
				CUserPtr pTarget = UserManager()->GetUser(szParam);
				if (pTarget)
				{
					int nPosX = pTarget->GetPosX();
					int nPosY = pTarget->GetPosY();

					pUser->FlyMap(pTarget->GetMapID(), nPosX, nPosY);
				}
			}
		}
		else if(stricmp(szCmd, "uplev") == 0)
		{
			if (pUser->IsPM())
			{
				int	nLev = atoi(szParam);
				if (nLev > 0)
				{
					pUser->IncLev(nLev, NULL);
					pUser->AllotPoint();

					pUser->AddAttrib(_USERATTRIB_LEV, 0, SYNCHRO_TRUE);
				}
			}
		}
		else if(stricmp(szCmd, "life") == 0)
		{
			if (pUser->IsGM())
			{
				int nData = pUser->GetMaxLife();
				pUser->SetAttrib(_USERATTRIB_LIFE, nData, SYNCHRO_TRUE);
			}
		}
		else if(stricmp(szCmd, "mana") == 0)
		{
			if (pUser->IsGM())
			{
				int nData = pUser->GetMaxMana();
				pUser->SetAttrib(_USERATTRIB_MANA, nData, SYNCHRO_TRUE);
			}
		}
		else if(stricmp(szCmd, "mapdata") == 0)
		{
			if (pUser->IsGM())
			{
				int x = 0, y = 0;
				if (3 == sscanf(szWords+1, "%s %d %d", szCmd, &x, &y))
				{
					int	nAttr=-1, nMask=-1, nAlt=-1, nAlt2=-1;
					if(pUser->GetMap()->GetDebugData(&nAttr, &nMask, &nAlt, &nAlt2, x, y))
						pUser->SendSysMsg("ATTR: %d, MASK: %d, ALT: %d, ALT2: %d", nAttr, nMask, nAlt, nAlt2);
				}
			}
		}
		else if(stricmp(szCmd, "showaction") == 0)
		{
			if (pUser->IsGM())
			{
				if(pUser->DebugShowAction())
					pUser->SendSysMsg("ACTION display on.");
				else
					pUser->SendSysMsg("ACTION display off.");
			}
		}
		else if(stricmp(szCmd, "fullxp") == 0)
		{
			if (pUser->IsGM())
			{
				pUser->SetXp(100);
			}
		}
		else if(stricmp(szCmd, "xp") == 0)
		{
			if (pUser->IsPM())
			{
				int nData = 0;
				if (2 == sscanf(szWords+1, "%s %d", szCmd, &nData))
				{
					pUser->SetXp(__min(100, nData));
				}
				else
					pUser->SetXp(100);
			}
		}
		else if(stricmp(szCmd, "awardwskill") == 0)
		{
			if (pUser->IsPM())
			{
				int nSkillType = 0, nLev = 0;
				if (3 == sscanf(szWords+1, "%s %d %d", szCmd, &nSkillType, &nLev))
				{
					pUser->AwardWeaponSkill(nSkillType, nLev);
				}
			}
		}
		else if(stricmp(szCmd, "awardmagic") == 0)
		{
			if (pUser->IsPM())
			{
				int nSkillType = 0, nLev = 0;
				if (3 == sscanf(szWords+1, "%s %d %d", szCmd, &nSkillType, &nLev))
				{

					pUser->QueryMagic()->LearnMagic(nSkillType, 0);
					for (int i=0; i<nLev; i++)
						pUser->QueryMagic()->UpLevelByTask(nSkillType, true);
				}
			}
		}
		else if(stricmp(szCmd, "superman") == 0)
		{
			if (pUser->IsPM())
			{
				pUser->AddAttrib(_USERATTRIB_SOUL, 500, SYNCHRO_TRUE);
				pUser->AddAttrib(_USERATTRIB_HEALTH, 500, SYNCHRO_TRUE);
				pUser->AddAttrib(_USERATTRIB_FORCE, 500, SYNCHRO_TRUE);
				pUser->AddAttrib(_USERATTRIB_SPEED, 500, SYNCHRO_TRUE);
			}
		}
		else if(stricmp(szCmd, "pos") == 0)
		{
			pUser->SendSysMsg("(%3d, %3d)", pUser->GetPosX(), pUser->GetPosY());
		}		
		else if (stricmp(szCmd, "player") == 0)
		{
			if (pUser->GetAccountID() <= 100)
			{
				if(stricmp(szParam, "all") == 0)
				{
					pUser->SendSysMsg(STR_SHOW_PLAYERS_uu, UserManager()->GetUserAmount(), UserManager()->GetMaxUser());
				}
				else if(stricmp(szParam, "map") == 0)
				{
					OBJID idMap = pUser->GetMapID();
					pUser->SendSysMsg("total %u player in this map.", UserManager()->GetMapUserAmount(idMap));
				}
			}
		}
		else if (stricmp(szCmd, "setmaxplayer") == 0)
		{
			if (pUser && pUser->IsPM() && strlen(szParam) > 0)
			{
				int nMaxPlayers = atoi(szParam);
				extern DWORD g_dwMaxPlayer;
				InterlockedExchange((long*)&g_dwMaxPlayer, nMaxPlayers);
				{
					pUser->SendSysMsg(STR_SET_MAX_PLAYERS_u, nMaxPlayers);
				}
			}
		}
		else if (stricmp(szCmd, "fly") == 0)
		{
			if (pUser && pUser->IsGM())
			{
				MSGBUF	szCmd	= "NO_CMD";
				int nPosX = 0, nPosY = 0;
				if (3 == sscanf(szWords+1, "%s %d %d", szCmd, &nPosX, &nPosY))
				{
					CMapPtr pMap = pUser->GetMap();
					if (pMap)
					{
						if(!pMap->IsStandEnable(nPosX, nPosY))
						{
							pUser->SendSysMsg(STR_CAN_STAND);
							return;
						}

						IMapThing* pMapThing = NULL;

						IThingSet* pSet = pMap->QueryBlock(nPosX, nPosY).QuerySet();
						for(int i = 0; i < pSet->GetAmount(); i++)
						{
							IMapThing* pTheMapThing = pSet->GetObjByIndex(i);
							if (pTheMapThing && 									
									pTheMapThing->GetObjType() == OBJ_NPC &&
										pTheMapThing->GetPosX() == nPosX && pTheMapThing->GetPosY() == nPosY)
							{
								pMapThing = pTheMapThing;
								break;		
							}
						}

						if (!pMapThing)
							pUser->FlyMap(ID_NONE, nPosX, nPosY);
						else
							pUser->SendSysMsg(STR_CAN_STAND);
					}
				}
			}
		}
		else if (stricmp(szCmd, "chgmap") == 0)
		{
			if (pUser && pUser->IsGM())
			{
				MSGBUF	szCmd	= "NO_CMD";
				OBJID idMap = ID_NONE;
				int nPosX = 0, nPosY = 0;
				if (4 == sscanf(szWords+1, "%s %u %d %d", szCmd, &idMap, &nPosX, &nPosY))
				{
					CMapPtr pMap = MapManager()->QueryMap(idMap);
					if (pMap)
					{
#ifdef	LOCAL_DEBUG
						if(!pMap->IsValidPoint(nPosX, nPosY))
#else
						if(!pMap->IsStandEnable(nPosX, nPosY))
#endif
						{
							pUser->SendSysMsg(STR_CAN_STAND);
							return;
						}
						IMapThing* pMapThing = NULL;

						IThingSet* pSet = pMap->QueryBlock(nPosX, nPosY).QuerySet();
						CHECK(pSet);
						for(int i = 0; i < pSet->GetAmount(); i++)
						{
							IMapThing* pTheMapThing = pSet->GetObjByIndex(i);
							if (pTheMapThing && 									
									pTheMapThing->GetObjType() == OBJ_NPC &&
										pTheMapThing->GetPosX() == nPosX && pTheMapThing->GetPosY() == nPosY)
							{
								pMapThing = pTheMapThing;
								break;		
							}
						}

						if (!pMapThing)
							pUser->FlyMap(idMap, nPosX, nPosY);
						else
							pUser->SendSysMsg(STR_CAN_STAND);
					}
					else
					{
						pUser->FlyMap(idMap, nPosX, nPosY);
					}
				}
			}
		}
		else if (stricmp(szCmd, "lotterytest") == 0)
		{
			if (pUser && pUser->IsPM())
			{
				MSGBUF	szCmd	= "NO_CMD";
				DWORD	nCount = 0, nLotteryType = 0, nColor = 0;
				if (4 == sscanf(szWords+1, "%s %u %u %u", szCmd, &nCount, &nLotteryType, &nColor))
				{
					ILotteryMgr*	pLotteryMgr		= (ILotteryMgr*)MapGroup(PID)->GetAdapter()->Find(ADAPTER_MODULE_LOTTERY, 0);
					IF_NOT(pLotteryMgr)
						return;
					ILotteryActor*	pActor			= (ILotteryActor*)MapGroup(PID)->GetAdapter()->GetActor(ADAPTER_MODULE_LOTTERY, pUser);
					IF_NOT(pActor)
						return;
					for (DWORD i = 0; i < nCount; i++)
						pLotteryMgr->Lottery(nLotteryType, nColor, pActor, true);
					pActor->Release();
				}
			}
		}
		else if (stricmp(szCmd, "千里传音") == 0 || stricmp(szCmd, "broadcast") == 0)
		{
			if (pUser->IsGM())
			{
				char szMsg[256] = "";
				if (2 == sscanf(szWords+1, "%s %s", szCmd, szMsg))
				{
					char szBuf[300]	= "";
					sprintf(szBuf, "%s：%s", pUser->GetName(), szMsg);
					UserManager()->BroadcastMsg(szBuf, NULL, NULL, 0xffffff, _TXTATR_GM);
				}
			}
		}
		else if (stricmp(szCmd, "reloadaction") == 0)
		{
			OBJID idAction=0;
			if (2 == sscanf(szWords+1, "%s %u", szCmd, &idAction))
			{
				if(idAction == ID_NONE)
				{
					if (pUser->IsPM())
					{
						pUser->SendSysMsg(STR_WARNING_CRASH);
						extern IActionSet* g_setAction;
						if(g_setAction)
							g_setAction->Release();
						g_setAction	= CActionSet::CreateNew(true);
						bool ret = g_setAction && g_setAction->Create("SELECT * FROM " _TBL_ACTION, Database());
						IF_OK_(ret)
							pUser->SendSysMsg("[ACTION UPDATE.]");
						LOGMSG("★process command: %s★", szCmd);
					}
				}
				else
				{
					if (pUser->IsGM())
					{
						extern IActionSet* g_setAction;
						SQLBUF	szSQL;
						sprintf(szSQL, "SELECT * FROM " _TBL_ACTION " WHERE id=%u LIMIT 1", idAction);
						CAutoPtr<IRecordset>	pRes = Database()->CreateNewRecordset(szSQL);
						CActionData* pData = CActionData::CreateNew();
						if(pData)
						{
							IF_OK(pData->Create(pRes))
							{
								DEBUG_TRY
								g_setAction->DelObj(pData->GetKey());
								g_setAction->AddObj(pData);
								pUser->SendSysMsg("[AN ACTION UPDATE.]");
								DEBUG_CATCH("reloadaction")
							}
							else
								pData->Release();
						}
						LOGMSG("%s process command: %s %d", pUser->GetName(), szCmd, idAction);
					}
				}
			}
		}
		else if (stricmp(szCmd, "reloadmagictype") == 0)
		{
			if (pUser->IsPM())
			{
				pUser->SendSysMsg(STR_WARNING_CRASH);
				
				extern IMagicTypeSet* g_setMagicType;
				if(g_setMagicType)
					g_setMagicType->Release();
				g_setMagicType	= CMagicTypeSet::CreateNew(true);
				bool ret = g_setMagicType && g_setMagicType->Create("SELECT * FROM " _TBL_MAGICTYPE, Database());
				IF_NOT_(ret)
					pUser->SendSysMsg("[ERROR: MAGICTYPE UPDATE ERROR]");

				extern IMagicTypeSet* g_setAutoMagicType;
				if(g_setAutoMagicType)
					g_setAutoMagicType->Release();
				g_setAutoMagicType	= CMagicTypeSet::CreateNew(true);
				ret = g_setAutoMagicType && g_setAutoMagicType->Create("SELECT * FROM " _TBL_MAGICTYPE " WHERE auto_learn!=0", Database());
				IF_NOT_(ret)
					pUser->SendSysMsg("[ERROR: MAGICTYPE UPDATE ERROR]");
				CUserManager::Iterator pUser = UserManager()->NewEnum();
				while(pUser.Next())
				{
					if(pUser)
						pUser->QueryMagic()->DebugReloadAll();
				}
				LOGMSG("★process command: %s★", szCmd);
			}
		}
		else if (stricmp(szCmd, "查询") == 0)
		{
			CUserPtr pTarget = UserManager()->GetUser(szParam);
			if(pTarget)
			{
				OBJID idSyn = pUser->QuerySynAttr()->GetSynID();
				OBJID idTargetSyn = pTarget->QuerySynAttr()->GetSynID();
				if(idSyn != ID_NONE && idTargetSyn == idSyn)
				{
					pUser->SendSysMsg("Contribution: %d.", pTarget->QuerySynAttr()->GetInt(SYNATTRDATA_PROFFER));
				}
			}
		}
		else if (stricmp(szCmd, "奖品") == 0)
		{
			if (!pUser->DoBonus())
			{
				pUser->SendSysMsg(STR_NO_BONUS);
			}
		}
		else if (stricmp(szCmd, "web") == 0)
		{
			if (pUser->IsGM() && strcmp(szReceiver, ALLUSERS_NAME) != 0)
			{
				char	szMsg[1024]	= "";
				strcpy(szMsg, szWords+5);
				CUserPtr pTarget = UserManager()->GetUser(szReceiver);
				if (pTarget)
				{
					CMsgTalk	msg;
					if (msg.Create(_TXTATR_WEBPAGE, szMsg))
						pTarget->SendMsg(&msg);
				}
				else
				{
					MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(this);
				}
			}
		}
		DEBUG_CATCH2("CMsgTalk CMD:[%s] error.", szCmd)
		return;
	}
	if(pUser && !pUser->IsAlive() && m_pInfo->unTxtAttribute != _TXTATR_TEAM)
	{
		m_pInfo->unTxtAttribute	= _TXTATR_GHOST;
		pUser->BroadcastRoomMsg(this, EXCLUDE_SELF);
		return ;
	}

	switch(m_pInfo->unTxtAttribute)
	{
	case	_TXTATR_GLOBAL:
		{
			if(pUser && pUser->IsGM())
				RoleManager()->BroadcastMsg(this);
		}
		break;
	case	_TXTATR_PRIVATE:
		{
			CUserPtr pTarget = UserManager()->GetUser(szReceiver);
			if(pTarget)
			{
				if(pUser)
				{
					OBJID idMap = pUser->GetMapID();
					if(idMap == 1036)
					{
						CUser* pWatchUser = g_linkWatchUser;
						if(pWatchUser && strlen(szWords) >= 40)
							pWatchUser->SendMsg(this);
					}
					else if(idMap == 6002 && !pUser->GetFriend(pTarget->GetID()))
						return ;
				}
				pUser->GetLookFace();
				pTarget->SendMsg(this);
			}
			else
				MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(this, szReceiver);
		}
		break;
	case	_TXTATR_TEAM:
		{
			if (pUser)
			{
				CTeam* pTeam = pUser->GetTeam();
				if (pTeam)
					pTeam->BroadcastTeamMsg(this, pUser);
			}
		}
		break;
	case	_TXTATR_FRIEND:
		{
			if (pUser)
				pUser->BroadcastFriendsMsg(this);
		}
		break;
	case	_TXTATR_SYNDICATE:
		{
			if (pUser)
			{
				OBJID idSyn = pUser->GetSynID();
				CSyndicate* pSyn = SynManager()->QuerySyndicate(idSyn);
				if (pSyn)
				{
					if(pSyn->GetInt(SYNDATA_MONEY) < SYN_MONEY_BASELINE)
					{
						pUser->SendSysMsg(STR_TOO_LOWER_SYN_MONEY);
						break;
					}
					pSyn->BroadcastSynMsg(this, pUser);

					m_pInfo->idTransmit	= pSyn->GetID();
					MapGroup(PID)->QueryIntraMsg()->TransmitMsg(this, GetSocketID(), GetNpcID());
				}
			}
		}
		break;
	case	_TXTATR_LEAVEWORD:
		{
			if (pUser)
			{
				if(!::TalkStrCheck(szWords, true))
				{
					pUser->SendSysMsg(STR_INVALID_CHARACTER);
					return ;
				}
				pUser->AddLeaveWord(szReceiver, szWords);
			}
		}
		break;
	case	_TXTATR_SYNANNOUNCE:
		{
			LPCTSTR pAnnounce = szWords;
			if(!pUser || pUser->GetSynID() == ID_NONE || pUser->GetSynRankShow() < 90)
				return ;
			if(!::TalkStrCheck(szWords, true))
			{
				pUser->SendSysMsg(STR_INVALID_CHARACTER);
				return ;
			}

			CSyndicate* pSyn = SynManager()->QuerySyndicate(pUser->GetSynID());
			IF_OK(pSyn)
			{
				pSyn->QueryModify()->SetTenet(pAnnounce);
				pSyn->BroadcastSubSynMsg(GetSocketID(), GetNpcID(), pAnnounce, NULL, NULL, 0xFFFFFF, _TXTATR_SYNANNOUNCE);
			}
		}
		break;
	case	_TXTATR_CRYOUT:
		{
			if (pUser && pUser->QueryBooth())
			{
				if(!::TalkStrCheck(szWords, false))
				{
					pUser->SendSysMsg(STR_INVALID_MSG);
					return ;
				}
				pUser->QueryBooth()->SetCryOut(szWords);
			}
		}
		break;
	case	_TXTATR_MSG_TRADE:
	case	_TXTATR_MSG_FRIEND:
	case	_TXTATR_MSG_TEAM:
	case	_TXTATR_MSG_SYN:
	case	_TXTATR_MSG_OTHER:
		{
			CUser* pUser = UserManager()->GetUser(this);
			if(!pUser)
				return ;
			if(pUser->GetLev() < LOGIN_FREE_LEVEL)
			{
				pUser->SendSysMsg(STR_LOW_LEVEL);
				return ;
			}
			if(!::TalkStrCheck(szWords, true))
			{
				pUser->SendSysMsg(STR_INVALID_CHARACTER);
				return ;
			}

				if(m_pInfo->unTxtAttribute == _TXTATR_MSG_SYN)
			{
				OBJID idSyn = pUser->GetSynID();
				if(idSyn == ID_NONE)
					return ;

				SetTransData(idSyn);
			}

			MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(this);
		}
		break;
	case	_TXTATR_MSG_SYSTEM:
		{
			MYASSERT(!"_TXTATR_MSG_OTHER");
		}
		break;

	case	_TXTATR_SERVE:
		{
			CUser* pUser = UserManager()->GetUser(this);
			if (!pUser)
				return;

			if (pUser->IsGM())
			{
				CUserPtr pTarget = UserManager()->GetUser(szReceiver);
				if (pTarget)
					pTarget->SendMsg(this);
				else
					MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(this, szReceiver);
			}
			else
			{
				CUserPtr pTarget = UserManager()->GetGM();
				if (pTarget)
					pTarget->SendMsg(this);
				else
					MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(this, "");
			}
		}
		break;
	case	_TXTATR_GM:	if(pUser && pUser->IsGM()) pRole->BroadcastRoomMsg(this, EXCLUDE_SELF);	break;
	case	_TXTATR_NORMAL:
		{
			pRole->BroadcastRoomMsg(this, EXCLUDE_SELF);
			CUser* pWatchUser = g_linkWatchUser;
			if(pWatchUser)
			{
				CGameMap* pMap = pRole->GetMap();
				POINT	pos = { pRole->GetPosX(), pRole->GetPosY() };
				if(pMap && g_pGMToolCfg && g_pGMToolCfg->IsInRegion(pMap->GetID(), pos))
					pWatchUser->SendMsg(this);
			}
		}
		break;
	}
}

static bool IsGemCode(int nGem)
{
	if (nGem == GEM_NONE || nGem == GEM_HOLE)
		return true;
	OBJID idGemType = 700000 + nGem;
	return nGem > 0 && nGem < 1000 && CItem::GetItemSort(idGemType) == ITEMSORT_OTHER && CItem::GetItemType(idGemType) == ITEMTYPE_GEM
			&& ItemType()->QueryItemType(idGemType) != NULL;
}

static CItem* AwardItemWithOptions(CUser* pUser, const char* szWords)
{
	MSGBUF	szCmd = "";
	int		nItemType = 0, nPlus = 0, nBless = 0, nHp = 0, nGem1 = 0, nGem2 = 0;
	if (sscanf(szWords+1, "%s %d %d %d %d %d %d", szCmd, &nItemType, &nPlus, &nBless, &nHp, &nGem1, &nGem2) < 3)
		return NULL;
	if (!CItem::IsEquipment(nItemType))
		return NULL;
	if (nPlus < 0 || (nPlus > 0 && !ItemAddition()->QueryItemAddition(nItemType, nPlus)))
		return NULL;
	if (nBless < 0 || nBless > 255 || nHp < 0 || nHp > 255 || !IsGemCode(nGem1) || !IsGemCode(nGem2))
		return NULL;

	CItem* pItem = pUser->AwardItem(nItemType, false);
	if (!pItem)
		return NULL;
	if (nGem1)
		pItem->SetInt(ITEMDATA_GEM1, nGem1);
	if (nGem2)
		pItem->SetInt(ITEMDATA_GEM2, nGem2);
	if (nPlus)
		pItem->SetInt(ITEMDATA_MAGIC3, nPlus);
	if (nBless)
		pItem->SetInt(ITEMDATA_REDUCE_DMG, nBless);
	if (nHp)
		pItem->SetInt(ITEMDATA_ADD_LIFE, nHp);

	CMsgItemInfo msg;
	if (msg.Create(pItem, ITEMINFO_ADDITEM))
		pUser->SendMsg(&msg);
	return pItem;
}
