#include "Pet.h"
#include "Hero.h"
#include "AllMsg.h"
#include "3DGameMap.h"
#include "GamePlayerSet.h"
#include "GameMsg.h"
#include "3DRoleData.h"

CPet::CPet()
{
	m_nBehaviorType	=	_PET_BEHAVIOR_READY;
	m_idTarget		=	ID_NONE;
	m_idHeroTarget	=	ID_NONE;
	m_dwLastJumpTime	=	::TimeGet();
}

CPet::~CPet()
{
}

CPet*	CPet::CreateNewPet	(OBJID idPet, int nType)
{
	CPet* pPet = new CPet;
	MYASSERT(pPet);
	pPet->SetID(idPet);
	pPet->SetAIType(nType);

	return pPet;
}
int		CPet::GetAtkRange	(void)
{
	return m_AIInfo.nAttackRange;
}
void	CPet::SetAlignPos(int nX, int nY)
{
	m_posAlign.x = nX;
	m_posAlign.y = nY;
}
void	CPet::GetAlignPos(CMyPos& pos)
{
	pos.x = m_posAlign.x;
	pos.y = m_posAlign.y;
}

void	CPet::SetAIType(int nType)
{
	PetInfo* pInfo = RoleDataQuery()->GetPetInfo(nType);
	MYASSERT(pInfo);
	m_AIInfo.setLocateTargetMethod.clear();
	m_AIInfo.nEnvMethod = pInfo->nEnvMethod;
	m_AIInfo.nScale = pInfo->nScale;
	m_AIInfo.nAttackRange = pInfo->nAttackRange;
	m_Info.iAddSize = pInfo->nAddSize;
	m_AIInfo.nAddSize = pInfo->nAddSize;
	m_AIInfo.dwAttackInterval = pInfo->dwAttackInterval;
	int nAmount = pInfo->setLocateTargetMethod.size();
	for(int i = 0; i < nAmount; i  ++)
	{
		int nMethod = pInfo->setLocateTargetMethod[i];
		m_AIInfo.setLocateTargetMethod.push_back(nMethod);
	}
}
BOOL	CPet::CheckEnemy(CPlayer* pPlayer)
{
	const int _PET_SEARCH_ENEMY_RANGE = 12;
	if(!pPlayer)
		return false;
	CMyPos posHero, posPet, posTarget;
	g_objHero.GetPos(posHero);
	pPlayer->GetPos(posTarget);
	this->GetPos(posPet);
	if(abs(posHero.x - posTarget.x) > _PET_SEARCH_ENEMY_RANGE ||
		abs(posHero.y - posTarget.y) > _PET_SEARCH_ENEMY_RANGE)
	{
		return false;
	}
	if(abs(posPet.x - posTarget.x) > _PET_SEARCH_ENEMY_RANGE ||
		abs(posPet.y - posTarget.y) > _PET_SEARCH_ENEMY_RANGE)
	{
		return false;
	}
	return true;
}
BOOL	CPet::SearchEnemy()
{
	int nAmount = m_AIInfo.setLocateTargetMethod.size();
	if(nAmount > 0 && m_AIInfo.setLocateTargetMethod[0] != _PET_SE_STATUS_ASSISTANT)
	{
		OBJID idSpecial = g_objHero.GetSpecialTarget();
		if(idSpecial != ID_NONE)
		{
			CPlayer* pTarget = g_objPlayerSet.GetPlayer(idSpecial, true);
			if(pTarget && 	this->CheckEnemy(pTarget))
			{
				m_idTarget	= idSpecial;
				return true;
			}
		}
	}
	for(int i = 0; i < nAmount; i ++)
	{
		int nMethod = m_AIInfo.setLocateTargetMethod[i];
		this->SearchEnemy(nMethod);
		if(m_idTarget	!= ID_NONE)
			return true;
	}
	return false;
}
void	CPet::SearchEnemy(int nMethod)
{
	switch(nMethod)
	{
	case 1:
		this->AttackAI_1 ();
		break;
	case 2:
		this->AttackAI_2 ();
		break;
	case 3:
		this->AttackAI_3 ();
		break;
	case 4:
		this->AttackAI_4 ();
		break;
	case 5:
		this->AttackAI_5 ();
		break;
	case 6:
        this->AttackAI_6();
		break;
	case 7:
		this->AttackAI_7();
		break;
	case 8:
		this->AttackAI_8();
		break;
	case 9:
		this->AttackAI_9();
		break;
	default:
		break;
	}

}
void	CPet::CheckTarget()
{
	BOOL bProcess =  false;
	if(!bProcess)
		bProcess = this->CheckTargetDie();
	if(!bProcess)
		bProcess = this->CheckTargetNotFound();
	if(!bProcess)
		bProcess = this->CheckTargetTooFar();
	if(!bProcess)
		bProcess = this->CheckTargetSpecialCommand();
	if(!bProcess)
		bProcess = this->CheckTargetElude();
	if (!bProcess)
		m_nBehaviorType = _PET_BEHAVIOR_FIND_HOST;
}
BOOL	CPet::CheckTargetDie()
{
	CPlayer* pTarget = g_objPlayerSet.GetPlayer(m_idTarget, true);
	if(pTarget && (pTarget->IsDead() || pTarget->TestStatus(USERSTATUS_TEAM_SPIRIT)))
	{
		m_idTarget = ID_NONE;
		this->SearchEnemy();
		if(m_idTarget != ID_NONE)
			return true;
	}
	return false;
}
BOOL	CPet::CheckTargetNotFound()
{
	CPlayer* pTarget = g_objPlayerSet.GetPlayer(m_idTarget, true);
	if(!pTarget)
	{
		m_idTarget = ID_NONE;
		this->SearchEnemy();
		if(m_idTarget != ID_NONE)
			return true;
	}
	return false;
}
BOOL	CPet::CheckTargetTooFar()
{
	CMyPos posHero, posPet;
	g_objHero.GetPos(posHero);
	this->GetPos(posPet);
	if(abs(posPet.x - posHero.x) > 14 || abs(posPet.y - posHero.y) > 14)
	{
		m_idTarget = ID_NONE;
		this->SearchEnemy();
		if(m_idTarget != ID_NONE)
			return true;
	}
	return false;
}
BOOL	CPet::CheckTargetSpecialCommand()
{
	if(m_idTarget != g_objHero.GetSpecialTarget() && g_objHero.GetSpecialTarget() != ID_NONE)
	{
		OBJID idSpecial = g_objHero.GetSpecialTarget();
		CPlayer* pTarget = g_objPlayerSet.GetPlayer(idSpecial);
		if(pTarget && this->CheckEnemy(pTarget))
		{
			m_idTarget = g_objHero.GetSpecialTarget();
			return true;
		}
	}
	return false;
}
BOOL	CPet::CheckTargetElude()
{
	return false;
}

