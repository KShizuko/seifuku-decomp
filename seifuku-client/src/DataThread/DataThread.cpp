#include <windows.h>
#include <vector>
#include "DataThread.h"

class CDataThread : public IDataThread
{
public:
	CDataThread ();
	virtual ~CDataThread ();

	virtual int		Release ();
	virtual void	SetPriority ( int nPriority );
	virtual void	SetSync ( bool bSync );
	virtual IDataThreadItem*	PopFinished ();
	virtual IDataThreadItem*	Add ( IDataThreadItem* pItem );
	virtual int		IsIdle ();
	virtual int		Commit ();

private:
	HANDLE	m_hThread;
	bool	m_bSync;
};
static_assert ( sizeof ( CDataThread ) == 0xC, "");

static CRITICAL_SECTION	s_csLock;
struct CDataThreadLockInit
{
	CDataThreadLockInit ()	{ InitializeCriticalSection ( &s_csLock ); }
	~CDataThreadLockInit ()	{ DeleteCriticalSection ( &s_csLock ); }
};
static CDataThreadLockInit				s_LockInit;
static std::vector<IDataThreadItem*>	s_vecFinished;
static std::vector<IDataThreadItem*>	s_vecWork;
static std::vector<IDataThreadItem*>	s_vecPending;
static HANDLE	s_hEventWork	= NULL;
static HANDLE	s_hEventQuit	= NULL;
static int		s_nIdle			= 0;

class CDataThreadLock
{
public:
	CDataThreadLock ( CRITICAL_SECTION* pcs ) : m_pcs ( pcs )	{ EnterCriticalSection ( m_pcs ); }
	~CDataThreadLock ()											{ LeaveCriticalSection ( m_pcs ); }
private:
	CRITICAL_SECTION*	m_pcs;
};

static DWORD WINAPI DataThreadProc ( LPVOID lpParam )
{
	HANDLE	hEvents[2];
	hEvents[0] = s_hEventWork;
	hEvents[1] = s_hEventQuit;
	DWORD dwWait = WaitForMultipleObjects ( 2, hEvents, FALSE, INFINITE );
	if ( dwWait != WAIT_OBJECT_0 )
	{
		s_nIdle = 1;
		return dwWait;
	}
	while ( true )
	{
		s_nIdle = 0;
		while ( s_vecWork.size () > 0 )
		{
			CDataThreadLock lock ( &s_csLock );
			IDataThreadItem* pItem = s_vecWork.front ();
			s_vecWork.erase ( s_vecWork.begin () );
			pItem->Process ();
			s_vecFinished.push_back ( pItem );
		}
		s_nIdle = 1;
		dwWait = WaitForMultipleObjects ( 2, hEvents, FALSE, INFINITE );
		if ( dwWait != WAIT_OBJECT_0 )
			break;
	}
	s_nIdle = 1;
	return dwWait;
}

CDataThread::CDataThread ()
{
	DWORD	dwThreadId = 0;
	m_hThread = NULL;
	s_hEventWork = CreateEventA ( NULL, FALSE, FALSE, NULL );
	if ( !s_hEventWork )
		return;
	s_hEventQuit = CreateEventA ( NULL, FALSE, FALSE, NULL );
	if ( !s_hEventQuit )
		return;
	s_nIdle = 1;
	m_bSync = false;
	m_hThread = CreateThread ( NULL, 0, DataThreadProc, this, CREATE_SUSPENDED, &dwThreadId );
	ResumeThread ( m_hThread );
	SetThreadPriority ( m_hThread, THREAD_PRIORITY_ABOVE_NORMAL );
}

CDataThread::~CDataThread ()
{
	if ( m_hThread )
	{
		SetEvent ( s_hEventQuit );
		Sleep ( 100 );
		CloseHandle ( m_hThread );
		m_hThread = NULL;
	}
	if ( s_hEventWork )
	{
		CloseHandle ( s_hEventWork );
		s_hEventWork = NULL;
	}
	if ( s_hEventQuit )
	{
		CloseHandle ( s_hEventQuit );
		s_hEventQuit = NULL;
	}
	std::vector<IDataThreadItem*>::iterator it;
	for ( it = s_vecWork.begin (); it != s_vecWork.begin (); it++ )
		(*it)->Destroy ();
	for ( it = s_vecPending.begin (); it != s_vecPending.begin (); it++ )
		(*it)->Destroy ();
	for ( it = s_vecFinished.begin (); it != s_vecFinished.begin (); it++ )
		(*it)->Destroy ();
	CloseHandle ( m_hThread );
}

int CDataThread::Release ()
{
	delete this;
	return 0;
}

void CDataThread::SetPriority ( int nPriority )
{
	if ( m_hThread )
		SetThreadPriority ( m_hThread, nPriority );
}

void CDataThread::SetSync ( bool bSync )
{
	m_bSync = bSync;
}

int CDataThread::IsIdle ()
{
	return s_nIdle;
}

IDataThreadItem* CDataThread::PopFinished ()
{
	if ( !IsIdle () )
		return NULL;
	EnterCriticalSection ( &s_csLock );
	IDataThreadItem* pItem = NULL;
	if ( s_vecFinished.size () > 0 )
	{
		pItem = s_vecFinished.front ();
		s_vecFinished.erase ( s_vecFinished.begin () );
	}
	LeaveCriticalSection ( &s_csLock );
	return pItem;
}

IDataThreadItem* CDataThread::Add ( IDataThreadItem* pItem )
{
	CDataThreadLock lock ( &s_csLock );
	if ( pItem->IsImmediate () != 1 && !m_bSync )
	{
		s_vecPending.push_back ( pItem );
		return NULL;
	}
	pItem->Process ();
	return pItem;
}

int CDataThread::Commit ()
{
	if ( !IsIdle () )
		return 0;
	if ( s_vecPending.size () > 0 )
	{
		CDataThreadLock lock ( &s_csLock );
		bool bMoved = false;
		std::vector<IDataThreadItem*>::iterator it;
		for ( it = s_vecPending.begin (); it != s_vecPending.end (); it++ )
		{
			bMoved = true;
			s_vecWork.push_back ( *it );
		}
		s_vecPending.clear ();
		if ( bMoved )
			SetEvent ( s_hEventWork );
	}
	return s_vecFinished.size () > 0 ? 1 : 0;
}

DATATHREAD_API
IDataThread* DataThreadCreateNew ()
{
	return new CDataThread;
}
