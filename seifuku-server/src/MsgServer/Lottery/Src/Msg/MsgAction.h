#if !defined(SFK_LOTTERY_MSGACTION_H_INCLUDED)
#define SFK_LOTTERY_MSGACTION_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

const USHORT	_MSG_ACTION		= 1010;

class CMsgAction : public CNetMsg
{
public:
	CMsgAction();
	virtual ~CMsgAction()	{}

public:
	bool	Create		(OBJID idUser, int nPosX, int nPosY, USHORT usDir, USHORT usAction, DWORD dwData);
	virtual void	Process		(void* pInfo)	{}

protected:
	typedef struct {
		PLUGINMSGHEAD_DEFINE

		DWORD		dwTimeStamp;
		OBJID		idUser;
		DWORD		dwData;
		USHORT		usPosX;
		USHORT		usPosY;
		USHORT		usDir;
		USHORT		usAction;
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
