#if !defined(AFX_TASKNPC_H__D7A2DFC3_2C2C_4F67_BB52_8F55409E76BC__INCLUDED_)
#define AFX_TASKNPC_H__D7A2DFC3_2C2C_4F67_BB52_8F55409E76BC__INCLUDED_

#include "MyHeap.h"
#include "GameData.h"
#include "NpcShop.h"
#include "NpcStorage.h"
#include "Myheap.h"
#include "Role.h"
#include "AutoPtr.h"
#if _MSC_VER > 1000
#pragma once
#endif

class CUser;
class CGameMap;

const	int	ADD_NEW_NPC				= 0;
const	int LAY_NEW_NPC				= 10;
const	int PRE_CHANGE_POSITION		= 11;
const	int CHANGE_POSITION			= 12;
const	int DYNANPCDIEDELAY_SECS	= 3;
const	int	GOAL_EXP_PERCENT		= 10;
const	int	SYNFLAG_SIZEADD			= 5;
const	int	SYNWAR_GAINMONEY_PERCENT			= 10;
const	int	DEFAULT_NPC_SIZEADD			= 4;
const	int SYNWARLETTERNPC_CHECKINTERVAL = 60*20;
const	int	_MAX_NPCTASK			=8;
const	int	MAX_NPCDATA				=4;

enum {
		SYNFLAGNPC_FIXFUND=0,SYNFLAGNPC_FIGHTBEGIN,SYNFLAGNPC_FIGHTEND,SYNFLAGNPC_MOREMAPID,
		SYNWARLETTER_ISATTACK=0,SYNWARLETTER_ATTACK_SYNID=1,SYNWARLETTER_OCCUPYTIME=2,SYNWARLETTER_DEPOSIT=3,
		SYNDRESS_SORT=0,SYNDRESS_SETTING=1,
		GOALNPC_LEVEL=3,
		TIMERNPC_TIME=3,
		TRUNKNPC_SIZE=3,
		EUDEMON_BROODER_NPC_SIZE=2, EUDEMON_STORAGE_NPC_SIZE=3,
		STORAGENPC_SIZE=3,
		NEIGHBORNPC_X=1,NEIGHBORNPC_Y=2,NEIGHBORNPC_SERIAL=3,
};

enum NPCTYPE{
		_NPC_NONE				= 0,
		_SHOPKEEPER_NPC			= 1,
		_TASK_NPC				= 2,
		_STORAGE_NPC			= 3,
		_TRUNCK_NPC				= 4,
		_FACE_NPC				= 5,
		_FORGE_NPC				= 6,
		_EMBED_NPC				= 7,
		_STATUARY_NPC			= 9,
		_SYNFLAG_NPC			= 10,
		_ROLE_PLAYER			= 11,
		_ROLE_HERO				= 12,
		_ROLE_MONSTER			= 13,
		_BOOTH_NPC				= 14,
		_SYNTRANS_NPC			= 15,
		_ROLE_BOOTH_FLAG_NPC	= 16,
		_ROLE_MOUSE_NPC			= 17,
		_ROLE_MAGICITEM			= 18,
		_ROLE_DICE_NPC			= 19,
		_ROLE_SHELF_NPC			= 20,
		_WEAPONGOAL_NPC			= 21,
		_MAGICGOAL_NPC			= 22,
		_BOWGOAL_NPC			= 23,
		_ROLE_TARGET_NPC		= 24,
		_ROLE_FURNITURE_NPC		= 25,
		_ROLE_CITY_GATE_NPC		= 26,
		_ROLE_NEIGHBOR_DOOR		= 27,
		_ROLE_CALL_PET			= 28,
		_EUDEMON_TRAINPLACE_NPC	= 29,
		 _AUCTION_NPC			= 30,
		 _ROLE_MINE_NPC			= 31,

		 _ROLE_3DFURNITURE_NPC	= 101,

		 _SYN_NPC_WARLETTER     = 110,
};

enum NPCSORT{
		NPCSORT_NONE		= 0,
		NPCSORT_TASK		= 1,
		NPCSORT_RECYCLE		= 2,
		NPCSORT_SCENE		= 4,
		NPCSORT_LINKMAP		= 8,
		NPCSORT_DIEACTION	= 16,
		NPCSORT_DELENABLE	= 32,
		NPCSORT_EVENT		= 64,
		NPCSORT_TABLE		= 128,

