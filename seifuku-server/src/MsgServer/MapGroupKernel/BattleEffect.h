#if !defined(SFK_BATTLEEFFECT_H__INCLUDED_)
#define SFK_BATTLEEFFECT_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "DisdainDataSet.h"

class IRole;
class IDatabase;
class CBattleEffect
{
public:
	static bool		Init				(IDatabase* pDb);
	static void		Destroy				();

public:
	static int		AdjustAttack		(IRole* pAtker, IRole* pTarget, int nAtk);
	static int		AdjustDamage		(IRole* pAtker, IRole* pTarget, int nDamage);
	static int		GetBattleExpRate	(IRole* pAtker, IRole* pTarget);
	static int		AdjustExp			(IRole* pAtker, IRole* pTarget, int nExp);

protected:
	static CDisdainData*	QueryDisdain	(IRole* pAtker, IRole* pTarget);
	static int		GetProfessionFactor	(IRole* pRole);
	static int		AdjustUserDamage	(IRole* pAtker, IRole* pTarget, int nDamage);
	static int		AdjustMonsterDamage	(IRole* pAtker, IRole* pTarget, int nDamage);
	static int		AdjustUserToMonster	(IRole* pAtker, IRole* pTarget, int nDamage);

protected:
	static CDisdainDataSet*	s_pDisdainSet;
};

#endif
