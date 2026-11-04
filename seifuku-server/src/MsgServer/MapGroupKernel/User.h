#if !defined(AFX_USER_H__BF5C20F1_5DBE_4E65_9099_E96EFAD7ECBA__INCLUDED_1)
#define AFX_USER_H__BF5C20F1_5DBE_4E65_9099_E96EFAD7ECBA__INCLUDED_1

#if _MSC_VER > 1000
#pragma once
#endif

#pragma warning(disable:4786)
#include "GameBlock.h"
#include "UserData.h"
#include "Friend.h"
#include "Item.h"
#include "Timeout.h"
#include "SquareDeal.h"
#include "gamemap.h"
#include "battleSystem.h"
#include "UserWeaponSkill.h"
#include "Myheap.h"
#include "T_SingleObjSet2.h"
#include "T_SingleMap2.h"
#include "team.h"
#include "magic.h"
#include "math.h"
#include "Package.h"
#include "SynAttr.h"
#include "protocol.h"
#include "booth.h"
#include "autolink.h"
#include "Enemy.h"
#include "AutoPtr.h"
#include "Role.h"

#include <vector>
#include <deque>
using namespace std;
#include <list>
#include "UserTable.h"
#include "define.h"
#include "ActionAnalyser.h"

const int	_MAX_USERFRIENDSIZE				= 50;
const int	_MAX_TUTORSIZE					= 15;
const int	_MAX_MONEYLIMIT					= 1000000000;
const int	USER_PACKAGE_SIZE				= 20;
const int	USERDROPITEM_RANGE				= 5;
const int	USERDROPITEM_RANGE2				= 9;
const int	JUMP_ALT						= 200;
const int	FLY_ALT							= 100;
const int	PERCENT_DECDUR					= 5;
const int	CRIME_KEEP_SECS					= 30;
const int   SELF_DEFENCE_SECS               = 20;
const int	SYNCRIME_KEEP_SECS				= 30;
const int	MAX_MENUTASKSIZE				= 20;

const int	PKVALUE_HALFKILL				= 2;
const int	PKVALUE_ONEKILL					= 20;
const int	PKVALUE_REDNAME					= 30;
const int	PKVALUE_BLACKNAME				= 100;
const int	MAX_ACTION_VAR					= 8;
const int	CHANCE_MAKEHOLE					= 300;

const int	SUPERMAN_KILLMONSTER_INCTIME		= 800;
const int	SUPERMAN_KILLMONSTER_TIMELIMIT		= 120*1000;
const int	TORNADO_KILLMONSTER_INCTIME			= 1000;
const int	TORNADO_KILLMONSTER_TIMELIMIT		= 120*1000;

const int	MASK_CHANGELOOK					= 10000000;
const int	MAX_SIZEADD						= 2;

const int	CHGMAP_LOCK_SECS				= 10;
const int	SYNWAR_NOMONEY_DAMAGETIMES		= 10;
const int	PICK_MORE_MONEY					= 1000;
const int	SYNCHRO_SECS					= 5;

const int	IDENT_FEE						= 100;
const int	UNIDENT_SALE_FEE				= 1;

const int	MAX_USER_XP						= 100;
const int	ADD_XP_SECS						= 30;
const int	ADD_XP_VALUE					= 10;
const int	MAX_XPFULL_SECS					= 60;

const int	ADJUST_EXPFORMAT_LEVEL			= 70;

const int	ADDSTUDENT_TUTOREXP				= 10;
const int	DELSTUDENT_TUTOREXP				= 1000;
const int	LEAVETUTOR_LEVEL				= 10;
const int	_MIN_GIVEEXP_LEV				= 30;
const int	TUTOREXP_PERCENT				= 40;
const int	TUTOREXP2_PERCENT				= 10;
const int	GETEXP_PER_TUTOREXP0			= 500;
const int	GETEXP_PER_TUTOREXP1			= 700;
const int	GETEXP_PER_TUTOREXP2			= 1000;

const int	SYNREPUTE_SECS					= 6*60;
const int	SYNREPUTE_VAL_PEREACH			= 10;		    
const int	SYNREPUTE_VAL_PROFFER			= 1;

#define		SYNMEM_TIMEONLINE_INTERVAL		15*60
#define		SYNMEM_TIMEONLINE_INCREPUTE		10
const int	EQUIPMENT_EXP_ADD_SECS			= 10*60;
const int	EQUIPMENT_EXP_ADD_SECS_AFTER9	= 15*60;
const int	WEAPON_UPEXP_MAXVALUE			= 8;
const int	ARMOR_UPEXP_MAXVALUE			= 10;
const int	NECKLACE_UPEXP_MAXVALUE			= 12;
const int	SHOE_UPEXP_MAXVALUE			= 14;
const int	BANGLE_UPEXP_MAXVALUE		= 16;
const int	MAX_LEVEL_WARLEVEL	= 9;
const int   DEC_WARGHOSTEXP_FORREPAIR = 3 ;

const int	PROFFER_BEKILL					= 100;
const int	SYNREPUTE_BEKILL				= 100;

const int	NEWBIE_LEVEL					= 40;
const int	_MIN_TUTOR_LEVEL				= 50;
enum { FLYMAP_ERROR=-1, FLYMAP_NORMAL, FLYMAP_MAPGROUP };
enum { ATTACK_TIME=false, BEATTACK_TIME=true, BY_WEAPON=false, BY_MAGIC=true };

enum {SEX_NONE = 0, SEX_MAN, SEX_WOMAN, SEX_ASEXUAL, SEX_ALL};

const unsigned char SEX_REQ_MAN			=	0x01;
const unsigned char SEX_REQ_WOMAN		=	0x02;
const unsigned char SEX_REQ_EUNUCH		=	0x04;

const int	MASTER_USERLEV				= 120;
const int	MAX_USERLEV					= 130;

typedef	IGameObjSet<CUserWeaponSkill>		IUserWeaponSkillSet;
typedef	CGameObjSet<CUserWeaponSkill>		CUserWeaponSkillSet;

enum 
{	
		RACE_NONE	= 0,
		RACE_HUMAN	= 1,
		RACE_SPRITE = 2,
		RACE_DARKSPRITE = 3,
};

enum {	PROFSORT_MAGICIAN	=1, 
		PROFSORT_SOLDIER	=2, 
		PROFSORT_ARCHER		=3,
		PROFSORT_HIGHEST_SOLDIER	=25,
		PROFSORT_WIZARD		=4, 
		PROFSORT_DAOIST		=10, 
		PROFSORT_DAOISTGOLD	=11, 
		PROFSORT_DAOISTWOOD	=12, 
		PROFSORT_DAOISTWATER=13, 
		PROFSORT_DAOISTFIRE	=14, 
		PROFSORT_DAOISTEARTH=15, 
		};

enum {
	NOBILITY_RANK_NONE		= 0,
	NOBILITY_RANK_BARON		= 1,
	NOBILITY_RANK_VISCOUNT	= 2,
	NOBILITY_RANK_EARL		= 3,
	NOBILITY_RANK_MARQUESS	= 4,
	NOBILITY_RANK_DUKE		= 5,
};

class CActionAnalyser; class CClientDetector;	enum USERSTATISTICDATA { USERSTATISTICDATA_ID=0, USERSTATISTICDATA_PLAYERID, USERSTATISTICDATA_NAME, USERSTATISTICDATA_EVENT, USERSTATISTICDATA_TYPE, USERSTATISTICDATA_DATA, USERSTATISTICDATA_TIMESTAMP };	extern char szStatisticTable[];	typedef CGameData<USERSTATISTICDATA, szStatisticTable, szID> CUserStatistic;	typedef ISingleObjSet2<CUserStatistic> IUserStatisticSet;
extern int g_nMaxLevel;
class CTradeBuddy;
class IUserStorage
{
public:
	virtual bool	SendStorageInfo(OBJID idNpc)															PURE_VIRTUAL_FUNCTION_0
	virtual bool	CheckIn(OBJID idNpc, CItem* pItem, bool bUpdate)										PURE_VIRTUAL_FUNCTION_0
	virtual CItem*	CheckOut(OBJID idNpc, OBJID idItem, bool bUpdate)										PURE_VIRTUAL_FUNCTION_0
	virtual int 	GetStorageCount()																		PURE_VIRTUAL_FUNCTION_0
};

const int	MASTER_WEAPONSKILLLEVEL	= 12;
const int	MAX_WEAPONSKILLLEVEL	= 20;
const int	nWeaponSkillLeveUpNeedExp[20] = {
				0,
				1200,
				68000,
				250000,
				640000,
				1600000,
				4000000,
				10000000,
				22000000,
				40000000,
				90000000,
				190000000/2,
				285000000/2,
				427500000/2,
				641250000/2,
				961875000/2,
				1442812500/2,
				2164218750/2,
				3246328125/2,
				4200000000/2
};