		NPCSORT_USELINKID	= NPCSORT_LINKMAP | NPCSORT_DIEACTION | NPCSORT_EVENT,
};

struct NPCINFO_ST{
	OBJID	idOwner;
	int		nOwnerType;
	OBJID	idMap;
	int		nPosX;
	int		nPosY;
	int		nType;
	int		nLookFace;
	int		setTask[_MAX_NPCTASK];
	int		setData[MAX_NPCDATA];
	NAMESTR	szData;
	OBJID	idLink;
	int		nLife;
	OBJID	idBase;
	int		nSort;
};

enum NPCDATA{
		NPCDATA_OWNERID=1,
		NPCDATA_OWNERTYPE,
		NPCDATA_NAME,
		NPCDATA_TYPE,
		NPCDATA_LOOKFACE,		STATUARYDATA_DIR=NPCDATA_LOOKFACE,
		NPCDATA_IDXSERVER,
		NPCDATA_MAPID,
		NPCDATA_POSX_,
		NPCDATA_POSY_,
		NPCDATA_TASK0,		STATUARYDATA_HELMET=NPCDATA_TASK0,
		NPCDATA_TASK1,		STATUARYDATA_ARMOR=NPCDATA_TASK1,
		NPCDATA_TASK2,		STATUARYDATA_WEAPONR=NPCDATA_TASK2,
		NPCDATA_TASK3,		STATUARYDATA_WEAPONL=NPCDATA_TASK3,
		NPCDATA_TASK4,		STATUARYDATA_FRAME=NPCDATA_TASK4,
		NPCDATA_TASK5,		STATUARYDATA_POSE=NPCDATA_TASK5,
		NPCDATA_TASK6,		STATUARYDATA_LOOKFACE=NPCDATA_TASK6,
		NPCDATA_TASK7,		STATUARYDATA_RANKSHOW_HAIR=NPCDATA_TASK7,
		NPCDATA_DATA0,
		NPCDATA_DATA1,
		NPCDATA_DATA2,
		NPCDATA_DATA3,
		NPCDATA_DATASTR,
		NPCDATA_LINKID,
		NPCDATA_LIFE,
		NPCDATA_MAXLIFE,
		NPCDATA_BASE,
		NPCDATA_SORT,
		NPCDATA_ITEMTYPE,

		NPCDATA_DEFENCE,
		NPCDATA_MAGIC_DEF,
};

typedef	IGameData<NPCDATA>	INpcData;
extern char	szNpcTable[];
typedef	CGameData<NPCDATA,szNpcTable,szID>	CNpcData;
extern char	szDynTable[];
typedef	CGameData<NPCDATA,szDynTable,szID>	CDynaNpcData;

class CNpcTask;
class CNpcScene;
class CNpcLink;
class CNpcBet;
class CNpc : public CGameObj, public CRole
{
protected:
	CNpc();
	virtual ~CNpc();
public:
	static CNpc* CreateNew()								{ return new CNpc; }
	bool	Create(PROCESS_ID idProcess, NPCINFO_ST* pInfo, OBJID idItemType, LPCTSTR szName);
	bool	Create(PROCESS_ID idProcess, IRecordset* pRecordset, bool bDynaNpc);

	IRole*			QueryRole()									{ return (IRole*)this; }
	virtual		bool		FindAroundObj	(OBJID idObjType, OBJID idObj, void** pp)		{ return GetMap()->QueryObj(GetPosX(), GetPosY(), idObjType, idObj, pp); }
public:
	virtual void	ReleaseByOwner()							{ delete this; }
	virtual OBJID	GetID()										{ return m_id; }
	virtual int 	GetObjType()								{ return CGameObj::GetObjType(); }
	virtual int		GetPosX()									{ return m_nPosX; }
	virtual int		GetPosY()									{ return m_nPosY; }
	virtual void	SendShow(IRole* pRole);

public:
	virtual LPCTSTR	GetName				();

