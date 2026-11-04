#include "BaseFunc.h"
#include "3DGameMap.h"
#include "GameMsg.h"
#include "Hero.h"
#include "GamePlayerSet.h"
#include "GameDataSet.h"
#include "AllMsg.h"
#include "MsgAllot.h"
#include "ImgNumCounter.h"
#include "3DRoleData.h"
#include "Pet.h"
#include "nd_bitmap.h"
#include "EncryptFile.h"
#include <time.h>
extern UINT g_uStatus;

CHero g_objHero;

CHero::CHero()
{
	m_nType = MAP_HERO;

	m_dwEmoney		=0;
	m_dwMoney		=0;
	m_timeDie		=0;
	m_timeGhost		=0;
	m_idAccount		=ID_NONE;

	m_posAlign.x	=-1;
	m_posAlign.y	=-1;

	m_nPkMode = PKMODE_MONSTERONLY;

	m_setFriend.clear();
	m_nHeightCanJump = 200;

	m_dwLuckyTime = 0;
	m_dwLuckyTimeStamp = 0;
	m_dwSuperSoldierTail = 0;
	for (int i = 0; i < _MAX_EQUIPMENT; i++)
		m_pEquipment[i] = NULL;
	m_dwXpStartTime = (DWORD)-1;
	this->ResetXpFullTime();

	m_setSingleHandWeaponSkill.clear();
	m_setDoubleHandWeaponSkill.clear();
	m_setOtherWeaponSkill.clear();
	m_bMagicAttackBusy = false;
	this->CreateLevelExp();
	m_bFriendQueryBusy = false;
	m_idFriendQuery = ID_NONE;
	m_bFriendQueryReturn = false;
	strcpy(m_szSyndicateAnnounce, "NULL");
	this->SetRoleType(_ROLE_HERO);
	m_dwTimeSynWarReportLastFlash = ::TimeGet();

	memset(m_szWanted, 0L, sizeof(m_szWanted));
	memset(m_szPoliceWanted, 0L, sizeof(m_szPoliceWanted));
	m_dwBeginUseTransformMagicTime = ::TimeGet();

	m_dwTransformBegin = ::TimeGet();
	m_dwTransformDuration = 0;
	m_dwLastBump = 0;
	m_bShowAbortTransformDialog = false;
	m_nSynListIndex = -1;
	m_dwSuperSoldierTicks = 0;
	m_bChargeUp = false;

	m_dwLastTalk = 0;
	m_idWhoAttackMe	=	ID_NONE;
	m_idMyTarget	=	ID_NONE;
	m_idSpecialTarget	=	ID_NONE;
	m_objCheckKeys.Clear();
	m_bMagicLock = false;
	m_pKillCounter	= new CKillCounter;
	m_nKillCount = 0;
	m_pKoCounter	= new CKoCounter;
	m_bTaskKillCount = false;
	m_nKillCountPending = 0;
	m_objLuckyEffect = ::Game3DEffectCreate(NULL);
	m_dwBlessBegin = 0;
	m_dwBlessSeconds = 0;
	m_objBlessEffect = ::Game3DEffectCreate(NULL);
	m_dwCurseBegin = 0;
	m_dwCurseSeconds = 0;
	m_objCurseEffect = ::Game3DEffectCreate(NULL);
	m_dwDblExpBegin = 0;
	m_dwDblExpSeconds = 0;
	m_objDblExpEffect = ::Game3DEffectCreate(NULL);
	m_dwDayBonus = 0;
	m_bTableRequested = false;
}

CHero::~CHero()
{
	this->ClearFriendList();
	this->ClearEnemyList();
	this->DestroyWeaponSkill();
	this->ClearItem();
	this->DestroyMagicSet();
	this->ClearSynWarReport();
	this->ClearPet();
	if (m_pKillCounter)
	{
		delete m_pKillCounter;
		m_pKillCounter = NULL;
	}
	if (m_pKoCounter)
	{
		delete m_pKoCounter;
		m_pKoCounter = NULL;
	}
	if (m_objLuckyEffect)
	{
		m_objLuckyEffect->Release();
		m_objLuckyEffect = NULL;
	}
	if (m_objBlessEffect)
	{
		m_objBlessEffect->Release();
		m_objBlessEffect = NULL;
	}
	if (m_objCurseEffect)
	{
		m_objCurseEffect->Release();
		m_objCurseEffect = NULL;
	}
}

void CHero::Init(void)
{
	CRole::Init();
	m_nType = MAP_HERO;

	m_dwEmoney		=0;
	m_dwMoney		=0;
	m_timeDie		=0;
	m_timeGhost		=0;
	m_idAccount		=ID_NONE;

	m_posAlign.x	=-1;
	m_posAlign.y	=-1;

	m_nPkMode = PKMODE_MONSTERONLY;

	m_setFriend.clear();
	m_nHeightCanJump = 200;
	m_dwSuperSoldierTail = 0;
	m_Info.nTransformLook = 0;
	for (int i = 0; i < _MAX_EQUIPMENT; i++)
		m_pEquipment[i] = NULL;

	m_dwXpStartTime = (DWORD)-1;
	this->ResetXpFullTime();

	m_setSingleHandWeaponSkill.clear();
	m_setDoubleHandWeaponSkill.clear();
	m_setOtherWeaponSkill.clear();
	m_bMagicAttackBusy = false;
	m_bFriendQueryBusy = false;
	m_idFriendQuery = ID_NONE;
	m_bFriendQueryReturn = false;
	strcpy(m_szSyndicateAnnounce, "NULL");
	this->SetRoleType(_ROLE_HERO);
	m_dwTimeSynWarReportLastFlash = ::TimeGet();

	memset(m_szWanted, 0L, sizeof(m_szWanted));
	memset(m_szPoliceWanted, 0L, sizeof(m_szPoliceWanted));
	this->SetData(CPlayer::_PLAYER_CURRENTLIFE, 0);
	m_objCheckKeys.Clear();
	m_nKillCount = 0;
	m_nKillCountPending = 0;
	m_nTaskKillCount = 0;
	m_bTaskKillCount = false;
	m_dwBlessBegin = 0;
	m_dwBlessSeconds = 0;
	m_dwCurseBegin = 0;
	m_dwCurseSeconds = 0;
	m_dwDblExpBegin = 0;
	m_dwDblExpSeconds = 0;
	m_dwDayBonus = 0;
}

void CHero::Reset()
{
	this->ClearFriendList();
	this->ClearEnemyList();
	this->DestroyWeaponSkill();
	this->ClearItem();
	this->DestroyMagicSet();
	this->ClearSynWarReport();
	this->ClearTeamMember();
	g_objPlayerSet.DelAllPlayer();
	m_objTeam.Destroy();

	this->Init();
	memset(&m_infoPlayer, 0L, sizeof(m_infoPlayer));
	strcpy(m_szCryOut, "");
	const DWORD MaxPhysicalForce = 100;
	m_infoPlayer.dwMaxPhysicalForce = MaxPhysicalForce;
	m_infoPlayer.dwPhysicalForce = 0;
	this->ClearSynMember();
	m_bShowAbortTransformDialog = false;
	if (this->m_objEffect)
		this->m_objEffect->Clear();
	this->m_objDiceManager.Close(false);
	::PostCmd(CMD_RESET);
	m_objCheck.Reset();
	m_idWhoAttackMe	=	ID_NONE;
	m_idMyTarget	=	ID_NONE;
	m_idSpecialTarget	=	ID_NONE;

	this->ClearPet();
	m_objDummy.ClearEquipment();
	m_Info.usHair = ID_NONE;
	m_Info.usFace = ID_NONE;
	m_Info.idLWeaponType = ID_NONE;
	m_Info.idRWeaponType = ID_NONE;
	m_Info.idArmorType = ID_NONE;
	m_Info.idArmetType = ID_NONE;
	m_Info.idMountType = ID_NONE;
	m_objCheckKeys.Clear();
	m_nKillCount = 0;
	m_nKillCountPending = 0;
	m_nTaskKillCount = 0;
	m_bTaskKillCount = false;
	if (m_pKillCounter)
		m_pKillCounter->SetEffectPrefix("CountB");
	m_dwLuckyTime = 0;
	m_bTableRequested = false;
}

void CHero::Walk(int nPosX, int nPosY)
{
	this->StopChargeUp();
	int nCommandType = this->GetCommandType();
	if(nCommandType == _COMMAND_ATTACK ||
		nCommandType == _COMMAND_ACTION ||
		nCommandType == _COMMAND_EMOTION ||
		nCommandType == _COMMAND_WOUND ||
		nCommandType == _COMMAND_DEFEND)
		this->ResetActionData();
	if(this->TestStatus(USERSTATUS_FLY))
	{
		this->Jump(nPosX, nPosY);
		return;
	}

	if(this->GetCommandType() == _COMMAND_ATTACK)
		this->ResetActionData();
	if(m_objBoothManager.IsActive())
		return;
	extern UINT	g_uStatus;
	if (_STATUS_NORMAL != g_uStatus)
		return;

	if (this->IsDead() && !this->TestStatus(USERSTATUS_GHOST))
		return;

	CCommand* pcmdNow	=this->GetCommand();
	if(_COMMAND_WALK == pcmdNow->iType)
	{
		if(nPosX ==  pcmdNow->posTarget.x &&
				nPosY ==  pcmdNow->posTarget .y)
			return;
	}

	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(nPosX, nPosY);
	if ((pPlayer && !pPlayer->IsDead()))
		return;

	CCommand cmd;
	cmd.iType			=_COMMAND_WALK;
	cmd.iStatus			=_CMDSTATUS_BEGIN;
	cmd.posTarget.x		=nPosX;
	cmd.posTarget.y		=nPosY;

	this->SetCommand(&cmd);
}

void CHero::Run(int nPosX, int nPosY)
{
	this->StopChargeUp();
	int nCommandType = this->GetCommandType();
	if(nCommandType == _COMMAND_ATTACK ||
		nCommandType == _COMMAND_ACTION ||
		nCommandType == _COMMAND_EMOTION ||
		nCommandType == _COMMAND_WOUND ||
		nCommandType == _COMMAND_DEFEND)
		this->ResetActionData();
	if(this->TestStatus(USERSTATUS_FLY))
	{
		this->Jump(nPosX, nPosY);
		return;
	}
	if(this->GetCommandType() == _COMMAND_ATTACK)
		this->ResetActionData();
	if(m_objBoothManager.IsActive())
		return;
	extern UINT	g_uStatus;
	if (_STATUS_NORMAL != g_uStatus)
		return;

	this->AbortIntone();
	if (this->IsDead())
	{
		if (this->TestStatus(USERSTATUS_GHOST))
			this->Walk(nPosX, nPosY);

		return;
	}

	CCommand* pcmdNow	=this->GetCommand();
	if(_COMMAND_RUN == pcmdNow->iType)
	{
		if(nPosX ==  pcmdNow->posTarget.x &&
				nPosY ==  pcmdNow->posTarget .y)
			return;
	}

	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(nPosX, nPosY);
	if (pPlayer && !pPlayer->IsDead())
		return;

	CCommand cmd;
	cmd.iType			=_COMMAND_RUN;
	cmd.iStatus			=_CMDSTATUS_BEGIN;
	cmd.posTarget.x		=nPosX;
	cmd.posTarget.y		=nPosY;

	this->SetCommand(&cmd);
}

