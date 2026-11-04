#include "BattleEffect.h"
#include "AllMsg.h"
#include "GameObj.h"
#include "Magic.h"
#include "MapGroup.h"
#include "StatusOnce.h"
#include "StatusMore.h"
#include "RoleManager.h"
#include "AiNpc.h"

char			szMagicTable[] = _TBL_MAGIC;
const OBJID		MIN_TEMPORARY_MAGICID	= (DWORD)40*10000*10000 + 1;
long			CMagic::m_lockNextID	= (long)MIN_TEMPORARY_MAGICID;
unsigned char	Prof2MaskTable[] = {
	100,
	101,
	12,
	13,
	14,
	15,
	22,
	23,
	24,
	25,
	132,
	133,
	134,
	135,
	142,
	143,
	144,
	145,
	42,
	43,
	44,
	45,
};

const int	MAX_USER_PROFS = sizeof(Prof2MaskTable)/sizeof(Prof2MaskTable[0]);

const DWORD MASK_ALLPROF	= 0;
DWORD	Prof2Mask(int nProf)
{
	for(int i = 0; i < MAX_USER_PROFS; i++)
		if(Prof2MaskTable[i] == nProf)
			return 1 << i;
	return 0;
}

MYHEAP_IMPLEMENTATION(CMagicData,s_heap)
bool CMagicData::Create(IRecordset* pDefaultRecord, OBJID idNewKey)
{
	return CMagicDataBase::Create(pDefaultRecord, idNewKey);
}

bool CMagicData::Create(IRecordset* pRecordset)
{
	return CMagicDataBase::Create(pRecordset);
}

int CMagicData::GetInt(MAGICDATA nDataName)
{

	if(m_pType == NULL)
	{
		IF_NOT(FindType())
		{
			LOGERROR("Magic type not found. type[%d],level[%d]", CMagicDataBase::GetInt(MAGICDATA_TYPE), CMagicDataBase::GetInt(MAGICDATA_LEVEL));
			return false;
		}
	}

	if(nDataName < MAGICTYPEDATA_OFFSET)
		return CMagicDataBase::GetInt(nDataName);

	return m_pType->GetInt((MAGICTYPEDATA)(nDataName - MAGICTYPEDATA_OFFSET));
}

void CMagicData::SetInt(MAGICDATA idx, int nData)
{
	MYASSERT(idx < MAGICTYPEDATA_OFFSET);

	CMagicDataBase::SetInt(idx, nData);

	if(idx == MAGICDATA_LEVEL)
	{
		CHECK(FindType());
	}
}

bool CMagicData::FindType()
{
	m_pType = CMagic::FindMagicType(CMagicDataBase::GetInt(MAGICDATA_TYPE), CMagicDataBase::GetInt(MAGICDATA_LEVEL));
	return (m_pType != NULL);
}
MYHEAP_IMPLEMENTATION(CMagic,s_heap)
CMagic::CMagic(PROCESS_ID idProcess, CUser* pUser)
	: m_bInMagicLoop()
{
	MYASSERT (pUser);

	m_idProcess = idProcess;
	m_pUser		= pUser;
	m_setData	= CMagicSet::CreateNew(true);
	m_pMagic	= NULL;
	m_nRawDelay = 800;
	m_tDelay.SetInterval(m_nRawDelay);
	m_nDelay	= m_nRawDelay;
}

CMagic::~CMagic()
{
	if(m_setData)
		m_setData->Release();
}

int CMagic::GetDieMode()
{
	return HitByMagic()?DIE_MAGIC:(m_pUser->IsBowman()?DIE_ARROWSHOOT:DIE_NORMAL);
}

CMagic* CMagic::CreateNew(PROCESS_ID idProcess, CUser* pUser)
{
	CMagic* pMagic = new CMagic(idProcess, pUser);
	CHECKF(pMagic);

	pMagic->m_idProcess = idProcess;
	pMagic->m_pUser		= pUser;

	return pMagic;
}

bool CMagic::CreateAll()
{
	CHECKF(CMapGroup::IsValidMapGroupProcessID(m_idProcess));
	CHECKF(m_setData);
	CHECKF(m_pUser);

	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT * FROM cq_magic WHERE ownerid=%u && unlearn=0", m_pUser->GetID());
	IF_NOT(m_setData->Create(szSQL, Database()))
		return false;

	return true;
}

bool CMagic::SendInfoToClient()
{
	for(int i = m_setData->GetAmount()-1; i >= 0; i--)
	{
		IMagicData* pData = m_setData->GetObjByIndex(i);
		if(pData && pData->GetInt(MAGICDATA_TYPE))
		{
			if(!IsWeaponMagic(pData->GetInt(MAGICDATA_TYPE)))
			{
				if(CMagic::FindMagicType(pData->GetInt(MAGICDATA_TYPE), pData->GetInt(MAGICDATA_LEVEL)))
				{
					CMsgMagicInfo msg;
					IF_OK(msg.Create(pData->GetInt(MAGICDATA_TYPE), pData->GetInt(MAGICDATA_LEVEL), pData->GetInt(MAGICDATA_EXP)))
						m_pUser->SendMsg(&msg);
				}
				else
				{
					LOGWARNING("Player learned an illegal magic: [%s][%d].", m_pUser->GetName(), m_pUser->GetID());
					m_setData->DelObj(pData->GetID());
				}
			}
		}
		else
		{
			LOGWARNING("Player learned an illegal magic: [%s][%d]", m_pUser->GetName(), m_pUser->GetID());
		}
	}

	return true;
}

bool CMagic::SaveInfo()
{
	for(int i = 0; i < m_setData->GetAmount(); i++)
	{
		IMagicData* pData = m_setData->GetObjByIndex(i);
		IF_OK(pData)
		{
			if(!IsWeaponMagic(pData->GetInt(MAGICDATA_TYPE)))
				pData->Update();
		}
	}
	return true;
}

bool CMagic::GetInfoByIndex(MagicInfoStruct* pInfo, int idx)
{
	CHECKF(pInfo);
	CHECKF(idx >= 0 && idx < m_setData->GetAmount());

	IMagicData* pData = m_setData->GetObjByIndex(idx);
	IF_OK(pData)
	{
		pInfo->id		= pData->GetID();
		pInfo->idOwner	= m_pUser->GetID();
		pInfo->nType	= pData->GetInt(MAGICDATA_TYPE);
		pInfo->nLevel	= pData->GetInt(MAGICDATA_LEVEL);
		pInfo->dwExp	= pData->GetInt(MAGICDATA_EXP);
		return true;
	}
	return false;
}