	virtual DWORD	GetLookFace			()						{ if(GetType()==_STATUARY_NPC) return m_pData->GetInt(STATUARYDATA_LOOKFACE); return m_pData->GetInt(NPCDATA_LOOKFACE); }
	virtual DWORD	GetHair				()						{ return m_pData->GetInt(STATUARYDATA_RANKSHOW_HAIR)&0xFFFF; }
	virtual int 	GetDir				()						{ return MaskDir(m_pData->GetInt(NPCDATA_LOOKFACE))%MAX_DIRSIZE; }
	virtual DWORD	GetEffect			(int nIndex)			{ return m_dwEffect; }
	virtual int 	GetPose				()						{ return m_pData->GetInt(STATUARYDATA_POSE); }
	virtual DWORD	GetLev				()						{ if(IsGoal()) return GetData(GOALNPC_LEVEL); return 0; }

	virtual DWORD	GetLife				()						{ return m_pData->GetInt(NPCDATA_LIFE); }
	virtual DWORD	GetMaxLife			()						{ return m_pData->GetInt(NPCDATA_MAXLIFE); }

	virtual DWORD	GetMinAtk			()						{ return 0; }
	virtual DWORD	GetMaxAtk			()						{ return 0; }
	virtual DWORD	GetAttack			()						{ return 0; }
	virtual DWORD	GetDef				()						{ return m_pData->GetInt(NPCDATA_DEFENCE); }
	virtual DWORD	GetDefence			()						{ return m_pData->GetInt(NPCDATA_DEFENCE); }
	virtual DWORD	GetDex				()						{ return 0; }
	virtual DWORD	GetDexterity		()						{ return 0; }
	virtual DWORD	GetDdg				()						{ return 0; }
	virtual DWORD	GetDodge			()						{ return 0; }
	virtual DWORD	GetMagicDef			()						{ return m_pData->GetInt(NPCDATA_MAGIC_DEF); }
	virtual	DWORD	GetInterAtkRate		()						{ return 0; }
	virtual DWORD	GetIntervalAtkRate	()						{ return 0; }

	virtual int		GetHelmetTypeID		();
	virtual int		GetArmorTypeID		();
	virtual int		GetWeaponRTypeID	();
	virtual int 	GetWeaponLTypeID	();
	virtual int 	GetMountTypeID		()						{ return 0; }
	virtual int		GetSpriteTypeID		()						{ return 0; }
	virtual int		GetMantleTypeID		()						{ return 0; }

	virtual int		GetDistance			(int x, int y)			{ return __max(abs(GetPosX() - x), abs(GetPosY() - y)); }
	virtual int		GetDistance			(IMapThing* pTarget)	{ return __max(abs(GetPosX() - pTarget->GetPosX()), abs(GetPosY() - pTarget->GetPosY())); }
	virtual CGameMap*	GetMap			()						{ CHECKF(m_pMap); return m_pMap; }

	virtual DWORD	GetSynID			()						{ return m_pData->GetInt(NPCDATA_OWNERTYPE)==OWNER_SYN ? m_pData->GetInt(NPCDATA_OWNERID) : ID_NONE; }
	virtual DWORD	GetSynRank			()						{ return 0; }
	virtual DWORD	GetSynRankShow		()						{ return m_pData->GetInt(STATUARYDATA_RANKSHOW_HAIR)>>16; }
	virtual bool	IsSimpleMagicAtk	()						{ return false; }
	virtual int 	GetSimpleMagicHitRate()						{ return 0; }
	virtual UCHAR	GetNobilityRank		()						{ return 0; }
	virtual DWORD	GetExploit			()						{ return 0; }
	virtual int		GetHelmetColor		()						{ return 0; }
	virtual int		GetArmorColor		()						{ return 0; }
	virtual int		GetShieldColor		()						{ return 0; }

public:
	virtual bool	IsAlive				()						{ return !(IsDynaNpc() && m_pData->GetInt(NPCDATA_MAXLIFE)>0 && m_pData->GetInt(NPCDATA_LIFE)<=0 || IsDeleted()); }
	virtual bool	IsTalkEnable		()						{ return false; }
	virtual bool	IsGM				()						{ return false; }
	virtual bool	IsPM				()						{ return false; }
	virtual bool	IsMonster			()						{ return false; }
	virtual bool	IsDelThis			()						{ return IsDeleted() && m_tDie.IsTimeOut(); }
	virtual bool	IsVirtuous			()						{ return false; }
	virtual bool	IsEvil				()						{ return true; }
	virtual bool	IsBowman			()						{ return false; }
	virtual bool	IsShieldEquip		()						{ return false; }
	virtual bool	IsWing				()						{ return false; }

