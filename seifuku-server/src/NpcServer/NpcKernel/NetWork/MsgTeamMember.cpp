#include "allmsg.h"
#include "team.h"
#include "npcWorld.h"
#include "Agent.h"

CMsgTeamMember::CMsgTeamMember()
{
	this->Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgTeamMember::~CMsgTeamMember()
{

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
	CAgent* pAgent = NpcManager()->QueryAgent(GetNpcID());
	if(!pAgent)
		return ;

	Cast<CTeam>(pAgent)->AddMember(m_pInfo->ucAmount, m_pInfo->setMember);
}