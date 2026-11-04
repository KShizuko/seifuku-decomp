#if !defined(AFX_MAGIC_H__B2689767_419D_487F_B0F9_FBB04C4619AA__INCLUDED_)
#define AFX_MAGIC_H__B2689767_419D_487F_B0F9_FBB04C4619AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "define.h"
#include "GameObj.h"
#include "GameData.h"
#include "MagicType.h"
#include "T_SingleObjSet2.h"
#include "Myheap.h"
#include "MagicData.h"
#include "TimeOut.h"

#pragma warning(disable:4786)
#include <vector>

class	CUser;
class	CItem;
class	IRole;

extern char	szMagicTable[];

enum {
		MAGICSORT_ATTACK			= 1,
		MAGICSORT_RECRUIT			= 2,
		MAGICSORT_CROSS				= 3,
		MAGICSORT_FAN				= 4,
		MAGICSORT_BOMB				= 5,
		MAGICSORT_ATTACHSTATUS		= 6,
		MAGICSORT_DETACHSTATUS		= 7,
		MAGICSORT_SQUARE			= 8,
		MAGICSORT_JUMPATTACK		= 9,
		MAGICSORT_RANDOMTRANS		= 10,
		MAGICSORT_DISPATCHXP		= 11,
		MAGICSORT_COLLIDE			= 12,
		MAGICSORT_SERIALCUT			= 13,
		MAGICSORT_LINE				= 14,
		MAGICSORT_ATKRANGE			= 15,
		MAGICSORT_ATKSTATUS			= 16,
		MAGICSORT_CALLTEAMMEMBER	= 17,
		MAGICSORT_RECORDTRANSSPELL	= 18,
		MAGICSORT_TRANSFORM			= 19,
		MAGICSORT_ADDMANA			= 20,
		MAGICSORT_LAYTRAP			= 21,
		MAGICSORT_DANCE				= 22,
		MAGICSORT_CALLPET			= 23,
		MAGICSORT_VAMPIRE			= 24,
		MAGICSORT_INSTEAD			= 25,
		MAGICSORT_DECLIFE			= 26,
		MAGICSORT_GROUNDSTING		= 27,
		MAGICSORT_REBORN			= 28,
};

const int	WEAPONMAGIC_OFFSET				= 10000;
const int	WEAPONMAGIC_MIN					= WEAPONMAGIC_OFFSET;
const int	WEAPONMAGIC_MAX					= WEAPONMAGIC_OFFSET+256;
const int	WEAPONMAGIC_LEVEL	= 0;
const OBJID	WEAPONR_MAGICID		= 4200000001;
const OBJID	WEAPONL_MAGICID		= 4200000002;

const int	MAGICDAMAGE_ALT					= 26;
const int	AUTOLEVELUP_EXP					= -1;
const int	DISABLELEVELUP_EXP				= 0;
const int	AUTOMAGICLEVEL_PER_USERLEVEL	= 10;
const int	USERLEVELS_PER_MAGICLEVEL		= 10;

const int	KILLBONUS_PERCENT				= 5;
const int	HAVETUTORBONUS_PERCENT			= 10;
const int	WITHTUTORBONUS_PERCENT			= 20;

const int	MAGIC_DELAY						= 1000;
const int	MAGIC_DECDELAY_PER_LEVEL		= 100;
const int	RANDOMTRANS_TRY_TIMES			= 10;
const int	DISPATCHXP_NUMBER				= 20;
const int	COLLIDE_POWER_PERCENT			= 80;
const int	COLLIDE_SHIELD_DURABILITY		= 2;
const int	LINE_WEAPON_DURABILITY			= 2;
const int	MAX_SERIALCUTSIZE				= 10;
const int	AWARDEXP_BY_TIMES				= 1;
const int	AUTO_MAGIC_DELAY_PERCENT		= 150;
const int	BOW_SUBTYPE						= 500;
const int	POISON_MAIGC_TYPE				= 10010;
const int	DEFAULT_MAGIC_FAN				= 120;

