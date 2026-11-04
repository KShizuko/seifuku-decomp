#if !defined(AFX_AINPC_H__52682938_5D82_4101_B888_6A4A3F30EB51__INCLUDED_)
#define AFX_AINPC_H__52682938_5D82_4101_B888_6A4A3F30EB51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "define.h"
#include "gameobj.h"
#include "GameBlock.h"
#include "Timeout.h"
#include "Item.h"
#include "battlesystem.h"
#include "Myheap.h"
#include "GameMap.h"
#include "StatusMore.h"
#include "protocol.h"
#include "UserTable.h"
#include "autolink.h"
#include "maptrap.h"
#include "Role.h"
#include "Magic.h"
enum {	ATKUSER_LEAVEONLY				= 0,
		ATKUSER_PASSIVE					= 0x01,
		ATKUSER_ACTIVE					= 0x02,
		ATKUSER_RIGHTEOUS				= 0x04,
		ATKUSER_GUARD					= 0x08,
		ATKUSER_PPKER					= 0x10,
		ATKUSER_JUMP					= 0x20,
		ATKUSER_FIXED					= 0x40,
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

const int	MONSTERDROPITEM_RANGE		= 2;
const int	NPCDIEDELAY_SECS			= 10;
const int	SIMPLEMAGIC_RECRUIT			= 1005;
const OBJID	TYPE_TASK_MONSTER			= 3000;
const bool	DEL_NOW						= true;

enum PETDATA{
		PETDATA_OWNERID=1,
		PETDATA_OWNERTYPE,
		PETDATA_GENID,
		PETDATA_NAME,
		PETDATA_TYPE,
		PETDATA_LIFE,
		PETDATA_MANA,
		PETDATA_MAPID,
		PETDATA_RECORD_X,
		PETDATA_RECORD_Y,
		PETDATA_DATA,
};

enum MONSTER_NAME_TYPE{
	NAME_GREEN,
	NAME_WHITE,
	NAME_RED,
	NAME_BLACK,
};

extern char	szPetTable[];
typedef		CGameData<PETDATA, szPetTable, szID>	CPetData;

class CUser;
class CMonster : public CGameObj, public CRole
{
protected:
	CMonster();
	virtual ~CMonster();
public:
	static CMonster*	CreateNew() 	{ return new CMonster; }

	bool	Create(PROCESS_ID idProcess, CNpcType* pType, const struct ST_CREATENEWNPC* pInfo, LPCTSTR pszName=NULL);
	bool	Create(PROCESS_ID idProcess, CNpcType* pType, const struct ST_CREATENEWNPC* pInfo, CUser* pUser);
	virtual void	ReleaseByOwner()					{ delete this; }

	virtual		bool		FindAroundObj	(OBJID idObjType, OBJID idObj, void** pp)		{ return GetMap()->QueryObj(GetPosX(), GetPosY(), idObjType, idObj, pp); }
	virtual		IRole*		FindAroundRole	(OBJID id)		{ return GetMap()->QueryRole(GetPosX(), GetPosY(), id); }

	virtual CUser*		QueryOwnerUser		()				{ if(IsCallPet()) return m_pOwner; return NULL; }
protected:
	virtual bool		QueryRole(void** ppv)	{ return *ppv=(IRole*)this,true; }
	virtual bool		QueryObj(OBJID idObjType, void** ppv)			{ if(idObjType == OBJ_MONSTER) return *ppv=this,true; return *ppv=NULL,false; }
public:
	virtual IMapThing*	QueryMapThing()			{ return (IMapThing*)this; }
	IRole*				QueryRole()				{ return (IRole*)this; }

public:
	virtual LPCTSTR	GetName				()				{ return m_pType->GetStr(NPCTYPEDATA_NAME); }
	virtual LPCTSTR	GetTitle()							{ return TITILE_NONE; }

