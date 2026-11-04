#ifndef _ROLE_H
#define _ROLE_H

#include "BaseFunc.h"
#include "Path.h"
#include "MapObj.h"
#include "IRoleView.h"
#include "AniEffect.h"
#include "SayHello.h"
#include "FloatingStr.h"
#include "Game3DEffectEx.h"
#include "ZoomNum.h"
#include "EncryptData.h"
#include <vector>
const int _MAX_LEVEL			=130;
enum{
		_PLAYER_ID_NONE		= 0,
		_PLAYER_ID_MONSTER	= 1,
		_PLAYER_ID_PET		= 2,
		_PLAYER_ID_SYSNPC	= 3,
		_PLAYER_ID_DYNNPC	= 4,
		_PLAYER_ID_CALLPET	= 5,
		_PLAYER_ID_PLAYER	= 6,
	};
const OBJID	SCENEID_FIRST		= 000001;
const OBJID	SYSNPCID_FIRST		= 00001;
const OBJID	SYSNPCID_LAST		= 99999;
const OBJID	DYNANPCID_FIRST		= 100001;
const OBJID	DYNANPCID_LAST		= 199999;
const OBJID	SCENEID_LAST		= 299999;

const OBJID	NPCSERVERID_FIRST	= 400001;
const OBJID	MONSTERID_FIRST		= 400001;
const OBJID	MONSTERID_LAST		= 499999;
const OBJID	PETID_FIRST			= 500001;
const OBJID	PETID_LAST			= 599999;
const OBJID	NPCSERVERID_LAST	= 899999;

const OBJID TRAPID_FIRST		= 900001;
const OBJID MAGICTRAPID_FIRST	= 900001;
const OBJID MAGICTRAPID_LAST	= 989999;
const OBJID SYSTRAPID_FIRST		= 990001;
const OBJID SYSTRAPID_LAST		= 999999;
const OBJID TRAPID_LAST			= 999999;

const OBJID PLAYER_ID_FIRST		= 1000000;
const OBJID PLAYER_ID_LAST		= 1999999999;
const OBJID	CALLPETID_FIRST		= 700001;
const OBJID	CALLPETID_LAST		= 799999;

const OBJID CLIENT_ID_FIRST		= 4000000000;
const OBJID MOUSE_PLAYER_ID		= 4000000001;

const OBJID _IDMAX_NPC	=999999;

enum
{
	ROLE_ATTRIBUTE_TASK			= 1,
	ROLE_ATTRIBUTE_RECYCLE		= 2,
	ROLE_ATTRIBUTE_SCENE		= 4,
};
enum{
		_ROLE_NPC_NONE				= 0,
		_ROLE_SHOPKEEPER_NPC		= 1,
		_ROLE_TASK_NPC				= 2,
		_ROLE_STORAGE_NPC			= 3,
		_ROLE_TRUNCK_NPC			= 4,
		_ROLE_FACE_NPC				= 5,
		_ROLE_FORGE_NPC				= 6,
		_ROLE_EMBED_NPC				= 7,

		_ROLE_STATUARY_NPC			= 9,
		_ROLE_SYNFLAG_NPC			= 10,

		_ROLE_PLAYER				= 11,
		_ROLE_HERO					= 12,
		_ROLE_MONSTER				= 13,
		
		_ROLE_BOOTH_NPC				= 14,
		_SYNTRANS_NPC				= 15,
		_ROLE_BOOTH_FLAG_NPC		= 16,
		_ROLE_MOUSE_NPC				= 17,
		_ROLE_MAGICITEM				= 18,

		_ROLE_DICE_NPC				= 19,
		
		_ROLE_WEAPONGOAL_NPC		= 21,
		_ROLE_MAGICGOAL_NPC			= 22,
		_ROLE_BOWGOAL_NPC			= 23,

		_ROLE_TARGET_NPC			= 24,

		_ROLE_FURNITURE_NPC			= 25,
		_ROLE_CITY_GATE_NPC			= 26,
		_ROLE_NEIGHBOR_DOOR			= 27,
		_ROLE_CALL_PET				= 28,

		_TRAINPLACE_NPC				= 29,
		_AUCTION_NPC				= 30,
		_ROLE_STONE_MINE			= 31,

		_ROLE_PKGAME_NPC			= 32,
		_ROLE_3DFURNITURE_NPC		= 101,
		
