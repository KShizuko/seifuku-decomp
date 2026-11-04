#include "MsgTeamAward.h"
#include "Hero.h"

CMsgTeamAward::CMsgTeamAward()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgTeamAward::~CMsgTeamAward()
{

}

BOOL CMsgTeamAward::Create(char* pMsgBuf, DWORD dwSize)
{
	if (!CNetMsg::Create(pMsgBuf, dwSize))
		return false;
	if(_MSG_TYPE1029 != this->GetType())
		return false;

	return true;
}

BOOL CMsgTeamAward::Create(DWORD dwTeamData0, DWORD dwTeamData1, DWORD dwTeamData2)
{
	this->Init();
	m_pInfo->unMsgSize	= sizeof(MSG_Info);
	m_pInfo->unMsgType	= _MSG_TYPE1029;
	m_pInfo->dwTeamData[0] = dwTeamData0;
	m_pInfo->dwTeamData[1] = dwTeamData1;
	m_pInfo->dwTeamData[2] = dwTeamData2;
	return true;
}
void CMsgTeamAward::Process(void* pInfo)
{
	g_objHero.SetTeamData1(g_objHero.GetTeamData1() + m_pInfo->dwTeamData[1]);
	g_objHero.SetTeamData0(g_objHero.GetTeamData0() + m_pInfo->dwTeamData[0]);
	g_objHero.SetTeamData2(g_objHero.GetTeamData2() + m_pInfo->dwTeamData[2]);
}
