#include "BattleEffect.h"
#include "MapGroup.h"
#include "User.h"
#include "Npc.h"
#include "AINpc.h"

CDisdainDataSet*	CBattleEffect::s_pDisdainSet	= NULL;

const int	PROFESSION_FACTOR[5]		= { 10, 6, 6, 6, 6 };
const int	PROFESSION_FACTOR_REBORN[5]	= { 18, 18, 14, 27, 18 };

bool CBattleEffect::Init(IDatabase* pDb)
{
	s_pDisdainSet = CDisdainDataSet::CreateNew();
	if(s_pDisdainSet && s_pDisdainSet->Init(pDb))
		return true;
	return false;
}

void CBattleEffect::Destroy()
{
	if(s_pDisdainSet)
		s_pDisdainSet->Release();
}

CDisdainData* CBattleEffect::QueryDisdain(IRole* pAtker, IRole* pTarget)
{
	if(!pAtker || !pTarget || !s_pDisdainSet)
		return NULL;

	int nType = 0;
	CMonster* pMonster = NULL;
	if(pAtker->IsMonster())
	{
		if(pAtker->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
			nType = pMonster->GetType();
	}
	else if(pTarget->IsMonster())
	{
		if(pTarget->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
			nType = pMonster->GetType();
	}

	CUser* pAtkUser		= pAtker->QueryOwnerUser();
	CUser* pTargetUser	= pTarget->QueryOwnerUser();
	int nDelta = 0;
	if(pAtkUser && pTargetUser)
		nDelta	= pAtkUser->GetBattlePower() - pTargetUser->GetBattlePower();
	else if(pAtkUser)
		nDelta	= pAtkUser->GetLev() - pTarget->GetLev();
	else if(pTargetUser)
		nDelta	= pAtker->GetLev() - pTargetUser->GetLev();
	else
		nDelta	= pAtker->GetLev() - pTarget->GetLev();

	CDisdainData* pData = s_pDisdainSet->Query(nType, nDelta);
	if(pData)
		return pData;
	return s_pDisdainSet->Query(0, nDelta);
}

int CBattleEffect::AdjustAttack(IRole* pAtker, IRole* pTarget, int nAtk)
{
	if(!pAtker || !pTarget)
		return 0;

	CDisdainData* pData = QueryDisdain(pAtker, pTarget);
	if(!pData)
		return nAtk;

	int nAdjust = 0;
	if(pAtker->IsMonster())
		nAdjust	= pData->GetInt(DISDAINDATA_MST_ATK);
	else if(pTarget->IsMonster())
		nAdjust	= pData->GetInt(DISDAINDATA_USR_ATK_MST);

	int nPercent = 100 + nAdjust;
	if(nPercent <= 0)
		return 0;
	return nPercent * nAtk / 100;
}

int CBattleEffect::AdjustDamage(IRole* pAtker, IRole* pTarget, int nDamage)
{
	if(!pAtker || !pTarget)
		return 0;

	int nResult = nDamage;
	CUser* pAtkUser		= pAtker->QueryOwnerUser();
	CUser* pTargetUser	= pTarget->QueryOwnerUser();
	if(pAtkUser && pTargetUser)
		nResult	= AdjustUserDamage(pAtker, pTarget, nDamage);

	CDisdainData* pData = QueryDisdain(pAtker, pTarget);
	if(pData && pTarget->IsMonster() && pAtkUser)
	{
		int nMaxAtk = 0;
		if(pAtkUser->QueryStatus(STATUS_SUPERMAN) || pAtkUser->QueryStatus(STATUS_TORNADO))
			nMaxAtk	= pData->GetInt(DISDAINDATA_MAX_XP_ATK);
		else
			nMaxAtk	= pData->GetInt(DISDAINDATA_MAX_ATK);
		int nMaxDamage = pTarget->GetMaxLife() * nMaxAtk / 100;
		if(nMaxDamage < nResult)
			nResult	= nMaxDamage;
	}

	CMonster* pMonster = NULL;
	if(pAtkUser)
	{
		if(!pTargetUser && pTarget->QueryObj(OBJ_MONSTER, IPP_OF(pMonster))
				&& pMonster->QueryType()->GetInt(NPCTYPEDATA_EXTRA_BATTLELEV) > 0)
			nResult	= AdjustUserToMonster(pAtker, pTarget, nDamage);
	}
	else
	{
		if(pAtker->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)) && pTargetUser
				&& pMonster->QueryType()->GetInt(NPCTYPEDATA_EXTRA_DAMAGE) > 0)
			nResult	= AdjustMonsterDamage(pAtker, pTarget, nDamage);
	}
	return nResult;
}

