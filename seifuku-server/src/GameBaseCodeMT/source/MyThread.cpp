#include <windows.h>
#include <mmsystem.h>
#include <process.h>
#include "MyThread.h"

MYHEAP_IMPLEMENTATION(CMyThread,s_heap)

CMyThread::CMyThread(IThreadEvent* pEvent)
{
	m_pEvent			= pEvent;
	m_hThread			= NULL;
	m_hCloseEvent		= NULL;
	m_hWorkEvent		= NULL;
	m_id				= 0;
	m_dwWorkInterval	= 0;
	m_nStatus			= STATUS_NONE;
}

CMyThread::~CMyThread()
{
	if (m_nStatus == STATUS_NONE)
		return;

	if (m_nStatus == STATUS_SUSPEND)
		this->Resume();

	if (m_nStatus != STATUS_CLOSING)
		this->Close();

	if (WAIT_TIMEOUT == ::WaitForSingleObject(m_hThread, 3000))
		::TerminateThread(m_hThread, 4444);

	if (m_hThread)
	{
		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if (m_hWorkEvent)
	{
		::CloseHandle(m_hWorkEvent);
		m_hWorkEvent = NULL;
	}

	if (m_hCloseEvent)
	{
		::CloseHandle(m_hCloseEvent);
		m_hCloseEvent = NULL;
	}

	m_id = 0;
}

CMyThread* CMyThread::CreateNew(IThreadEvent* pEvent, int bSuspend, DWORD dwWorkInterval)
{
	CMyThread* pThread = new CMyThread(pEvent);
	if (!pThread)
		return NULL;

	if (!pThread->Create(bSuspend, dwWorkInterval))
	{
		delete pThread;
		return NULL;
	}

	return pThread;
}

bool CMyThread::Create(int bSuspend, DWORD dwWorkInterval)
{
	if (m_nStatus != STATUS_NONE)
		return false;

	m_hCloseEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!m_hCloseEvent)
		return false;

	m_hWorkEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!m_hWorkEvent)
		return false;

	DWORD dwCreationFlags = 0;
	if (bSuspend)
		dwCreationFlags = CREATE_SUSPENDED;

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, RunThread, this, dwCreationFlags, &m_id);
	if (!m_hThread)
	{
		::CloseHandle(m_hCloseEvent);
		m_hCloseEvent = NULL;
		::CloseHandle(m_hWorkEvent);
		m_hWorkEvent = NULL;
		return false;
	}

	m_dwWorkInterval = dwWorkInterval;
	m_nStatus = bSuspend ? STATUS_SUSPEND : STATUS_RUNNING;
	return true;
}

void CMyThread::Resume()
{
	if (m_nStatus == STATUS_SUSPEND && ::ResumeThread(m_hThread) != (DWORD)-1)
		m_nStatus = STATUS_RUNNING;
}

bool CMyThread::Close()
{
	if (::SetEvent(m_hCloseEvent))
	{
		m_nStatus = STATUS_CLOSING;
		return true;
	}

	return false;
}

unsigned __stdcall CMyThread::RunThread(void* pParam)
{
	CMyThread* pThis = (CMyThread*)pParam;
	if (!pThis)
		return 1;

	if (pThis->m_pEvent->OnThreadCreate() == -1)
		return 2;

	HANDLE hEvents[2] = { pThis->m_hCloseEvent, pThis->m_hWorkEvent };
	DWORD dwCost = 0;
	while (true)
	{
		DWORD dwTimeout = pThis->m_dwWorkInterval;
		if (dwTimeout != INFINITE)
		{
			if (dwTimeout > dwCost)
				dwTimeout -= dwCost;
			else
				dwTimeout = 0;
		}

		DWORD rval = ::WaitForMultipleObjects(2, hEvents, FALSE, dwTimeout);
		DWORD dwWake = ::timeGetTime();
		if (rval == WAIT_OBJECT_0)
			break;
		else if (rval == WAIT_OBJECT_0 + 1)
		{
			if (pThis->m_pEvent->OnThreadWorkEvent() == -1)
				break;
		}
		else if (rval == WAIT_TIMEOUT)
		{
			if (pThis->m_pEvent->OnThreadProcess() == -1)
				break;
		}
		else
			break;

		dwCost = ::timeGetTime() - dwWake;
	}

	pThis->m_nStatus = STATUS_CLOSED;
	return pThis->m_pEvent->OnThreadDestroy();
}
