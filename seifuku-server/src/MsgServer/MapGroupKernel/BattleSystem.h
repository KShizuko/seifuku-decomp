#if !defined(AFX_BATTLESYSTEM_H__291473E1_45B6_4A0A_8567_548852B93C33__INCLUDED_)
#define AFX_BATTLESYSTEM_H__291473E1_45B6_4A0A_8567_548852B93C33__INCLUDED_

#include "T_SingleObjSet2.h"

enum LEVUPEXPDATA{
		LEVUPEXPDATA_LEVEL		=0,
		LEVUPEXPDATA_EXP,
		LEVUPEXPDATA_UP_LEV_TIME,
};
extern char	szZcxKey[];
extern char	szLevupexpTable[];
typedef	CGameData<LEVUPEXPDATA, szLevupexpTable, szZcxKey>	CLevupexpData;
typedef	ISingleObjSet2<CLevupexpData>	ILevupexpSet;
typedef	CSingleObjSet2<CLevupexpData>	CLevupexpSet;

enum {
	MAGICTYPE_NONE		= 0,
	MAGICTYPE_NORMAL	= 1,
	MAGICTYPE_XPSKILL	= 2,
};

class IRole;

#if _MSC_VER > 1000
#pragma once
#endif

class CBattleSystem
{
public:
	CBattleSystem(PROCESS_ID idProcess, IRole* pOwner);
	virtual ~CBattleSystem();

public:
	void		ResetBattle				();
	BOOL		CreateBattle			(OBJID idTarget);
	IRole*		GetOwner				()					{return m_pOwner;}

	BOOL		ProcAttack_Hand2Hand	();
	BOOL		IsBattleMaintain		();
	OBJID		GetTargetID				()					{return m_idTarget;}
	BOOL		IsActived				()					{return m_idTarget!=ID_NONE;}

public:
	static int	CalcAttackPower(IRole* pAtker, IRole* pTarget, bool bAdjustDamage, int nAdjustAtk, int nAdjustDef, bool bGemAtk);

protected:
	static int	CalcHandPower	(IRole* pAtker, IRole* pTarget, bool bAdjustDamage, int nAdjustAtk, int nAdjustDef);
	static int	CalcBowPower	(IRole* pAtker, IRole* pTarget, bool bAdjustDamage, int nAdjustAtk, int nAdjustDef);

public:
	BOOL		IsTargetDodged			(IRole* pAtker, IRole* pTarget);

private:
	OBJID			m_idTarget;
	IRole*			m_pOwner;

	PROCESS_ID		m_idProcess;

public:
	MYHEAP_DECLARATION(s_heap)
};

#endif
