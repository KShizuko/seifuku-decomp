#if !defined(AFX_NPC_H__6093FC28_BAC7_4C65_AF21_721F11F1C975__INCLUDED_)
#define AFX_NPC_H__6093FC28_BAC7_4C65_AF21_721F11F1C975__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "GameObj.h"
#include "Timeout.h"
#include "NpcType.h"
#include "I_MapData.h"
#include "I_Role.h"
#include "MsgWalk.h"
#include "NpcGenerator.h"
#include "AutoPtr.h"
#include "item.h"

const int	THINK_SECS					= 1;
const int	DORMANCY_SECS				= 60;
const int	SHORTWEAPON_RANGE_LIMIT		= 2;
const int	NPC_CLIMBCAP				= 26;
const int	GUARD_LEAVEDISTANCE			= 48;
const int	PKKILLER_JUMPDISTANCE		= 6;
const int	RANDOMMOVE_RATE				= 5;
const int	GUARD_RANDOMMOVE_RATE		= 5;
const int	SIMPLEMAGIC_RECRUIT			= 1020;

enum {	ATKUSER_LEAVEONLY				= 0,
		ATKUSER_PASSIVE					= 0x0001,
		ATKUSER_ACTIVE					= 0x0002,
		ATKUSER_RIGHTEOUS				= 0x0004,
		ATKUSER_GUARD					= 0x0008,
		ATKUSER_PPKER					= 0x0010,
		ATKUSER_JUMP					= 0x0020,
		ATKUSER_FIXED					= 0x0040,
		ATKUSER_FASTBACK				= 0x0080,
		ATKUSER_LOCKUSER				= 0x0100,
		ATKUSER_LOCKONE					= 0x0200,
		ATKUSER_ADDLIFE					= 0x0400,
		ATKUSER_EVIL_KILLER				= 0x0800,
		ATKUSER_WING					= 0x1000,
		ATKUSER_NEUTRAL					= 0x2000,
		ATKUSER_ROAR					= 0x4000,
		ATKUSER_NOESCAPE				= 0x8000,
		ATKUSER_EQUALITY				= 0x00010000,
};
const int	ATKUSER_NO_DORMANCY				= ATKUSER_LOCKUSER;

enum { OWNER_NONE, OWNER_USER, OWNER_SYN, OWNER_NPC };

