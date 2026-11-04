#if !defined(AFX_HERO_H__E1E1F17C_8E9A_439A_B54A_E7A6D3213111__INCLUDED_)
#define AFX_HERO_H__E1E1F17C_8E9A_439A_B54A_E7A6D3213111__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetworkDef.h"
#include "Player.h"
#include "Pet.h"
#include "Friend.h"
#include "Enemy.h"
#include "Item.h"
#include "Trade.h"
#include "Package.h"
#include "Bank.h"
#include "XpCounterDown.h"
#include "KillCounter.h"
#include "WeaponSkill.h"
#include "TaskDialog.h"
#include "Team.h"
#include "Professional.h"
#include "Magic.h"
#include "MyTimer.h"
#include "ImageString.h"
#include "Booth.h"
#include "BoothManager.h"
#include "Bulletin.h"
#include "Dummy.h"
#include "DiceManager.h"
#include "Check.h"
#include <list>
#include <map>
const int _MAGIC_DECDELAY_PER_LEVEL		= 100;
const int _MAGIC_DELAY					= 600;

const int _MAX_FRIEND_AMOUNT	= 50;
const int _MAX_HEROITEM			= 40;
const int _POWERCOST_JUMP		= 10;

enum {_DEL_PT = false, _DEL_OBJ = true};

enum {PKMODE_FREE=0, PKMODE_MONSTERONLY, PKMODE_TMEXCLUDE, PKMODE_ARRESTMENT, PKMODE_SYNDICATE};

enum {REJECT_FRIEND=0, REJECT_TRADE=1, REJECT_TEAM=2, REJECT_TEACHER =3 ,REJECT_STUDENT = 4};

#include <deque>
typedef deque<char*> DEQUE_STRING;

typedef struct
{
	int		ucRank;
	BOOL	bOnLine;
	char	szProffer[_MAX_NAMESIZE];
	char	szName[_MAX_NAMESIZE];
	char	szRank[_MAX_NAMESIZE];
}SynMemberInfo;
typedef deque<SynMemberInfo*> DEQUE_SYNMEMBER;

const int _LEVELEXP_AMOUNT = 129;

typedef struct
{
	DWORD	dwExp;
	DWORD	dwExpUnit;
} LevelExpInfo;

typedef struct
{
	int		nIndex;
	float	fValue;
	int		nData;
} TableRecord;
typedef map<int, TableRecord>		MAP_TABLERECORD;
typedef map<int, MAP_TABLERECORD>	MAP_TABLE;
const int _TABLERECORD_NONE = 999;

typedef struct
{
	BYTE			btType;
	list<OBJID>		setItem;
} FUSE_INFO;

class CHero : public CPlayer
{
	friend struct HeroProbe;
public:
	CHero();
	virtual ~CHero();

	void	Init	(void);
	void	Reset	(void);

public:
	void	Walk			(int nPosX, int nPosY);
	void	Run				(int nPosX, int nPosY);
	void	Jump			(int nPosX, int nPosY);
	void	Direction		(int nPosX, int nPosY);

	void	Shoot			(CPlayer* pTarget, OBJID idBullet, BOOL bForced);
	void	Attack			(CPlayer* pTarget, BOOL bForced=false);
	void	RushAttack		(CPlayer* pTarget, BOOL bForced=false);
	void	SkillAttack		(CPlayer* pTarget, OBJID idSkillType, BOOL bForced=false);
	void	Poison			(CPlayer* pTarget, OBJID idItem);
	void	Dart			(CPlayer* pTarget, OBJID idItem);
	void	Steal			(CPlayer* pTarget);
	void	TeamFollow		();

	void	Emotion			(int nEmotionType, CPlayer* pTarget=NULL, BOOL bFollow=false);
	void	Stop			(void);

	void	DeleteRole		(DWORD dwData = 0);

	void	SetAlignPos(int nPosX, int nPosY);
	CMyPos	GetAlignPos(void)	{return m_posAlign;}