void CHero::Jump(int nPosX, int nPosY)
{
	extern UINT	g_uStatus;
	if (_STATUS_NORMAL != g_uStatus)
		return;
	if (this->IsDead())
		return;
	if(m_objBoothManager.IsActive())
		return;
	int nLookClone = m_Info.nTransformLook;
	if (nLookClone != 0)
	{
		CTransformInfo* pTransform = RoleDataQuery()->GetTransformInfo(nLookClone);
		if (pTransform && !pTransform->bCanJump)
			return;
	}
	else
	{
		if (::TimeGet() - m_dwBeginUseTransformMagicTime < 3000)
			return;
	}
	CMyPos posTarget;
	posTarget.x = nPosX;
	posTarget.y = nPosY;
	const OBJID idCanNotJumpSoFar = 100032;
	const OBJID idCanNotJump = 100022;
	const OBJID idCanNotStay = 100023;

	CMyPos posHero;
	this->GetPos(posHero);
	CONST int MAX_JUMPDISTANCE = 16;
	int nDistance = g_objGameMap.GetDistance(posHero.x, posHero.y, nPosX, nPosY);
	if (nDistance <= 0)
		return;
	if (MAX_JUMPDISTANCE < nDistance)
	{
		g_objGameMsg.AddMsg((char*)GameDataSetQuery()->GetStr(idCanNotJumpSoFar));
		return;
	}

	if(!this->CanJump(posTarget))
	{
		g_objGameMsg.AddMsg((char*)GameDataSetQuery()->GetStr(idCanNotJump));
		return;
	}

	CellInfo* pCellInfo = g_objGameMap.GetCell(nPosX, nPosY);
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(nPosX, nPosY);
	if(!pCellInfo ||
		g_objGameMap.GetGroundMask(pCellInfo) == 1 ||
		(pPlayer && !pPlayer->IsDead()))
	{
		g_objGameMsg.AddMsg((char*)GameDataSetQuery()->GetStr(idCanNotStay));
		return;
	}
	this->StopChargeUp();
	this->AbortIntone();
	CCommand* pcmdNow	=this->GetCommand();
	if(_COMMAND_JUMP == pcmdNow->iType)
	{
		if(nPosX ==  pcmdNow->posTarget.x &&
				nPosY ==  pcmdNow->posTarget.y)
			return;
	}

	CCommand cmd;
	cmd.iType			=_COMMAND_JUMP;
	cmd.iStatus			=_CMDSTATUS_BEGIN;
	cmd.posTarget.x		=nPosX;
	cmd.posTarget.y		=nPosY;
	cmd.dwData			=::TimeGet();

	this->SetCommand(&cmd);
}

void CHero::Direction(int nPosX, int nPosY)
{
	if (this->IsDead() && !this->TestStatus(USERSTATUS_GHOST))
		return;

	CMyPos posTarget;
	posTarget.x	=nPosX;
	posTarget.y	=nPosY;

	int nDir	=this->GetDir(posTarget);
	this->SetDir(nDir);

	CMyPos posHero;
	this->GetPos(posHero);

	CMsgAction msg;
	if (msg.Create(this->GetID(), posHero.x, posHero.y, nDir, actionChgDir))
		msg.Send();
}

void CHero::Talk(const char* pszTarget, const char* pszWords, const char* pszEmotion,
				 DWORD dwColor, unsigned short unAttribute, unsigned short unStyle)
{
	if (!pszTarget || !pszWords)
		return;

	char szWords[1024] = "";
	strncpy(szWords, pszWords, sizeof(szWords));
	if (0 == strcmp(GameDataSetQuery()->GetVersionInfo(), "中文"))
	{
		const char szFilter[2] = {'[', ']'};
		for (int i = 0; i < 2; i++)
			::ReplaceString(szWords, szFilter[i], '?');
	}

	if (this->IsGhost())
	{
		if (unAttribute == _TXTATR_NORMAL)
			unAttribute = _TXTATR_GHOST;
		else if (unAttribute != _TXTATR_TEAM)
		{
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(100025));
			return;
		}
	}

	if (szWords[0] == '/')
	{
		const char* pszCmdLine = szWords+1;

		char szCmd[256] = "";

		if (1 == sscanf(pszCmdLine, "%s", szCmd))
		{
			if (0 == stricmp(szCmd, "scrshot"))
			{
				char szFile[256] = "";
				if (2 == sscanf(pszCmdLine, "%s %s", szCmd, szFile))
				{
					CMyBitmap::ScreenShot(szFile);
					return;
				}
			}
			else if (0 == stricmp(szCmd, "break"))
			{
				g_objNetwork.Destroy();
			}
			else if (0 == stricmp(szCmd, "color"))
			{
				float f1 = 0.0f, f2 = 0.0f, f3 = 0.0f, f4 = 0.0f;
				if (5 == sscanf(pszCmdLine, "%s %f %f %f %f", szCmd, &f1, &f2, &f3, &f4))
					g_objHero.SetRGBA(f4, f1, f2, f3);
			}
			else if (0 == stricmp(szCmd, "look"))
			{
				int nLook = 0;
				if (2 == sscanf(pszCmdLine, "%s %d", szCmd, &nLook))
					g_objHero.SetLook(nLook);
			}
			else if (0 == stricmp(szCmd, "mount"))
			{
				int nMount = 0;
				if (2 == sscanf(pszCmdLine, "%s %d", szCmd, &nMount))
					g_objHero.SetMount(nMount);
			}
			else if (0 == stricmp(szCmd, "rolescale"))
			{
				int nScale = 0;
				if (2 == sscanf(pszCmdLine, "%s %d", szCmd, &nScale))
					CRole::s_fBaseScale = (float)nScale * 1.0f / 100.0f;
			}
			else if (0 == stricmp(szCmd, "ver"))
			{
				char szVer[256] = "";
				sprintf(szVer, "version %s-%s", "22:07:33", "Jun  1 2007");
				g_objGameMsg.AddMsg(szVer);
			}
			else if (0 == stricmp(szCmd, "scale"))
			{
				UINT uScale = 10000;
				if (2 == sscanf(pszCmdLine, "%s %u", szCmd, &uScale))
					g_objGameMap.SetScale(uScale);
			}
			else if (0 == stricmp(szCmd, "data"))
			{
				char szWhat[256] = "";
				sscanf(pszCmdLine, "%s %s", szCmd, szWhat);
			}
			else if (0 == stricmp(szCmd, "addeffect"))
			{
				char szEffect[64];
				if (2 == sscanf(pszCmdLine, "%s %s", szCmd, szEffect))
				{
					if (strstr(szEffect, "scr"))
						g_objHero.AddScrEffect(szEffect);
					else if (g_objHero.m_objEffect)
						g_objHero.m_objEffect->Add(szEffect);
				}
			}
			else if (0 == stricmp(szCmd, "addmsg"))
			{
				char szText[256] = "";
				if (2 == sscanf(pszCmdLine, "%s %s", szCmd, szText))
					g_objGameMsg.AddMsg(szText);
			}
			else if (0 == stricmp(szCmd, "mapcolor"))
			{
				DWORD dwARGB = 0;
				if (2 == sscanf(pszCmdLine, "%s %x", szCmd, &dwARGB))
					g_objGameMap.SetARGB(dwARGB);
			}
		}

	}
	if (_TXTATR_ACTION == unAttribute)
	{
		CONST DWORD INTERVAL_TALK = 5000;

		DWORD dwTimeNow = ::TimeGet();
		if (dwTimeNow > m_dwLastTalk + INTERVAL_TALK)
		{
			m_dwLastTalk = dwTimeNow;

			char szAction[256] = "";
			sprintf(szAction, szWords, this->GetName(), pszTarget);

			CMsgTalk msg;
			if (msg.Create((char*)this->GetName(), (char*)pszTarget, szAction, (char*)pszEmotion, dwColor, unAttribute, unStyle))
				msg.Send();
		}
	}
	else if (_TXTATR_SERVE == unAttribute)
	{
		char szTemp[1024] = "";
		strcpy(szTemp, szWords);
		::ReplaceString(szTemp, ' ', '_');
		CMsgTalk msg;
		if (msg.Create((char*)this->GetName(), (char*)pszTarget, szWords, (char*)pszEmotion, dwColor, unAttribute, unStyle))
			msg.Send();
	}
	else
	{
		CMsgTalk msg;
		if (msg.Create((char*)this->GetName(), (char*)pszTarget, szWords, (char*)pszEmotion, dwColor, unAttribute, unStyle))
			msg.Send();
	}

	g_objGameMsg.AddMsg((char*)this->GetName(), (char*)pszTarget, (char*)pszEmotion, szWords, dwColor, unStyle, unAttribute, ::SysTimeGet());

	if (unAttribute != _TXTATR_ACTION &&
			unAttribute != _TXTATR_PRIVATE)
		this->Say(szWords);
}

void CHero::Talk(CPlayer* pTarget, const char* pszWords, const char* pszEmotion,
				 DWORD dwColor, unsigned short unAttribute, unsigned short unStyle)
{
	if (!pszWords)
		return;

    char szSender[256];
	strcpy (szSender, this->GetName());

	char szTarget[256];
	const OBJID idEveryOne = 100026;
	if (!pTarget)
        strcpy(szTarget, GameDataSetQuery()->GetStr(idEveryOne));
	else
		strcpy(szTarget, pTarget->GetName());

	this->Talk(szTarget, pszWords, pszEmotion, dwColor, unAttribute, unStyle);
}

BOOL CHero::CanPk(CPlayer* pTarget)
{
	if (!pTarget)
		return false;

	switch(m_nPkMode)
	{
		case PKMODE_ARRESTMENT:
			{
				if (!pTarget->IsNpc() && !pTarget->IsBaddy())
				{
					if (this->GetDistance(pTarget) == 1)
					{
						const OBJID idCanNotPk = 100027;
						g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idCanNotPk));
					}
					return false;
				}
				if (pTarget->IsPet() && !pTarget->IsBaddy())
				{
					if (this->GetDistance(pTarget) == 1)
					{
						const OBJID idCanNotPk = 100027;
						g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idCanNotPk));
					}
					return false;
				}
			}
			break;
		case PKMODE_MONSTERONLY:
			{
				if (!pTarget->IsNpc())
				{
					if (this->GetDistance(pTarget) == 1)
					{
						const OBJID idCanNotPk = 100027;
						g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idCanNotPk));
					}
					return false;
				}
			}
			break;

		case PKMODE_TMEXCLUDE:
			{
				if (this->IsTeamMember(pTarget->GetID()))
				{
					if (this->GetDistance(pTarget) == 1)
					{
						const OBJID idCanNotPkTeamMember = 100072;
						g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idCanNotPkTeamMember));
					}
					return false;
				}

				if (0 == strcmp(this->GetMate(), pTarget->GetName()))
					return false;
			}
			break;

		default:
			break;
	}

	return true;
}

