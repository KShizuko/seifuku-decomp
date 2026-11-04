#ifndef _PET_H
#define _PET_H

#include "Player.h"
#include "Magic.h"
#include "3DRoleData.h"
class CPet	: public CPlayer
{
	friend struct PetProbe;
public:
	enum {_PET_IDLE, _PET_BEHAVIOR_READY, _PET_BEHAVIOR_FIND_HOST, _PET_ATTACK, _PET_MAGICATTACK};
	enum
	{
			_PET_SE_STATUS_ASSISTANT	=	1,
			_PET_SE_ATTACK_ASSISTANT	=	2,
			_PET_SE_REVENGE				=	3,
			_PET_SE_GUARD				=	4,
			_PET_SE_WARLIKE				=	5,
			_PET_SE_JUSTICAL_A			=	6,
			_PET_SE_JUSTICAL_B			=	7,
			_PET_SE_JUSTICAL_C			=	8,
	};
	enum
	{
			_PET_ENV_ELUDE				=	1,
			_PET_ENV_FEARLESS			=	2,
	};
	enum{_NORMAL_MAGIC = 1, _ALTITUTE_MAGIC = 2, _UITIMATE_MAGIC = 3, _COMBO_MAGIC = 4};
public:
	CPet();
	virtual ~CPet();

	int		GetAtkRange	(void);
	void	SetAlignPos(int nX, int nY);
	void	GetAlignPos(CMyPos& pos);
public:
	void	LockAttack		(CPlayer* pTarget);
	void	Walk			(int nPosX, int nPosY);
	void	Run				(int nPosX, int nPosY);
	void	Transfer		(int nPosX, int nPosY);
	void	Jump			(int nPosX, int nPosY);

	DWORD	SetCommand		(CCommand* pCommand);
	BOOL	ProcessCommand	();

	void	Process		(void* pInfo);
	void	ProcessAI	();

	BOOL	CanJump(CMyPos posTarGet);
	void	SetAIType(int nType);

public:
	static CPet*	CreateNewPet	(OBJID idPet, int nType);
public:
private:
	int		m_nBehaviorType;
public:
	BOOL	m_bFirstAtk;
private:
	DWORD	m_dwIdleEnd;
	OBJID	m_idTarget;
	PetInfo m_AIInfo;
	CMyPos	m_posAlign;
	OBJID	m_idHeroTarget;
	DWORD	m_dwHeroTargetTime;
	DWORD	m_dwLastJumpTime;
	DWORD	m_dwReserved4294;
public:
	void	SetHeroTarget(OBJID idTarger);
private:
	BOOL	SearchEnemy();
	void	SearchEnemy(int nMethod);

	void	AttackAI_1();
	void	AttackAI_2();
	void	AttackAI_3();
	void	AttackAI_4();
	void	AttackAI_5();
	void	AttackAI_6();
	void	AttackAI_7();
	void	AttackAI_8();
	void	AttackAI_9();
	BOOL	CheckEnemy(CPlayer* pPlayer);

	void	CheckTarget();
	BOOL	CheckTargetDie();
	BOOL	CheckTargetNotFound();
	BOOL	CheckTargetTooFar();
	BOOL	CheckTargetSpecialCommand();
	BOOL	CheckTargetElude();



	void	Idle(DWORD dwDelay);
private:
	void	ProcessAI_Idle();
	void	ProcessAI_Ready();
	void	ProcessAI_GenerateNewBehavior();
	void	ProcessAI_FindHost();
	void	ProcessAI_Attack();
};

#endif
