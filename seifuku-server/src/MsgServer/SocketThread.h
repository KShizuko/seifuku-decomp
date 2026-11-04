#if !defined(AFX_SOCKETTHREAD_H__FB24259F_D321_441C_A6C7_33970E96B6C3__INCLUDED_)
#define AFX_SOCKETTHREAD_H__FB24259F_D321_441C_A6C7_33970E96B6C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ThreadBase.h"
#include "TimeOut.h"

class IMessagePort;
class CSocketThread : public CThreadBase  
{
public:
	CSocketThread(IMessagePort* pPort);
	virtual ~CSocketThread();

public:
	virtual	bool	CreateThread(bool bRun = true);

protected:
	virtual	void	OnInit();
	virtual bool	OnProcess();
	virtual void	OnDestroy();

protected:
	CTimeOutMS		m_tProcessSocket;
	clock_t m_tNextClock;
	ISocketKernel*	m_pServerSocket;
	IMessagePort*	m_pMsgPort;

protected:
	HANDLE	m_hMutexThread;
};

#endif