bool CMagic::AppendMagic(const MagicInfoStruct* pInfo)
{
	IMagicData* pData = CMagicData::CreateNew();
	CHECKF(pData);
	IF_NOT(pData->Create(GameDataDefault()->GetMagicData(), pInfo->id))
		return false;

	pData->SetInt(MAGICDATA_OWNERID, pInfo->idOwner);
	pData->SetInt(MAGICDATA_TYPE, pInfo->nType);
	pData->SetInt(MAGICDATA_LEVEL, pInfo->nLevel);
	pData->SetInt(MAGICDATA_EXP, pInfo->dwExp);
	pData->ClearUpdateFlags();
	pData->FindType();

	m_setData->AddObj(pData);
	return true;
}

bool CMagic::IsImmunity(IRole* pRole, int nSort)
{
	CHECKF(m_pMagic);
	CHECKF(pRole);

	if(!m_pUser->IsAtkable(pRole, false))
		return true;

	if(pRole->IsWing() && !m_pUser->IsWing() && m_pMagic->GetInt(MAGICDATA_WEAPON_HIT) && m_pMagic->GetInt(MAGICDATA_WEAPON_SUBTYPE) != BOW_SUBTYPE)
		return true;

	if (m_pUser->GetPk() >= 1000)
	{
		if (!m_pUser->GetMap()->IsPkField() && !m_pUser->GetMap()->IsSynMap())
		{
			CUser* pOwner = pRole->QueryOwnerUser();
			if (pOwner && pOwner->GetPk() < 30 && !pOwner->IsEvil())
			{
				m_pUser->SendSysMsg(STR_PK);
				return true;
			}
		}
	}

	switch(nSort)
	{
	case MAGICSORT_CROSS:
	case MAGICSORT_FAN:
	case MAGICSORT_BOMB:
	case MAGICSORT_SQUARE:
	case MAGICSORT_LINE:
		{
			CMonster* pMonster = NULL;
			if (pRole->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)) && pMonster->IsGuard())
				return true;
		}
		break;
	}

	return m_pUser->IsImmunity(pRole);
}

IMagicData* CMagic::FindMagic(int nType)
{
	for(int i = 0; i < m_setData->GetAmount(); i++)
	{
		IMagicData* pData = m_setData->GetObjByIndex(i);
		if(pData && pData->GetInt(MAGICDATA_TYPE) == nType)
			return pData;
	}
	return NULL;
}

CMagicTypeData* CMagic::FindMagicType(int nType, int nLevel)
{
	for(int i = 0; i < MagicTypeSet()->GetAmount(); i++)
	{
		CMagicTypeData* pType = MagicTypeSet()->GetObjByIndex(i);
		if(pType && pType->GetInt(MAGICTYPEDATA_TYPE) == nType && pType->GetInt(MAGICTYPEDATA_LEVEL) == nLevel)
			return pType;
	}
	return NULL;
}

bool CMagic::AwardExpOfLife(IRole* pTarget, int nLifeLost, BOOL bMagicRecruit )
{
	CNpc* pNpc = NULL;
	pTarget->QueryObj(OBJ_NPC, IPP_OF(pNpc));
	if(pNpc && pNpc->IsAwardScore() && !bMagicRecruit)
	{
		m_pUser->AwardSynWarScore(pNpc, nLifeLost);
	}
	if(pTarget->IsMonster() || pNpc && pNpc->IsGoal())
	{
		int nExp = m_pUser->AdjustExp(pTarget, nLifeLost);

		if (!pTarget->IsAlive() && !bMagicRecruit && m_pUser && m_pUser->GetMap()->GetID() != 1039)
		{
			int	nBonusExp	= pTarget->GetMaxLife()*KILLBONUS_PERCENT/100;

			OtherMemberAwardExp(pTarget, nBonusExp);

			nBonusExp = m_pUser->AdjustExp(pTarget, nBonusExp);
			m_pUser->SendSysMsg(STR_KILLING_EXPERIENCE, nBonusExp);
			nExp += nBonusExp;
		}

		this->AwardExp(nExp, nExp);
	}

	return true;
}

bool CMagic::AwardExp(int nExp, int nBattleExp)
{
	CHECKF(m_pMagic);

	if(nExp <= 0)
		return false;

	if(m_pUser->GetMap()->GetID() == 1039)
	{
		if(nExp == AWARDEXP_BY_TIMES &&
			(m_pMagic->GetInt(MAGICDATA_SORT) == MAGICSORT_ATTACHSTATUS
				|| m_pMagic->GetInt(MAGICDATA_SORT) == MAGICSORT_TRANSFORM
				|| m_pMagic->GetInt(MAGICDATA_SORT) == MAGICSORT_ATKSTATUS
				))
		{
			if(::RandGet(20) == 0)
				nExp = AWARDEXP_BY_TIMES;
			else
				nExp = 0;
		}
		else
		{
			if(m_pUser->IsBowman())
				nExp	/= 2;
			nExp = ::CutTrail(1, MulDiv(nExp, GOAL_EXP_PERCENT, 100));
		}

		if(nBattleExp)
		{
			if(m_pUser->IsBowman())
				nBattleExp	/= 2;
			nBattleExp = ::CutTrail(1, MulDiv(nBattleExp, GOAL_EXP_PERCENT, 100));
		}
	}

	if(nBattleExp)
		m_pUser->AwardBattleExp(nBattleExp, true, true);

	if(!CheckAwardExpEnable(Prof2Mask(m_pUser->GetProfession())))
		return false;

	if(nExp && m_pMagic->GetInt(MAGICDATA_NEED_EXP) >= 0)
	{
		nExp += m_pUser->GetGemMgcExpEffect()*nExp/100;
		if (m_pUser->IsIncomeLimited())
			nExp = m_pUser->GetIncomePercent()*nExp/100;

		int	nAllExp = m_pMagic->GetInt(MAGICDATA_EXP) + nExp;
		m_pMagic->SetInt(MAGICDATA_EXP, nAllExp);

		CMsgFlushExp	msg;
		IF_OK(msg.Create(MSGFLUSHEXP_MAGIC, m_pMagic->GetInt(MAGICDATA_TYPE), nAllExp))
			m_pUser->SendMsg(&msg);
		UpLevel(SYNCHRO_TRUE);
		m_pUser->AddContributeExp(nExp);
		return true;
	}
	return false;
}