void CHero::Shoot(CPlayer* pTarget, OBJID idBullet, BOOL bForced)
{
	int nAction = _ACTION_ATTACK0+::RandGet(3);
	int nDir	= this->GetDir(pTarget);
	this->SetActionCmd(nAction, nDir);
	if(this->GetLWeapon() == ID_NONE)
		return;
	if(pTarget)
	{
		CMagicEffect* pEffect = CMagicEffect::CreateNewArrow(this->GetID(), pTarget->GetID(), this->GetLWeapon());
		g_objGameMap.AddMagicEffect(pEffect);
		pEffect->AddRoleInfo(pTarget->GetID(), 50);
	}
}
void CHero::Attack (CPlayer* pTarget, BOOL bForced )
{
	if (!pTarget)
		return;
	if (this->IsDead() || pTarget->IsDead())
		return;
	if (!this->CanPk(pTarget))
		return;
	CCommand* pcmdNow = this->GetCommand();
	if (this->IsAttacking() || _COMMAND_ATTACK == pcmdNow->iType)
	{
		if (pcmdNow->idTarget == pTarget->GetID())
		{
			::DebugMsg("same atk cmd, same target, ignor this cmd.");
			return;
		}
	}
	CCommand cmd;
	cmd.iType		=_COMMAND_ATTACK;
	cmd.iStatus		=_CMDSTATUS_BEGIN;
	cmd.idTarget	=pTarget->GetID();
	CMyPos posTarget;
	pTarget->GetPos(posTarget);
	cmd.nDir	=this->GetDir(posTarget);
	this->SetCommand(&cmd);
}
void CHero::TeamFollow()
{
	this->AbortIntone();
	TeamMemberInfo* pInfo = this->GetTeamMemberInfo(0);
	if(!pInfo)
		return;
	if(pInfo->id == this->GetID())
		return;
	CPlayer* pTarget = g_objPlayerSet.GetPlayer(pInfo->id);
	if(!pTarget)
		return;
	CCommand cmd;
	cmd.iType		=_COMMAND_TEAMFOLLOW;
	cmd.iStatus		=_CMDSTATUS_BEGIN;
	cmd.idTarget	=pTarget->GetID();
	int nAmount = this->GetTeamMemberAmount();
	for(int i = 0; i < nAmount; i ++)
	{
		TeamMemberInfo* pInfo = this->GetTeamMemberInfo(i);
		if(pInfo && pInfo->id == this->GetID())
		{
			cmd.dwData = i;
			break;
		}
	}

	CMyPos posTarget;
	pTarget->GetPos(posTarget);
	cmd.posTarget.x = m_Info.posMap.x - posTarget.x;
	cmd.posTarget.y = m_Info.posMap.y - posTarget.y;

	cmd.nDir	=this->GetDir(posTarget);

	this->SetCommand(&cmd);
}
void CHero::LockAttack (CPlayer* pTarget, BOOL bForced, BOOL bMove)
{
	if (!pTarget)
		return;
	if(this->HaveBow())
	{
		if(this->GetLWeapon() == ID_NONE)
		{
			const OBJID idStr = 100140;
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idStr));
			return;
		}
		if(this->TestStatus(USERSTATUS_CHARGEUP) || !bMove)
		{
			CMyPos posTarget;
			pTarget->GetPos(posTarget);
			int nAtkRange = (this->GetAddSize()+pTarget->GetAddSize()+1)/2+this->GetAtkRange();
			if (abs(m_Info.posMap.x - posTarget.x) > nAtkRange ||
				abs(m_Info.posMap.y - posTarget.y) > nAtkRange)
			{
				const OBJID idStr = 100141;
				g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idStr));
				return;
			}
		}
	}

	if (g_objGameMap.GetType() & MAPTYPE_BOOTH_ENABLE)
		return;
	this->AbortIntone();

	if (this->IsDead() || pTarget->IsDead())
		return;

	if (!this->CanPk(pTarget))
	{
		CMyPos posTarget;
		pTarget->GetPos(posTarget);

		int nDir = this->GetDir(posTarget);
		if(!this->TestStatus(USERSTATUS_FLY))
		{
			posTarget.x -= _DELTA_X[nDir];
			posTarget.y -= _DELTA_Y[nDir];

			this->Walk(posTarget.x, posTarget.y);
 		}
		else
		{
			posTarget.x -= _DELTA_X[nDir];
			posTarget.y -= _DELTA_Y[nDir];
			this->Jump(posTarget.x, posTarget.y);
		}
	}
	else
	{
		const OBJID idGuard = 100028;
		if (pTarget->IsNpc() && strstr(pTarget->GetName(), GameDataSetQuery()->GetStr(idGuard)))
		{
			if (!(bForced && PKMODE_FREE == this->GetPkMode()))
			{
				const OBJID idCanNotPkGuard = 100029;
				g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idCanNotPkGuard));
				return;
			}
		}

		if (pTarget->IsPlayer() &&
				this->GetSydicateID() != ID_NONE &&
					pTarget->GetSydicateID() == this->GetSydicateID() &&
						!bForced)
		{
			OBJID idStr = 100126;
			g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idStr));
			return;
		}
		if (this->IsAttacking() && m_Info.cmdProc.idTarget == pTarget->GetID())
		{
			::DebugMsg("same atk cmd, same target, ignor this cmd.");
			return;
		}
		{
			CCommand cmd;
			cmd.iType		=_COMMAND_LOCKATK;
			cmd.iStatus		=_CMDSTATUS_BEGIN;
			cmd.idTarget	=pTarget->GetID();
			this->SetCommand(&cmd);

			m_bFirstAtk = true;
		}
	}
}
void  CHero::LockBooth(CPlayer* pTarget)
{
	if (!pTarget)
		return;
	CCommand cmd;
	cmd.iType		=_COMMAND_LOCKBOOTH;
	cmd.iStatus		=_CMDSTATUS_BEGIN;
	cmd.idTarget	=pTarget->GetID();
	cmd.nDir		=pTarget->GetDir();
	this->SetCommand(&cmd);
}
BOOL CHero::CanBooth(CPlayer* pTarget)
{
	if(this->m_objBoothManager.IsActive())
		return false;
	if(!pTarget)
		return false;
	CMyPos posTarget;
	pTarget->GetPos(posTarget);
	int nDir = pTarget->m_pIRoleView->GetVariable(_VAR_DIR);
	CMyPos posBooth;
	posBooth.x = posTarget.x + _DELTA_X[nDir]*2;
	posBooth.y = posTarget.y + _DELTA_Y[nDir]*2;
	CPlayer* pPlayer=g_objPlayerSet.GetPlayer(posBooth.x, posBooth.y);
	CMyPos posHero;
	this->GetPos(posHero);
	if(posHero.x == posBooth.x && posHero.y == posBooth.y)
		return false;
	if(pPlayer)
		return false;
	return true;
}

void CHero::RushAttack (CPlayer* pTarget, BOOL bForced )
{
	if (!pTarget)
		return;
	this->AbortIntone();
	if (this->IsDead() || pTarget->IsDead())
		return;
	if (!this->CanPk(pTarget))
		return;
	CMyPos posTarget;
	pTarget->GetPos(posTarget);
	CMyPos pos;
	int nDir = pTarget->GetDir(this);
	for (int i=0; i<8; i++)
	{
		pos.x = posTarget.x+_DELTA_X[nDir];
		pos.y = posTarget.y+_DELTA_Y[nDir];
		if (this->CanJump(pos))
			break;
		nDir++;
	}
	CCommand cmd;
	cmd.iType		=_COMMAND_RUSHATK;
	cmd.iStatus		=_CMDSTATUS_BEGIN;
	cmd.idTarget	=pTarget->GetID();
	cmd.nDir		=this->GetDir(pos);
	cmd.posTarget.x	=pos.x;
	cmd.posTarget.y	=pos.y;
	this->SetCommand(&cmd);
}
void CHero::SkillAttack (CPlayer* pTarget, OBJID idSkillType, BOOL bForced )
{
	if (!pTarget || idSkillType == ID_NONE)
		return;
	this->AbortIntone();
	if (this->IsDead() || pTarget->IsDead())
		return;
	if (!this->CanPk(pTarget))
		return;
}
void CHero::Emotion	(int nEmotionType, CPlayer* pTarget, BOOL bFollow)
{
	if (this->IsDead() || this->TestStatus(USERSTATUS_FLY))
		return;

	this->AbortIntone();
	if (nEmotionType == _ACTION_DANCE1 &&
			!this->TestStatus(USERSTATUS_PRAYFOLLOW) &&
			(m_pEquipment[ITEMPOSITION_WEAPONR-1] || m_pEquipment[ITEMPOSITION_WEAPONL-1]))
	{
		const OBJID idCanNotDanceWithWeapon = 100030;
		g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idCanNotDanceWithWeapon));
		return;
	}
	if (!bFollow && this->TestStatus(USERSTATUS_PRAYFOLLOW))
	{
		const OBJID idFollowingPrayer = 10477;
		g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idFollowingPrayer));
		return;
	}

	CCommand cmd;
	cmd.iType		=_COMMAND_EMOTION;
	cmd.iStatus		=_CMDSTATUS_BEGIN;
	cmd.nData		=nEmotionType;

	if (pTarget)
	{
		CMyPos posTarget;
		pTarget->GetPos(posTarget);

		cmd.nDir	=this->GetDir(posTarget);
	}
	else
		cmd.nDir	=this->GetDir();

	this->SetCommand(&cmd);

	if (_ACTION_COOLPOSE == nEmotionType)
	{
		if (this->IsAllEquipmentNonsuch())
			this->AddProfessionalCoolposEffect(this->GetProfession(), true);
		else
		{
			if ((this->GetArmor()%10) == 9)
				this->AddProfessionalCoolposEffect(this->GetProfession(), false);
		}
	}
}

void CHero::Stop(void)
{
}

void CHero::Poison(CPlayer* pTarget, OBJID idItem)
{
	if (!pTarget || idItem == ID_NONE)
		return;
	if (this->IsDead() || pTarget->IsDead())
		return;
	if (!this->GetItem(idItem))
	{
		const OBJID idItemNotFound = 100031;
		g_objGameMsg.AddMsg((char*)GameDataSetQuery()->GetStr(idItemNotFound));
		return;
	}
	CCommand cmd;
	cmd.iType		=_COMMAND_ACTION;
	cmd.iStatus		=_CMDSTATUS_BEGIN;
	cmd.nData		=_ACTION_CAST;
	cmd.nDir		=this->GetDir(pTarget);
	this->SetCommand(&cmd);
	CMyPos posHero;
	this->GetPos(posHero);
	CMsgInteract msg;
	if(msg.Create(INTERACT_POISON, this->GetID(), pTarget->GetID(), posHero.x, posHero.y, idItem))
		msg.Send();
}
void CHero::Dart(CPlayer* pTarget, OBJID idItem)
{
	if (!pTarget || idItem == ID_NONE)
		return;
	if (this->IsDead() || pTarget->IsDead())
		return;
	if (!this->GetItem(idItem))
	{
		const OBJID idItemNotFound = 100031;
		g_objGameMsg.AddMsg((char*)GameDataSetQuery()->GetStr(idItemNotFound));
		return;
	}
	CCommand cmd;
	cmd.iType		=_COMMAND_ACTION;
	cmd.iStatus		=_CMDSTATUS_BEGIN;
	cmd.nData		=_ACTION_CAST;
	cmd.nDir		=this->GetDir(pTarget);
	this->SetCommand(&cmd);
	CMyPos posHero;
	this->GetPos(posHero);
	CMsgInteract msg;
	if(msg.Create(INTERACT_ASSASSINATE, this->GetID(), pTarget->GetID(), posHero.x, posHero.y, idItem))
		msg.Send();
}
void CHero::Steal(CPlayer* pTarget)
{
	if (!pTarget)
		return;
	if (this->IsDead() || pTarget->IsDead())
		return;
	CMyPos posHero;
	this->GetPos(posHero);
	CMsgInteract msg;
	if(msg.Create(INTERACT_STEAL, this->GetID(), pTarget->GetID(), posHero.x, posHero.y))
		msg.Send();
}
DWORD CHero::SetCommand(CCommand* pCommand)
{
	if (!pCommand)
		return false;
	if (this->IsIdle())
		this->ResetActionData();
	if((_COMMAND_ATTACK == m_Info.cmdProc.iType ||
		_COMMAND_SHOOT == m_Info.cmdProc.iType ||
		_COMMAND_INTONE == m_Info.cmdProc.iType)
		&& (pCommand->iType == _COMMAND_WALK ||
			pCommand->iType == _COMMAND_RUN ||
			pCommand->iType == _COMMAND_JUMP))
		this->ResetActionData();
	if (_COMMAND_DIE == pCommand->iType)
		this->ResetActionData();
	return CRole::SetCommand(pCommand);
}

