#include "AllMsg.h"
#include "NpcWorld.h"

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

void CMsgMagicEffect::Process(void* pSocket)
{
	CNpcPtr pNpc = NpcManager()->QueryNpc(m_pInfo->idUser);
	if(pNpc)
		pNpc->AttackOnce();

	IRole* pRole = UserManager()->QueryRole(m_pInfo->idUser);
	if(pRole && m_pInfo->usType != MAGICSORT_RECRUIT)
	{
		OBJID idTarget = m_pInfo->idTarget;
		if(idTarget && idTarget != pRole->GetID())
		{
			CNpcPtr pNpc = NpcManager()->QueryNpc(idTarget);
			if(pNpc)
				pNpc->BeAttack(pRole);
		}

		for(int i = 0; i < m_pInfo->ucEffectNum; i++)
		{
			OBJID idRole = m_pInfo->setEffectRole[i].idRole;

			if(idRole != idTarget)
			{
				CNpcPtr pNpc = NpcManager()->QueryNpc(idRole);
				if(pNpc)
					pNpc->BeAttack(pRole);
			}
		}
	}

	CUserPtr pUser = UserManager()->QueryUser(m_pInfo->idUser);
	if(!pUser)
		return;

	switch(m_pInfo->usType)
	{
	case	MAGICSORT_JUMPATTACK:
		{
			pUser->JumpPos(m_pInfo->usPosX, m_pInfo->usPosY, 0);
		}
		break;
	case	MAGICSORT_RANDOMTRANS:
		{
			pUser->JumpPos(m_pInfo->usPosX, m_pInfo->usPosY, 0);
		}
		break;
	case	MAGICSORT_COLLIDE:
		{
		}
		break;
	}
}
