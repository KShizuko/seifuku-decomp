#include "AllMsg.h"
#include "Item.h"
#pragma	warning(disable:4786)

CMsgItemInfoEx::CMsgItemInfoEx()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgItemInfoEx::~CMsgItemInfoEx()
{

}

BOOL CMsgItemInfoEx::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_ITEMINFOEX != this->GetType())
		return false;

	return true;
}

BOOL CMsgItemInfoEx::Create(CItem* pItem, OBJID idOwner, int nCost, int nAction )
{
	CHECKF(pItem);
	ItemInfoStruct info;
	CHECKF(pItem->GetInfo(&info));

	this->Init();

	m_pInfo->id					= info.id;
	m_pInfo->idOwner			= idOwner;
	m_pInfo->dwPrice			= nCost;
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

	m_unMsgType	=_MSG_ITEMINFOEX;
	m_unMsgSize	=sizeof(MSG_Info);

	return true;
}

void CMsgItemInfoEx::Process(void *pInfo)
{
	ASSERT(!"CMsgItemInfoEx::Process()");
	return;

#ifdef _MSGDEBUG
	::LogMsg("Process CMsgItemInfoEx, id:%u", m_info.id);
#endif
}