BOOL CHero::ProcessCommand(void)
{
	int iPosX, iPosY;
	this->GetPos(iPosX, iPosY);
	if(_COMMAND_JUMP == m_Info.cmdProc.iType)
	{
		if(_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
		{
			CMsgAction msg;
			if(msg.Create(this->GetID(), iPosX, iPosY, m_Info.cmdProc.nDir, actionJump, m_Info.cmdProc.posTarget.x, m_Info.cmdProc.posTarget.y, m_Info.cmdProc.dwData))
 				msg.Send();
		}
	}

	if(_COMMAND_EMOTION == m_Info.cmdProc.iType)
	{
		if(_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
		{
			CMsgAction	msg;
			if(msg.Create(this->GetID(), iPosX, iPosY, m_Info.cmdProc.nDir, actionEmotion, m_Info.cmdProc.nData))
				msg.Send();
		}
	}

	if(_COMMAND_ATTACK == m_Info.cmdProc.iType)
	{
		if(_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
		{
			CMsgInteract msg;
			if(msg.Create(INTERACT_ATTACK, this->GetID(), m_Info.cmdProc.idTarget, iPosX, iPosY))
				msg.Send();
			this->OnAttackSent(m_Info.cmdProc.idTarget, 0, 0);
		}
	}

	if(_COMMAND_SHOOT == m_Info.cmdProc.iType)
	{
		if(_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
		{
			m_Info.cmdProc.iType = _COMMAND_ATTACK;
			CMsgInteract msg;
			if(msg.Create(INTERACT_SHOOT, this->GetID(), m_Info.cmdProc.idTarget, m_Info.cmdProc.dwData))
			{
				msg.Send();
				CMagicEffect* pEffect = CMagicEffect::CreateNewArrow(this->GetID(), m_Info.cmdProc.idTarget, m_Info.cmdProc.nData);
				g_objGameMap.AddMagicEffect(pEffect);
			}
			this->OnAttackSent(m_Info.cmdProc.idTarget, 1, 0);
		}
	}

	if(_COMMAND_RUSHATK == m_Info.cmdProc.iType)
	{
		if(_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
		{
			CMsgInteract msg;
			if(msg.Create(INTERACT_RUSHATK, this->GetID(), m_Info.cmdProc.idTarget, iPosX, iPosY, m_Info.cmdProc.posTarget.x, m_Info.cmdProc.posTarget.y, m_Info.cmdProc.idTarget))
				msg.Send();
			this->OnAttackSent(m_Info.cmdProc.idTarget, 2, 0);
		}
	}

	BOOL bAcomplished	=CRole::ProcessCommand();
	this->GetPos(iPosX, iPosY);

	switch(this->GetActionType())
	{
	case _ACTION_PICKUP:
		{
			CMsgMapItem msg;
			if(msg.Create(m_Info.cmdProc.idTarget, iPosX, iPosY))
				msg.Send();
		}
		break;

	case _ACTION_BUMP:
		{
			CONST int MAGIC_BUMP = 1051;
			CMagic* pMagic = this->GetMagic(MAGIC_BUMP);
			if (pMagic)
			{
				int nDir = this->GetDir();
				CMsgInteract msg;
				OBJID idUser = this->GetID();
				unsigned short usMagicType = MAGIC_BUMP;
				OBJID idTarget = 0;
				int nPosX = iPosX;
				int nPosY = iPosY;

				ENCODE_MAGICATTACK(idUser, usMagicType, nDir, nPosX, nPosY)

				IF_SUC (msg.Create( INTERACT_MAGICATTACK,
									idUser,
									nDir,
									nPosX,
									nPosY,
									usMagicType,
									pMagic->m_infoMagicType.dwLevel,
									nDir))
					msg.Send();
			}
		}
		break;

	case _ACTION_WALKL:
		{
			CMyPos posHero;
			this->GetPos(posHero);
			CMsgWalk msg;
			if(msg.Create(this->GetID(),this->GetDir()+8*::RandGet(31)))
				msg.Send();
		}
		break;

	case _ACTION_RUNL:
	case _ACTION_RUNR:
		{
			CMyPos posHero;
			this->GetPos(posHero);
			CMsgWalk msg;
			if(msg.Create(this->GetID(),this->GetDir()+8*::RandGet(31), 1))
				msg.Send();
		}
		break;
	}

	if (this->GetLastCommand() == _COMMAND_LOCKBOOTH &&
			this->GetCommandStatus() == _CMDSTATUS_ACCOMPLISH)
	{
		CMyPos posHero;
		this->GetPos(posHero);
		this->m_objBoothManager.Open();
		CMsgAction msg;
		if (msg.Create(this->GetID(), posHero.x, posHero.y, this->GetDir(), actionChgDir))
			msg.Send();
	}
	if (this->GetLastCommand() == _COMMAND_LOCKATK &&
			this->GetCommandStatus() == _CMDSTATUS_ACCOMPLISH &&
			!m_Info.cmdProc.bData)
	{
		if(!this->HaveBow())
		{
			CMsgInteract msg;
			if (msg.Create(INTERACT_ATTACK, this->GetID(), m_Info.cmdProc.idTarget, iPosX, iPosY))
				msg.Send();
			this->OnAttackSent(m_Info.cmdProc.idTarget, 0, 0);
		}
		else
		{
			CMsgInteract msg;
			if(msg.Create(INTERACT_SHOOT, this->GetID(), m_Info.cmdProc.idTarget, m_Info.cmdProc.dwData))
				msg.Send();
			this->OnAttackSent(m_Info.cmdProc.idTarget, 1, 0);
		}
		m_Info.cmdProc.bData = true;
	}

	return bAcomplished;
}

void CHero::SetAlignPos(int nPosX, int nPosY)
{
	CMyPos posHeroAlign	=g_objHero.GetAlignPos();
	g_objPlayerSet.ProcessManager(posHeroAlign.x, posHeroAlign.y);
	g_objGameMap.ProcessManager();
	m_posAlign.x	=nPosX;
	m_posAlign.y	=nPosY;
	g_objGameMap.ProcessRegion(m_posAlign);
}

DWORD CHero::GetMaxAtk()
{
	int nAttack = this->GetForce();

	for (int i=0; i<_MAX_EQUIPMENT; i++)
	{
		if(m_pEquipment[i])
		{
			int nItemAttack = (unsigned short)m_pEquipment[i]->GetMaxAttack();
			if (i + 1 == ITEMPOSITION_WEAPONL)
				nItemAttack /= 2;
			nAttack += nItemAttack;
		}
	}

	return __max(0, nAttack);
}

DWORD CHero::GetMinAtk()
{
	int nAttack = this->GetForce();

	for (int i=0; i<_MAX_EQUIPMENT; i++)
	{
		if(m_pEquipment[i])
		{
			int nItemAttack = (unsigned short)m_pEquipment[i]->GetMinAttack();
			if (i + 1 == ITEMPOSITION_WEAPONL)
				nItemAttack /= 2;
			nAttack += nItemAttack;
		}
	}

	return __max(0, nAttack);
}

DWORD	CHero::GetDef()
{
	int nDef = 0;

	for (int i=0; i<_MAX_EQUIPMENT; i++)
	{
		if(m_pEquipment[i])
		{
			nDef += m_pEquipment[i]->GetDefense();
		}
	}

	return __max(0, nDef);
}

DWORD	CHero::GetMagicDef()
{
	int nDef = this->GetSpeed();
	for (int i = 0; i < _MAX_EQUIPMENT; i++)
	{
		if (m_pEquipment[i])
			nDef += m_pEquipment[i]->GetDexterity();
	}
	if (nDef < 0)
		return 0;
	return nDef;
}
DWORD CHero::GetMaxLife()
{
	if (this->IsTransform())
		return CPlayer::GetMaxLife();

	int nLife = (this->GetForce() + this->GetSpeed() + this->GetHealth() * 8 + this->GetSoul()) * 3;
	if (this->GetProfessionSort() == 1)
	{
		int nPercent = 0;
		switch (this->GetProfession() % 10)
		{
		case 1:	nPercent = 5;	break;
		case 2:	nPercent = 8;	break;
		case 3:	nPercent = 10;	break;
		case 4:	nPercent = 12;	break;
		case 5:	nPercent = 15;	break;
		default:	nPercent = 0;	break;
		}
		nLife += nLife * nPercent / 100;
	}
	for (int i=0; i<_MAX_EQUIPMENT; i++)
	{
		if(m_pEquipment[i])
			nLife += m_pEquipment[i]->GetLife() + m_pEquipment[i]->GetAdditionLife() + m_pEquipment[i]->GetEnchant();
	}

	return __max(0, nLife);
}

DWORD CHero::GetMaxXP()
{
	return 100;
}
DWORD CHero::GetMaxMana()
{
	int nMaxMana = this->GetSoul() * 5;
	if (this->GetProfessionSort() > 10)
	{
		int nGrade = this->GetProfession() % 10;
		if (nGrade >= 5)
			nMaxMana = this->GetSoul() * 30;
		else if (nGrade >= 4)
			nMaxMana = this->GetSoul() * 25;
		else if (nGrade >= 3)
			nMaxMana = this->GetSoul() * 20;
		else if (nGrade >= 2)
			nMaxMana = this->GetSoul() * 15;
		else if (nGrade >= 1)
			nMaxMana = this->GetSoul() * 10;
	}
	for (int i=0; i<_MAX_EQUIPMENT; i++)
	{
		if(m_pEquipment[i])
			nMaxMana += m_pEquipment[i]->GetMana();
	}
	return __max(0, nMaxMana);
}

BOOL CHero::CanJump(CMyPos posTarGet)
{
	int nCanJumpHeight =  m_nHeightCanJump;
	if(this->TestStatus(USERSTATUS_FLY))
		nCanJumpHeight = 100;
	CMyPos posStart, posEnd;
	posEnd = posTarGet;


	DEQUE_CELL setCell;
	this->GetPos(posStart);

	CMyPos posHighest;
	g_objGameMap.Sampling(setCell, posHighest, posStart, posEnd, 32);

	CellInfo* pTargetCell = g_objGameMap.GetCell(posHighest.x, posHighest.y);
	if(!pTargetCell)
		return false;

	CellInfo* pCellInfo = g_objGameMap.GetCell(posStart.x, posStart.y);
	if(!pCellInfo)
		return false;

	if(g_objGameMap.GetGroundAltitude(pTargetCell) - g_objGameMap.GetGroundAltitude(pCellInfo) > nCanJumpHeight)
		return false;

	return true;
}

void CHero::JoinSynMember	(CPlayer* pTarget)
{
	if (!pTarget)
		return;
	CMyPos posHero;
	this->GetPos(posHero);
	CMsgInteract msg;
	MYASSERT (msg.Create(INTERACT_JOINSYN, this->GetID(), pTarget->GetID(), posHero.x, posHero.y));
	msg.Send();
}
void CHero::AcceptSynMember	(CPlayer* pTarget)
{
	if (!pTarget)
		return;
	CMyPos posHero;
	this->GetPos(posHero);
	CMsgInteract msg;
	MYASSERT (msg.Create(INTERACT_ACCEPTSYNMEMBER, this->GetID(), pTarget->GetID(), posHero.x, posHero.y));
	msg.Send();
}
void CHero::TitleSynMember	(CPlayer* pTarget, const char* pszTitle)
{
	if (!pTarget || !pszTitle)
		return;
	CMsgName msg;
	MYASSERT (msg.Create(NAMEACT_CHANGE_SYNTITLE, pszTitle, pTarget->GetID()));
	msg.Send();
}
void CHero::KickoutSynMember(CPlayer* pTarget)
{
	if (!pTarget)
		return;
	CMsgSyndicate msg;
	if(msg.Create(KICKOUT_MEMBER, pTarget->GetID()))
		msg.Send();
}
void CHero::QuitSyn(void)
{
	CMyPos posHero;
	this->GetPos(posHero);
	CMsgAction msg;
	MYASSERT (msg.Create(this->GetID(), posHero.x, posHero.y, this->GetDir(), actionQuitSyn));
	msg.Send();
}
BOOL CHero::ReBorn(DWORD dwData)
{
	if (!this->TestStatus(USERSTATUS_GHOST))
		return false;
	if (m_timeGhost != 0)
	{
		const OBJID idCanNotReBornYet = 100033;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idCanNotReBornYet));
		return false;
	}
	if (this->IsMoving())
	{
		const OBJID idCanNotReBorn = 100034;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idCanNotReBorn));
		return false;
	}
	if (dwData == 1)
	{
		if (!this->IsBlessed())
		{
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(10484));
			return false;
		}
		if (!g_objGameMap.IsRebornNowEnable())
		{
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(10485));
			return false;
		}
	}

	CMsgAction msg;
	if (msg.Create(this->GetID(), 0, 0, 0, actionReborn, dwData))
		msg.Send();
	return true;
}

