#include "windows.h"
#include "define.h"
#include "Array.h"
#include "VarType.h"
#include "I_Aicenter.h"
#include "I_role.h"
#include "Sense.h"
#include "AGent.h"
#include "AGentData.h"
#include "NpcWorld.h"

CSense::CSense()
{

}

CSense::~CSense()
{

}

bool CSense::Create(CAgent* pOwner)
{
	m_pOwner		= pOwner;

	return true;
}

bool CSense::CheckCondition(int idxFactFunction, VarTypeSetRef setParam, ARGUMENT_SET* psetArgument)
{
	CHECKF(idxFactFunction >= 0);

	switch(idxFactFunction)
	{
	case	FACTFUNC_IS_ALIVE:
		{
			OBJID	idRole	= (int)IAiCenter::GetValue(setParam[0], psetArgument);
			IRole* pRole	= UserManager()->QueryRole(idRole);
			if(pRole && pRole->IsAlive())
				return true;
		}
		break;
	case	FACTFUNC_IS_WING:
		{
			OBJID	idRole	= (int)IAiCenter::GetValue(setParam[0], psetArgument);
			IRole* pRole	= UserManager()->QueryRole(idRole);
			if(pRole && pRole->IsWing())
				return true;
		}
		break;
	case	FACTFUNC_IS_USER:
		{
			OBJID	idRole	= (int)IAiCenter::GetValue(setParam[0], psetArgument);
			IRole* pRole	= UserManager()->QueryRole(idRole);
			if(pRole && Cast<CUser>(pRole))
				return true;
		}
		break;
	case	FACTFUNC_IS_MONSTER:
		{
			OBJID	idRole	= (int)IAiCenter::GetValue(setParam[0], psetArgument);
			IRole* pRole	= UserManager()->QueryRole(idRole);
			CNpc* pNpc;
			if(pRole && Cast(&pNpc, pRole) && pNpc->IsMonster())
				return true;
		}
		break;
	case	FACTFUNC_EVIL_VALUE:
		{
			OBJID	idRole	= (int)IAiCenter::GetValue(setParam[0], psetArgument);
			IRole* pRole	= UserManager()->QueryRole(idRole);
			if(pRole)
			{
				VarTypeSet	setFactParam;
				setFactParam.Push(idRole);
				if(pRole->IsEvil())
					setFactParam.Push(100);
				else if(pRole->IsRighteous())
					setFactParam.Push(0);
				else
					setFactParam.Push(50);

				return IAiCenter::MatchParam(setFactParam, setParam, psetArgument);
			}
			return false;
		}
		break;
	case	FACTFUNC_COOL_VALUE:
		{
			OBJID	idRole	= (int)IAiCenter::GetValue(setParam[0], psetArgument);
			IRole* pRole	= UserManager()->QueryRole(idRole);
			if(pRole)
			{
				CUser* pUser = Cast<CUser>(pRole);
				if(pUser)
				{
					VarTypeSet	setFactParam;
					setFactParam.Push(idRole);

					int nCool = 0;
					if(CItem::IsWeapon2(pUser->GetWeaponRType()))
					{
						if(CItem::IsNonsuch(pUser->GetWeaponRType()))
							nCool += 70;
						else if(CItem::IsCool8(pUser->GetWeaponRType()))
							nCool += 35;
					}
					else
					{
						if(CItem::IsNonsuch(pUser->GetWeaponRType()))
							nCool += 50;
						else if(CItem::IsCool8(pUser->GetWeaponRType()))
							nCool += 25;
					}

					if(CItem::IsNonsuch(pUser->GetWeaponLType()))
						nCool += 20;
					else if(CItem::IsCool8(pUser->GetWeaponLType()))
						nCool += 10;

					if(CItem::IsNonsuch(pUser->GetArmorType()))
						nCool += 20;
					else if(CItem::IsCool8(pUser->GetArmorType()))
						nCool += 10;

					if(CItem::IsNonsuch(pUser->GetHelmetType()))
						nCool += 10;
					else if(CItem::IsCool8(pUser->GetHelmetType()))
						nCool += 5;

					setFactParam.Push(nCool);
					return IAiCenter::MatchParam(setFactParam, setParam, psetArgument);
				}
			}
			return false;
		}
		break;
	case	FACTFUNC_LEVEL:
		{
			OBJID	idRole	= (int)IAiCenter::GetValue(setParam[0], psetArgument);
			IRole* pRole	= UserManager()->QueryRole(idRole);
			if(pRole)
			{
				CUser* pUser = Cast<CUser>(pRole);
				if(pUser)
				{
					VarTypeSet	setFactParam;
					setFactParam.Push(idRole);

					int	nLevel = 0;
					CItemTypeData* pType = NpcWorld()->GetItemType()->QueryItemType(pUser->GetWeaponRType());
					int nLev = pType->GetInt(ITEMTYPEDATA_REQ_LEVEL);
					if(nLevel < nLev)
						nLevel = nLev;

					 pType = NpcWorld()->GetItemType()->QueryItemType(pUser->GetWeaponLType());
					 nLev = pType->GetInt(ITEMTYPEDATA_REQ_LEVEL);
					if(nLevel < nLev)
						nLevel = nLev;

					 pType = NpcWorld()->GetItemType()->QueryItemType(pUser->GetArmorType());
					 nLev = pType->GetInt(ITEMTYPEDATA_REQ_LEVEL);
					if(nLevel < nLev)
						nLevel = nLev;

					 pType = NpcWorld()->GetItemType()->QueryItemType(pUser->GetHelmetType());
					 nLev = pType->GetInt(ITEMTYPEDATA_REQ_LEVEL);
					if(nLevel < nLev)
						nLevel = nLev;

					setFactParam.Push(nLevel);
					return IAiCenter::MatchParam(setFactParam, setParam, psetArgument);
				}
			}
			return false;
		}
		break;
	case	FACTFUNC_MY_LIFE:
		{
			int nLife = m_pOwner->GetLife();
			VarTypeSet	setFactParam;
			setFactParam.Push(nLife);
			return IAiCenter::MatchParam(setFactParam, setParam, psetArgument);
		}
		break;
	case	FACTFUNC_ROLE_POS:
		{
			OBJID	idRole	= (int)IAiCenter::GetValue(setParam[0], psetArgument);
			IRole* pRole	= UserManager()->QueryRole(idRole);
			if(pRole && pRole->GetMapID() == m_pOwner->GetMapID())
			{
				VarTypeSet	setFactParam;
				setFactParam.Push(idRole);
				setFactParam.Push(pRole->GetPosX());
				setFactParam.Push(pRole->GetPosY());
				return IAiCenter::MatchParam(setFactParam, setParam, psetArgument);
			}
		}
		break;
	case	FACTFUNC_MYDISTANCE:
		{
			int 	nPosX	= (int)IAiCenter::GetValue(setParam[0], psetArgument);
			int 	nPosY	= (int)IAiCenter::GetValue(setParam[1], psetArgument);
			{
				VarTypeSet	setFactParam;
				setFactParam.Push(nPosX);
				setFactParam.Push(nPosY);
				setFactParam.Push(m_pOwner->GetDistance(nPosX, nPosY));
				return IAiCenter::MatchParam(setFactParam, setParam, psetArgument);
			}
		}
		break;
	case	FACTFUNC_ATTACK_DISTNACE:
		{
			OBJID 	idRole	= (int)IAiCenter::GetValue(setParam[0], psetArgument);
			IRole* pRole	= UserManager()->QueryRole(idRole);
			if(pRole && pRole->GetMapID() == m_pOwner->GetMapID())
			{
				VarTypeSet	setFactParam;
				setFactParam.Push(idRole);
				setFactParam.Push(m_pOwner->GetAttackRange(pRole->GetSizeAdd()));
				return IAiCenter::MatchParam(setFactParam, setParam, psetArgument);
			}
		}
		break;
	case	FACTFUNC_DANGER_VALUE:
		{
			int	nDanger = 0;
			CNpcBigSet::Iterator pNpc = NpcManager()->QuerySet()->NewEnum();
			while(pNpc.Next())
			{
				if(pNpc && pNpc->IsEvil())
				{
					if(m_pOwner->IsCloseTarget(Cast<IRole,INpc>(pNpc)))
						nDanger = ::CutTrail(50, nDanger + 20);
					else if(m_pOwner->IsLookTarget(Cast<IRole,INpc>(pNpc)))
						nDanger += 5;
				}
			}

			CUserSet::Iterator pUser = UserManager()->QuerySet()->NewEnum();
			while(pUser.Next())
			{
				if(pUser && pUser->IsEvil())
				{
					if(m_pOwner->IsCloseTarget(Cast<IRole,CUser>(pUser)))
						nDanger = ::CutTrail(50, nDanger + 20);
					else if(m_pOwner->IsLookTarget(Cast<IRole,CUser>(pUser)))
						nDanger += 5;
				}
			}

			VarTypeSet	setFactParam;
			setFactParam.Push(nDanger);
			return IAiCenter::MatchParam(setFactParam, setParam, psetArgument);
		}
		break;
	case	FACTFUNC_ITEM_COUNT:
		{
			CItemPack* pPack = Cast<CItemPack>(m_pOwner);
			IF_OK(pPack)
			{
				VarTypeSet	setFactParam;
				setFactParam.Push(pPack->QueryItemSet()->GetAmount());
				return IAiCenter::MatchParam(setFactParam, setParam, psetArgument);
			}
		}
		break;
	case	FACTFUNC_MY_LEADER:
		{
			CTeam* pTeam = Cast<CTeam>(m_pOwner);
			if(pTeam && pTeam->IsValid())
			{
				VarTypeSet	setFactParam;
				setFactParam.Push(pTeam->GetLeaderID());
				return IAiCenter::MatchParam(setFactParam, setParam, psetArgument);
			}
		}
		break;
	case	FACTFUNC_IS_LEADER:
		{
			OBJID	idRole	= (int)IAiCenter::GetValue(setParam[0], psetArgument);
			IRole* pRole	= UserManager()->QueryRole(idRole);
			CUser* pUser;
			if(pRole && Cast(&pUser, pRole) && pUser->IsTeamLeader())
			{
				return true;
			}
		}
		break;
	default:
		{
			MYASSERT(!"switch(idxFactFunction)");
		}
	}

	return false;
}

