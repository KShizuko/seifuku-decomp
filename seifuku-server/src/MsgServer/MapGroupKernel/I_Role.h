#if !defined(I_ROLE_HEADER)
#define I_ROLE_HEADER

#if _MSC_VER > 1000
#pragma once
#endif

#include <math.h>
#include "define.h"
#include "GameObjSet.h"
#include "TimeOut.h"

enum { POSE_STAND=0, };

const int	ADJUST_PERCENT			= 30000;
const int	ADJUST_SET				= -30000;
const int	ADJUST_FULL				= -32768;
const int	DEFAULT_DEFENCE2		= 10000;

const int	SYNMEMSUM_LOWERLIMIT_TWO = 60;
const int	SYNMEMSUM_LOWERLIMIT_THREE = 150;
const int	SYNMEMSUM_LOWERLIMIT_FOUR = 380;	
const int SYNRANK_FOUR	= 4;
const int SYNRANK_THREE	= 3;
const int SYNRANK_TWO	= 2;
const int SYNRANK_ONE	= 1;

const DWORD TIME_FOOTPRINT	= 1000;

const int	TEAM_STATUS_REQ_ROLES	= 3;

const int	MAX_FRENZY_LIFE_PERCENT	= 70;

const int	ARROW_FLY_ALT					= 60;

enum {
		RANK_LEADER			= 100,
		RANK_SUBLEADER		= 80,
		RANK_TEAMLEADER		= 60,
		RANK_NEWBIE			= 50,
		RANK_RESERVED		= 10,
		RANK_NONE			= 0,

		RANK_SUB		= RANK_LEADER - RANK_SUBLEADER,
		RANK_PRE		= 20,
};

enum
{
	SYNMEM_LEVEL_ONE		= 1,
	SYNMEM_LEVEL_TWO		= 2,
	SYNMEM_LEVEL_THREE		= 3,
	SYNMEM_LEVEL_FOUR		= 4,
	SYNMEM_LEVEL_FIVE		= 5,		
};