	DWORD	SetCommand		(CCommand* pCommand);
	BOOL	ProcessCommand	();
private:
	void	OnAttackSent	(OBJID idTarget, int nKind, DWORD dwData);
public:

private:
	CKoCounter*		m_pKoCounter;
	CKillCounter*	m_pKillCounter;
	int		m_nKillCount;
	BOOL	m_bTaskKillCount;
	int		m_nKillCountPending;
	int		m_nTaskKillCount;
public:
	OBJID	m_idAccount;

public:
	BOOL	ProcessChgMap	(void);

	void	Process(void* pInfo);
	void	Show	(void* pInfo);
	void	ShowXp	(void);
	void	ProcessLuckyTime(void);
	void	ShowLuckyTime(void);
	void	SetBlessTime(DWORD dwSeconds);
	void	ShowBless(void);
	void	SetCurseTime(DWORD dwSeconds);
	void	ShowCurse(void);
	void	SetDblExpTime(DWORD dwSeconds);
	void	ShowDblExp(void);

public:
	void	Talk			(CPlayer* pTarget, const char* pszWords, const char* pszEmotion=NULL, DWORD dwColor=0xffffff, unsigned short unAttribute=_TXTATR_NORMAL, unsigned short unStyle=_TXT_NORMAL);
	void	Talk			(const char* pszTarget, const char* pszWords, const char* pszEmotion=NULL, DWORD dwColor=0xffffff, unsigned short unAttribute=_TXTATR_NORMAL, unsigned short unStyle=_TXT_NORMAL);

private:
	DWORD	m_dwLastTalk;

public:
	void	Mine	(void);
	BOOL	IsMiner		(void);

public:
	void	ImproveItem(OBJID idItem, OBJID idTreasure);
	void	UpLevelItem(OBJID idItem, OBJID idTreesure);

	void	EmbedGem	(OBJID idItem, OBJID idGem, int nPos);
	void	TakeOutGem	(OBJID idItem, int nPos);

	BOOL	CombinItem	(OBJID idItemSource, OBJID idItemTarget);
	BOOL	SplitItem	(OBJID idItem, int nAmount);
	void	SendDataArray(OBJID id0, OBJID id1, OBJID id2, OBJID id3, OBJID id4);
	void	EnchantItem(OBJID idItem, OBJID idGem);
	void	FuseItem(OBJID idItem, FUSE_INFO infoFuse);

public:
	void	AllotPoint(int nForce, int nSpeed, int nHealth, int nSoul);
	void	DoLevelUp();
	DWORD	GetNextLevelExp();
	DWORD	GetDayBonus();
	void	GetLevelForecast(int& nLevel, int& nPercent);
	void	SetDayBonus(DWORD dwDayBonus)	{m_dwDayBonus = dwDayBonus;}

private:
	DWORD	m_dwMoney;
	DWORD	m_dwEmoney;

	DWORD	m_dwAutoAllot;
	DWORD	m_dwSuperSoldierTicks;
	CMyProtectPos	m_posAlign;
public:

public:
	void	LockAttack		(CPlayer* pTarget, BOOL bForced=false, BOOL bMove = true);

	BOOL	m_bFirstAtk;

public:
	enum {XPFULL_TIME = 20000,KILLAWARD_XPTIME = 0};

	void	BurstXp();
	void	ResetXpFullTime();
	BOOL	AddXp(int nXp);
	void	ProcessXp	(void);

private:
	CXpCounterDown	m_objXpCounterDown;
	DWORD	m_dwXpStartTime;
	CMyTimer	m_tmXpAdd;
public:
	void	AddKoCount	();
	void	BeginTaskKillCount	();
	void	EndTaskKillCount	();
	void	SetTaskKillCount	(int nCount);
	void	SetKillCountPending	(int nCount);
	BOOL	IsTaskKillCount		()	{return m_bTaskKillCount;}
	int		GetTaskKillCount	()	{return m_nTaskKillCount;}

	enum {KILL_AWARD_SPSOLDIER = 800, KILL_AWARD_TORNADO = 1000};

