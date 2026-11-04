#if !defined(SFK_ADAPTER_H_INCLUDED)
#define SFK_ADAPTER_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>
#include <map>
#include "I_Adapter.h"

class CUser;

class CAdapter : public IAdapter, public IRegister
{
public:
	CAdapter(PROCESS_ID idProcess);
	virtual ~CAdapter();

public:
	virtual void*	CreateActor		(LPCTSTR szName);
	virtual void*	Find			(int idModule, int nUnknown);
	virtual void*	GetActor		(int idModule, void* pBuf);

public:
	virtual bool	Init				();
	virtual ILotteryActor*	CreateLotteryActor	(CUser* pUser);
	virtual bool	SendMsg				(SOCKET_ID idSocket, OBJID idMsg, const char* pBuf, DWORD dwSize);

public:
	virtual bool	Register		(int idModule, void* pModule);

protected:
	HMODULE			m_hCommon;
	PROCESS_ID		m_idProcess;

	typedef std::map<int, void*>	MODULE_SET;
	MODULE_SET		m_setModule;
};

IAdapter*	AdapterCreate(PROCESS_ID idProcess);

#endif