	virtual bool	QueryObj(OBJID idObjType, void** ppv)		{ if(idObjType==OBJ_NPC) return *ppv=this,true; return *ppv=NULL,false; }
	virtual IMapThing*	QueryMapThing	()						{ return (IMapThing*)this; }
	virtual IRole*	FindAroundRole		(OBJID id)				{ return GetMap()->QueryRole(GetPosX(), GetPosY(), id); }
	virtual bool	QueryRole(void** ppv)						{ return *ppv=QueryRole(),true; }
	virtual CUser*	QueryOwnerUser		()						{ return NULL; }

public:
	virtual void	SetDir				(int nDir)							{}
	virtual void	SetPose				(int nPose)							{}
	virtual	void	SetStatus			(int nStatus, bool bSynchro=true);
	virtual	void	ClsStatus			(int nStatus, bool bSynchro=true);
	virtual bool	AddAttrib			(int idxAttr, __int64 i64Data, int bSynchro, DWORD* pMask);
	virtual void	ProcessOnMove		(int nMoveMode)						{}
	virtual void	GetFootPrint		(int& nPosX, int& nPosY)			{ nPosX = GetPosX(); nPosY = GetPosY(); }
	virtual bool	SpendArrow			()									{ return true; }

	void				SetLookFace(int nLook, bool bSynchro);
	virtual	IStatus*	QueryStatus(OBJID idType)							{ ASSERT(IsDynaNpc()); ASSERT(m_setStatus); return m_setStatus->GetObj(idType); }
	virtual	IStatusSet*	QueryStatusSet()									{ ASSERT(IsDynaNpc()); ASSERT(m_setStatus); return m_setStatus; }
	virtual __int64	AwardBattleExp		(__int64 i64Exp, bool bGemEffect, int nReserved)	{ return 0; }
	virtual int 	AdjustHitRate		(int nHitRate)						{ return nHitRate; }
	virtual int 	AdjustAttack		(int nAtk)							{ return nAtk; }
	virtual int 	AdjustDefence		(int nDef)							{ return nDef; }
	virtual int 	AdjustBowDefence	(int nDef)							{ return nDef; }
	virtual int 	AdjustWeaponDamage	(IRole* pAtker, int nDamage)		{ return nDamage; }
	virtual int 	AdjustMagicDamage	(IRole* pAtker, int nDamage)		{ return nDamage; }
	virtual void	SetFightPause		(int nInterval)						{}
	virtual void	SaveInfo			();

public:

public:
	virtual void	BroadcastRoomMsg(CNetMsg* pMsg, bool bSendSelf = false);
	virtual void	BroadcastRoomMsg(LPCTSTR szMsg, bool bSendSelf = false);
	virtual bool	SendMsg(CNetMsg* pMsg)									{ return true; }
	virtual bool	SendSysMsg(const char* fmt, ...)						{ return true; }
	virtual void	OnTimer(time_t tCurr);

public:
	virtual bool	MoveToward(int nDir)									{ return 0; }
	virtual void	TransPos(int nPosX, int nPosY);
	virtual void	JumpPos(int nPosX, int nPosY)							{}

public:
	virtual void	ClrAttackTarget		()									{ SetAttackTarget(NULL); }
	virtual bool	SetAttackTarget(IRole* pTarget = NULL)					{ return true; }
	virtual bool	AutoSkillAttack(IRole* pTarget)							{ return false; }
	virtual bool	AdditionMagic(int nLifeLost, int nDamage)				{ return false; }
	virtual int		Attack(IRole* pTarget)									{ ASSERT(!"Attack"); return 0; }
	virtual int		BeAttack(IRole* pTarget, int nPower, bool bReflectEnable);
	virtual int		BeAttackEx(IRole* pTarget, int nPower, bool bReflectEnable);
	virtual bool	MagicAttack(int nType, OBJID idTarget, int x, int y)	{ ASSERT(!"MagicAttack"); return 0; }
	virtual void	Kill(IRole* pTarget, DWORD dwDieWay, int nReserved)		{ ASSERT(!"Kill"); }
	virtual void	BeKill(IRole* pTarget = NULL, int nReserved = 0);
	virtual bool	IsAtkable(IRole* pTarget, bool bSendHint=false)			{ return false; }
	virtual bool	IsBeAtkable();
	virtual int		GetAttackRange(int nTargetSizeAdd)						{ ASSERT(!"GetAttackRange"); return 0; }
	virtual int		GetSizeAdd()											{ if(IsSynFlag()) return SYNFLAG_SIZEADD; return DEFAULT_NPC_SIZEADD; }
	virtual void	SendDamageMsg(OBJID idTarget, int nDamage)				{ ASSERT(!"SendDamageMsg"); }
private:
	int			m_nPosX, m_nPosY;

public:
	int 			GetSort()									{ return m_pData->GetInt(NPCDATA_SORT); }
	int 			GetType()									{ return m_pData->GetInt(NPCDATA_TYPE); }
	void			ChangeDir(int nLook)						{ m_pData->SetInt(NPCDATA_LOOKFACE, MakeLookFace(MaskLook(GetLookFace()), MaskDir(nLook))); }
	bool			IsRecycleEnable()							{ return (GetSort() & NPCSORT_RECYCLE) != 0; }
	enum { MATE_ENABLE=true };
	bool			IsOwnerOf(CUser* pUser, bool bMateEnable=false);
	bool			IsDieAction()								{ return (GetSort() & NPCSORT_DIEACTION) != 0; }
	bool			IsStatuary()								{ return GetType() == _STATUARY_NPC; }
	bool			IsGoal()									{ return GetType()==_WEAPONGOAL_NPC || GetType()==_MAGICGOAL_NPC || GetType()==_BOWGOAL_NPC; }
	bool			IsSynFlag()									{ return GetType() == _SYNFLAG_NPC; }
	bool			IsAwardScore()								{ return GetType() == _SYNFLAG_NPC; }
	bool			IsDelEnable();
	bool			IsSynNpc()									{ return GetInt(NPCDATA_OWNERTYPE)==OWNER_SYN && GetInt(NPCDATA_OWNERID)!=ID_NONE; }
	bool			IsUserNpc()									{ return GetInt(NPCDATA_OWNERTYPE)==OWNER_USER && GetInt(NPCDATA_OWNERID)!=ID_NONE; }
	int 			GetStorageSize()							{ return GetData(STORAGENPC_SIZE); }
	bool			IsSynMoneyEmpty();
	bool			IsShelfNpc()								{ return m_pData->GetInt(NPCDATA_TYPE) == _ROLE_SHELF_NPC; }
	bool			IsSynWarLetter()							{ return GetType() == _SYN_NPC_WARLETTER; }

public:
	bool			SendSelfToBlock();
	bool			SendLeaveFromBlock();
	bool			ActivateNpc(IRole* pRole, int nAction);
	CItem*			Recycle(OBJID idUser);
	bool			DelNpc();
protected:
	bool			DeleteRecord()								{ CHECKF(IsDynaNpc()); return m_pData->DeleteRecord(); }
	void			CheckFightTime();
	void			RecruitLife();
	void			BroadcastScore();

public:
	int				GetInt		(NPCDATA idx)					{ CHECKF(m_pData); return m_pData->GetInt(idx); }
	LPCTSTR			GetStr		(NPCDATA idx)					{ CHECKF(m_pData); return m_pData->GetStr(idx); }
	int				GetData		(int idx)						{ CHECKF(m_pData); CHECKF(idx>=0 && idx<MAX_NPCDATA); return m_pData->GetInt(NPCDATA(NPCDATA_DATA0 + idx)); }
	OBJID			GetMapID	();

