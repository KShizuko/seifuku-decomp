#include "AllMsg.h"
#include "GameMsg.h"
#include "Hero.h"
#include "GameDataSet.h"

CMsgTick::CMsgTick()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgTick::~CMsgTick()
{

}

BOOL CMsgTick::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_TICK != this->GetType())
		return false;

	return true;
}

void CMsgTick::Send(void)
{
	g_objHero.m_objCheckKeys.SetKey(0x10, 3);
	CNetMsg::Send();
}

void CMsgTick::Process(void* pInfo)
{
	static DWORD dwLastTick	=0;

	DWORD dwTimeNow		= ::TimeGet();
	m_pInfo->dwData		= dwTimeNow^g_objHero.GetID();
	const char* pszName = g_objHero.GetName();
	if (strlen(pszName) >= 4)
		m_pInfo->dwChkData	= ((BYTE)pszName[0] ^ 0x23) + (((BYTE)pszName[1] ^ 0x98) << 8) + ((((BYTE)pszName[2] ^ 0xE4) << 16) * ((BYTE)pszName[3] ^ 0x81) << 24);
	else
		m_pInfo->dwChkData	= 0x9D4B5703;
	for (int i = 0; i < 4; i++)
		m_pInfo->dwRandom[i] = (::RandGet(0xFFFF) << 16) + ::RandGet(0xFFFF);
	this->Send();

	const DWORD INTERVAL_TICK = 10*1000;
	if (dwLastTick != 0 && dwTimeNow > dwLastTick+INTERVAL_TICK*(100+30)/100)
	{
		const OBJID idNetBlock = 100115;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idNetBlock));
	}

	dwLastTick = dwTimeNow;
}
