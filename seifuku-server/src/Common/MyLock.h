#ifndef	MYLOCK_H
#define	MYLOCK_H

#include <windows.h>

class CMyCriticalSection
{
public:
	CMyCriticalSection()					{ ::InitializeCriticalSection(&m_xCrit); }
	virtual void	Lock()					{ ::EnterCriticalSection(&m_xCrit); }
	virtual void	Unlock()				{ ::LeaveCriticalSection(&m_xCrit); }
	virtual ~CMyCriticalSection()			{ ::DeleteCriticalSection(&m_xCrit); }

protected:
	CRITICAL_SECTION	m_xCrit;
};

class CMySingleLock
{
public:
	CMySingleLock(CMyCriticalSection* pObj, bool bInitialLock) : m_pObj(pObj)	{ if(bInitialLock) m_pObj->Lock(); }
	~CMySingleLock()						{ m_pObj->Unlock(); }

protected:
	CMyCriticalSection*	m_pObj;
};

#endif