enum INFO_ID{
		INFO_NONE=0,
		INFO_ITEM,
		INFO_FRIEND,
		INFO_ENEMY,
		INFO_WEAPONSKILL,
		INFO_MAGIC,
		INFO_SYNATTR,
		INFO_STATUS,
		INFO_TEAM,
};

enum REMOTE_CALL{
		REMOTE_CALL_DROP_LEADER,
		REMOTE_CALL_LEAVE_SYN,
		REMOTE_CALL_TEAM_CLOSE_MONEY,
		REMOTE_CALL_TEAM_CLOSE_ITEM,
		REMOTE_CALL_TEAM_CLOSE_GEM_ACCESS,
		REMOTE_CALL_TEAM_HUNTKILL,
};

class	IChangeTeam
{
public:
	virtual bool AddTeamMember(OBJID idUser)				PURE_VIRTUAL_FUNCTION_0
	virtual bool DelTeamMember(OBJID idUser)				PURE_VIRTUAL_FUNCTION_0
	virtual bool DelTeam()									PURE_VIRTUAL_FUNCTION_0
	virtual bool SendInfo(SOCKET_ID idSocket, OBJID idAgent)				PURE_VIRTUAL_FUNCTION_0
};
class IUserChest
{
public:
	virtual void	SendChestInfo(OBJID idChest)							PURE_VIRTUAL_FUNCTION
	virtual void	CheckInChest(OBJID idChest, OBJID idItem)				PURE_VIRTUAL_FUNCTION
	virtual void	CheckOutChest(OBJID idChest, OBJID idItem)				PURE_VIRTUAL_FUNCTION
};

class CNetMsg;
class CPackage;
class CTransformation;
class CAgent;
class CUser : public CGameObj, public CRole, IUserStorage, IChangeTeam, IUserChest
{
protected:
	CUser();
	virtual ~CUser();
public:
	static		CUser*		CreateNew	()				{ return new CUser; }
	void		ReleaseByOwner			()				{ delete this; }
	void		OnTimer					(time_t tCurr);
	static		CUser*	CreateNewUser	(PROCESS_ID idProcess, SOCKET_ID idSocket, UserInfoStruct* pInfo);
	bool		Create				(PROCESS_ID idProcess, SOCKET_ID idSocket, OBJID idUser);
	bool		Init					();

	virtual		bool		FindAroundObj	(OBJID idObjType, OBJID idObj, void** pp)		{ return GetMap()->QueryObj(GetPosX(), GetPosY(), idObjType, idObj, pp); }
	virtual		IRole*		FindAroundRole	(OBJID id)		{ return GetMap()->QueryRole(GetPosX(), GetPosY(), id); }
	virtual		CUser*		FindAroundUser	(OBJID id);
protected:
	virtual		bool		QueryRole		(void** ppv)	{ return *ppv=(IRole*)this,true; }
	virtual		bool		QueryObj		(OBJID idObjType, void** ppv)			{ if(idObjType == OBJ_USER) return *ppv=this,true; return *ppv=NULL,false; }
	virtual		CUser*		QueryOwnerUser	()				{ return this; }
public:
	virtual		IMapThing*	QueryMapThing	()				{ return (IMapThing*)this; }
	IRole*					QueryRole		()				{ return (IRole*)this; }
	IChangeTeam*			QueryChangeTeam	()				{ return (IChangeTeam*)this; }
protected:
	virtual		IUserWeaponSkillSet*	QueryWeapenSkill()	{ return m_pUserWeaponSkillSet; }

public:
	virtual		int			GetObjType		()			{ return CGameObj::GetObjType(); }

	virtual		void		SendShow		(IRole* pRole);

public:
	virtual OBJID	GetID			()				{ return m_data.m_Info.id; }
	virtual LPCTSTR	GetName			()				{ return m_data.m_Info.szName; }
	virtual LPCTSTR	GetMate			()				{ return m_data.m_Info.szMate; }
	virtual int		GetSex			()				{ return (m_data.GetLookFace()%10000)/1000; }
	virtual DWORD	GetLookFace		();
	virtual DWORD 	GetHair()						{ return m_data.m_Info.dwHair; }
	virtual int		GetPosX			()				{return m_nPosX;}
	virtual int		GetPosY			()				{return m_nPosY;}
	virtual int 	GetDir			();
	virtual void	SetDir			(int nDir)		{ m_nDir = nDir%8; }
	virtual int		GetPose			();
	virtual void	SetPose			(int nPose)		{ m_nPose = nPose; }
	virtual void	SetEmotion		(int nEmotion)	{ m_nEmotin = nEmotion; }
	virtual DWORD	GetMoney		()				{ return m_data.m_Info.dwMoney; }
	virtual DWORD	GetMoneySaved	()				{ return m_data.m_Info.dwMoney_saved; }

	virtual OBJID	GetMateID		();

	void	SetLife(int nLife, BOOL bUpdate=false);
	virtual DWORD	GetLife				();
	virtual DWORD	GetMaxLife			();
	virtual DWORD	GetMana				()			{ return m_data.GetMana(); }
	virtual DWORD	GetMaxMana			();

	virtual DWORD	GetMinAtk			();
	virtual DWORD	GetMaxAtk			();
	virtual DWORD	GetMetempsychosisRate(IRole* pTarget);
	virtual DWORD	GetAttack			();
	virtual DWORD	GetMagicAttack		();

	virtual DWORD	GetDef				();
	virtual DWORD	GetDefence			();

	virtual DWORD	GetDex				();
	virtual DWORD	GetDexterity		();

	virtual DWORD	GetDdg				();
	virtual DWORD	GetDodge			();

	virtual DWORD	GetMagicDef			();

	virtual DWORD	GetInterAtkRate		();
	virtual DWORD	GetIntervalAtkRate	();
	virtual int		GetAttackRange		(int nTargetSizeAdd);
	virtual	int		GetSizeAdd			();

	virtual __int64	AwardBattleExp		(__int64 i64Exp, bool bGemEffect, int nReserved);
	virtual int 	AdjustHitRate		(int nHitRate);
	virtual int 	AdjustAttack		(int nAtk);
	virtual int 	AdjustDefence		(int nDef);
	virtual int 	AdjustBowDefence	(int nDef);
	virtual int 	AdjustWeaponDamage	(IRole* pAtker, int nDamage);
	virtual int 	AdjustMagicDamage	(IRole* pAtker, int nDamage);
	virtual void	SetFightPause		(int nInterval)			{ m_nFightPause = nInterval; }
	int				LuckyCriticalHit	(int& nDamage, IRole* pTarget);
	virtual int		GetDistance			(IMapThing* pTarget)	{ return __max(abs(GetPosX() - pTarget->GetPosX()), abs(GetPosY() - pTarget->GetPosY())); }
	virtual int		GetDistance			(int x, int y)			{ return __max(abs(GetPosX() - x), abs(GetPosY() - y)); }

	virtual __int64	GetExp				()						{ return m_data.GetExp(); }
	virtual void	SetStatus			(int nStatus, bool bSynchro=true);
	virtual void	ClsStatus			(int nStatus, bool bSynchro=true);
	virtual DWORD	GetStatus			()						{ return m_data.m_Info.dwStatus; }
	virtual void	ClearAllStatus		(DWORD dwKeepStatus);
	virtual DWORD	GetEffect			(int nIndex);

	virtual DWORD	GetLev				();
	virtual void	IncLev				(int nLev, bool* pbSatori);
	virtual void	AllotPoint			();

	virtual void	ClrAttackTarget		()						{ SetAttackTarget(NULL); QueryMagic()->BreakAutoAttack(); }
	virtual bool	SetAttackTarget		(IRole* pTarget);
	virtual OBJID	GetAttackTargetID	()						{ if (m_pBattleSystem) return m_pBattleSystem->GetTargetID(); return ID_NONE; }
	virtual bool	AutoSkillAttack		(IRole* pTarget)		{ return QueryMagic()->AutoAttackTarget(pTarget); }
	virtual bool	AdditionMagic(int nLifeLost, int nDamage)				{ return false; }
	virtual int		Attack				(IRole* pTarget);
	virtual int		BeAttack			(IRole* pTarget, int nPower, bool bReflectEnable);
	virtual bool	MagicAttack			(int nType, OBJID idTarget, int x, int y);
	virtual void	Kill				(IRole* pTarget, DWORD dwDieWay, int nReserved);
	virtual void	BeKill				(IRole* pTarget, int nReserved);
	virtual bool	BroadcastTeamLife	(bool bMaxLife=false);
	virtual bool	IsAtkable			(IRole* pTarget, bool bSendHint=false);
	virtual bool	IsBeAtkable			();
	virtual void	SendDamageMsg		(OBJID idTarget, int nDamage);
	virtual void	ProcessPk			(CUser* pTargetUser);
	virtual int		BeAttackEx			(IRole* pTarget, int nPower, bool bReflectEnable);

