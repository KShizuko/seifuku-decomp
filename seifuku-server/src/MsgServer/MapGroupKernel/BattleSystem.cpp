#include "Allmsg.h"
#include "I_Role.h"
#include "BattleSystem.h"
#include "mapgroup.h"
#include "BattleEffect.h"

const int	BOW_SHIELD_HITRATE_PERCENT			= 12;
MYHEAP_IMPLEMENTATION(CBattleSystem,s_heap)

char	szZcxKey[]			= "level";
char	szLevupexpTable[]	= _TBL_LEVEXP;

CBattleSystem::CBattleSystem(PROCESS_ID idProcess, IRole* pOwner)
{
	m_idProcess	= idProcess;
	m_pOwner	= pOwner;
	m_idTarget	= ID_NONE;
}

CBattleSystem::~CBattleSystem()
{

}

BOOL CBattleSystem::ProcAttack_Hand2Hand()
{
	DEBUG_TRY
	if (!m_pOwner || m_idTarget == ID_NONE)
		return false;

	if (!IsBattleMaintain())
		return false;

	DEBUG_CATCH("[early debug]CBattleSystem::ProcAttack_Hand2Hand IsBattleMain()");

	IRole* pTarget	= m_pOwner->GetMap()->QueryRole(m_pOwner->GetPosX(), m_pOwner->GetPosY(), m_idTarget);
	if (!pTarget)
	{
		this->ResetBattle();
		return true;
	}

	if(m_pOwner->IsBowman() && !m_pOwner->GetMap()->IsTrainMap())
	{
		if(!m_pOwner->SpendArrow())
		{
			this->ResetBattle();
			return true;
		}
	}

	if(m_pOwner->AutoSkillAttack(pTarget))
		return true;

	DEBUG_TRY
	m_pOwner->GetPosX();	m_pOwner->GetPosY();
	if (this->IsTargetDodged(m_pOwner, pTarget))
	{
		m_pOwner->SendDamageMsg(pTarget->GetID(), 0);
		m_pOwner->QueryStatusSet()->DelObj(STATUS_KEEPBOW);
		return true;
	}
	DEBUG_CATCH("[early debug]CBattleSystem::ProcAttack_Hand2Hand TryDodge!");

	DEBUG_TRY
	int nTargetOrgLife	= pTarget->GetLife();
	int nDamage			= m_pOwner->Attack(pTarget);
	pTarget->GetLife();
	int nTargetLifeLost = nTargetOrgLife-pTarget->GetLife();
	int nExp = nTargetLifeLost;

	m_pOwner->SendDamageMsg(pTarget->GetID(), nDamage);
	m_pOwner->QueryStatusSet()->DelObj(STATUS_KEEPBOW);

	CUser* pUser = m_pOwner->QueryOwnerUser();
	CNpc* pNpc;
	pTarget->QueryObj(OBJ_NPC, IPP_OF(pNpc));
	DEBUG_TRY
	if(pUser && pNpc && pNpc->IsSynFlag())
	{
		pUser->AwardSynWarScore(pNpc, nTargetLifeLost);
	}
	DEBUG_CATCH("[early debug]CBattleSystem:: AwardSynWarScore!");

	DEBUG_TRY
	if (pTarget->IsMonster() || (pNpc && pNpc->IsGoal()))
	{
		bool bNewbieTeam = false;
		int nAdditionExp = 0;
		if (!pTarget->IsAlive())
		{
			nAdditionExp = pTarget->GetMaxLife()*KILLBONUS_PERCENT/100;
			if (m_pOwner->GetMap()->IsTrainMap())
				nAdditionExp = 0;

			DEBUG_TRY
			if(pUser)
			{
				CTeam* pTeam	= pUser->GetTeam();
				if (pTeam && !(pNpc && pNpc->IsGoal()))
				{
					pTeam->AwardMemberExp(pUser->GetID(), pTarget, nAdditionExp);
					if (pTeam->IsTeamWithNewbie(pUser->GetID(), pTarget))
					{
						bNewbieTeam = true;
						nAdditionExp *= 2;
					}
				}
			}
			DEBUG_CATCH("[early debug] AwardMemberExp error in BattleSystem");

			if (pUser && pUser->m_dwUnk638 > 0 && ::RandGet(1000) < 5)
			{
				nAdditionExp *= 10;
				pUser->SendSysMsg(_TXTATR_SYSTEM, STR_LUCK_EXP);
				CMsgName msg;
				IF_OK(msg.Create(NAMEACT_ROLEEFFECT, STR_LUCKY_EFFECT, pUser->GetID()))
					pUser->SendMsg(&msg);
			}
			nExp = nTargetLifeLost + nAdditionExp;
		}

		if (pNpc && pNpc->IsGoal())
		{
			if (m_pOwner->IsBowman())
				nExp /= 2;
			nExp = ::CutTrail(1, ::MulDiv(nExp, 10, 100));
		}

		int nRate = CBattleEffect::GetBattleExpRate(m_pOwner, pTarget);
		nExp += nExp * (nRate + (pUser ? pUser->GetGemExpEffect() : 0)) / 100;
		m_pOwner->AwardBattleExp(CBattleEffect::AdjustExp(m_pOwner, pTarget, nExp), true, true);

		int nShowExp = CBattleEffect::AdjustExp(m_pOwner, pTarget, nAdditionExp);
		if (!pTarget->IsAlive() && nShowExp > 0 && pUser && !pUser->IsTimeCardStatus())
		{
			if (!m_pOwner->GetMap()->IsTrainMap())
			{
				if (bNewbieTeam)
					m_pOwner->SendSysMsg(STR_KILLING_EXPEX, nShowExp);
				else
					m_pOwner->SendSysMsg(STR_KILLING_EXPERIENCE, nShowExp);
			}
		}
	}
	DEBUG_CATCH("[early debug]CBattleSystem:: awardexp!");

	DEBUG_TRY
	m_pOwner->AdditionMagic(nTargetLifeLost, nDamage);
	DEBUG_CATCH("[early debug]CBattleSystem:: AdditionMagic!");

	DEBUG_TRY
	CUser* pOwnerUser = NULL;
	if (m_pOwner->QueryObj(OBJ_USER, IPP_OF(pOwnerUser)))
		pOwnerUser->WeaponMagicAttack(pTarget->GetID(), pTarget->GetPosX(), pTarget->GetPosY());
	DEBUG_CATCH("[early debug]CBattleSystem:: weapon with magic!");

	DEBUG_TRY
	if (!pTarget->IsAlive())
		m_pOwner->Kill(pTarget, m_pOwner->IsBowman()?DIE_ARROWSHOOT:DIE_NORMAL, 0);
	DEBUG_CATCH("[early debug]CBattleSystem:: Kill!");

	DEBUG_CATCH("[early debug]CBattleSystem::ProcAttack_Hand2Hand Attack and awardexp!");

	return true;
}