	DWORD	m_dwSuperSoldierBegin;
	DWORD	m_dwSuperSoldierBeginCopy;
	DWORD	m_dwSuperSoldierTail;

	DWORD	m_dwKoCount;
	DWORD	m_dwKoScale;

public:
	void	ProcessReborn	(void);
	DWORD	m_timeDie, m_timeGhost;

private:
	DWORD	m_dwTimeSynWarReportLastFlash;
public:
	void	JoinSynMember	(CPlayer* pTarget);
	void	AcceptSynMember	(CPlayer* pTarget);
	void	TitleSynMember	(CPlayer* pTarget, const char* pszTitle);
	void	KickoutSynMember(CPlayer* pTarget);
	void	QuitSyn			(void);
	void	SynContribute	(DWORD dwMoney);
	int		GetSynWarReportAmount ();
	char*	GetSynWarReportLine(int nIndex);
	void	ClearSynWarReport();
	void	AddSynWarReport(char* pszLine);
	void	ShowSynWarReport();
	void	UniteSubSyn(OBJID idSource, OBJID idTarget);
	void	QuerySynMember(char* pszName);
	void	KickoutSynMember(char* pszTargetName);
	void	UniteSyn(OBJID idSource, OBJID idTarget);
private:
	DEQUE_STRING m_dequeSynWarReport;

public:
	void	Court			(CPlayer* pTarget);
	void	Marriage		(CPlayer* pTarget);
	void	Divorce			(CPlayer* pTarget);

public:
	CItem*	GetItem			(OBJID id, BOOL bExcludeEquipment = true);
	BOOL	AddItem			(CItem* pItem);

	int		GetItemAmount	()	{return (int)m_dequeItem.size();}
	CItem*	GetItemByIndex	(UINT uIndex);

	BOOL	DelItem			(OBJID id, BOOL bDelete=_DEL_PT);
	void	ClearItem		(void);
	DWORD	GetCarryingCapacity();
	DWORD	GetItemTotalWeight();

	BOOL	TryItem			(CItem* pItem);
	void	UseItem			(OBJID id);
	void	RepairItem		(OBJID id);
	void	SellItem		(OBJID id);
	void	BuyItem			(OBJID idType);
	BOOL	DropItem		(OBJID id, int x, int y);
	void	PickUpItem		(OBJID id, int x, int y);

	DWORD	GetMoney(){return m_dwMoney;}
	void	SetMoney(DWORD dwMoney){m_dwMoney = dwMoney;}
	DWORD	GetEmoney(){return m_dwEmoney;}
	void	SetEmoney(DWORD dwEmoney){m_dwEmoney = dwEmoney;}

private:
	DEQUE_ITEM	m_dequeItem;

public:
	CItem*	GetEquipment(int nPosition)	{ MYASSERT(nPosition-1>=0 && nPosition-1<_MAX_EQUIPMENT); return m_pEquipment[nPosition-1]; }
	void	SetEquipment(int nPosition, CItem* pItem);
	BOOL	CanEquip(int nPosition, CItem* pItem) const;

	BOOL	EquipItem	(int nPosition, OBJID idItem);
	BOOL	UnequipItem	(int nPosition);

	CItem*	SearchEquipment	(OBJID idItem);

private:
	CItem*	m_pEquipment[_MAX_EQUIPMENT];

public:
	DWORD	GetMaxAtk();
	DWORD	GetMinAtk();
	DWORD	GetDef();
	DWORD	GetMagicDef();

	DWORD	GetMaxLife();
	DWORD	GetMaxMana();
	DWORD	GetMaxXP();

	void	SetAutoAllot(DWORD dwAutoAllot){m_dwAutoAllot = dwAutoAllot;}
	DWORD	GetAutoAllot(void) {return m_dwAutoAllot;}

	DWORD	m_dwNobility;