class CNetMsg;
class CNpc : public CGameObj, INpc
{
public:
	CNpc();
	virtual ~CNpc();
	static CNpc*	CreateNew()			{ return new CNpc; }
	void			OnTimer();
	virtual bool	Create(OBJID idNpc, int nType, OBJID idMap, int nPosX, int nPosY, OBJID idGenerator, OBJID idOwner, int nOwnerType, int nData);
	void		ReleaseByOwner()		{ delete this; }
	IRole*		QueryRole()				{ return (IRole*)this; }
	IRoleAttr*	QueryRoleAttr()			{ return static_cast<IRoleAttr*>(this); }
	virtual CAutoLink<IRole>&	QueryLink(const CAutoLink<IRole>&)				{ return m_lnkThis; }
	template<TYPENAME T>
		T*		QueryInterface(T** ppT=NULL)	{ if(ppT) *ppT = this; return this; }
	virtual void*	QueryInterface(int id, void** ppT)	{ void* p=NULL; if(id==GUID_OF(CNpc)) p=this; if(ppT) *ppT=p; return p; }

public:
	OBJID		GetID()				{ return m_id; }
	OBJID		GetType()			{ return m_pType->GetID(); }
	LPCTSTR		GetStr(NPCTYPEDATA idx)	{ return m_pType->GetStr(idx); }
	int			GetInt(NPCTYPEDATA idx)	{ return m_pType->GetInt(idx); }
public:
	int			GetLife()			{ return m_nCurrLife; }
	int			GetPower()			{ return m_nCurrPower; }
	OBJID		GetMapID();
	CGameMap*	GetMap()			{ return m_pMap; }
	int			GetPosX()			{ return m_nPosX; }
	int			GetPosY()			{ return m_nPosY; }
	int			GetDir()			{ return m_nDir; }
	IGenerator* 	GetGenerator()			{ return m_pGenerator; }
	int			GetAttackRange(int nSizeAdd)			{ return GetInt(NPCTYPEDATA_ATKRANGE)+(GetSizeAdd()+nSizeAdd+1)/2; }
	int			GetSizeAdd()		{ return GetInt(NPCTYPEDATA_SIZEADD); }
	DWORD		GetEffect()			{ return m_i64Effect; }
	int			GetOwnerID()		{ return m_idOwner; }
public:
	bool		IsMonster()			{ return !IsSynPet() && !IsGuard() && !IsPkKiller() && !IsEvilKiller(); }
	bool		IsSynPet()			{ return m_nOwnerType == OWNER_SYN; }
	bool		IsOpposedSyn(OBJID idSyn);
	bool		IsLurker()			{ return 0; }
	bool		IsDormancy()		{ return (m_nActMode == MODE_DORMANCY); }
	bool		IsAlive()			{ return !m_bDie && m_nCurrLife > 0 && (GetEffect()&KEEPEFFECT_DIE) == 0; }
	bool		IsFarWeapen()		{ return GetInt(NPCTYPEDATA_ATKRANGE) > SHORTWEAPON_RANGE_LIMIT; }
	bool		IsRighteous()		{ return (GetInt(NPCTYPEDATA_ATKUSER) & ATKUSER_RIGHTEOUS) != 0; }
	bool		IsEvil()			{ return (GetInt(NPCTYPEDATA_ATKUSER) & (ATKUSER_RIGHTEOUS|ATKUSER_NEUTRAL)) == 0; }
	bool		IsGuard()			{ return (GetInt(NPCTYPEDATA_ATKUSER) & ATKUSER_GUARD) != 0; }
	bool		IsPkKiller()		{ return (GetInt(NPCTYPEDATA_ATKUSER) & ATKUSER_PPKER) != 0; }
	bool		IsActiveEnable()	{ return (GetEffect() & (KEEPEFFECT_NOT_ACTIVE)) == 0; }
	bool		IsMoveEnable()		{ return IsActiveEnable() && (IsWalkEnable() || IsJumpEnable()); }
	bool		IsWalkEnable()		{ return (GetInt(NPCTYPEDATA_ATKUSER) & ATKUSER_FIXED) == 0; }
	bool		IsJumpEnable()		{ return (GetInt(NPCTYPEDATA_ATKUSER) & ATKUSER_JUMP) != 0; }
	bool		IsFastBack()		{ return (GetInt(NPCTYPEDATA_ATKUSER) & ATKUSER_FASTBACK) != 0; }
	bool		IsLockUser()		{ return (GetInt(NPCTYPEDATA_ATKUSER) & ATKUSER_LOCKUSER) != 0; }
	bool		IsLockOne()			{ return (GetInt(NPCTYPEDATA_ATKUSER) & ATKUSER_LOCKONE		) != 0; }
	bool		IsAddLife()			{ return (GetInt(NPCTYPEDATA_ATKUSER) & ATKUSER_ADDLIFE		) != 0; }
	bool		IsEvilKiller()		{ return (GetInt(NPCTYPEDATA_ATKUSER) & ATKUSER_EVIL_KILLER	) != 0; }
	bool		IsDormancyEnable()	{ return (GetInt(NPCTYPEDATA_ATKUSER) & ATKUSER_NO_DORMANCY	) == 0; }
	bool		IsWing()			{ return (GetInt(NPCTYPEDATA_ATKUSER) & ATKUSER_WING		) != 0; }
	bool		IsBowman()			{ int nType=GetInt(NPCTYPEDATA_WEAPONR_TYPE); return ((nType%10000000)/100000) == ITEMSORT_WEAPON2 && ((nType%100000)/1000)*1000 == DWEAPON_BOW; }
	bool		IsSimpleMagicAtk()	{ return GetInt(NPCTYPEDATA_MAGIC_TYPE) != ID_NONE && GetInt(NPCTYPEDATA_MAGIC_TYPE) != SIMPLEMAGIC_RECRUIT; }
	bool		IsCloseAttack()		{ return !IsBowman() && !IsSimpleMagicAtk(); }
	virtual bool		IsBlockNpc()								{ return (GetEffect() & KEEPEFFECT_NOT_BLOCKNPC) == 0; }
	bool		IsEscapeEnable()	{ return (GetInt(NPCTYPEDATA_ATKUSER) & (ATKUSER_NOESCAPE)) == 0; }
public:
	void		JumpPos(int nPosX, int nPosY, int nDir);
	void		SetDir(int nDir)				{ m_nDir = nDir; }
	bool		IsAttackEnable(IRole* pTarget);

public:
	void		SetLife(int nLife)				{ m_nCurrLife = nLife; }
	void		SetPower(int nPower)			{ m_nCurrPower = nPower; }
	void		SetEffect(DWORD dwEffect)		{ m_i64Effect = dwEffect; }
	void		SetSizeAdd(int nSizeAdd)		{}
	void		SetDie()						{ if(m_bDie) return; m_bDie = true; m_pGenerator->DeleteNpc(this->GetID()); }

public:
	bool		PathMove(int nMode);
	bool		MoveForward(int nDir, int nMode);
	bool		SynchroPos(int nSourX, int nSourY, int nTargX, int nTargY);
	bool		KickBack(int nTargX, int nTargY);
	void		SetPos(int nPosX, int nPosY);
	void		Attack();
	void		AttackOnce()					{ m_bAttackFlag = true; }
	bool		CheckTarget();
	bool		FindNewTarget();
	bool		JumpBlock(int x, int y, int dir);
	bool		FarJump(int x, int y, int dir);
	void		LockMove(int nMilliSecs);

public:
	bool		ChangeMode(int nMode);
	void		WakeUp()						{ m_tDormancy.Update(); if(IsDormancy()) ChangeMode(MODE_IDLE); }
	void		BeAttack(IRole* pRole);
	void		BeKill(IRole* pRole)			{ ChangeMode(CNpc::MODE_IDLE); }
	bool		ProcessEscape();
	bool		ProcessAttack();
	bool		ProcessForward();
	bool		ProcessIdle();
	bool		CheckDormancy();

public:
	bool		SendMsg(CNetMsg* pMsg);
	bool		SendMsg(LPCTSTR szMsg, LPCTSTR szReceiver = NULL);

public:
	int			Distance(int x, int y)			{ return __max(abs(x-GetPosX()), abs(y-GetPosY())); }

public:
	void		TalkToMe(LPCTSTR szSender, LPCTSTR szWords, LPCTSTR szEmotion);

protected:
	OBJID		m_id;
	CNpcType*	m_pType;

protected:
	int			m_nCurrLife;
	int			m_nCurrPower;
	CAutoLink<IRole>	m_lnkThis;

public:
	enum NPC_MODE { MODE_DORMANCY=0, MODE_IDLE, MODE_WALK, MODE_GOTO, MODE_TALK, MODE_FOLLOW, MODE_ESCAPE, MODE_FORWARD, MODE_ATTACK,  };
protected:
	int			m_nActMode;

	OBJID		m_idActTarget;
	CTimeOut	m_tAction;

protected:
	int			m_nPosX, m_nPosY;
	DWORD		m_i64Effect;
	int			m_nDir;
	int			m_nPose;
	CMapPtr		m_pMap;
	CTimeOut	m_tDormancy;

protected:
	CTimeOutMS	m_tMove;
	OBJID		m_idAtkMe;
	bool		m_bAtkFirst;
	bool		m_bAttackFlag;

protected:
	int			m_nOwnerType;
	OBJID		m_idOwner;

protected:
	CAutoPtr<IGenerator> 	m_pGenerator;
	bool		m_bDie;

protected:
	bool		FindPath(int nEscapeSteps = 0);
	bool		FindPath(int x, int y, int nEscapeSteps = 0);
	bool		DetectPath(int nNoDir);
	bool		TestPath(int nDir);
	void		ClearPath()				{ m_nPathLen = 0; }
protected:
	POINT		m_posTarget;
	bool		m_bAheadPath;
	int			m_nPathLen;
	char		m_bufPath[_MAX_PATHSTEP];
};

#endif