BOOL CBattleSystem::IsBattleMaintain()
{
	if(m_idTarget == ID_NONE)
		return false;

	IRole* pTarget	= RoleManager()->QueryRole(m_idTarget);
	if(!pTarget)
	{
		this->ResetBattle();
		return false;
	}

	if(!m_pOwner->IsAtkable(pTarget, false))
	{
		this->ResetBattle();
		return false;
	}

	if(pTarget->IsWing() && !m_pOwner->IsWing() && !(m_pOwner->IsBowman() || m_pOwner->IsSimpleMagicAtk()))
	{
		this->ResetBattle();
		return false;
	}

	int nDistance = 0;
	if (m_pOwner->IsMonster())
	{
		nDistance = m_pOwner->GetDistance(pTarget->QueryMapThing());
	}
	else
	{
		int nPosX = 0, nPosY = 0;
		pTarget->GetFootPrint(nPosX, nPosY);

		nDistance = m_pOwner->GetDistance(nPosX, nPosY);
	}

	if (nDistance > m_pOwner->GetAttackRange(pTarget->GetSizeAdd()))
	{
		this->ResetBattle();
		return false;
	}
	return true;
}

BOOL CBattleSystem::CreateBattle(OBJID idTarget)
{
	if (idTarget == ID_NONE)
		return false;

	m_idTarget	= idTarget;
	return true;
}