	int		GetAtkRange	(void);

private:
	DEQUE_FRIEND		m_setFriend;
	OBJID				m_idFriendQuery;
	DWORD				m_dwFriendBeginQuery;
	BOOL				m_bFriendQueryBusy;
	BOOL				m_bFriendQueryReturn;
	OBJID				m_idFriendLastApply;
	DWORD				m_dwReserved4294[4];
public:
	OBJID	GetLastFriendApply	(void)	{return m_idFriendLastApply;}
	BOOL	AddFriend		(CFriend* pFriend);
	BOOL	DelFriend		(OBJID id);
	void	QueryFriend	(OBJID id);
	void	ProcessFriend();
	void	QueryFriendReturn(){m_bFriendQueryReturn = true;}
	OBJID	GetCurrentQueryFriendID(){return m_idFriendQuery;}
	void ClearFriendList();

	BOOL MakeFriend		(OBJID id);
	BOOL BreakOffFriend	(OBJID id);

	int  GetFriendAmount();
	CFriend* GetFriend	(OBJID id);
	CFriend* GetFriendByIndex	(int nIndex);
	CFriend* GetFriendByName	(const char* pszName);
	void LeaveWords2Friend(OBJID idFriend, char* pszWords);

private:
	DEQUE_ENEMY			m_setEnemy;
	OBJID				m_idEnemyQuery;
	DWORD				m_dwEnemyBeginQuery;
	BOOL				m_bEnemyQueryBusy;
	BOOL				m_bEnemyQueryReturn;
public:
	BOOL	AddEnemy		(CEnemy* pEnemy);
	BOOL	DelEnemy		(OBJID id, BOOL bSendMsg = true);
	void	QueryEnemy		(OBJID id);
	void	ProcessEnemy	();
	void	QueryEnemyReturn(){m_bEnemyQueryReturn = true;}
	OBJID	GetCurrentQueryEnemyID(){return m_idEnemyQuery;}
	void	ClearEnemyList();

	int			GetEnemyAmount();
	CEnemy*	GetEnemy(OBJID id);
	CEnemy*	GetEnemyByIndex	(int nIndex);
	CEnemy*	GetEnemyByName	(const char* pszName);

public:
	void	CloseTrade(BOOL	bSuc);
	void	OpenTrade(OBJID idPlayer);
	BOOL	IsTradeActive();

	DWORD	GetHeroTradeMoney();
	DWORD	GetPlayerTradeMoney();
	CItem*	GetHeroTradeItemByIndex(int nIndex);
	CItem*	GetPlayerTradeItemByIndex(int nIndex);
	int		GetHeroTradeItemAmount();
	int		GetPlayerTradeItemAmount();
	char*	GetPlayerTradeName();
	char*	GetApplyTradeName();
	DWORD	GetPlayerTradeLevel();
	void	SetPlayerTradeLevel(DWORD dwLevel);
	OBJID	GetPlayerTradeID();

	void	SetPlayerTradeMoney(DWORD dwMoney);
	void	SetHeroTradeMoney(DWORD dwMoney);
	void	AddHeroTradeMoney(DWORD dwMoney);
	DWORD	GetPlayerTradeEmoney();
	DWORD	GetHeroTradeEmoney();
	void	SetPlayerTradeEmoney(DWORD dwEmoney);
	void	SetHeroTradeEmoney(DWORD dwEmoney);
	void	AddHeroTradeEmoney(DWORD dwEmoney);
	void	AddHeroTradeItem(CItem* pItem);
	void	AddPlayerTradeItem(CItem* pItem);
	void	TradePlayerAgree();
	void	AddApply(OBJID idPlayer);
	void	AddItemFalse(OBJID idItem);

