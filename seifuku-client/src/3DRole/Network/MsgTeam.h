#if !defined(AFX_MSGTEAM_H__EC4ACF00_270F_4BEF_8911_2587461A2C67__INCLUDED_)
#define AFX_MSGTEAM_H__EC4ACF00_270F_4BEF_8911_2587461A2C67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

enum 
{
	_MSGTEAM_CREATE				=	0,
	_MSGTEAM_APPLYJOIN			=	1,
	_MSGTEAM_LEAVE				=	2,
	_MSGTEAM_ACCEPTINVITE		=	3,

	_MSGTEAM_INVITE				=	4,
	_MSGTEAM_AGREEJOIN			=	5,
	_MSGTEAM_DISMISS			=	6,
	_MSGTEAM_KICKOUT			=	7,
	_MSGTEAM_CLOSETEAM			=	8,
	_MSGTEAM_OPENTEAM			=	9,
	_MSGTEAM_CLOSEMONEYACCESS	=	10,
	_MSGTEAM_OPENMONEYACCESS	=	11,
	_MSGTEAM_CLOSEITEMACCESS	=	12,
	_MSGTEAM_OPENITEMACCESS		=	13,
	_MSGTEAM_CLOSEGEMACCESS		=   15,
	_MSGTEAM_OPENGEMACCESS		=   16,
};

class CMsgTeam  : public CNetMsg
{
	friend struct MsgTeamProbe;
public:
	CMsgTeam();
	virtual ~CMsgTeam();

public:
	BOOL	Create	(char* pbufMsg, DWORD dwMsgSize);
	void	Process	(void* pInfo);
	BOOL	Create	(OBJID idPlayer, int nAction);
private:
	typedef struct 
	{
		unsigned short	unMsgSize;
		unsigned short	unMsgType;

		unsigned short	unAction;
		OBJID			idPlayer;
	}MSG_Info;
	
	MSG_Info* m_pInfo;


};

#endif
