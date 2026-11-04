#include "AllMsg.h"
#include "basefunc.h"
using namespace world_kernel;
#include "worldkernel.h"

CMsgRegister::CMsgRegister()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgRegister::~CMsgRegister()
{

}

BOOL CMsgRegister::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_REGISTER != this->GetType())
		return false;

	return true;
}

void CMsgRegister::Process(void *pInfo)
{
	if(!StringCheck(m_pInfo->szAccount))
	{
		CMsgTalk	msg;
		if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, STR_ERROR_ILLEGAL_ACCOUNT, NULL, _COLOR_WHITE, _TXTATR_REGISTER))
			SendMsg(&msg);
		return;
	}

	if(!StringCheck(m_pInfo->szPassword))
	{
		CMsgTalk	msg;
		if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, STR_ERROR_ILLEGAL_PASSWORD, NULL, _COLOR_WHITE, _TXTATR_REGISTER))
			SendMsg(&msg);
		return;
	}

	if(!NameStrCheck(m_pInfo->szName) || GameWorld()->IsNameForbidden(m_pInfo->szName))
	{
		CMsgTalk	msg;
		if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, STR_ERROR_ILLEGAL_NAME, NULL, _COLOR_WHITE, _TXTATR_REGISTER))
			SendMsg(&msg);
		return;
	}

	if(GameWorld()->QueryUserList()->CreateNewPlayer(m_pInfo->szAccount, m_pInfo->szName, m_pInfo->szPassword,
					m_pInfo->usLook, m_pInfo->usProfession, m_pInfo->idAccount))
	{
		CMsgTalk	msg;
		if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, REPLAY_OK_STR, NULL, _COLOR_WHITE, _TXTATR_REGISTER))
			SendMsg(&msg);
	}
	else
	{
		CMsgTalk	msg;
		if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, STR_ERROR_DUPLICATE_NAME, NULL, _COLOR_WHITE, _TXTATR_REGISTER))
			SendMsg(&msg);
	}
}
