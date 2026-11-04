#include "AllMsg.h"
#include "mapgroup.h"

CMsgTeamAward::CMsgTeamAward()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgTeamAward::~CMsgTeamAward()
{

}

BOOL CMsgTeamAward::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	return _MSG_TEAMAWARD == this->GetType();
}

BOOL CMsgTeamAward::Create(DWORD dwAward0, DWORD dwMoney, DWORD dwVirtue)
{
	this->Init();

	m_unMsgSize	=sizeof(MSG_Info);
	m_unMsgType	=_MSG_TEAMAWARD;

	m_pInfo->dwAward0	=dwAward0;
	m_pInfo->dwMoney	=dwMoney;
	m_pInfo->dwVirtue	=dwVirtue;
	return true;
}

void CMsgTeamAward::Process(void *pInfo)
{
	ASSERT(!"CMsgTeamAward::Process()");
}
