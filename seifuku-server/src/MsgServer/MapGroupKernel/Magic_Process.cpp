#include "AllMsg.h"
#include "GameObj.h"
#include "Magic.h"
#include "MapGroup.h"
#include "StatusOnce.h"
#include "StatusMore.h"
#include "MapTrap.h"

const int	SUPER_MAGIC_RECRUIT			= 3500;
const int	SUPER_MAGIC_DEFENCE			= 3501;
const int	SUPER_MAGIC_MGC_DEFENCE		= 3502;
const int	SUPER_MAGIC_ATTACK			= 3503;
const int	SUPER_MAGIC_MGC_ATTACK		= 3504;

const int	THROW_WEAPON_DROP_RANGE		= 1;

void	MovePos(POINT* pPos, int nDir)
{
	nDir %= 8;
	pPos->x	+= _DELTA_X[nDir];
	pPos->y	+= _DELTA_X[nDir];
}

void	DDALine(int x0, int y0, int x1, int y1, int nRange, vector<POINT>& vctPoint)
{
	if (x0 == x1 && y0 == y1)
		return;

	float scale = 1.0f*nRange/sqrt((double)((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)));
	x1 = int(0.5f+scale*(x1-x0)+x0);
	y1 = int(0.5f+scale*(y1-y0)+y0);

	::DDALineEx(x0, y0, x1, y1, vctPoint);
}

int CMagic::Launch()
{
	LaunchSort();
	if(m_pMagic && !IsWeaponMagic(m_pMagic->GetInt(MAGICDATA_TYPE)) && m_pMagic->GetInt(MAGICDATA_CRIME) && !m_pMagic->GetInt(MAGICDATA_GROUND))
	{
		CHECKF(!m_bInMagicLoop.data);
		m_bInMagicLoop.data	= true;
		m_pUser->WeaponMagicAttack(m_idTarget, m_pos.x, m_pos.y);
		m_bInMagicLoop.data	= false;
	}

	ResetDelay();
	return 1;
}

int CMagic::LaunchSort()
{
	if(!m_pMagic)
		return 0;

	if(!m_pUser->IsAlive())
	{
		ShowMiss();
		return 0;
	}

	if(::RandGet(100) >= m_pMagic->GetInt(MAGICDATA_PERCENT) && m_pMagic->GetInt(MAGICDATA_SORT) != MAGICSORT_COLLIDE)
	{
		ShowMiss();
		return 0;
	}

	int ret = 0;
	switch(m_pMagic->GetInt(MAGICDATA_SORT))
	{
	case	MAGICSORT_ATTACK:
		ret = ProcessAttack();
		break;
	case	MAGICSORT_RECRUIT:
		ret = ProcessRecruit();
		break;
	case	MAGICSORT_CROSS:
		ret = ProcessCross();
		break;
	case	MAGICSORT_LINE:
		ret = ProcessLine();
		break;
	case	MAGICSORT_FAN:
		ret = ProcessFan();
		break;
	case	MAGICSORT_BOMB:
		ret = ProcessBomb();
		break;
	case	MAGICSORT_ATTACHSTATUS:
		ret = ProcessAttach();
		break;
	case	MAGICSORT_DETACHSTATUS:
		ret = ProcessDetach();
		break;
	case	MAGICSORT_SQUARE:
		ret = ProcessSquare();
		break;
	case	MAGICSORT_JUMPATTACK:
		ret = ProcessJumpAtk();
		break;
	case	MAGICSORT_RANDOMTRANS:
		ret = ProcessRandomTrans();
		break;
	case	MAGICSORT_DISPATCHXP:
		ret = ProcessDispatchXP();
		break;
	case	MAGICSORT_COLLIDE:
		ret = ProcessCollide();
		break;
	case	MAGICSORT_ATKSTATUS:
		ret = ProcessAtkStatus();
		break;
	case	MAGICSORT_CALLTEAMMEMBER:
		ret = ProcessCallTeamMember();
		break;
	case	MAGICSORT_RECORDTRANSSPELL:
		ret = ProcessRecordTransSpell();
		break;
	case	MAGICSORT_TRANSFORM:
		ret = ProcessTransform();
		break;
	case	MAGICSORT_ADDMANA:
		ret = ProcessAddMana();
		break;
	case	MAGICSORT_LAYTRAP:
		ret = ProcessLayTrap();
		break;
	case	MAGICSORT_CALLPET:
		ret = ProcessCallPet();
		break;
	case	MAGICSORT_DECLIFE:
		ret = ProcessDecLife();
		break;
	default:
		MYASSERT(!"switch magic sort");
	}

	return ret;
}