bool CMagic::UpLevel(bool bSynchro)
{
	CHECKF(m_pMagic);
	CHECKF(!IsWeaponMagic(m_pMagic->GetInt(MAGICDATA_TYPE)));

	int	nNeedExp = m_pMagic->GetInt(MAGICDATA_NEED_EXP);

	if(!( nNeedExp > 0
			&& ( m_pMagic->GetInt(MAGICDATA_EXP) >= nNeedExp
					|| m_pMagic->GetInt(MAGICDATA_OLD_LEVEL)
						&& m_pMagic->GetInt(MAGICDATA_LEVEL) >= m_pMagic->GetInt(MAGICDATA_OLD_LEVEL)/2
						&& m_pMagic->GetInt(MAGICDATA_LEVEL) < m_pMagic->GetInt(MAGICDATA_OLD_LEVEL) ) ))
		return false;

	int	nNewLevel = m_pMagic->GetInt(MAGICDATA_LEVEL) + 1;
	m_pMagic->SetInt(MAGICDATA_EXP, 0);
	m_pMagic->SetInt(MAGICDATA_LEVEL, nNewLevel);
	m_pMagic->Update();

	if(bSynchro)
	{
		CMsgMagicInfo msg;
		IF_OK(msg.Create(m_pMagic->GetInt(MAGICDATA_TYPE), nNewLevel, 0))
			m_pUser->SendMsg(&msg);
	}

	return true;
}

bool CMagic::AddMagicExp(int nType, int nExp)
{
	if(!m_pUser || nExp <= 0)
		return false;

	IMagicData* pMagic = FindMagic(nType);
	if(!pMagic)
		return false;

	int nNeedExp = pMagic->GetInt(MAGICDATA_NEED_EXP);
	if(nNeedExp == -1 || nNeedExp == 0)
		return false;
	if(m_pUser->GetLev() < pMagic->GetInt(MAGICDATA_NEED_LEVEL))
		return false;
	if(pMagic->GetInt(MAGICDATA_NEED_PROFMASK))
	{
		if((Prof2Mask(m_pUser->GetProfession()) & pMagic->GetInt(MAGICDATA_NEED_PROFMASK)) == 0)
			return false;
	}

	int nLevelUp = 0;
	int nCurExp = pMagic->GetInt(MAGICDATA_EXP) + nExp;
	while(true)
	{
		nNeedExp = pMagic->GetInt(MAGICDATA_NEED_EXP);
		if(nNeedExp == -1 || nNeedExp == 0 || m_pUser->GetLev() < pMagic->GetInt(MAGICDATA_NEED_LEVEL))
		{
			nCurExp = 0;
			break;
		}
		if(nCurExp < nNeedExp)
			break;

		nCurExp -= nNeedExp;
		int nOldLevel = pMagic->GetInt(MAGICDATA_LEVEL);
		UpLevelByTask(nType, false);
		nLevelUp++;
		if(pMagic->GetInt(MAGICDATA_LEVEL) > nOldLevel+1)
		{
			nCurExp = 0;
			break;
		}
	}
	pMagic->SetInt(MAGICDATA_EXP, nCurExp);
	pMagic->Update();

	if(nLevelUp > 0)
	{
		CMsgMagicInfo msg;
		IF_OK(msg.Create(pMagic->GetInt(MAGICDATA_TYPE), pMagic->GetInt(MAGICDATA_LEVEL), nCurExp))
			m_pUser->SendMsg(&msg);
	}
	else
	{
		CMsgFlushExp	msg;
		IF_OK(msg.Create(MSGFLUSHEXP_MAGIC, pMagic->GetInt(MAGICDATA_TYPE), nCurExp))
			m_pUser->SendMsg(&msg);
	}
	return true;
}

bool CMagic::MagicAttack(int nType, OBJID idTarget, int x, int y)
{
	if(m_tIntone.IsActive())
		m_tIntone.Clear();
	m_pUser->SetAttackTarget(NULL);

	IMagicData* pData = FindMagic(nType);
	if(!pData || pData->GetInt(MAGICDATA_AUTO_ACTIVE))
	{
		LOGCHEAT("invalid magic type: %d, user[%s][%d]", nType, m_pUser->GetName(), m_pUser->GetID());
		AbortMagic(true);
		return false;
	}

	if(!CheckCondition(pData, idTarget, x, y))
	{
		if(pData->GetInt(MAGICDATA_SORT) == MAGICSORT_COLLIDE)
		{
			ProcessCollideFail(x, y, idTarget);
		}
		else if(pData->GetInt(MAGICDATA_SORT) == MAGICSORT_JUMPATTACK)
		{
			IF_NOT(m_pUser->GetDistance(x, y) <= pData->GetInt(MAGICDATA_DISTANCE))
			{
				m_pUser->SendSysMsg(STR_INVALID_MSG);
				UserManager()->KickOutSocket(m_pUser->GetSocketID(), "JUMPATTACK too far!");
				return false;
			}
			m_pMagic		= pData;
			ProcessJumpAttackFaild(x, y);
		}
		AbortMagic(true);
		return false;
	}

	m_pMagic		= pData;
	m_idTarget		= idTarget;
	if(pData->GetInt(MAGICDATA_GROUND))
		m_idTarget	= ID_NONE;

	m_bAutoAttack	= true;

	if (MAGICSORT_COLLIDE == pData->GetInt(MAGICDATA_SORT))
		m_nData	= (int)idTarget;

	m_pos.x			= x;
	m_pos.y			= y;
	m_tIntone.Startup(m_pMagic->GetInt(MAGICDATA_INTONE));

	IRole* pRole = RoleManager()->QueryRole(m_idTarget);
	if(m_pUser->GetMap()->GetID() != 1039)
	{
		if(m_pMagic->GetInt(MAGICDATA_USE_MP) > 0)
			m_pUser->AddAttrib(_USERATTRIB_MANA, -1*m_pMagic->GetInt(MAGICDATA_USE_MP), SYNCHRO_TRUE);
		if(m_pMagic->GetInt(MAGICDATA_USE_EP) > 0)
			m_pUser->AddAttrib(_USERATTRIB_ENERGY, -1*m_pMagic->GetInt(MAGICDATA_USE_EP), SYNCHRO_TRUE);
		if(pData->GetInt(MAGICDATA_USE_ITEM) && pData->GetInt(MAGICDATA_ITEM_NUM) > 0)
			m_pUser->SpendEquipItem(pData->GetInt(MAGICDATA_USE_ITEM), pData->GetInt(MAGICDATA_ITEM_NUM), SYNCHRO_TRUE);
	}

	if (pRole && pData->GetInt(MAGICDATA_NEXT_MAGIC) == pData->GetInt(MAGICDATA_TYPE))
	{
		CUser* pTargetUser = NULL;
		if (pRole->QueryObj(OBJ_USER, IPP_OF(pTargetUser)))
			m_bAutoAttack	= false;
	}

	if (pData->GetInt(MAGICDATA_USE_XP) == TYPE_XPSKILL)
		m_pUser->BurstXp();

	if(!IsWeaponMagic(m_pMagic->GetInt(MAGICDATA_TYPE)))
	{
		int nDeltaBattleEffect = 0;
		CUser* pTargetUser = pRole ? pRole->QueryOwnerUser() : NULL;
		if (pTargetUser)
			nDeltaBattleEffect = m_pUser->GetBattlePower() - pTargetUser->GetBattlePower();

		CMsgInteract	msg;
		IF_OK(msg.Create(INTERACT_MAGICATTACK, m_pUser->GetID(), idTarget, x, y, m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), nDeltaBattleEffect))
			m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}

	if (m_pMagic->GetInt(MAGICDATA_USE_MP) != 0)
	{
		if(m_pUser->GetMap()->GetID() != 1039)
			m_pUser->DecEquipmentDurability(false, true);
	}
	if (::RandGet(10) == 7)
		m_pUser->SendGemEffect(true);

	if(m_pMagic->GetInt(MAGICDATA_INTONE) <= 0)
	{
		m_tIntone.Clear();
		Launch();
		return true;
	}

	return true;
}

