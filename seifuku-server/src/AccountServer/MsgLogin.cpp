#include "MsgLogin.h"

CMsgLogin::CMsgLogin()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgLogin::~CMsgLogin()
{

}

BOOL CMsgLogin::Create(OBJID idAccount, DWORD dwCheckData, char* pszName, USHORT usVersion)
{
	if(!pszName || strlen(pszName) >= _MAX_NAMESIZE)
		return false;

	this->Init();
	
	m_pInfo->unMsgType	=_MSG_LOGIN;
	m_pInfo->unMsgSize	=GetStdSize();

	m_pInfo->idAccount	=idAccount;
	m_pInfo->dwCheckData=dwCheckData;
	m_pInfo->usVersion	=usVersion;
	strcpy(m_pInfo->szUserName, pszName);
	return true;
}

BOOL CMsgLogin::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if(!pbufMsg)
		return false;

	if((unsigned short)dwMsgSize != GetSize(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_LOGIN != GetType(pbufMsg, dwMsgSize))
		return false;

	memcpy(this->m_bufMsg, pbufMsg, dwMsgSize);
	return true;
}