#include <assert.h>
#include "MsgFee.h"

CMsgFee::CMsgFee()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgFee::~CMsgFee()
{

}

BOOL CMsgFee::Create(OBJID idAccount, unsigned char ucType)
{
	this->Init();

	m_pInfo->idAccount	= idAccount;
	m_pInfo->ucType		= ucType;

	m_pInfo->unMsgType	=_MSG_FEE;
	m_pInfo->unMsgSize	=sizeof(MSG_Info);

	return true;
}

BOOL CMsgFee::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_FEE != this->GetType())
		return false;

	return true;
}