BOOL CHero::ProcessChgMap(void)
{
	if (this->IsDead() || this->IsGhost())
		return false;

	if (_CMDSTATUS_ACCOMPLISH != this->GetCommandStatus())
		return false;
	int nActionType = this->GetActionType();
	if(_ACTION_STANDBY != nActionType &&
		_ACTION_REST1 != nActionType &&
		_ACTION_REST2 != nActionType &&
		_ACTION_REST3 != nActionType &&
		_ACTION_STANDBY_I != nActionType &&
		_ACTION_FLY_STANDBY != nActionType &&
		_ACTION_FLY_ALERT != nActionType)
		return false;

	CMyPos posHero;
	this->GetPos(posHero);

	int nX[9]={0,1,-1,1,0,1,-1,-1,0};
	int nY[9]={0,1,-1,0,1,-1,1,0,-1};

	for(int i = 0; i < 9; i ++)
	{
		CMyPos posTest;
		posTest.x = posHero.x + nX[i];
		posTest.y = posHero.y + nY[i];

		if (g_objGameMap.GetExitIndex(posTest) >= 0)
		{
			CMsgAction msg;
			IF_SUC (msg.Create (this->GetID(), posHero.x, posHero.y, this->GetDir(), actionChgMap, posTest.x, posTest.y))
			{
				msg.Send();

				g_uStatus = _STATUS_WAITING;
				return true;
			}
		}
	}

	return false;
}

void CHero::ProcessReborn(void)
{
	if (this->IsGhost() && m_timeGhost != 0 && ::TimeGet()-m_timeGhost >= 20*1000)
	{
		m_timeGhost = 0;
	}

	if (this->IsDead() && m_timeDie != 0 && ::TimeGet()-m_timeDie >= 3000)
	{
		m_timeDie = 0;

		CMsgAction msg;
		if (msg.Create(this->GetID(), 0, 0, 0, actionGhost))
			msg.Send();
	}
}

void CHero::Process(void* pInfo)
{
	m_objCheck.Process();
	CPlayer::Process(pInfo);

	m_objTeam.Process();
	m_objDummy.ProcessDummy();
	m_objPlayerDummy.ProcessDummy();
	this->ProcessMagicBusy();
	this->ProcessReborn();
	this->ProcessXp();
	this->ProcessChgMap();
	this->ProcessFriend();
	this->ProcessEnemy();
	this->ProcessMousePlayer();
	this->ProcessMagic();
	this->ProcessAbortTransformDialog();
	this->ProcessTeamMemberPos();
	this->ProcessPing();
	m_objDiceManager.Process();
	this->ProcessLuckyTime();

	int nLife = this->GetData(CPlayer::_PLAYER_LIFE);
	int nCurLife = this->GetData(CPlayer::_PLAYER_CURRENTLIFE);
	int nMaxLife = this->GetMaxLife();
	if(nCurLife < nLife)
	{
		if(nMaxLife >= 80)
			nCurLife += nMaxLife/80;
		else
			nCurLife ++;
	}
	if(nCurLife > nLife)
		nCurLife = nLife;
	this->SetData(CPlayer::_PLAYER_CURRENTLIFE, nCurLife);

	this->SetCurLife(::RandGet(INT_MAX));

	int nMana = this->GetMana();
	int nCurMana = this->GetCurMana();
	int nMaxMana = this->GetMaxMana();
	if(nCurMana < nMana)
	{
		if(nMaxMana >= 80)
			nCurMana += nMaxMana/80;
		else
			nCurMana ++;
	}
	if(nCurMana > nMana)
		nCurMana = nMana;
	{
		DWORD dwKey[4] = {::RandGet(INT_MAX), ::RandGet(INT_MAX), ::RandGet(INT_MAX), ::RandGet(INT_MAX)};
		this->m_infoPlayer.dataCurMana.set_key(dwKey, 4);
		this->SetCurMana(nCurMana);
	}
	{
		DWORD dwKey[4] = {::RandGet(INT_MAX), ::RandGet(INT_MAX), ::RandGet(INT_MAX), ::RandGet(INT_MAX)};
		this->m_infoPlayer.dataMana.set_key(dwKey, 4);
		this->SetMana(nMana);
	}
}

void CHero::DoLevelUp()
{
	OBJID idLevelUp = 100035;
	g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idLevelUp));

	CPlayer::DoLevelUp();
	::PostCmd(CMD_LEVELUP);
}

void CHero::AllotPoint(int nForce, int nSpeed, int nHealth, int nSoul)
{
	int nAddPoint = this->GetAddPoint();
	if (nAddPoint <= 0)
		return;

	int nAddSpeed	= (nSpeed - this->GetSpeed());
	int nAddForce	= (nForce - this->GetForce());
	int nAddHealth	= (nHealth - this->GetHealth());
	int nAddSoul	= (nSoul - this->GetSoul());

	if (nAddSpeed == 0 && nAddForce == 0 && nAddHealth == 0 && nAddSoul == 0)
		return;

	if ((nAddSpeed + nAddForce + nAddHealth + nAddSoul) > nAddPoint)
		return;

	this->SetSpeed(nSpeed);
	this->SetForce(nForce);
	this->SetSoul(nSoul);
	this->SetHealth(nHealth);

	this->SetAddPoint(nAddPoint-(nAddSpeed + nAddForce + nAddHealth + nAddSoul));

	CMsgAllot msg;
	if(msg.Create(nAddForce, nAddSpeed, nAddHealth, nAddSoul))
		msg.Send();
}

void CHero::Show(void* pInfo)
{
	CPlayer::Show(pInfo);
	this->ShowChargeUp();
}

void CHero::ShowXp(void)
{
	this->ShowFly();
	this->ShowTransform();
	this->ShowMagicShield();
	if (this->TestStatus(USERSTATUS_XPFULL))
	{
		m_objXpCounterDown.Show(m_dwXpStartTime - ::TimeGet() + XPFULL_TIME, CMyBitmap::GetScreenHeight() - 85, XPFULL_TIME, false);
		if (m_bTaskKillCount || (g_bKoShowRecord && !this->IsDead()))
			m_pKillCounter->Show();
	}
	else if (this->TestStatus(USERSTATUS_SPSOLDIER) || this->TestStatus(USERSTATUS_TORNADO))
	{
		int nTicksLeft = m_dwSuperSoldierTicks - (::TimeGet() - m_dwSuperSoldierBeginCopy) / 1000;
		if (nTicksLeft > 0)
		{
			int nX = CMyBitmap::GetScreenWidth() - 150;
			int nY = CMyBitmap::GetScreenHeight() - 140;
			int x = nTicksLeft * 35 + nX;
			for (int i = nTicksLeft; i >= 1; i--)
			{
				IAni* pAni = GameDataSetQuery()->GetDataAni("ani/common.ani", "XpBlock");
				if (pAni)
				{
					int nFrame = -1;
					switch (m_dwSuperSoldierTicks)
					{
					case 1:	nFrame = 0;	break;
					case 2:	nFrame = 1;	break;
					case 3:	nFrame = 2;	break;
					}
					if (nFrame >= 0)
					{
						CMyBitmap* pBmp = pAni->GetFrame(nFrame);
						if (pBmp)
							pBmp->Show(x, nY);
					}
				}
				x -= 35;
			}
			m_objXpCounterDown.Show(XPFULL_TIME, CMyBitmap::GetScreenHeight() - 85, XPFULL_TIME, false);
		}
		else
		{
			DWORD dwTail = m_dwSuperSoldierTail * 1000;
			if (dwTail > XPFULL_TIME)
				dwTail = XPFULL_TIME;
			m_objXpCounterDown.Show(dwTail, CMyBitmap::GetScreenHeight() - 85, XPFULL_TIME, false);
		}

		if (!this->IsDead())
			m_pKoCounter->Show();
	}
	else
	{
		if (m_bTaskKillCount || (g_bKoShowRecord && !this->IsDead()))
			m_pKillCounter->Show();
	}

	g_objImgNumCounter.Show();

	if (m_tmNameAlert.IsActive() && !m_tmNameAlert.TimeOver())
	{
		if (!(CPlayer::s_bShowName || g_objGameMap.GetShowMapObjInfoFlag() || this->IsDead()))
			this->ShowName();
	}
}

