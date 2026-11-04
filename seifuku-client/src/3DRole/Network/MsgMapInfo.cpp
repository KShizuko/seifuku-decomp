#include "MsgMapInfo.h"
#include "3DGameMap.h"
#include "Hero.h"

CMsgMapInfo::CMsgMapInfo()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgMapInfo::~CMsgMapInfo()
{

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
	g_objGameMap.SetID(m_pInfo->idMap);
	g_objGameMap.SetIDDoc(m_pInfo->idDoc);
	g_objGameMap.SetType(m_pInfo->dwType);
}