void CBattleSystem::ResetBattle()
{
	m_idTarget	= ID_NONE;
}

BOOL CBattleSystem::IsTargetDodged(IRole* pAtker, IRole* pTarget)
{
	if (!pAtker || !pTarget)
		return true;

	if (pAtker->IsSimpleMagicAtk())
		return (::RandGet(100) >= pAtker->GetSimpleMagicHitRate());

	int nHitRate = pAtker->GetDexterity()/2 - pTarget->GetDdg() + 100;
	nHitRate = pAtker->AdjustHitRate((int)(nHitRate * (pAtker->GetDexterity() > 0 ? ((double)(pAtker->GetDex()/pAtker->GetDexterity()) - 100) * 0.01 + 1 : 1.0)));
	if (pAtker->IsBowman() && pTarget->IsShieldEquip())
		nHitRate = ::MulDiv(nHitRate, BOW_SHIELD_HITRATE_PERCENT, 100);
	nHitRate = ::CutRange(nHitRate, 10, 90);

	CUser* pAtkerUser = NULL;
	pAtker->QueryObj(OBJ_USER, IPP_OF(pAtkerUser));
	if (pAtkerUser)
	{
		int nOldHitRate = nHitRate;
		if (pTarget->IsMonster())
		{
			int nDeltaLev = pAtkerUser->GetLev() - pTarget->GetLev();
			if (nDeltaLev > 0)
				nHitRate += nDeltaLev / 5 * 10;
			if (pAtkerUser->QueryStatus(STATUS_SUPERMAN))
				nHitRate = 100;
		}
		if (pAtkerUser->IsPM())
			pAtkerUser->SendSysMsg(_TXTATR_NORMAL, "lev hit rate+: %d", nHitRate - nOldHitRate);
	}

	return  (::RandGet(100) >= nHitRate);
}

int CBattleSystem::CalcAttackPower(IRole* pAtker, IRole* pTarget, bool bAdjustDamage, int nAdjustAtk, int nAdjustDef, bool bGemAtk)
{
	int nPower = 0;
	if (pAtker->IsBowman())
		nPower = CalcBowPower(pAtker, pTarget, bAdjustDamage, nAdjustAtk, nAdjustDef);
	else
		nPower = CalcHandPower(pAtker, pTarget, bAdjustDamage, nAdjustAtk, nAdjustDef);

	CUser* pUser = NULL;
	pAtker->QueryObj(OBJ_USER, IPP_OF(pUser));
	if (bGemAtk && pUser && pUser->GetGemAtkEffect() >= 210)
		nPower += nPower * 50 / 100;

	CUser* pTargetUser = NULL;
	if (pTarget->QueryObj(OBJ_USER, IPP_OF(pTargetUser)))
	{
		int nGemDef = __min(100, pTargetUser->GetGemMgcAtkEffect());
		if (nGemDef > 0)
		{
			nPower -= (nPower/2) * nGemDef / 100;
			if (nPower < 0)
				nPower = 0;
		}
	}

	if (pUser && pUser->IsAntiMonster(pTarget))
		nPower *= 2;

	nPower = CBattleEffect::AdjustDamage(pAtker, pTarget, nPower);
	nPower = pTarget->AdjustWeaponDamage(pAtker, nPower);

	if (pTarget->QueryObj(OBJ_USER, IPP_OF(pTargetUser)))
	{
		int nReduce = __min(100, pTargetUser->GetEquipReduceDmg());
		if (nReduce > 0)
			nPower += nPower * nReduce / -100;
	}
	return __max(1, nPower);
}