	void	ApplyTrade(CPlayer* pPlayer);
	void	AcceptTrade();
	void	AgreeTrade();
	void	QuitTrade();

public:
	void	OpenPackage(OBJID idPackage, int nType);
	int		GetPackageItemAmount();
	CItem*	GetPackageItemByIndex(int nIndex);
	void	AddPackageItem(CItem* pItem);
	int		GetPackageMaxItemAmount();
	void	QueryPackage();
	void	CheckInItem(OBJID idItem);
	void	CheckOutItem(OBJID idItem);
	int		GetPackageType();
	void	ResetPackage();
	void	DelPackageItem(OBJID idItem);
	void	ClosePackage();

public:
	void	DrawMoney		(DWORD dwMoney);
	void	SaveMoney		(DWORD dwMoney);
	DWORD	GetSavedMoney	();
	void	SetSavedMoney	(DWORD dwMoney);
	void	QueryMoney		();
	void	OpenBank		(OBJID idBank);
	void	CloseBank		();
	BOOL	DropMoney		(DWORD dwMoney, int x, int y);

public:
	void			DestroyTaskDialog();
	void			AddTaskDialogLine(int nType, DWORD dwData, int nIndex, char* szStr, CMyPos posLine);
	int				GetTaskDialogLineAmount();
	TaskDialogLine*	GetTaskDialogLineByIndex(int nIndex);
	void			AddTaskDialogLine(TaskDialogLine* pLine);
	void			AnswerTaskDialog(int nIndex, char* pszEdit);
	void			CreateTaskDialog();

	void			ActiveNpc(OBJID idNpc);
	void			ActiveAction(OBJID idAction, char* pszStr=NULL);
public:
	void	SetLastDialogPara(OBJID idDialog){m_idDialogPara = idDialog;}
	OBJID	GetLastDialogPara(){return m_idDialogPara;}
public:
	virtual void	ReplaceStatus	(DWORD dwStatus);

private:
	CMyTimer		m_tmNameAlert;

private:
	int		m_nPkMode;

public:
	void	SetPkMode(int nPkMode, BOOL bSubmit = true);
	int	GetPkMode(){return m_nPkMode;}
private:
	BOOL	CanPk	(CPlayer* pTarget);

private:
	CProfessional		m_objProfessional;
	CTeam				m_objTeam;
	DWORD				m_dwLastQueryTeamLeader;
	DWORD				m_dwTeamData[3];
public:
	DWORD	GetTeamData0() { return m_dwTeamData[0]; }
	void	SetTeamData0(DWORD dw) { m_dwTeamData[0] = dw; }
	DWORD	GetTeamData1() { return m_dwTeamData[1]; }
	void	SetTeamData1(DWORD dw) { m_dwTeamData[1] = dw; }
	DWORD	GetTeamData2() { return m_dwTeamData[2]; }
	void	SetTeamData2(DWORD dw) { m_dwTeamData[2] = dw; }
private:

	CTrade				m_objTrade;
	CPackage			m_objPackage;
	CBank				m_objBank;
	CTaskDialog			m_objTaskDialog;
	DEQUE_WEAPONSKILL	m_setSingleHandWeaponSkill;
	DEQUE_WEAPONSKILL	m_setDoubleHandWeaponSkill;
	DEQUE_WEAPONSKILL	m_setOtherWeaponSkill;
	LevelExpInfo		m_setLevelExp[_LEVELEXP_AMOUNT];

	VECTOR_MAGIC		m_setMagic;

	DWORD				m_dwMagicAttackBegin;
	DWORD				m_dwMagicAttackBusy;
	BOOL				m_bMagicAttackBusy;
private:
	void			DestroyWeaponSkill();
	CWeaponSkill*	GetWeaponSkillByType(DEQUE_WEAPONSKILL& setSkill, int nType);
	CWeaponSkill*	GetWeaponSkill(DEQUE_WEAPONSKILL& setSkill, int nIndex);
	void			AddNewWeaponSkill(DEQUE_WEAPONSKILL& setSkill, int nSkillType, DWORD dwLevel, DWORD dwExp);