void CHero::BurstXp()
{
	m_dwXpStartTime = ::TimeGet();
	this->SetXP(100);
	if (m_objEffect)
		m_objEffect->Add((char*)"CircleUp");
}
void CHero::ProcessXp(void)
{
	if (this->IsDead())
		return;
	if (this->GetLev() < 3)
		return;
	if (this->TestStatus(USERSTATUS_XPFULL))
	{
		if (::TimeGet() >= m_dwXpStartTime + XPFULL_TIME)
		{
			CMsgAction msg;
			IF_SUC (msg.Create(this->GetID(), 0, 0, 0, actionXpCLear))
				msg.Send();
			this->SetXP(0);
			this->ClrStatus(USERSTATUS_XPFULL);
			::PostCmd(CMD_XPEMPTY);
		}
	}
	else if (m_tmXpAdd.ToNextTime())
		this->AddXp(1);
}
void CHero::ResetXpFullTime()
{
	m_tmXpAdd.Startup(3000);
	this->SetXP(0);
	if (m_objEffect)
		m_objEffect->DeleteByName((char*)"CircleUp");
}
BOOL CHero::AddXp(int nXp)
{
	int nNewXp = this->GetXP() + nXp;
	if (nNewXp > 100)
		nNewXp = 100;
	this->SetXP(nNewXp);
	return nNewXp >= 100;
}
void CHero::AddKoCount()
{
	if (this->TestStatus(USERSTATUS_SPSOLDIER))
	{
		m_dwKoCount ++;
		m_nKillCount ++;
		m_dwSuperSoldierBegin += KILL_AWARD_SPSOLDIER;
		if(m_dwSuperSoldierBegin >= ::TimeGet())
			m_dwSuperSoldierBegin = ::TimeGet();
		m_pKoCounter->Add(m_dwKoCount);
	}
	else if(this->TestStatus(USERSTATUS_TORNADO))
	{
		m_dwKoCount ++;
		m_nKillCount ++;
		m_dwSuperSoldierBegin += KILL_AWARD_TORNADO;
		if(m_dwSuperSoldierBegin >= ::TimeGet())
			m_dwSuperSoldierBegin = ::TimeGet();
		m_pKoCounter->Add(m_dwKoCount);
	}
	else if (m_bTaskKillCount)
		m_nKillCountPending ++;
	else
	{
		m_nKillCount ++;
		m_pKillCounter->Add(m_nKillCount);
	}
}
void CHero::ReplaceStatus(DWORD dwStatus)
{
	if (!this->TestStatus(USERSTATUS_RED))
	{
		if ((dwStatus&USERSTATUS_RED) != 0)
		{
			m_tmNameAlert.Startup(60*1000);
			g_objGameMsg.AddMsg(_TXTATR_GM, GameDataSetQuery()->GetStr(100153));
		}
	}

	if (!this->TestStatus(USERSTATUS_DARKRED))
	{
		if ((dwStatus&USERSTATUS_DARKRED) != 0)
		{
			m_tmNameAlert.Startup(60*1000);
			g_objGameMsg.AddMsg(_TXTATR_GM, GameDataSetQuery()->GetStr(100154));
		}
	}

	if (this->TestStatus(USERSTATUS_RED))
	{
		if ((dwStatus&USERSTATUS_RED) == 0)
		{
			g_objGameMsg.AddMsg(_TXTATR_GM, GameDataSetQuery()->GetStr(100155));
		}
	}

	if (this->TestStatus(USERSTATUS_DARKRED))
	{
		if ((dwStatus&USERSTATUS_DARKRED) == 0)
		{
			g_objGameMsg.AddMsg(_TXTATR_GM, GameDataSetQuery()->GetStr(100156));
		}
	}

	{
		if (this->TestStatus(USERSTATUS_XPFULL))
		{
			if ((dwStatus&USERSTATUS_XPFULL)==0)
			{
				this->SetXP(0);
				this->ResetXpFullTime();
				::PostCmd(CMD_XPEMPTY);
			}
		}
		else
		{
			if ((dwStatus&USERSTATUS_XPFULL))
			{
				this->BurstXp();
				::PostCmd(CMD_XPFULL);
			}
		}
	}

	if (!this->TestStatus(USERSTATUS_GHOST))
	{
		if (dwStatus&USERSTATUS_GHOST)
		{
			m_timeGhost = ::TimeGet();

			const int TIME_DELAY = 20;
			CMyPos posShow;
			posShow.x = CMyBitmap::GetScreenWidth()-50;
			posShow.y = CMyBitmap::GetScreenHeight()-200;
			g_objImgNumCounter.SetShowPos(posShow);
			g_objImgNumCounter.Start(TIME_DELAY, -1, 1000);
			OBJID idHowtoReborn = 100036;
			g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idHowtoReborn));

			::PostCmd(CMD_GHOST_BEGIN);
		}
	}
	else
	{
		if ((dwStatus&USERSTATUS_GHOST) == 0)
		{
			::PostCmd(CMD_REBORN);
			::PostCmd(CMD_GHOST_END);
			g_objImgNumCounter.End();
		}
	}

	if (this->TestStatus(USERSTATUS_DIE))
	{
		if (0 == (dwStatus&USERSTATUS_DIE))
		{
			this->ResetXpFullTime();
		}
	}
	else
	{
		if (0 != (dwStatus&USERSTATUS_DIE))
		{
			m_timeDie = ::TimeGet();
			this->QuitTrade();
			this->CloseTrade(false);
		}
	}

	if (dwStatus&USERSTATUS_SPSOLDIER)
	{
		if (!this->TestStatus(USERSTATUS_SPSOLDIER))
		{
			m_dwKoCount	= 0;
			m_dwKoScale = 100;
			m_dwSuperSoldierBeginCopy = ::TimeGet();
			m_dwSuperSoldierBegin = m_dwSuperSoldierBeginCopy + m_dwSuperSoldierTicks*1000;
			if(m_dwSuperSoldierBegin >= ::TimeGet())
				m_dwSuperSoldierBegin = ::TimeGet();
		}
		m_pKoCounter->Add(0);
	}
	else if (this->TestStatus(USERSTATUS_SPSOLDIER) && m_dwKoCount > 0)
	{
		if (!m_bTaskKillCount)
		{
			for (DWORD i = m_nKillCount - m_dwKoCount; i <= (DWORD)m_nKillCount; i++)
				m_pKillCounter->Add(i);
		}
		char szCount[256] = "";
		sprintf(szCount, "%d", m_dwKoCount);
		m_pKillCounter->SetString(szCount);
	}
	if (dwStatus&USERSTATUS_TORNADO)
	{
		if (!this->TestStatus(USERSTATUS_TORNADO))
		{
			m_dwKoCount	= 0;
			m_dwKoScale = 100;
			m_dwSuperSoldierBeginCopy = ::TimeGet();
			m_dwSuperSoldierBegin = m_dwSuperSoldierBeginCopy + m_dwSuperSoldierTicks*1000;
			if(m_dwSuperSoldierBegin >= ::TimeGet())
				m_dwSuperSoldierBegin = ::TimeGet();
			m_pKoCounter->Add(0);
		}
	}
	else if (this->TestStatus(USERSTATUS_TORNADO) && m_dwKoCount > 0)
	{
		if (!m_bTaskKillCount)
		{
			for (DWORD i = m_nKillCount - m_dwKoCount; i <= (DWORD)m_nKillCount; i++)
				m_pKillCounter->Add(i);
		}
		char szCount[256] = "";
		sprintf(szCount, "%d", m_dwKoCount);
		m_pKillCounter->SetString(szCount);
	}
	if(this->TestStatus(USERSTATUS_CHARGEUP))
	{
		if (!(dwStatus&USERSTATUS_CHARGEUP))
		{
			m_bChargeUp = false;
		}
	}
	if (dwStatus&USERSTATUS_PRAY)
	{
		if (!this->TestStatus(USERSTATUS_PRAY))
		{
			m_dwLuckyTimeStamp = ::TimeGet();
			g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(10479));
		}
	}
	else if (this->TestStatus(USERSTATUS_PRAY))
		m_dwLuckyTimeStamp = ::TimeGet();
	if (dwStatus&USERSTATUS_PRAYFOLLOW)
	{
		if (!this->TestStatus(USERSTATUS_PRAYFOLLOW))
		{
			m_dwLuckyTimeStamp = ::TimeGet();
			g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(10478));
			if (this->GetCommandType() == _COMMAND_STANDBY || this->GetCommandType() == _COMMAND_EMOTION)
			{
				int nAmount = g_objPlayerSet.GetPlayerAmount();
				for (int i = 0; i < nAmount; i++)
				{
					CPlayer* pPlayer = g_objPlayerSet.GetPlayerByIndex(i);
					if (!pPlayer || !pPlayer->TestStatus(USERSTATUS_PRAY) || this->GetDistance(pPlayer) > 3)
						continue;
					int nEmotion = 0;
					switch (pPlayer->GetActionType())
					{
					case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
					case 150: case 160: case 170: case 190: case 200: case 210: case 250:
						nEmotion = pPlayer->GetActionType();
						break;
					case 230: case 231:
						nEmotion = 230;
						break;
					case 270: case 271:
						nEmotion = 270;
						break;
					default:
						nEmotion = 0;
						break;
					}
					this->Emotion(nEmotion, NULL, true);
					break;
				}
			}
		}
	}
	else if (this->TestStatus(USERSTATUS_PRAYFOLLOW))
		m_dwLuckyTimeStamp = ::TimeGet();
	CPlayer::ReplaceStatus(dwStatus);
	if (((dwStatus&USERSTATUS_SPSOLDIER) && !this->TestStatus(USERSTATUS_SPSOLDIER)) ||
		((dwStatus&USERSTATUS_TORNADO) && !this->TestStatus(USERSTATUS_TORNADO)))
		this->ReportSuperState();
}
void CHero::CreateLevelExp()
{
	memset(m_setLevelExp, 0, sizeof(m_setLevelExp));
	char szDataFile[]="ini/LevExp.dat";
	CEncryptFile* pFile = new CEncryptFile(1234);
	if (!pFile)
		return;
	if (!pFile->Open(szDataFile))
	{
		::ErrorMsg("%s open failed.", szDataFile);
		return;
	}
	char szLine[1024] = "";
	while (pFile->ReadLine(szLine, sizeof(szLine)))
	{
		if (strlen(szLine) <= 0)
			continue;
		int nLevel = 0;
		DWORD dwExp = 0, dwExpUnit = 0;
		if (3 == sscanf(szLine, "%d %u %u", &nLevel, &dwExp, &dwExpUnit))
		{
			m_setLevelExp[nLevel - 1].dwExp = dwExp;
			m_setLevelExp[nLevel - 1].dwExpUnit = dwExpUnit;
		}
	}
	delete pFile;
}
DWORD CHero::GetNextLevelExp()
{
	if(this->GetLev() < 1)
		return 0;
	if(this->GetLev() >= _MAX_LEVEL)
		return 0;
	return m_setLevelExp[this->GetLev()-1].dwExp;
}
DWORD CHero::GetDayBonus()
{
	DWORD dwBonus = m_dwDayBonus;
	time_t tNow = ::time(NULL);
	struct tm* pNow = ::localtime(&tNow);
	if ((DWORD)((pNow->tm_mon + 1) * 100 + pNow->tm_mday) != dwBonus / 10)
		return 0;
	return m_dwDayBonus % 10;
}
void CHero::GetLevelForecast(int& nLevel, int& nPercent)
{
	int nLev = (int)this->GetLev();
	LevelExpInfo* pInfo = &m_setLevelExp[nLev - 1];
	DWORD dwExpNeed = pInfo->dwExp < 1 ? 1 : pInfo->dwExp;
	int nExp = nLev >= _MAX_LEVEL ? 0 : m_infoPlayer.nExp;
	int nScaled = ::MulDiv(nExp, pInfo->dwExpUnit, dwExpNeed);
	nLevel = nLev;
	int nBonusUnits = 600;
	int nDayBonus = (int)this->GetDayBonus();
	if (nDayBonus >= 9)
	{
		nPercent = ::MulDiv(nExp, 100, dwExpNeed);
		return;
	}
	if (nDayBonus >= 4)
		nBonusUnits = 300;
	DWORD dwRest = nScaled + nBonusUnits;
	DWORD dwUnit = 0;
	int nPassed = 0;
	for (int i = nLev - 1; i < _MAX_LEVEL; i++)
	{
		dwUnit = m_setLevelExp[i].dwExpUnit;
		if (dwRest < dwUnit)
			break;
		dwRest -= dwUnit;
		nPassed++;
	}
	nLevel += nPassed;
	if (dwUnit < 1)
		dwUnit = 1;
	nPercent = ::MulDiv(dwRest, 100, dwUnit);
}
void CHero::DeleteRole(DWORD dwData)
{
	CMyPos posHero;
	this->GetPos(posHero);

	CMsgAction msg;
	if (msg.Create(this->GetID(), posHero.x, posHero.y, this->GetDir(), actionDelRole, dwData))
		msg.Send();
}

