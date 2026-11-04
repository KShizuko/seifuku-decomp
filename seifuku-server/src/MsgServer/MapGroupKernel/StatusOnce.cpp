#include "windows.h"
#include "define.h"
#include "I_Role.h"
#include "TimeOut.h"
#include "myheap.h"
#include "StatusMore.h"
#include "StatusOnce.h"
#include "MapGroup.h"
#include "MsgUserAttrib.h"

MYHEAP_IMPLEMENTATION(CStatusOnce,s_heap)
CStatusOnce::CStatusOnce()
{
	m_pOwner	= NULL;
	m_nStatus	= 0;
}

CStatusOnce::~CStatusOnce()
{
	if(m_pOwner)
		m_pOwner->ClsStatus(m_nStatus);
}
bool CStatusOnce::GetInfo(StatusInfoStruct* pInfo)
{
	pInfo->nPower	= m_nData;
	pInfo->nStatus	= m_nStatus;
	pInfo->nSecs	= m_tKeep.GetRemain();
	pInfo->nTimes	= 0;

	return IsValid();
}

bool CStatusOnce::ChangeData(int nPower, int nSecs, int nTimes)
{
	CHECKF(nSecs > 0 && nTimes == 0);

	if(nPower > 0)
	{
		if(nSecs >= m_tKeep.GetRemain())
		{
			m_tKeep.SetInterval(nSecs);
			m_nData		= nPower;
			m_tKeep.Update();
		}
		return true;
	}
	else if(nPower < 0)
	{
		m_tKeep.SetInterval(m_tKeep.GetRemain() + nSecs);
		m_tKeep.Update();
	}
	else
	{
		m_tKeep.DecInterval(nSecs);
	}
	return false;
}

bool CStatusOnce::IncTime(int nMilliSecs, int nLimit)
{
	m_tKeep.IncInterval(nMilliSecs, nLimit);
	return true;
}

void CStatusOnce::OnTimer(DWORD tCurr)
{
	if (m_nStatus == STATUS_SUPERMAN || m_nStatus == STATUS_TORNADO)
	{
		if (!(m_nCount & 1))
		{
			CMsgUserAttrib	msg;
			msg.Create(m_pOwner->GetID(), 31, m_tKeep.GetRemain());
			m_pOwner->SendMsg(&msg);
		}
	}
	m_nCount = m_nCount + 1;
}
