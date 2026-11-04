#ifndef	MSGFUSE_H
#define MSGFUSE_H

#include "NetMsg.h"

class CMsgFuse : public CNetMsg
{
public:
	CMsgFuse();
	virtual ~CMsgFuse();

public:
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		OBJID			idNpc;
		unsigned short	usType;
		unsigned short	usAmount;
		OBJID			idItem[1];
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