int CBattleSystem::CalcHandPower(IRole* pAtker, IRole* pTarget, bool bAdjustDamage, int nAdjustAtk, int nAdjustDef)
{
	CHECKF(pAtker && pTarget);

	IStatus* pSuperMan = pAtker->QueryStatus(STATUS_SUPERMAN);
	CUser* pUser = NULL;
	pAtker->QueryObj(OBJ_USER, IPP_OF(pUser));

	int nAtk = ::RandGet(pAtker->GetMaxAtk() - pAtker->GetMinAtk()) + pAtker->GetMinAtk();
	if (pUser && !pUser->QueryTransformation())
	{
		int nGemAtk = pUser->GetGemAtkEffect();
		if (nGemAtk > 0)
			nAtk += nAtk * nGemAtk / 100;
		CItem* pWeapon = pUser->GetEquipItemByPos(ITEMPOSITION_WEAPONR);
		if (pWeapon)
		{
			int nSkillAdd = pUser->GetWeaponSkillLev(pWeapon->GetInt(ITEMDATA_TYPE)) - 12;
			if (nSkillAdd > 0)
				nAtk += nAtk * nSkillAdd / 100;
		}
	}

	CUser* pTmpUser;
	if ((pAtker->QueryObj(OBJ_USER, IPP_OF(pTmpUser)) && pTarget->IsMonster())
			|| (pAtker->IsMonster() && pTarget->QueryObj(OBJ_USER, IPP_OF(pTmpUser))))
	{
		CMonster* pAtkMonster = NULL;
		pAtker->QueryObj(OBJ_MONSTER, IPP_OF(pAtkMonster));
		CMonster* pTargetMonster = NULL;
		pTarget->QueryObj(OBJ_MONSTER, IPP_OF(pTargetMonster));
		if ((pAtkMonster && (DWORD)pAtkMonster->GetType() < 3000) || (pTargetMonster && (DWORD)pTargetMonster->GetType() < 3000))
			nAtk = CBattleEffect::AdjustAttack(pAtker, pTarget, nAtk);
		if (pSuperMan)
			nAtk = __max(0, CUser::AdjustData(nAtk, pSuperMan->GetPower()));
	}
	nAtk	= pAtker->AdjustAttack(nAtk);
	if (nAdjustAtk && !bAdjustDamage)
		nAtk = ::CutTrail(0, CUser::AdjustData(nAtk, nAdjustAtk));

	int nRawDef	= pTarget->GetDef();
	int nDef = pTarget->AdjustDefence(nRawDef);
	if(pAtker->IsBowman())
		nDef = pTarget->AdjustBowDefence(nDef);
	if (nAdjustDef)
		nDef = ::CutTrail(0, CUser::AdjustData(nDef, nAdjustDef));
	if (pSuperMan)
		nDef /= 2;

	int	nDamage	= (nAtk > nDef) ? nAtk - nDef : 0;
	if (pUser)
		nDamage = pUser->LuckyCriticalHit(nDamage, pTarget);
	if (nAdjustAtk && bAdjustDamage)
		nDamage = ::CutTrail(0, CUser::AdjustData(nDamage, nAdjustAtk));

	if (nDef <= nRawDef)
	{
		int nDivisor = 10;
		switch ((DWORD)pTarget->GetArmorTypeID() % 10)
		{
		case 6:	nDivisor = 100;		break;
		case 7:	nDivisor = 200;		break;
		case 8:	nDivisor = 500;		break;
		case 9:	nDivisor = 1000;	break;
		}
		if (__max(1, (int)pAtker->GetAttack() / nDivisor) > nDamage)
			nDamage = __max(1, (int)pAtker->GetAttack() / nDivisor);
	}
	return ::CutTrail(0, nDamage);
}

