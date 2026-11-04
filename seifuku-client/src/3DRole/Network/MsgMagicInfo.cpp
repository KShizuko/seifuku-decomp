#include "MsgMagicInfo.h"
#include "Hero.h"
#include "GamePlayerSet.h"
#include "Pet.h"

CMsgMagicInfo::CMsgMagicInfo()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgMagicInfo::~CMsgMagicInfo()
{

}

BOOL CMsgMagicInfo::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_MAGICINFO != this->GetType())
		return false;

	return true;
}

void CMsgMagicInfo::Process(void* pInfo)
{
	g_objHero.AddMagic(m_pInfo->usType, m_pInfo->usLevel, m_pInfo->dwExp);
	::PostCmd(CMD_FLASH_MAGIC);
}