	void			SetInt		(NPCDATA idx, int nData, bool bUpdate = false)	{ CHECK(m_pData && IsDynaNpc()); m_pData->SetInt(idx, nData); if(bUpdate) m_pData->Update(); }
	void			SetStr		(NPCDATA idx, LPCTSTR szData, int nSize, bool bUpdate = false)	{ CHECK(m_pData && IsDynaNpc()); m_pData->SetStr(idx, szData, nSize); if(bUpdate) m_pData->Update(); }
	void			SetData		(int idx, int nData, BOOL bUpdate)	{
										CHECK(m_pData && IsDynaNpc());
										CHECK(idx < MAX_NPCDATA);
										m_pData->SetInt(NPCDATA(NPCDATA_DATA0 + idx), nData);
										if(bUpdate) m_pData->Update(); }
	BOOL			ChangePos		(OBJID idMap, int nPosX, int nPosY);
	bool			SetSynOwnerID(OBJID idSyn, BOOL bWithLinkMap);
	bool			SynchroData(OBJID idNpc, OBJID idOwner, int nData0, int nData1, int nData2);

public:
	virtual bool		IsShopNpc()							{ return m_pData->GetInt(NPCDATA_TYPE) == _SHOPKEEPER_NPC; }
	virtual CNpcShop*	QueryShop()							{ CHECKF(m_pShop); return m_pShop; }

