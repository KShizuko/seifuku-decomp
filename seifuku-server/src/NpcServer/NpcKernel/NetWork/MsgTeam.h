#if !defined(AFX_MSGTEAM_H__3E7E3CFB_3522_4444_941B_BFDCABD1EFB7__INCLUDED_)
#define AFX_MSGTEAM_H__3E7E3CFB_3522_4444_941B_BFDCABD1EFB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

enum 
{
	_MSGTEAM_CREATE			=	0,
	_MSGTEAM_APPLYJOIN		=	1,
	_MSGTEAM_LEAVE			=	2,
	_MSGTEAM_ACCEPTINVEIT	=	3,

	_MSGTEAM_INVITE			=	4,
	_MSGTEAM_AGREEJOIN		=	5,
	_MSGTEAM_DISMISS		=	6,
	_MSGTEAM_KICKOUT		=	7,
	_MSGTEAM_CLOSETEAM		=	8,
	_MSGTEAM_OPENTEAM		=	9,
};

class CMsgTeam : public CNetMsg  
{
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
		MSGHEAD_DEFINE

		unsigned short	unAction;
		OBJID			idPlayer;
	}MSG_Info;
	
	MSG_Info* m_pInfo;
};

#endif