void	CPet::Idle(DWORD dwDelay)
{
	m_dwIdleEnd = ::TimeGet() + dwDelay;
	m_nBehaviorType = _PET_IDLE;
}
void	CPet::Process (void* pInfo)
{
	if(this->IsMyPet())
		this->ProcessAI();
	CPlayer::Process(pInfo);

	if(m_idHeroTarget != ID_NONE)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_idHeroTarget, true);
		if(!pPlayer)
		{
			if(::TimeGet() - m_dwHeroTargetTime > 10000)
				m_idHeroTarget = ID_NONE;
		}
		else if(pPlayer->IsDead())
			m_idHeroTarget = ID_NONE;
		else
			m_dwHeroTargetTime = ::TimeGet();
	}
}
void	CPet::SetHeroTarget(OBJID idTarger)
{
	m_idHeroTarget = idTarger;
	m_dwHeroTargetTime = ::TimeGet();
	if(idTarger == ID_NONE)
	{
		if(this->GetCommandType() == _COMMAND_LOCKATK)
			this->InitCommand();
	}
}
void	CPet::ProcessAI	()
{
	if(this->IsDead())
		return;
	if(this->IsAttacking())
		this->Idle(4000);

	switch(m_nBehaviorType)
	{
	case _PET_IDLE:
		this->ProcessAI_Idle();
		break;
	case _PET_BEHAVIOR_READY:
		this->ProcessAI_Ready();
		break;
	case _PET_BEHAVIOR_FIND_HOST:
		this->ProcessAI_FindHost();
		break;
	case _PET_ATTACK:
		this->ProcessAI_Attack();
		break;
	default:
		break;
	}
	this->ProcessAI_GenerateNewBehavior();
}
void	CPet::ProcessAI_Idle()
{
	if(::TimeGet() > m_dwIdleEnd)
		m_nBehaviorType = _PET_BEHAVIOR_READY;
}
void	CPet::ProcessAI_Ready()
{
}
void	CPet::ProcessAI_Attack()
{
	if(this->IsAttacking())
		return;
	else if(m_idTarget == ID_NONE)
		return;
	else
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_idTarget, true);
		if(pPlayer)
		{
			this->LockAttack(pPlayer);
		}
	}
}