	void			CreateLevelExp();
public:
	CWeaponSkill*	GetWeaponSkill(int nWeaponSkillClass, int nIndex);
	CWeaponSkill*	GetWeaponSkill(int nWeaponSkillType);
	int				GetWeaponSkillAmount(int nWeaponSkillClass);
	void			AddNewWeaponSkill(int nSkillType, DWORD dwLevel, DWORD dwExp);
	void			DelWeaponSkill(int nSkillType);

public:
	void			TeamCreate();
	void			TeamApplyJion(OBJID idTarget);
	void			TeamLeave();
	void			TeamAcceptInvite();
	void			SetTeamLeaderPos(CMyPos pos)	{ m_objTeam.m_posLeader = pos, m_dwLastQueryTeamLeader = ::TimeGet();}
	DWORD			GetTeamLeaderFlashTime()		{ return m_dwLastQueryTeamLeader;}
	CMyPos			GetTeamLeaderPos(void)			{ return m_objTeam.m_posLeader; }
	void			SetTeamMoneyAccess(BOOL bAccess);
	void			SetTeamItemAccess(BOOL bAccess);
	BOOL			IsTeamClosed(){return m_objTeam.IsTeamClosed();}
	void			TeamInvite(OBJID idTarget);
	void			TeamKickOut(OBJID idTarget);
	void			TeamDismiss();
	void			TeamClose();
	void			TeamOpen();
	void			TeamAgreeApply();
	char*			GetTeamApplyerName();

	void			ClearTeamMember();
	void			SetTeamMemberHp(OBJID idMember, int nHp);
	void			SetTeamMemberMaxHp(OBJID idMember, int nMaxHp);
	int				GetTeamMemberAmount();
	TeamMemberInfo* GetTeamMemberInfo(int nIndex);
	TeamMemberInfo* GetTeamMemberInfoByID(OBJID idMember);
	BOOL			SetTeamLastApply(OBJID idPlayer);
	void			AddTeamMember(TeamMemberInfo* pInfo);
	void			DelTeamMember(OBJID id);
	void			CloseTeam();
	void			OpenTeam();
	BOOL			IsTeamMember(OBJID idPlayer);
	BOOL			IsTeamMember(char* pszPlayer);

	BOOL			IsTeamLeader();
	void			ShowTeam(CMyPos posShow);
	void			SetTeamMemberFace(OBJID idMember, int nFace);

public:
	char*			GetProfessionalName(DWORD dwIndex);

private:
	int			 m_nHeightCanJump;
	BOOL		 CanJump(CMyPos posTarGet);
	DWORD		 m_dwBeginUseTransformMagicTime;
	bool		 m_bMagicLock;

	DWORD		 m_dwFlyBegin;
	DWORD		 m_dwFlyDuration;
	void		 ShowFly();

	DWORD		 m_dwTransformBegin;
	DWORD		 m_dwTransformDuration;
	BOOL		 m_bShowAbortTransformDialog;
	void		 ShowTransform();

	DWORD		 m_dwMagicShieldBegin;
	DWORD		 m_dwMagicShieldDuration;
	void		 ShowMagicShield();

public:
	void		 BeginFly(DWORD dwMagicType, DWORD dwLevel);
	BOOL		 IsTransform();
	void		 BeginTransForm(DWORD dwMagicType, DWORD dwLevel);
	void		 BeginMagicShield(DWORD dwMagicType, DWORD dwLevel);
	void		 ProcessAbortTransformDialog();
	void		 AbortTransform();
public:
	void		DestroyMagicSet();
	void		AddMagic(OBJID idMagic, DWORD dwLevel, DWORD dwExp);
	void		DelMagic(OBJID idMagicType);
	void		StartMagicBusy(DWORD dwDuration);
	void		EndMagicBusy();
	void		ProcessMagicBusy();
	int			GetMagicAmount();
	CMagic*		GetMagicByIndex(int nIndex);
	CMagic*		GetMagic(OBJID idType);

	int			GetXpSkillAmount();
	CMagic*		GetXpSkillByIndex(int nIndex);

	int			GetKongfuAmount();
	CMagic*		GetKongfuByIndex(int nIndex);

	void		MagicAttack(OBJID idMagicType, OBJID idPlayer, BOOL bForce=false);
	void		MagicAttack(OBJID idMagicType, CMyPos posTarget);
	DWORD		GetMagicDelay(){return _MAGIC_DELAY - _MAGIC_DECDELAY_PER_LEVEL*this->GetProfessionLevel();}
	void		AbortIntone();
	void		ProcessMagic();
private:
	DWORD		m_dwLastBump;

private:
	OBJID		m_idLastSyndicateApply;
	OBJID		m_idLastSyndicate;
	char		m_szSyndicateAnnounce[_MAX_STRING];
	int			m_nSynListIndex;
	DEQUE_SYNMEMBER m_setSyndicateMember;
	SynMemberInfo m_objSynMemberInfo;

public:
	SynMemberInfo* GetReturnSynMemberInfo(){return &m_objSynMemberInfo;}

