#include "AllMsg.h"
#include "UserData.h"
#pragma	warning(disable:4786)

CMsgUserInfo::CMsgUserInfo()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;

	m_StrPacker.SetBuf(m_pInfo->szBuf, _MAX_MSGSIZE-offsetof(MSG_Info, szBuf));
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

BOOL CMsgUserInfo::Create(OBJID idUser, const UserInfoStruct* pInfo)
{
	if(!idUser)
		return false;

	this->Init();
	m_pInfo->id					= pInfo->id;
	m_pInfo->dwLookFace			= pInfo->dwLookface;
	m_pInfo->usHair				= pInfo->dwHair;
	m_pInfo->dwMoney			= pInfo->dwMoney;
	m_pInfo->dwEmoney			= pInfo->dwEmoney;
	m_pInfo->nExp				= (LONG)pInfo->i64Exp;

	m_pInfo->nDeed				= pInfo->nDeed;
	m_pInfo->dwMedal			= pInfo->dwMedal;
	m_pInfo->dwMedal_select		= pInfo->dwMedal_select;
	m_pInfo->dwVirtue			= pInfo->dwVirtue;
	m_pInfo->usForce			= pInfo->usForce;
	m_pInfo->usDexterity		= pInfo->usSpeed;
	m_pInfo->usHealth			= pInfo->usHealth;
	m_pInfo->usSoul				= pInfo->usSoul;

	m_pInfo->usAdditionalPoint	= pInfo->usAdditional_point;

	m_pInfo->usLife				= pInfo->usLife;

	m_pInfo->usMana				= pInfo->usMana;
	m_pInfo->sPk				= pInfo->sPk;
	m_pInfo->ucLevel			= pInfo->ucLevel;
	m_pInfo->ucProfession		= pInfo->ucProfession;

	m_pInfo->ucNobility			= pInfo->ucNobility;
	m_pInfo->ucMetempsychosis	= pInfo->ucMetempsychosis;
	m_pInfo->ucAutoAllot		= pInfo->ucAuto_allot;

	BOOL bSucMake	=true;
	bSucMake	&=m_StrPacker.AddString((char*)pInfo->szName);
	bSucMake	&=m_StrPacker.AddString((char*)pInfo->szMate);

	m_unMsgType	=_MSG_USERINFO;
	m_unMsgSize	=offsetof(MSG_Info, szBuf)+m_StrPacker.GetSize();

	return true;
}

void CMsgUserInfo::Process(void *pInfo)
{
	ASSERT(!"CMsgUserInfo::Process()");
	return;

#ifdef _MSGDEBUG
	::LogMsg("Process CMsgUserInfo, id:%u", m_pInfo->id);
#endif

	char szName[_MAX_NAMESIZE]		="";
	char szMate[_MAX_NAMESIZE]		="";
	char szSyndicate[_MAX_NAMESIZE]	="";
	char szTitle[_MAX_NAMESIZE]		="";

	m_StrPacker.GetString(0, szName, _MAX_NAMESIZE);
	m_StrPacker.GetString(1, szMate, _MAX_NAMESIZE);
}