	virtual DWORD 	GetHair				()				{ return 0; }
	virtual int 	GetPosX				()				{ return m_nPosX; }
	virtual int 	GetPosY				()				{ return m_nPosY; }
	virtual int 	GetDir				()				{ return m_nDir; }
	virtual DWORD	GetEffect			(int nIndex)	{ return m_dwEffect; }
	virtual int 	GetPose				()				{ return m_nPose; }
	virtual DWORD	GetLev				()				{ return m_pType->GetInt(NPCTYPEDATA_LEVEL); }

	virtual	DWORD	GetLife				()				{ return m_nCurrLife; }
	virtual DWORD	GetMaxLife			()				{ return m_pType->GetInt(NPCTYPEDATA_LIFE); }
	virtual DWORD	GetMinAtk			();
	virtual DWORD	GetMaxAtk			();
	virtual DWORD	GetAttack			()				{ return (m_pType->GetInt(NPCTYPEDATA_ATTACKMIN)+m_pType->GetInt(NPCTYPEDATA_ATTACKMAX))/2; }
	virtual DWORD	GetDef				();
	virtual DWORD	GetDefence			()				{ return m_pType->GetInt(NPCTYPEDATA_DEFENCE); }
	virtual DWORD	GetDefence2			()				{ return m_pType->GetInt(NPCTYPEDATA_DEFENCE2); }
	virtual DWORD	GetDex				();
	virtual DWORD	GetDexterity		()				{ return m_pType->GetInt(NPCTYPEDATA_DEXTERITY); }
	virtual DWORD	GetDdg				();
	virtual DWORD	GetDodge			()				{ return m_pType->GetInt(NPCTYPEDATA_DODGE); }
	virtual DWORD	GetMagicDef			();
	virtual	DWORD	GetInterAtkRate		();
	virtual	DWORD	GetIntervalAtkRate	()				{ return m_pType->GetInt(NPCTYPEDATA_ATKSPEED); }
	virtual __int64	AwardBattleExp		(__int64 i64Exp, bool bGemEffect, int nReserved);
	virtual int 	AdjustHitRate		(int nHitRate)	{ IStatus* pStatus = QueryStatus(STATUS_HITRATE); if(pStatus) return ::CutTrail(0, AdjustData(nHitRate, pStatus->GetPower())); return nHitRate; }
	virtual int 	AdjustAttack		(int nAtk)		{ IStatus* pStatus = QueryStatus(STATUS_ATTACK); if(pStatus) return ::CutTrail(0, AdjustData(nAtk, pStatus->GetPower())); return nAtk; }
	virtual int 	AdjustDefence		(int nDef)		{ IStatus* pStatus = QueryStatus(STATUS_DEFENCE); if(pStatus) return ::CutTrail(0, AdjustData(nDef, pStatus->GetPower())); return nDef; }
	virtual int 	AdjustBowDefence	(int nDef)		{ IStatus* pStatus = QueryStatus(STATUS_SUPER_MATK); if(pStatus) return ::CutTrail(0, AdjustData(nDef, pStatus->GetPower())); return nDef; }
	virtual int 	AdjustWeaponDamage	(IRole* pAtker, int nDamage)	{ IStatus* pStatus = QueryStatus(STATUS_WEAPONDAMAGE); if(pStatus) return ::CutTrail(0, AdjustData(nDamage, pStatus->GetPower())); return ::MulDiv(nDamage, GetDefence2(), DEFAULT_DEFENCE2); }
	virtual int 	AdjustMagicDamage	(IRole* pAtker, int nDamage)	{ IStatus* pStatus = QueryStatus(STATUS_MAGICDAMAGE); if(pStatus) return ::CutTrail(0, AdjustData(nDamage, pStatus->GetPower())); return ::MulDiv(nDamage, GetDefence2(), DEFAULT_DEFENCE2); }
	virtual void	SetFightPause		(int nInterval)			{ m_nFightPause = nInterval; }

	virtual DWORD	GetLookFace()						{ return m_pType->GetInt(NPCTYPEDATA_LOOKFACE); }