int CBattleSystem::CalcBowPower(IRole* pAtker, IRole* pTarget, bool bAdjustDamage, int nAdjustAtk, int nAdjustDef)
{
	CHECKF(pAtker && pTarget);

	IStatus* pSuperMan = pAtker->QueryStatus(STATUS_SUPERMAN);
	CUser* pUser = NULL;
	pAtker->QueryObj(OBJ_USER, IPP_OF(pUser));

	int nAtk = ::RandGet(pAtker->GetMaxAtk() - pAtker->GetMinAtk()) + pAtker->GetMinAtk();
	if (pUser && !pUser->QueryTransformation())
	{
		int nGemAtk = pUser->GetGemAtkEffect();
		if (nGemAtk > 0)
			nAtk += nAtk * nGemAtk / 100;
		CItem* pWeapon = pUser->GetEquipItemByPos(ITEMPOSITION_WEAPONR);
		if (pWeapon)
		{
			int nSkillAdd = pUser->GetWeaponSkillLev(pWeapon->GetInt(ITEMDATA_TYPE)) - 12;
			if (nSkillAdd > 0)
				nAtk += nAtk * nSkillAdd / 100;
		}
	}

	if ((!pAtker->IsMonster() && pTarget->IsMonster()) || (pAtker->IsMonster() && !pTarget->IsMonster()))
	{
		CMonster* pAtkMonster = NULL;
		pAtker->QueryObj(OBJ_MONSTER, IPP_OF(pAtkMonster));
		CMonster* pTargetMonster = NULL;
		pTarget->QueryObj(OBJ_MONSTER, IPP_OF(pTargetMonster));
		if ((pAtkMonster && (DWORD)pAtkMonster->GetType() < 3000) || (pTargetMonster && (DWORD)pTargetMonster->GetType() < 3000))
			nAtk = CBattleEffect::AdjustAttack(pAtker, pTarget, nAtk);
		if (pSuperMan)
			nAtk = ::CutTrail(0, CUser::AdjustData(nAtk, pSuperMan->GetPower()));
	}
	nAtk	= pAtker->AdjustAttack(nAtk);
	if (nAdjustAtk && !bAdjustDamage)
		nAtk = ::CutTrail(0, CUser::AdjustData(nAtk, nAdjustAtk));

	int nRawDef	= pTarget->GetDef();
	int nDef = pTarget->AdjustDefence(nRawDef);
	if(pAtker->IsBowman())
		nDef = pTarget->AdjustBowDefence(nDef);
	if (nAdjustDef)
		nDef = ::CutTrail(0, CUser::AdjustData(nDef, nAdjustDef));
	if (pSuperMan)
		nDef /= 2;

	CUser* pTargetUser = NULL;
	int nDodge = pTarget->GetDdg();
	if (pTarget->QueryObj(OBJ_USER, IPP_OF(pTargetUser)))
		nDodge = ::CutTrail(0, nDodge - 50);
	else
		nDodge = ::CutTrail(0, nDodge - 25);
	int	nDamage	= __max(0, nAtk - ::MulDiv(nAtk, nDodge, 100));
	if (pUser)
		nDamage = pUser->LuckyCriticalHit(nDamage, pTarget);
	if (nAdjustAtk && bAdjustDamage)
		nDamage = ::CutTrail(0, CUser::AdjustData(nDamage, nAdjustAtk));

	if (nDef <= nRawDef)
	{
		int nDivisor = 10;
		switch ((DWORD)pTarget->GetArmorTypeID() % 10)
		{
		case 6:	nDivisor = 100;		break;
		case 7:	nDivisor = 200;		break;
		case 8:	nDivisor = 500;		break;
		case 9:	nDivisor = 1000;	break;
		}
		if (__max(1, (int)pAtker->GetAttack() / nDivisor) > nDamage)
			nDamage = __max(1, (int)pAtker->GetAttack() / nDivisor);
	}

	if (pTargetUser)
	{
		int nLev = pTarget->GetLev();
		if (nLev >= 110)
			nDamage = ::MulDiv(nDamage, 12, 100);
		else if (nLev >= 90)
			nDamage = ::MulDiv(nDamage, 15, 100);
		else if (nLev >= 50)
			nDamage = ::MulDiv(nDamage, 20, 100);
		else if (nLev >= 22)
			nDamage = ::MulDiv(nDamage, 25, 100);
		else
			nDamage = ::MulDiv(nDamage, 35, 100);
	}
	return ::CutTrail(0, nDamage);
}