	virtual int		GetHelmetTypeID		()			{ if(m_pHelmet) return m_pHelmet->GetInt(ITEMDATA_TYPE); return ID_NONE; }
	virtual int		GetArmorTypeID		()			{ if(m_pArmor) return m_pArmor->GetInt(ITEMDATA_TYPE);  return ID_NONE;}
	virtual int		GetWeaponRTypeID	()			{ if(m_pWeaponR) return m_pWeaponR->GetInt(ITEMDATA_TYPE); return ID_NONE; }
	virtual int		GetWeaponLTypeID	()			{ if(m_pWeaponL) return m_pWeaponL->GetInt(ITEMDATA_TYPE); return ID_NONE; }
	virtual int		GetNecklaceTypeID	()			{ if(m_pNecklace) return m_pNecklace->GetInt(ITEMDATA_TYPE); return ID_NONE; }
	virtual int		GetRingRTypeID		()			{ if(m_pRingR) return m_pRingR->GetInt(ITEMDATA_TYPE); return ID_NONE; }
	virtual int		GetRingLTypeID		()			{ if(m_pRingL) return m_pRingL->GetInt(ITEMDATA_TYPE); return ID_NONE; }
	virtual int		GetShoesTypeID		()			{ if(m_pShoes) return m_pShoes->GetInt(ITEMDATA_TYPE); return ID_NONE; }
	virtual int		GetMountTypeID		()			{ if(m_pOvercoat) return m_pOvercoat->GetInt(ITEMDATA_TYPE); return ID_NONE; }
	virtual int		GetMantleTypeID		()			{ return m_data.m_Info.dwMedal_select; }
	virtual int		GetHelmetColor		();	virtual int	GetArmorColor	();	virtual int	GetShieldColor	();
	virtual CItem*	GetWeaponR			()			{ return m_pWeaponR; }
	virtual CItem*	GetWeaponL			()			{ return m_pWeaponL; }

	virtual DWORD	GetMetempsychosis	()			{ return m_data.m_Info.ucMetempsychosis; }

	virtual bool	IsSimpleMagicAtk	();
	virtual int 	GetSimpleMagicHitRate();

	virtual DWORD	GetExploit			()			{ return m_dwUnk20; }

	virtual bool	IsAlive				();
	virtual bool	IsTalkEnable		()			{ return true; }
	virtual bool	IsGM				()			{ return (strstr(GetName(), "[GM]") || strstr(GetName(), "[PM]")); }
	virtual bool	IsPM				()			{ return (strstr(GetName(), "[PM]") != NULL); }
	virtual bool	IsMonster			()			{ return false; }
	virtual bool	IsDelThis			()			{ return false; }
	virtual bool	IsVirtuous()					{ return GetPk() < PKVALUE_REDNAME && !QueryStatus(STATUS_CRIME); }
	virtual bool	IsEvil()						{ return GetPk() >= PKVALUE_REDNAME || QueryStatus(STATUS_CRIME); }
	virtual bool	IsBowman			()			{ return m_pWeaponR && m_pWeaponR->IsBowSort(); }
	virtual bool	IsShieldEquip		()			{ return m_pWeaponL && m_pWeaponL->IsShield(); }
	virtual bool	IsWing				()			{ return QueryStatus(STATUS_WING) != NULL; }
	virtual bool	IsSendBlockInfo		()			{ return true; }
	virtual bool	IsAutoAllot			(void)		{ return (m_data.m_Info.ucAuto_allot != 0); }
	virtual bool	IsNewbie			()			{ return GetLev() < NEWBIE_LEVEL; }

	virtual bool	SendMsg				(CNetMsg* pMsg);
	virtual void	BroadcastRoomMsg	(CNetMsg* pMsg, bool bSendSelf);
	virtual void	BroadcastRoomMsg	(LPCTSTR szMsg, bool bSendSelf);
	virtual void	BroadcastMapMsg		(CNetMsg* pMsg, bool bSendSelf);
	virtual void	BroadcastMapMsg		(LPCTSTR szMsg, bool bSendSelf);
	virtual bool	SendSysMsg			(const char* fmt, ...);
	virtual bool	SendSysMsg	(int nChannel, const char* fmt, ...);

	virtual DWORD	GetSynID			()				{ return QuerySynAttr()->GetSynID(); }
	virtual DWORD	GetMasterSynID		()				{ if(QuerySynAttr()->GetSynID()==ID_NONE) return ID_NONE; return QuerySynAttr()->GetMasterSynID(); }
	virtual DWORD	GetSynRank			()				{ if(QuerySynAttr()->GetSynID()==ID_NONE) return RANK_NONE; return QuerySynAttr()->GetInt(SYNATTRDATA_RANK); }
	virtual DWORD	GetSynRankShow		()				{ if(QuerySynAttr()->GetSynID()==ID_NONE) return RANK_NONE; return QuerySynAttr()->GetSynRankShow(); }
	UCHAR			GetPeerageRank		()				{ return GetNobilityRank(); }
	virtual UCHAR	GetNobilityRank		();
public:
	bool		CheckInCard	(void);
	bool		CheckOutCard(void);
	int			CountCard	(void);

	bool		CheckInCard2	(void);
	bool		CheckOutCard2	(void);
	int			CountCard2		(void);	int CountCardByType(OBJID idCardType);

public:
	void		Mine	(void);
	void		StopMine(void);

private:
	void		ProcessMineTimer(void);
	int			m_nMineCount;	DWORD m_dwMineFactor;
	CTimeOutMS	m_tMine;

public:
	OBJID		ChkUpEqQuality	(CItem* pItem, bool bSendHint=false);
	OBJID		ChkUpEqLevel	(CItem* pItem, bool bSendHint=false);

	bool		GetUpEpQualityInfo		(CItem* pItem, int& nChance, OBJID& idNewType, bool bSendHint=false);
	bool		GetUpEpLevelInfo		(CItem* pItem, int& nChance, OBJID& idNewType, bool bSendHint=false);

	int			GetChanceUpEpQuality	(int nPosition, bool bSendHint=false);
	int			GetChanceUpEpLevel		(int nPosition, bool bSendHint=false);

	bool		UpEquipmentQuality		(int nPosition, bool bFree);
	bool		UpEquipmentLevel		(int nPosition, bool bFree);		bool IsMeteorEnough(int nMeteors); bool IsMeteorChangeSpare(int nMeteors); int SpendMeteor(int nMeteors);
	bool		DownEquipmentLevel		(int nPosition, int nUserLevel);		bool UpEquipmentLevelUltra(int nPosition); bool UpEquipmentLevelUltra2(int nPosition);

	void		UpEquipmentQuality		(OBJID idEquipment, OBJID idTreasure);
	void		UpEquipmentLevel		(OBJID idEquipment, OBJID idTreasure);
	void		EmbedGem				(OBJID idItem, OBJID idGem, int nPos);
	void		TakeOutGem				(OBJID idItem, int nPos);

	bool		RecoverEquipmentDur		(int nPos);

	bool		IsAllNonsuchEquip		(void);

public:
	int			GetGemMgcExpEffect	(void);
	int			GetGemWpnExpEffect	(void);
	int			GetGemExpEffect		(void);
	int			GetGemAtkEffect		(void);
	int			GetGemMgcAtkEffect	(void);

	void		SendGemEffect		(bool bAttack);
	int			GetEquipMagicAtkAdd	();		int			GetEquipMagicDefAdd	();
public:

public:
	bool		SetLock(DWORD dwKey);
	bool		OpenLock(DWORD dwKey);
	bool		ClearLock(DWORD dwKey);
	bool		CheckLock(bool bSynchro=true);
	bool		SetAcceptData(DWORD dwKey);		DWORD		GetAcceptKey();
public:
	virtual		void	TransPos		(int nPosX, int nPosY);
	virtual		void	JumpPos			(int nPosX, int nPosY);
	virtual		bool	MoveToward		(int nDir);