enum 
{
		DIE_NORMAL			= 1,
		DIE_CRITICALHIT		= 2,
		DIE_MAGIC			= 5,
		DIE_FLOAT			= 4,
		DIE_ARROWSHOOT		= 7,
};
const DWORD	KEEPEFFECT_NORMAL		= 0x00000000;
enum { OWNER_NONE, OWNER_USER, OWNER_SYN, OWNER_NPC };
const DWORD	STATUS_POISON			= 0x00000004;
const DWORD	STATUS_WEAPONDAMAGE		= 0x00004000;
const DWORD	STATUS_WING				= 0x00400000;
const DWORD	STATUS_KEEPBOW			= 0x00800000;
const DWORD	STATUS_LUCKY_PRAY		= 0x02000000;
const DWORD	STATUS_LUCKY			= 0x04000000;
const DWORD	STATUS_ADJUST_MAGICDEF	= 0x00000200;
const DWORD	STATUS_ADJUST_DODGE		= 0x00200000;
const DWORD	STATUS_CRIME			= 0x00000002;
const DWORD	STATUS_LURKER			= 0x00020000;
const DWORD	KEEPEFFECT_RED			= 0x00004000;
const DWORD	KEEPEFFECT_DEEPRED		= 0x00008000;
const DWORD	KEEPEFFECT_XPFULL		= 0x00000010;
const DWORD	STATUS_SUPERMAN			= 0x00002000;
const DWORD	STATUS_TORNADO			= 0x00040000;
const DWORD	STATUS_DIE				= 0x00000001;
const DWORD	STATUS_NORMAL			= 0x00000000;
const DWORD	STATUS_PKVALUE			= 0x00000010;
const DWORD	KEEPEFFECT_GHOST		= 0x00000400;
const DWORD	STATUS_TEAMLEADER		= 0x00000008;
const DWORD	STATUS_SYNCRIME			= 0x00080000;
const DWORD	KEEPEFFECT_CRIME		= 0x00000001;
const DWORD	KEEPEFFECT_DIE			= 0x00000020;
const DWORD	KEEPEFFECT_TEAMLEADER	= 0x00000040;
const DWORD	KEEPEFFECT_NOT_CLEAR	= KEEPEFFECT_RED | KEEPEFFECT_DEEPRED | KEEPEFFECT_TEAMLEADER;
const DWORD	KEEPEFFECT_SYNCRIME		= 0x01000000;
const DWORD	KEEPEFFECT_LURKER		= 0x00400000;
const DWORD	KEEPEFFECT_WING			= 0x08000000;
const DWORD	STATUS_HITRATE			= 0x00000020;
const DWORD	STATUS_DEFENCE			= 0x00000040;
const DWORD	STATUS_ATTACK			= 0x00000080;
const DWORD	STATUS_SUPER_MATK		= 0x00000400;
const DWORD	STATUS_ATKRANGE			= 0x00000800;
const DWORD	STATUS_ATKSPEED			= 0x00010000;
const DWORD	STATUS_MAGICDAMAGE		= 0x00008000;
const DWORD	KEEPEFFECT_INSTEAD		= 0x20000000;
const DWORD	STATUS_STOP				= 0x01000000;
const DWORD	STATUS_REFLECT			= 0x00001000;
const DWORD	STATUS_REFLECTMAGIC		= 0x00100000;
const DWORD	STATUS_DETACH_BADLY		= 2;
const DWORD	STATUS_DETACH_ALL		= 8;
const DWORD	STATUS_DISAPPEARING		= 0x00000100;
struct StatusInfoStruct;
class IStatus
{
public:
	virtual OBJID	GetID()							PURE_VIRTUAL_FUNCTION_0
	virtual ULONG	ReleaseByOwner()				PURE_VIRTUAL_FUNCTION_0

	virtual bool		IsValid()							PURE_VIRTUAL_FUNCTION_0
	virtual int			GetPower()							PURE_VIRTUAL_FUNCTION_0
	virtual bool		GetInfo(StatusInfoStruct* pInfo)	PURE_VIRTUAL_FUNCTION_0
	virtual void		OnTimer(DWORD tCurr)				PURE_VIRTUAL_FUNCTION
	virtual bool		ChangeData(int nPower, int nSecs, int nTimes=0)		PURE_VIRTUAL_FUNCTION_0
	virtual bool		IncTime(int nMilliSecs, int nLimit)		PURE_VIRTUAL_FUNCTION_0
	virtual int			GetRemain()							PURE_VIRTUAL_FUNCTION_0
};

typedef	IGameObjSet<IStatus>		IStatusSet;
typedef	CGameObjSet<IStatus>		CStatusSet;

enum {	MOVEMODE_WALK=0,
		MOVEMODE_RUN,
		MOVEMODE_SHIFT,
		MOVEMODE_JUMP,
		MOVEMODE_TRANS,
		MOVEMODE_CHGMAP,
		MOVEMODE_JUMPMAGICATTCK,
		MOVEMODE_COLLIDE,
		MOVEMODE_SYNCHRO,
		MOVEMODE_TRACK,

		MOVEMODE_RUN_DIR0 = 20,

		MOVEMODE_RUN_DIR7 = 27,
};

