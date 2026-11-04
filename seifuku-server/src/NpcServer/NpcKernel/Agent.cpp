#include "windows.h"
#include "define.h"
#include "AllMsg.h"
#include "common.h"
#include "I_MAPDATA.h"
#include "I_Role.h"
#include "item.h"
#include "GameMap.h"
#include "NpcWorld.h"
#include "Agent.h"
#include "Array.h"

#include "AgentData.h"

CAgent::CAgent()
{
	m_nStep = STEP_NONE;
	m_lnkThis.Init(Cast<IRole>(this));
	m_nSizeAdd		= 0;
}

CAgent::~CAgent()
{
}

bool CAgent::Create(OBJID idAgent, int nTimeOfLife)
{
	m_info.id		= idAgent;
	m_nTimeOfLife	= nTimeOfLife;

	Array<String> setFunction;
	const const char* const * pStrSet = ::GetFactFuncStringSet();
	while(*pStrSet)
		setFunction.Push(*(pStrSet++));

	Array<String> setAction;
							 pStrSet = ::GetActionStringSet();
	while(*pStrSet)
		setAction.Push(*(pStrSet++));

	CHECKF(m_pAiCenter->Create(this, NpcWorld()->GetDatabase(), setFunction, setAction, m_nTimeOfLife));
	CHECKF(m_pBody->Create(this));
	CHECKF(m_pSense->Create(this));
	CHECKF(m_pTeam->Create(this));
	CHECKF(m_pMove->Create(this));
	CHECKF(m_pFight->Create(this));

	MYASSERT(m_nStep == STEP_NONE);
	m_nStep	= STEP_OFFLINE;
	LOGDUMP("Agent Create() OK");
	return true;
}

bool CAgent::AppendInfo(const UserInfoStruct& info)
{
	memcpy(&m_info, &info, sizeof(UserInfoStruct));

	CMsgAction	msg;
	IF_OK(msg.Create(GetID(), GetPosX(), GetPosY(), GetDir(), actionEnterMap))
		SendMsg(&msg);

	LOGDUMP("Agent AppendInfo() OK");
	return true;
}

bool CAgent::KickBack(int nTargX, int nTargY)
{
	{
		CMsgAction	msg;
		IF_OK(msg.Create(GetID(), nTargX, nTargY, GetDir(), actionSynchro, nTargX, nTargY))
			SendMsg(&msg);
	}

	Cast<CRoleMove>(this)->SetPos(nTargX, nTargY);
	return true;
}

bool CAgent::SynchroPos(int nSourX, int nSourY, int nTargX, int nTargY)
{
	if(GetPosX() != nSourX || GetPosY() != nSourY)
	{
		CMsgAction	msg;
		IF_OK(msg.Create(GetID(), GetPosX(), GetPosY(), GetDir(), actionSynchro, GetPosX(), GetPosY()))
			SendMsg(&msg);
	}
	else if(GetPosX() != nTargX || GetPosY() != nTargY)
	{
		Cast<CRoleMove>(this)->SetPos(nTargX, nTargY);
	}
	return true;
}

void CAgent::OnTimer()
{
	if(m_nStep != STEP_LOGIN_OK)
		return ;

	DEBUG_TRY
	m_pSense->OnTimer();
	DEBUG_CATCH("CSense::OnTimer()");

	DEBUG_TRY
	m_pAiCenter->OnTimer();
	DEBUG_CATCH("CAgent::OnTimer()");

	DEBUG_TRY
	m_pBody->OnTimer();
	m_pFight->OnTimer();
	m_pMove->OnTimer();
	DEBUG_CATCH("CAgent::OnTimer()");
}

void CAgent::SetDie()
{
	m_pSense->Die();
}

void CAgent::BeAttack(IRole* pRole)
{
	m_pSense->BeAttack(pRole);
}

void CAgent::BeKill(IRole* pRole)
{
	m_pSense->BeKill(pRole);
}

void CAgent::AttackOnce()
{
	m_pFight->AttackOnce();
}

void CAgent::SetLife(int nData)
{
	int nOldLife = m_info.usLife;
	m_info.usLife	= nData;

	if(nData > nOldLife && nData < GetMaxLife())
		m_pSense->Recruit(nData - nOldLife);
}

void CAgent::SetPower(int nData)
{
	m_info.usMana	= nData;
}

void CAgent::SetEffect(DWORD dwEffect)
{
	DWORD	dwOldEffect	= m_info.dwEffect;
	m_info.dwEffect	= dwEffect;

	DWORD	dwEffectInc = dwEffect & (~dwOldEffect);
	DWORD	dwEffectDec = (~dwEffect) & dwOldEffect;
	if(dwEffectInc & KEEPEFFECT_DIE)
		m_pSense->Die();
	else if(dwEffectDec & KEEPEFFECT_DIE)
		m_pSense->Reborn();
}

OBJID CAgent::GetMapID()
{
	CHECKF(m_pMove);
	CGameMap* pMap = m_pMove->GetMap();
	CHECKF(pMap);
	return pMap->GetID();
}

bool CAgent::SendMsg(CNetMsg* pMsg)
{
	return NpcWorld()->SendMsg(GetID(), pMsg);
}

int	 CAgent::Priority2Durable(int nPriority)
{
	IF_NOT(nPriority >= 0 && nPriority <= 100)
		return 100;

	return ::CutTrail(1, ::Priority2Durable(nPriority) * TICKS_PER_SEC);
}

int CAgent::GetMaxLife()
{
	int nMaxLife = (m_info.usForce + m_info.usDexterity + m_info.usHealth*8 + m_info.usSoul) * 3;

	if(GetProfessionSort() == 1)
	{
		int nPercent;
		switch(GetProfessionLevel())
		{
		case 1:
			nPercent = 5;
			break;
		case 2:
			nPercent = 8;
			break;
		case 3:
			nPercent = 10;
			break;
		case 4:
			nPercent = 12;
			break;
		case 5:
			nPercent = 15;
			break;
		default:
			nPercent = 0;
			break;
		}
		nMaxLife += nMaxLife * nPercent / 100;
	}

	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = Cast<CItemPack>(this)->GetEquipItem(i);
		if(pEquip && pEquip->GetInt(ITEMDATA_AMOUNT))
		{
			nMaxLife += pEquip->GetInt(ITEMDATA_LIFE);
		}
	}

	if(nMaxLife < 0)
		return 0;
	return nMaxLife;
}

void CAgent::PrivateTalkToMe(LPCTSTR szSender, LPCTSTR szWords, LPCTSTR szEmotion)
{
	TalkToMe(szSender, szWords, szEmotion);
}

void CAgent::TalkToMe(LPCTSTR szSender, LPCTSTR szWords, LPCTSTR szEmotion)
{
	int		nPriority	= 50;
	String	strFact = FACT_TALK_TO_ME;
	strFact	+= "<\"";
	strFact	+= szSender;
	strFact	+= "\">(\"";
	strFact	+= szWords;
	strFact	+= "\")";

	IAiCenter* pAi = Cast<IAiCenter>(this);
	pAi->AddFact(strFact, nPriority);
}

bool CAgent::Login()
{
	DWORD dwVersion = 0;

	CMsgLogin	msg;
	BOOL	ret = msg.Create(dwVersion, "", "");
	IF_OK_(ret)
		SendMsg(&msg);

	MYASSERT(m_nStep == STEP_OFFLINE);
	m_nStep	= STEP_LOGIN;
	LOGDUMP("Agent Login() OK");
	return true;
}
