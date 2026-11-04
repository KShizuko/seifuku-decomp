#include <string.h>
#include "MsgAccount_A.h"

CMsgAccount_A::CMsgAccount_A()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgAccount_A::~CMsgAccount_A()
{

}

BOOL CMsgAccount_A::Create(const char* pszAccount, const char* pszPsw, const char* pszServer, int nServerPort)
{
	if(!pszAccount || strlen(pszAccount) >= _MAX_NAMESIZE)
		return false;

	if(!pszPsw || strlen(pszPsw) >= _MAX_NAMESIZE)
		return false;

	if(!pszServer || strlen(pszServer) >= _MAX_NAMESIZE)
		return false;

	this->Init();

	strcpy(m_pInfo->szAccount, pszAccount);
	strcpy(m_pInfo->szPassword, pszPsw);
	strcpy(m_pInfo->szServer, pszServer);
	m_pInfo->nServerPort	= nServerPort;

	m_pInfo->unMsgType	=_MSG_ACCOUNT_A;
	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	return true;
}

BOOL CMsgAccount_A::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_ACCOUNT_A != this->GetType())
		return false;

	return true;
}