void CMagic::BreakAutoAttack()
{
	m_bAutoAttack	= false;
}

bool CMagic::AbortMagic(bool bSynchro)
{
	BreakAutoAttack();
	m_pMagic = NULL;

	if (m_tIntone.IsActive())
	{
		m_tIntone.Clear();
		if (m_pUser)
		{
			CMsgAction msg;
			if (msg.Create(m_pUser->GetID(), 0, 0, 0, actionAbordMagic))
				m_pUser->SendMsg(&msg);
		}
	}
	else if (bSynchro && m_pUser)
	{
		CMsgAction msg;
		if (msg.Create(m_pUser->GetID(), 0, 0, 0, actionAbordMagic))
			m_pUser->SendMsg(&msg);
	}

	return true;
}

void CMagic::ShowMiss()
{
	CHECK(m_pMagic);

	CMsgMagicEffect	msg;
	if(m_pMagic->GetInt(MAGICDATA_GROUND))
		msg.CreateByPos(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_pos.x, m_pos.y, 0);
	else
		msg.Create(m_pUser->GetID(), m_pMagic->GetInt(MAGICDATA_TYPE), m_pMagic->GetInt(MAGICDATA_LEVEL), m_idTarget, 0, 0);
	m_pUser->BroadcastRoomMsg(&msg, true);
}

bool CMagic::CheckCondition(IMagicData* pData, OBJID idTarget, int x, int y)
{
	if(!pData || !m_pUser)
		return false;

	if(m_pUser->GetMap()->IsBoothEnable())
	{
		if(pData->GetInt(MAGICDATA_SORT) != MAGICSORT_ATTACHSTATUS || pData->GetInt(MAGICDATA_STATUS) != STATUS_LUCKY_PRAY)
			return false;
	}

	if(m_pUser->GetMap()->IsDeadIsland() && pData->GetInt(MAGICDATA_CRIME) && idTarget)
	{
		IRole* pTarget = m_pUser->FindAroundRole(idTarget);
	}

	if(m_pUser->GetPk() >= 1000 && pData->GetInt(MAGICDATA_CRIME)
			&& !m_pUser->GetMap()->IsPkField() && !m_pUser->GetMap()->IsSynMap() && idTarget)
	{
		IRole* pTarget = m_pUser->FindAroundRole(idTarget);
		CUser* pOwner = pTarget ? pTarget->QueryOwnerUser() : NULL;
		if(pOwner && pOwner->GetPk() < 30 && !pOwner->IsEvil())
		{
			m_pUser->SendSysMsg(STR_PK);
			return false;
		}
	}

	if(pData->GetInt(MAGICDATA_TARGET) & 2)
	{
		if(idTarget != m_pUser->GetID())
			return false;
	}

	int nDelay = m_nDelay;
	if(pData->GetInt(MAGICDATA_DELAY))
		nDelay = pData->GetInt(MAGICDATA_DELAY);
	if(!m_tDelay.IsTimeOut(nDelay) && MAGICSORT_COLLIDE != pData->GetInt(MAGICDATA_SORT))
		return false;

	if(m_pUser->GetMap()->GetID() != 1039)
	{
		if((int)m_pUser->GetMana() < pData->GetInt(MAGICDATA_USE_MP))
			return false;
		if(m_pUser->GetEnergy() < pData->GetInt(MAGICDATA_USE_EP))
			return false;
		if(pData->GetInt(MAGICDATA_USE_ITEM))
		{
			if(!m_pUser->CheckWeaponSubType(pData->GetInt(MAGICDATA_USE_ITEM), pData->GetInt(MAGICDATA_ITEM_NUM)))
				return false;
		}
	}
	if(pData->GetInt(MAGICDATA_USE_XP) == TYPE_XPSKILL)
	{
		if(m_pUser->GetXp() < 100)
			return false;
	}

	if(pData->GetInt(MAGICDATA_WEAPON_SUBTYPE))
	{
		if(!m_pUser->CheckWeaponSubType(pData->GetInt(MAGICDATA_WEAPON_SUBTYPE), 0))
			return false;
	}

	int nSort = pData->GetInt(MAGICDATA_SORT);
	if((nSort == MAGICSORT_CALLTEAMMEMBER || nSort == MAGICSORT_RECORDTRANSSPELL) && m_pUser->GetMap()->IsFlyToDisable())
	{
		return false;
	}
	if(nSort == MAGICSORT_RECORDTRANSSPELL && m_pUser->GetMap()->QueryRegion(REGION_CITY, m_pUser->GetPosX(), m_pUser->GetPosY()))
	{
		return false;
	}
	if(m_pUser->QueryTransformation() && nSort != MAGICSORT_TRANSFORM)
	{
		return false;
	}
	if(m_pUser->IsWing() && nSort == MAGICSORT_TRANSFORM)
	{
		return false;
	}
	if(nSort == MAGICSORT_ATTACHSTATUS && pData->GetInt(MAGICDATA_STATUS) == STATUS_WING && !m_pUser->GetMap()->IsWingEnable())
	{
		return false;
	}
	if((m_pUser->GetMap()->IsBoothEnable() || m_pUser->GetMap()->GetID() == 1039) && nSort == MAGICSORT_CALLPET)
	{
		return false;
	}

	IRole* pRole = NULL;
	if (!pData->GetInt(MAGICDATA_GROUND))
	{
		pRole = m_pUser->FindAroundRole(idTarget);
		if (!pRole)
			return false;

		if (!pRole->IsAlive() && nSort != MAGICSORT_DETACHSTATUS)
			return false;

		x = pRole->GetPosX();
		y = pRole->GetPosY();
	}
	if(m_pUser->GetDistance(x, y) > pData->GetInt(MAGICDATA_DISTANCE))
		return false;

	if(!pRole)
		return true;

	CNpc* pNpc = NULL;
	pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc));
	if(pNpc)
	{
		if(!pNpc->IsBeAtkable())
			return false;
		if(pNpc->IsGoal())
		{
			if(m_pUser->GetLev() < pNpc->GetLev())
				return false;
			if(pNpc->GetType() != _MAGICGOAL_NPC)
				return false;
		}
	}

	if(pData->GetInt(MAGICDATA_SORT) == MAGICSORT_RECRUIT)
	{
		CMonster* pMonster=NULL;
		pRole->QueryObj(OBJ_MONSTER, IPP_OF(pMonster));
		if(pNpc && !pNpc->IsGoal())
			return false;
		if(pMonster && pMonster->GetID() >= PETID_FIRST && pMonster->GetID() <= PETID_LAST)
			return false;
	}

	if(m_pUser->IsBowman() && !m_pUser->IsArrowPass(x, y))
		return false;

	if(pRole->QueryStatus(64) && pData->GetInt(MAGICDATA_SORT) == MAGICSORT_ATTACHSTATUS)
		return false;

	return true;
}

