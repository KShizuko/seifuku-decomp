#include "windows.h"
#include "define.h"
#include "BaseFunc.h"
#include "ClientDetector.h"
#include "User.h"

CClientDetector::CClientDetector(CUser* pUser, int nType, int nInterval, int nTimeout)
	: m_bFirst(true), m_nType(nType), m_nInterval(nInterval), m_nTimeout(nTimeout), m_pUser(pUser)
{
	m_tDetect.Startup(m_nInterval);
}

void CClientDetector::OnTimer()
{
	if((m_bFirst && ::RandGet(m_tDetect.GetInterval() / 50) == 0) || m_tDetect.TimeOver())
	{
		if(m_bFirst)
		{
			m_bFirst = false;
			m_tDetect.Clear();
		}

		if(m_pUser->IsVerifyEnable(m_nType))
		{
			m_pUser->SendDetectMsg(m_nType);
			m_tWait.Startup(m_nTimeout);
		}
		else
			m_tDetect.Startup(m_nInterval);
	}

	if(m_tWait.TimeOver())
	{
		m_pUser->VerifyDataFailed(m_nType);
		m_tDetect.Startup(m_nInterval);
	}
}

void CClientDetector::OnMatched()
{
	m_tWait.Clear();
	m_tDetect.Startup(m_nInterval);
}

CClientDetector* CClientDetector::CreateNew(CUser* pUser, int nType, int nInterval, int nTimeout)
{
	CClientDetector* pDetector = new CClientDetector(pUser, nType, nInterval, nTimeout);
	IF_NOT(pDetector)
		return NULL;

	return pDetector;
}
