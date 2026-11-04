#if !defined(AFX_MSGFRIEND_H__DC85CF8B_3B54_4C8D_94EA_F68172C0A179__INCLUDED_)
#define AFX_MSGFRIEND_H__DC85CF8B_3B54_4C8D_94EA_F68172C0A179__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

WORLDKERNEL_BEGIN
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
public:
	CMsgFriend();
	virtual ~CMsgFriend();

	enum { OFFLINE_STATUS, ONLINE_STATUS };
	BOOL	Create	(int nAction, OBJID idFriend, DWORD dwData1, DWORD dwData2, LPCTSTR szName, bool bOnline = false);

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

private:
	typedef struct
	{
		MSGHEAD_DEFINE

		OBJID			idFriend;		
		unsigned char	ucAction;
		unsigned char	ucOnlineFlag;
		union{
			char			szName[_MAX_NAMESIZE];
			OBJID			idTransmit;
		};
	}MSG_Info;

	MSG_Info*	m_pInfo;
};
WORLDKERNEL_END

#endif