	void		SetSynchro				(bool bSynchro)		{ if(bSynchro) m_tSynchro.Clear(); else m_tSynchro.Startup(SYNCHRO_SECS); }
	bool		IsSynchro				()					{ return !(m_tSynchro.IsActive() && !m_tSynchro.TimeOver()); }
	bool		SynPosition			(int nPosX, int nPosY, int nMaxDislocation = 8);
	void		ProcessOnAttack		();
	void		ProcessOnMove		(int nMoveMode);
	void		ProcessAfterMove	();
	void		GetFootPrint	(int& nPosX, int& nPosY);
	bool		KickBack();
	bool		Synchro();
	void		StandTurnon()								{ ASSERT(!m_bStand); m_tStand.Clear(); m_bStand = true;  }
	void		StandTurnoff()								{ m_tStand.Clear(); if(m_bStand){ m_bStand = false;  }}
	void		StandRestart()								{ StandTurnoff(); m_tStand.Update(); }
protected:
	CTimeOut	m_tSynchro;
	DWORD		m_tFootPrint;
	POINT		m_posFootPrint;
	auto_long	m_bRun;
	CTimeOutMS	m_tStand;
	auto_long	m_bStand;
	CTimeOut	m_tAutoReborn;

public:
	OBJID		GetMapID						();
	OBJID		GetRecordMapID					();
	bool		ChangeMap						();
	int 		FlyMap							(OBJID idMap, int nPosX, int nPosY);
	void		GotoJail						(OBJID idMap, LPCTSTR pszText);
	virtual bool		SendSelfToBlock			();
	
public:
	SOCKET_ID	GetSocketID			()						{ return m_idSocket; }
	virtual bool	IsAgent			()						{ return false; }
	__int64		GetVarData			(int idx);
	void		SetVarData			(int idx, __int64 i64Data);
	LPCTSTR		GetVarStr			(int idx);
	void		SetVarStr			(int idx, LPCTSTR pszStr);
	void		SaveEmoneyLog		(int nType, OBJID idSource, OBJID idTarget, int nNumber);
	int			GetDragonType		(int nType);
protected:	string		m_szVarStr[MAX_ACTION_VAR];			public:
	int			GetEquipBattlePower	(CItem* pItem);
	void		AwardOnlineTrainingExp	();
	void		AddUnk8E8			(int nData);
	int			GetBigPrizeFails	()	{ return m_data.m_Info.usBigPrizeFails; }
	int			GetSmallPrizeFails	()	{ return m_data.m_Info.usSmallPrizeFails; }		DWORD GetCurrentLayoutType() { return m_data.m_Info.dwCurrentLayoutType; }
	void		SetBigPrizeFails	(int nData, bool bUpdate)	{ m_data.SetBigPrizeFails(nData, bUpdate); }
	void		SetSmallPrizeFails	(int nData, bool bUpdate)	{ m_data.SetSmallPrizeFails(nData, bUpdate); }
	DWORD		GetEmoney			()	{ return m_data.m_Info.dwEmoney; }
	bool		IsBackPackFull		()	{ return m_setItem.size() >= 40; }
	bool		IsItemProtected		(CItem* pItem);	bool IsProtectedAgainst(CItem* pItem, CItem* pOther, int nKind, int* pCount, bool bEquip);
	bool		IsBackPackSpare		(int nNum, int nWeight);
	void		UpdateGhostContract	(CItem* pItem, OBJID idOwner);		void SetItemLockDate(OBJID idItem, DWORD dwTimeout);		map<OBJID, DWORD>	m_mapItemLockDate;
	struct DELAY_ACTION { CTimeOut tDelay; OBJID idAction; };	typedef list<DELAY_ACTION> DELAY_ACTION_SET;
	DELAY_ACTION_SET	m_setDelayAction;
	void		AddDelayAction		(int nSecs, OBJID idAction);
	void		ProcessLuckyTime	();
	void		ProcessOnlineTraining();	void	ProcessCheckBattleEffect();
	void		OnTimeCardEnd		();		void	AbortLook	();
	void		ProcessFeeInfo		(int nType, DWORD dwData, int nTime);
	void		SynchroFeeData		(DWORD dwData);	void	SynchroFeeTime	(int nTime);
	void		SetMinorFlag		(DWORD dwFlag)		{ m_dwUnk1C = dwFlag; }
	DWORD		GetClientVersion	()			{ return m_dwUnk34; }	void	SetClientVersion(DWORD dwVer)	{ m_dwUnk34 = dwVer; }	void	SetSocketIP(DWORD dwIP)	{ m_dwUnk914 = dwIP; }
	int			GetBusinessmanDays	();	void	BecomeBusinessman	();	bool	IsBusinessman	();
	bool		CheckActionCount	(DWORD dwCount);	void	IncActionCount	()	{ m_dwUnk6AC++; }
	int			GetBattlePowerBase	()			{ return m_dwUnk8D8; }
	bool		LoadStatistic		();
	DWORD		GetStatisticTimestamp(int idEvent, int idType);	void	SetStatisticTimestamp(int idEvent, int idType, DWORD dwTime);
	void		AddTimeCardHours	(int nHours);	time_t	GetGodStatusTime();	void	SetGodStatusTime(time_t tTime);
	void		CalcLevByExp		(__int64 i64Exp, int* pLev, int* pPercent);	__int64	CalcExpByTime(int nTime);	static __int64 GetDigits(__int64 n, int nFirst, int nLast);
	void		SaveStatistic		();
	vector<CTradeBuddy*>	m_vecTradeBuddy;
	void		OnLoginComplete		();
	void		OnTimeCardMapOut	();
	void		OnTimeCardMapIn		();
	void		SetDoubleExp		(int nDoubleExp, int nSecs);
	int			GetTimeCardLeft		(int nUnit, bool bRound);
	void		GetBlessTimeStr		(char* pszBuf);
	int			GetEmoneyCard1		();
	int			GetEmoneyCard2		();
	int			GetBusinessman		();
	void		SetBusinessman		(int nDate);
	void		SynchroBusinessman	(int nDate);
	void		InitVerifyAnswer	(DWORD dwSeed);
	bool		CreateAllBusiness	(IDatabase* pDb);
	bool		IsTimeCardStatus	();
	bool		IsVirtueAwardLev	()		{ return GetLev() >= 40 && GetLev() < 70; }
	__int64		AwardExp			(__int64 i64Exp, bool bContribute);
	void		ContributeExp		(bool bLev);
	void		AddContributeExp	(__int64 i64Exp);
	vector<CItem*>	m_setItem;
	bool		IsTimeCardOn		();
	int			GetStatistic		(int idEvent, int idType);
	void		SetStatistic		(int idEvent, int idType, int nData, bool bStamp, bool bUpdate, bool bSynchro);
	int			GetTeamKillCount	(OBJID idUser);
	void		SetTeamKillCount	(OBJID idUser, int nCount);
	int			GetKillStatistic	(int nType);
	void		SetKillStatistic	(int nType, int nCount);
	bool		IsNewInteractData	(int nData);
	void		AddInteractData		(int nData);
	int			SumKillStatistic	();
	void		SendKillStatistic	(bool bTeam);
	vector<int>	m_setInteractData;	map<int,int>	m_mapKillStatistic;	map<OBJID,int>	m_mapTeamKill;
	bool		IsAntiMonster		(IRole* pTarget);
	bool		WeaponMagicAttack	(OBJID idTarget, int x, int y);
	bool		CheckReflect		();
	int			GetReflectLimit		();
	int			GetBattlePower		()		{ return m_dwUnk8D8 + m_dwUnk20; }
	int			GetGemHitRateEffect	(void);
	int			GetEquipReduceDmg	();
	void		OnTimeCardMapLeave	();
	void		ClearUnkItemSet		();		vector<CItem*>	m_setUnkItem;
	bool		IsIncomeLimited		();
	int			GetIncomePercent	()		{ return m_nUnk24; }
	virtual CAgent*	QueryAgent		()						{ MYASSERT(!"QueryAgent"); return NULL; }
	void		SaveInfo			();
	void		RemoteCall			(REMOTE_CALL0* pInfo);
	OBJID		GetAccountID		()						{ return m_data.m_Info.idAccount; }

public:

public:
	bool		IsMoreLeaveWord()							{ return m_bHaveLeaveWord; }
	void		SetLeaveWord(bool nFlag)					{ m_bHaveLeaveWord = nFlag; }
	bool		AddLeaveWord(LPCTSTR szRecv, LPCTSTR szWords);
protected:
	bool		m_bHaveLeaveWord;
	CTimeOut	m_tLeaveWord;
public:	bool	IsLeaveWordEnable()	{ return m_tLeaveWord.ToNextTime(LEAVEWORD_INTERVAL_MINS*60); }
public:
	bool	DoBonus			(void);
	void	SendBonusInfo	(void);

public:
	static int	AdjustData			(int nData, int nAdjust, int nMaxData=0);
	static int	GetLook(DWORD dwLookFace)					{ return dwLookFace%1000; }
	static int	GetFace(DWORD dwLookFace)					{ return (dwLookFace/10000)%1000; }
	static int	GetTransFormLook(DWORD dwLookFace)			{ return dwLookFace/10000000; }

public:
	IUserStorage*	QueryStorage()							{ return (IUserStorage*)this; }
protected: 
	virtual bool	SendStorageInfo(OBJID idNpc);
	virtual bool	CheckIn(OBJID idNpc, CItem* pItem, bool bUpdate = true);
	virtual CItem*	CheckOut(OBJID idNpc, OBJID idItem, bool bUpdate);
	virtual int 	GetStorageCount()						{ IF_OK(m_pStorage) return m_pStorage->GetAmount(); return 1234567890; }
public:	bool	ProcessTargetRole(OBJID idNpc);	bool	IsChestFull(CItem* pItemPack);
protected:
	virtual void	SendChestInfo(OBJID idChest);
	virtual void	CheckInChest(OBJID idChest, OBJID idItem);
	virtual void	CheckOutChest(OBJID idChest, OBJID idItem);

protected:
	CPackage*		m_pStorage;
public:
    int  GetAuctionPackageAmount(OBJID idNpc,int nPosition = ITEMPOSITION_AUCTION_STORAGE);
	bool AuctionGoodsBackOff(CItem * pItem, int nPosition = ITEMPOSITION_AUCTION_STORAGE);
public:
	int			GetAddPoint			();