const int _DELTA_X[9]={ 0, -1, -1, -1, 0, 1, 1, 1, 0 };
const int _DELTA_Y[9]={ 1, 1, 0, -1, -1, -1, 0, 1, 0 };
const int MAX_DIRSIZE = 8;
inline int GetDirByPos(int nFromX, int nFromY, int nToX, int nToY)
{
	if(nFromX < nToX)
	{
		if(nFromY < nToY)
			return 7;
		else if(nFromY > nToY)
			return 5;
		else
			return 6;
	}
	else if(nFromX > nToX)
	{
		if(nFromY < nToY)
			return 2;
		else if(nFromY > nToY)
			return 4;
		else
			return 3;
	}
	else
	{
		if(nFromY < nToY)
			return 0;
		else if(nFromY > nToY)
			return 4;
	}
	return MAX_DIRSIZE;
}
inline int		MaskLook(int look)							{ return look/10; }
inline int		MaskDir(int look)							{ return look%10; }
inline DWORD	MakeLookFace(int nMaskLook, int nMaskDir)	{ return (nMaskLook*10) + (nMaskDir%10); }
inline float	GetRadian(float posSourX, float posSourY, float posTargetX, float posTargetY)
{
	CHECKF(posSourX != posTargetX || posSourY != posTargetY);

	float PI = static_cast<float>(3.1415926535);
	float fDeltaX = posTargetX - posSourX;
	float fDeltaY = posTargetY - posSourY;
	float fDistance	= sqrt(fDeltaX*fDeltaX + fDeltaY*fDeltaY);
	CHECKF(fDeltaX <= fDistance && fDistance > 0);
	float fRadian = asin(fDeltaX / fDistance);

	return fDeltaY > 0 ? (PI/2 - fRadian) : (PI + fRadian + PI/2);
}

class IRole;
class IMapThing
{
public:
	virtual void	ReleaseByOwner()							PURE_VIRTUAL_FUNCTION
	virtual OBJID	GetID()										PURE_VIRTUAL_FUNCTION_0
	virtual int 	GetObjType()								PURE_VIRTUAL_FUNCTION_0
	virtual int		GetPosX()									PURE_VIRTUAL_FUNCTION_0
	virtual int		GetPosY()									PURE_VIRTUAL_FUNCTION_0
	virtual void	SendShow(IRole* pRole)						PURE_VIRTUAL_FUNCTION

	virtual bool	QueryObj(OBJID idObjType, void** ppv)				PURE_VIRTUAL_FUNCTION_0
	virtual bool	QueryRole(void** ppv)						PURE_VIRTUAL_FUNCTION_0
};

class IMapThing;
class IStatus;
class IRole
{
protected:
	IRole(){}
	~IRole(){}
public:
	virtual void	ReleaseByOwner		()						PURE_VIRTUAL_FUNCTION
public:
	virtual OBJID	GetID				()						PURE_VIRTUAL_FUNCTION_0
	virtual LPCTSTR	GetName				()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetLookFace			()						PURE_VIRTUAL_FUNCTION_0

