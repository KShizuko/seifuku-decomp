#if !defined(AFX_MAPTRAP_H__1A117AC2_A313_4179_8F93_3D308C29F0EB__INCLUDED_)
#define AFX_MAPTRAP_H__1A117AC2_A313_4179_8F93_3D308C29F0EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>
#include "common.h"
#include "gameObj.h"
#include "define.h"
#include "Role.h"
#include "mycom.h"
#include "i_mydb.h"
#include "timeout.h"
#include "GameData.h"
#include "T_SingleMap2.h"
#include "gameMap.h"
#include "UserTable.h"
#include "autolink.h"

struct TRAP_INFO
{
	OBJID		id;
	USHORT		usType;
	USHORT		usLook;
	OBJID		idOwner;
	OBJID		idMap;
	USHORT		usPosX;
	USHORT		usPosY;
};

enum TRAPDATA
{
	TRAPDATA_ID_=0,
	TRAPDATA_TYPE=1,
	TRAPDATA_LOOK,
	TRAPDATA_OWNERID,
	TRAPDATA_MAPID,
	TRAPDATA_POS_X,
	TRAPDATA_POS_Y,
	TRAPDATA_DATA,
};

enum TRAP_SORT
{
	TRAPSORT_SYSTRAP	=1,
	TRAPSORT_TRAPSORT_	=10,

	TRAPSORT_FIREWALL	=10,
};

enum ATK_MODE
{
	ATKMODE_USER		= 1,
	ATKMODE_MONSTER		= 2,
	ATKMODE_NOATTACK	= 4,
};

class CUser;
class CBattleSystem;
class CMapTrap : public CGameObj, public CRole
{
	COM_STYLE(CMapTrap)
protected:
	CMapTrap();
	virtual ~CMapTrap();
	bool	Init();

public:
	static CMapTrap* CreateNew(PROCESS_ID idProcess, TRAP_INFO* pInfo, CUser* pUser=NULL);
	static CMapTrap* CreateNew(PROCESS_ID idProcess, IRecord* pRes);
	bool		ChangeType(int nNewType);
	bool		ChangeLook(int nNewLook);

	IRole*			QueryRole()									{ return (IRole*)this; }
	virtual		bool		FindAroundObj	(OBJID idObjType, OBJID idObj, void** pp)
																{ return GetMap()->QueryObj(GetPosX(), GetPosY(), idObjType, idObj, pp); }
public:
	virtual OBJID	GetID()										{ return m_info.id; }
	virtual int 	GetObjType()								{ return CGameObj::GetObjType(); }
	virtual int		GetPosX()									{ return m_info.usPosX; }
	virtual int		GetPosY()									{ return m_info.usPosY; }
	virtual void	SendShow(IRole* pRole);

public:
	virtual LPCTSTR	GetName				()						{ return "trap"; }

	virtual DWORD	GetLookFace			()						{ return m_info.usLook; }
	virtual DWORD	GetHair				()						{ return 0; }
	virtual int 	GetDir				()						{ return GetLookFace() % MAX_DIRSIZE; }
	virtual DWORD	GetEffect			(int nIndex)			{ return 0; }
	virtual int 	GetPose				()						{ return 0; }
	virtual DWORD	GetLev				()						{ return m_pType->GetInt(TRAPTYPEDATA_LEVEL); }
	virtual DWORD	GetLife				()						{ return 0; }
	virtual DWORD	GetMaxLife			()						{ return 0; }

	virtual DWORD	GetMinAtk			()						{ return m_pType->GetInt(TRAPTYPEDATA_ATK_MIN); }
	virtual DWORD	GetMaxAtk			()						{ return m_pType->GetInt(TRAPTYPEDATA_ATK_MAX); }
	virtual DWORD	GetAttack			()						{ return GetMinAtk() + ::RandGet(GetMaxAtk()-GetMinAtk()); }
	virtual DWORD	GetDef				()						{ return 0; }
	virtual DWORD	GetDefence			()						{ return 0; }

	virtual DWORD	GetDex				()						{ return m_pType->GetInt(TRAPTYPEDATA_DEX); }
	virtual DWORD	GetDexterity		()						{ return m_pType->GetInt(TRAPTYPEDATA_DEX); }
	virtual DWORD	GetDdg				()						{ return 0; }
	virtual DWORD	GetDodge			()						{ return 0; }
	virtual DWORD	GetMagicDef			()						{ return 0; }