		_ROLE_CITY_WALL_NPC			= 102,
		_ROLE_CITY_MOAT_NPC			= 103,

		SYN_NPC_WARLETTER			= 110,
	};

const int _COMMAND_NULL			=0;
const int _COMMAND_STOP			=28;
const int _COMMAND_STANDBY		=5;
const int _COMMAND_WALK			=14;
const int _COMMAND_RUN			=15;
const int _COMMAND_JUMP         =16;
const int _COMMAND_EMOTION      =6;
const int _COMMAND_ACTION		=7;
const int _COMMAND_ATTACK		=20;
const int _COMMAND_FOLLOW		=2;
const int _COMMAND_SHITHAPPEN	=26;
const int _COMMAND_DIE			=27;
const int _COMMAND_SPATTACK		=22;
const int _COMMAND_FAINT		=29;
const int _COMMAND_RUSHATK		=23;
const int _COMMAND_WOUND		=9;
const int _COMMAND_DEFEND		=10;
const int _COMMAND_WALKFORWARD	=17;
const int _COMMAND_RUNFORWARD	=18;
const int _COMMAND_LOCKATK		=24;
const int _COMMAND_PICKUP		=1;
const int _COMMAND_TEAMFOLLOW	=3;
const int _COMMAND_POSE			=4;
const int _COMMAND_INTONE		=19;
const int _COMMAND_BUMP			=25;
const int _COMMAND_MINE			=8;
const int _COMMAND_SHOOT		=21;
const int _COMMAND_LOCKBOOTH	=11;

const int _COMMAND_TAKEOFF		=12;
const int _COMMAND_LANDING		=13;
const int _COMMAND_FLOAT		=30;
const int _COMMAND_FLOAT_DIE	=32;

const int _COMMAND_CHARGEUP			=33;
const int _CPMMAND_CHARGEUP_SHOOT	=34;

const int _CMDSTATUS_BEGIN		=0;
const int _CMDSTATUS_DEPART		=1;
const int _CMDSTATUS_PROGRESS	=2;
const int _CMDSTATUS_CONTINUE	=3;
const int _CMDSTATUS_WAITING	=4;
const int _CMDSTATUS_OVER		=5;
const int _CMDSTATUS_ACCOMPLISH	=6;

const DWORD  				USERSTATUS_NORMAL			=0x00000000;
const DWORD  				USERSTATUS_TEAMLEADER		=0x00000001;
const DWORD  				USERSTATUS_DIE				=0x00000020;
const DWORD  				USERSTATUS_GHOST			=0x00000400;
const DWORD  				USERSTATUS_DISAPPEARING		=0x00000008;
const DWORD  				USERSTATUS_CRIME			=0x00000010;
const DWORD				USERSTATUS_XPFULL		=0x00000010;
const DWORD  				USERSTATUS_RED				=0x00004000;
const DWORD  				USERSTATUS_DARKRED			=0x00008000;
const DWORD  				USERSTATUS_SYNCRIME			=0x00000080;
const DWORD  				USERSTATUS_POISON			=0x00000100;
const DWORD				USERSTATUS_MAGICSHIELD	=0x00000100;
const DWORD  				USERSTATUS_INVISIBLE		=0x00000200;
const DWORD  				USERSTATUS_CANNOTMOVE		=0x00000400;
const DWORD  				USERSTATUS_LURKER			=0x00001000;
const DWORD  				USERSTATUS_DEFENCE1			=0x00002000;
const DWORD  				USERSTATUS_DEFENCE2			=0x00004000;
const DWORD  				USERSTATUS_DEFENCE3			=0x00008000;
const DWORD  				USERSTATUS_ATTACK			=0x00010000;
const DWORD  				USERSTATUS_ATKSPEED			=0x00020000;
const DWORD  				USERSTATUS_MAGICDAMAGE		=0x00040000;
const DWORD				USERSTATUS_SPSOLDIER	=0x00040000;
const DWORD  				USERSTATUS_MAGICDEFENCE 	=0x00080000;
const DWORD  				USERSTATUS_REFLECT			=0x00100000;
const DWORD  				USERSTATUS_REFLECTMAGIC		=0x00200000;
const DWORD  				USERSTATUS_SLOWDOWN1		=0x00400000;
const DWORD  				USERSTATUS_SLOWDOWN2		=0x00800000;
const DWORD				USERSTATUS_TORNADO		=0x00800000;
const DWORD  				USERSTATUS_TEAM_HEALTH		=0x01000000;
const DWORD  				USERSTATUS_TEAM_ATTACK		=0x02000000;
const DWORD  				USERSTATUS_TEAM_DEFENCE		=0x04000000;
const DWORD  				USERSTATUS_TEAM_SPEED		=0x08000000;
const DWORD				USERSTATUS_FLY			=0x08000000;
const DWORD				USERSTATUS_PRAY			=0x40000000;
const DWORD				USERSTATUS_PRAYFOLLOW	=0x80000000;
const DWORD				USERSTATUS2_CURSE		=0x00000001;
const DWORD				USERSTATUS2_BLESS		=0x00000002;
const DWORD  				USERSTATUS_TEAM_EXP			=0x10000000;
const DWORD				USERSTATUS_CHARGEUP		=0x10000000;
const DWORD  				USERSTATUS_TEAM_SPIRIT		=0x20000000;
const DWORD  				USERSTATUS_TEAM_CLEAN		=0x40000000;
const DWORD  				USERSTATUS_SMOKE			=0x80000000;

