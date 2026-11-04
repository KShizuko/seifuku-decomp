#include "AllMsg.h"
#include "MsgMapItem.h"
#include "MapGroup.h"
#include "MapItem.h"
#include "MapTrap.h"

CMsgMapItem::CMsgMapItem()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgMapItem::~CMsgMapItem()
{

}

BOOL CMsgMapItem::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_MAPITEM != this->GetType())
		return false;

	return true;
}

BOOL CMsgMapItem::Create(int nAction, CMapItem* pMapItem)
{
	CHECKF(pMapItem && nAction);

	this->Init();

	m_pInfo->nAction	= nAction;
	m_pInfo->id			= pMapItem->GetID();
	m_pInfo->idType		= pMapItem->GetType();
	m_pInfo->nPosX		= pMapItem->GetPosX();
	m_pInfo->nPosY		= pMapItem->GetPosY();

	m_unMsgType	=_MSG_MAPITEM;
	m_unMsgSize	=sizeof(MSG_Info);

	return true;
}

BOOL CMsgMapItem::Create(int nAction, CMapTrap* pTrap)
{
	CHECKF(pTrap && nAction);

	this->Init();

	m_pInfo->nAction	= nAction;
	m_pInfo->id			= pTrap->GetID();
	m_pInfo->usLook		= pTrap->GetLookFace();
	m_pInfo->nPosX		= pTrap->GetPosX();
	m_pInfo->nPosY		= pTrap->GetPosY();

	m_unMsgType	=_MSG_MAPITEM;
	m_unMsgSize	=sizeof(MSG_Info);

	return true;
}

BOOL CMsgMapItem::Create(OBJID id, int nPosX, int nPosY, int nAction)
{
	if (id == ID_NONE)
		return false;

	this->Init();

	m_pInfo->nAction	= nAction;
	m_pInfo->id			= id;
	m_pInfo->nPosX		= nPosX;
	m_pInfo->nPosY		= nPosY;

	m_unMsgType	= _MSG_MAPITEM;
	m_unMsgSize	= sizeof(MSG_Info);

	return true;
}

void CMsgMapItem::Process(void *pInfo)
{
	CUserPtr pUser = UserManager()->GetUser(this);
	if(!pUser)
		return;

	if(!pUser->IsAlive())
	{
		pUser->SendSysMsg(STR_DIE);
		return ;
	}

	switch(m_pInfo->nAction)
	{
	case MSGMAPITEM_PICK:
		{
			if (pUser->SynPosition(m_pInfo->nPosX, m_pInfo->nPosY, 0))
			{
				pUser->PickMapItem(m_pInfo->id);
			}
			else
			{
				UserManager()->KickOutSocket(m_idSocket, "ITEM_PICK SynPosition");
			}
		}
		break;
	default:
		{
			ASSERT(!"switch");
		}
	}
	return;

#ifdef _MSGDEBUG
	::LogMsg("Process CMsgMapItem, id:%u", m_pInfo->id);
#endif
}
