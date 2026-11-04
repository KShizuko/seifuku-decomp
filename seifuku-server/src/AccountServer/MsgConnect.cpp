#include <assert.h>
#include "MsgConnect.h"

CMsgConnect::CMsgConnect()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;

}

CMsgConnect::~CMsgConnect()
{

}

BOOL CMsgConnect::Create(OBJID idAccount, DWORD dwData, char* pszInfo)
{
	if (!pszInfo || strlen(pszInfo) >= _MAX_NAMESIZE)
		return false;
	
	this->Init();

	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_CONNECT;

	m_pInfo->idAccount		=idAccount;
	m_pInfo->dwData			=dwData;
	strcpy(m_pInfo->szInfo, pszInfo);

	return true;
}

BOOL CMsgConnect::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_CONNECT != this->GetType())
		return false;

	return true;
}