void	CPet::ProcessAI_GenerateNewBehavior()
{
	CMyPos posHero, posPet;
	g_objHero.GetPos(posHero);
	this->GetPos(posPet);
	if(m_nBehaviorType == _PET_IDLE)
		return;
	this->CheckTarget();

		if(m_idTarget != ID_NONE)
	{
		m_nBehaviorType = _PET_ATTACK;
		return;
	}
}
void	CPet::ProcessAI_FindHost()
{
    CMyPos posPet,posNext,posHero;
    g_objHero.GetPos(posHero);
	this->GetPos(posPet);
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(posPet.x, posPet.y);
	BOOL bBusy = false;
	if(this->IsMoving())
	{
		m_dwLastJumpTime = ::TimeGet();
		bBusy = true;
	}
	if(this->GetCommandType() == _COMMAND_JUMP)
	{
		m_dwLastJumpTime = ::TimeGet();
		bBusy = true;
	}
	if(m_lstStepDir.size() != 0)
		bBusy = true;
	if(::TimeGet() - m_dwLastJumpTime < 1000)
		bBusy = true;

	if(abs(posHero.x - posPet.x) < 5 && abs(posHero.y - posPet.y) < 5 && pPlayer == this)
	{
		m_nBehaviorType = _PET_BEHAVIOR_READY;
		return;
	}
	if(bBusy)
		return;

	int nDir = ::RandGet(8);
	posNext.x = posHero.x + _DELTA_X[nDir]*4-::RandGet(2);
	posNext.y = posHero.y + _DELTA_Y[nDir]*4-::RandGet(2);

	CellInfo* pCellInfo = g_objGameMap.GetCell(posPet.x, posPet.y);
	if(!pCellInfo || g_objGameMap.GetGroundMask(pCellInfo) == 1 )
		return;
	if (abs(posHero.x - posPet.x) < 12 && abs(posHero.y - posPet.y) < 12)
	{
		this->Run(posNext.x, posNext.y);
	}
	else if(this->CanJump(posNext))
	{
		this->Jump(posNext.x, posNext.y);
	}
	else
	{
		this->Transfer(posNext.x, posNext.y);
	}
}

void CPet::LockAttack (CPlayer* pTarget)
{
	if (!pTarget)
		return;
	if (this->IsDead() || pTarget->IsDead())
		return;
	{
		CCommand cmd;
		cmd.iType		=_COMMAND_LOCKATK;
		cmd.iStatus		=_CMDSTATUS_BEGIN;
		cmd.idTarget	=pTarget->GetID();
		this->SetCommand(&cmd);

		m_bFirstAtk = true;
	}
}

