#include "AllMsg.h"
#include "3DGameMap.h"
#include "GamePlayerSet.h"
#include "Role.h"
#include "Hero.h"

CMsgNpc::CMsgNpc()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgNpc::~CMsgNpc()
{

}

BOOL CMsgNpc::Create(int nAction, OBJID id, DWORD dwData, USHORT usType)
{
	this->Init();

	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_NPC;

	m_pInfo->dwData		= dwData;
	m_pInfo->id			= id;
	m_pInfo->usAction	= nAction;
	m_pInfo->usType		= usType;
	return true;
}

BOOL CMsgNpc::Create(char* pMsgBuf, DWORD dwSize)
{
	if (!CNetMsg::Create(pMsgBuf, dwSize))
		return false;
	if(_MSG_NPC != this->GetType())
		return false;

	return true;
}

void CMsgNpc::Process(void* pInfo)
{
	switch(m_pInfo->usAction)
	{
	case EVENT_LEAVEMAP:
	case EVENT_DELNPC:
		g_objPlayerSet.DelPlayer(m_pInfo->id);
		break;
	case EVENT_LAYNPC:
		if(m_pInfo->usType == _ROLE_STATUARY_NPC)
		{
			::PostCmd(CMD_OPEN_PLACE_STATUARY_DIALOG);
		}
		else if(m_pInfo->usType == _ROLE_FURNITURE_NPC || m_pInfo->usType == _ROLE_CITY_GATE_NPC ||
			_ROLE_CITY_WALL_NPC == m_pInfo->usType || _ROLE_CITY_MOAT_NPC == m_pInfo->usType)
		{
			g_objHero.CreateMousePlayer(m_pInfo->dwData, true);
		}
		else
		{
			g_objHero.CreateMousePlayer(m_pInfo->dwData);
		}
		break;
	default:
		break;
	}
}