typedef	CGameData<MAGICDATA, szMagicTable, szID>	CMagicDataBase;
class	CMagicData : public CMagicDataBase
{
protected:
	CMagicData()			{ m_pType = NULL; }
public:
	static CMagicData* CreateNew()		{ return new CMagicData; }
	bool	Create(IRecordset* pRecordset);
	bool	Create(IRecordset* pDefaultRecord, OBJID idNewKey);
	int		GetInt(MAGICDATA nDataName);
	void	SetInt(MAGICDATA idx, int nData);
	bool	FindType();

protected:
	CMagicTypeData*	m_pType;

protected:
	MYHEAP_DECLARATION(s_heap)
};
typedef	CMagicData		IMagicData;

typedef	ISingleObjSet2<CMagicData>		IMagicSet;
typedef	CSingleObjSet2<CMagicData>		CMagicSet;

struct MagicInfoStruct
{
	OBJID	id;
	OBJID	idOwner;
	int		nType;
	int		nLevel;
	DWORD	dwExp;
};

enum
{
	TYPE_MAGIC		= 0,
	TYPE_XPSKILL	= 1,
	TYPE_KONGFU		= 2,
};

enum
{
	MAGICSTATE_NONE		= 0,
	MAGICSTATE_INTONE	= 1,
	MAGICSTATE_LAUNCH	= 2,
	MAGICSTATE_DELAY	= 3,
};

enum AUTO_ACTIVE{
	AUTOACTIVE_KILL			= 0x01,
	AUTOACTIVE_FOREVER		= 0x02,
	AUTOACTIVE_RANDOM		= 0x04,
	AUTOACTIVE_HIDDEN		= 0x08,
	AUTOACTIVE_EXPDISABLE	= 0x10,
};

enum THROW_WEAPON{
	THROW_WERPONR_TARGET			= 1,
};

inline bool		IsWeaponMagic(int nType)			{ return nType >= WEAPONMAGIC_MIN && nType < WEAPONMAGIC_MAX; }
inline int		MagicType(int idx)					{ int nType = idx + WEAPONMAGIC_OFFSET; CHECKF(IsWeaponMagic(nType)); return nType; }
inline OBJID	POSITION2MAGIC_ID(int nPosition)	{ if(nPosition == ITEMPOSITION_WEAPONR) return WEAPONR_MAGICID; else return WEAPONL_MAGICID; }
void	DDALine(int x0, int y0, int x1, int y1, int nRange, vector<POINT>& vctPoint);

class CMagic
{
protected:
	CMagic(PROCESS_ID idProcess, CUser* pUser);
	virtual ~CMagic();

private:
	CMagic() { m_bAutoAttack = false;}

public:
	static CMagic* CreateNew(PROCESS_ID idProcess, CUser* pUser);
	ULONG	ReleaseByOwner()	{ delete this; return 0; }
	void	OnTimer(DWORD tCurr);

public:
	bool	CreateAll();
	bool	SendInfoToClient();

public:
	bool	SaveInfo();
	bool	GetInfoByIndex(MagicInfoStruct* pInfo, int idx);
	bool	AppendMagic(const MagicInfoStruct* pInfo);

public:
	IMagicSet*	QuerySet()		{ CHECKF(m_setData); return m_setData; }
	bool		MagicAttack(int nType, OBJID idTarget, int x, int y);
	bool		AbortMagic(bool bSynchro=false);
	void		OwnerUpLevel(int nOwnerLev);

	void		UserWeaponSkillUpLevel(int nWeaponSkillType, int nWeaponSkillLevel);
	bool		AutoLearnWeaponSkill(int nWeaponSkillType, int nWeaponSkillLevel, int nWeaponSkillExp=0);
	bool		UserKillTarget(IRole* pTarget);
	bool		AutoAttackTarget(IRole* pTarget);
	IMagicData*	FindMagic(int nType);
	int			GetExtendAtkRange	(CItem* pWeaponR, CItem* pWeaponL);
	bool		AwardExp(int nExp, int nBattleExp);
	void		BreakAutoAttack();	void DebugReloadAll() { for(int i = 0; i < m_setData->GetAmount(); i++) m_setData->GetObjByIndex(i)->FindType(); }
	bool		WeaponMagicAttack(int nType, OBJID idTarget, int x, int y);

