#ifndef	MSGSYNDICATE_H
#define MSGSYNDICATE_H

#include "NetMsg.h"

enum{
	ACTION_NONE = 0,

	APPLY_JOIN			= 1,
	INVITE_JOIN			= 2,
	LEAVE_SYN			= 3,
	KICKOUT_MEMBER		= 4,
	QUERY_SYN_NAME		= 6,
	SET_ALLY			= 7,
	CLEAR_ALLY			= 8,
	SET_ANTAGONIZE		= 9,
	CLEAR_ANTAGONIZE	= 10,
	DONATE_MONEY		= 11,
	QUERY_SYNATTR		= 12,

	SET_MAINSYN			= 13,
	SET_SUBSYN			= 14,
	SET_WHITE_SYN		= 17,
	SET_BLACK_SYN		= 18,
	DESTROY_SYN			= 19,

	NPCMSG_CREATE_SYN	= 101,
	NPCMSG_DESTROY_SYN	= 102,

};

class CMsgSyndicate : public CNetMsg
{
public:
	CMsgSyndicate();
	virtual ~CMsgSyndicate();

	BOOL	Create(unsigned short usAction, OBJID idTarget);

public:

	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pSocket);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		unsigned short	usAction;
		union
		{
			OBJID	idTarget;
			DWORD	dwData;
		};
	}MSG_Info;

	MSG_Info*	m_pInfo;

};

#endif