	void		QuerySynicateInfo();
	void		SetSynListIndex(int nIndex){m_nSynListIndex = nIndex;}
	void		SetSyndicateRank(DWORD dwRank);
	void		SetLastSyndicateApplyID(OBJID idApply);
	void		SetLastSyndicateID(OBJID idSynDicate);
	OBJID		GetLastSyndicateID();
	OBJID		GetLastSyndicateApplyID()	{ return m_idLastSyndicateApply; }
	void		SendSyndicateAnnounce(char* pszStr);
	void		SyndicateApplyJoin(OBJID idTarget);
	void		SynDicateAcceptJoin();
	void		SyndicateApplyInvite(OBJID idTarget);
	void		SyndicateAcceptInvite();
	void		LeaveSyndicate();
	void		SetSyndicateAnnounce(char* pszWord);
	char*		GetSyndicateAnnounce();

	void		QuerySynMemberList(int nIndex);
	int			GetSynMemberAmount();
	SynMemberInfo*	GetSynMemberByIndex(int nIndex);
	void		ClearSynMember();
	void		AddSynMember(char* pszMember);
	BOOL		IsAllEquipmentNonsuch();
	DWORD		GetBowShootRange();

public:
	BOOL ReBorn(DWORD dwData);
	BOOL IsRebornEnable()	{ return (this->IsGhost() && (::TimeGet() - m_timeGhost) > 20000); }

public:
	CDummy  m_objDummy;
	CDummy  m_objPlayerDummy;

public:
	void CreateStatuty();
public:
	CBooth m_objBooth;
	CBoothManager m_objBoothManager;

public:
	CBulletin m_objBulletin;
	void LockBooth(CPlayer* pTarget);
	BOOL CanBooth(CPlayer* pTarget);

public:
	void ChangeFace(int nFace);

private:
	BOOL m_bMousePlayerFixed;
	BOOL n_bMousePlayerTerrain;
public:
	void ProcessMousePlayer();
	void FixMousePlayer();
	void CreateMousePlayer(int nLookType, BOOL bTerrain = false);
	void SetMousePlayerDir(int nDir);
	void PutMousePlayer();
	void DelMousePlayer();
public:
	void DelNpc(OBJID idNpc);

public:
	char m_szWanted[_MAX_NAMESIZE];
	char m_szPoliceWanted[_MAX_NAMESIZE];
public:
	DiceManager m_objDiceManager;
public:
	void ApplyLanding();

private:
	BOOL	m_bChargeUp;
	DWORD	m_dwTimeChargeUp;
	DWORD	m_dwChargeUpTotalTime;

public:
	void BeginChangeUp(OBJID idMagicType, DWORD dwDelay);
	void StopChargeUp();
	void ShowChargeUp();
	void AbortChargeUp();

public:
	int		GetUserHelpInfoLineAmount();
	const char*	GetUserHelpInfoLineByIndex(int nIndex);

private:
	BOOL	m_bShowArrow;
public:
	CCheck  m_objCheck;
	CCheckKeys	m_objCheckKeys;
public:
	void	ShowArrow(int nOffset);

private:
	OBJID	m_idWhoAttackMe;
	OBJID	m_idMyTarget;
	OBJID	m_idSpecialTarget;
	DWORD	m_dwTimeSpecialTarget;
	DWORD   m_dwTimeWhoAttackMe;
	DWORD	m_dwTimeMyTarget;
	vector<OBJID>	m_setPetInfo;
public:
	void	SetIDWhoAttackMe(OBJID id){m_idWhoAttackMe = id;m_dwTimeWhoAttackMe = ::TimeGet();}
	CKillCounter*	GetKillCounter()	{return m_pKillCounter;}
	CKoCounter*		GetKoCounter()		{return m_pKoCounter;}
	void	SetIDMyTarget(OBJID id){m_idMyTarget = id;m_dwTimeMyTarget = ::TimeGet();}
	OBJID	GetIDWhoAttackMe(){return m_idWhoAttackMe;}
	OBJID	GetIDMyTarget(){return m_idMyTarget;}
	OBJID	GetSpecialTarget();
	void	SetSpecialTarget(OBJID idTarget);
	void	ProcessSpecialTarget();
	DWORD	GetTimeWhoAttackme(){return m_dwTimeWhoAttackMe;}
	void	AddPet(OBJID idPet);
	void	DelPet(OBJID idPet);
	int		GetCallPetAmount() {return m_setPetInfo.size();}
	void	ClearPet();
	BOOL	CheckPet(OBJID idPet);
public:
	BOOL	IsMagicAttackBusy(){return m_bMagicAttackBusy || this->IsMagicLock();}