	bool		IsActive()		{ return m_pMagic != NULL; }

public:
	static CMagicTypeData*	FindMagicType(int nType, int nLevel);
	static int 	CalcMagicPower(IRole* pAtker, IRole* pTarget, int nAtk);

protected:
	bool		AwardExpOfLife(IRole* pRole, int nLoseLife, BOOL bMagicRecruit=false);
	void		ShowMiss();
	int			Attack(int nType, IRole* pRole);
	int			Attack(int nType, int x, int y);
	int			Launch();			int	LaunchSort();
	bool		UpLevel(bool bSynchro = true);
	bool		CheckCondition(IMagicData* pData, OBJID idTarget, int x, int y);
	bool		CheckCrime(const vector<IRole*>* pSet);
	bool		CheckCrime(IRole* pRole);

protected:
	bool		CheckAwardExpEnable(DWORD dwProf);
	void		OtherMemberAwardExp(IRole* pTarget, int nRawExp);

public:
	enum { HIT_BY_MAGIC, HIT_BY_WEAPON, HIT_BY_ITEM };
	bool		IsImmunity(IRole* pRole, int nSort);
	bool		HitByMagic();
	bool		HitByWeapon();
	int			GetDieMode();
	bool		IsKeepBow()						{ return m_pMagic && m_pMagic->GetInt(MAGICDATA_SORT) == MAGICSORT_ATTACHSTATUS && m_pMagic->GetInt(MAGICDATA_STATUS) == STATUS_KEEPBOW; }
	bool		IsCrimeMagic()					{ return m_pMagic && m_pMagic->GetInt(MAGICDATA_CRIME); }

public:
	bool		CheckType(int nType);
	bool		CheckLevel(int nType, int nLevel);
	bool		UpLevelByTask(int nType, bool bSynchro);
	bool		AddMagicExp(int nType, int nExp);

public:
	void		ResetDelay();
	OBJID		LearnMagic(int nType, int nLevel=0);
	bool		UnlearnMagic(int nType, int nMode);
	bool		EquipWeaponMagic(OBJID idMagic, int nType, int nLevel);
	bool		UnequipWeaponMagic(OBJID idMagic);

protected:
	bool		ProcessAttack();
	bool		ProcessAtkStatus();
	bool		ProcessRecruit();
	bool		ProcessCross();
	bool		ProcessLine();
	bool		ProcessFan();
	bool		ProcessBomb();
	bool		ProcessSquare();
	bool		ProcessAttach();
	bool		ProcessDetach();
	bool		ProcessJumpAttackFaild(int x, int y);
	bool		ProcessJumpAtk();
	bool		ProcessRandomTrans();
	bool		ProcessDispatchXP();
	bool		ProcessCollideFail(int x, int y, int nDir);
	bool		ProcessCollide();
	bool		ProcessSerialCut();
	bool		ProcessCallTeamMember();
	bool		ProcessRecordTransSpell();
	bool		ProcessTransform();
	bool		ProcessAddMana();
	bool		ProcessLayTrap();
	bool		ProcessCallPet();
	bool		ProcessDecLife();

	typedef std::vector<IRole*>	ROLE_SET;

protected:
	static OBJID	SpawnMagicID()						{ return InterlockedIncrement(&m_lockNextID); }

protected:
	IMagicSet*		m_setData;

protected:
	IMagicData*		m_pMagic;
	OBJID			m_idTarget;
	POINT			m_pos;
	int				m_nData;
	CTimeOutMS		m_tIntone;
	CTimeOutMS		m_tDelay;
	bool			m_bAutoAttack;
	int				m_nRawDelay;
	int				m_nDelay;
	DWORD			m_dwUnk38;
	DWORD			m_dwUnk3C;

protected:
	PROCESS_ID		m_idProcess;
	CUser*			m_pUser;
	struct { int data; }	m_bInMagicLoop;
	static long		m_lockNextID;
	MYHEAP_DECLARATION(s_heap)
};

enum { UNLEARN_NORMAL = 0, UNLEARN_RESET = 1, UNLEARN_DROP = 2 };

#endif
