#include "allmsg.h"
#include "team.h"
#include "mapgroup.h"
#include <memory.h>

CMsgTeamMember::CMsgTeamMember()
{
	this->Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgTeamMember::~CMsgTeamMember()
{

}

BOOL CMsgTeamMember::Create(int nAction, const TeamMemberInfo& setInfo)
{
	this->Init();

	m_unMsgSize	=sizeof(MSG_Info);
	m_unMsgType	=_MSG_TEAMMEMBER;

	m_pInfo->ucAction	= nAction;
	m_pInfo->ucAmount	= 1;
	memcpy(m_pInfo->setMember, &setInfo, sizeof(TeamMemberInfo));
	return true;
}

BOOL CMsgTeamMember::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if(!pbufMsg)
		return false;

	memcpy(this->m_bufMsg, pbufMsg, dwMsgSize);
	return true;
}

void CMsgTeamMember::Process(void* pInfo)
{
}