enum {ANIEFFECT_NORMAL=0, ANIEFFECT_CASTMAGIC=1, ANIEFFECT_BLASTMAGIC=2};
enum {ROLE_PLAYER = 0, ROLE_MONSTER = 1};
enum {SEX_NONE = 0, SEX_MALE, SEX_FEMALE, SEX_ASEXUAL, SEX_ALL};
enum 
{	
		RACE_NONE	= 0,
		RACE_HUMAN	= 2,
		RACE_SPRITE = 3,
		RACE_DARKSPRITE = 1,
};
enum{
		PROFESSIONAL_ENCHANTER	=	10,
		PROFESSIONAL_WARRIOR	=	20,
		PROFESSIONAL_KORADJI	=	30,
	};

const int OFFSET_NAMESHOW	= 90;
const int OFFSET_SYNDICATESHOW	= 102;
const DWORD TIME_INBATTLE	=10000;

class I3DObj;
class I3DMotion;
class C3DRoleData;
class CCommand {
public:
	int		iType;
	int		iStatus;

	union {
		char	szCmd[256];

		struct {
			OBJID	idTarget;
			CMyPos	posTarget;
			int		nDir;
			char	szString[_MAX_NAMESIZE];
			DWORD	dwData;
			int		nData;
			BOOL	bData;
			BOOL	bHeroSendMsg;
			int		nFrameStart;
			int		nFrameEnd;
			int		nFrameInterval;
			BOOL	bAddUp;
			DWORD	dwIndex;
		};
	};

	void Format(const char* fmt, ...) { vsprintf( szCmd, fmt, (char*) ((&fmt)+1) ); }
	CCommand(void) {memset(this, 0L, sizeof(CCommand));}
};

class CPath;
typedef struct{
	OBJID	id;
	USHORT	usLook, usHair, usHead, usFace, usPose;
	int		nTransformLook;
	OBJID	idArmorType, idRWeaponType, idLWeaponType, idMountType, idArmetType;
	OBJID	idMiscType;
	OBJID	idMantleType;
	char	szName[_MAX_NAMESIZE];

	int		iSize, iAddSize, nSex;
	int		iDirection, iAngle;
	int		iZoomPercent;

	CMyPos	posRush;
	CMyPos	posWorld, posScr;
	CMyPos	posActionBegin, posActionEnd;
	int		iActType;
	int		iActFrameIndex;
	int		iActFrameAmount;
	DWORD	timeFirstFrame;
	DWORD	timeFrameInterval;

	CPath		objPath;
	CCommand	cmdProc;
	CMySize		sizeBase;
	int			nFlyOffset;
	int			nFlyOffsetGoal;
	CMyProtectPos	posMap;
}CRoleInfo;


#include <list>
using namespace std;

typedef list<UINT> LIST_DIR;

class CRole :public CMapObj
{
private:
	void	StaticAction(int nAction, int nDir=-1);

