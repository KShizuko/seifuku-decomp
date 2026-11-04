#if !defined(AFX_MSGTIME_H__34A26FCD_99B1_4B9B_B3FA_472A2B21796A__INCLUDED_)
#define AFX_MSGTIME_H__34A26FCD_99B1_4B9B_B3FA_472A2B21796A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

class CMsgTime : public CNetMsg    
{
	friend struct MsgTimeProbe;
public:
	CMsgTime();
	virtual ~CMsgTime();
	
public:
	BOOL	Create	(char* pbufMsg, DWORD dwMsgSize);
	void	Process	(void* pInfo);
	
private:
	typedef struct {
		unsigned short	unMsgSize;
		unsigned short	unMsgType;
		UCHAR			ucHour;
	}MSG_Info;
	MSG_Info* m_pInfo;
};

#endif