	virtual	DWORD	GetInterAtkRate		()						{ return m_pType->GetInt(TRAPTYPEDATA_SPEED); }
	virtual DWORD	GetIntervalAtkRate	()						{ return m_pType->GetInt(TRAPTYPEDATA_SPEED); }

	virtual int		GetHelmetTypeID		()						{ return 0; }
	virtual int		GetArmorTypeID		()						{ return 0; }
	virtual int		GetWeaponRTypeID	()						{ return 0; }
	virtual int 	GetWeaponLTypeID	()						{ return 0; }
	virtual int 	GetMountTypeID		()						{ return 0; }
	virtual int		GetSpriteTypeID		()						{ return 0; }
	virtual int		GetMantleTypeID		()						{ return 0; }

	virtual int		GetDistance			(int x, int y)			{ return __max(abs(GetPosX() - x), abs(GetPosY() - y)); }
	virtual int		GetDistance			(IMapThing* pTarget)	{ return __max(abs(GetPosX() - pTarget->GetPosX()), abs(GetPosY() - pTarget->GetPosY())); }
	virtual CGameMap*	GetMap			()						{ CHECKF(m_pMap); return m_pMap; }
	virtual DWORD	GetSynID			()						{ return ID_NONE; }
	virtual DWORD	GetSynRank			()						{ return RANK_NONE; }
	virtual DWORD	GetSynRankShow		()						{ return RANK_NONE; }

	virtual bool	IsSimpleMagicAtk	()						{ return m_pType->GetInt(TRAPTYPEDATA_MAGICTYPE) != 0; }
	virtual int 	GetSimpleMagicHitRate()						{ return m_pType->GetInt(TRAPTYPEDATA_MAGICHITRATE); }
	virtual UCHAR	GetNobilityRank		()						{ return 0; }
	virtual DWORD	GetExploit			()						{ return 0; }
	virtual int		GetHelmetColor		()						{ return 0; }
	virtual int		GetArmorColor		()						{ return 0; }
	virtual int		GetShieldColor		()						{ return 0; }

public:
	virtual bool	IsAlive				()						{ return !m_bDie; }
	virtual bool	IsTalkEnable		()						{ return false; }
	virtual bool	IsGM				()						{ return false; }
	virtual bool	IsPM				()						{ return false; }
	virtual bool	IsMonster			()						{ return false; }
	virtual bool	IsDelThis			()						{ return m_bDie; }
	virtual bool	IsVirtuous			()						{ return false; }
	virtual bool	IsEvil				()						{ return true; }
	virtual bool	IsBowman			()						{ return false; }
	virtual bool	IsShieldEquip		()						{ return false; }
	virtual bool	IsWing				()						{ return false; }

