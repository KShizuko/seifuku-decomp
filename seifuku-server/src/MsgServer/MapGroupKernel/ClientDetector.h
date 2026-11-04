#if !defined(AFX_CLIENTDETECTOR_H__SFK_2008_INCLUDED_)
#define AFX_CLIENTDETECTOR_H__SFK_2008_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "TimeOut.h"

class CUser;

class CClientDetector
{
public:
	CClientDetector(CUser* pUser, int nType, int nInterval, int nTimeout);
	virtual ~CClientDetector()		{}
	static CClientDetector*	CreateNew	(CUser* pUser, int nType, int nInterval, int nTimeout);

public:
	void	OnTimer		();
	void	OnMatched	();

protected:
	BOOL		m_bFirst;
	int			m_nType;
	int			m_nInterval;
	int			m_nTimeout;
	CTimeOut	m_tDetect;
	CTimeOut	m_tWait;
	CUser*		m_pUser;
};

#endif
