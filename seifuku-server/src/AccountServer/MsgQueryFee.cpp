#include <assert.h>
#include "AllHeads.h"
#include "MsgQueryFee.h"

CMsgQueryFee::CMsgQueryFee()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgQueryFee::~CMsgQueryFee()
{

}

BOOL CMsgQueryFee::Create(OBJID idAccount, unsigned char ucType, DWORD dwData, int nTime)
{
	this->Init();

	m_pInfo->idAccount	= idAccount;
	m_pInfo->ucType		= ucType;
	m_pInfo->dwData		= dwData;
	m_pInfo->nTime		= nTime;

	m_pInfo->unMsgType	=_MSG_QUERYFEE;
	m_pInfo->unMsgSize	=sizeof(MSG_Info);

	return true;
}

BOOL CMsgQueryFee::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_QUERYFEE != this->GetType())
		return false;

	return true;
}