void	CPet::Transfer(int nPosX, int nPosY)
{
	this->ResetActionData();
	CMyPos posPet;
	posPet.x = nPosX;
	posPet.y = nPosY;
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(nPosX, nPosY);
	CellInfo* pCellInfo = g_objGameMap.GetCell(nPosX, nPosY);
	if(!pCellInfo ||
		g_objGameMap.GetGroundMask(pCellInfo) == 1 )
				{
		return;
				}
	CMsgAction msg;
	if(msg.Create(this->GetID(), m_Info.posMap.x, m_Info.posMap.y, this->GetDir(), actionAlignPos, posPet.x, posPet.y))
		msg.Send();
	this->SetPos(posPet);

	CCommand cmd;
	cmd.iType			=_COMMAND_JUMP;
	cmd.iStatus			=_CMDSTATUS_BEGIN;
	cmd.posTarget.x		=nPosX;
	cmd.posTarget.y		=nPosY;
	cmd.dwData			=::TimeGet();

	this->SetCommand(&cmd);
}
BOOL	CPet::CanJump(CMyPos posTarGet)
{
	int nCanJumpHeight =  200;
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
void	CPet::Walk(int nPosX, int nPosY)
{
	int nCommandType = this->GetCommandType();
	if(nCommandType == _COMMAND_ATTACK ||
		nCommandType == _COMMAND_ACTION ||
		nCommandType == _COMMAND_EMOTION ||
		nCommandType == _COMMAND_WOUND ||
		nCommandType == _COMMAND_DEFEND)
		this->ResetActionData();

	if(this->GetCommandType() == _COMMAND_ATTACK)
		this->ResetActionData();
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

	CCommand cmd;
	cmd.iType			=_COMMAND_WALK;
	cmd.iStatus			=_CMDSTATUS_BEGIN;
	cmd.posTarget.x		=nPosX;
	cmd.posTarget.y		=nPosY;

	this->SetCommand(&cmd);

}
void	CPet::Run(int nPosX, int nPosY)
{
	int nCommandType = this->GetCommandType();
	if(nCommandType == _COMMAND_ATTACK ||
		nCommandType == _COMMAND_ACTION ||
		nCommandType == _COMMAND_EMOTION ||
		nCommandType == _COMMAND_WOUND ||
		nCommandType == _COMMAND_DEFEND)
		this->ResetActionData();
	if(this->GetCommandType() == _COMMAND_ATTACK)
		this->ResetActionData();
	extern UINT	g_uStatus;
	if (_STATUS_NORMAL != g_uStatus)
		return;

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

	CCommand cmd;
	cmd.iType			=_COMMAND_RUN;
	cmd.iStatus			=_CMDSTATUS_BEGIN;
	cmd.posTarget.x		=nPosX;
	cmd.posTarget.y		=nPosY;

	this->SetCommand(&cmd);
}

DWORD	CPet::SetCommand(CCommand* pCommand)
{
	if (!pCommand)
		return 0;

	if (_COMMAND_STANDBY != pCommand->iType)
	{
		if (_COMMAND_STANDBY == m_Info.cmdProc.iType
			|| _COMMAND_EMOTION == m_Info.cmdProc.iType
			|| _COMMAND_INTONE == m_Info.cmdProc.iType)
			this->ResetActionData();

		if (_CMDSTATUS_ACCOMPLISH == m_Info.cmdProc.iStatus)
		{
			if (_ACTION_STANDBY == m_Info.iActType
				|| _ACTION_REST1 == m_Info.iActType
				|| _ACTION_REST2 == m_Info.iActType
				|| _ACTION_REST3 == m_Info.iActType)
				this->ResetActionData();
		}

		if (_COMMAND_DIE == pCommand->iType)
			this->ResetActionData();
	}
	memcpy(&(m_Info.cmdProc), pCommand, sizeof(CCommand));
	return 1;
}

void	CPet::Jump(int nPosX, int nPosY)
{
	int nCommandType = this->GetCommandType();
	if(nCommandType == _COMMAND_ATTACK ||
		nCommandType == _COMMAND_ACTION ||
		nCommandType == _COMMAND_EMOTION ||
		nCommandType == _COMMAND_WOUND ||
		nCommandType == _COMMAND_DEFEND)
		this->ResetActionData();
	extern UINT	g_uStatus;
	if (_STATUS_NORMAL != g_uStatus)
		return;
	if (this->IsDead())
		return;

	CMyPos posTarget;
	posTarget.x = nPosX;
	posTarget.y = nPosY;
	const OBJID idCanNotJump = 100022;
	const OBJID idCanNotStay = 100023;

	CMyPos posPet;
	this->GetPos(posPet);
	CONST int MAX_JUMPDISTANCE = 16;
	BOOL bFar = false;
	if (MAX_JUMPDISTANCE < g_objGameMap.GetDistance(posPet.x, posPet.y, nPosX, nPosY))
	{
		bFar = true;
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
	if (posPet.x == nPosX && posPet.y == nPosY)
		return;

	CCommand cmd;
	cmd.iType			=_COMMAND_JUMP;
	cmd.iStatus			=_CMDSTATUS_BEGIN;
	cmd.posTarget.x		=nPosX;
	cmd.posTarget.y		=nPosY;
	cmd.dwData			=::TimeGet();
	cmd.bData			=bFar;

	this->SetCommand(&cmd);
	m_dwLastJumpTime	=::TimeGet();
}
BOOL	CPet::ProcessCommand()
{
	int iPosX, iPosY;
	this->GetPos(iPosX, iPosY);

	if(_COMMAND_JUMP == m_Info.cmdProc.iType)
	{
		if(_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
		{
			CMsgAction msg;
			if(msg.Create(this->GetID(), iPosX, iPosY, m_Info.cmdProc.nDir, m_Info.cmdProc.bData ? actionAlignPos : actionJump, m_Info.cmdProc.posTarget.x, m_Info.cmdProc.posTarget.y, m_Info.cmdProc.dwData))
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
		}
	}

	if(_COMMAND_RUSHATK == m_Info.cmdProc.iType)
	{
		if(_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
		{
			CMsgInteract msg;
			if(msg.Create(INTERACT_RUSHATK, this->GetID(), m_Info.cmdProc.idTarget, iPosX, iPosY, m_Info.cmdProc.posTarget.x, m_Info.cmdProc.posTarget.y))
				msg.Send();
		}
	}
	BOOL bAcomplished	=CRole::ProcessCommand();
	this->GetPos(iPosX, iPosY);

	switch(this->GetActionType())
	{

	case _ACTION_WALKL:
		{
			CMyPos posPet;
			this->GetPos(posPet);

			CMsgWalk msg;
			if(msg.Create(this->GetID(),this->GetDir()+8*::RandGet(31)))
				msg.Send();
		}
		break;

	case _ACTION_RUNL:
	case _ACTION_RUNR:
		{
			CMyPos posPet;
			this->GetPos(posPet);

			CMsgWalk msg;
			if(msg.Create(this->GetID(),this->GetDir()+8*::RandGet(31), 1))
				msg.Send();
		}
		break;
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
		}
		else
		{
			CMsgInteract msg;
			if(msg.Create(INTERACT_SHOOT, this->GetID(), m_Info.cmdProc.idTarget, m_Info.cmdProc.dwData))
				msg.Send();
		}
		m_Info.cmdProc.bData = true;
	}

	return bAcomplished;
}

void CPet::AttackAI_1()
{
	if (this->CheckEnemy (&g_objHero))
		m_idTarget = g_objHero.GetID ();
}

void CPet::AttackAI_2 ()
{
	OBJID id = g_objHero.GetIDMyTarget ();
	if (id != ID_NONE)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayer (id);
		if (pPlayer && id != this->GetID () && this->CheckEnemy (pPlayer))
			m_idTarget = id;
	}
}

void CPet::AttackAI_3 ()
{
	OBJID id = g_objHero.GetIDWhoAttackMe ();
	if (id != ID_NONE)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayer (id);
		if (pPlayer && !pPlayer->IsDead() && id != this->GetID () && this->CheckEnemy (pPlayer))
			m_idTarget = id;
	}
}

void CPet::AttackAI_4 ()
{
	int nAmount = g_objPlayerSet.GetPlayerAmount ();
	DWORD dwDistance = 100;
	CPlayer* pTarget = NULL;
	for(int i=0;i<nAmount;i++)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayerByIndex (i);

		if (pPlayer && pPlayer->IsMonster () && pPlayer->GetID () != this->GetID () && !pPlayer->IsGuard () && !pPlayer->IsDead())
		{
			DWORD dwTmpDistance = g_objHero.GetDistance (pPlayer);
			if (dwTmpDistance < dwDistance)
			{
				dwDistance = dwTmpDistance;
				pTarget = pPlayer;
			}
		}
	}
	if (pTarget && this->CheckEnemy (pTarget))
		m_idTarget = pTarget->GetID ();
}