bool CMagic::ProcessAttack()
{
	CHECKF(m_pMagic);
	CHECKF(m_pUser);

	IRole* pTarget = m_pUser->FindAroundRole(m_idTarget);
	if(!pTarget || !pTarget->IsAlive())
		return false;

	if(m_pUser->GetDistance(pTarget->GetPosX(), pTarget->GetPosY()) > m_pMagic->GetInt(MAGICDATA_DISTANCE))
		return false;

	if (this->IsImmunity(pTarget, 0))
		return false;

	if (m_pMagic->GetInt(MAGICDATA_FLOOR_ATTR))
	{
		int nAttr = m_pUser->GetMap()->GetFloorAttr(pTarget->GetPosX(), pTarget->GetPosY());
		if(m_pMagic->GetInt(MAGICDATA_FLOOR_ATTR) != nAttr)
			return false;
	}

	int	nPower = 0;
	if(HitByMagic())
		nPower = CMagic::CalcMagicPower(m_pUser, pTarget, m_pMagic->GetInt(MAGICDATA_POWER));
	if(HitByWeapon())
		nPower += CBattleSystem::CalcAttackPower(m_pUser, pTarget, true, m_pMagic->GetInt(MAGICDATA_POWER), 0, false);

	CMsgMagicEffect	msg;
	int nDeltaBattleEffect = 0;
	if(m_pUser)
	{
		IRole* pRole = RoleManager()->QueryRole(m_idTarget);
		CUser* pTargetUser = pRole ? pRole->QueryOwnerUser() : NULL;
		if(pTargetUser)
			nDeltaBattleEffect = m_pUser->GetBattlePower() - pTargetUser->GetBattlePower();
	}
	IF_OK(msg.Create(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_idTarget, nPower, nDeltaBattleEffect))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	CheckCrime(pTarget);

	if (nPower)
	{
		nPower = pTarget->BeAttackEx(m_pUser, nPower, true);
		int nLifeLost = __min((int)pTarget->GetLife(), nPower);
		if (nLifeLost > 0)
		{
			pTarget->AddAttrib(_USERATTRIB_LIFE, -1*nLifeLost, SYNCHRO_TRUE, NULL);

			CUser* pTargetUser = NULL;
			if(pTarget->QueryObj(OBJ_USER, IPP_OF(pTargetUser)))
				pTargetUser->DecEquipmentDurability(true, true);

			AwardExpOfLife(pTarget, nLifeLost);

			if (!pTarget->IsAlive())
			{
				m_pUser->Kill(pTarget, GetDieMode(), 0);	}	}	}

				if (m_pMagic->GetInt(MAGICDATA_USE_XP) == TYPE_KONGFU && m_pMagic->GetInt(MAGICDATA_DROPWEAPON) == 1)
				{
					CItem* pWeapon = m_pUser->GetWeaponR();
					IF_OK(pWeapon)
					{
						POINT	pos;
						pos.x	= pTarget->GetPosX();	pos.y	= pTarget->GetPosY();
						m_pUser->GetMap()->FindDropItemCell(THROW_WEAPON_DROP_RANGE, &pos);
						m_pUser->DropItem(pWeapon->GetID(), pos.x, pos.y);
					}
				}

	return true;
}
bool CMagic::ProcessAtkStatus()
{
	CHECKF(m_pMagic);

	IRole* pTarget = m_pUser->FindAroundRole(m_idTarget);
	if(!pTarget || !pTarget->IsAlive())
		return false;

	if(m_pUser->GetDistance(pTarget->GetPosX(), pTarget->GetPosY()) > m_pMagic->GetInt(MAGICDATA_DISTANCE))
		return false;

	if (this->IsImmunity(pTarget, 0))
		return false;

	int	nPower	= 0;
	if(HitByWeapon())
	{
		switch(m_pMagic->GetInt(MAGICDATA_STATUS))
		{
		case	STATUS_ATTACK:
			{
				int	nAdjustAtk	= m_pMagic->GetInt(MAGICDATA_POWER);
				nPower = CBattleSystem::CalcAttackPower(m_pUser, pTarget, false, nAdjustAtk, 0, false);
			}
			break;
		case	STATUS_DEFENCE:
			{
				int	nAdjustDef	= m_pMagic->GetInt(MAGICDATA_POWER);
				nPower = CBattleSystem::CalcAttackPower(m_pUser, pTarget, false, 0, nAdjustDef, false);
			}
			break;
		case	STATUS_WEAPONDAMAGE:
			{
				int	nAdjustDmg	= m_pMagic->GetInt(MAGICDATA_POWER);
				nPower = CBattleSystem::CalcAttackPower(m_pUser, pTarget, true, nAdjustDmg, 0, false);
				nPower = ::CutRange(nPower, 0, (int)pTarget->GetMaxLife()*9/10);
			}
			break;
		case	STATUS_ATKSPEED:
			{
				int	nPause	= m_pMagic->GetInt(MAGICDATA_POWER);
				nPower = CBattleSystem::CalcAttackPower(m_pUser, pTarget, false, 0, 0, false);
				pTarget->SetFightPause(nPause);
			}
			break;
		case	STATUS_STOP:
			{
				int	nMilliSecs	= m_pMagic->GetInt(MAGICDATA_POWER);
				nPower = CBattleSystem::CalcAttackPower(m_pUser, pTarget, false, 0, 0, false);
				if(pTarget->IsMonster())
				{
					CMsgAction	msg;
					IF_OK(msg.Create(pTarget->GetID(), pTarget->GetPosX(), pTarget->GetPosY(), pTarget->GetDir(), actionMoveStop, nMilliSecs))
						pTarget->SendMsg(&msg);
				}
			}
			break;
		case	STATUS_NORMAL:
			break;
		default:
			{
				int		nStatusPower	= m_pMagic->GetInt(MAGICDATA_POWER);
				int		nSecs	= m_pMagic->GetInt(MAGICDATA_STEP);
				int		nTimes	= m_pMagic->GetInt(MAGICDATA_TIMES);
				DWORD	nStatus	= m_pMagic->GetInt(MAGICDATA_STATUS);
				MYASSERT(nStatusPower > 0);
				CRole::AttachStatus(pTarget, nStatus, nStatusPower, nSecs, nTimes);

				nPower = CBattleSystem::CalcAttackPower(m_pUser, pTarget, false, 0, 0, false);
			}
			break;
		}
	}

	CMsgMagicEffect	msg;
	int nDeltaBattleEffect = 0;
	if(m_pUser)
	{
		IRole* pRole = RoleManager()->QueryRole(m_idTarget);
		CUser* pTargetUser = pRole ? pRole->QueryOwnerUser() : NULL;
		if(pTargetUser)
			nDeltaBattleEffect = m_pUser->GetBattlePower() - pTargetUser->GetBattlePower();
	}
	IF_OK(msg.Create(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_idTarget, nPower, nDeltaBattleEffect))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	CheckCrime(pTarget);

	if (nPower)
	{
		nPower = pTarget->BeAttackEx(m_pUser, nPower, true);
		int nLifeLost = __min((int)pTarget->GetLife(), nPower);
		if (nLifeLost > 0)
		{
			pTarget->AddAttrib(_USERATTRIB_LIFE, -1*nLifeLost, SYNCHRO_TRUE, NULL);
			CUser* pTargetUser=NULL;
			if(pTarget->QueryObj(OBJ_USER, IPP_OF(pTargetUser)))
				pTargetUser->BroadcastTeamLife();

			CNpc* pNpc = NULL;
			if(pTarget->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsAwardScore())
				m_pUser->AwardSynWarScore(pNpc, nLifeLost);

			int nExp = 0;
			if (pTarget->IsMonster() || pNpc && pNpc->IsGoal() && m_pUser->GetLev() >= pNpc->GetLev())
			{
				nExp += m_pUser->AdjustExp(pTarget, nLifeLost);
				if (!pTarget->IsAlive())
				{
					int	nBonusExp	= pTarget->GetMaxLife()*KILLBONUS_PERCENT/100;
					OtherMemberAwardExp(pTarget, nBonusExp);

					nExp += m_pUser->AdjustExp(pTarget, nBonusExp, true);
				}
			}
			if (!pTarget->IsAlive())
				m_pUser->Kill(pTarget, GetDieMode(), 0);

			this->AwardExp(AWARDEXP_BY_TIMES, nExp);
		}
	}

	return true;
}

bool CMagic::ProcessRecruit()
{
	CHECKF(m_pMagic);

	vector<IRole*>	setRole;
	vector<int>		setPower;
	CTeam* pTeam = m_pUser->GetTeam();
	if(pTeam && m_pMagic->GetInt(MAGICDATA_MULTI))
	{
		for(int i = 0; i < pTeam->GetMemberAmount(); i++)
		{
			OBJID idTarget = pTeam->GetMemberByIndex(i);
			IRole* pRole = m_pUser->FindAroundRole(idTarget);
			CNpc* pNpc;
			if(!pRole || !pRole->IsAlive() || pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)))
				continue;
			setRole.push_back(pRole);

			int nPower = m_pMagic->GetInt(MAGICDATA_POWER);
			if (nPower == ADJUST_FULL)
				nPower = pRole->GetMaxLife() - pRole->GetLife();
			setPower.push_back(nPower);
		}
	}
	else
	{
		IRole* pRole = m_pUser->FindAroundRole(m_idTarget);
		if(!pRole || !pRole->IsAlive())
			return false;
		setRole.push_back(pRole);
		int nPower = m_pMagic->GetInt(MAGICDATA_POWER);
		if (nPower == ADJUST_FULL)
			nPower = pRole->GetMaxLife() - pRole->GetLife();
		setPower.push_back(nPower);
	}

	int	nExp = 0;
	CMsgMagicEffect	msg;
	IF_OK(msg.Create(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_idTarget))
	{
		for(int i = 0; i < (int)setRole.size(); )
		{
			if(msg.AppendRole(setRole[i]->GetID(), setPower[i], 0))
				i++;
			else
			{
				m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
				msg.Create(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_idTarget);
			}
		}
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}

	for(int i = 0; i < (int)setRole.size(); i++)
	{
		int nAddLife = ::CutOverflow(setPower[i], ::CutTrail(0, (int)(setRole[i]->GetMaxLife()-setRole[i]->GetLife())));
		if(nAddLife > 0)
		{
			setRole[i]->AddAttrib(_USERATTRIB_LIFE, nAddLife, SYNCHRO_TRUE, NULL);
			CUser* pUser=NULL;
			if(setRole[i]->QueryObj(OBJ_USER, IPP_OF(pUser)))
				pUser->BroadcastTeamLife();
		}

		CNpc* pNpc = NULL;
		if (setRole[i])
		{
			setRole[i]->QueryObj(OBJ_NPC, IPP_OF(pNpc));
			if(pNpc && pNpc->IsGoal())
			{
				nAddLife = nAddLife*GOAL_EXP_PERCENT/100;
			}
		}

		nExp += nAddLife;
	}

	this->AwardExp(nExp, 0);

	return true;
}

