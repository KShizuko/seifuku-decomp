#ifndef	MSGQUERYFEE_H
#define MSGQUERYFEE_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "OnlineTable.h"

class CMsgQueryFee : public CNetMsg  
{
public:
	CMsgQueryFee();
	virtual ~CMsgQueryFee();

	BOOL	Create	(OBJID idAccount, unsigned char ucType, DWORD dwData, int nTime);

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);

	typedef struct{
		unsigned short	unMsgSize;
		unsigned short	unMsgType;
		OBJID			idAccount;
		DWORD			dwData;
		int				nTime;
		unsigned char	ucType;
	}MSG_Info;
	
	MSG_Info*	m_pInfo;

};

#endif
