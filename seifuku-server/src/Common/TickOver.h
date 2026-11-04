#if !defined(AFX_TICKOVER_H__059AED43_D21C_4072_A172_20A7D8B6DF52__INCLUDED_)
#define AFX_TICKOVER_H__059AED43_D21C_4072_A172_20A7D8B6DF52__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif
#include "BaseFunc.h"
class CTickOver  
{
public:
	CTickOver() { m_tTickEnd = 0; m_tTickBegin = 0; }
public:
	void	Startup		(int nNow, int nDur)	{ MYASSERT(nDur > 0); m_tTickBegin = nNow; m_tTickEnd = nNow+nDur; }
	bool	IsTickOver	(int nNow)	const		{ return nNow >= m_tTickEnd; }
	bool	ToNextTime	(int nNow)				{ if(!IsTickOver(nNow)) return false; int nDur=m_tTickEnd-m_tTickBegin; m_tTickBegin+=nDur; m_tTickEnd+=nDur; return true; }

public:
	int		GetBegin()				const		{ return m_tTickBegin; }
	int		GetEnd()				const		{ return m_tTickEnd; }
	int		GetDurable()			const		{ return m_tTickEnd-m_tTickBegin; }
	int		GetDurable(int nNow)	const		{ return m_tTickEnd-nNow; }

protected:
	int		m_tTickBegin;
	int		m_tTickEnd;
};

#endif
