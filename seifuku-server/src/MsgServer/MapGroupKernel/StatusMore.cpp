#include "AllMsg.h"
#include "windows.h"
#include "define.h"
#include "I_Role.h"
#include "TimeOut.h"
#include "StatusMore.h"
#include "magic.h"

const int	MAX_POISON_LOSE_LIFE			= 200;

MYHEAP_IMPLEMENTATION(CStatusMore,s_heap)
CStatusMore::CStatusMore()
{
	m_pOwner	= NULL;
	m_nStatus	= 0;
}

CStatusMore::~CStatusMore()
{
	if(m_pOwner)
		m_pOwner->ClsStatus(m_nStatus);
}

bool CStatusMore::GetInfo(StatusInfoStruct* pInfo)
{
	pInfo->nPower	= m_nData;
	pInfo->nStatus	= m_nStatus;
	pInfo->nSecs	= m_tKeep.GetInterval();
	pInfo->nTimes	= m_nTimes;

	return IsValid();
}

void CStatusMore::OnTimer(DWORD tCurr)
{
	if(!IsValid())
		return ;

	if(!m_tKeep.ToNextTime())
		return ;
	m_nTimes--;

	if(m_nStatus == STATUS_POISON)
	{
		if(m_pOwner->IsAlive())
		{
			int nLoseLife = CUser::AdjustData(m_pOwner->GetLife(), m_nData); if(nLoseLife < 0) nLoseLife = 0;
			int nLife = m_pOwner->GetLife(); if(nLoseLife > nLife) nLoseLife = nLife;
			if(nLoseLife > MAX_POISON_LOSE_LIFE) nLoseLife = MAX_POISON_LOSE_LIFE;
			m_pOwner->AddAttrib(_USERATTRIB_LIFE, -1*nLoseLife, SYNCHRO_TRUE, NULL);

			CMsgMagicEffect	msg;
			IF_OK(msg.Create(m_pOwner->GetID(), POISON_MAIGC_TYPE, 0, m_pOwner->GetID(), nLoseLife))
				m_pOwner->BroadcastRoomMsg(&msg, EXCLUDE_SELF);

			if(!m_pOwner->IsAlive())
				m_pOwner->BeKill(NULL, 1);
		}
	}
	m_nCount = m_nCount + 1;
}

bool CStatusMore::ChangeData(int nPower, int nSecs, int nTimes)
{
	CHECKF(nSecs > 0 && nTimes > 0);

	if(nPower)
	{
		if(nTimes > m_nTimes)
		{
			m_nData		= nPower;
			m_tKeep.SetInterval(nSecs);
			m_nTimes	= nTimes;
		}
		return true;
	}
	else
	{
		m_nTimes = ::CutTrail(m_nTimes - nTimes, 0);
	}
	return false;
}