bool CMagic::ProcessCross()
{
	CHECKF(m_pMagic);

	int nDir	= ::GetDirByPos(m_pUser->GetPosX(), m_pUser->GetPosY(), m_pos.x, m_pos.y);
	POINT	pos;
	pos.x		= m_pUser->GetPosX();
	pos.y		= m_pUser->GetPosY();

	int	nRange1	= m_pMagic->GetInt(MAGICDATA_RANGE) % 100;
	int	nRange2	= m_pMagic->GetInt(MAGICDATA_RANGE) / 100;

	int	nSize		= nRange1*2 + 1;
	MAPTHING_SET	setTarget;
	m_pUser->GetMap()->CollectMapThing(setTarget, pos, nRange1, OBJ_USER|OBJ_MONSTER|OBJ_NPC);

	vector<IRole*>	setRole;
	vector<int>		setPower;

	int nThisDir = nDir;
	for(int i = 0; i < 4; i++)
	{
		POINT	posThis = pos;
		for(int j = 0; j < (i%2 ? nRange1 : nRange2); j++)
		{
			MovePos(&posThis, nThisDir);
			if(!m_pUser->GetMap()->IsValidPoint(posThis))
				continue;
			if(!m_pUser->GetMap()->IsAltEnable(pos, posThis, MAGICDAMAGE_ALT))
				continue;

			int idx = POS2INDEX(posThis.x - pos.x + nRange1, posThis.y - pos.y + nRange1, nSize, nSize);
			MYASSERT(0 <= idx && idx < (int)setTarget.size());
			if(setTarget[idx])
			{
				IRole* pRole = NULL;
				if(setTarget[idx]->QueryRole(IPP_OF(pRole)) && pRole->IsAlive() && !IsImmunity(pRole, m_pMagic->GetInt(MAGICDATA_SORT)))
				{
					int nPower = 0;
					if(HitByMagic())
						nPower += CMagic::CalcMagicPower(m_pUser, pRole, m_pMagic->GetInt(MAGICDATA_POWER));
					if(HitByWeapon())
						nPower += CBattleSystem::CalcAttackPower(m_pUser, pRole, true, m_pMagic->GetInt(MAGICDATA_POWER), 0, false);
					setRole.push_back(pRole);
					setPower.push_back(nPower);
				}
			}
		}
		nThisDir = (nThisDir+2) % 8;
	}

	CMsgMagicEffect	msg;
	IF_OK(msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), pos.x, pos.y))
	{
		for(int i = 0; i < (int)setRole.size(); )
		{
			int nDeltaBattleEffect = 0;
			CUser* pTargetUser = setRole[i]->QueryOwnerUser();
			if(pTargetUser)
				nDeltaBattleEffect = m_pUser->GetBattlePower() - pTargetUser->GetBattlePower();
			if(msg.AppendRole(setRole[i]->GetID(), setPower[i], nDeltaBattleEffect))
				i++;
			else
			{
				m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
				msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), pos.x, pos.y);
			}
		}
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}

	CheckCrime(&setRole);

	int nExp = 0;
	map<OBJID, int>	setExpRate;
	for (int i = 0; i < (int)setRole.size(); i++)
	{
		setPower[i] = setRole[i]->BeAttackEx(m_pUser, setPower[i], true);
		int nLifeLost = __min((int)setRole[i]->GetLife(), setPower[i]);
		if (nLifeLost <= 0)
			continue;

		setRole[i]->AddAttrib(_USERATTRIB_LIFE, -1*nLifeLost, SYNCHRO_TRUE, NULL);
		CNpc* pNpc = NULL;
		if(setRole[i]->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsAwardScore())
			m_pUser->AwardSynWarScore(pNpc, nLifeLost);

		if (setRole[i]->IsMonster() || pNpc && pNpc->IsGoal() && m_pUser->GetLev() >= pNpc->GetLev())
		{
			int nAdjustExp = 0;
			CMonster* pMonster = NULL;
			if(setRole[i]->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
			{
				if(setExpRate.find(pMonster->GetType()) == setExpRate.end())
				{
					nAdjustExp = m_pUser->AdjustExp(setRole[i], nLifeLost);
					setExpRate[pMonster->GetType()] = nAdjustExp*10000 / nLifeLost;
				}
				else
					nAdjustExp = setExpRate[pMonster->GetType()] * nLifeLost / 10000;
			}
			else
				nAdjustExp = m_pUser->AdjustExp(setRole[i], nLifeLost);
			nExp += nAdjustExp;

			if (!setRole[i]->IsAlive())
			{
				int	nBonusExp	= setRole[i]->GetMaxLife()*KILLBONUS_PERCENT/100;
				OtherMemberAwardExp(setRole[i], nBonusExp);

				nExp += m_pUser->AdjustExp(setRole[i], nBonusExp, true);
			}
		}

		if(!setRole[i]->IsAlive())
			m_pUser->Kill(setRole[i], DIE_MAGIC, 0);
	}

	AwardExp(nExp, nExp);

	return true;
}

bool CMagic::ProcessLine()
{
	CHECKF(m_pMagic);

	POINT pos;
	pos.x	= m_pUser->GetPosX();
	pos.y	= m_pUser->GetPosY();

	vector<IRole*>	setTarget;
	vector<int>		setPower;
	vector<POINT>	setPoint;
	DDALine(pos.x, pos.y, m_pos.x, m_pos.y, m_pMagic->GetInt(MAGICDATA_RANGE), setPoint);

	for(int i = 0; i < (int)setPoint.size(); i++)
	{
		POINT posThis = setPoint[i];
		if(!m_pUser->GetMap()->IsValidPoint(posThis))
			continue;

		if(!m_pUser->GetMap()->IsAltEnable(pos, posThis, MAGICDAMAGE_ALT))
			continue;

		IRole* pTarget = m_pUser->GetMap()->QueryRole(setPoint[i].x, setPoint[i].y, ID_NONE);
		if(!pTarget || !pTarget->IsAlive() || this->IsImmunity(pTarget, m_pMagic->GetInt(MAGICDATA_SORT)))
			continue;

		int nPower = 0;
		if(HitByMagic())
			nPower += CMagic::CalcMagicPower(m_pUser, pTarget, m_pMagic->GetInt(MAGICDATA_POWER));
		if(HitByWeapon())
			nPower += CBattleSystem::CalcAttackPower(m_pUser, pTarget, true, m_pMagic->GetInt(MAGICDATA_POWER), 0, false);
		setTarget.push_back(pTarget);
		setPower.push_back(nPower);
	}

	CMsgMagicEffect	msg;
	IF_OK(msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_pos.x, m_pos.y))
	{
		for(int i = 0; i < (int)setTarget.size(); )
		{
			int nDeltaBattleEffect = 0;
			CUser* pTargetUser = setTarget[i]->QueryOwnerUser();
			if(pTargetUser)
				nDeltaBattleEffect = m_pUser->GetBattlePower() - pTargetUser->GetBattlePower();
			if(msg.AppendRole(setTarget[i]->GetID(), setPower[i], nDeltaBattleEffect))
				i++;
			else
			{
				m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
				msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_pos.x, m_pos.y);
			}
		}
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}

	CheckCrime(&setTarget);

	int	nExp = 0;
	for (int i = 0; i < (int)setTarget.size(); i++)
	{
		setPower[i] = setTarget[i]->BeAttackEx(m_pUser, setPower[i], true);
		int nLifeLost = __min((int)setTarget[i]->GetLife(), setPower[i]);
		if (nLifeLost <= 0)
			continue;

		setTarget[i]->AddAttrib(_USERATTRIB_LIFE, -1*nLifeLost, SYNCHRO_TRUE, NULL);
		CNpc* pNpc = NULL;
		if(setTarget[i]->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsAwardScore())
			m_pUser->AwardSynWarScore(pNpc, nLifeLost);

		if (setTarget[i]->IsMonster() || pNpc && pNpc->IsGoal() && m_pUser->GetLev() >= pNpc->GetLev())
		{
			nExp += this->m_pUser->AdjustExp(setTarget[i], nLifeLost);
			if (!setTarget[i]->IsAlive())
			{
				int	nBonusExp	= setTarget[i]->GetMaxLife()*KILLBONUS_PERCENT/100;
				OtherMemberAwardExp(setTarget[i], nBonusExp);

				nExp += m_pUser->AdjustExp(setTarget[i], nBonusExp, true);
			}
		}

		if(!setTarget[i]->IsAlive())
			m_pUser->Kill(setTarget[i], GetDieMode(), 0);
	}

	if(HitByWeapon() && !(m_pUser->GetMap()->GetID() == 1039))
	{
		CItem* pWeaponR = m_pUser->GetWeaponR();
		if(pWeaponR && pWeaponR->GetInt(ITEMDATA_FRAY_MODE) == 0)
			m_pUser->AddEquipmentDurability(ITEMPOSITION_WEAPONR, -1*LINE_WEAPON_DURABILITY);
		CItem* pWeaponL = m_pUser->GetWeaponL();
		if(pWeaponL && pWeaponL->GetInt(ITEMDATA_FRAY_MODE) == 0)
			m_pUser->AddEquipmentDurability(ITEMPOSITION_WEAPONL, -1*LINE_WEAPON_DURABILITY);
	}

	AwardExp(nExp, nExp);
	return true;
}