void CSense::Die()
{
}

void CSense::BeAttack(IRole *pRole)
{
}

void CSense::BeKill(IRole *pRole)
{
}

void CSense::Recruit(int nLife)
{
}

void CSense::Reborn()
{
}

void CSense::TeamApply(OBJID idUser)
{
	char	szNum[20];
	_ltoa(idUser, szNum, 10);

	int		nPriority	= 50;
	String	strFact = FACT_TEAM_INVITE;
	strFact += '(';
	strFact	+= szNum;
	strFact += ')';

	IAiCenter* pAi = Cast<IAiCenter>(m_pOwner);
	pAi->AddFact(strFact, nPriority);
}

void CSense::OnTimer()
{
	int		nPriority	= 80;
	if(m_tLook.ToNextTime(::Priority2Durable(80)))
	{
		vector<IRole*>	setRole;
		POINT	pos;
		pos.x	= m_pOwner->GetPosX();
		pos.y	= m_pOwner->GetPosY();
		CGameMap* pMap = m_pOwner->GetMap();
		CHECK(pMap);
		pMap->CollectRole(&setRole, pos, m_pOwner->GetLookRange());
		for(int i = 0; i < (int)setRole.size(); i++)
		{
			IRole* pRole = setRole[i];
			if(pRole && pRole->GetID() != m_pOwner->GetID())
			{
				char	szNum[20];
				_ltoa(pRole->GetID(), szNum, 10);
				String	strFact = FACT_LOOK_TARGET;
				strFact	+= '<';
				strFact	+= szNum;
				strFact	+= '>';

				IAiCenter* pAi = Cast<IAiCenter>(m_pOwner);
				pAi->AddFact(strFact, nPriority);
			}
		}
	}
}
