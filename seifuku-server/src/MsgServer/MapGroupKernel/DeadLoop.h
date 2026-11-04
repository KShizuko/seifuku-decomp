#ifndef	DEADLOOP_H
#define DEADLOOP_H

#include "common.h"
#include "MyStack.h"
#include "Array.h"
#include "MyLock.h"
#include <mmsystem.h>
#include "TimeOut.h"

class CDeadLoop
{
public:
	static void	Dump(const char* pszText, int nMs)
	{
		CMySingleLock xLock(&s_xDump, true);
		s_nDumpCount++;
		if(s_tDump.ToNextTime(60))
			s_nDumpLeft	= 10;
		if(s_nDumpLeft--)
			LOGERROR("DeadLoopDump[%s], ms[%d], count[%d]", pszText, nMs, s_nDumpCount);
	}
public:
	static CMyCriticalSection	s_xCtrl;
	static CMyCriticalSection	s_xDump;
	static CTimeOut			s_tDump;
	static int				s_nDumpLeft;
	static int				s_nDumpCount;
};

template	< TYPENAME T, TYPENAME S >
class CAutoPop
{
public:
	CAutoPop(T& set, const S& obj)		{ m_pSet = &set; { CMySingleLock xLock(&CDeadLoop::s_xCtrl, true); m_pSet->push(obj); } m_tStart = ::timeGetTime(); }
	~CAutoPop()							{ if(m_pSet->size()) { S obj; { CMySingleLock xLock(&CDeadLoop::s_xCtrl, true); obj = m_pSet->pop(); } int nMs = ::timeGetTime() - m_tStart; if(nMs > 1000) CDeadLoop::Dump(obj, nMs); } }
protected:
	T*		m_pSet;
	DWORD	m_tStart;
};

typedef	CMyStack<const char*>	DEADLOOP;
typedef	Array<DEADLOOP>		DEADLOOP_SET;
extern DEADLOOP_SET	g_setDeadLoop;
inline DEADLOOP_SET&	DeadLoopSet() { return g_setDeadLoop; }

typedef	CAutoPop<DEADLOOP,const char*>		AUTO_POP;

#define	DEADLOOP_CHECK(p,s)	AUTO_POP	xAutoPop(DeadLoopSet()[(p)], (s));

#endif
