#if !defined(AFX_MAGICDATA_H__B1F17B6C_5DC6_4DC8_ACBC_598255551440__INCLUDED_)
#define AFX_MAGICDATA_H__B1F17B6C_5DC6_4DC8_ACBC_598255551440__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h" 
#include <stdio.h>

enum
{
	MAGIC_SORT_ATTACK_SINGLE_HP			= 1,
	MAGIC_SORT_RECOVER_SINGLE_HP		= 2,
	MAGIC_SORT_ATTACK_CROSS_HP			= 3,
	MAGIC_SORT_ATTACK_SECTOR_HP			= 4,
	MAGIC_SORT_ATTACK_ROUND_HP			= 5,
	MAGIC_SORT_ATTACK_SINGLE_STATUS		= 6,
	MAGIC_SORT_RECOVER_SINGLE_STATUS	= 7,
	MAGICSORT_SQUARE					= 8,
	MAGICSORT_JUMPATTACK				= 9,
	MAGICSORT_RANDOMTRANS				= 10,
	MAGICSORT_DISPATCHXP				= 11,
	MAGICSORT_COLLIDE					= 12,
	MAGICSORT_SERIALCUT					= 13,
	MAGICSORT_LINE						= 14,
	MAGICSORT_ATKRANGE					= 15,
	MAGICSORT_ATKSTATUS					= 16,
	MAGICSORT_CALL_TEAMMEMBER			= 17,
	MAGICSORT_RECORDTRANSSPELL			= 18,
	MAGICSORT_TRANSFORM					= 19,
	MAGICSORT_ADDMANA					= 20,
	MAGICSORT_LAYTRAP					= 21,
	MAGICSORT_DANCE						= 22,
	MAGICSORT_CALLPET			= 23,
	MAGICSORT_VAMPIRE			= 24,
	MAGICSORT_INSTEAD			= 25,
	MAGICSORT_DECLIFE			= 26,
	MAGICSORT_GROUNDSTING		= 27,
	MAGICSORT_REBORN			= 28,
	MAGICSORT_TEAM_MAGIC		= 29,
	MAGICSORT_BOMB_LOCKALL		= 30,
	MAGICSORT_SORB_SOUL			= 31,
	MAGICSORT_STEAL				= 32,
	MAGICSORT_LINE_PENETRABLE	= 33,
	
	MAGICSORT_BLAST_THUNDER			= 34,
	MAGICSORT_MULTI_ATTACHSTATUS	= 35,
	MAGICSORT_MULTI_DETACHSTATUS	= 36,
	MAGICSORT_MULTI_CURE			= 37,
	MAGICSORT_STEAL_MONEY			= 38,
	MAGICSORT_KO					= 39,
	MAGICSORT_ESCAPE				= 40,
	MAGICSORT_ATTRACK_MONSTER		= 42,
};

enum
{
	MAGIC_SHAKENORMAL		  = 0x00000001,
	MAGIC_COLORNORMAL		  = 0x00000002,
	MAGIC_SCALENORMAL		  = 0x00000004,
};

enum 
{
	MAGIC_TARGET_SELF		  = 0x00000001,
	MAGIC_TARGET_NONE		  = 0x00000002,
	MAGIC_TARGET_TERRAIN      = 0x00000004,
	MAGIC_PASSIVE			  = 0x00000008,
	MAGIC_TARGET_BODY		  = 0x00000010,
};

enum
{
	TYPE_MAGIC		= 0,
	TYPE_XPSKILL	= 1,
	TYPE_KONGFU		= 2,
};

#include <map>
using namespace std;

typedef struct
{
	OBJID	idMagicType;
	DWORD	dwActionSort;
	char	szName[_MAX_NAMESIZE];
	DWORD	dwCrime;
	DWORD	dwGround;
	DWORD	dwMulti;
	DWORD	dwTarget;
	DWORD	dwLevel;
	DWORD	dwMpCost;
	DWORD	dwPower;
	DWORD	dwIntoneDuration;
	DWORD	dwHitPoint;
	DWORD	dwDuration;
	DWORD	dwRange;
	DWORD	dwDistance;
	DWORD	dwStatus;
	DWORD	dwProfessionalRequired;
	DWORD	dwExpRequired;
	DWORD	dwMonsterLevelRequired;
	
	DWORD	dwXp;
	DWORD	dwWeaponSubType;
	
	DWORD	dwActiveTime;
	DWORD	dwAutoActive;
	DWORD	dwFloorAttribute;
	DWORD	dwAutoLearn;
	DWORD	dwLearnLevel;
	DWORD	dwDropWeapon;
	DWORD	dwUsePP;
	DWORD	dwWeaponHit;
	OBJID	idUseItem;
	OBJID	idNextMagic;
	DWORD	dwMagicBreak;

	DWORD	dwClientRepresent;
	DWORD	dwSenderAction;
	char	szDisc[320];
	char	szIntoneEffect[64];
	char	szIntoneSound[_MAX_PATH];
	char	szSenderEffect[64];
	char	szSenderSound[_MAX_PATH];
	DWORD	dwTargetDelay;
	char	szTargetEffect[64];
	char	szTargetSound[_MAX_PATH];
	char	szGroundEffect[64];
	char	szTraceEffect[64];
	DWORD	dwScreenRepresent;
	DWORD	dwCanBeusedInMarket;
	DWORD	dwTargetWoundDelay;
}MagicTypeInfo;

typedef map<DWORD, MagicTypeInfo> MAP_MAGICTYPE;

class CMagicData  
{
public:
	CMagicData();
	virtual ~CMagicData();
private:
	DWORD*	m_pIDIndex;
	FILE*	m_fp;
	DWORD	m_dwIDAmount;
	MAP_MAGICTYPE m_setMagicType;
private:
	void Create();
	void Destroy();
public:
	BOOL GetMagicTypeInfo(OBJID dwIDType, DWORD dwLevel, MagicTypeInfo& infoMagic);
};

#endif