bool CMagic::ProcessFan()
{
	CHECKF(m_pMagic);

	POINT	pos;
	pos.x		= m_pUser->GetPosX();
	pos.y		= m_pUser->GetPosY();
	int	nRange	= m_pMagic->GetInt(MAGICDATA_RANGE)+MAX_SIZEADD;

	MAPTHING_SET	setTarget;
	m_pUser->GetMap()->CollectMapThing(setTarget, pos, nRange, OBJ_USER|OBJ_MONSTER|OBJ_NPC);

	vector<IRole*>	setRole;
	vector<int>		setPower;
	for(int i = 0; i < (int)setTarget.size(); i++)
	{
		IRole* pRole = NULL;
		if(!setTarget[i] || !setTarget[i]->QueryRole(IPP_OF(pRole)))
			continue;

		POINT posThis;
		posThis.x	= pRole->GetPosX();
		posThis.y	= pRole->GetPosY();
		if(!m_pUser->IsInFan(posThis, nRange, m_pos))
			continue;
		if(!pRole->IsAlive() || IsImmunity(pRole, m_pMagic->GetInt(MAGICDATA_SORT)))
			continue;
		if(m_pUser->GetDistance(pRole->QueryMapThing()) > m_pUser->GetAttackRange(pRole->GetSizeAdd()))
		{
			if(m_pMagic->GetInt(MAGICDATA_TYPE) != 2001 || m_pUser->GetDistance(pRole->QueryMapThing()) > m_pMagic->GetInt(MAGICDATA_RANGE))
				continue;
		}

		int nPower = 0;
		if(HitByMagic())
			nPower += CMagic::CalcMagicPower(m_pUser, pRole, m_pMagic->GetInt(MAGICDATA_POWER));
		if(HitByWeapon())
			nPower += CBattleSystem::CalcAttackPower(m_pUser, pRole, true, m_pMagic->GetInt(MAGICDATA_POWER), 0, false);
		setRole.push_back(pRole);
		setPower.push_back(nPower);
	}

	CMsgMagicEffect	msg;
	IF_OK(msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_pos.x, m_pos.y))
	{
		for(int i = 0; i < (int)setRole.size(); )
		{
			int nDeltaBattleEffect = 0;
			CUser* pTargetUser = setRole[i]->QueryOwnerUser();
			if(pTargetUser)
				nDeltaBattleEffect = m_pUser->GetBattlePower() - pTargetUser->GetBattlePower();
			if(msg.AppendRole(setRole[i]->GetID(), setPower[i], nDeltaBattleEffect))
				i++;
			else
			{
				m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
				msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_pos.x, m_pos.y);
			}
		}
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}

	CheckCrime(&setRole);

	int nExp = 0;
	map<OBJID, int>	setExpRate;
	for (int i = 0; i < (int)setRole.size(); i++)
	{
		setPower[i] = setRole[i]->BeAttackEx(m_pUser, setPower[i], true);
		int nLifeLost = __min((int)setRole[i]->GetLife(), setPower[i]);
		if (nLifeLost <= 0)
			continue;

		setRole[i]->AddAttrib(_USERATTRIB_LIFE, -1*nLifeLost, SYNCHRO_TRUE, NULL);
		CNpc* pNpc = NULL;
		if(setRole[i]->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsAwardScore())
			m_pUser->AwardSynWarScore(pNpc, nLifeLost);

		if (setRole[i]->IsMonster() || pNpc && pNpc->IsGoal() && m_pUser->GetLev() >= pNpc->GetLev())
		{
			int nAdjustExp = 0;
			CMonster* pMonster = NULL;
			if(setRole[i]->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
			{
				if(setExpRate.find(pMonster->GetType()) == setExpRate.end())
				{
					nAdjustExp = m_pUser->AdjustExp(setRole[i], nLifeLost);
					setExpRate[pMonster->GetType()] = nAdjustExp*10000 / nLifeLost;
				}
				else
					nAdjustExp = setExpRate[pMonster->GetType()] * nLifeLost / 10000;
			}
			else
				nAdjustExp = m_pUser->AdjustExp(setRole[i], nLifeLost);
			nExp += nAdjustExp;

			if (!setRole[i]->IsAlive())
			{
				int	nBonusExp	= setRole[i]->GetMaxLife()*KILLBONUS_PERCENT/100;
				OtherMemberAwardExp(setRole[i], nBonusExp);

				nExp += m_pUser->AdjustExp(setRole[i], nBonusExp, true);
			}
		}

		if(!setRole[i]->IsAlive())
			m_pUser->Kill(setRole[i], GetDieMode(), 0);
	}

	AwardExp(nExp, nExp);
	return true;
}

bool CMagic::ProcessBomb()
{
	CHECKF(m_pMagic);

	int nDir	= m_pUser->GetDir();
	POINT	pos;
	if (m_pMagic->GetInt(MAGICDATA_GROUND))
	{
		pos.x		= m_pUser->GetPosX();
		pos.y		= m_pUser->GetPosY();
	}
	else
	{
		IRole* pTarget = m_pUser->FindAroundRole(m_idTarget);
		if(!pTarget || !pTarget->IsAlive())
			return false;
		if(m_pUser->GetDistance(pTarget->GetPosX(), pTarget->GetPosY()) > m_pMagic->GetInt(MAGICDATA_DISTANCE))
			return false;
		pos.x		= pTarget->GetPosX();
		pos.y		= pTarget->GetPosY();
	}

	int	nRange	= m_pMagic->GetInt(MAGICDATA_RANGE);
	MAPTHING_SET	setTarget;
	m_pUser->GetMap()->CollectMapThing(setTarget, pos, nRange, OBJ_USER|OBJ_MONSTER|OBJ_NPC);

	vector<IRole*>	setRole;
	vector<int>		setPower;
	for(int i = 0; i < (int)setTarget.size(); i++)
	{
		IRole* pRole = NULL;
		if(!setTarget[i] || !setTarget[i]->QueryRole(IPP_OF(pRole)))
			continue;

		POINT posThis;
		posThis.x	= pRole->GetPosX();
		posThis.y	= pRole->GetPosY();
		if (!CUser::IsInCircle(posThis, pos, nRange))
			continue;
		if(!pRole->IsAlive() || IsImmunity(pRole, m_pMagic->GetInt(MAGICDATA_SORT)))
			continue;

		int nPower = 0;
		if(HitByMagic())
			nPower += CMagic::CalcMagicPower(m_pUser, pRole, m_pMagic->GetInt(MAGICDATA_POWER));
		if(HitByWeapon())
			nPower += CBattleSystem::CalcAttackPower(m_pUser, pRole, true, m_pMagic->GetInt(MAGICDATA_POWER), 0, false);
		setRole.push_back(pRole);
		setPower.push_back(nPower);
	}

	CMsgMagicEffect	msg;
	IF_OK(msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), pos.x, pos.y))
	{
		for(int i = 0; i < (int)setRole.size(); )
		{
			int nDeltaBattleEffect = 0;
			CUser* pTargetUser = setRole[i]->QueryOwnerUser();
			if(pTargetUser)
				nDeltaBattleEffect = m_pUser->GetBattlePower() - pTargetUser->GetBattlePower();
			if(msg.AppendRole(setRole[i]->GetID(), setPower[i], nDeltaBattleEffect))
				i++;
			else
			{
				m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
				msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), pos.x, pos.y);
			}
		}
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}

	CheckCrime(&setRole);

	int nKillCount = 0;
	int nExp = 0;
	map<OBJID, int>	setExpRate;
	for (int i = 0; i < (int)setRole.size(); i++)
	{
		setPower[i] = setRole[i]->BeAttackEx(m_pUser, setPower[i], true);
		int nLifeLost = __min((int)setRole[i]->GetLife(), setPower[i]);
		if (nLifeLost <= 0)
			continue;

		setRole[i]->AddAttrib(_USERATTRIB_LIFE, -1*nLifeLost, SYNCHRO_TRUE, NULL);
		CNpc* pNpc = NULL;
		if(setRole[i]->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsAwardScore())
			m_pUser->AwardSynWarScore(pNpc, nLifeLost);

		if (setRole[i]->IsMonster() || pNpc && pNpc->IsGoal() && m_pUser->GetLev() >= pNpc->GetLev())
		{
			int nAdjustExp = 0;
			CMonster* pMonster = NULL;
			if(setRole[i]->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
			{
				if(setExpRate.find(pMonster->GetType()) == setExpRate.end())
				{
					nAdjustExp = m_pUser->AdjustExp(setRole[i], nLifeLost);
					setExpRate[pMonster->GetType()] = nAdjustExp*10000 / nLifeLost;
				}
				else
					nAdjustExp = setExpRate[pMonster->GetType()] * nLifeLost / 10000;
			}
			else
				nAdjustExp = m_pUser->AdjustExp(setRole[i], nLifeLost);
			nExp += nAdjustExp;

			if (!setRole[i]->IsAlive())
			{
				int	nBonusExp	= setRole[i]->GetMaxLife()*KILLBONUS_PERCENT/100;
				OtherMemberAwardExp(setRole[i], nBonusExp);

				nExp += m_pUser->AdjustExp(setRole[i], nBonusExp, true);
			}
		}

		if(!setRole[i]->IsAlive())
			nKillCount++;
	}

	for (int i = 0; i < (int)setRole.size(); i++)
	{
		if(!setRole[i]->IsAlive())
			m_pUser->Kill(setRole[i], nKillCount > 1 ? DIE_CRITICALHIT : GetDieMode(), m_pMagic->GetInt(MAGICDATA_TYPE) == 1120);
	}

	AwardExp(nExp, nExp);
	return true;
}

