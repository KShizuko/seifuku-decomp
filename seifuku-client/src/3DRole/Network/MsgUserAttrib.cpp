#include "AllMsg.h"
#include "Hero.h"
#include "GamePlayerSet.h"
#include "3DGameMap.h"

CMsgUserAttrib::CMsgUserAttrib()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgUserAttrib::~CMsgUserAttrib()
{

}

BOOL CMsgUserAttrib::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_USERATTRIB != this->GetType())
		return false;

	return true;
}

void CMsgUserAttrib::Process(void *pInfo)
{
	CPlayer* pPlayer	=g_objPlayerSet.GetPlayer(m_pInfo->idUser, true);
	if (!pPlayer)
	{
		{
			int nAmount = (int)m_pInfo->dwAttributeNum;
			for(int i = 0; i < nAmount; i ++)
			{
				switch(m_pInfo->aUserAttrib[i].ucAttributeType)
				{
				case _USERATTRIB_MAXLIFE:
					g_objHero.SetTeamMemberMaxHp(m_pInfo->idUser, m_pInfo->aUserAttrib[i].dwAttributeData);
					break;
				case _USERATTRIB_LIFE:
					g_objHero.SetTeamMemberHp(m_pInfo->idUser, m_pInfo->aUserAttrib[i].dwAttributeData);
					break;
				case _USERATTRIB_LEV:
					if (g_objHero.GetPlayerTradeID() == m_pInfo->idUser)
						g_objHero.SetPlayerTradeLevel(m_pInfo->aUserAttrib[i].dwAttributeData);
					break;
				default:
					break;
				}
			}
			return;
		}
	}

	int nAmount = (int)m_pInfo->dwAttributeNum;
	int i;
	for(i = 0; i < nAmount; i ++)
	{
		switch(m_pInfo->aUserAttrib[i].ucAttributeType)
		{
		case _USERATTRIB_LEV:
			pPlayer->SetLev(m_pInfo->aUserAttrib[i].dwAttributeData);
			if (pPlayer->GetID() == g_objHero.GetID())
			{
				::PostCmd(CMD_LEVELUP);
			}
			break;

		case _USERATTRIB_MAXLIFE:
			pPlayer->SetMaxLife(m_pInfo->aUserAttrib[i].dwAttributeData);
			g_objHero.SetTeamMemberMaxHp(pPlayer->GetID(), m_pInfo->aUserAttrib[i].dwAttributeData);
			break;

		case _USERATTRIB_LIFE:
			pPlayer->SetData(CPlayer::_PLAYER_LIFE, m_pInfo->aUserAttrib[i].dwAttributeData);
			g_objHero.SetTeamMemberHp(pPlayer->GetID(), m_pInfo->aUserAttrib[i].dwAttributeData);
			break;

		case _USERATTRIB_MONEY:
			if (g_objHero.GetID() == m_pInfo->idUser)
				g_objHero.SetMoney(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;
		case _USERATTRIB_MONEYSAVED:
			if (g_objHero.GetID() == m_pInfo->idUser)
				g_objHero.SetSavedMoney(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;

		case _USERATTRIB_MANA:
			pPlayer->SetMana(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;

		case _USERATTRIB_HAIR:
			pPlayer->SetHair((USHORT)(m_pInfo->aUserAttrib[i].dwAttributeData%1000));
			pPlayer->SetHead((USHORT)(m_pInfo->aUserAttrib[i].dwAttributeData/1000));
			break;

		case _USERATTRIB_EXP:
			{
				DWORD dwExp = pPlayer->GetExp();
				if(dwExp < m_pInfo->aUserAttrib[i].dwAttributeData)
				{
					g_objGameMap.SetShowExp(m_pInfo->aUserAttrib[i].dwAttributeData - dwExp);
				}
				pPlayer->SetExp(m_pInfo->aUserAttrib[i].dwAttributeData);
			}
			break;
		case _USERATTRIB_LOOK:
			pPlayer->SetFace((m_pInfo->aUserAttrib[i].dwAttributeData/10000)%1000);
			pPlayer->SetSex((m_pInfo->aUserAttrib[i].dwAttributeData/1000)%10);
			pPlayer->SetLook(m_pInfo->aUserAttrib[i].dwAttributeData%1000);
			pPlayer->Transform(m_pInfo->aUserAttrib[i].dwAttributeData/10000000);
			break;
		case _USERATTRIB_USERSTATUS:
			pPlayer->ReplaceStatus(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;
		case _USERATTRIB_SIZEADD:
			pPlayer->ReplaceStatus2(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;
		case _USERATTRIB_BLESSTIME:
			g_objHero.SetBlessTime(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;
		case _USERATTRIB_DBLEXPTIME:
			g_objHero.SetDblExpTime(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;
		case _USERATTRIB_CURSETIME:
			g_objHero.SetCurseTime(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;
		case _USERATTRIB_EMONEY:
			g_objHero.SetEmoney(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;
		case _USERATTRIB_SUPERSOLDIERTAIL:
			g_objHero.m_dwSuperSoldierTail = m_pInfo->aUserAttrib[i].dwAttributeData;
			break;
		case _USERATTRIB_DAYBONUS:
			g_objHero.SetDayBonus(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;
		case _USERATTRIB_ADDPOINT:
			pPlayer->SetAddPoint(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;

		case _USERATTRIB_PK:
			pPlayer->SetPk(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;

		case _USERATTRIB_PORFESSION:
			pPlayer->SetProfession(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;

		case _USERATTRIB_SOUL:
			pPlayer->SetSoul(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;

		case _USERATTRIB_HEALTH:
			pPlayer->SetHealth(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;

		case _USERATTRIB_FORCE:
			pPlayer->SetForce(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;

		case _USERATTRIB_SPEED:
			pPlayer->SetSpeed(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;
		case _USERATTRIB_XP:
			pPlayer->SetXP(m_pInfo->aUserAttrib[i].dwAttributeData);
			::PostCmd(CMD_XPCHANGE);
			break;
		case _USERATTRIB_PP:
			pPlayer->SetPhysicalForce(m_pInfo->aUserAttrib[i].dwAttributeData);
			::PostCmd(CMD_PPCHANGE);
			break;
		case _USERATTRIB_PROFFER:
			pPlayer->SetSyndicateProffer((int)m_pInfo->aUserAttrib[i].dwAttributeData);
			::PostCmd(CMD_FLASH_SYNDICATE);
			break;
		case _USERATTRIB_KO_ORDER:
			if(pPlayer->GetID() == g_objHero.GetID())
				g_objHero.m_dwSuperSoldierTicks = m_pInfo->aUserAttrib[i].dwAttributeData;
			break;
		case _USERATTRIB_EXPLOIT:
			pPlayer->SetVirtue(m_pInfo->aUserAttrib[i].dwAttributeData);
			break;
		case _USERATTRIB_METEMPSYCHOSIS:
			pPlayer->SetMetempsychosis((int)m_pInfo->aUserAttrib[i].dwAttributeData);
			break;
		case _USERATTRIB_LUCKYTIME:
			g_objHero.m_dwLuckyTime = m_pInfo->aUserAttrib[i].dwAttributeData;
			g_objHero.m_dwLuckyTimeStamp = ::TimeGet();
			break;
		default:
			break;
		}
	}
	if((m_pInfo->aUserAttrib[i].ucAttributeType != _USERATTRIB_EXP) &&
		(m_pInfo->aUserAttrib[i].ucAttributeType != _USERATTRIB_MANA) &&
		(m_pInfo->aUserAttrib[i].ucAttributeType != _USERATTRIB_LIFE) &&
		(m_pInfo->aUserAttrib[i].ucAttributeType != _USERATTRIB_MONEY))
	{
		if (m_pInfo->idUser == g_objHero.GetID())
			::PostCmd(CMD_LEVELUP);
	}
}