	virtual int		GetHelmetTypeID		()				{ return m_pType->GetInt(NPCTYPEDATA_HELMET_TYPE); }
	virtual int		GetArmorTypeID		()				{ return m_pType->GetInt(NPCTYPEDATA_ARMOR_TYPE); }
	virtual int 	GetWeaponRTypeID	()				{ return m_pType->GetInt(NPCTYPEDATA_WEAPONR_TYPE); }
	virtual int 	GetWeaponLTypeID	()				{ return m_pType->GetInt(NPCTYPEDATA_WEAPONL_TYPE); }
	virtual int 	GetMountTypeID		()				{ return 0; }
	virtual int		GetSpriteTypeID		()				{ return 0; }
	virtual int		GetMantleTypeID		()				{ return 0; }
	virtual int		GetDistance			(int x, int y)			{ return __max(abs(GetPosX() - x), abs(GetPosY() - y)); }
	virtual int		GetDistance			(IMapThing* pTarget)	{ if(!pTarget) return 255; return __max(abs(GetPosX() - pTarget->GetPosX()), abs(GetPosY() - pTarget->GetPosY())); }

	virtual void	BroadcastRoomMsg(CNetMsg* pMsg, bool bSendSelf = false);
	virtual void	BroadcastRoomMsg(LPCTSTR szMsg, bool bSendSelf = false);
	virtual void	BroadcastMapMsg(CNetMsg* pMsg, bool bSendSelf = false);
	virtual void	BroadcastMapMsg(LPCTSTR szMsg, bool bSendSelf = false);
	virtual bool	SendMsg(CNetMsg* pMsg);
	virtual bool	SendSysMsg(const char* fmt, ...);
	virtual void	OnTimer(time_t tCurr);

	virtual DWORD	GetSynID			()				{ return m_pData && m_pData->GetInt(PETDATA_OWNERTYPE)==OWNER_SYN ? m_pData->GetInt(PETDATA_OWNERID) : ID_NONE; }
	virtual DWORD	GetSynRank			()				{ return RANK_NONE; }
	virtual DWORD	GetSynRankShow		()				{ return RANK_NONE; }

	virtual bool	IsSimpleMagicAtk	()				{ return m_pType->GetInt(NPCTYPEDATA_MAGIC_TYPE) != ID_NONE && m_pType->GetInt(NPCTYPEDATA_MAGIC_TYPE) != SIMPLEMAGIC_RECRUIT && m_pType->GetInt(NPCTYPEDATA_MAGIC_HITRATE) !=0; }
	virtual int 	GetSimpleMagicHitRate()				{ return m_pType->GetInt(NPCTYPEDATA_MAGIC_HITRATE); }
	int 			GetRecruitMagicPercent()			{ return m_pType->GetInt(NPCTYPEDATA_MAGIC_HITRATE); }
	virtual UCHAR	GetNobilityRank		()				{ return 0; }
	virtual DWORD	GetExploit			()				{ return 0; }
	virtual int		GetHelmetColor		()				{ return 0; }
	virtual int		GetArmorColor		()				{ return 0; }
	virtual int		GetShieldColor		()				{ return 0; }

public:
	bool	EnterMapGroup();
	bool	LeaveMapGroup();
	bool	SendSelfToBlock();
	bool	SendLeaveFromBlock();
	virtual bool	MoveToward(int nDir);
	virtual void	TransPos(int nPosX, int nPosY);
	virtual void	JumpPos(int nPosX, int nPosY);
public:
	virtual void	GetFootPrint	(int& nPosX, int& nPosY);

protected:
	DWORD	m_tFootPrint;
	POINT	m_posFootPrint;

public:
	virtual OBJID	GetID()			{ return m_idNpc; }
	virtual int		GetObjType()	{ return CGameObj::GetObjType(); }
	virtual void	SendShow(IRole* pRole);

public:
	virtual int		GetType			()		{ return m_pType->GetID(); }
	virtual bool	IsAlive			();
	virtual bool	IsTalkEnable	()		{ return m_dwStatus == STATUS_NORMAL; }
	virtual bool	IsGM			()		{ return false; }
	virtual bool	IsPM			()		{ return false; }
	virtual bool	IsMonster		()		{ return IsMonsterID(GetID()) && !IsGuard() && !IsPkKiller(); }
	virtual bool	IsDelThis		()		{ return IsDeleted() && m_tDie.IsTimeOut(); }
	virtual bool	IsVirtuous()			{ return (m_pType->GetInt(NPCTYPEDATA_ATKUSER)&ATKUSER_RIGHTEOUS) != 0; }
	virtual bool	IsEvil();
	virtual bool	IsBowman()				{ return CItem::IsBowSort(GetWeaponRTypeID()); }
	virtual bool	IsShieldEquip()			{ return CItem::IsShield(GetWeaponLTypeID()); }
	virtual bool	IsWing()				{ return (m_pType->GetInt(NPCTYPEDATA_ATKUSER)&ATKUSER_WING) != 0; }
	int			GetData()			{ CHECKF(m_pData); return m_pData->GetInt(PETDATA_DATA); }