	int			GetAutoAllot		()					{return m_data.GetAutoAllot();}
	void		SetAutoAllot		(int nAutoAllot)	{m_data.SetAutoAllot(nAutoAllot);}

	void		SetMetempsychosis		(int nMetempsychosis)	{m_data.SetMetempsychosis(nMetempsychosis);}
    void		SetMaxLifePercent	(int nPercent, BOOL bUpdate = TRUE);
	bool		AddAttrib			(int idxAttr, __int64 i64Data, int nSynchro, DWORD* pMask = NULL);
	void		SetAttrib			(int idxAttr, __int64 i64Data, int nSynchro);

public:
	void		UpdateMate			(const char* pszName, bool bUpdate = true);
	bool		IsMarried			(void);
	bool		IsMate				(CUser* pUser);
	bool		Court				(OBJID idTarget);
	bool		Marry				(OBJID idTarget);
	bool		Divorce				(void);
	bool		DropItem3			(OBJID idMap, int x, int y);
public:
	bool		IsMasterPrentice	(CUser* pUser)		{ return false; }

public:
protected:
	CTimeOut	m_tMount;

public:
protected:
	CTimeOut			m_tSprite;

public:
	enum { _TYPE_MAGIC=0, _TYPE_WS=1, _TYPE_FY=2, _TYPE_USE_LIFE=3, _TYPE_USE_MANA=4, _TYPE_USE_WS2_32=5, _TYPE_TICK_ERROR=6, _TYPE_INVALID_CLIENT=7, _TYPE_ACTION_MODE=8, _TYPE_VERIFY_DATA=9, _TYPE_AUTO_ROBOT=10, _TYPE_ZFJL=11, };
	bool		SetCheatLevel(int nLevel);
	void		KickoutCheat(OBJID idKind);
	int			GetCheatLevel()						{ return m_nCheatLevel; }
	bool		IsCheater(OBJID idKind);
	bool		IsMagicAtkCheat	(OBJID idTarget, int nTargetX, int nTargetY, DWORD dwTimeStamp);
	void		SetCheatMark(OBJID idCheatKind, DWORD dwCheatMark);
	void		DoCheaterPunish	(int nData = 0);

	DWORD		m_dwLastJump;
	DWORD		m_dwMsgCount;

	bool		CheckTimeStamp(DWORD dwTimeStamp)				{ if(dwTimeStamp-m_dwLastTimeStamp>50) { m_dwLastTimeStamp=dwTimeStamp; return true; } return false; }

protected:
	void		SetCheat(int nType, int nLevel);
protected:
	auto_long	m_nCheatLevel;
	auto_long	m_nCheatCount;
	auto_long	m_maskCheat;

	deque<DWORD>	m_setActionTime;

	auto_long	m_dwLastTimeStamp;

protected:
	virtual		DWORD	SetEffect			(DWORD dwEffect, int nIndex)	{ m_data.m_Info.dwEffect[nIndex % 2] |= dwEffect; return m_data.m_Info.dwEffect[nIndex % 2]; }
	virtual		DWORD	ResetEffect			(DWORD dwEffect, int nIndex)	{ m_data.m_Info.dwEffect[nIndex % 2] &= (~dwEffect); return m_data.m_Info.dwEffect[nIndex % 2]; }

public:
	UserInfoStruct*	GetInfo	(void)		{return &m_data.m_Info;}

	virtual DWORD			GetWeightLimit	();
public:
	bool		IsInCheatJail		();
	bool		CheckJumpTime		(DWORD dwTimeStamp);
	void		SetCheatKickout		(int nSecs);
	void		SendVerifyCode		(int nSecs);
	void		CheckVerifyCode		(DWORD dwAnswer);
	bool		IsVerifyEnable		(int nData);
	void		SendDetectMsg		(int nType);
	void		VerifyDataFailed	(int nData);
	void		OnDetectMsg			(int nType, DWORD dwData);
protected:
	deque<DWORD>	m_setJumpTime;
	CTimeOut		m_tCheatKickout;
	DWORD			m_setVerifyAnswer[256];
	int				m_nVerifyIndex;
	CTimeOut		m_tVerify;
	DWORD			m_dwDetectData;
	CClientDetector*	m_pVerifyDataDetector;
public:
public:
	void		OnLogout			();
	void		DetachLuckyStatus	();
	void		UpdateTimeUnk63C	()	{ m_dwUnk63C = ::TimeGet(); }	int			GetTimeCardStatusRemain();
protected:
	DWORD		m_dwUnk1C;		DWORD	m_dwUnk20;
	int			m_nUnk24;		bool	m_bUnk28;
	DWORD		m_dwUnk2C;
	IUserStatisticSet*	m_pStatisticSet;
	DWORD		m_dwUnk34;
	CActionAnalyser*	m_pAnalyser;
	DWORD		m_dwUnk5B0;
	DWORD		m_dwUnk610;		CTimeOut	m_tUnk614;
public:	DWORD		m_dwUnk638;	protected:	DWORD	m_dwUnk63C;
	int			m_nDoubleExp;
	CTimeOut	m_tDoubleExp;
	DWORD		m_dwUnk6AC;
	__int64		m_i64LastSaveExp;
	DWORD		m_dwLastSaveExpTime;	DWORD	m_dwUnk8CC;
	int			m_nUnk8D0;		int		m_nUnk8D4;
	DWORD		m_dwUnk8D8;		__int64	m_i64ContributeExp;
	DWORD		m_dwUnk8E8;
	DWORD		m_dwUnk8EC;		int		m_nUnk8F0;
	DWORD		m_dwTimerCount;		DWORD	m_dwUnk914;
public:
	int			m_nUnk858;
	void		UpdateBattlePower	();
	virtual DWORD	GetMeteLev			();
	virtual CGameMap*		GetMap			()			{ CHECKF(m_pMap); return m_pMap; }
public:
	enum { 
		APPLY_NONE=0,
		APPLY_FRIEND, 
		APPLY_MARRY, 
		APPLY_TRADE, 
		APPLY_TEAMAPPLY, 
		APPLY_TEAMINVIT,
		APPLY_JOIN_SYN, 
		APPLY_INVITE_JOIN_SYN, 
		APPLY_ALLY_SYN, 

		APPLY_STUDENTAPPLY,
		APPLY_TEACHERAPPLY,
	};

	void	SetApply		(int nApply, OBJID idTarget)	{ m_nApply = nApply; m_idApplyTarget = idTarget; }
	OBJID	FetchApply		(int nApply)					{ if(nApply == m_nApply) { m_nApply = APPLY_NONE; return m_idApplyTarget; } return ID_NONE; }
	bool	FetchApply		(int nApply, OBJID idTarget)	{ if(nApply == m_nApply && idTarget == m_idApplyTarget) { m_nApply = APPLY_NONE; return true; } return false; }
protected:
	int		m_nApply;
	OBJID	m_idApplyTarget;

public:
	void		SetForce			(DWORD dwForce);
	DWORD		GetForce			()				{ return m_data.GetForce();}

	void		SetSpeed			(DWORD dwSpeed);
	DWORD		GetSpeed			()				{ return m_data.GetSpeed();}
	DWORD		AdjustSpeed			(int nSpeed);

