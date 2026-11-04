#if !defined(SFK_LOTTERY_NETMSG_H_INCLUDED)
#define SFK_LOTTERY_NETMSG_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>
#include "define.h"

const int	_MAX_PLUGINMSGSIZE	= 0x200;

#define	PLUGINMSGHEAD_DEFINE	USHORT unMsgSize; USHORT unMsgType;

class CNetMsg
{
public:
	CNetMsg()			{ Init(); }
	virtual ~CNetMsg()	{}

public:
	virtual bool	Create		(char* pbufMsg, USHORT unMsgSize);
	virtual void	Process		(void* pInfo)	{}

public:
	void			Init		();
	char*			GetBuf		()	{ return m_bufMsg; }
	USHORT			GetSize		()	{ return *(USHORT*)m_bufMsg; }
	USHORT			GetType		()	{ return *(USHORT*)(m_bufMsg+2); }

protected:
	char			m_bufMsg[_MAX_PLUGINMSGSIZE];
};

#endif
