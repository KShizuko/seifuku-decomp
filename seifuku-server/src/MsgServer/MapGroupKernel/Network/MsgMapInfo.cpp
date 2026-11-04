#include "AllMsg.h"

CMsgMapInfo::CMsgMapInfo()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgMapInfo::~CMsgMapInfo()
{

}

BOOL CMsgMapInfo::Create(CGameMap* pMap)
{
	IF_NOT (pMap)
		return false;

	this->Init();

	m_pInfo->idMap		= pMap->GetID();
	m_pInfo->idDoc		= pMap->GetDocID();
	m_pInfo->dwType		= pMap->GetType();

	m_unMsgType	= _MSG_MAPINFO;
	m_unMsgSize	= sizeof(MSG_Info);

	return true;

}

BOOL CMsgMapInfo::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_MAPINFO != this->GetType())
		return false;

	return true;
}

void CMsgMapInfo::Process(void* pInfo)
{
}