bool CMagic::CheckCrime(IRole* pRole)
{
	CHECKF(m_pMagic);
	CHECKF(pRole);

	if(!m_pMagic->GetInt(MAGICDATA_CRIME))
		return false;

	return m_pUser->CheckCrime(pRole);
}

bool CMagic::CheckCrime(const vector<IRole*>* pSet)
{
	CHECKF(m_pMagic);

	if(!m_pMagic->GetInt(MAGICDATA_CRIME))
		return false;

	for(int i = 0; i < (int)pSet->size(); i++)
	{
		if(m_pUser->CheckCrime(pSet->at(i)))
			return true;
	}
	return false;
}

void CMagic::OwnerUpLevel(int nOwnerLev)
{
	m_tDelay.SetInterval(m_nRawDelay);
	m_nDelay	= m_nRawDelay;

	if (m_setData->GetAmount() == 0)
		return;

	DWORD	dwProf = Prof2Mask(m_pUser->GetProfession());
	for(int i = 0; i < m_setData->GetAmount(); i++)
	{
		IMagicData* pData = m_setData->GetObjByIndex(i);
		if(pData && pData->GetInt(MAGICDATA_NEED_EXP) == AUTOLEVELUP_EXP
				&& pData->GetInt(MAGICDATA_USE_XP) != TYPE_KONGFU
				&& nOwnerLev >= pData->GetInt(MAGICDATA_NEED_LEVEL)
				&& (pData->GetInt(MAGICDATA_NEED_PROFMASK) == 0 || (pData->GetInt(MAGICDATA_NEED_PROFMASK) & dwProf))
				&& !IsWeaponMagic(pData->GetInt(MAGICDATA_TYPE))
			)
		{
			int nNewLevel = pData->GetInt(MAGICDATA_LEVEL) + 1;

			pData->SetInt(MAGICDATA_LEVEL, nNewLevel);
			pData->Update();

			CMsgMagicInfo	msg;
			IF_OK(msg.Create(pData->GetInt(MAGICDATA_TYPE), nNewLevel, 0))
				m_pUser->SendMsg(&msg);
		}
	}
}

void CMagic::UserWeaponSkillUpLevel(int nWeaponSkillType, int nWeaponSkillLevel)
{
	if(m_setData->GetAmount() == 0)
		return;

	DWORD	dwProf = Prof2Mask(m_pUser->GetProfession());
	for(int i = 0; i < m_setData->GetAmount(); i++)
	{
		IMagicData* pData = m_setData->GetObjByIndex(i);
		if(pData && pData->GetInt(MAGICDATA_NEED_EXP) == AUTOLEVELUP_EXP
				&& pData->GetInt(MAGICDATA_USE_XP) == TYPE_KONGFU
				&& pData->GetInt(MAGICDATA_WEAPON_SUBTYPE) == nWeaponSkillType
				&& nWeaponSkillLevel >= pData->GetInt(MAGICDATA_NEED_LEVEL)
				&& (pData->GetInt(MAGICDATA_NEED_PROFMASK) == 0 || (pData->GetInt(MAGICDATA_NEED_PROFMASK) & dwProf))
				&& !IsWeaponMagic(pData->GetInt(MAGICDATA_TYPE))
			)
		{
			int nNewLevel = pData->GetInt(MAGICDATA_LEVEL) + 1;

			pData->SetInt(MAGICDATA_LEVEL, nNewLevel);
			pData->Update();

			CMsgMagicInfo	msg;
			IF_OK(msg.Create(pData->GetInt(MAGICDATA_TYPE), nNewLevel, 0))
				m_pUser->SendMsg(&msg);
		}
	}
}

bool CMagic::AutoLearnWeaponSkill(int nWeaponSkillType, int nWeaponSkillLevel, int nWeaponSkillExp )
{
	CMagicTypeData* pLearnMagic = NULL;
	for(int i = 0; i < AutoMagicTypeSet()->GetAmount(); i++)
	{
		CMagicTypeData* pData = AutoMagicTypeSet()->GetObjByIndex(i);
		if(pData && pData->GetInt(MAGICTYPEDATA_AUTO_LEARN)
				&& pData->GetInt(MAGICTYPEDATA_WEAPON_SUBTYPE) == nWeaponSkillType
				&& nWeaponSkillLevel >= pData->GetInt(MAGICTYPEDATA_LEARN_LEVEL)%100
				&& FindMagic(pData->GetInt(MAGICTYPEDATA_TYPE)) == NULL )
		{
			int nMinLevel = pData->GetInt(MAGICTYPEDATA_LEARN_LEVEL) % 100;
			int nMaxLevel = pData->GetInt(MAGICTYPEDATA_LEARN_LEVEL) / 100;

			if(nMaxLevel == 0 || nWeaponSkillLevel >= nMaxLevel)
			{
				pLearnMagic = pData;
				break;
			}
			else if(nWeaponSkillExp > 0)
			{
				MYASSERT(nMinLevel < nMaxLevel);
				int nExp = 0;
				for(int i = nMinLevel; i < nMaxLevel; i++)
				{
					CHECKF(i >= 1 && i < MAX_WEAPONSKILLLEVEL);
					nExp += nWeaponSkillLeveUpNeedExp[i];
				}

				const int nMaxRand = 30000;

				int nRand = (nExp / nWeaponSkillExp) / 2;
				if(::RandGet(nRand <= nMaxRand ? nRand : nMaxRand) == 0 && ::RandGet(nRand/nMaxRand) == 0)
				{
					pLearnMagic = pData;
					break;
				}
			}
		}
	}

	if(pLearnMagic)
	{
		LearnMagic(pLearnMagic->GetInt(MAGICTYPEDATA_TYPE), pLearnMagic->GetInt(MAGICTYPEDATA_LEVEL));

		return true;
	}
	return false;
}

