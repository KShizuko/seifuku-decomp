#if !defined(AFX_MSGFRIEND_H__DC85CF8B_3B54_4C8D_94EA_F68172C0A179__INCLUDED_)
#define AFX_MSGFRIEND_H__DC85CF8B_3B54_4C8D_94EA_F68172C0A179__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

const int _FRIEND_APPLY			= 10;
const int _FRIEND_ACCEPT		= 11;
const int _FRIEND_ONLINE		= 12;
const int _FRIEND_OFFLINE		= 13;
const int _FRIEND_BREAK			= 14;
const int _FRIEND_GETINFO		= 15;

const int _ENEMY_ONLINE			= 16;
const int _ENEMY_OFFLINE		= 17;
const int _ENEMY_DEL			= 18;
const int _ENEMY_ADD			= 19;

class CMsgFriend : public CNetMsg
{
	friend struct MsgFriendProbe;
public:
	CMsgFriend();
	virtual ~CMsgFriend();

	BOOL	Create	(int nAction, OBJID idFriend, BOOL bOnline, const char* pszName);

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

private:
	typedef struct
	{
		unsigned short	unMsgSize;
		unsigned short	unMsgType;
		OBJID			idFriend;
		unsigned char	ucAction;
		unsigned char	ucOnlineFlag;
		unsigned char	ucLevel;
		union {
			char	szName[_MAX_NAMESIZE];
			OBJID	idData;
		};

	}MSG_Info;
		
	MSG_Info*	m_pInfo;
};

#endif
