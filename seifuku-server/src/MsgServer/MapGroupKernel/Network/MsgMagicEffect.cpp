#include "AllMsg.h"

CMsgMagicEffect::CMsgMagicEffect()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgMagicEffect::~CMsgMagicEffect()
{

}

BOOL CMsgMagicEffect::Create(OBJID idUser, int nType, int nLevel, OBJID idTarget)
{
	CHECKF(idUser != ID_NONE && idTarget != ID_NONE);

	this->Init();

	m_unMsgSize		= sizeof(MSG_Info);
	m_unMsgType		= _MSG_MAGICEFFECT;

	m_pInfo->idUser			= idUser;
	m_pInfo->usType			= nType;
	m_pInfo->usLevel		= nLevel;
	m_pInfo->idTarget		= idTarget;
	m_pInfo->ucEffectNum	= 0;

	return true;
}

BOOL CMsgMagicEffect::Create(OBJID idUser, int nType, int nLevel, OBJID idTarget, DWORD dwData, int nDeltaBattleEffect )
{
	CHECKF(idUser != ID_NONE && idTarget != ID_NONE);

	this->Init();

	m_unMsgSize		= sizeof(MSG_Info);
	m_unMsgType		= _MSG_MAGICEFFECT;

	m_pInfo->idUser			= idUser;
	m_pInfo->usType			= nType;
	m_pInfo->usLevel		= nLevel;
	m_pInfo->idTarget		= idTarget;
	m_pInfo->ucEffectNum	= 0;

	{
		m_unMsgSize		+= sizeof(EffectRoleStruct);
		m_pInfo->setEffectRole[m_pInfo->ucEffectNum].idRole	= idTarget;
		m_pInfo->setEffectRole[m_pInfo->ucEffectNum].dwData	= dwData;
		m_pInfo->ucEffectNum++;
	}

	return true;
}

BOOL CMsgMagicEffect::CreateByPos(OBJID idUser, int nType, int nLevel, int x, int y, OBJID idTarget , DWORD dwData , int nDeltaBattleEffect )
{
	CHECKF(idUser != ID_NONE);

	this->Init();

	m_unMsgSize		= sizeof(MSG_Info);
	m_unMsgType		= _MSG_MAGICEFFECT;

	m_pInfo->idUser			= idUser;
	m_pInfo->usType			= nType;
	m_pInfo->usLevel		= nLevel;
	m_pInfo->usPosX			= x;
	m_pInfo->usPosY			= y;
	m_pInfo->ucEffectNum	= 0;

	if(idTarget != ID_NONE)
	{
		m_unMsgSize		+= sizeof(EffectRoleStruct);
		m_pInfo->setEffectRole[m_pInfo->ucEffectNum].idRole	= idTarget;
		m_pInfo->setEffectRole[m_pInfo->ucEffectNum].dwData	= dwData;
		m_pInfo->ucEffectNum++;
	}

	return true;
}

BOOL CMsgMagicEffect::AppendRole(OBJID idRole, DWORD dwData, int nDeltaBattleEffect)
{
	MYASSERT(idRole != ID_NONE);
	if(m_unMsgSize + sizeof(EffectRoleStruct) >= MAX_PACKETSIZE)
		return false;

	m_unMsgSize		+= sizeof(EffectRoleStruct);
	m_pInfo->setEffectRole[m_pInfo->ucEffectNum].idRole	= idRole;
	m_pInfo->setEffectRole[m_pInfo->ucEffectNum].dwData	= dwData;
	m_pInfo->ucEffectNum++;

	return true;
}

BOOL CMsgMagicEffect::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_MAGICEFFECT != this->GetType())
		return false;

	return true;
}

void CMsgMagicEffect::Process(void* pInfo)
{
	::LogSave("invalid msg CMsgMagicEffect.");
}