	void		SetHealth			(DWORD dwHealth);
	DWORD		GetHealth			()				{ return m_data.GetHealth();}

	void		SetSoul				(DWORD dwSoul);
	DWORD		GetSoul				()				{ return m_data.GetSoul();}
	DWORD		GetSoulSum			();

	void		SetFace				(DWORD dwFace);
	DWORD		GetFace				()				{ return (m_data.GetLookFace()/10000)%1000; }
	void		SetLook				(DWORD dwLook);

	void		ClsXpVal			(bool bSynchro = SYNCHRO_TRUE);
	void		ProcXpVal			();
	bool		IsFullXp			()				{ return m_dwXP >= MAX_USER_XP; }
	bool		BurstXp				();
	void		SetXp				(int nXp);
	void		AddXp				(int nXp);
	int			GetXp				()				{ return m_dwXP; }
	OBJID		GetHomeID			()				{ return m_data.GetHomeID(); }
	OBJID		GetMateHomeID();

	int			GetRace				()				{ if(GetLook(GetLookFace()) > 6) return RACE_NONE; return (GetLook(GetLookFace())+1) / 2; }

	void		ProcAutoHealLife	();

#ifdef _DEBUG
	void		SetName	(const char* pszName)	{ m_data.SetName(pszName); }
#endif

public:
	DWORD		GetProfession		()	{ return m_data.GetProfession(); }
	DWORD		GetProfessionSort	()	{ return (m_data.GetProfession()%1000)/10; }
	DWORD		GetProfessionLevel	()	{ return m_data.GetProfession()%10; }

public:
	void		IncAddPoint			(int nAddPoint);
	void		SetExp				(__int64 i64Exp, BOOL bUpdate=true)	{m_data.SetExp(i64Exp, bUpdate);}
	void		CalcFightRate		()						{ m_nFightRate = GetInterAtkRate(); }
	void		ProcessAutoAttack	();
	int			GetDelay			()						{ return m_nFightPause; }
	void		SetDelay			(int nDelay)			{ m_nFightPause = nDelay; }
	int			AdjustExp			(IRole* pTarget, int nRawExp, bool bNewbieBonusMsg=false);
	bool		CheckCrime			(IRole* pRole);		
	bool		IsImmunity			(IRole* pRole);		

public:
	BOOL		CreateTeam			();
	BOOL		AcceptTeamMember	(CUser* pMemeber);
	BOOL		AcceptTeamInvit		(CUser* pLeader);
	BOOL		QuitTeam			();
	BOOL		DismissTeam			();
	BOOL		KickOut				(OBJID idMember);
	void		SetTeam				(CTeam* pTeam);
	void		ClrTeam				();
	CTeam*		GetTeam				();
	void		ProcessTeamTimer	();
	void		SendShow			(CUser* pNewMember);
	void		AppendTeam			(TeamInfoStruct* pInfo);
protected:
	virtual bool AddTeamMember(OBJID idUser);
	virtual bool DelTeamMember(OBJID idUser);
	virtual bool DelTeam();
	virtual bool SendInfo(SOCKET_ID idSocket, OBJID idAgent);
private:
	CTeam*		m_pTeam;
	CTimeOut	m_tTeam;
public:
	int			GetPk				()				{ return m_data.GetPk(); }
	int			IncPk				()				{ return AddPk(PKVALUE_ONEKILL); }
	int			GetPkMode			()				{ return m_data.m_Info.ucPkMode; }
	void		SetPkMode			(int nMode)		{ m_data.m_Info.ucPkMode = nMode; }
	bool		IsPkEnable			(IRole* pRole);
	bool		IsGhost				()				{ return m_bGhost; }
	void		Reborn				(bool bChgPos, bool bAction);
	void		TransformGhost		();
	__int64		CalcExpLostOfDeath	(IRole* pKiller);
protected:
	bool		m_bGhost;

public:
	int			AddPk				(int nAdd);
	void		ProcessPkTimer		(void);
	void		SendSystemBBS		(LPCTSTR szText);

public:
	enum { IDENT_OK = true, NO_COMBINE=false, };
	CItem*	AwardItem		(int nItemType, bool bSynchro);	CItem* AwardNewItem(int nItemType, bool bSynchro);
	CItem*	AwardItem		(ItemInfoStruct* pInfo, bool bSynchro, bool bAutoCombine=true);
	bool	EraseItem		(OBJID idItem, bool bSynchro);
	bool	EraseEquip		(int nPosition, bool bSynchro);				bool ComposeItem(OBJID idMain, OBJID idAccessory1, OBJID idAccessory2, OBJID idGem1, OBJID idGem2);		bool FuseItem(OBJID* pidItem, int nSize, int nType);	int FindEquipPos(int nPos, OBJID idxField, int nOpt, int nData);	bool EnhanceItem(OBJID idItem, OBJID idGem);
	CItem*	GetItem			(OBJID idItem);
	CItem*	GetItemByType	(OBJID idItemType);
	CItem*	GetEquipItem	(OBJID idItem);
	CItemPtr*	GetEquipItemPtr	(int nPosition);
	CItemPtr&	GetEquipItemRef	(int nPosition)						{ CItemPtr* ppEquip = GetEquipItemPtr(nPosition); MYASSERT(ppEquip); return *ppEquip; }
	CItem*	GetEquipItemByPos	(int nPosition);
	bool	AddItem			(CItem* pItem, bool bSynchro, bool bUpdate = true);
	bool	DelItem			(OBJID idItem, bool bSynchro);
	bool	DelItemPt		(OBJID idItem);
	CItem*	PopItem			(OBJID idItem, bool bSynchro, bool bUpdate);					void PushItem(CItem* pItem, bool bSynchro, bool bUpdate);	CItem* GetUnkItem(OBJID idItem);	CItem* GetStorageItem(OBJID idItem);	vector<CItem*>& GetItemSet();
	bool	TryItem			(OBJID idItem, int nPosition);	bool UseExpBall(OBJID idItem);	void GuideNotify(int nData);	void GuideAddTime(int nHours);
	bool	UseItem			(OBJID idItem, int nPosition, bool bSynchro);
	bool	UnEquipItem		(int nPosition, bool bSynchro);
	bool	SplitItem		(OBJID idItem, int nNum);
	bool	CombineItem		(OBJID idItem, OBJID idOther);
	CItem*	FindCombineItem	(OBJID idType);
	CItem*	CombineNewItem	(CItem* pItem, ItemInfoStruct* pInfo, bool bSynchro);
	bool	PickMapItem		(OBJID idItem);
	bool	DropItem		(OBJID idItem, int x, int y);
	bool	DropMoney		(int nMoney, int x, int y);
	bool	RepairItem		(OBJID idItem, bool bSynchro, bool bEquip);
	bool	RepairAll		(bool bSynchro);
	bool	IdentItem		(OBJID idItem, bool bSynchro);
	CItem*	GetTaskItem		(LPCTSTR szItemName);
	CItem*	GetTaskItemByType		(OBJID idItemType);
	int		GetWeight		();
	CItem*	FindSpaceTransSpell();
	void	AddEquipmentDurability		(int nPosition, int nAddValue);
	bool	MultiDelItem(OBJID idTypeFirst, OBJID idTypeLast, int nNum);
	bool	MultiCheckItem(OBJID idTypeFirst, OBJID idTypeLast, int nNum);
	bool	DecEquipmentDurability(bool bBeAttack, bool bMagic);
	bool	SpendItemByType	(OBJID idType, bool bSynchro);
	bool	SpendEquipItem	(int nSubType, int nNum, bool bSynchro);
	virtual bool	SpendArrow();
	bool	SpendItem(CItem* pItem, int nNum=1, int nPosition=ITEMPOSITION_BACKPACK, bool bSynchro=true);
protected:
	bool	EquipItem		(CItem* pItem, int nPosition, bool bSynchro);
	CItem*	UnEquipOnly		(int nPosition);
	void	UpdateWeight	();
	void	UpdateEquipmentDurability	();
protected:
	bool	CreateAllItem	(IDatabase* pDb);
	void	SaveItemInfo	();
	void	DeleteAllItem	();
	void	SendAllItemInfo	();
public:
	void	SendAllEquipInfoTo(CUser* pTarget);
	void	BuyItem			(OBJID idNpc, OBJID idType, int nCount);
	void	SellItem		(OBJID idNpc, OBJID idItem, DWORD dwData);	bool IsWeaponR562();
protected:
	bool	m_bUpdateWeight;
	int		m_nAllWeight;
	int		m_idxLastArrow;

public:
	void	ClearTaskID		()									{ m_setTaskID.clear(); m_setIterID.clear(); }
	void	SetTaskNpcID	(OBJID idNpc)						{ m_idTaskNpc = idNpc; }
	void	SetTaskItemID	(OBJID idItem)						{ m_idTaskItem = idItem; }
	OBJID	GetTaskItemID	()									{ return m_idTaskItem; }
	OBJID	GetTaskItemActionID()								{ return m_idTaskItemAction; }
	void	SetTaskItemActionID(OBJID idAction)					{ m_idTaskItemAction = idAction; }
	bool	TestTask		(CTaskData* pTask);
	bool	ProcessTask		(int idx, LPCTSTR pszAccept);
	bool	ProcessClientTask(OBJID idTask, LPCTSTR pszAccept);
	bool	DebugShowAction	()									{ m_bDebugAction = !m_bDebugAction; return m_bDebugAction; }
	int&	TaskIterator	()									{ return m_nTaskIterator; }
	int		PushTaskID			(OBJID idTask)					{ if(idTask!=ID_NONE && m_setTaskID.size() < MAX_MENUTASKSIZE) return m_setTaskID.push_back(idTask), m_setTaskID.size()-1; return INDEX_NONE; }

