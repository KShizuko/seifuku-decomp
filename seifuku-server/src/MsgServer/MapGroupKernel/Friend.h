#if !defined(AFX_FRIEND_H__5845339C_B1A7_4C13_AC56_D1374692D706__INCLUDED_)
#define AFX_FRIEND_H__5845339C_B1A7_4C13_AC56_D1374692D706__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "FriendData.h"
#include "Myheap.h"
#include "TimeOut.h"

const int	LEAVEWORD_INTERVAL_MINS	= 1;

class CUser;
class CFriend : public CFriendData  
{
public:
	CFriend();
	virtual ~CFriend();
	ULONG	ReleaseByOwner() { delete this; return 0; }

public:
	static CFriend*	CreateNewFriend		(const FriendInfoStruct* pInfo);

public:
	static CFriend*	CreateNew() { return (new CFriend); }

public:

protected:

protected:
public:	MYHEAP_DECLARATION(s_heap)
};

#endif
