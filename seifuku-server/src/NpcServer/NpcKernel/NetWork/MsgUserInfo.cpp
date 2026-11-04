#include "AllMsg.h"
#include "agent.h"
#include "NPCWORLD.h"
#pragma	warning(disable:4786)

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
	UserInfoStruct	info;
	memset(&info, 0, sizeof(UserInfoStruct));

	m_StrPacker.GetString(0, info.szName, _MAX_NAMESIZE);
	m_StrPacker.GetString(1, info.szMate, _MAX_NAMESIZE);
	IF_NOT(strlen(info.szName) > 0)
		return ;

	info.id					= m_pInfo->id;
	info.dwLookface			= m_pInfo->dwLookFace;
	info.dwHair				= m_pInfo->usHair;
	info.dwMoney			= m_pInfo->dwMoney;
	info.nPotential			= m_pInfo->nPotential;
	info.dwReserved3C		= m_pInfo->dwReserved16;
	info.nDeed				= m_pInfo->nDeed;
	info.dwMedal			= m_pInfo->dwMedal;
	info.dwMedal_select		= m_pInfo->dwMedalSelect;
	info.dwVirtue			= m_pInfo->dwVirtue;

	info.usForce			= m_pInfo->usForce;
	info.usDexterity		= m_pInfo->usDexterity;
	info.usHealth			= m_pInfo->usHealth;
	info.usSoul				= m_pInfo->usSoul;

	info.usAdditional_point	= m_pInfo->usAdditionalPoint;

	info.usLife				= m_pInfo->usLife;
	info.usMana				= m_pInfo->usMana;
	info.sPk				= m_pInfo->sPk;

	info.ucLevel			= m_pInfo->ucLevel;
	info.ucProfession		= m_pInfo->ucProfession;
	info.ucNobility			= m_pInfo->ucNobility;
	info.ucMetempsychosis	= m_pInfo->ucMetempsychosis;
	info.ucAuto_allot		= m_pInfo->ucAutoAllot;

	INpc* pNpc = NpcManager()->QueryNpc(info.id);
	CAgent*	pAgent = Cast<CAgent>(pNpc);
	IF_OK(pAgent)
		pAgent->AppendInfo(info);
}
