#if !defined(AFX_PLAYERLEAVEWORD_H__6547133F_17B9_4978_93AB_BAA88B109312__INCLUDED_)
#define AFX_PLAYERLEAVEWORD_H__6547133F_17B9_4978_93AB_BAA88B109312__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>
#include "define.h"
#include "TimeOut.h"
#include "Myheap.h"

enum LEAVEWORDDATA {
	LEAVEWORDDATA_ID=0,
	LEAVEWORDDATA_USER=1,
	LEAVEWORDDATA_SENDER,
	LEAVEWORDDATA_TIME,
	LEAVEWORDDATA_WORDS,
};

const int	PLAYER_WORDS_PER_TIME	= 1;
const int	NEW_WORDS_PER_MINUTE	= 5;
const int	DELETE_LEAVEWORD_DAYS	= 31;
const int	SECS_PER_AUTOFLUSH		= 60;

class	CUser;
class CLeaveWord  
{
public:
	CLeaveWord();
	virtual ~CLeaveWord();

public:
	bool	Create			(PROCESS_ID idProcess);
	ULONG	Release			()							{ delete this; return 0; }
	bool	InsertWords		(LPCTSTR szSender, LPCTSTR szRecvUser, LPCTSTR szWords);
	bool	ShowWords		(CUser* pUser, int nMaxNum = PLAYER_WORDS_PER_TIME);
	void	OnTimer			(DWORD tCurr);

protected:
	DWORD			m_idLastMsg;
	CTimeOut		m_tAutoFlush;

protected:
	PROCESS_ID		m_idProcess;

public:
	MYHEAP_DECLARATION(s_heap)
};

#endif