	BOOL	Step		(void);
	BOOL	Walk		(void);
	BOOL	WalkForward	(void);
	BOOL	Run			(void);
	BOOL	RunForward	(void);
	BOOL	Jump		(void);
	BOOL    Emotion		(void);
	BOOL	Attack		(void);
	BOOL	RushAtk		(void);
	BOOL	InTone		(void);
	BOOL	LockAtk		(void);
	BOOL	TeamFollow	(void);
	BOOL	Follow		(void);
	BOOL	Action		(void);
	BOOL	Die			(void);
	BOOL	DieNormal	(void);
	BOOL	DieDelay	(void);
	BOOL	DieFloat	(void);

	BOOL	PickUp		(void);
	BOOL	Pose		(void);
	BOOL	Bump		(void);
	BOOL	Mine		(void);
	BOOL	LockBooth	(void);
	BOOL	TakeOff		(void);
	BOOL	Landing		(void);
	BOOL	Wound		(void);
	BOOL	DieFlying	(int nDistance);
	BOOL	IsInBattle	(void)	{return ::TimeGet() - m_tmLastAtk < TIME_INBATTLE;}
	static BOOL	IsAttackAction(int nAction)	{return (nAction >= _ACTION_ATTACK0 && nAction <= _ACTION_ATTACK3) || nAction == _ACTION_BUMP || nAction == _ACTION_FLY_ATTACK || nAction == _ACTION_FAST_SHOOT;}
	BOOL	IsMapAttainable	(int iMapX, int iMapY);
	BOOL	IsMapReached	(void);
	int		GetStepDir		(void);
protected:
	int		GetOffsetY();
	BOOL	StandBy		(void);

private:
	int		GetJumpHeight	();
	int		GetJumpShadowHeight();
	DWORD	GetJumpFrameInterval();
	int     GetWalkHeight	();

public:
	CRole(void){Init();}
	virtual ~CRole(void){Destroy();}
	
	void	Init			(void);

	BOOL	Create	(UINT uLook=1, int nViewType = _ROLE_VIEW_ROLE);
	BOOL	Create	(UINT uLook, int nViewType, OBJID id, CMyPos pos);
	void	Destroy	(void);
		
	virtual void	GetWorldPos	(int& iPosX, int& iPosY)	{iPosX=m_Info.posWorld.x, iPosY=m_Info.posWorld.y;}
	virtual void	GetScrPos	(int& iPosX, int& iPosY)	{iPosX=m_Info.posScr.x, iPosY=m_Info.posScr.y;}
	virtual void	GetPos		(int& iPosX, int& iPosY)	{iPosX=m_Info.posMap.x, iPosY=m_Info.posMap.y;}
	
	virtual void	GetSize		(int& iWidth, int& iHeight)	{iWidth=iHeight=m_Info.iSize;}
	virtual void	Show		(void);
	virtual void	Process		(void* pInfo);

	
	virtual BOOL	ProcessAction	(void);

	virtual BOOL	IsMouseFocus(int evtMouse=_MOUSE_NONE);
	virtual BOOL	IsVisible(void);
	virtual void	SetVisible(BOOL bVisible)	{m_bVisible = bVisible;}

	virtual int		GetAtkRange	(void) { return 1; }
	
	void	SetActionData	(void);
	void	ResetActionData	(void);

	DWORD   GetFrameInterval(int nActType);
	
	DWORD	GetDistance	(CRole* pRole);
	BOOL	IsMoving	(void);
	BOOL	IsWalking	(void);
	BOOL	IsRunning	(void);
	BOOL	IsStandBy	(void);
	BOOL	IsJumping	(void);
	BOOL	IsIdle		(void);
	BOOL	IsBumpBackEnable	(void);

	
	void	AddStepDir	(UINT uDir)	{m_lstStepDir.push_back(uDir%8);}
	void	ResetStepDir();
	
	BOOL	SetPos		(int iPosX, int iPosY);
	void	SetScrPos	(int iPosX, int iPosY)	{m_Info.posScr.x = iPosX;m_Info.posScr.y = iPosY;}
	void	SetRushPos	(int iPosX, int iPosY)	{m_Info.posRush.x = iPosX, m_Info.posRush.y = iPosY;}
	void	ClrRushPos	(void)	{m_Info.posRush.x = m_Info.posRush.y = 0;}
			
	void	Say			(const char* pszWords);
	void	ShowName	(void);
	void	AddZoomNum		(int nNum, int nType, DWORD dwColor, DWORD dwDelay, int nRandOffset = 0)	{m_objZoomNum.Add(nNum, nType, dwColor, dwDelay, nRandOffset);}
	DWORD	GetActionDelayInfoKey	(CRole* pTarget);
	DWORD	GetWeaponInfoKey		(CRole* pTarget);
	int		GetHitDir				(CRole* pTarget);