	virtual DWORD	GetHair				()						PURE_VIRTUAL_FUNCTION_0
	virtual int 	GetPosX				()						PURE_VIRTUAL_FUNCTION_0
	virtual int 	GetPosY				()						PURE_VIRTUAL_FUNCTION_0
	virtual int 	GetDir				()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetEffect			(int nIndex)			PURE_VIRTUAL_FUNCTION_0
	virtual int 	GetPose				()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetLev				()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetLife				()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetMaxLife			()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetMinAtk			()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetMaxAtk			()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetAttack			()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetDef				()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetDefence			()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetDex				()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetDexterity		()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetDdg				()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetDodge			()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetMagicDef			()						PURE_VIRTUAL_FUNCTION_0
	virtual	DWORD	GetInterAtkRate		()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetIntervalAtkRate	()						PURE_VIRTUAL_FUNCTION_0
	virtual int		GetHelmetTypeID		()						PURE_VIRTUAL_FUNCTION_0
	virtual int		GetArmorTypeID		()						PURE_VIRTUAL_FUNCTION_0
	virtual int		GetWeaponRTypeID	()						PURE_VIRTUAL_FUNCTION_0
	virtual int 	GetWeaponLTypeID	()						PURE_VIRTUAL_FUNCTION_0
	virtual int 	GetMountTypeID		()						PURE_VIRTUAL_FUNCTION_0
	virtual int		GetSpriteTypeID		()						PURE_VIRTUAL_FUNCTION_0
	virtual int		GetMantleTypeID		()						PURE_VIRTUAL_FUNCTION_0
	virtual int		GetDistance			(int x, int y)			PURE_VIRTUAL_FUNCTION_0
	virtual int		GetDistance			(IMapThing* pTarget)	PURE_VIRTUAL_FUNCTION_0
	virtual CGameMap*	GetMap			()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetSynID			()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetSynRank			()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetSynRankShow		()						PURE_VIRTUAL_FUNCTION_0
	virtual bool	IsSimpleMagicAtk	()						PURE_VIRTUAL_FUNCTION_0
	virtual int 	GetSimpleMagicHitRate()						PURE_VIRTUAL_FUNCTION_0
	virtual UCHAR	GetNobilityRank		()						PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetExploit			()						PURE_VIRTUAL_FUNCTION_0
	virtual int		GetHelmetColor		()						PURE_VIRTUAL_FUNCTION_0
	virtual int		GetArmorColor		()						PURE_VIRTUAL_FUNCTION_0
	virtual int		GetShieldColor		()						PURE_VIRTUAL_FUNCTION_0

public:
	virtual bool	IsAlive				()						PURE_VIRTUAL_FUNCTION_0
	virtual bool	IsTalkEnable		()						PURE_VIRTUAL_FUNCTION_0
	virtual bool	IsGM				()						PURE_VIRTUAL_FUNCTION_0
	virtual bool	IsPM				()						PURE_VIRTUAL_FUNCTION_0
	virtual bool	IsMonster			()						PURE_VIRTUAL_FUNCTION_0
	virtual bool	IsDelThis			()						PURE_VIRTUAL_FUNCTION_0
	virtual bool	IsVirtuous()								PURE_VIRTUAL_FUNCTION_0
	virtual bool	IsEvil()									PURE_VIRTUAL_FUNCTION_0
	virtual bool	IsBowman()									PURE_VIRTUAL_FUNCTION_0
	virtual bool	IsShieldEquip()								PURE_VIRTUAL_FUNCTION_0
	virtual bool	IsWing()									PURE_VIRTUAL_FUNCTION_0

public:
	virtual bool	QueryObj			(OBJID idObjType, void** ppv)	PURE_VIRTUAL_FUNCTION_0
	virtual IMapThing*	QueryMapThing	()						PURE_VIRTUAL_FUNCTION_0
	virtual IRole*	FindAroundRole		(OBJID id)				PURE_VIRTUAL_FUNCTION_0
	virtual CUser*	QueryOwnerUser		()						PURE_VIRTUAL_FUNCTION_0

public:
	virtual void	SetDir				(int nDir)							PURE_VIRTUAL_FUNCTION
	virtual void	SetPose				(int nPose)							PURE_VIRTUAL_FUNCTION
	virtual	void	SetStatus			(int nStatus, bool bSynchro=true)						PURE_VIRTUAL_FUNCTION
	virtual	void	ClsStatus			(int nStatus, bool bSynchro=true)						PURE_VIRTUAL_FUNCTION
	virtual bool	AddAttrib			(int idxAttr, __int64 i64Data, int bSynchro, DWORD* pMask)	PURE_VIRTUAL_FUNCTION_0
	virtual void	ProcessOnMove		(int nMoveMode)						PURE_VIRTUAL_FUNCTION
	virtual void	GetFootPrint		(int& x, int& y)					PURE_VIRTUAL_FUNCTION
	virtual bool	SpendArrow			()									PURE_VIRTUAL_FUNCTION_0
	virtual	IStatus*	QueryStatus(OBJID idType)										PURE_VIRTUAL_FUNCTION_0
	virtual	IStatusSet*	QueryStatusSet()												PURE_VIRTUAL_FUNCTION_0
	virtual __int64	AwardBattleExp		(__int64 i64Exp, bool bGemEffect, int nReserved)	PURE_VIRTUAL_FUNCTION_0
	virtual int 	AdjustHitRate		(int nHitRate)									PURE_VIRTUAL_FUNCTION_0
	virtual int 	AdjustAttack		(int nAtk)										PURE_VIRTUAL_FUNCTION_0
	virtual int 	AdjustDefence		(int nDef)										PURE_VIRTUAL_FUNCTION_0
	virtual int 	AdjustBowDefence	(int nDef)										PURE_VIRTUAL_FUNCTION_0
	virtual int 	AdjustWeaponDamage	(IRole* pAtker, int nDamage)					PURE_VIRTUAL_FUNCTION_0
	virtual int 	AdjustMagicDamage	(IRole* pAtker, int nDamage)					PURE_VIRTUAL_FUNCTION_0
	virtual void	SetFightPause		(int nInterval)									PURE_VIRTUAL_FUNCTION
	virtual void	SaveInfo			()												PURE_VIRTUAL_FUNCTION

public:
	virtual void	BroadcastRoomMsg(CNetMsg* pMsg, bool bSendSelf = false)	PURE_VIRTUAL_FUNCTION
	virtual void	BroadcastRoomMsg(LPCTSTR szMsg, bool bSendSelf = false)	PURE_VIRTUAL_FUNCTION
	virtual bool	SendMsg(CNetMsg* pMsg)									PURE_VIRTUAL_FUNCTION_0
	virtual bool	SendSysMsg(const char* fmt, ...)						PURE_VIRTUAL_FUNCTION_0
	virtual void	OnTimer(time_t tCurr)									PURE_VIRTUAL_FUNCTION

public:
	virtual bool	MoveToward(int nDir)									PURE_VIRTUAL_FUNCTION_0
	virtual void	TransPos(int nPosX, int nPosY)							PURE_VIRTUAL_FUNCTION
	virtual void	JumpPos(int nPosX, int nPosY)							PURE_VIRTUAL_FUNCTION
	virtual void	ClrAttackTarget		()									PURE_VIRTUAL_FUNCTION
	virtual bool	SetAttackTarget(IRole* pTarget = NULL)					PURE_VIRTUAL_FUNCTION_0
	virtual bool	AutoSkillAttack(IRole* pTarget)							PURE_VIRTUAL_FUNCTION_0
	virtual bool	AdditionMagic(int nLifeLost, int nDamage)				PURE_VIRTUAL_FUNCTION_0
	virtual int		Attack(IRole* pTarget)									PURE_VIRTUAL_FUNCTION_0
	virtual int		BeAttack(IRole* pTarget, int nPower, bool bReflectEnable)		PURE_VIRTUAL_FUNCTION_0
	virtual int		BeAttackEx(IRole* pTarget, int nPower, bool bReflectEnable)	PURE_VIRTUAL_FUNCTION_0
	virtual bool	MagicAttack(int nType, OBJID idTarget, int x, int y)	PURE_VIRTUAL_FUNCTION_0
	virtual void	Kill(IRole* pTarget, DWORD dwDieWay, int nReserved)		PURE_VIRTUAL_FUNCTION
	virtual void	BeKill(IRole* pTarget, int nReserved)					PURE_VIRTUAL_FUNCTION
	virtual bool	IsAtkable(IRole* pTarget, bool bSendHint=false)			PURE_VIRTUAL_FUNCTION_0
	virtual bool	IsBeAtkable()											PURE_VIRTUAL_FUNCTION_0
	virtual int		GetAttackRange(int nTargetSizeAdd)						PURE_VIRTUAL_FUNCTION_0
	virtual int		GetSizeAdd()											PURE_VIRTUAL_FUNCTION_0
	virtual void	SendDamageMsg(OBJID idTarget, int nDamage)				PURE_VIRTUAL_FUNCTION
};

#endif
