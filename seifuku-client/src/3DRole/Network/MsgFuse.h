#if !defined(AFX_MSGFUSE_H__SEIFUKU_1028__INCLUDED_)
#define AFX_MSGFUSE_H__SEIFUKU_1028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

class CMsgFuse : public CNetMsg
{
public:
	CMsgFuse();
	virtual ~CMsgFuse();

	BOOL	Create		(OBJID idUser, USHORT usAction, DWORD dwData);
	void	Append		(DWORD dwData);

public:
	BOOL	Create		(char* pMsgBuf, DWORD dwSize);
	void	Process		(void* pInfo);

protected:
	typedef struct {
		USHORT	unMsgSize;
		USHORT	unMsgType;

		OBJID	idUser;
		USHORT	usAction;
		USHORT	usAmount;
		DWORD	dwData[1];
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
