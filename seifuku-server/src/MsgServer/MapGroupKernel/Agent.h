#if !defined(AFX_AGENT_H__F52C1E05_69FA_4094_A1DE_CCCCE4F4FF51__INCLUDED_)
#define AFX_AGENT_H__F52C1E05_69FA_4094_A1DE_CCCCE4F4FF51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "User.h"
#include "mycom.h"

class CAgent : public CUser  
{
	COM_STYLE(CAgent)
protected:
	CAgent();
	virtual ~CAgent();

public:
	bool Create(PROCESS_ID idProcess, OBJID idAgent);

public:
	bool FullItem(int nItemType);
	virtual bool SendMsg(CNetMsg* pMsg);
	virtual bool IsAgent()									{ return true; }
	virtual CAgent*	QueryAgent		()						{ return this; }
	 bool LeaveMapGroup	();

protected:
	MYHEAP_DECLARATION(s_heap)
};

#endif