	static	bool	IsMonsterID(OBJID id)	{ return id>=MONSTERID_FIRST && id<=MONSTERID_LAST; }
	bool		IsGuard()			{ return (m_pType->GetInt(NPCTYPEDATA_ATKUSER)&ATKUSER_GUARD) != 0; }
	bool		IsPkKiller()		{ return (m_pType->GetInt(NPCTYPEDATA_ATKUSER)&ATKUSER_PPKER) != 0; }
	bool		IsDeleted()			{ return m_tDie.IsActive(); }
	void		DelMonster(bool bNow=false);
	bool		IsSynPet()				{ return m_idNpc >= PETID_FIRST && m_idNpc <= PETID_LAST; }
	bool		IsSynMonster()			{ return (m_idNpc >= PETID_FIRST && m_idNpc <= PETID_LAST) && !m_pData; }
	bool		IsCallPet()				{ return m_idNpc >= CALLPETID_FIRST && m_idNpc <= CALLPETID_LAST; }	CUser* GetOwnerLink() { return m_pOwner; }
	OBJID		GetMasterID();
	OBJID		GetGenID()			{ return m_idGen; }
	bool		IsTaskMonster()		{ return this->GetType() >= TYPE_TASK_MONSTER; }
	bool		IsMagicAtk()			{ return m_pType->GetInt(NPCTYPEDATA_MAGIC_TYPE) != ID_NONE && m_pType->GetInt(NPCTYPEDATA_MAGIC_HITRATE) ==0; }
	OBJID		GetMagicType()			{ return m_pType->GetInt(NPCTYPEDATA_MAGIC_TYPE); }
	bool		IsMapRoar()				{ return (m_pType->GetInt(NPCTYPEDATA_ATKUSER)&ATKUSER_ROAR) != 0; }
	bool		IsEquality()			{ return (m_pType->GetInt(NPCTYPEDATA_ATKUSER)&ATKUSER_EQUALITY) != 0; }
	CNpcType*	QueryType()				{ return m_pType; }

public:
	virtual void	SetDir(int nDir)			{ m_nDir = nDir%8; }
	virtual void	SetPose(int nPose)			{ m_nPose=nPose;}

	virtual	void	SetStatus(int nStatus, bool bSynchro=true);
	virtual	void	ClsStatus(int nStatus, bool bSynchro=true);

