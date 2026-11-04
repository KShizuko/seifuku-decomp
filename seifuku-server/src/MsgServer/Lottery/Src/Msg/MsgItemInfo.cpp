#include "MsgItemInfo.h"
#include "common.h"

CMsgItemInfo::CMsgItemInfo()
{
	Init();
	m_pInfo	= (MSG_Info*)m_bufMsg;
}

bool CMsgItemInfo::Create(ILotteryPrize* pPrize, int nAction , OBJID idUser )
{
	CHECKF(pPrize);
	PLUGIN_ITEMINFO info;
	pPrize->GetInfo(&info);

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

	m_pInfo->unMsgSize			= sizeof(MSG_Info);
	m_pInfo->unMsgType			= _MSG_ITEMINFO;
	return true;
}

void CMsgItemInfo::Process(void* pInfo)
{
	MYASSERT(!"CMsgItemInfo::Process()");
}
