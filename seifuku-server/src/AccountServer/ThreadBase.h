#ifndef THREADBASE_H
#define THREADBASE_H

#include <afxmt.h>

class CThreadBase
{
protected:
	CThreadBase();

	virtual ~CThreadBase();

public:
	bool	CreateThread(bool bRun = true);

	bool	ResumeThread();

	bool	CloseThread(long nMilliseconds = 0);

protected:
	virtual	void	OnInit() { }
	virtual bool	OnProcess() = 0;
	virtual void	OnDestroy() { }

private:
	bool	IsCloseEvent(long nMilliseconds = 0);
	DWORD	ThreadProc();
	bool	IsCreated() { return (m_hThread != NULL); }

protected:
	HANDLE		m_hThread;
	HANDLE		m_hCloseThreadEvent;
	CCriticalSection		m_xCtrl;
	
private:
	static DWORD WINAPI TrueThreadProc(LPVOID pParam);
};

#endif