bool CMagic::ProcessSquare()
{
	CHECKF(m_pMagic);

	int nDir	= m_pUser->GetDir();
	POINT	pos;
	if(m_pMagic->GetInt(MAGICDATA_GROUND))
	{
		pos.x		= m_pos.x;
		pos.y		= m_pos.y;
	}
	else
	{
		IRole* pTarget = m_pUser->FindAroundRole(m_idTarget);
		if(!pTarget || !pTarget->IsAlive())
			return false;
		if(m_pUser->GetDistance(pTarget->GetPosX(), pTarget->GetPosY()) > m_pMagic->GetInt(MAGICDATA_DISTANCE))
			return false;
		pos.x		= pTarget->GetPosX();
		pos.y		= pTarget->GetPosY();
	}

	int	nRange	= m_pMagic->GetInt(MAGICDATA_RANGE);
	if(HitByWeapon())
		nRange	= m_pUser->GetAttackRange(MAX_SIZEADD);

	MAPTHING_SET	setTarget;
	m_pUser->GetMap()->CollectMapThing(setTarget, pos, nRange, OBJ_USER|OBJ_MONSTER|OBJ_NPC);

	vector<IRole*>	setRole;
	vector<int>		setPower;
	for(int i = 0; i < (int)setTarget.size(); i++)
	{
		IRole* pRole = NULL;
		if(!setTarget[i] || !setTarget[i]->QueryRole(IPP_OF(pRole)))
			continue;
		if(!pRole->IsAlive() || IsImmunity(pRole, m_pMagic->GetInt(MAGICDATA_SORT)))
			continue;
		if(m_pUser->GetDistance(pRole->QueryMapThing()) > m_pUser->GetAttackRange(pRole->GetSizeAdd()))
			continue;

		int nPower = 0;
		if(HitByMagic())
			nPower += CMagic::CalcMagicPower(m_pUser, pRole, m_pMagic->GetInt(MAGICDATA_POWER));
		if(HitByWeapon())
			nPower += CBattleSystem::CalcAttackPower(m_pUser, pRole, true, m_pMagic->GetInt(MAGICDATA_POWER), 0, false);
		setRole.push_back(pRole);
		setPower.push_back(nPower);
	}

	CMsgMagicEffect	msg;
	IF_OK(msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), pos.x, pos.y))
	{
		for(int i = 0; i < (int)setRole.size(); )
		{
			int nDeltaBattleEffect = 0;
			CUser* pTargetUser = setRole[i]->QueryOwnerUser();
			if(pTargetUser)
				nDeltaBattleEffect = m_pUser->GetBattlePower() - pTargetUser->GetBattlePower();
			if(msg.AppendRole(setRole[i]->GetID(), setPower[i], nDeltaBattleEffect))
				i++;
			else
			{
				m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
				msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), pos.x, pos.y);
			}
		}
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}

	CheckCrime(&setRole);

	int nKillCount = 0;
	int nExp = 0;
	map<OBJID, int>	setExpRate;
	for (int i = 0; i < (int)setRole.size(); i++)
	{
		setPower[i] = setRole[i]->BeAttackEx(m_pUser, setPower[i], true);
		int nLifeLost = __min((int)setRole[i]->GetLife(), setPower[i]);
		if (nLifeLost <= 0)
			continue;

		setRole[i]->AddAttrib(_USERATTRIB_LIFE, -1*nLifeLost, SYNCHRO_TRUE, NULL);
		CNpc* pNpc = NULL;
		if(setRole[i]->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsAwardScore())
			m_pUser->AwardSynWarScore(pNpc, nLifeLost);

		if (setRole[i]->IsMonster() || pNpc && pNpc->IsGoal() && m_pUser->GetLev() >= pNpc->GetLev())
		{
			int nAdjustExp = 0;
			CMonster* pMonster = NULL;
			if(setRole[i]->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
			{
				if(setExpRate.find(pMonster->GetType()) == setExpRate.end())
				{
					nAdjustExp = m_pUser->AdjustExp(setRole[i], nLifeLost);
					setExpRate[pMonster->GetType()] = nAdjustExp*10000 / nLifeLost;
				}
				else
					nAdjustExp = setExpRate[pMonster->GetType()] * nLifeLost / 10000;
			}
			else
				nAdjustExp = m_pUser->AdjustExp(setRole[i], nLifeLost);
			nExp += nAdjustExp;

			if (!setRole[i]->IsAlive())
			{
				int	nBonusExp	= setRole[i]->GetMaxLife()*KILLBONUS_PERCENT/100;
				OtherMemberAwardExp(setRole[i], nBonusExp);

				nExp += m_pUser->AdjustExp(setRole[i], nBonusExp, true);
			}
		}

		if(!setRole[i]->IsAlive())
			nKillCount++;
	}

	for (int i = 0; i < (int)setRole.size(); i++)
	{
		if(!setRole[i]->IsAlive())
			m_pUser->Kill(setRole[i], nKillCount > 1 ? DIE_CRITICALHIT : GetDieMode(), 0);
	}

	AwardExp(nExp, nExp);
	return true;
}

bool CMagic::ProcessAttach()
{
	CHECKF(m_pMagic);

	IRole* pRole = m_pUser->FindAroundRole(m_idTarget);
	if(!pRole || !pRole->IsAlive())
		return false;
	if(m_pUser->GetDistance(pRole->GetPosX(), pRole->GetPosY()) > m_pMagic->GetInt(MAGICDATA_DISTANCE))
		return false;

	int nPower	= m_pMagic->GetInt(MAGICDATA_POWER);
	int nSecs	= m_pMagic->GetInt(MAGICDATA_STEP);
	int	nTimes	= m_pMagic->GetInt(MAGICDATA_TIMES);
	DWORD nStatus	= m_pMagic->GetInt(MAGICDATA_STATUS);
	MYASSERT(nPower > 0);

	CMsgMagicEffect	msg;
	IF_OK(msg.Create(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_idTarget, 0))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	CheckCrime(pRole);

	CUser* pUser = NULL;
	if(!pRole->QueryObj(OBJ_USER, IPP_OF(pUser))
				&& (nStatus == STATUS_LURKER || nStatus == STATUS_ATKRANGE || nStatus == STATUS_REFLECT || nStatus == STATUS_REFLECTMAGIC))
		return false;

	if(nStatus == STATUS_LURKER || nStatus == STATUS_ATKSPEED)
	{
		CRole::DetachWellStatus(pRole);
	}

	if(nStatus == STATUS_SUPERMAN || nStatus == STATUS_TORNADO)
	{
		nSecs	+= m_pUser->GetSupermanAddSecs();
	}

	if(nStatus == STATUS_LUCKY_PRAY && pUser)
		pUser->AddAttrib(_USERATTRIB_UNK29, 0, SYNCHRO_TRUE, NULL);

	CRole::AttachStatus(pRole, nStatus, nPower, nSecs, nTimes);

	AwardExp(AWARDEXP_BY_TIMES, 0);

	MSGBUF	szMsg;
	switch(nStatus)
	{
	case	STATUS_HITRATE:
		sprintf(szMsg, STR_HITRATE_ADD, nSecs, nPower-30000);
		break;
	case	STATUS_DEFENCE:
		{
			int nPowerTimes = (nPower-30000) / 100;
			if(nPowerTimes > 1)
				sprintf(szMsg, STR_DEFENCE_ADD, nSecs, nPowerTimes);
			else
				sprintf(szMsg, STR_DEFENCE_ADD2, nSecs, nPower-30100);
		}
		break;
	case	STATUS_ATTACK:
		sprintf(szMsg, STR_ATTACK_ADD, nSecs, nPower-30000);
		break;
	case	STATUS_ADJUST_DODGE:
		sprintf(szMsg, STR_DODGE_ADD, nSecs, nPower-30000);
		break;
	default:
		return true;
	}
	pRole->SendSysMsg(szMsg);

	return true;
}

bool CMagic::ProcessDetach()
{
	CHECKF(m_pMagic);

	IRole* pRole = m_pUser->FindAroundRole(m_idTarget);
	if(!pRole)
		return false;

	int nPower	= m_pMagic->GetInt(MAGICDATA_POWER);
	int nSecs	= m_pMagic->GetInt(MAGICDATA_STEP);
	int	nTimes	= m_pMagic->GetInt(MAGICDATA_TIMES);
	int nStatus	= m_pMagic->GetInt(MAGICDATA_STATUS);
	MYASSERT(nPower == 0);

	if (!pRole->IsAlive())
	{
		if (STATUS_DIE != nStatus)
			return false;
		void* pTemp;
		if (!pRole->QueryObj(OBJ_USER, IPP_OF(pTemp)))
			return false;
	}

	if (STATUS_DIE == nStatus)
	{
		CGameMap* pMap = m_pUser->GetMap();
		if (pMap && pMap->IsPkField())
			return false;
	}

	if(nStatus == STATUS_DETACH_ALL)
	{
		CRole::DetachAllStatus(pRole);
	}
	else if(nStatus == STATUS_DETACH_BADLY)
	{
		CRole::DetachBadlyStatus(pRole);
	}
	else
	{
		IStatus* pStatus = pRole->QueryStatus(nStatus);
		if(pStatus)
			pStatus->ChangeData(nPower, nSecs, nTimes);
	}
	CMsgMagicEffect	msg;
	IF_OK(msg.Create(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_idTarget, nStatus))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	CheckCrime(pRole);

	pRole->ClsStatus(nStatus, SYNCHRO_TRUE);

	if (STATUS_DIE == nStatus)
	{
		CUser* pUser = NULL;
		pRole->QueryObj(OBJ_USER, IPP_OF(pUser));
		if (pUser)
		{
			pUser->Reborn(false, false);
		}
	}

	return true;
}