char* CHero::GetProfessionalName(DWORD dwIndex)
{
	return m_objProfessional.GetProfessionalName(dwIndex);
}
void CHero::SetPkMode(int nPkMode, BOOL bSubmit)
{
	if(bSubmit)
	{
		CMsgAction msg;
		if(msg.Create(g_objHero.GetID(), 0, 0, 0, actionSetPkMode, nPkMode))
			msg.Send();
		if (nPkMode == PKMODE_MONSTERONLY)
		{
			m_idMyTarget = ID_NONE;
			m_dwTimeMyTarget = ::TimeGet();
			m_idWhoAttackMe = ID_NONE;
			m_dwTimeWhoAttackMe = ::TimeGet();
			int nAmount = m_setPetInfo.size();
			for (int i = 0; i < nAmount; i++)
			{
				CPet* pPet = (CPet*)g_objPlayerSet.GetPlayer(m_setPetInfo[i], false);
				if (pPet)
					pPet->SetHeroTarget(ID_NONE);
			}
		}
	}
	else
	{
		m_nPkMode = nPkMode;
		::PostCmd(CMD_CHANGEPKMODE);
	}
}
void CHero::AbortIntone()
{
	return;
	if(this->GetCommandType() == _COMMAND_INTONE)
	{
		if(this->GetCommandFlag())
			this->ResetActionData();
		CMsgInteract msg;
		if(msg.Create(INTERACT_ABORTMAGIC, this->GetID(), ID_NONE, 0, 0))
			msg.Send();
	}
}
void CHero::ChangeFace(int nFace)
{
	if (this->GetMoney() < 500)
	{
		OBJID idMoneyNotEnough = 100037;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idMoneyNotEnough));
		return;
	}

	CMsgAction msgAction;
	if(msgAction.Create(this->GetID(), 0, 0, 0, actionChangeFace, nFace))
		msgAction.Send();
}
void CHero::SetSyndicateRank(DWORD dwRank)
{
	if(this->GetSyndicateRank() != dwRank)
		::PostCmd(CMD_FLASH_SYNDICATE);
	this->CPlayer::SetSyndicateRank(dwRank);
}
void CHero::ProcessMousePlayer()
{
	if(!n_bMousePlayerTerrain)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayer(MOUSE_PLAYER_ID);
		if(pPlayer)
		{
			pPlayer->Process(NULL);
			if(!m_bMousePlayerFixed)
			{
				CMyPos posMouseCell, posMouseScr, posMouseWorld;
				::MouseCheck(posMouseScr.x, posMouseScr.y);
				g_objGameMap.Mouse2Cell(posMouseScr.x, posMouseScr.y, posMouseCell.x, posMouseCell.y);
				pPlayer->SetPos(posMouseCell);

				g_objGameMap.Cell2World(posMouseCell.x, posMouseCell.y, posMouseWorld.x, posMouseWorld.y);
				pPlayer->SetWorldPos(posMouseWorld);
			}
		}
	}
	else
	{
		if(!m_bMousePlayerFixed)
		{
			CMyPos posMouseCell, posMouseScr;
			::MouseCheck(posMouseScr.x, posMouseScr.y);
			g_objGameMap.Mouse2Cell(posMouseScr.x, posMouseScr.y, posMouseCell.x, posMouseCell.y);
			g_objGameMap.SetTerrainNpcPos(MOUSE_PLAYER_ID, posMouseCell);
		}
	}
}
void CHero::FixMousePlayer()
{
	m_bMousePlayerFixed = true;
	::PostCmd(CMD_OPEN_PLACE_NPC_MODIFY_DIALOG);
}
void CHero::CreateMousePlayer(int nLookType, BOOL bTerrain)
{
	n_bMousePlayerTerrain = bTerrain;

	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(MOUSE_PLAYER_ID);
	if(pPlayer)
		g_objPlayerSet.DelPlayer(MOUSE_PLAYER_ID);
	g_objGameMap.DeleteTerrainNpc(MOUSE_PLAYER_ID);
	m_bMousePlayerFixed = false;
	if(!bTerrain)
	{
		pPlayer	=new CPlayer;
		MYASSERT (pPlayer);
		pPlayer->SetID(MOUSE_PLAYER_ID);
		pPlayer->SetName("");
		pPlayer->Create(nLookType, _ROLE_VIEW_TASKNPC);

		pPlayer->SetRoleType(_ROLE_MOUSE_NPC);
		NpcTypeInfo* pInfo = RoleDataQuery()->GetNpcTypeInfo(nLookType/10);
		MYASSERT(pInfo);
		pPlayer->SetDir(nLookType%10);
		if (pPlayer->m_objEffect)
			pPlayer->m_objEffect->Add(pInfo->szEffectIndex);
		this->ProcessMousePlayer();
		g_objPlayerSet.AddPlayer(pPlayer);
	}
	else
	{
		CMyPos posCell;
		CMyPos posMouse;
		::MouseCheck(posMouse.x, posMouse.y);
		g_objGameMap.Screen2Cell(posMouse.x, posMouse.y, posCell.x, posCell.y);
		g_objGameMap.AddTerrainNpc(nLookType/10, nLookType%10, MOUSE_PLAYER_ID, posCell);
	}
	::PostCmd(CMD_OPEN_PLACE_NPC_INFO_DIALOG);
}
void CHero::SetMousePlayerDir(int nDir)
{
	if(!n_bMousePlayerTerrain)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayer(MOUSE_PLAYER_ID);
		if(pPlayer)
			pPlayer->m_pIRoleView->SetVariable(_VAR_DIRECT_DIR, nDir);
	}
	else
	{
		g_objGameMap.SetTerrainNpcDir(MOUSE_PLAYER_ID, nDir);
	}
}
void CHero::PutMousePlayer()
{
	if(!n_bMousePlayerTerrain)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayer(MOUSE_PLAYER_ID);
		if(pPlayer)
		{
			CMsgNpcInfo msg;
			CMyPos posNpc;
			pPlayer->GetPos(posNpc);
			if(msg.Create(posNpc, pPlayer->m_pIRoleView->GetVariable(_VAR_LOOK)))
				msg.Send();
			this->DelMousePlayer();
		}
	}
	else
	{
		CMyPos posCell;
		g_objGameMap.GetTerrainNpcPos(MOUSE_PLAYER_ID, posCell);
		int nLook = g_objGameMap.GetTerrainNpcLook(MOUSE_PLAYER_ID);

		CMsgNpcInfo msg;
		if(msg.Create(posCell, nLook))
			msg.Send();
		this->DelMousePlayer();
	}
}
void CHero::DelMousePlayer()
{
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(MOUSE_PLAYER_ID);
	if(pPlayer)
		g_objPlayerSet.DelPlayer(MOUSE_PLAYER_ID);
	g_objGameMap.DeleteTerrainNpc(MOUSE_PLAYER_ID);
	m_bMousePlayerFixed = true;
}
void CHero::DelNpc(OBJID idNpc)
{
	CMsgNpc msg;
	if(msg.Create(EVENT_DELNPC, idNpc))
		msg.Send();
}
BOOL CHero::IsTransform()
{
	return m_Info.nTransformLook != 0;
}
void CHero::BeginTransForm(DWORD dwMagicType, DWORD dwLevel)
{
	MagicTypeInfo infoMagic;
	if(!CMagic::s_objMagicData.GetMagicTypeInfo(dwMagicType, dwLevel, infoMagic))
		return;
	if(infoMagic.dwActionSort != MAGICSORT_TRANSFORM)
		return;
	m_dwTransformDuration = infoMagic.dwDuration*1000;
	m_dwTransformBegin = ::TimeGet();
}
void  CHero::BeginFly(DWORD dwMagicType, DWORD dwLevel)
{
	MagicTypeInfo infoMagic;
	if(!CMagic::s_objMagicData.GetMagicTypeInfo(dwMagicType, dwLevel, infoMagic))
		return;
	if((infoMagic.dwStatus  & USERSTATUS_FLY) == 0)
		return;
	m_dwFlyDuration = infoMagic.dwDuration*1000;
	m_dwFlyBegin = ::TimeGet();
}
void	CHero::ShowFly()
{
	if(!this->TestStatus(USERSTATUS_FLY))
	{
		m_dwFlyDuration = 0;
		return;
	}
	else
	{
		DWORD dwTimePass = ::TimeGet()-m_dwFlyBegin;
		if(dwTimePass > m_dwFlyDuration)
			return;
		CMyPos posTF;
		posTF.x = CMyBitmap::GetScreenWidth()-200;
		posTF.y = CMyBitmap::GetScreenHeight()-220;
		m_objImgNumber.SetShowPos(posTF);

		char szInfo[256] = "";
		if(m_dwFlyDuration/1000 > 100)
			sprintf(szInfo, "%03u", (m_dwFlyDuration - dwTimePass)/1000);
		else
			sprintf(szInfo, "%02u", (m_dwFlyDuration - dwTimePass)/1000);
		m_objImgNumber.SetString(szInfo);

		m_objImgNumber.SetScalePercent(130);
		if((m_dwFlyDuration - dwTimePass)/1000 < 10)
			m_objImgNumber.SetColor(_COLOR_RED, 0x99);
		else
			m_objImgNumber.SetColor(_COLOR_YELLOW, 0x77);
		m_objImgNumber.Show();
	}

}
void	CHero::BeginMagicShield(DWORD dwMagicType, DWORD dwLevel)
{
	MagicTypeInfo infoMagic;
	if(!CMagic::s_objMagicData.GetMagicTypeInfo(dwMagicType, dwLevel, infoMagic))
		return;
	if((infoMagic.dwStatus  & USERSTATUS_MAGICSHIELD) == 0)
		return;
	m_dwMagicShieldDuration = infoMagic.dwDuration*1000;
	m_dwMagicShieldBegin = ::TimeGet();
}
void	CHero::ShowMagicShield()
{
	if(!this->TestStatus(USERSTATUS_MAGICSHIELD))
	{
		m_dwMagicShieldDuration = 0;
		return;
	}
	else
	{
		DWORD dwTimePass = ::TimeGet()-m_dwMagicShieldBegin;
		if(dwTimePass > m_dwMagicShieldDuration)
			return;
		CMyPos posTF;
		posTF.x = CMyBitmap::GetScreenWidth()-200;
		posTF.y = CMyBitmap::GetScreenHeight()-220;
		m_objImgNumber.SetShowPos(posTF);

		char szInfo[256] = "";
		if((m_dwMagicShieldDuration - dwTimePass)/1000 > 100)
			sprintf(szInfo, "%03u", (m_dwMagicShieldDuration - dwTimePass)/1000);
		else
			sprintf(szInfo, "%02u", (m_dwMagicShieldDuration - dwTimePass)/1000);
		m_objImgNumber.SetString(szInfo);

		m_objImgNumber.SetScalePercent(130);
		if((m_dwMagicShieldDuration - dwTimePass)/1000 < 10)
			m_objImgNumber.SetColor(_COLOR_RED, 0x99);
		else
			m_objImgNumber.SetColor(_COLOR_YELLOW, 0x77);
		m_objImgNumber.Show();
	}
}

void CHero::ShowTransform()
{
	if(!this->IsTransform())
	{
		m_dwTransformDuration = 0;
		return;
	}
	else
	{
		DWORD dwTimePass = ::TimeGet()-m_dwTransformBegin;
		if(dwTimePass > m_dwTransformDuration)
			return;
		CMyPos posTF;
		posTF.x = CMyBitmap::GetScreenWidth()-200;
		posTF.y = CMyBitmap::GetScreenHeight()-220;
		m_objImgNumber.SetShowPos(posTF);

		char szInfo[256] = "";
		if(m_dwTransformDuration/1000 > 100)
			sprintf(szInfo, "%03u", (m_dwTransformDuration - dwTimePass)/1000);
		else
			sprintf(szInfo, "%02u", (m_dwTransformDuration - dwTimePass)/1000);
		m_objImgNumber.SetString(szInfo);

		m_objImgNumber.SetScalePercent(130);
		if((m_dwTransformDuration - dwTimePass)/1000 < 10)
			m_objImgNumber.SetColor(_COLOR_RED, 0x99);
		else
			m_objImgNumber.SetColor(_COLOR_YELLOW, 0x77);
		m_objImgNumber.Show();
	}
}
void CHero::ProcessAbortTransformDialog()
{
	BOOL bShow = true;

	if(! this->IsTransform() || this->IsGhost())
		bShow = false;
	if(this->IsDead())
		bShow = false;
	DWORD dwTimePass = ::TimeGet()-m_dwTransformBegin;
	const DWORD dwShowDelay = 30*1000;
	if(dwTimePass < dwShowDelay)
		bShow = false;
	if(m_bShowAbortTransformDialog != bShow)
	{
		m_bShowAbortTransformDialog = bShow;
		if(!bShow)
		{
			::PostCmd(CMD_CLOSE_ABORT_TRANSFORM_DLG);
		}
		else
		{
			::PostCmd(CMD_OPEN_ABORT_TRANSFORM_DLG);
		}
	}
}

