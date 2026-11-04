#include "AllMsg.h"
#include "GamePlayerSet.h"
#include "Hero.h"
#include "MsgUserInfo.h"
#include "GameMsg.h"

CMsgUserInfo::CMsgUserInfo()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;

	m_StrPacker.SetBuf(m_pInfo->szBuf, _MAX_MSGSIZE-sizeof(MSG_Info)+1);
}

CMsgUserInfo::~CMsgUserInfo()
{

}

BOOL CMsgUserInfo::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_USERINFO != this->GetType())
		return false;

	return true;
}

void CMsgUserInfo::Process(void *pInfo)
{
#ifdef _MSGDEBUG
	::LogMsg("Process CMsgUserInfo, id:%u", m_pInfo->id);
#endif

	char szName[_MAX_NAMESIZE]		="";
	char szMate[_MAX_NAMESIZE]		="";

	m_StrPacker.GetString(0, szName, _MAX_NAMESIZE);
	m_StrPacker.GetString(1, szMate, _MAX_NAMESIZE);

	g_objHero.SetName(szName);
	g_objHero.SetMate(szMate);

	g_objHero.SetID(m_pInfo->id);
	g_objNetwork.InitStampTable(m_pInfo->id);

	g_objHero.SetData(CPlayer::_PLAYER_LIFE, m_pInfo->usLife);
	g_objHero.SetMana(m_pInfo->usMana);

	g_objHero.SetForce(m_pInfo->usForce);
	g_objHero.SetSpeed(m_pInfo->usDexterity);
	g_objHero.SetHealth(m_pInfo->usHealth);
	g_objHero.SetSoul(m_pInfo->usSoul);

	g_objHero.SetExp(m_pInfo->nExp);
	g_objHero.SetDeed(m_pInfo->lDeed);
	g_objHero.SetMedalSelect(m_pInfo->dwMedalSelect);
	g_objHero.SetPotential(m_pInfo->nPotential);

	g_objHero.SetMoney(m_pInfo->dwMoney);
	g_objHero.SetEmoney(m_pInfo->dwEmoney);

	g_objHero.Create(m_pInfo->dwLookFace%1000);
	g_objHero.SetFace((m_pInfo->dwLookFace/10000)%1000);
	g_objHero.SetSex((m_pInfo->dwLookFace/1000)%10);

	g_objHero.SetHair(m_pInfo->usHair % 1000);
	g_objHero.SetHead(m_pInfo->usHair / 1000);
	g_objHero.ResetXpFullTime();

	g_objHero.SetAutoAllot((DWORD)m_pInfo->ucAutoAllot);

	g_objHero.SetLev(m_pInfo->ucLevel);
	g_objHero.SetProfession(m_pInfo->ucProfession);
	g_objHero.SetAddPoint(m_pInfo->usAdditionalPoint);

	g_objHero.m_dwNobility = (DWORD)m_pInfo->ucNobility;
	g_objHero.SetMetempsychosis(m_pInfo->ucMetempsychosis);

	g_objHero.SetPk(m_pInfo->sPk);
	g_objHero.GetKillCounter()->Add(0);
	g_objHero.SetPhysicalForce(0);

	g_objGameMsg.m_objChatLog.SearchFile();
	g_objGameMsg.m_objChatLog.LoadLeaveWordLog();
	g_objHero.SetVirtue((DWORD)m_pInfo->dwVirtue);

	CMsgAction msg;
	MYASSERT (msg.Create(g_objHero.GetID(), 0, 0, 0, actionEnterMap));
	msg.Send();
}
