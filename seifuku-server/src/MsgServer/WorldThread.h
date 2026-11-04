#if !defined(AFX_WORLDTHREAD_H__F8D42B39_4635_4C70_8EE1_36B8B497546F__INCLUDED_)
#define AFX_WORLDTHREAD_H__F8D42B39_4635_4C70_8EE1_36B8B497546F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ThreadBase.h"

class IMessagePort;
class CWorldThread : public CThreadBase  
{
public:
	CWorldThread(IMessagePort* pPort);
	virtual ~CWorldThread();

public:
	virtual	bool	CreateThread(bool bRun = true);

protected:
	virtual	void	OnInit();
	virtual bool	OnProcess();
	virtual void	OnDestroy();

protected:
	clock_t m_tNextClock;
	IWorld*			m_pWorld;
	IMessagePort*	m_pMsgPort;

	HANDLE	m_hMutexThread;
};

#endif
