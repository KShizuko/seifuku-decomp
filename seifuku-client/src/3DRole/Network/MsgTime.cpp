#include "AllMsg.h"
#include "3DGameMap.h"

CMsgTime::CMsgTime()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgTime::~CMsgTime()
{

}

BOOL CMsgTime::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_TIME != this->GetType())
		return false;

	return true;
}

void CMsgTime::Process(void* pInfo)
{
} 
