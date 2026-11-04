#include "MsgFlushExp.h"
#include "Hero.h"

CMsgFlushExp::CMsgFlushExp()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgFlushExp::~CMsgFlushExp()
{

}
BOOL CMsgFlushExp::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_FLUSHEXP != this->GetType())
		return false;
	
	return true;
}

void CMsgFlushExp::Process(void* pInfo)
{
	switch(m_pInfo->ucAction)
	{
	case MSGFLUSHEXP_WEAPONSKILL:
		{
			CWeaponSkill* pSkill = g_objHero.GetWeaponSkill(m_pInfo->usType);
			if(pSkill)
				pSkill->SetExp(m_pInfo->dwExp);

			::PostCmd(CMD_FLASHWEAPONSKILL);
		}
		break;

	case MSGFLUSHEXP_MAGIC:
		{
			{
				int nAmount = g_objHero.GetMagicAmount();
				for(int i = 0; i < nAmount; i ++)
				{
					CMagic* pMagic = g_objHero.GetMagicByIndex(i);
					if(pMagic && pMagic->GetIDType() == m_pInfo->usType)
					{
						pMagic->m_dwExp = m_pInfo->dwExp;
					}
				}
			}
			{
				int nAmount = g_objHero.GetXpSkillAmount();
				for(int i = 0; i < nAmount; i ++)
				{
					CMagic* pMagic = g_objHero.GetXpSkillByIndex(i);
					if(pMagic && pMagic->GetIDType() == m_pInfo->usType)
					{
						pMagic->m_dwExp = m_pInfo->dwExp;
					}
				}
			}

			{
				int nAmount = g_objHero.GetKongfuAmount();
				for(int i = 0; i < nAmount; i ++)
				{
					CMagic* pMagic = g_objHero.GetKongfuByIndex(i);
					if(pMagic && pMagic->GetIDType() == m_pInfo->usType)
					{
						pMagic->m_dwExp = m_pInfo->dwExp;
					}
				}
			}
		}
	default:
		break;
	}
}