bool CMagic::UserKillTarget(IRole* pTarget)
{
	for(int i = 0; i < m_setData->GetAmount(); i++)
	{
		IMagicData* pData = m_setData->GetObjByIndex(i);
		if( pData && (pData->GetInt(MAGICDATA_AUTO_ACTIVE) & AUTOACTIVE_KILL)
				&& !m_pUser->QueryTransformation()
				&& (!pData->GetInt(MAGICDATA_WEAPON_SUBTYPE) || m_pUser->CheckWeaponSubType(pData->GetInt(MAGICDATA_WEAPON_SUBTYPE)))
				&& ::RandGet(100) < pData->GetInt(MAGICDATA_PERCENT) )
		{
			IMagicData*	pMagic		= m_pMagic;
			OBJID		idTarget	= m_idTarget;
			POINT		pos;
			pos.x			= m_pos.x;
			pos.y			= m_pos.y;

			switch(pData->GetInt(MAGICDATA_SORT))
			{
			case	MAGICSORT_RECRUIT:
				{
					m_pMagic		= pData;
					m_idTarget		= m_pUser->GetID();
					m_pos.x			= m_pUser->GetPosX();
					m_pos.y			= m_pUser->GetPosY();
					ProcessRecruit();
				}
				break;
			case	MAGICSORT_SERIALCUT:
				{
					m_pMagic		= pData;
					m_idTarget		= pTarget->GetID();
					m_pos.x			= pTarget->GetPosX();
					m_pos.y			= pTarget->GetPosY();
					ProcessSerialCut();
				}
				break;
			}

			m_pMagic		= pMagic;
			m_idTarget		= idTarget;
			m_pos.x			= pos.x;
			m_pos.y			= pos.y;

			return true;
		}
	}

	return true;
}

bool CMagic::AutoAttackTarget(IRole* pTarget)
{
	for(int i = 0; i < m_setData->GetAmount(); i++)
	{
		IMagicData* pData = m_setData->GetObjByIndex(i);
		if( pData && (pData->GetInt(MAGICDATA_AUTO_ACTIVE) & AUTOACTIVE_RANDOM)
				&& !m_pUser->QueryTransformation()
				&& (!pData->GetInt(MAGICDATA_WEAPON_SUBTYPE) || m_pUser->CheckWeaponSubType(pData->GetInt(MAGICDATA_WEAPON_SUBTYPE)))
				&& ::RandGet(100) < pData->GetInt(MAGICDATA_PERCENT) )
		{
			IMagicData*	pMagic		= m_pMagic;
			OBJID		idTarget	= m_idTarget;
			POINT		pos;
			pos.x			= m_pos.x;
			pos.y			= m_pos.y;
			bool		bAutoAttack	= m_bAutoAttack;

			switch(pData->GetInt(MAGICDATA_SORT))
			{
			case	MAGICSORT_FAN:
				{
					m_pMagic		= pData;
					m_idTarget		= pTarget->GetID();
					m_pos.x			= pTarget->GetPosX();
					m_pos.y			= pTarget->GetPosY();
					ProcessFan();
				}
				break;
			case	MAGICSORT_BOMB:
				{
					m_pMagic		= pData;
					m_idTarget		= pTarget->GetID();
					m_pos.x			= m_pUser->GetPosX();
					m_pos.y			= m_pUser->GetPosY();
					ProcessBomb();
				}
				break;
			case	MAGICSORT_LINE:
				{
					m_pMagic		= pData;
					m_idTarget		= pTarget->GetID();
					m_pos.x			= pTarget->GetPosX();
					m_pos.y			= pTarget->GetPosY();
					ProcessLine();
				}
				break;
			case	MAGICSORT_ATKSTATUS:
				{
					m_pMagic		= pData;
					m_idTarget		= pTarget->GetID();
					m_pos.x			= pTarget->GetPosX();
					m_pos.y			= pTarget->GetPosY();
					ProcessAtkStatus();
				}
				break;
			}

			if(pData->GetInt(MAGICDATA_PERCENT) < 100)
			{
				m_pMagic		= pMagic;
				m_idTarget		= idTarget;
				m_pos.x			= pos.x;
				m_pos.y			= pos.y;
				m_bAutoAttack	= bAutoAttack;
			}
			else
				m_bAutoAttack	= true;

			return true;
		}
	}

	return false;
}

bool CMagic::WeaponMagicAttack(int nType, OBJID idTarget, int x, int y)
{
	IMagicData* pData = FindMagic(nType);
	if(!pData || m_pUser->QueryTransformation() || ::RandGet(100) >= pData->GetInt(MAGICDATA_PERCENT))
		return true;

	IMagicData*	pMagic		= m_pMagic;
	OBJID		idOldTarget	= m_idTarget;
	POINT		pos;
	pos.x			= m_pos.x;
	pos.y			= m_pos.y;

	(void)idTarget;
	switch(pData->GetInt(MAGICDATA_SORT))
	{
	case	MAGICSORT_ATTACK:
		if(idOldTarget != ID_NONE)
		{
			m_pMagic		= pData;
			m_idTarget		= idOldTarget;
			m_pos.x			= x;
			m_pos.y			= y;
			ProcessAttack();
		}
		break;
	case	MAGICSORT_RECRUIT:
		{
			m_pMagic		= pData;
			m_idTarget		= m_pUser->GetID();
			m_pos.x			= m_pUser->GetPosX();
			m_pos.y			= m_pUser->GetPosY();
			ProcessRecruit();
		}
		break;
	case	MAGICSORT_ATTACHSTATUS:
		{
			m_pMagic		= pData;
			m_idTarget		= m_pUser->GetID();
			m_pos.x			= m_pUser->GetPosX();
			m_pos.y			= m_pUser->GetPosY();
			ProcessAttach();
		}
		break;
	case	MAGICSORT_ATKSTATUS:
		if(idOldTarget != ID_NONE)
		{
			m_pMagic		= pData;
			m_idTarget		= idOldTarget;
			m_pos.x			= x;
			m_pos.y			= y;
			ProcessAtkStatus();
		}
		break;
	case	MAGICSORT_ADDMANA:
		{
			m_pMagic		= pData;
			m_idTarget		= m_pUser->GetID();
			m_pos.x			= m_pUser->GetPosX();
			m_pos.y			= m_pUser->GetPosY();
			ProcessAddMana();
		}
		break;
	default:
		return false;
	}

	m_pMagic		= pMagic;
	m_idTarget		= idOldTarget;
	m_pos.x			= pos.x;
	m_pos.y			= pos.y;

	return true;
}

