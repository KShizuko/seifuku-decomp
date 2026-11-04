#include "MsgMagicEffect.h"
#include "3DGameMap.h"
#include "Magic.h"
#include "Hero.h"
#include "GamePlayerSet.h"
#include "Pet.h"

CMsgMagicEffect::CMsgMagicEffect()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgMagicEffect::~CMsgMagicEffect()
{

}

BOOL CMsgMagicEffect::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_MAGICEFFECT != this->GetType())
		return false;

	return true;
}

void CMsgMagicEffect::Process(void* pInfo)
{
	MagicTypeInfo infoMagicType;
	if(m_pInfo->idUser == g_objHero.GetID())
	{
		g_objHero.BeginTransForm(m_pInfo->usType, m_pInfo->usLevel);
		g_objHero.BeginFly(m_pInfo->usType, m_pInfo->usLevel);
		g_objHero.BeginMagicShield(m_pInfo->usType, m_pInfo->usLevel);
	}
	if(!CMagic::s_objMagicData.GetMagicTypeInfo(m_pInfo->usType, m_pInfo->usLevel, infoMagicType))
		return;
	if((infoMagicType.dwStatus & USERSTATUS_TEAM_EXP) != 0)
		return;
	CPlayer* pSender = g_objPlayerSet.GetPlayer(m_pInfo->idUser, true);
	CPlayer* pTarget = g_objPlayerSet.GetPlayer(m_pInfo->idTarget, true);

	if(infoMagicType.dwCrime > 0)
	{
		if(m_pInfo->idUser == g_objHero.GetID())
		{
			g_objHero.SetIDMyTarget(m_pInfo->setEffectRole[0].idRole);
		}
		else
		{
			for(int i = 0; i < m_pInfo->ucEffectNum; i ++)
			{
				if(m_pInfo->setEffectRole[i].idRole == g_objHero.GetID())
					g_objHero.SetIDWhoAttackMe(m_pInfo->idUser);
				CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_pInfo->setEffectRole[i].idRole);
				if(pPlayer && pPlayer->IsMyPet())
					((CPet*)pPlayer)->SetHeroTarget(m_pInfo->idUser);
			}
		}
	}
	if (!pTarget && m_pInfo->ucEffectNum > 0)
		m_pInfo->idTarget = m_pInfo->setEffectRole[::RandGet(m_pInfo->ucEffectNum)].idRole;

	if(infoMagicType.dwSenderAction == _ACTION_TM_FAST_SHOOT || infoMagicType.dwSenderAction == _ACTION_TM_DISPERSION_SHOOT)
	{
		if(!pSender)
			return;

		pSender->ResetStepDir();
		if (!pSender->IsMoving())
		{
			pSender->ResetActionData();
		}
		int nDir = pSender->GetDir(pTarget);
		if(m_pInfo->ucEffectNum > 0)
			nDir = pSender->GetDir(g_objPlayerSet.GetPlayer(m_pInfo->setEffectRole[::RandGet(m_pInfo->ucEffectNum)].idRole, true));

		switch(infoMagicType.dwSenderAction )
		{
		case _ACTION_TM_FAST_SHOOT:
			{
				pSender->SetActionCmd(_ACTION_FAST_SHOOT, nDir);
				int nAmount = infoMagicType.dwLevel +2;
				if(m_pInfo->ucEffectNum > 0)
				{
					int nLife = m_pInfo->setEffectRole[0].dwData;
					int nLifeLost = nLife/nAmount;
					int nLastLifeLost = nLife - (nAmount-1)*nLifeLost;
					DWORD dwDelay = 0;
					for(int i = 0; i < nAmount; i ++)
					{
						CMagicEffect* pEffect = CMagicEffect::CreateNewArrow(pSender->GetID(), m_pInfo->setEffectRole[0].idRole, pSender->GetLWeapon(), dwDelay);
						if(pEffect)
						{
							g_objGameMap.AddMagicEffect(pEffect);
							if(i != nAmount -1)
								pEffect->AddRoleInfo(m_pInfo->setEffectRole[0].idRole,
									nLifeLost);
							else
								pEffect->AddRoleInfo(m_pInfo->setEffectRole[0].idRole,
									nLastLifeLost);
						}
						dwDelay += 150;
					}
				}
			}
			return;
		case _ACTION_TM_DISPERSION_SHOOT:
			{
				pSender->SetActionCmd(_ACTION_ATTACK0, nDir);
				for(int i = 0; i < m_pInfo->ucEffectNum; i ++)
				{
					CMagicEffect* pEffect = CMagicEffect::CreateNewArrow(pSender->GetID(), m_pInfo->setEffectRole[i].idRole, pSender->GetLWeapon());
					if(pEffect)
					{
						g_objGameMap.AddMagicEffect(pEffect);
						pEffect->AddRoleInfo(m_pInfo->setEffectRole[i].idRole,
							m_pInfo->setEffectRole[i].dwData);
					}
				}
			}
			return;
		default:
			pSender->SetActionCmd(_ACTION_ATTACK0, nDir);
			break;
		}

	}

	CMagicEffect* pEffect = NULL;
	if(infoMagicType.dwClientRepresent == MAGICEFFECT_BODY_TERRAIN_BODY)
	{
		CMyPos posTarget;
		posTarget.x = m_pInfo->usPosX;
		posTarget.y = m_pInfo->usPosY;

		pEffect =
			CMagicEffect::CreateNewTerrain(m_pInfo->idUser,
				posTarget, m_pInfo->usType, m_pInfo->usLevel);
	}
	else
	{
		pEffect =
			CMagicEffect::CreateNew(m_pInfo->idUser, m_pInfo->idTarget,
			m_pInfo->usType, m_pInfo->usLevel);
	}

	if(pEffect)
	{
		for(int i = 0; i < m_pInfo->ucEffectNum; i ++)
			pEffect->AddRoleInfo(m_pInfo->setEffectRole[i].idRole,
						m_pInfo->setEffectRole[i].dwData);
		g_objGameMap.AddMagicEffect(pEffect);
	}
}
