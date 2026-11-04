#include "AllMsg.h"

CMsgAccount::CMsgAccount()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgAccount::~CMsgAccount()
{

}

BOOL CMsgAccount::Create(const char* pszAccount, const char* pszPsw, const char* pszServer)
{
	if (!pszAccount || strlen(pszAccount) >= _MAX_NAMESIZE)
		return false;

	if (!pszPsw)
		return false;

	if (!pszServer || strlen(pszServer) >= _MAX_NAMESIZE)
		return false;

	this->Init();

	strcpy(m_pInfo->szAccount, pszAccount);
	memcpy(m_pInfo->szPassword, pszPsw, _MAX_NAMESIZE);
	strcpy(m_pInfo->szServer, pszServer);

	m_pInfo->unMsgType	=_MSG_ACCOUNT;
	m_pInfo->unMsgSize	=sizeof(MSG_Info);

	return true;
}

BOOL CMsgAccount::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_ACCOUNT != this->GetType())
		return false;

	return true;
}

void CMsgAccount::Process(void* pInfo)
{
}
