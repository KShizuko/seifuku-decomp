#include "AllMsg.h"
#include "mapgroup.h"

CMsgTick::CMsgTick()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgTick::~CMsgTick()
{

}

BOOL CMsgTick::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_TICK != this->GetType())
		return false;

	return true;
}

BOOL CMsgTick::Create(OBJID idPlayer)
{
	if (idPlayer == ID_NONE)
		return false;

	this->Init();

	m_unMsgSize	=sizeof(MSG_Info);
	m_unMsgType	=_MSG_TICK;

	m_pInfo->idPlayer	=idPlayer;
	for(int i = 8; i < 0x18; i += 4)
	{
		DWORD dwHigh	= ::RandGet(0xFFFF);
		DWORD dwLow		= ::RandGet(0xFFFF);
		*(DWORD*)((char*)m_pInfo + i)	= (dwHigh << 16) + dwLow;
	}

	return true;
}

void CMsgTick::Process(void *pInfo)
{
	CUserPtr pUser = UserManager()->GetUser(this);
	if(pUser)
	{

		DWORD dwTime = m_pInfo->dwData ^ pUser->GetID();
		pUser->ProcessTick(dwTime);
	}
}