	void	MagicLock(){m_bMagicLock = true;}
	void	UnMagicLock(){m_bMagicLock = false;}
	BOOL	IsMagicLock(){return m_bMagicLock;}

private:
	friend class CNetwork;
	friend class CMsgAction;
	friend class CMsgItem;
	friend class CGameMap;
	friend class CMsgTeamAward;
	friend class CMsgUserAttrib;
	CMyTimer	m_tmTeamMemberPos;
	DWORD		m_dwTeamMemberPosKey;
	CMyTimer	m_tmPing;
	DWORD		m_dwPing;
	OBJID		m_idDialogPara;
public:
	DWORD	GetPing(void)	{return m_dwPing;}
	void	SetTeamMemberPosTimer(DWORD dwInterval, DWORD dwKey);
	void	ProcessTeamMemberPos();
	void	ProcessPing();
	void	RequestAction(DWORD dwData);
	DWORD	m_dwLuckyTime;
	DWORD	m_dwLuckyTimeStamp;
	IGame3DEffect*	m_objLuckyEffect;
	DWORD			m_dwBlessBegin;
	DWORD			m_dwBlessSeconds;
	IGame3DEffect*	m_objBlessEffect;
	DWORD			m_dwCurseBegin;
	DWORD			m_dwCurseSeconds;
	IGame3DEffect*	m_objCurseEffect;
	DWORD			m_dwDblExpBegin;
	DWORD			m_dwDblExpSeconds;
	IGame3DEffect*	m_objDblExpEffect;
	DWORD			m_dwDayBonus;
	MAP_TABLE		m_mapTable;
	DWORD			m_dwTableParam[3];
	BOOL			m_bTableRequested;
	DWORD			m_idExercise;
public:
	DWORD			GetExerciseID() { return m_idExercise; }
	void			SetExerciseID(DWORD idExercise) { m_idExercise = idExercise; }
	TableRecord*	FindTableRecord(int nTable, int nIndex, BOOL bByIndex);
	void			AddTableRecord(int nTable, int nValue, int nData, int nIndex, BOOL bByIndex);
	void			ClearTableRecords();
	void			RequestTable(DWORD dwData);
	void			ActTable(DWORD dwAction, DWORD dwData);
	DWORD			GetTableParam(int nIndex) { return m_dwTableParam[nIndex]; }
	void			SetTableParam0(DWORD dwParam) { m_dwTableParam[0] = dwParam; }
private:
	void			ReportSuperState();
};

extern CHero	g_objHero;
const int Statuary1037[7][3] =
{
	{187, 191, 100},
	{184, 198, 100},
	{194, 188, 100},
	{185, 210, 100},
	{200, 210, 100},
	{206, 204, 100},
	{206, 189, 100}
};
const int Statuary1038[7][3] =
{
	{140, 134, 100},
	{130, 138, 100},
	{144, 124, 100},
	{153, 124, 100},
	{161, 124, 100},
	{130, 147, 100},
	{130, 155, 100}
};

#endif
