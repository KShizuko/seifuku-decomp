#ifndef _MSG_LOGINREPLY_H
#define _MSG_LOGINREPLY_H
#include "NetMsg.h"

class CMsgLoginReply : public CNetMsg  
{
public:
	CMsgLoginReply();
	virtual ~CMsgLoginReply();

public:		
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

protected:
	typedef struct {
		unsigned short	unMsgSize;
		unsigned short	unMsgType;
		OBJID			idAccount;
		DWORD			dwData;
		char			szInfo[_MAX_NAMESIZE];
		int				nServePort;
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
