#if !defined(SFK_MYTHREAD_H__INCLUDED_)
#define SFK_MYTHREAD_H__INCLUDED_

#include <windows.h>
#include "MyHeap.h"

class IThreadEvent
{
public:
	virtual int		OnThreadCreate()		=0;
	virtual int		OnThreadDestroy()		=0;
	virtual int		OnThreadWorkEvent()		=0;
	virtual int		OnThreadProcess()		=0;
};

class CMyThread
{
public:
	CMyThread(IThreadEvent* pEvent);
	virtual ~CMyThread();

public:
	static CMyThread*	CreateNew(IThreadEvent* pEvent, int bSuspend, DWORD dwWorkInterval);

	bool	Create	(int bSuspend, DWORD dwWorkInterval);
	void	Resume	();
	bool	Close	();
	void	SetWorkEvent()		{ ::SetEvent(m_hWorkEvent); }

protected:
	static unsigned __stdcall	RunThread	(void* pParam);

protected:
	enum { STATUS_NONE = 0, STATUS_SUSPEND, STATUS_RUNNING, STATUS_CLOSING, STATUS_CLOSED };

	DWORD			m_dwWorkInterval;
	IThreadEvent*	m_pEvent;
	HANDLE			m_hThread;
	HANDLE			m_hCloseEvent;
	HANDLE			m_hWorkEvent;
	unsigned		m_id;
	int				m_nStatus;

public:
	MYHEAP_DECLARATION(s_heap)
};

#endif
