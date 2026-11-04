#ifndef THREADBASE_H
#define THREADBASE_H

#include <windows.h>

class CThreadBase
{
public:
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
	
private:
	static DWORD WINAPI TrueThreadProc(LPVOID pParam);
};

#endif