int CBattleEffect::GetProfessionFactor(IRole* pRole)
{
	CUser* pUser = pRole->QueryOwnerUser();
	if(!pUser)
		return 1;

	int nIndex = 0;
	int nProfession = pUser->GetProfession();
	if(nProfession >= 10 && nProfession <= 15)
		nIndex	= 0;
	else if(nProfession >= 20 && nProfession <= 25)
		nIndex	= 1;
	else if(nProfession >= 40 && nProfession <= 45)
		nIndex	= 2;
	else if((nProfession >= 100 && nProfession <= 101) || (nProfession >= 142 && nProfession <= 145))
		nIndex	= 3;
	else if(nProfession >= 132 && nProfession <= 135)
		nIndex	= 4;
	else
		return 1;

	if(pUser->GetMetempsychosis() > 0)
		return PROFESSION_FACTOR_REBORN[nIndex];
	return PROFESSION_FACTOR[nIndex];
}

int CBattleEffect::AdjustUserDamage(IRole* pAtker, IRole* pTarget, int nDamage)
{
	CDisdainData* pData = QueryDisdain(pAtker, pTarget);
	if(!pData)
		return nDamage;

	CUser* pAtkUser		= pAtker->QueryOwnerUser();
	CUser* pTargetUser	= pTarget->QueryOwnerUser();
	if(!pAtkUser || !pTargetUser)
		return nDamage;

	int nMin = 0, nMax = 0, nOverAdjust = 0;
	if(pAtkUser->GetLev() < 110)
	{
		if(pTargetUser->GetLev() < 110)
		{
			nMin		= pData->GetInt(DISDAINDATA_USR_ATK_USR_MIN);
			nMax		= pData->GetInt(DISDAINDATA_USR_ATK_USR_MAX);
			nOverAdjust	= pData->GetInt(DISDAINDATA_USR_ATK_USR_OVERADJ);
		}
		else
		{
			nMin		= pData->GetInt(DISDAINDATA_USR_ATK_USRX_MIN);
			nMax		= pData->GetInt(DISDAINDATA_USR_ATK_USRX_MAX);
			nOverAdjust	= pData->GetInt(DISDAINDATA_USR_ATK_USRX_OVERADJ);
		}
	}
	else
	{
		if(pTargetUser->GetLev() < 110)
		{
			nMin		= pData->GetInt(DISDAINDATA_USRX_ATK_USR_MIN);
			nMax		= pData->GetInt(DISDAINDATA_USRX_ATK_USR_MAX);
			nOverAdjust	= pData->GetInt(DISDAINDATA_USRX_ATK_USR_OVERADJ);
		}
		else
		{
			nMin		= pData->GetInt(DISDAINDATA_USRX_ATK_USRX_MIN);
			nMax		= pData->GetInt(DISDAINDATA_USRX_ATK_USRX_MAX);
			nOverAdjust	= pData->GetInt(DISDAINDATA_USRX_ATK_USRX_OVERADJ);
		}
	}

	int nMinDamage = pTargetUser->GetLev() * GetProfessionFactor(pTarget) * nMin / 100;
	if(nDamage < nMinDamage)
		return nMinDamage;

	int nMaxDamage = pTargetUser->GetLev() * GetProfessionFactor(pTarget) * nMax / 100;
	if(nDamage > nMaxDamage)
		return nMaxDamage + ::MulDiv(nDamage - nMaxDamage, nOverAdjust, 100);
	return nDamage;
}