DWORD CHero::GetBowShootRange()
{
	return 8;
}
void CHero::ApplyLanding()
{
	if(g_objHero.TestStatus(USERSTATUS_FLY))
	{
		CMsgAction msg;
		if(msg.Create(g_objHero.GetID(), 0, 0, 0, actionLanding))
			msg.Send();
	}

}
int	CHero::GetUserHelpInfoLineAmount()
{
	BOOL bSyndicate = this->GetLastSyndicateID() != ID_NONE;
	BOOL bMate = strcmp(this->GetMate(), GameDataSetQuery()->GetStr(100142)) != 0;
	return RoleDataQuery()->GetUserHelpInfoLineAmount(this->GetProfession(), this->GetLev(), bSyndicate, bMate, this->GetMetempsychosis());
}
const char* CHero::GetUserHelpInfoLineByIndex(int nIndex)
{
	BOOL bSyndicate = this->GetLastSyndicateID() != ID_NONE;
	BOOL bMate = strcmp(this->GetMate(), GameDataSetQuery()->GetStr(100142)) != 0;
	return RoleDataQuery()->GetUserHelpInfoLineByIndex(this->GetProfession(), this->GetLev(), bSyndicate, bMate, this->GetMetempsychosis(), nIndex);
}
void CHero::ShowArrow(int nOffset)
{
	if(!this->HaveBow())
		return;
	int nArrowAmount = 0;
	CItem* pItem = this->GetEquipment(ITEMPOSITION_WEAPONL);
	if(pItem && pItem->GetTypeID()/10000 == 105)
	{
		nArrowAmount = pItem->GetAmount();
	}
	IAni* pAni = GameDataSetQuery()->GetDataAni("ani/common.ani", "NoArrow");

	if(pAni)
	{
		CMyPos posShow;
		posShow.x = 135;
		posShow.y = CMyBitmap::GetScreenHeight() - 126;
		pAni->Show(0, posShow.x, posShow.y);
		char szAmount[64];
		sprintf(szAmount, "%d", nArrowAmount);
		CMyBitmap::ShowString(posShow.x-1, posShow.y-1, 0xff000000, szAmount);
		CMyBitmap::ShowString(posShow.x, posShow.y, 0xffffffff, szAmount);
	}
}
void	CHero::SetSpecialTarget(OBJID idTarget)
{
	m_idSpecialTarget = idTarget;
	m_dwTimeSpecialTarget = ::TimeGet();
}
OBJID	CHero::GetSpecialTarget()
{
	return m_idSpecialTarget;
}
void	CHero::ProcessSpecialTarget()
{
	if(m_idSpecialTarget != ID_NONE)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_idSpecialTarget, true);
		if(!pPlayer)
		{
			if(::TimeGet() - m_dwTimeSpecialTarget > 40000)
				m_dwTimeSpecialTarget = ID_NONE;
		}
		else
		{
			if(pPlayer->IsDead())
				m_dwTimeSpecialTarget = ID_NONE;
			else
				m_dwTimeSpecialTarget = ::TimeGet();
		}
	}
	if(m_idWhoAttackMe != ID_NONE)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_idWhoAttackMe, true);
		if(!pPlayer)
		{
			if(::TimeGet() - m_dwTimeWhoAttackMe > 10000)
				m_idWhoAttackMe = ID_NONE;
		}
		else
		{
			if(pPlayer->IsDead())
				m_idWhoAttackMe = ID_NONE;
			else
				m_dwTimeWhoAttackMe = ::TimeGet();
		}
	}
	if(m_idMyTarget != ID_NONE)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_idMyTarget, true);
		if(!pPlayer)
		{
			if(::TimeGet() - m_dwTimeMyTarget > 10000)
				m_idMyTarget = ID_NONE;
		}
		else
		{
			if(pPlayer->IsDead())
				m_idMyTarget = ID_NONE;
			else
				m_dwTimeMyTarget = ::TimeGet();
		}
	}
}
void	CHero::AddPet(OBJID idPet)
{
	int nAmount = m_setPetInfo.size();
	for(int i = 0; i < nAmount; i ++)
	{
		if(m_setPetInfo[i] == idPet)
			return;
	}
	m_idWhoAttackMe = ID_NONE;
	m_idMyTarget = ID_NONE;
	m_setPetInfo.push_back(idPet);
}
void	CHero::DelPet(OBJID idPet)
{
	int nAmount = m_setPetInfo.size();
	for(int i = nAmount-1; i >= 0; i --)
	{
		if(m_setPetInfo[i] == idPet)
		{
			m_setPetInfo.erase(m_setPetInfo.begin() + i);
		}
	}
}
void	CHero::ClearPet()
{
	m_setPetInfo.clear();
}
BOOL	CHero::CheckPet(OBJID idPet)
{
	int nAmount = m_setPetInfo.size();
	for(int i = 0; i < nAmount; i ++)
	{
		OBJID idMyPet = m_setPetInfo[i];
		if(idMyPet == idPet)
			return true;
	}
	return false;
}
void	CHero::SetTeamMemberPosTimer(DWORD dwInterval, DWORD dwKey)
{
	m_tmTeamMemberPos.Startup(dwInterval);
	m_dwTeamMemberPosKey = dwKey;
}
void	CHero::ProcessTeamMemberPos()
{
	if (m_tmTeamMemberPos.TimeOver())
	{
		CMyPos posHero;
		posHero.x = 0;
		posHero.y = 0;
		this->GetPos(posHero);
		CMsgAction msg;
		if (msg.Create(this->GetID(), posHero.x, posHero.y, this->GetDir(), actionTeamMemeberPos, ::TimeGet()))
			msg.Send();
	}
}
void	CHero::ProcessPing()
{
	if (m_tmPing.ToNextTime())
	{
		CMsgItem msg;
		if (msg.Create(this->GetID(), 0, ITEMACT_PING))
			msg.Send();
	}
}
void	CHero::RequestAction(DWORD dwData)
{
	CMsgAction msg;
	if (msg.Create(this->GetID(), 0, 0, 0, actionRequest, dwData))
		msg.Send();
}

void	CHero::OnAttackSent(OBJID idTarget, int nKind, DWORD dwData)
{
}
void	CHero::ProcessLuckyTime()
{
	if (m_dwLuckyTimeStamp == 0)
		return;
	DWORD dwNow = ::TimeGet();
	DWORD dwPassed = dwNow >= m_dwLuckyTimeStamp ? dwNow - m_dwLuckyTimeStamp : 0;
	if (this->TestStatus(USERSTATUS_PRAY))
		m_dwLuckyTime += dwPassed * 3;
	else if (this->TestStatus(USERSTATUS_PRAYFOLLOW))
		m_dwLuckyTime += dwPassed;
	else if (m_dwLuckyTime >= dwPassed)
		m_dwLuckyTime -= dwPassed;
	if (m_dwLuckyTime > 7200000)
		m_dwLuckyTime = 7200000;
	m_dwLuckyTimeStamp = dwNow;
}
void	CHero::ShowLuckyTime()
{
	if (m_dwLuckyTime == 0)
		return;
	if (!m_objLuckyEffect->TestEffect("Lucky"))
		m_objLuckyEffect->Add("Lucky");
	m_objLuckyEffect->SetPos(0.0f, -200.0f, 0.0f, -1, false);
	m_objLuckyEffect->Process();
	int nWorldX = 0, nWorldY = 0, nBgX = 0, nBgY = 0;
	g_objGameMap.Screen2World(100, 75, nWorldX, nWorldY);
	g_objGameMap.World2Bg(nWorldX, nWorldY, nBgX, nBgY);
	CMyPos posShow = { nBgX, nBgY };
	m_objLuckyEffect->Show(posShow);
	char szTime[256] = "";
	sprintf(szTime, "%02u:%02u:%02u", m_dwLuckyTime / 3600000 % 24, m_dwLuckyTime / 60000 % 60, m_dwLuckyTime / 1000 % 60);
	DWORD dwColor = _COLOR_YELLOW;
	if (this->TestStatus(USERSTATUS_PRAY) || this->TestStatus(USERSTATUS_PRAYFOLLOW))
		dwColor = _COLOR_GREEN;
	CMyBitmap::ShowString(50, 100, dwColor, szTime);
}
void CHero::SetBlessTime(DWORD dwSeconds)
{
	m_dwBlessSeconds = dwSeconds;
	m_dwBlessBegin = (DWORD)::time(NULL);
}
void CHero::ShowBless()
{
	if (m_dwBlessSeconds <= 0)
		return;
	if (!m_objBlessEffect->TestEffect("BlessFlag"))
		m_objBlessEffect->Add("BlessFlag");
	m_objBlessEffect->SetPos(0.0f, -200.0f, 0.0f, -1, false);
	m_objBlessEffect->Process();
	int nWorldX = 0, nWorldY = 0, nBgX = 0, nBgY = 0;
	g_objGameMap.Screen2World(100, 95, nWorldX, nWorldY);
	g_objGameMap.World2Bg(nWorldX, nWorldY, nBgX, nBgY);
	CMyPos posShow = { nBgX, nBgY };
	m_objBlessEffect->Show(posShow);
	int nLeft = (int)(m_dwBlessBegin - (DWORD)::time(NULL) + m_dwBlessSeconds);
	if (nLeft <= 0)
	{
		this->SetBlessTime(0);
		return;
	}
	char szTime[256] = "";
	sprintf(szTime, "%02u:%02u:%02u", nLeft / 86400, nLeft / 3600 % 24, nLeft / 60 % 60);
	CMyBitmap::ShowString(50, 120, 0xFFFF00, szTime);
}
void CHero::SetCurseTime(DWORD dwSeconds)
{
	m_dwCurseSeconds = dwSeconds;
	m_dwCurseBegin = (DWORD)::time(NULL);
}
void CHero::ShowCurse()
{
	if (m_dwCurseSeconds <= 0)
		return;
	if (!m_objCurseEffect->TestEffect("CurseFlag"))
		m_objCurseEffect->Add("CurseFlag");
	m_objCurseEffect->SetPos(0.0f, -200.0f, 0.0f, -1, false);
	m_objCurseEffect->Process();
	int nWorldX = 0, nWorldY = 0, nBgX = 0, nBgY = 0;
	g_objGameMap.Screen2World(100, 95, nWorldX, nWorldY);
	g_objGameMap.World2Bg(nWorldX, nWorldY, nBgX, nBgY);
	CMyPos posShow = { nBgX, nBgY };
	m_objCurseEffect->Show(posShow);
	int nLeft = (int)(m_dwCurseBegin - (DWORD)::time(NULL) + m_dwCurseSeconds);
	if (nLeft <= 0)
	{
		this->SetCurseTime(0);
		return;
	}
	char szTime[256] = "";
	sprintf(szTime, "%02u:%02u:%02u", nLeft / 3600 % 24, nLeft / 60 % 60, nLeft % 60);
	CMyBitmap::ShowString(50, 120, 0xFFFF00, szTime);
}
void CHero::SetDblExpTime(DWORD dwSeconds)
{
	m_dwDblExpSeconds = dwSeconds;
	m_dwDblExpBegin = (DWORD)::time(NULL);
}
void CHero::ShowDblExp()
{
	if (m_dwDblExpSeconds <= 0)
		return;
	if (!m_objDblExpEffect->TestEffect("DblExpFlag"))
		m_objDblExpEffect->Add("DblExpFlag");
	m_objDblExpEffect->SetPos(0.0f, -200.0f, 0.0f, -1, false);
	m_objDblExpEffect->Process();
	int nWorldX = 0, nWorldY = 0, nBgX = 0, nBgY = 0;
	g_objGameMap.Screen2World(100, 115, nWorldX, nWorldY);
	g_objGameMap.World2Bg(nWorldX, nWorldY, nBgX, nBgY);
	CMyPos posShow = { nBgX, nBgY };
	m_objDblExpEffect->Show(posShow);
	int nLeft = (int)(m_dwDblExpBegin - (DWORD)::time(NULL) + m_dwDblExpSeconds);
	if (nLeft <= 0)
	{
		this->SetDblExpTime(0);
		return;
	}
	char szTime[256] = "";
	sprintf(szTime, "%02u:%02u:%02u", nLeft / 3600, nLeft / 60 % 60, nLeft % 60);
	CMyBitmap::ShowString(50, 140, 0xFFFF00, szTime);
}
void CHero::ReportSuperState()
{
	int nPosX = 0, nPosY = 0;
	this->GetPos(nPosX, nPosY);
	CMsgAction msg;
	msg.Create(this->GetID(), nPosX, nPosY, this->GetDir(), actionReportSuperState, m_idAccount ^ m_dwStatus[0]);
}
void CHero::RequestTable(DWORD dwData)
{
	CMsgAction msg;
	if (msg.Create(this->GetID(), this->GetID(), dwData, actionTableRequest))
		msg.Send();
	if (dwData == 1)
		m_bTableRequested = true;
}
void CHero::ActTable(DWORD dwAction, DWORD dwData)
{
	CMsgAction msg;
	if (msg.Create(dwAction, this->GetID(), dwData, actionTableAct))
		msg.Send();
	if (dwAction == 3)
		this->ClearTableRecords();
}
void CHero::ClearTableRecords()
{
	for (MAP_TABLE::iterator it = m_mapTable.begin(); it != m_mapTable.end(); ++it)
		it->second.clear();
	m_mapTable.clear();
}
TableRecord* CHero::FindTableRecord(int nTable, int nIndex, BOOL bByIndex)
{
	MAP_TABLE::iterator it = m_mapTable.find(nTable);
	if (it == m_mapTable.end())
		return NULL;
	MAP_TABLERECORD& mapRecord = it->second;
	MAP_TABLERECORD::iterator itRecord;
	if (!bByIndex)
		itRecord = mapRecord.find(_TABLERECORD_NONE);
	else
		itRecord = mapRecord.find(nIndex);
	if (itRecord == mapRecord.end())
		return NULL;
	return &itRecord->second;
}
void CHero::AddTableRecord(int nTable, int nValue, int nData, int nIndex, BOOL bByIndex)
{
	MAP_TABLERECORD& mapRecord = m_mapTable[nTable];
	TableRecord record;
	record.nIndex = bByIndex ? nIndex : _TABLERECORD_NONE;
	record.fValue = (float)nValue;
	record.nData = nData;
	mapRecord[record.nIndex] = record;
}

