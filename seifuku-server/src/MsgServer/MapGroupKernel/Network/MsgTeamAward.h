#if !defined(AFX_MSGTEAMAWARD_H__INCLUDED_)
#define AFX_MSGTEAMAWARD_H__INCLUDED_

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
	BOOL	Create	(char* pbufMsg, DWORD dwMsgSize);
	BOOL	Create	(DWORD dwAward0, DWORD dwMoney, DWORD dwVirtue);
	void	Process	(void *pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE
		DWORD			dwAward0;
		DWORD			dwMoney;
		DWORD			dwVirtue;
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
