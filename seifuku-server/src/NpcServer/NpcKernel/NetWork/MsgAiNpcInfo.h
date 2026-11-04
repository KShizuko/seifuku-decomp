#if !defined(MSGAINPC_HEAD)
#define MSGAINPC_HEAD

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "Npc.h"

enum {
		MSGAINPCINFO_CREATENEW		= 1,
};

class CMsgAiNpcInfo  : public CNetMsg
{
public:
	CMsgAiNpcInfo();
	virtual ~CMsgAiNpcInfo();

	BOOL	Create	(int nAction, OBJID idNpc, OBJID idGen, int nType, OBJID idMap, int nPosX, int nPosY);
public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		OBJID			id;
		OBJID			idMap;
		OBJID			idGen;
		OBJID			idOwner;
		int				nData;
		unsigned short	usPosX;
		unsigned short	usPosY;
		unsigned short	usAction;
		unsigned short	usType;
		unsigned short	ucOwnerType;
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