void CMagic::OnTimer(DWORD tCurr)
{
	if (!m_pMagic)
		return;

	if (m_tIntone.TimeOver())
	{
		Launch();
		return;
	}

	if (!m_bAutoAttack || m_tIntone.IsActive())
		return;

	int nDelay = m_nDelay;
	if (m_pMagic->GetInt(MAGICDATA_DELAY))
		nDelay = m_pMagic->GetInt(MAGICDATA_DELAY);

	if (m_pUser->GetMap()->GetID() != 1039)
	{
		if (!m_tDelay.IsTimeOut(m_pUser->GetDelay() + nDelay))
			return;

		if (m_pMagic->GetInt(MAGICDATA_NEXT_MAGIC) != ID_NONE)
		{
			if (m_pMagic->GetInt(MAGICDATA_NEXT_MAGIC) != m_pMagic->GetInt(MAGICDATA_TYPE)
					&& !FindMagic(m_pMagic->GetInt(MAGICDATA_NEXT_MAGIC)))
				MYASSERT(EquipWeaponMagic(SpawnMagicID(), m_pMagic->GetInt(MAGICDATA_NEXT_MAGIC), 0));
			MagicAttack(m_pMagic->GetInt(MAGICDATA_NEXT_MAGIC), m_idTarget, m_pos.x, m_pos.y);
		}
		m_pUser->SetDelay(0);
		return;
	}

	if (!m_tDelay.IsTimeOut(nDelay))
		return;

	if (!(m_pMagic->GetInt(MAGICDATA_USE_XP) == TYPE_XPSKILL
			|| m_pMagic->GetInt(MAGICDATA_AUTO_ACTIVE)
			|| m_pMagic->GetInt(MAGICDATA_SORT) == MAGICSORT_COLLIDE))
		MagicAttack(m_pMagic->GetInt(MAGICDATA_TYPE), m_idTarget, m_pos.x, m_pos.y);

	if (m_pUser)
	{
		m_pUser->DetachLuckyStatus();
		m_pUser->UpdateTimeUnk63C();
	}
}

bool CMagic::CheckType(int nType)
{
	return FindMagic(nType) != NULL;
}

bool CMagic::CheckLevel(int nType, int nLevel)
{
	IMagicData* pData = FindMagic(nType);
	if(pData && pData->GetInt(MAGICDATA_LEVEL) == nLevel)
		return true;
	return false;
}

bool CMagic::UpLevelByTask(int nType, bool bSynchro)
{
	IMagicData* pData = FindMagic(nType);
	if(!pData)
		return false;
	CHECKF(!IsWeaponMagic(pData->GetInt(MAGICDATA_TYPE)));

	int	nNewLevel = pData->GetInt(MAGICDATA_LEVEL) + 1;	bool bOldLevel = false;
	if((float)(nNewLevel + 0.1f) >= pData->GetInt(MAGICDATA_OLD_LEVEL) * 0.5f
			&& nNewLevel < pData->GetInt(MAGICDATA_OLD_LEVEL))
	{
		nNewLevel = pData->GetInt(MAGICDATA_OLD_LEVEL);	bOldLevel = true;
		IF_NOT(CMagic::FindMagicType(nType, nNewLevel))
			return false;
	}
	else
	{
		IF_NOT(CMagic::FindMagicType(nType, nNewLevel))
			return false;
	}

	if (bOldLevel) {	pData->SetInt(MAGICDATA_UNLEARN, false);
	pData->SetInt(MAGICDATA_OLD_LEVEL, 0);	}
	pData->SetInt(MAGICDATA_EXP, 0);
	pData->SetInt(MAGICDATA_LEVEL, nNewLevel);
	pData->Update();
	if(bSynchro)
	{
		CMsgMagicInfo msg;
		IF_OK(msg.Create(pData->GetInt(MAGICDATA_TYPE), nNewLevel, 0))
			m_pUser->SendMsg(&msg);
	}

	return true;
}

OBJID CMagic::LearnMagic(int nType, int nLevel )
{
	CHECKF(CMagic::FindMagicType(nType, nLevel));

	if(FindMagic(nType))
	{
		return ID_NONE;
	}

	DWORD	dwExp	= 0;
	OBJID	idMagic	= ID_NONE;

	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT id FROM cq_magic WHERE ownerid=%u && type=%d", m_pUser->GetID(), nType);
	CAutoPtr<IRecordset> pRes = _Database(PID)->CreateNewRecordset(szSQL);
	if(pRes)
		idMagic = pRes->GetInt(0);

	IMagicData* pData = CMagicData::CreateNew();
	CHECKF(pData);
	IF_NOT(pData->Create(GameDataDefault()->GetMagicData(), idMagic))
		return ID_NONE;

	pData->SetInt(MAGICDATA_OWNERID, m_pUser->GetID());
	pData->SetInt(MAGICDATA_TYPE, nType);
	pData->SetInt(MAGICDATA_LEVEL, nLevel);
	pData->SetInt(MAGICDATA_EXP, dwExp);
	pData->SetInt(MAGICDATA_UNLEARN, false);
	if(idMagic != ID_NONE)
	{
		pData->Update();
	}
	else
	{
		pData->SetInt(MAGICDATA_OLD_LEVEL, 0);
		idMagic = pData->InsertRecord();
		MYASSERT(idMagic);
	}
	pData->FindType();

	CMsgMagicInfo msg;
	IF_OK(msg.Create(pData->GetInt(MAGICDATA_TYPE), pData->GetInt(MAGICDATA_LEVEL), pData->GetInt(MAGICDATA_EXP)))
		m_pUser->SendMsg(&msg);

	m_setData->AddObj(pData);
	return idMagic;
}

bool CMagic::UnlearnMagic(int nType, int nMode)
{
	IMagicData* pData = FindMagic(nType);
	if(!pData)
		return false;

	if(nMode == UNLEARN_DROP)
	{
		MYASSERT(pData->DeleteRecord());
	}
	else
	{
		pData->SetInt(MAGICDATA_OLD_LEVEL, pData->GetInt(MAGICDATA_LEVEL));
		if(nMode == UNLEARN_NORMAL)
			pData->SetInt(MAGICDATA_UNLEARN, true);
		else if(nMode == UNLEARN_RESET)
		{
			pData->SetInt(MAGICDATA_LEVEL, 0);
			pData->SetInt(MAGICDATA_EXP, 0);
		}
		pData->Update();
	}

	if(m_pMagic && m_pMagic->GetID() == pData->GetID())
		m_pMagic = NULL;
	if(nMode != UNLEARN_RESET)
		m_setData->DelObj(pData->GetID());

	CMsgAction	msg;
	if(nMode == UNLEARN_RESET)
	{	CMsgMagicInfo	msgInfo;	if(msgInfo.Create(nType, 0, 0))	m_pUser->SendMsg(&msgInfo);
		return true;	}
	IF_OK(msg.Create(m_pUser->GetID(), m_pUser->GetPosX(), m_pUser->GetPosY(), m_pUser->GetDir(), (nMode!=UNLEARN_RESET)?actionDropMagic:actionResetMagic, nType))
		m_pUser->SendMsg(&msg);

	return true;
}

