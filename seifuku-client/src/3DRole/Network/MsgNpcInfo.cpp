#include "MsgNpcInfo.h"
#include "3DGameMap.h"
#include "GamePlayerSet.h"
#include "Player.h"
#include "3DRoleData.h"

CMsgNpcInfo::CMsgNpcInfo()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
	m_StrPacker.SetBuf(m_pInfo->szName, _MAX_MSGSIZE-sizeof(MSG_Info)+1);
}

CMsgNpcInfo::~CMsgNpcInfo()
{

}

BOOL CMsgNpcInfo::Create(char* pMsgBuf, DWORD dwSize)
{
	if (!CNetMsg::Create(pMsgBuf, dwSize))
		return false;
	if(_MSG_NPCINFO != this->GetType())
		return false;

	return true;
}

BOOL CMsgNpcInfo::Create(CMyPos posNpc, int nDir, int nFrame, int nAction)
{
	this->Init();

	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_NPCINFO;

	m_pInfo->usLook = nDir;
	m_pInfo->usType = nFrame;
	m_pInfo->usSort = nAction;
	m_pInfo->usCellX = posNpc.x;
	m_pInfo->usCellY = posNpc.y;
	return true;
}

void CMsgNpcInfo::Process(void* pInfo)
{
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_pInfo->id);
	if(pPlayer)
	{
		g_objPlayerSet.DelPlayer(m_pInfo->id);
		pPlayer = NULL;
	}
	if(!pPlayer)
	{
		pPlayer	=new CPlayer;
		MYASSERT (pPlayer);
		pPlayer->SetID(m_pInfo->id);
		pPlayer->SetName("");
		if(m_pInfo->usType == _ROLE_BOOTH_NPC)
			pPlayer->Create(m_pInfo->usLook, _ROLE_VIEW_BOOTH);
		else if(m_pInfo->usType == _ROLE_DICE_NPC)
			pPlayer->Create(m_pInfo->usLook, _ROLE_VIEW_DICENPC);
		else if(m_pInfo->usType == _ROLE_FURNITURE_NPC || m_pInfo->usType == _ROLE_CITY_GATE_NPC ||
			_ROLE_CITY_WALL_NPC == m_pInfo->usType || _ROLE_CITY_MOAT_NPC == m_pInfo->usType)
		{
			CMyPos posNpc;
			posNpc.x = (int)m_pInfo->usCellX;
			posNpc.y = (int)m_pInfo->usCellY;
			pPlayer->Create(m_pInfo->usLook, _ROLE_VIEW_TERRAIN, m_pInfo->id, posNpc);
			pPlayer->SetPos(posNpc);
		}
		else
			pPlayer->Create(m_pInfo->usLook, _ROLE_VIEW_TASKNPC);
		pPlayer->SetRoleType(m_pInfo->usType);
		if(!(m_pInfo->usType == _ROLE_FURNITURE_NPC || m_pInfo->usType == _ROLE_CITY_GATE_NPC ||
			_ROLE_CITY_WALL_NPC == m_pInfo->usType || _ROLE_CITY_MOAT_NPC == m_pInfo->usType))
		{
			if(m_pInfo->usType == _ROLE_CITY_GATE_NPC)
				pPlayer->SetAddSize(2);
			CMyPos posNpc;
			posNpc.x = (int)m_pInfo->usCellX;
			posNpc.y = (int)m_pInfo->usCellY;
			pPlayer->SetPos(posNpc);
			NpcTypeInfo* pInfo = RoleDataQuery()->GetNpcTypeInfo(m_pInfo->usLook/10);
			MYASSERT(pInfo);
			pPlayer->SetDir(m_pInfo->usLook%10);
			pPlayer->SetName(pInfo->szNpcName);
			if (pPlayer->m_objEffect)
				pPlayer->m_objEffect->Add(pInfo->szEffectIndex);
			if(m_StrPacker.GetStrAmount() > 0)
			{
				char szName[64];
				m_StrPacker.GetString(0, szName, 16);
				pPlayer->SetName(szName);
			}
		}
		g_objPlayerSet.AddPlayer(pPlayer);
	}
}

BOOL CMsgNpcInfo::Create(CMyPos posNpc, int nLook)
{
	this->Init();

	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_NPCINFO;

	m_pInfo->usLook = nLook;
	m_pInfo->usCellX = posNpc.x;
	m_pInfo->usCellY = posNpc.y;
	return true;
}
