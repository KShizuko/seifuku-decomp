#ifndef	MSGFEE_H
#define MSGFEE_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

class CMsgFee : public CNetMsg  
{
public:
	CMsgFee();
	virtual ~CMsgFee();

	BOOL	Create	(OBJID idAccount, unsigned char ucType);

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);

	enum {FEE_BEGIN=0, FEE_END, FEE_POINT, HEARTBEAT, FEE_KICKOUT, 
				FEE_OFFLINE, SERVER_BUSY, SERVER_FULL, SERVER_CLOSE, FEE_TICK };
	typedef struct{
		unsigned short	unMsgSize;
		unsigned short	unMsgType;
		OBJID			idAccount;
		unsigned char	ucType;
	}MSG_Info;

	MSG_Info*	m_pInfo;

};

#endif
