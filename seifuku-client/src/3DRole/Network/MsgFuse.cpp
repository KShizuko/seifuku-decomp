#include "AllMsg.h"

CMsgFuse::CMsgFuse()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgFuse::~CMsgFuse()
{

}

BOOL CMsgFuse::Create(char* pMsgBuf, DWORD dwSize)
{
	if (!CNetMsg::Create(pMsgBuf, dwSize))
		return false;
	if(_MSG_TYPE1028 != this->GetType())
		return false;

	return true;
}

BOOL CMsgFuse::Create(OBJID idUser, USHORT usAction, DWORD dwData)
{
	this->Init();

	m_pInfo->idUser		=idUser;
	m_pInfo->usAction	=usAction;
	m_pInfo->usAmount	=1;
	m_pInfo->dwData[0]	=dwData;
	m_unMsgType	=_MSG_TYPE1028;
	m_unMsgSize	=sizeof(MSG_Info);
	return true;
}

void CMsgFuse::Append(DWORD dwData)
{
	m_pInfo->dwData[m_pInfo->usAmount]	=dwData;
	m_pInfo->usAmount++;
	m_unMsgSize	+=sizeof(DWORD);
}

void CMsgFuse::Process(void* pInfo)
{
}