	virtual bool		IsTaskNpc()							{ return m_pData->GetInt(NPCDATA_TYPE)==_TASK_NPC || (m_pData->GetInt(NPCDATA_SORT)&NPCSORT_TASK)!=0; }
	virtual CNpcTask*	QueryTask()							{ CHECKF(m_pTask); return m_pTask; }

	virtual	bool		IsStorageNpc()						{ return m_pData->GetInt(NPCDATA_TYPE) == _STORAGE_NPC; }
	virtual	bool		IsTrunkNpc()						{ return m_pData->GetInt(NPCDATA_TYPE) == _TRUNCK_NPC || m_pData->GetInt(NPCDATA_TYPE) == _ROLE_SHELF_NPC; }
	virtual CNpcTrunk*	QueryTrunk()						{ if(!m_pTrunk) CreateFuncTrunk(m_idProcess); CHECKF(m_pTrunk); return m_pTrunk; }

	virtual bool		IsSceneNpc()						{ return (m_pData->GetInt(NPCDATA_SORT) & NPCSORT_SCENE) != 0; }
	virtual CNpcScene*	QueryScene()						{ CHECKF(m_pScene); return m_pScene; }

	virtual bool		IsLinkNpc()							{ return (m_pData->GetInt(NPCDATA_SORT) & NPCSORT_LINKMAP) != 0; }
	virtual CNpcLink*	QueryLinkMap()						{ CHECKF(m_pLinkMap); return m_pLinkMap; }

	virtual bool		IsBetNpc()							{ return m_pData->GetInt(NPCDATA_TYPE) == _ROLE_DICE_NPC; }
	virtual CNpcBet*	QueryBet()							{ CHECKF(m_pBet); return m_pBet; }

	OBJID				GetRecordID()						{ CHECKF(m_pData); return m_pData->GetID(); }
	bool				IsDeleted()							{ return m_tDie.IsActive(); }

	bool				IsDynaNpc()							{ return m_bDynaNpc; }
private:
	bool				CreateFuncObj(PROCESS_ID idProcess);
	bool				CreateFuncTrunk(PROCESS_ID idProcess);
	bool				CheckSortMutex();

protected:
	CNpcShop*			m_pShop;
	CNpcTask*			m_pTask;
	CNpcTrunk*			m_pTrunk;
	CNpcScene*			m_pScene;
	CNpcLink*			m_pLinkMap;
	CNpcBet*			m_pBet;

protected:
	OBJID			m_id;
	INpcData*		m_pData;
	bool			m_bDynaNpc;
	CGameMap*		m_pMap;
	IStatusSet*		m_setStatus;
	DWORD			m_dwEffect;

protected:
	CTimeOut		m_tSynEvent;
	CTimeOut		m_tBroadcastScore;
	CTimeOut		m_tDie;
	bool			m_bFeeOK;
	PROCESS_ID		m_idProcess;

	MYHEAP_DECLARATION(s_heap)
};

#endif
