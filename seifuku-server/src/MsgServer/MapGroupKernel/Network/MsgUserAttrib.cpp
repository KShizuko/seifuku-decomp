#include "AllMsg.h"

CMsgUserAttrib::CMsgUserAttrib()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgUserAttrib::~CMsgUserAttrib()
{

}

BOOL CMsgUserAttrib::Create(OBJID idUser, int nType, DWORD dwData)
{
	this->Init();

	m_unMsgSize		= sizeof(MSG_Info);
	m_unMsgType		= _MSG_USERATTRIB;

	m_pInfo->idUser	=idUser;
	m_pInfo->dwAttributeNum	= 0;

	m_unMsgSize		+= sizeof(UserAttribStruct);
	m_pInfo->setUserAttrib[m_pInfo->dwAttributeNum].ucAttributeType	= nType;
	m_pInfo->setUserAttrib[m_pInfo->dwAttributeNum].dwAttributeData	= dwData;
	m_pInfo->dwAttributeNum++;

	return true;
}

BOOL CMsgUserAttrib::Append(int nType, I64 i64Data)
{
	CHECKF(m_unMsgSize + sizeof(UserAttribStruct) < MAX_PACKETSIZE);

	m_unMsgSize		+= sizeof(UserAttribStruct);
	m_pInfo->setUserAttrib[m_pInfo->dwAttributeNum].ucAttributeType	= nType;
	m_pInfo->setUserAttrib[m_pInfo->dwAttributeNum].dwAttributeData	= (DWORD)i64Data;
	m_pInfo->dwAttributeNum++;

	return true;
}

BOOL CMsgUserAttrib::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_USERATTRIB != this->GetType())
		return false;

	return true;
}

void CMsgUserAttrib::Process(void* pInfo)
{
	::LogSave("invalid msg CMsgUserAttrib.");
}