void CPet::AttackAI_5 ()
{
	int nAmount = g_objPlayerSet.GetPlayerAmount ();
	DWORD dwDistance = 100;
	CPlayer* pTarget = NULL;
	for(int i=0;i<nAmount;i++)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayerByIndex (i);

		if (pPlayer && pPlayer->IsMonster () && pPlayer->GetID () != this->GetID () && !pPlayer->IsGuard () && !pPlayer->IsDead())
		{
			DWORD dwTmpDistance = this->GetDistance (pPlayer);
			if (dwTmpDistance < dwDistance)
			{
				dwDistance = dwTmpDistance;
				pTarget = pPlayer;
			}
		}
	}
	if (pTarget && this->CheckEnemy (pTarget))
		m_idTarget = pTarget->GetID ();
}

void CPet::AttackAI_6()
{
	int nAmount = g_objPlayerSet.GetPlayerAmount ();
	DWORD dwDistance = 100;
	CPlayer* pTarget = NULL;
	for(int i=0;i<nAmount;i++)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayerByIndex (i);

		if (pPlayer && pPlayer->TestStatus (USERSTATUS_DEFENCE3) && pPlayer->GetID () != this->GetID () && !pPlayer->IsGuard () && !pPlayer->IsDead())
		{
			DWORD dwTmpDistance = this->GetDistance (pPlayer);
			if (dwTmpDistance < dwDistance)
			{
				dwDistance = dwTmpDistance;
				pTarget = pPlayer;
			}
		}
	}
	if (pTarget && this->CheckEnemy (pTarget))
		m_idTarget = pTarget->GetID ();
}