bool CMagic::EquipWeaponMagic(OBJID idMagic, int nType, int nLevel)
{

	CMagicTypeData* pType = CMagic::FindMagicType(nType, nLevel);
	CHECKF(pType);

	DWORD	dwExp = 0;

	IMagicData* pData = CMagicData::CreateNew();
	CHECKF(pData);
	IF_NOT(pData->Create(GameDataDefault()->GetMagicData(), idMagic))
		return false;

	pData->SetInt(MAGICDATA_ID_, idMagic);
	pData->SetInt(MAGICDATA_OWNERID, m_pUser->GetID());
	pData->SetInt(MAGICDATA_TYPE, nType);
	pData->SetInt(MAGICDATA_LEVEL, nLevel);
	pData->SetInt(MAGICDATA_EXP, dwExp);
	pData->ClearUpdateFlags();
	pData->FindType();

	m_setData->DelObj(idMagic);
	m_setData->AddObj(pData);

	return true;
}

bool CMagic::UnequipWeaponMagic(OBJID idMagic)
{
	if(m_pMagic && m_pMagic->GetID() == idMagic)
	{
		m_pMagic		= NULL;
		m_bAutoAttack	= false;
	}
	return m_setData->DelObj(idMagic);
}

int CMagic::CalcMagicPower(IRole* pAtker, IRole* pTarget, int nAtk)
{
	CUser* pUser = NULL;
	if(pAtker->QueryObj(OBJ_USER, IPP_OF(pUser)))
	{
		pUser->GetMagicAttack();
		nAtk += pUser->GetMagicAttack();
	}

	CUser* pTargetUser = NULL;
	if(pTarget->QueryObj(OBJ_USER, IPP_OF(pTargetUser)))
		nAtk += nAtk / -4;
	else
	{
		CMonster* pMonster = NULL;
		if(pTarget->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)) && (DWORD)pMonster->GetType() < 3000)
			nAtk = CBattleEffect::AdjustAttack(pAtker, pTarget, nAtk);
	}

	nAtk += nAtk * (int)__min((DWORD)pTarget->GetMagicDef(), (DWORD)90) / -100;

	if(pUser)
	{
		int nGemEffect = pUser->GetGemHitRateEffect();
		nAtk += nAtk * nGemEffect / 100;
		nAtk += pUser->GetEquipMagicAtkAdd();
		if(nGemEffect >= 180)
			nAtk += nAtk / 20;
		pUser->LuckyCriticalHit(nAtk, pTarget);
	}

	if(pTargetUser)
	{
		nAtk -= pTargetUser->GetEquipMagicDefAdd();
		nAtk += nAtk / -2 * __min(100, pTargetUser->GetGemMgcAtkEffect()) / 100;
	}

	if(pUser && pUser->IsAntiMonster(pTarget))
		nAtk *= 2;

	if(nAtk < 0)
		nAtk = 0;

	CNpc* pNpc = NULL;
	if(pTarget->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsSynFlag() && pNpc->IsSynMoneyEmpty())
		nAtk *= SYNWAR_NOMONEY_DAMAGETIMES;

	nAtk = CBattleEffect::AdjustDamage(pAtker, pTarget, nAtk);
	nAtk = pTarget->AdjustMagicDamage(pAtker, nAtk);

	if(pTargetUser)
	{
		int nReduce = __min(100, pTargetUser->GetEquipReduceDmg());
		if(nReduce > 0)
			nAtk += nAtk * nReduce / -100;
	}

	return __max(1, nAtk);
}

bool CMagic::CheckAwardExpEnable(DWORD dwProf)
{
	CHECKF(m_pMagic);
	return (int)m_pUser->GetLev() >= m_pMagic->GetInt(MAGICDATA_NEED_LEVEL)
								&& m_pMagic->GetInt(MAGICDATA_NEED_EXP) != AUTOLEVELUP_EXP
								&& m_pMagic->GetInt(MAGICDATA_NEED_EXP) != DISABLELEVELUP_EXP
								&& (m_pMagic->GetInt(MAGICDATA_NEED_PROFMASK) == 0 || (dwProf & m_pMagic->GetInt(MAGICDATA_NEED_PROFMASK)))
								;
}

void CMagic::OtherMemberAwardExp(IRole* pTarget, int nRawExp)
{
	if(m_pUser->GetMap()->GetID() == 1039)
		return ;

	CTeam* pTeam	= m_pUser->GetTeam();
	if (pTeam)
	{
		pTeam->AwardMemberExp(m_pUser->GetID(), pTarget, nRawExp);
	}
}

int	CMagic::GetExtendAtkRange	(CItem* pWeaponR, CItem* pWeaponL)
{
	CHECKF(m_pUser);

	int nRange = 0;
	for(int i = 0; i < m_setData->GetAmount(); i++)
	{
		IMagicData* pData = m_setData->GetObjByIndex(i);
		if (pData
				&& (pData->GetInt(MAGICDATA_AUTO_ACTIVE) & AUTOACTIVE_FOREVER)
				&& MAGICSORT_ATKRANGE == pData->GetInt(MAGICDATA_SORT))
		{
			int nWeaponRequired = pData->GetInt(MAGICDATA_WEAPON_SUBTYPE);
			if (0 == nWeaponRequired
					|| (pWeaponR && pWeaponR->GetItemSubType() == nWeaponRequired)
					|| (pWeaponL && pWeaponL->GetItemSubType() == nWeaponRequired))
			{
				nRange += pData->GetInt(MAGICDATA_POWER);
			}
		}
	}

	return nRange;
}

bool CMagic::HitByMagic()
{
	CHECKF(m_pMagic);

	if(m_pMagic->GetInt(MAGICDATA_WEAPON_HIT) == HIT_BY_MAGIC)
		return true;

	CItem* pItem = m_pUser->GetEquipItemByPos(ITEMPOSITION_WEAPONL);
	if(m_pMagic->GetInt(MAGICDATA_WEAPON_HIT) == HIT_BY_ITEM && pItem && pItem->GetInt(ITEMDATA_MAGICATK) > 0 && pItem->GetInt(ITEMDATA_AMOUNT))
		return true;

	return false;
}

bool CMagic::HitByWeapon()
{
	CHECKF(m_pMagic);

	if(m_pMagic->GetInt(MAGICDATA_WEAPON_HIT) == HIT_BY_WEAPON)
		return true;

	CItem* pItem = m_pUser->GetEquipItemByPos(ITEMPOSITION_WEAPONL);
	if(m_pMagic->GetInt(MAGICDATA_WEAPON_HIT) == HIT_BY_ITEM && pItem && pItem->GetInt(ITEMDATA_MAGICATK) <= 0 && pItem->GetInt(ITEMDATA_AMOUNT))
		return true;

	return false;
}

void CMagic::ResetDelay()
{
	CHECK(m_pMagic);

	m_nDelay	= m_nRawDelay;
	if(m_pMagic->GetInt(MAGICDATA_DELAY))
		m_nDelay	= m_pMagic->GetInt(MAGICDATA_DELAY);
	m_tDelay.Update();
}
