#include "stdafx.h"
#include "NpcServer.h"
#include "NpcServerDlg.h"
#include "KernelThread.h"
#include "inifile.h"
#include "typedef.h"
#include "protocol.h"
#include <mmsystem.h>

CKernelThread::CKernelThread()
{
	m_pKernel		= NULL;
	m_tNextClock	= 0;
}

CKernelThread::~CKernelThread()
{
}

void CKernelThread::OnInit()
{
	m_tNextClock = timeGetTime();
}

bool CKernelThread::OnProcess()
{
	clock_t	tStart = timeGetTime();
	if(!m_sockClient.IsOpen())
	{
		m_pKernel->SetDisconnect();
		m_pDialog->PrintText("Connect to game server again . . .");
		LOGDUMP("Connect to game server again . . .");
		if(!m_sockClient.Open(CONNECT_DELAY_MS))
			return true;
	}

	const char* pBuf;
	int			nSize;
	while(( pBuf = m_sockClient.GetPacket(&nSize) ))
	{
		if(!nSize)
			break;

		char*		pMsg;
		int			nMsgSize;
		OBJID		idPacket;
		OBJID		idNpc;
		int			nNetPacketSize = SplitNpcPacket(pBuf, nSize, &idPacket, &pMsg, &nMsgSize, &idNpc);
		if(nNetPacketSize)
		{
			DEBUG_TRY
			m_pKernel->ProcessMsg(idPacket, pMsg, nMsgSize, idNpc);
			DEBUG_CATCH("m_pKernel->ProcessMsg()")

			IF_NOT(m_sockClient.ClearPacket(nNetPacketSize))
				break;
		}
		else
			break;
	}

	clock_t	tCurr = timeGetTime();
	if(tCurr >= m_tNextClock)
	{
		m_tNextClock	+= SYSTEM_ONTIMER_MS;

		{
			int nOffset	= (tCurr - m_tNextClock) / CLOCKS_PER_SEC;
			if(abs(nOffset) > TIMER_OFFSET_LIMIT)
			{
				m_tNextClock = tCurr + CLOCKS_PER_SEC;
				LOGWARNING("CKernelThread::OnProcess() clock skew %d seconds, corrected.", nOffset);
			}
		}

		DEBUG_TRY
		m_pKernel->OnTimer(tCurr);
		DEBUG_CATCH("m_pKernel->OnTimer()")
	}

	m_sockClient.Flush();

	{
		extern struct STAT_ST	g_stat;
		int	 tUsed = timeGetTime() - tStart;
		InterlockedIncrement(&g_stat.nKernelCount);
		InterlockedExchangeAdd(&g_stat.nKernelSum, tUsed);
		if(tUsed > g_stat.nKernelMax)
			InterlockedExchange(&g_stat.nKernelMax, tUsed);
	}

	Sleep(50);
	return true;
}

bool CKernelThread::CreateThread(IDialog* pDialog, bool bRun )
{
	m_pDialog	= pDialog;

	CIniFile	ini(CONFIG_INI_FILENAME, "GameServer");
	IPSTR	szIP;
	DWORD	nPort;
	int		nDelay	= CONNECT_DELAY_MS * 3;
	int		nSndSize;
	int		nRcvSize;
	ini.GetString(szIP, "GAMESERVER_IP", IPSTR_SIZE);
	nPort		= ini.GetInt("GAMESERVER_PORT");
	nSndSize	= ini.GetInt("SND_SIZE");
	nRcvSize	= ini.GetInt("RCV_SIZE");

	m_sockClient.Open(szIP, nPort, nDelay, nSndSize, nRcvSize);

	m_pKernel	= ::CreateNewKernel();
	if(!m_pKernel)
		return false;
	if(!m_pKernel->Create(this))
	{
		m_pKernel->Release();
		return false;
	}

	return CThreadBase::CreateThread(bRun);
}

void CKernelThread::OnDestroy()
{
	if(m_pKernel)
	{
		m_pKernel->Release();
		m_pKernel = NULL;
	}

	m_sockClient.Close();
}
