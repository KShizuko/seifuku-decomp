#if !defined(AFX_TIMEOUT_H__059AED43_D21C_4072_A172_20A7D8B6DF52__INCLUDED_)
#define AFX_TIMEOUT_H__059AED43_D21C_4072_A172_20A7D8B6DF52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include <time.h>
#include "BaseFunc.h"

template<int I>
class CTimeOut_
{
public:
	CTimeOut_(int nIntervalSecs = I) { m_nInterval = nIntervalSecs; m_tUpdateTime = 0; }

public:
	void	Update		()						{ m_tUpdateTime = (::TimeGet()/1000); }
	bool	IsTimeOut	()						{ return (::TimeGet()/1000) >= m_tUpdateTime+m_nInterval; }
	bool	ToNextTime	()						{ if(IsTimeOut()) return Update(),true; else return false; }
	void	SetInterval	(int nSecs)				{ m_nInterval = nSecs; }

	void	Startup		(int nSecs)				{ m_nInterval = nSecs; Update(); }
	bool	TimeOver	()						{ if(IsActive() && IsTimeOut()) return Clear(),true; return false; }
	void	Clear		()						{ m_nInterval = m_tUpdateTime = 0; }
	bool	IsActive	()						{ return m_tUpdateTime != 0; }
	void	IncInterval	(int nSecs, int nLimit)	{ m_nInterval = ::CutOverflow(m_nInterval+nSecs, nLimit); }
	void	DecInterval	(int nSecs)				{ m_nInterval = ::CutTrail(m_nInterval-nSecs, 0); }

	bool	IsTimeOut	(int nSecs)				{ return (::TimeGet()/1000) >= m_tUpdateTime+nSecs; }
	bool	ToNextTime	(int nSecs)				{ if(IsTimeOut(nSecs)) return Update(),true; else return false; }
	bool	TimeOver	(int nSecs)					{ if(IsActive() && IsTimeOut(nSecs)) return Clear(),true; return false; }
	bool	ToNextTick	(int nSecs);

public:
	int		GetRemain()							{ return m_tUpdateTime ? (int)::CutOverflow((DWORD)(m_nInterval - ((::TimeGet()/1000)-(DWORD)m_tUpdateTime)), (DWORD)m_nInterval) : 0; }
	int		GetInterval()						{ return m_nInterval; }

protected:
	int		m_nInterval;
	time_t	m_tUpdateTime;
};
typedef	CTimeOut_<0>	CTimeOut;

template<int I>
class CTimeOutMS_
{
public:
	CTimeOutMS_(int nIntervalSecs = I) { m_nInterval = nIntervalSecs; m_tUpdateTime = 0; }

public:
	void	Update		()						{ m_tUpdateTime = ::TimeGet(); }
	bool	IsTimeOut	()						{ return ::TimeGet() >= m_tUpdateTime+m_nInterval; }
	bool	ToNextTime	()						{ if(IsTimeOut()) return Update(),true; else return false; }
	void	SetInterval	(int nMilliSecs)		{ m_nInterval = nMilliSecs; }

	void	Startup		(int nMilliSecs)		{ m_nInterval = nMilliSecs; Update(); }
	bool	TimeOver	()						{ if(IsActive() && IsTimeOut()) return Clear(),true; return false; }
	void	Clear		()						{ m_nInterval = m_tUpdateTime = 0; }
	bool	IsActive	()						{ return m_tUpdateTime != 0; }
	void	IncInterval	(int nMilliSecs, int nLimit)		{ m_nInterval = ::CutOverflow(m_nInterval+nMilliSecs, nLimit); }
	void	DecInterval	(int nMilliSecs)		{ m_nInterval = ::CutTrail(m_nInterval-nMilliSecs, 0); }

	bool	IsTimeOut	(int nMilliSecs)		{ return ::TimeGet() >= m_tUpdateTime+nMilliSecs; }
	bool	ToNextTime	(int nMilliSecs)		{ if(IsTimeOut(nMilliSecs)) return Update(),true; else return false; }
	bool	TimeOver	(int nMilliSecs)		{ if(IsActive() && IsTimeOut(nMilliSecs)) return Clear(),true; return false; }
	bool	ToNextTick	(int nMilliSecs);

public:
	int		GetRemain()							{ return m_tUpdateTime ? ::CutRange(m_nInterval - ((int)::TimeGet()-(int)m_tUpdateTime), 0, m_nInterval) : 0; }
	int		GetInterval()						{ return m_nInterval; }

protected:
	int		m_nInterval;
	clock_t	m_tUpdateTime;
};
typedef	CTimeOutMS_<0>	CTimeOutMS;

#endif