bool CMagic::ProcessJumpAttackFaild(int x, int y)
{
	CHECKF(m_pMagic);

	int		nPower		= 0;
	OBJID	idTarget	= ID_NONE;

	CMsgMagicEffect	msg;
	int nDeltaBattleEffect = 0;
	if(m_pUser)
	{
		IRole* pRole = RoleManager()->QueryRole(idTarget);
		CUser* pTargetUser = pRole ? pRole->QueryOwnerUser() : NULL;
		if(pTargetUser)
			nDeltaBattleEffect = m_pUser->GetBattlePower() - pTargetUser->GetBattlePower();
	}
	IF_OK(msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), x, y, idTarget, nPower, nDeltaBattleEffect))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	m_pUser->ProcessOnMove(MOVEMODE_JUMPMAGICATTCK);
	m_pUser->JumpPos(x, y);

	return true;
}

bool CMagic::ProcessJumpAtk()
{
	CHECKF(m_pMagic);

	if(m_pUser->GetDistance(m_pos.x, m_pos.y) > m_pMagic->GetInt(MAGICDATA_DISTANCE))
	{
		::GmLogSave("Player [%s]: illegal XP jump-attack distance!", m_pUser->GetName());
		return false;
	}

	POINT	pos;
	IRole* pTarget	= NULL;
	if(m_pMagic->GetInt(MAGICDATA_GROUND))
	{
		pos.x		= m_pos.x;
		pos.y		= m_pos.y;
	}
	else
	{
		pTarget = m_pUser->FindAroundRole(m_idTarget);
		if(!pTarget || !pTarget->IsAlive())
			return false;

		if(pTarget->GetDistance(m_pos.x, m_pos.y) > m_pUser->GetAttackRange(pTarget->GetSizeAdd()))
			return false;

		if (this->IsImmunity(pTarget, 0))
			return false;

		pos.x		= pTarget->GetPosX();
		pos.y		= pTarget->GetPosY();
	}

	if(m_pUser->GetDistance(pos.x, pos.y) > m_pMagic->GetInt(MAGICDATA_DISTANCE))
		return false;

	int	nRange	= m_pMagic->GetInt(MAGICDATA_RANGE);

	int	nPower	= 0;
	if(pTarget)
	{
		if(HitByMagic())
			nPower = CMagic::CalcMagicPower(m_pUser, pTarget, m_pMagic->GetInt(MAGICDATA_POWER));
		else if(HitByWeapon())
			nPower = CBattleSystem::CalcAttackPower(m_pUser, pTarget, true, m_pMagic->GetInt(MAGICDATA_POWER), 0, false);
	}

	CMsgMagicEffect	msg;
	int nDeltaBattleEffect = 0;
	if(m_pUser)
	{
		IRole* pRole = RoleManager()->QueryRole(m_idTarget);
		CUser* pTargetUser = pRole ? pRole->QueryOwnerUser() : NULL;
		if(pTargetUser)
			nDeltaBattleEffect = m_pUser->GetBattlePower() - pTargetUser->GetBattlePower();
	}
	IF_OK(msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_pos.x, m_pos.y, m_idTarget, nPower, nDeltaBattleEffect))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	if(pTarget)
		CheckCrime(pTarget);

	m_pUser->ProcessOnMove(MOVEMODE_JUMPMAGICATTCK);
	m_pUser->JumpPos(m_pos.x, m_pos.y);

	if (nPower && pTarget)
	{
		nPower = pTarget->BeAttackEx(m_pUser, nPower, true);
		int nLifeLost = __min((int)pTarget->GetLife(), nPower);
		if (nLifeLost <= 0)
			return true;

		pTarget->AddAttrib(_USERATTRIB_LIFE, -1*nLifeLost, SYNCHRO_TRUE, NULL);

		CNpc* pNpc = NULL;
		if(pTarget->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsAwardScore())
			m_pUser->AwardSynWarScore(pNpc, nLifeLost);

		if(pTarget->IsMonster() || pNpc && pNpc->IsGoal() && m_pUser->GetLev() >= pNpc->GetLev())
		{
			int nExp = this->m_pUser->AdjustExp(pTarget, nLifeLost);

			if (!pTarget->IsAlive())
			{
				int	nBonusExp	= pTarget->GetMaxLife()*KILLBONUS_PERCENT/100;
				OtherMemberAwardExp(pTarget, nBonusExp);

				nExp += m_pUser->AdjustExp(pTarget, nBonusExp, true);
			}

			this->AwardExp(nExp, nExp);
		}

		if (!pTarget->IsAlive())
			m_pUser->Kill(pTarget, GetDieMode(), 0);
	}

	return true;
}

bool CMagic::ProcessRandomTrans()
{
	CHECKF(m_pMagic);

	for(int i = 0; i < RANDOMTRANS_TRY_TIMES; i++)
	{
		int nWidth	= m_pUser->GetMap()->GetWidth();
		int nHeight	= m_pUser->GetMap()->GetHeight();
		int x = (int)m_pUser->GetPosX()+nWidth/2-::RandGet(nWidth);
		int y = (int)m_pUser->GetPosX()+nHeight/2-::RandGet(nHeight);
		if(m_pUser->GetMap()->IsStandEnable(x, y))
		{
			CMsgMagicEffect	msg;
			IF_OK(msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), x, y))
				m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

			m_pUser->ProcessOnMove(MOVEMODE_TRANS);
			m_pUser->TransPos(x, y);
			m_pUser->Synchro();

			return true;
		}
	}

	m_pUser->SendSysMsg(STR_MAGIC_FAILED);
	return false;
}

bool CMagic::ProcessDispatchXP()
{
	CHECKF(m_pMagic);

	vector<CUser*>	setUser;
	vector<int>		setPower;
	CTeam* pTeam = m_pUser->GetTeam();
	if(pTeam)
	{
		MSGBUF	szMsg;
		sprintf(szMsg, STR_DISPATCHXP_s, m_pUser->GetName());
		for(int i = 0; i < pTeam->GetMemberAmount(); i++)
		{
			OBJID idUser = pTeam->GetMemberByIndex(i);
			CUser* pUser = m_pUser->FindAroundUser(idUser);
			if(idUser != ID_NONE && pUser && pUser->IsAlive() && pUser->GetID() != m_pUser->GetID())
			{
				pUser->AddXp(DISPATCHXP_NUMBER);
				setUser.push_back(pUser);
				setPower.push_back(DISPATCHXP_NUMBER);
				pUser->SendSysMsg(szMsg);
			}
		}
	}

	CMsgMagicEffect	msg;
	IF_OK(msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_pos.x, m_pos.y))
	{
		for(int i = 0; i < (int)setUser.size(); )
		{
			int nDeltaBattleEffect = 0;
			CUser* pTargetUser = setUser[i];
			if(pTargetUser)
				nDeltaBattleEffect = m_pUser->GetBattlePower() - pTargetUser->GetBattlePower();
			if(msg.AppendRole(setUser[i]->GetID(), setPower[i], nDeltaBattleEffect))
				i++;
			else
			{
				m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
				msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_pos.x, m_pos.y);
			}
		}
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}

	return true;
}

