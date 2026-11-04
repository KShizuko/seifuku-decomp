#if !defined(MSGAINPC_HEAD)
#define MSGAINPC_HEAD

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "protocol.h"

enum {
		MSGAINPCINFO_CREATENEW		= 1,
};

class CMsgMonsterInfo  : public CNetMsg
{
public:
	CMsgMonsterInfo();
	virtual ~CMsgMonsterInfo();

	BOOL	Create	(const ST_CREATENEWNPC* pInfo);
public:	
	void			Process		(void* pSocket);

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