	void	SetArmor	(OBJID idType);
	void	SetArmet	(OBJID idType);
	void	SetRWeapon	(OBJID idType);
	void	SetLWeapon	(OBJID idType);
	void	SetMount	(OBJID idType);
	void	SetMisc		(OBJID idType);
	void	SetMantle	(OBJID idType);
	BOOL	HaveBow		();
	OBJID	GetArmor	(){return m_Info.idArmorType;}
	OBJID	GetArmet	(){return m_Info.idArmetType;}
	OBJID	GetRWeapon	(){return m_Info.idRWeaponType;}
	OBJID	GetLWeapon	(){return m_Info.idLWeaponType;}
	OBJID	GetMount	(){return m_Info.idMountType;}
	OBJID	GetMisc		(){return m_Info.idMiscType;}
	OBJID	GetMantle	(){return m_Info.idMantleType;}
	void	SetRGBA		(float r, float g, float b, float a);

public: 
	enum {SHADOW_NONE, SHADOW_SIMPLE, SHADOW_REAL};
	static int s_nShadowType;
	
public:
	int		GetActionType	(void)	{return m_Info.iActType;}
	BOOL	IsAttacking		(void);
	void	GetActionStartPos	(int& nWorldX, int& nWorldY)	{nWorldX=m_Info.posActionBegin.x, nWorldY=m_Info.posActionBegin.y;}
	void	GetActionEndPos		(int& nWorldX, int& nWorldY)	{nWorldX=m_Info.posActionEnd.x, nWorldY=m_Info.posActionEnd.y;}
	
	void	GetPrePos	(int& nPosX, int& nPosY);
	int		GetDir		(CMyPos posTarget) const; 	
	int		GetDir		(CRole* pTarget) const;

public:
	void	SetStatus	(DWORD dwStatus, int nIndex = 0);
	void	ClrStatus	(DWORD dwStatus, int nIndex = 0);
	DWORD	GetStatus	(void)	{return m_dwStatus[0];}
	BOOL	TestStatus	(DWORD dwStatus, int nIndex = 0);

protected:
	DWORD	m_dwStatus[2];

public:

	BOOL	AddAniEffect	(const char* pszEffect);
	BOOL	AddAniEffectEx	(const char* pszEffectTitle, int nTimes, DWORD dwInterval, int nShowWay, DWORD dwLoopInterval, int nOffsetX, int nOffsetY, int nExigence, DWORD dwDelay);
	void	ClrAniEffect	(const char* pszEffect=NULL);
	void	AddScrEffect	(const char* pszEffect);
	void	DelScrEffect	(const char* pszEffect);
	void	ShowScrEffect	(void);

	void	ShowAniEffect	(int nPosX, int nPosY);
	void	ProcessAniEffect();
	BOOL	TestAniEffect(const char* pszEffect);
	BOOL	TestEffect	(const char* pszIndex)	{if (m_objEffect) return m_objEffect->TestEffect(pszIndex); return false;}
private:
	DEQUE_EFFECT	m_dequeAniEffect;

public:
	virtual DWORD	SetCommand	(CCommand* pCommand);
	virtual BOOL	ProcessCommand	(void);

	void		InitCommand	(void);
	CCommand*	GetCommand	(void)	{return &(m_Info.cmdProc);}

	int			GetCommandType(void);
	BOOL		GetCommandFlag(void){return m_Info.cmdProc.bData;}
	int			GetLastCommand(void)	{return m_Info.cmdProc.iType;}
	int			GetCommandStatus(void)	{return m_Info.cmdProc.iStatus;}

public:
	BOOL	IsNpc(void) {return GetID() <= _IDMAX_NPC;}
	BOOL	IsPlayer(void) {return (GetID() >= PLAYER_ID_FIRST && GetID() <= PLAYER_ID_LAST);}

	BOOL	IsMonster(void) {return (GetID() >= MONSTERID_FIRST && GetID() <= MONSTERID_LAST);}
	