	virtual bool	QueryObj(OBJID idObjType, void** ppv)		{ if(idObjType==OBJ_TRAP) return *ppv=this,true; return *ppv=NULL,false; }
	virtual IMapThing*	QueryMapThing	()						{ return (IMapThing*)this; }
	virtual IRole*	FindAroundRole		(OBJID id)				{ return GetMap()->QueryRole(GetPosX(), GetPosY(), id); }
	virtual bool	QueryRole(void** ppv)						{ return *ppv=QueryRole(),true; }
	virtual CUser*	QueryOwnerUser		()						{ if(m_pUser) return m_pUser; return NULL; }

public:
	virtual void	SetDir				(int nDir)							{}
	virtual void	SetPose				(int nPose)							{}
	virtual	void	SetStatus			(int nStatus, bool bSynchro=true)	{}
	virtual	void	ClsStatus			(int nStatus, bool bSynchro=true)	{}
	virtual bool	AddAttrib			(int idxAttr, __int64 i64Data, int bSynchro, DWORD* pMask)	{ return false; }
	virtual void	ProcessOnMove		(int nMoveMode)						{ ASSERT(!"move"); }
	virtual void	GetFootPrint		(int& nPosX, int& nPosY)			{ nPosX = GetPosX(); nPosY = GetPosY(); }
	virtual bool	SpendArrow			()									{ return true; }
	virtual	IStatus*	QueryStatus(OBJID idType)							{ return NULL; }
	virtual	IStatusSet*	QueryStatusSet()									{ return m_setStatus; }
	virtual __int64	AwardBattleExp		(__int64 i64Exp, bool bGemEffect, int nReserved)	{ return 0; }
	virtual int 	AdjustHitRate		(int nHitRate)						{ return nHitRate; }
	virtual int 	AdjustAttack		(int nAtk)							{ return nAtk; }
	virtual int 	AdjustDefence		(int nDef)							{ return nDef; }
	virtual int 	AdjustBowDefence	(int nDef)							{ return nDef; }
	virtual int 	AdjustWeaponDamage	(IRole* pAtker, int nDamage)		{ return nDamage; }
	virtual int 	AdjustMagicDamage	(IRole* pAtker, int nDamage)		{ return nDamage; }
	virtual int 	AdjustDodge			(int nDodge)						{ return nDodge; }
	virtual void	SetFightPause		(int nInterval)						{}
	virtual void	SaveInfo			()									{}
public:
	virtual void	BroadcastRoomMsg(CNetMsg* pMsg, bool bSendSelf = false);
	virtual void	BroadcastRoomMsg(LPCTSTR szMsg, bool bSendSelf = false);
	virtual bool	SendMsg(CNetMsg* pMsg)									{ return true; }
	virtual bool	SendSysMsg(const char* fmt, ...)						{ return true; }
	virtual void	OnTimer(time_t tCurr);
public:
	virtual bool	MoveToward(int nDir)									{ return 0; }
	virtual void	TransPos(int nPosX, int nPosY)							{ ASSERT(!"error"); }
	virtual void	JumpPos(int nPosX, int nPosY)							{ ASSERT(!"error"); }

public:
	virtual void	ClrAttackTarget		()									{}
	virtual bool	SetAttackTarget(IRole* pTarget = NULL)					{ return true; }
	virtual bool	AutoSkillAttack(IRole* pTarget)							{ return false; }
	virtual bool	AdditionMagic(int nLifeLost, int nDamage)				{ return false; }
	virtual int		Attack(IRole* pTarget);
	virtual int		BeAttack(IRole* pTarget, int nPower, bool bReflectEnable)		{ return 0; }
	virtual int		BeAttackEx(IRole* pTarget, int nPower, bool bReflectEnable)	{ return 0; }
	virtual bool	MagicAttack(int nType, OBJID idTarget, int x, int y)	{ ASSERT(!"MagicAttack"); return 0; }
	virtual void	Kill(IRole* pTarget, DWORD dwDieWay, int nReserved);
	virtual void	BeKill(IRole* pTarget, int nReserved)					{ ASSERT(!"error"); }
	virtual bool	IsAtkable(IRole* pTarget, bool bSendHint=false);
	virtual bool	IsBeAtkable()											{ return false; }
	virtual int		GetAttackRange(int nTargetSizeAdd)						{ return 999; }
	virtual int		GetSizeAdd()											{ return 0; }
	virtual void	SendDamageMsg(OBJID idTarget, int nDamage);

public:
	bool			IsDeleted()							{ return m_bDie; }
	bool			IsTrapSort()						{ return m_pType->GetInt(TRAPTYPEDATA_SORT) < TRAPSORT_TRAPSORT_; }
	bool			IsAutoSort()						{ return !IsTrapSort(); }
	static bool		IsDynaTrapID(OBJID id)				{ return id>=MAGICTRAPID_FIRST && id<=MAGICTRAPID_LAST; }
	int				GetType()							{ return m_info.usType; }

public:
	bool	DelTrap();
	void	TrapAttack(IRole* pTarget);
protected:
	void	MagicAttack();

protected:
	void	ProcessFireWall();

protected:
	PROCESS_ID		m_idProcess;
	TRAP_INFO		m_info;
	CTimeOutMS		m_tFight;
	bool			m_bDie;
	int				m_nTimes;

	CAutoLink<CUser>	m_pUser;
	IStatusSet*		m_setStatus;
	CGameMap*		m_pMap;
	CTrapTypeData*	m_pType;
	CBattleSystem*	m_pBattleSystem;

	MYHEAP_DECLARATION(s_heap)
};

#endif
