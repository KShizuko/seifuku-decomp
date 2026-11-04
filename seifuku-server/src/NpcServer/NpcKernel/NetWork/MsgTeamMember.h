#if !defined(AFX_MSGTEAMMEMBER_H__0C122D39_8E5B_48A9_94BA_0C20ED3C3E23__INCLUDED_)
#define AFX_MSGTEAMMEMBER_H__0C122D39_8E5B_48A9_94BA_0C20ED3C3E23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "team.h"

const int _MAX_TEAMMEMBER = 5;
enum
{
	_MSG_TEAMMEMBER_ADDMEMBER = 0,
};

class CMsgTeamMember : public CNetMsg  
{
public:
	CMsgTeamMember();
	virtual ~CMsgTeamMember();

public:

	BOOL	Create			(char* pbufMsg, DWORD dwMsgSize);
	void	Process			(void* pInfo);

private:
	typedef struct 
	{
		MSGHEAD_DEFINE
		
		unsigned char		ucAction;
		unsigned char		ucAmount;
		TeamMemberInfo		setMember[_MAX_TEAMMEMBER];
	}MSG_Info;
	
	MSG_Info* m_pInfo;
};

#endif
