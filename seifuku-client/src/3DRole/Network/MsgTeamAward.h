#if !defined(AFX_MSGTEAMAWARD_H__SEIFUKU_1029__INCLUDED_)
#define AFX_MSGTEAMAWARD_H__SEIFUKU_1029__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

class CMsgTeamAward : public CNetMsg
{
public:
	CMsgTeamAward();
	virtual ~CMsgTeamAward();

public:
	BOOL	Create		(char* pMsgBuf, DWORD dwSize);
	BOOL	Create		(DWORD dwTeamData0, DWORD dwTeamData1, DWORD dwTeamData2);
	void	Process		(void* pInfo);

protected:
	typedef struct {
		USHORT	unMsgSize;
		USHORT	unMsgType;

		DWORD	dwTeamData[3];
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
