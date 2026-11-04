#ifndef	MSGQUERYFEE_HA
#define MSGQUERYFEE_HA

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

class CMsgQueryFee_A : public world_kernel::CNetMsg
{
public:
	CMsgQueryFee_A();
	virtual ~CMsgQueryFee_A();

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
