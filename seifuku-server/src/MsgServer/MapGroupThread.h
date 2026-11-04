#if !defined(AFX_MAPGROUPTHREAD_H__946D8B82_F18C_4C03_9985_C5EFD89B9969__INCLUDED_)
#define AFX_MAPGROUPTHREAD_H__946D8B82_F18C_4C03_9985_C5EFD89B9969__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ThreadBase.h"

class IMessagePort;
class CMapGroupThread : public CThreadBase  
{
public:
	CMapGroupThread(IMessagePort* pPort);
	virtual ~CMapGroupThread();

public:
	virtual	bool	CreateThread(bool bRun = true);

protected:
	virtual	void	OnInit();
	virtual bool	OnProcess();
	virtual void	OnDestroy();

protected:
	clock_t m_tNextClock;
	IMapGroup*		m_pMapGroup;
	IMessagePort*	m_pMsgPort;

	HANDLE	m_hMutexThread;
};

#endif