bool CMagic::ProcessCollideFail(int x, int y, int nDir)
{
	int nTargetX	= m_pos.x+_DELTA_X[nDir];
	int nTargetY	= m_pos.y+_DELTA_Y[nDir];
	int	nPower		= 0;
	OBJID	idTarget	= ID_NONE;

	if(!m_pUser->GetMap()->IsStandEnable(nTargetX, nTargetY))
	{
		LOGERROR("CMagic::ProcessCollideFail() map%u(%d, %d) at %s, %d", m_pUser->GetMapID(), nTargetX, nTargetY, __FILE__, __LINE__);
		m_pUser->SendSysMsg(STR_INVALID_MSG);
		UserManager()->KickOutSocket(m_pUser->GetSocketID(), "COLLIDE coord!");
		return false;
	}

	int nDeltaBattleEffect = 0;
	IRole* pRole = RoleManager()->QueryRole(idTarget);
	CUser* pTargetUser = pRole ? pRole->QueryOwnerUser() : NULL;
	if(pTargetUser)
		nDeltaBattleEffect = m_pUser->GetBattlePower() - pTargetUser->GetBattlePower();
	CMsgInteract msg;
	IF_OK (msg.Create(INTERACT_BUMP, m_pUser->GetID(), idTarget, nTargetX, nTargetY, nDir*0x01000000+nPower, nDeltaBattleEffect))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	m_pUser->ProcessOnMove(MOVEMODE_COLLIDE);
	m_pUser->MoveToward(nDir);
	return true;
}

bool CMagic::ProcessCollide()
{
	CHECKF(m_pMagic);
	CHECKF(m_pUser);

	IF_NOT(m_pUser->SynPosition(m_pos.x, m_pos.y))
	{
		UserManager()->KickOutSocket(m_pUser->GetSocketID(), "ProcessCollide SynPosition");
		return false;
	}

	int nDir		= m_nData;
	int nTargetX	= m_pos.x+_DELTA_X[nDir];
	int nTargetY	= m_pos.y+_DELTA_Y[nDir];

	if(!m_pUser->GetMap()->IsStandEnable(nTargetX, nTargetY))
	{
		LOGERROR("CMagic::ProcessCollide() map%u(%d, %d) at %s, %d", m_pUser->GetMapID(), nTargetX, nTargetY, __FILE__, __LINE__);
		m_pUser->SendSysMsg(STR_INVALID_MSG);
		UserManager()->KickOutSocket(m_pUser->GetSocketID(), "COLLIDE coord!");
		return false;
	}

	bool bSuc = true;
	IRole* pTarget = m_pUser->GetMap()->QueryRole(nTargetX, nTargetY, ID_NONE);
	if(!pTarget || !pTarget->IsAlive())
		bSuc = false;

	if (!pTarget || this->IsImmunity(pTarget, 0))
		bSuc = false;

	int	nPower	= 0;
	bool bBackEnable = false;
	OBJID idTarget = ID_NONE;

	CNpc* pNpc = NULL;
	if (pTarget)
	{
		pTarget->QueryObj(OBJ_NPC, IPP_OF(pNpc));

		int nHitRate = 100;
		if (::RandGet(100) >= nHitRate)
			bSuc = false;

		if (bSuc)
		{
			bBackEnable	= pTarget->GetMap()->IsStandEnable(nTargetX+_DELTA_X[nDir], nTargetY+_DELTA_Y[nDir]);
			if(pNpc)
				bBackEnable = false;

			if (!bBackEnable)
				nDir = MAX_DIRSIZE;

			if (HitByWeapon())
			{
				if(bBackEnable || pNpc)
					nPower	= MulDiv(CBattleSystem::CalcAttackPower(m_pUser, pTarget, false, 0, 0, false), COLLIDE_POWER_PERCENT, 100);
				else
					nPower	= CBattleSystem::CalcAttackPower(m_pUser, pTarget, true, m_pMagic->GetInt(MAGICDATA_POWER), 0, false);
			}

			if(!m_pUser->GetMap()->IsTrainMap())
			{
				CItem* pWeaponL = m_pUser->GetWeaponL();
				if(pWeaponL && !pWeaponL->QueryType()->GetInt(ITEMTYPEDATA_FRAY_MODE))
					m_pUser->AddEquipmentDurability(ITEMPOSITION_WEAPONL, -1*COLLIDE_SHIELD_DURABILITY);
			}
		}

		idTarget = pTarget->GetID();
	}

	int nDeltaBattleEffect = 0;
	IRole* pRole = RoleManager()->QueryRole(idTarget);
	CUser* pOwnerUser = pRole ? pRole->QueryOwnerUser() : NULL;
	if(pOwnerUser)
		nDeltaBattleEffect = m_pUser->GetBattlePower() - pOwnerUser->GetBattlePower();

	CMsgInteract msg;
	DWORD dwData = nDir*0x01000000+nPower;
	if (!bBackEnable)
		dwData += 1*0x10000000;
	IF_OK (msg.Create(INTERACT_BUMP, m_pUser->GetID(), idTarget, nTargetX, nTargetY, dwData, nDeltaBattleEffect))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	if (m_pUser->IsPM())
	{
		m_pUser->SendSysMsg(_TXTATR_NORMAL, "bump move:(%u, %u)->(%u, %u)",
							m_pUser->GetPosX(), m_pUser->GetPosY(),
							nTargetX, nTargetY);
	}

	if (bBackEnable)
	{
		m_pUser->ProcessOnMove(MOVEMODE_COLLIDE);
		m_pUser->MoveToward(nDir);
	}

	if (pTarget && bBackEnable && bSuc)
	{
		pTarget->ProcessOnMove(MOVEMODE_COLLIDE);
		pTarget->MoveToward(nDir);

		CUser* pTargetUser;
		if(pTarget->QueryObj(OBJ_USER, IPP_OF(pTargetUser)))
		{
			pTargetUser->SetSynchro(false);
		}
	}

	if(pTarget)
		CheckCrime(pTarget);

	if (nPower && pTarget)
	{
		nPower = pTarget->BeAttackEx(m_pUser, nPower, true);
		int nLifeLost = __min((int)pTarget->GetLife(), nPower);
		if (nLifeLost > 0)
		{
			pTarget->AddAttrib(_USERATTRIB_LIFE, -1*nLifeLost, SYNCHRO_TRUE, NULL);

			CNpc* pNpc = NULL;
			if(pTarget->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsAwardScore())
				m_pUser->AwardSynWarScore(pNpc, nLifeLost);

			if(pTarget->IsMonster() || pNpc && pNpc->IsGoal() && m_pUser->GetLev() >= pNpc->GetLev())
			{
				int nExp = this->m_pUser->AdjustExp(pTarget, nLifeLost);

				if (!pTarget->IsAlive())
				{
					int	nBonusExp	= pTarget->GetMaxLife()*KILLBONUS_PERCENT/100;
					OtherMemberAwardExp(pTarget, nBonusExp);

					nExp += m_pUser->AdjustExp(pTarget, nBonusExp, true);
				}

				this->AwardExp(nExp, nExp);
			}

			if (!pTarget->IsAlive())
				m_pUser->Kill(pTarget, GetDieMode(), 0);
		}
	}

	return true;
}

bool CMagic::ProcessSerialCut()
{
	CHECKF(m_pMagic);

	POINT pos;
	pos.x	= m_pos.x;
	pos.y	= m_pos.y;

	int	nRange	= 1;
	int	nSize		= nRange*2 + 1;
	int	nBufSize	= nSize * nSize;
	vector<IRole*>	setRole;
	vector<int>		setPower;

	for(int i = 0; i < MAX_SERIALCUTSIZE; i++)
	{
		MAPTHING_SET	setTarget;
		m_pUser->GetMap()->CollectMapThing(setTarget, pos, nRange, OBJ_USER|OBJ_MONSTER|OBJ_NPC);
		IRole* pTarget = NULL;
		for(int j = 0; j < (int)setTarget.size(); j++)
		{
			int idx = j;
			MYASSERT(0 <= idx && idx < (int)setTarget.size());
			if(setTarget[idx])
			{
				IRole* pRole = NULL;
				if(setTarget[idx]->QueryRole(IPP_OF(pRole)) && pRole->IsAlive() && !IsImmunity(pRole, 0))
				{
					int	nPower = 0;
					if(HitByWeapon())
						nPower += CBattleSystem::CalcAttackPower(m_pUser, pRole, true, m_pMagic->GetInt(MAGICDATA_POWER), 0, false);

					{
						setRole.push_back(pRole);
						setPower.push_back(nPower);

						if(nPower >= pRole->GetLife())
							pTarget = pRole;
						break;
					}
				}
			}
		}
		if(!pTarget)
			break;
		pos.x = pTarget->GetPosX();
		pos.y = pTarget->GetPosY();
	}

	CMsgMagicEffect	msg;
	IF_OK(msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), pos.x, pos.y))
	{
		for(int i = 0; i < (int)setRole.size(); )
		{
			int nDeltaBattleEffect = 0;
			CUser* pTargetUser = setRole[i]->QueryOwnerUser();
			if(pTargetUser)
				nDeltaBattleEffect = m_pUser->GetBattlePower() - pTargetUser->GetBattlePower();
			if(msg.AppendRole(setRole[i]->GetID(), setPower[i], nDeltaBattleEffect))
				i++;
			else
			{
				m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
				msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), pos.x, pos.y);
			}
		}
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}

	int	nExp = 0;
	map<OBJID, int>	setExpRate;
	for (int i = 0; i < (int)setRole.size(); i++)
	{
		setPower[i] = setRole[i]->BeAttackEx(m_pUser, setPower[i], true);
		int nLifeLost = __min((int)setRole[i]->GetLife(), setPower[i]);
		if (nLifeLost <= 0)
			continue;

		setRole[i]->AddAttrib(_USERATTRIB_LIFE, -1*nLifeLost, SYNCHRO_TRUE, NULL);
		CNpc* pNpc = NULL;
		if(setRole[i]->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsAwardScore())
			m_pUser->AwardSynWarScore(pNpc, nLifeLost);

		if (setRole[i]->IsMonster() || pNpc && pNpc->IsGoal() && m_pUser->GetLev() >= pNpc->GetLev())
		{
			int nAdjustExp = 0;
			CMonster* pMonster = NULL;
			if(setRole[i]->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
			{
				if(setExpRate.find(pMonster->GetType()) == setExpRate.end())
				{
					nAdjustExp = m_pUser->AdjustExp(setRole[i], nLifeLost);
					setExpRate[pMonster->GetType()] = nAdjustExp*10000 / nLifeLost;
				}
				else
					nAdjustExp = setExpRate[pMonster->GetType()] * nLifeLost / 10000;
			}
			else
				nAdjustExp = m_pUser->AdjustExp(setRole[i], nLifeLost);
			nExp += nAdjustExp;

			if (!setRole[i]->IsAlive())
			{
				int	nBonusExp	= setRole[i]->GetMaxLife()*KILLBONUS_PERCENT/100;
				OtherMemberAwardExp(setRole[i], nBonusExp);

				nExp += m_pUser->AdjustExp(setRole[i], nBonusExp, true);
			}
		}

		if(!setRole[i]->IsAlive())
			m_pUser->Kill(setRole[i], GetDieMode(), 0);
	}

	AwardExp(nExp, nExp);

	return true;
}

