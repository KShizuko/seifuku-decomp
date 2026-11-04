#if !defined(AFX_MSGSYNDICATE_H__6D5AF55F_20CC_4A86_931B_878B4A787FEC__INCLUDED_)
#define AFX_MSGSYNDICATE_H__6D5AF55F_20CC_4A86_931B_878B4A787FEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif
#include "NetMsg.h"

enum
{
		ACTION_NONE			= 0,
		APPLY_JOIN			= 1,
		INVITE_JOIN			= 2,
		LEAVE_SYN			= 3,
		KICKOUT_MEMBER		= 4,
		QUERY_SYN_NAME		= 6,
		ALLY_APPLY			= 7,
		CLEAR_ALLY			= 8,
		ANTAGONIZE			= 9,
		CLEAR_ANTAGONIZE	= 10,
		DONATE_MONEY		= 11,
		QUERY_SYNATTR		= 12,
		SET_MAINSYN			= 13,
		SET_SYN				= 14,
		UNITE_SUBSYN		= 15,
		UNITE_SYN			= 16,
		SET_WHITE_SYN		= 17,
		SET_BLACK_SYN		= 18,
		DESTROY_SYN			= 19,
		SET_MANTLE			= 20,
};

class CMsgSyndicate  : public CNetMsg    
{
public:
	CMsgSyndicate();
	virtual ~CMsgSyndicate();

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);
	BOOL			Create		(unsigned short usAction, OBJID idTarget);
protected:
	typedef struct {
		unsigned short	unMsgSize;
		unsigned short	unMsgType;
		
		unsigned short	usAction;
		union
		{
			OBJID idTarget;
			DWORD dwData;
		};
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
