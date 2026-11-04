#include "stdafx.h"
#include "MessagePort.h"
#include "MsgServer.h"
#include "I_Shell.h"
#include "WorldThread.h"
#include "protocol.h"
#include <mmsystem.h>

CWorldThread::CWorldThread(IMessagePort* pPort)
{
	MYASSERT(pPort);
	m_pMsgPort		= pPort;
	m_pWorld		= NULL;
	m_tNextClock	= 0;
	m_hMutexThread	= NULL;
}

CWorldThread::~CWorldThread()
{
}

bool CWorldThread::CreateThread(bool bRun )
{
#ifdef	DEBUG_MULTITHREAD
	m_hMutexThread    =::CreateMutex(NULL, false, "FW_DEBUG_MULTITHREAD");
	if(!m_hMutexThread)
	{
		LOGERROR("Cannot create mutex handle!");
		return false;
	}
#endif
	if(!CThreadBase::CreateThread(bRun))
		return false;

	m_pWorld	= IWorld::CreateNew();
	return m_pWorld->Create(m_pMsgPort);
}

void CWorldThread::OnDestroy()
{
	if(m_pWorld)
		m_pWorld->Release();
	m_pWorld = NULL;

#ifdef	DEBUG_MULTITHREAD
	if(m_hMutexThread)
	{
		::CloseHandle(m_hMutexThread);
		m_hMutexThread = NULL;
	}
#endif
}

void CWorldThread::OnInit()	
{ 
	m_tNextClock = timeGetTime() + CLOCKS_PER_SEC;
	char	szText[1024];
	sprintf(szText, "#%u: World kernel thread running.", m_pMsgPort->GetID());
	m_pMsgPort->Send(MSGPORT_SHELL, SHELL_PRINTTEXT, STRING_TYPE(szText), szText);
}

bool CWorldThread::OnProcess()
{
#ifdef	DEBUG_MULTITHREAD
	if(::WaitForSingleObject(m_hMutexThread, INFINITE) == WAIT_ABANDONED)
		return false;
#endif
	char	buf[MAX_MESSAGESIZE];
	CMessageStatus	cStatus;
	clock_t	clkStart = timeGetTime();

	DEBUG_TRY
	if(m_pMsgPort->Recv(PORT_ANY, PACKET_ANY, STRUCT_TYPE(buf), buf, &cStatus))
	{
		if(!m_pWorld->ProcessMsg(cStatus.m_nPacket, buf, cStatus.m_nVarType, cStatus.m_nPortFrom))
		{
			LOGERROR("m_pWorld->ProcessMsg internal message error");
		}
	}
	DEBUG_CATCH("CWorldThread::OnProcess()")

	clock_t	tCurr = timeGetTime();
	if(tCurr >= m_tNextClock)
	{
		m_tNextClock	+= WORLDKERNEL_ONTIMER_MS;

		{
			int nOffset	= (tCurr - m_tNextClock) / CLOCKS_PER_SEC;
			if(abs(nOffset) > TIMER_OFFSET_LIMIT)
			{
				m_tNextClock = tCurr + WORLDKERNEL_ONTIMER_MS;
				LOGWARNING("CWorldThread::OnProcess() clock skew %d seconds, corrected.", nOffset);
			}
		}

		DEBUG_TRY
		m_pWorld->OnTimer(tCurr);
		DEBUG_CATCH("m_pWorld->OnTimer()")
	}

#ifdef	DEBUG_MULTITHREAD
	::ReleaseMutex(m_hMutexThread);
#endif
	{
		clock_t	 clkUsed	= timeGetTime() - clkStart;
		extern struct STAT_STRUCT	g_stat;
		InterlockedExchangeAdd(&g_stat.nAllWorldMS, clkUsed);
		InterlockedIncrement(&g_stat.nWorldTimes);
		if(g_stat.nMaxWorldMS < clkUsed)
			g_stat.nMaxWorldMS = clkUsed;
	}
	DEBUG_TRY
	m_pMsgPort->WaitMsg(__min(THREADDELAY_MS, WORLDKERNEL_ONTIMER_MS));
	DEBUG_CATCH("CMapGroupThread::OnProcess(2)")

	return true;
}