bool CMagic::ProcessCallTeamMember()
{
	CHECKF(m_pMagic);

	CTeam* pTeam = m_pUser->GetTeam();
	if(!pTeam)
		return false;

	CMsgMagicEffect	msg;
	IF_OK(msg.Create(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_pUser->GetID(), 0))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	for(int i = 0; i < pTeam->GetMemberAmount(); i++)
	{
		OBJID idTarget = pTeam->GetMemberByIndex(i);
		if(idTarget != ID_NONE && idTarget != m_pUser->GetID())
		{
			CUser* pTarget = UserManager()->GetUser(idTarget);
			if(pTarget)
			{
				if (!pTarget->IsAlive())
					return false;

				pTarget->FlyMap(m_pUser->GetMapID(), m_pUser->GetPosX(), m_pUser->GetPosY());
			}
			else
				MapGroup(PID)->QueryIntraMsg()->ChangeTeam(CHANGETEAM_FLY, m_pUser->GetMapID(), idTarget, MAKELONG(m_pUser->GetPosX(), m_pUser->GetPosY()));
		}
	}
	return true;
}

bool CMagic::ProcessRecordTransSpell()
{
	CHECKF(m_pMagic);

	CMsgMagicEffect	msg;
	IF_OK(msg.Create(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_idTarget, 0))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	CItem* pItem = m_pUser->FindSpaceTransSpell();
	if(pItem)
	{
		pItem->SetInt(ITEMDATA_DATA, m_pUser->GetMapID(), UPDATE_FALSE);
		pItem->SetInt(ITEMDATA_AMOUNT, m_pUser->GetPosX(), UPDATE_FALSE);
		pItem->SetInt(ITEMDATA_AMOUNTLIMIT, m_pUser->GetPosY(), UPDATE_TRUE);

		CMsgItemInfo	msg;
		msg.Create(pItem, ITEMINFO_UPDATE);
		m_pUser->SendMsg(&msg);
	}
	return true;
}

bool CMagic::ProcessTransform()
{
	CHECKF(m_pMagic);

	CMsgMagicEffect	msg;
	IF_OK(msg.Create(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_idTarget, 0))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	m_pUser->Transform(m_pMagic->GetInt(MAGICDATA_POWER), m_pMagic->GetInt(MAGICDATA_STEP), true);

	this->AwardExp(AWARDEXP_BY_TIMES, 0);

	return true;
}

bool CMagic::ProcessAddMana()
{
	CHECKF(m_pMagic);

	int	nAddMana = m_pMagic->GetInt(MAGICDATA_POWER);
	nAddMana = ::CutOverflow(nAddMana, (int)m_pUser->GetMaxMana()-(int)m_pUser->GetMana());

	CMsgMagicEffect	msg;
	IF_OK(msg.Create(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_pUser->GetID(), nAddMana))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	int nExp = m_pUser->GetMaxMana() - m_pUser->GetMana();
	m_pUser->AddAttrib(_USERATTRIB_MANA, nAddMana, SYNCHRO_TRUE, NULL);

	if(nExp > 0)
		this->AwardExp(nExp, 0);

	return true;
}

bool CMagic::ProcessLayTrap()
{
	CHECKF(m_pMagic);

	int	nTrapType = m_pMagic->GetInt(MAGICDATA_POWER);

	CMsgMagicEffect	msg;
	IF_OK(msg.Create(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_pUser->GetID(), 0))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	int nCostDir	= abs(m_pUser->GetPosX() - m_pos.x) < abs(m_pUser->GetPosY() - m_pos.y) ? 0 : 2;

	TRAP_INFO	info;
	info.id				= ID_NONE;
	info.idMap			= m_pUser->GetMapID();
	info.idOwner		= m_pUser->GetID();
	info.usLook			= (nCostDir + 4) % MAX_DIRSIZE;
	info.usPosX			= m_pos.x;
	info.usPosY			= m_pos.y;
	info.usType			= nTrapType;
	CMapTrap* pTrap	= CMapTrap::CreateNew(m_idProcess, &info, m_pUser);
	if(pTrap)
		RoleManager()->QuerySet()->AddObj(pTrap->QueryRole());
	this->AwardExp(AWARDEXP_BY_TIMES, 0);

	return true;
}

bool CMagic::ProcessCallPet()
{
	CHECKF(m_pMagic);

	CMsgMagicEffect	msg;
	IF_OK(msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_pos.x, m_pos.y))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	m_pUser->CallPet(m_pMagic->GetInt(MAGICDATA_POWER), m_pos.x, m_pos.y, m_pMagic->GetInt(MAGICDATA_STEP));

	this->AwardExp(AWARDEXP_BY_TIMES, 0);

	return true;
}

bool CMagic::ProcessDecLife()
{
	IRole* pTarget = m_pUser->FindAroundRole(m_idTarget);
	if(!pTarget || !pTarget->IsAlive())
		return false;

	if(m_pUser->GetDistance(pTarget->GetPosX(), pTarget->GetPosY()) > m_pMagic->GetInt(MAGICDATA_DISTANCE))
		return false;

	if (this->IsImmunity(pTarget, 0))
		return false;

	int	nPower	= ::CutTrail(0, CUser::AdjustData(pTarget->GetLife(), m_pMagic->GetInt(MAGICDATA_POWER)));
	nPower = ::CutOverflow(nPower, (int)m_pUser->GetLev()*30);

	CMsgMagicEffect	msg;
	int nDeltaBattleEffect = 0;
	if(m_pUser)
	{
		IRole* pRole = RoleManager()->QueryRole(m_idTarget);
		CUser* pTargetUser = pRole ? pRole->QueryOwnerUser() : NULL;
		if(pTargetUser)
			nDeltaBattleEffect = m_pUser->GetBattlePower() - pTargetUser->GetBattlePower();
	}
	IF_OK(msg.Create(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_idTarget, nPower, nDeltaBattleEffect))
		m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	CheckCrime(pTarget);

	if (nPower)
	{
		nPower = pTarget->BeAttackEx(m_pUser, nPower, true);
		int nLifeLost = __min((int)pTarget->GetLife(), nPower);
		if (nLifeLost > 0)
		{
			pTarget->AddAttrib(_USERATTRIB_LIFE, -1*nLifeLost, SYNCHRO_TRUE, NULL);

			CUser* pTargetUser = NULL;
			if(pTarget->QueryObj(OBJ_USER, IPP_OF(pTargetUser)))
				pTargetUser->DecEquipmentDurability(true, true);

			AwardExpOfLife(pTarget, nLifeLost);

			if (!pTarget->IsAlive())
				m_pUser->Kill(pTarget, GetDieMode(), 0);
		}
	}

	return true;
}