	virtual bool	AddAttrib(int idxAttr, __int64 i64Data, int nSynchro, DWORD* pMask);
	virtual	void	ProcessOnMove(int nMoveMode);
	void			ProcessAfterMove();
	virtual bool	SpendArrow			()				{ return true; }
public:
	virtual void	ClrAttackTarget		()									{ SetAttackTarget(NULL); }
	virtual bool	SetAttackTarget(IRole* pTarget = NULL);
	virtual bool	AutoSkillAttack(IRole* pTarget);
	virtual bool	AdditionMagic(int nLifeLost, int nDamage);
	virtual int		Attack(IRole* pTarget);
	virtual int		BeAttack(IRole* pTarget, int nPower, bool bReflectEnable);
	virtual int		BeAttackEx(IRole* pTarget, int nPower, bool bReflectEnable)		{ return BeAttack(pTarget, nPower, bReflectEnable); }
	virtual bool	MagicAttack(int nType, OBJID idTarget, int x, int y)	{ return false; }
	virtual void	Kill(IRole* pTarget, DWORD dwDieWay, int nReserved);
	virtual void	BeKill(IRole* pRole = NULL, int nReserved = 0);
	virtual bool	IsAtkable(IRole* pTarget, bool bSendHint=false);
	virtual bool	IsBeAtkable()							{ return true; }
	virtual int		GetAttackRange(int nTargetSizeAdd)		{ return m_pType->GetInt(NPCTYPEDATA_ATKRANGE)+(GetSizeAdd()+nTargetSizeAdd+1)/2; }
	virtual int		GetSizeAdd()							{ return m_pType->GetInt(NPCTYPEDATA_SIZEADD); }
	virtual void	SendDamageMsg(OBJID idTarget, int nDamage);
	void	ProcessBomb();
	int 	GetDieMode();
	bool	IsInsteadMagic();
	bool	IsImmunity			(IRole* pRole);

public:
	virtual bool	Freeze(int nSecs)						{ return IsAlive() && nSecs <= 0; }
	virtual void	SaveInfo();
	virtual CGameMap*		GetMap			()			{ ASSERT(m_pMap); return m_pMap; }

public:
	bool	ProcessAction	(CUser* pUser, OBJID idAction, LPCTSTR pszAccept = NULL);

public:
public:
	virtual	IStatus*	QueryStatus(OBJID idType)		{ CHECKF(m_setStatus); return m_setStatus->GetObj(idType); }
	virtual	IStatusSet*	QueryStatusSet()				{ CHECKF(m_setStatus); return m_setStatus; }
	void		ClearAllStatus	();
protected:
	IStatusSet*	m_setStatus;
public:	static int	AdjustData			(int nData, int nAdjust, int nMaxData=0);
public:
	bool	Create(PROCESS_ID idProcess, IRecordset* pRes);

public:
	CAutoLink<CMonster>&	QueryLink()			{ return m_link; }
protected:
	CAutoLink<CUser>	m_pOwner;
	CAutoLink<CMonster>	m_link;
	CMagicTypeData*		m_pMagicType;
	CTimeOut			m_tMagic;

public:
	DWORD	GetDropMoneyAvg();
	bool	DropMoney	(DWORD dwMoney, OBJID idOwner);
	bool	DropItem	(OBJID idItemType, OBJID idOwner);

protected:
	CPetData*	m_pData;

protected:
	bool	DropMedicine	(OBJID idOwner);
	bool	DropEquipment	(DWORD dwValue, OBJID idOwner, DWORD dwQuality=0);
	void	DropEventItem	(OBJID idOwner);

protected:
	OBJID		m_idNpc;
	int			m_nPosX;
	int			m_nPosY;
	int			m_nDir;
	DWORD		m_dwStatus;
	DWORD		m_dwEffect;
	int			m_nPose;
	CGameMap*	m_pMap;

	int			m_nCurrLife;
	int			m_nCurrMana;

protected:
	CBattleSystem*		m_pBattleSystem;
	CTimeOutMS			m_tFight;
	int					m_nFightPause;

protected:
	CNpcType*	m_pType;

protected:
	CTimeOut	m_tDie;
	auto_long	m_bLeaveMap;

protected:
	CTimeOut	m_tAddLife;
	OBJID		m_idGen;
	PROCESS_ID	m_idProcess;

	MYHEAP_DECLARATION(s_heap)
};

#endif