	int		PushTaskID			(OBJID idTask, OBJID idIter);
	CUserData* QueryDataForAction()								{ return &m_data; }
	void 	SetRecordPos	(OBJID idMap, DWORD dwX, DWORD dwY, BOOL bUpdate = false);

	void	AddTaskMask		(int idx);
	void	ClrTaskMask		(int idx);
	bool	ChkTaskMask		(int idx);

	bool	AcceptMercenaryTask(OBJID idTask);
	bool	CancelMercenaryTask(OBJID idTask);
	bool	CompleteMercenaryTask(OBJID idTask, OBJID idItem);

protected:
	OBJID	GetTaskID			(int idx)						{ if(idx >= 0 && idx < m_setTaskID.size()) return m_setTaskID[idx]; return ID_NONE; }
	bool	CheckItem			(CTaskData* pTask);
	void	SetIterID			(int idx)						{ if(idx >= 0 && idx < m_setIterID.size()) m_nTaskIterator = m_setIterID[idx]; }

protected:
	vector<OBJID>			m_setTaskID;
	vector<OBJID>			m_setIterID;
	OBJID					m_idTaskNpc;
	OBJID					m_idTaskItemAction;
	OBJID					m_idTaskItem;
	bool					m_bDebugAction;
	int						m_nTaskIterator;
	
public:
	WantedInfoStruct& WantedInfo(void)		{ return m_data.GetInfo().infoWanted; }
	void	ResetWantedInfo(void)	{ memset(&m_data.GetInfo().infoWanted, 0L, sizeof(WantedInfoStruct)); }
	OBJID&	PoliceWantedID()	{ return m_data.GetInfo().idPoliceWanted; }
	void	TeamHuntKill		(OBJID idTarget, int nTargetLev, const char* pszTargetName);
	bool	IsMyWantedTarget	(CUser* pTarget);

public:
	bool		SendAllMagicInfo()		{ return m_pMagic->SendInfoToClient(); }
	CMagic*		QueryMagic()			{ CHECKF(m_pMagic); return m_pMagic; }
	static bool	IsInCircle(POINT pos, POINT posCenter, int nRange)	{ return sqrt((double)(posCenter.x-pos.x)*(posCenter.x-pos.x) + (double)(posCenter.y-pos.y)*(posCenter.y-pos.y)) < nRange+.9; }
	bool		IsInFan(POINT pos, int nRange, POINT posCenter);
	bool		IsArrowPass(int x, int y, int nAlt=ARROW_FLY_ALT);
	bool		IsJumpPass(int x, int y, int nAlt);
	bool		EquipMagicItem(CItem* pItem, int nPosition);
	bool		UnequipMagicItem(int nPosition);
	void		CalcSupermanKills();
	void		SynchroSupermanOrder(int nOrder);
	int			GetSupermanAddSecs()			{ return m_nSupermanOrder; }
	void		QuerySupermanSecs();
	void		QuerySupermanList(int nIndex, OBJID idAction, int nNumber);
protected:
	CMagic*		m_pMagic;
	int			m_nCurrSupermanKills;
	int			m_nTopSupermanKills;
	int			m_nSupermanOrder;

public:
	bool		Transform(DWORD dwType, int nKeepSecs, bool bSynchro=true);
	CTransformation* QueryTransformation()		{ return m_pTransformation; }
	void		AbortTransform();
	bool		SynchroTransform();
protected:
	CTransformation*		m_pTransformation;
	CTimeOut				m_tTransformation;
public:	bool	TransformLook(DWORD dwType, int nKeepSecs, bool bSynchro);
public:
	bool		CallPet(OBJID idMonsterType, int x, int y, int nKeepSecs=0);
	CMonster*	QueryCallPet()			{ return m_pCallPet; }
	void		KillCallPet(bool bNow=false);
	OBJID		GetCallPetID();
protected:
	CAutoLink<CMonster>		m_pCallPet;
	CTimeOut				m_tCallPet;

public:
	void		SetSynProffer	(int nData)		{ if(m_pSyn) m_pSyn->SetProffer(nData); }
	bool		SendAllSynInfo()		{ return m_pSyn->SendInfoToClient(); }
	CSynAttr*	QuerySynAttr()			{ CHECKF(m_pSyn); return m_pSyn; }
	void		AwardSynWarScore(CNpc* pNpc, int nScore);
	bool		IsHostileSynMember	(CUser* pUser);
	CSyndicate* GetSyndicate	(void);
	int			QuerySynProffer	(void)	{ if(m_pSyn) return m_pSyn->GetProffer(); return 0; }
	int			QuerySynRank	(void)	{ if(m_pSyn) return m_pSyn->GetSynRankShow(); return RANK_NONE; }
protected:
	CSynAttr*		m_pSyn;
	CTimeOut		m_tIncRepute;
	CTimeOut		m_tIncPotential;
public:
	IStatus*	QueryStatus(OBJID idType)		{ CHECKF(m_setStatus); return m_setStatus->GetObj(idType); }
	IStatusSet*	QueryStatusSet()				{ CHECKF(m_setStatus); return m_setStatus; }
	void		SetCrimeStatus(int nStatus=STATUS_CRIME, int nTime=CRIME_KEEP_SECS);
protected:
	IStatusSet*	m_setStatus;

public:
	int			GetFriendAmount	()					{ return m_setFriend.size(); }
	CTradeBuddy*	GetTradeBuddy		(OBJID idBuddy);
	int				GetTradeBuddyAmount	();
	CTradeBuddy*	GetTradeBuddyByIndex(int idx);
	bool			DelTradeBuddy		(OBJID idBuddy);
	void			DeleteAllTradeBuddy	();
	void			SendTradeBuddyInfo	();
	CFriend*	GetFriendByIndex(int idx)			{ CHECKF(0 <= idx && idx < (int)m_setFriend.size()); return m_setFriend[idx]; }
	CFriend*	GetFriend		(OBJID idFriend);
	bool		AddFriend		(OBJID idFriend, LPCTSTR szFriendName);
	bool		DelFriend		(OBJID idFriend);
	void		BroadcastFriendsMsg	(CNetMsg* pMsg);
protected:
	bool		CreateAllFriend	(IDatabase* pDb);
	void		DeleteAllFriend	();
	void		SendFriendInfo	();

protected:

public:
	CEnemy*		QueryEnemy()						{ return m_pEnemy; }
protected:
	CJavaObj<CEnemy>	m_pEnemy;

public:
	void		SendAllWeaponSkillInfo	();
	void		AddWeaponSkillExp		(int nSkillType, int nExp);
	bool		CheckWeaponSubType		(int nSubType, int nNum=0);

	BOOL		AwardWeaponSkill	(int nSkillType, int nLev);
	CUserWeaponSkill* GetWeaponSkill(int nSkillType);

	DWORD		GetWeaponSkillLev	(OBJID idWeaponType);
	bool		UnlearnAllSkill		();
	bool		AddMoneySaved		(int nAdd, bool bSynchro);
protected:
	BOOL		CreateWeaponSkill	(IDatabase* pDb);
	bool		SaveAllWeaponSkillInfo	();

public:
	virtual bool	SpendMoney	(DWORD nMoney, bool bSynchro);
	virtual bool	GainMoney	(DWORD nMoney, bool bSynchro);
	bool			SaveMoney	(DWORD nMoney, bool bSynchro = false);
	bool			DrawMoney	(DWORD nMoney, bool bSynchro = false);
public:
	void			ProcessTick	(DWORD dwTime);
protected:
	deque<DWORD>	m_dequeServerTick;
	DWORD			m_dwFirstServerTick;
	DWORD			m_dwLastServerTick;