void CPet::AttackAI_7()
{
	int nAmount = g_objPlayerSet.GetPlayerAmount ();
	DWORD dwDistance = 100;
	CPlayer* pTarget = NULL;
	for(int i=0;i<nAmount;i++)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayerByIndex (i);

		if (pPlayer && pPlayer->TestStatus (USERSTATUS_DEFENCE2) && pPlayer->GetID () != this->GetID () && !pPlayer->IsGuard () && !pPlayer->IsDead())
		{
			DWORD dwTmpDistance = this->GetDistance (pPlayer);
			if (dwTmpDistance < dwDistance)
			{
				dwDistance = dwTmpDistance;
				pTarget = pPlayer;
			}
		}
	}
	if (pTarget && this->CheckEnemy (pTarget))
		m_idTarget = pTarget->GetID ();
}

void CPet::AttackAI_8()
{
	int nAmount = g_objPlayerSet.GetPlayerAmount ();
	DWORD dwDistance = 100;
	CPlayer* pTarget = NULL;
	for(int i=0;i<nAmount;i++)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayerByIndex (i);

		if (pPlayer && pPlayer->TestStatus (USERSTATUS_TEAMLEADER) && pPlayer->GetID () != this->GetID () && !pPlayer->IsGuard () && !pPlayer->IsDead())
		{
			DWORD dwTmpDistance = this->GetDistance (pPlayer);
			if (dwTmpDistance < dwDistance)
			{
				dwDistance = dwTmpDistance;
				pTarget = pPlayer;
			}
		}
	}
	if (pTarget && this->CheckEnemy (pTarget))
		m_idTarget = pTarget->GetID ();
}

void CPet::AttackAI_9()
{
	if (m_idHeroTarget != ID_NONE)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayer (m_idHeroTarget);
		if (pPlayer && m_idHeroTarget != this->GetID () && this->CheckEnemy (pPlayer))
			m_idTarget = m_idHeroTarget;
	}
}
