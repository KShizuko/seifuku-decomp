#include "AllMsg.h"
#include "MapGroup.h"
#include "ItemType.h"
#pragma	warning(disable:4786)

CMsgItemInfo::CMsgItemInfo()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgItemInfo::~CMsgItemInfo()
{

}

BOOL CMsgItemInfo::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_ITEMINFO != this->GetType())
		return false;

	return true;
}
BOOL CMsgItemInfo::Create(CItem* pItem, int nAction , OBJID idUser )
{
	CHECKF(pItem);
	ItemInfoStruct info;
	CHECKF(pItem->GetInfo(&info));

	this->Init();

	m_pInfo->id					= info.id;
	if(idUser != ID_NONE)
		m_pInfo->id					= idUser;
	m_pInfo->dwType				= info.idType;
	m_pInfo->usAmount			= info.nAmount;
	m_pInfo->usAmountLimit		= info.nAmountLimit;
	m_pInfo->ucAction			= nAction;
	m_pInfo->ucIdent			= info.nIdent;
	m_pInfo->ucPosition			= info.nPosition;
	m_pInfo->nData				= info.nData;
	m_pInfo->ucGem1				= info.nGem1;
	m_pInfo->ucGem2				= info.nGem2;
	m_pInfo->ucMagic1			= info.nMagic1;
	m_pInfo->ucMagic2			= info.nMagic2;
	m_pInfo->ucMagic3			= info.nMagic3;
	if(m_pInfo->ucIdent & _ITEM_STATUS_NOT_IDENT)
	{
		if(info.nGem1)
			m_pInfo->ucGem1		= 0xFF;
		if(info.nGem2)
			m_pInfo->ucGem2		= 0xFF;
		m_pInfo->ucMagic1		= 0;
		m_pInfo->ucMagic2		= 0;
		m_pInfo->ucMagic3		= 0;
	}
	m_pInfo->ucReduceDmg		= info.nReduceDmg;
	m_pInfo->ucAddLife			= info.nAddLife;

	m_unMsgType	=_MSG_ITEMINFO;
	m_unMsgSize	=sizeof(MSG_Info);

	return true;
}

void CMsgItemInfo::Process(void *pInfo)
{
	ASSERT(!"CMsgItemInfo::Process()");
	return;

#ifdef _MSGDEBUG
	::LogMsg("Process CMsgItemInfo, id:%u", m_info.id);
#endif
}