int CBattleEffect::AdjustMonsterDamage(IRole* pAtker, IRole* pTarget, int nDamage)
{
	CMonster* pMonster = NULL;
	CUser* pTargetUser = pTarget->QueryOwnerUser();
	if(!pAtker->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)) || !pTargetUser)
		return nDamage;

	int nDelta = pTargetUser->GetBattlePower() - pMonster->QueryType()->GetInt(NPCTYPEDATA_EXTRA_BATTLELEV);
	int nPercent = 30;
	if(nDelta < 5)
		nPercent	= 100;
	else if(nDelta < 10)
		nPercent	= 80;
	else if(nDelta < 15)
		nPercent	= 60;
	else if(nDelta < 20)
		nPercent	= 40;

	int nMinDamage = ::MulDiv(pTarget->GetMaxLife(), pMonster->QueryType()->GetInt(NPCTYPEDATA_EXTRA_DAMAGE)*nPercent, 1000000);
	if(nDelta < 0)
		nMinDamage	= pAtker->GetAttack();
	return __max(nMinDamage, nDamage);
}

int CBattleEffect::AdjustUserToMonster(IRole* pAtker, IRole* pTarget, int nDamage)
{
	CUser* pUser = pAtker->QueryOwnerUser();
	CMonster* pMonster = NULL;
	if(!pUser || !pTarget->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
		return nDamage;

	int nDelta = pMonster->QueryType()->GetInt(NPCTYPEDATA_EXTRA_BATTLELEV) - pUser->GetBattlePower();
	if(nDelta <= 0)
		return nDamage;

	int nPercent = 100;
	if(nDelta >= 10)
		nPercent	= 1;
	else if(nDelta >= 5)
		nPercent	= 5;
	else if(nDelta >= 1)
		nPercent	= 10;
	return ::MulDiv(nDamage, nPercent, 100);
}

int CBattleEffect::GetBattleExpRate(IRole* pAtker, IRole* pTarget)
{
	if(!pAtker || !pTarget)
		return 0;

	CMonster* pMonster = NULL;
	if(!pTarget->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
		return 0;
	CUser* pUser = pAtker->QueryOwnerUser();
	if(!pUser)
		return 0;

	int nDelta = pUser->GetBattlePower() - pMonster->QueryType()->GetInt(NPCTYPEDATA_EXTRA_BATTLELEV);
	if(nDelta < 0)
		return 0;
	return ::MulDiv(nDelta, pMonster->QueryType()->GetInt(NPCTYPEDATA_EXTRA_EXP), 100);
}

int CBattleEffect::AdjustExp(IRole* pAtker, IRole* pTarget, int nExp)
{
	if(!pAtker || !pTarget || !s_pDisdainSet)
		return 0;

	CMonster*	pMonster	= NULL;
	CNpc*		pNpc		= NULL;
	int nLev = 0, nType = 0;
	pTarget->QueryObj(OBJ_MONSTER, IPP_OF(pMonster));
	if(pMonster)
	{
		if(pMonster->QueryOwnerUser())
			return nExp;
		nLev	= pMonster->GetLev();
		nType	= pMonster->GetType();
	}
	else if(pTarget->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsGoal())
	{
		nLev	= pNpc->GetLev();
		nType	= pNpc->GetType();
	}
	else
		return nExp;

	CUser* pUser = pAtker->QueryOwnerUser();
	if(!pUser)
		return nExp;

	CDisdainData* pData = s_pDisdainSet->Query(nType, pUser->GetLev() - nLev);
	if(!pData)
	{
		pData = s_pDisdainSet->Query(0, pUser->GetLev() - nLev);
		if(!pData)
			return nExp;
	}

	int nFactor = 0;
	if(pUser->QueryStatus(STATUS_SUPERMAN) || pUser->QueryStatus(STATUS_TORNADO))
		nFactor	= pData->GetInt(DISDAINDATA_XP_EXP_FACTOR);
	else
		nFactor	= pData->GetInt(DISDAINDATA_EXP_FACTOR);
	return (nFactor + 100) * nExp / 100;
}
