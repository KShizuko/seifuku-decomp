#include "AllMsg.h"

CMsgLogin::CMsgLogin()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgLogin::~CMsgLogin()
{

}

BOOL CMsgLogin::Create(DWORD dwVersion, const char* pszAccount, const char* pszPassword)
{
	if (!pszAccount || strlen(pszAccount) >= _MAX_NAMESIZE)
		return false;
	if (!pszPassword || strlen(pszPassword) >= _MAX_NAMESIZE)
		return false;
	this->Init();
	m_pInfo->unMsgType	=_MSG_LOGIN;
	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->dwVersion	=dwVersion;
	strcpy(m_pInfo->szAccount, pszAccount);
	strcpy(m_pInfo->szPassword, pszPassword);
	return true;
}

BOOL CMsgLogin::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_LOGIN != this->GetType())
		return false;

	return true;
}

void CMsgLogin::Process(void* pInfo)
{
#ifdef _MSGDEBUG
	::LogMsg("Process CMsgLogin, Account:%d, Name:%s", 
					m_pInfo->idAccount, 
					m_pInfo->szUserName);
#endif

}