	BOOL	IsGuard(void);
	BOOL	IsPet(void) {return (GetID() >= CALLPETID_FIRST && GetID() <= CALLPETID_LAST);}
	BOOL	IsMyPet(void);
	void	SetID		(OBJID id)		{m_Info.id =id;}
	OBJID	GetID		(void) const	{return m_Info.id;}
	
	void    AllotRes	();
	void    SetLook		(USHORT usLook);
	USHORT	GetLook		(void) const	{return m_Info.usLook;}

	void    SetHair		(USHORT usHair);
	USHORT	GetHair		(void) const	{return m_Info.usHair;}

	void    SetHead		(USHORT usHead);
	USHORT	GetHead		(void) const	{return m_Info.usHead;}

	void    SetFace		(USHORT usFace)	{m_Info.usFace=usFace;}
	USHORT	GetFace		(void) const	{return m_Info.usFace;}

	void    SetPose		(USHORT usPose)	{m_Info.usPose=usPose;}
	USHORT	GetPose		(void) const	{return m_Info.usPose;}

	void	SetDir		(int nDir);
	int		GetDir		(void) const	{return m_Info.iDirection;}

	const char*	GetName	(void) const	{return m_Info.szName;}
	void	SetName		(const char* pszName);


	int		GetAddSize	(void) const	{return m_Info.iAddSize;}
	void	SetAddSize	(int nSize)		{m_Info.iAddSize=nSize;}

	int		GetZoomPercent	(void) const	{return m_Info.iZoomPercent;}
	void	SetZoomPercent	(int nPercent)	{m_Info.iZoomPercent=nPercent;}

	void	AddFStr(char* pszStr, int nMaxHeight=64, DWORD dwColor=0x00ffffff, int nStepHeight = 3);
	void	SetActionCmd (int nAction, int nDir=-1, DWORD dwType=0, BOOL bData = false);
	void	SetIntoneCmd (int nAction, int nDir=-1, DWORD dwType=0, BOOL bData = false);
	void	Transform(int nLook);
	void	SetWorldPos(CMyPos posWorld){m_Info.posWorld = posWorld;}
	int		GetFrameIndex(){return m_Info.iActFrameIndex;}

	int		GetSex(){return m_Info.nSex;}
	void	SetSex(int nSex){m_Info.nSex = nSex;}
	


	int		GetViewType(){return m_nViewType;}

	static  int		GetIDType(OBJID id);
	int		GetRoleType(){return m_nRoleType;}
	void	SetRoleType(int nRoleType){m_nRoleType = nRoleType;}
	void	SetRoleAttribute(DWORD dwAttribute){m_dwRoleAttribute = dwAttribute;}
	DWORD	GetRoleAttribute(){return m_dwRoleAttribute;}
	void	SetFlyOffset(int nOffset);
	int		GetFlyOffset();
	void	SetFlyOffsetGoal(int nOffset);
	int		GetFlyOffsetGoal();
	void	ShootArrow(OBJID idTarget, int nHpLose);
	void	FlashLastAttackTime(){m_tmLastAtk = ::TimeGet();}
protected:
	CRoleInfo		m_Info;
	int				m_nType;
	int				m_nViewType;
	int				m_nRoleType;
	DWORD			m_dwRoleAttribute;
	DWORD			m_tmDisappear;
	DWORD			m_tmLastAtk;
	int				m_nLastAttackAction;
	LIST_DIR		m_lstStepDir;
	BOOL			m_bVisible;
	int				m_nPreAction;
public:
	IRoleView*		m_pIRoleView;
	CSayHello		m_objSayHello;
	CFloatingStr	m_objFStr;
	CZoomNum		m_objZoomNum;
	IGame3DEffect*	m_objEffect;
	IGame3DEffect*	m_objScrEffect;


public:
	static float s_fBaseScale;
public:
	virtual int  GetObjType()               {return m_nType;}
	virtual BOOL IsFocus()                  {return false;}
	virtual void GetWorldPos(CMyPos& posWorld){posWorld = m_Info.posWorld;}
	virtual void GetPos(CMyPos& posCell)	{posCell = m_Info.posMap;}
	virtual void SetPos(CMyPos posCell)		{SetPos(posCell.x, posCell.y);}
	virtual void GetBase(CMySize& infoSize)	{infoSize.iWidth = 1; infoSize.iHeight = 1;}
	virtual void SetBase(CMySize& infoSize)	{m_Info.sizeBase = infoSize;}
};



#endif