	DWORD			m_dwFirstClientTick;
	DWORD			m_dwLastClientTick;

	DWORD			m_dwLastRcvClientTick;

public:
	OBJID			GetWeatherID()					{ return m_idWeather; }
	void			SetWeatherID(OBJID id)			{ m_idWeather = id; }
	bool			SetWeather(int nType, int nIntensity, int nDir, DWORD nColor, DWORD nKeepSecs, DWORD nSpeedSecs);
protected:
	OBJID			m_idWeather;

public:
	void		SetEnergy			(int nEnergy)	{ m_data.m_Info.usEnergy = nEnergy; }
	int			GetEnergy			()				{ return m_data.m_Info.usEnergy; }
	int			GetMaxEnergy		();
	void		AddEnergy			(int nEnergy)	{ m_data.m_Info.usEnergy = ::CutRange(m_data.m_Info.usEnergy+nEnergy, 0, GetMaxEnergy()); }
	void		ResetEnergyInc		()				{ m_tEnergy.Update(); }
protected:
	CTimeOut		m_tEnergy;

public:
	void		SetLight			(int dwRGB, int nSecs=0);
	void		SendLight			();

public:
	CBooth*		QueryBooth()						{ return m_pBooth; }
	bool		CreateBooth(int nPosX, int nPosY, int nDir);
	void		DestroyBooth()						{ if(m_pBooth) m_pBooth->ReleaseByOwner(); m_pBooth = NULL; }
protected:
	CBooth*			m_pBooth;

public:
	bool			EnterMap		();
	bool			LeaveMap		();
	bool			SendItemSet		();
	bool			SendGoodFriend	();
	void			ResetUserPoint	();
public:
	bool		Rebirth(int nProf, int nLook, int nMetempsychosis);
protected:
	bool		CheckRebirth(int nProf, CRebirthData* pData);
	void		ResetUserAttrib(int nOldProf, int nOldLevel, int nNewProf, int nNewLevel, int nNewLook, int nMetempsychosis);
	void		ResetEquipLevel(int nNewLevel);
	int			GetRebirthAddPoint(int nOldProf, int nOldLevel, int nNewLevel, int nMetempsychosis);

public:
	bool			ChangeMapGroup	(PROCESS_ID idProcess, OBJID idMap, int nPosX, int nPosY);
	bool			JoinMap			(OBJID idMap, int nPosX, int nPosY);
	bool			AppendObjInfo	(INFO_ID nType, void* pInfo);
protected:
	bool			AppendItem		(ItemInfoStruct* pInfo);
	bool			AppendFriend	(FriendInfoStruct* pInfo);
	bool			AppendWeaponSkill	(UserWeaponSkillInfoStruct* pInfo);
	bool			AppendStatus	(StatusInfoStruct* pInfo);
	bool			LeaveMapGroup	();

public:
	CAutoLink<CUser>&	QueryLink	()					{ return m_link; }	CAutoLink<CUser>& QueryLink(const CAutoLink<CUser>&) { return m_link; }

public:
	bool	CreateSquareDeal		(CUser* pTarget);
	ISquareDeal*	QuerySquareDeal	()		{ m_ifaceSquareDeal.CheckDistance(); return m_ifaceSquareDeal.m_pSquareDeal; }

	class XUserSquareDeal : IUserSquareDeal
	{
	public:
		XUserSquareDeal()			{ m_pSquareDeal = NULL; }
		~XUserSquareDeal()			{ if(m_pSquareDeal) m_pSquareDeal->Release(); }
		void	CheckDistance();

	public:
		virtual bool	Create		(ISquareDeal* pSquareDeal);
		virtual ULONG	Release		();
		virtual CUser*	QueryUser	()					{ return This(); }

	public:
		virtual bool	AddItem		(CItem* pItem, int nAmount, int nAllWeight);
		virtual bool	AddMoney	(int nAllMoney);	virtual bool	AddEmoney	(int nAllEmoney);
		virtual void	ClickOK		();	virtual OBJID	GetAccountID	();	virtual DWORD	GetEmoney	();	virtual DWORD	GetMoney	();	virtual LPCTSTR	GetName	();

	protected:
		ISquareDeal*		m_pSquareDeal;

	private:
		CUser*		This()	{ return m_pParent; }
		CUser*		m_pParent;
		friend class CUser;
	}	m_ifaceSquareDeal;

	friend class CUser::XUserSquareDeal;

protected:
	enum { STEP_NONE=0, STEP_LOGIN, STEP_ENTERMAP };
	int				m_nStep;
	
protected:
	typedef vector<CFriend*>	FRIEND_SET;
	FRIEND_SET	m_setFriend;
	
protected:

	CItemPtr	m_pHelmet;
	CItemPtr	m_pNecklace;	
	CItemPtr	m_pArmor;	
	CItemPtr	m_pWeaponR;	
	CItemPtr	m_pWeaponL;	
	CItemPtr	m_pRingR;	
	CItemPtr	m_pRingL;	
	CItemPtr	m_pShoes;
	CItemPtr	m_pOvercoat;

	CTimeOut    m_arrTimeOutForEquipExp[ITEMPOSITION_EQUIPEND-ITEMPOSITION_EQUIPBEGIN];
	int			m_arrEquipIncExp[ITEMPOSITION_EQUIPEND-ITEMPOSITION_EQUIPBEGIN];
	CTimeOut    m_tEquipWarGhostExp;
	bool		m_bIsEnableWarGhostExp;

	CTimeOut	m_tTimeOnline;

	CUserData	m_data;

protected:
	int			m_nPosX;
	int			m_nPosY;
	int			m_nDir;
	int			m_nPose;
	int			m_nEmotin;
	int			m_dwXP;
	int			m_nSynDressArmorType;
	Array<int>	m_setSlowHealUp2Life;
	Array<int>	m_setSlowHealUp2Mana;
	CGameMap*	m_pMap;

protected:
	void UpdateEquipmentExp_Init();
	void UpdateEquipmentExp_Startup(DWORD dwPos,int nSecs);
	void UpdateEquipmentExp_Stop(DWORD dwPos);
	void UpdateEquipmentExp();
public:
	bool UseSkillAtk(OBJID idDes);
public:	
	void SetSynDressArmorType(int nArmorType);	
	bool ApplyAttackSyn(CNpc * pNpc);
	int GetAvailableSynFund();
	bool FlyToRandomPos();
	bool FlyToMainMap();
	bool IsEmbedGemType(int nGemType);
	DWORD GetLastLogin();
	void AddMoney(DWORD dwMoney);
	bool IsSynLeader();
	bool CheckBadge(int nItemType=0);
	bool DelAllBadge();
	bool AddBadge();
	void ProcessMsgAction(int nAction, int nData, OBJID id);
	DWORD		m_dwLastCoolShow;

private:
	IUserWeaponSkillSet*	m_pUserWeaponSkillSet;

protected:
	CBattleSystem*		m_pBattleSystem;
	CTimeOutMS			m_tFight;
	CTimeOut			m_tXp;
	CTimeOut			m_tPK;
	CTimeOut			m_tOnlineTraining;
	CTimeOut			m_tBattleEffect;
	CTimeOut			m_tContributeExp;
	CTimeOut			m_tUnk89C;
	CTimeOut			m_tAutoHealLife;
	CTimeOut			m_tEquipmentDurability;
	int					m_nFightRate;
	int					m_nFightPause;
protected:
	CTimeOut	m_tLock;
	CAutoLink<CUser>	m_link;

protected:
	SOCKET_ID	m_idSocket;
	PROCESS_ID	m_idProcess;
	DWORD		m_dwUnkF4;
public:
	bool		IsKickOut			()		{ return m_dwUnkF4 != 0; }
	void		SetKickOut			()		{ m_dwUnkF4 = 1; }
	DWORD		GetVirtue			()		{ return m_data.GetVirtue(); }
	OBJID		GetLastDelItemID	()		{ return m_dwUnk5B0; }
	CItem*		GetLastAddItem		()		{ CItem** ppEnd = m_setItem.empty() ? NULL : &m_setItem[0] + m_setItem.size(); return ppEnd[-1]; }
	bool		IsBackPackSpare		(int nNum)	{ return m_setItem.size() + nNum <= 40; }
protected:
protected:
	map<OBJID,CTimeOut*> m_mapEquipmentsUsed ;
	typedef pair <OBJID,CTimeOut*> PairID;	

	MYHEAP_DECLARATION(s_heap)
};

#endif
