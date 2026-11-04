#include "AllMsg.h"

CMsgLoginAccountEx::CMsgLoginAccountEx()
{
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgLoginAccountEx::~CMsgLoginAccountEx()
{

}

void	CMsgLoginAccountEx::Create	(const char* pszAccount, const char* pszPsw, const char* pszServer)
{
	SafeCopy(m_pInfo->szAccount, pszAccount, sizeof(m_pInfo->szAccount));
	SafeCopy(m_pInfo->szPassword, pszPsw, sizeof(m_pInfo->szPassword));
	SafeCopy(m_pInfo->szServer, pszServer, sizeof(m_pInfo->szServer));
	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_LOGINACCOUNTEX;
}
