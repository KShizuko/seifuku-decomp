#include "stdafx.h"
#include "I_Shell.h"
#include "SocketThread.h"
#include "MessagePort.h"
#include "MsgServer.h"
#include "protocol.h"
#include <mmsystem.h>

CSocketThread::CSocketThread(IMessagePort* pPort)
{
	MYASSERT(pPort);
	m_pMsgPort		= pPort;
	m_pServerSocket	= NULL;
	m_tNextClock	= 0;

	m_hMutexThread	= NULL;
}

CSocketThread::~CSocketThread()
{
}

bool CSocketThread::CreateThread(bool bRun )
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

	m_pServerSocket	= ISocketKernel::CreateNew();
	if(!m_pServerSocket->Create(m_pMsgPort))
	{
		::MessageBox(NULL, "Server Socket create failed.", "Conquer Online Server", MB_ICONERROR);
		return false;
	}
	return true;
}

void CSocketThread::OnDestroy()
{
	if(m_pServerSocket)
		m_pServerSocket->Release();
	m_pServerSocket = NULL;

#ifdef	DEBUG_MULTITHREAD
	if(m_hMutexThread)
	{
		::CloseHandle(m_hMutexThread);
		m_hMutexThread = NULL;
	}
#endif
}

void CSocketThread::OnInit()
{
	m_tNextClock = timeGetTime() + CLOCKS_PER_SEC;
	char	szText[1024];
	sprintf(szText, "#%u: Socket kernel thread running.", m_pMsgPort->GetID());
	m_pMsgPort->Send(MSGPORT_SHELL, SHELL_PRINTTEXT, STRING_TYPE(szText), szText);
}

bool CSocketThread::OnProcess()
{
#ifdef	DEBUG_MULTITHREAD
	if(::WaitForSingleObject(m_hMutexThread, INFINITE) == WAIT_ABANDONED)
		return false;
#endif
	char	buf[MAX_MESSAGESIZE];
	CMessageStatus	cStatus;
	clock_t	clkStart = timeGetTime();

	clock_t	clkDebug = timeGetTime();
	DEBUG_TRY
	while(m_pMsgPort->Recv(PORT_ANY, PACKET_ANY, STRUCT_TYPE(buf), buf, &cStatus))
	{
		clock_t	clkDebug = timeGetTime();
		if(!m_pServerSocket->ProcessMsg(cStatus.m_nPacket, buf, cStatus.m_nVarType, cStatus.m_nPortFrom))
		{
			LOGERROR("CSocketThread internal message error");
		}
		{
			extern struct STAT_STRUCT	g_stat;
			InterlockedExchangeAdd(&g_stat.setDebug[1], timeGetTime() - clkDebug);
		}
	}
	DEBUG_CATCH("CMapGroupThread::OnProcess(msg)")
	{
		extern struct STAT_STRUCT	g_stat;
		InterlockedExchangeAdd(&g_stat.setDebug[2], timeGetTime() - clkDebug);
		clkDebug = timeGetTime();
	}

	clock_t	tCurr = timeGetTime();
	if(tCurr >= m_tNextClock)
	{
		m_tNextClock	+= SOCKETKERNEL_ONTIMER_MS;

		{
			int nOffset	= (tCurr - m_tNextClock) / CLOCKS_PER_SEC;
			if(abs(nOffset) > TIMER_OFFSET_LIMIT)
			{
				m_tNextClock = tCurr + SOCKETKERNEL_ONTIMER_MS;
				LOGWARNING("CSocketThread::OnProcess() clock skew %d seconds, corrected.", nOffset);
			}
		}

		DEBUG_TRY
		m_pServerSocket->OnTimer(tCurr);
		DEBUG_CATCH("m_pServerSocket->OnTimer()")
	}

	clkDebug = timeGetTime();
	DEBUG_TRY
	if(m_tProcessSocket.ToNextTime(SOCKET_PROCESS_INTERVAL))
	{
		if(!m_pServerSocket->ProcessSocket())
		{
			LOGERROR("CSocketThread SOCKET processing error");
		}
	}
	DEBUG_CATCH("CMapGroupThread::OnProcess(socket)")
	{
		extern struct STAT_STRUCT	g_stat;
	}
	{
		extern struct STAT_STRUCT	g_stat;
		InterlockedExchangeAdd(&g_stat.setDebug[4], timeGetTime() - clkStart);
	}
#ifdef	DEBUG_MULTITHREAD
	::ReleaseMutex(m_hMutexThread);
#endif
	clock_t	 clkLeave	= SOCKET_PROCESS_INTERVAL - (timeGetTime() - clkStart);
	if(clkLeave < 0 || clkLeave > SOCKET_PROCESS_INTERVAL)
		clkLeave = 0;
	{
		clock_t	 clkUsed	= timeGetTime() - clkStart;
		extern struct STAT_STRUCT	g_stat;
		InterlockedExchangeAdd(&g_stat.nAllSocketMS, clkUsed);
		InterlockedIncrement(&g_stat.nSocketTimes);
		if(g_stat.nMaxSocketMS < clkUsed)
		{
			g_stat.nMaxSocketMS = clkUsed;
		}
	}
	DEBUG_TRY
	m_pMsgPort->WaitMsg(clkLeave);
	DEBUG_CATCH("CMapGroupThread::OnProcess(wait msg)")

	return true;
}
