#include "AllMsg.h"
#include "package.h"
#include "MapGroup.h"
#include "Npc.h"

CMsgPackage::CMsgPackage()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgPackage::~CMsgPackage()
{

}
BOOL CMsgPackage::Create(int nAction,OBJID id , OBJID idItem)
{
	CHECKF(id != ID_NONE && idItem != ID_NONE);

	this->Init();

	m_unMsgSize		= sizeof(MSG_Info);
	m_unMsgType		= _MSG_PACKAGE;

	m_pInfo->id			= id;
	m_pInfo->ucAction	= nAction;
	m_pInfo->ucType		= MSGPACKAGE_TYPE_NONE;
	m_pInfo->idItem		= idItem;

	return true;
}

BOOL CMsgPackage::Create(int nAction, OBJID id, const MsgPackageItemInfo* buf, int nSize)
{
	CHECKF(buf && nSize >= 0 && nSize <= PACKAGE_LIMIT);

	this->Init();

	m_unMsgSize		= sizeof(MSG_Info) + (nSize-1) * sizeof(MsgPackageItemInfo);
	if(m_unMsgSize > _MAX_MSGSIZE)
	{
		LOGERROR("CMsgPackage::Create() MsgSize Overflow at %s, %d", __FILE__, __LINE__);
		return false;
	}
	m_unMsgType		= _MSG_PACKAGE;

	m_pInfo->id			= id;
	m_pInfo->ucAction	= nAction;
	m_pInfo->usSize		= nSize;

	if(nSize > 0)
		memcpy(&m_pInfo->setItemInfo, buf, nSize * sizeof(MsgPackageItemInfo));

	return true;
}

BOOL CMsgPackage::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_PACKAGE != this->GetType())
		return false;

	return true;
}

void CMsgPackage::Process(void* pInfo)
{
	DEBUG_TRY
	CUserPtr pUser = UserManager()->GetUser(this);
	CHECK(pUser);

	switch(m_pInfo->ucType)
	{
	case	MSGPACKAGE_TYPE_STORAGE:
		switch(m_pInfo->ucAction)
		{
		case	MSGPACKAGE_QUERYLIST:
			{
				OBJID	idNpc = m_pInfo->id;
				CNpc* pNpc = NULL;
				if(!pUser->GetMap()->QueryObj(pUser->GetPosX(), pUser->GetPosY(), OBJ_NPC, idNpc, IPP_OF(pNpc)))
					return ;
				if(!pNpc->IsStorageNpc())
					return ;

				pUser->QueryStorage()->SendStorageInfo(idNpc);
			}
			break;
		case	MSGPACKAGE_CHECKIN:
			{
				OBJID	idItem = m_pInfo->idItem;
				CItemPtr pItem = pUser->GetItem(idItem);
				if(!pItem || !pItem->IsStorageEnable())
				{
					pUser->SendSysMsg(STR_CANT_STORAGE);
					return ;
				}

				OBJID	idNpc = m_pInfo->id;
				CNpc* pNpc = NULL;
				if(!pUser->GetMap()->QueryObj(pUser->GetPosX(), pUser->GetPosY(), OBJ_NPC, idNpc, IPP_OF(pNpc)))
					return ;
				if(!pNpc->IsStorageNpc())
					return ;
				int nSize = pNpc->GetStorageSize();
				if(nSize == 0)
					nSize	= USER_PACKAGE_SIZE;
				if(pUser->QueryStorage()->GetStorageCount() >= nSize)
					return ;

				DEBUG_TRY
				pUser->PopItem(idItem, SYNCHRO_TRUE, UPDATE_FALSE);
				pUser->QueryStorage()->CheckIn(idNpc, pItem, UPDATE_TRUE);
				DEBUG_CATCH("MSGPACKAGE_CHECKIN")
			}
			break;
		case	MSGPACKAGE_CHECKOUT:
			{
				OBJID	idNpc = m_pInfo->id;
				OBJID	idItem = m_pInfo->idItem;
				CNpc* pNpc = NULL;
				if(!pUser->GetMap()->QueryObj(pUser->GetPosX(), pUser->GetPosY(), OBJ_NPC, idNpc, IPP_OF(pNpc)))
					return ;
				if(!pNpc->IsStorageNpc())
					return ;
				if(!pUser->IsBackPackSpare(1, 10))
					return ;

				DEBUG_TRY
				CItemPtr pItem = pUser->QueryStorage()->CheckOut(idNpc, idItem, UPDATE_TRUE);
				if(!pItem)
					return ;
				pUser->AddItem(pItem, SYNCHRO_TRUE);
				DEBUG_CATCH("MSGPACKAGE_CHECKIN")
			}
			break;
		default:
			ASSERT(!"action");
		}
		break;
	case	MSGPACKAGE_TYPE_TRUNK:
		switch(m_pInfo->ucAction)
		{
		case	MSGPACKAGE_QUERYLIST:
			{
				OBJID	idNpc = m_pInfo->id;
				CNpc* pNpc = NULL;
				if(!pUser->GetMap()->QueryObj(pUser->GetPosX(), pUser->GetPosY(), OBJ_NPC, idNpc, IPP_OF(pNpc)))
					return ;
				if(!pNpc->IsTrunkNpc())
					return ;

				pNpc->QueryTrunk()->QueryPackage()->SendInfo(pUser);
			}
			break;
		case	MSGPACKAGE_CHECKIN:
			{
				OBJID	idItem = m_pInfo->idItem;
				CItemPtr pItem = pUser->GetItem(idItem);
				if(!pItem || !pItem->IsExchangeEnable())
				{
					pUser->SendSysMsg(STR_CANT_STORAGE);
					return ;
				}

				OBJID	idNpc = m_pInfo->id;
				CNpc* pNpc = NULL;
				if(!pUser->GetMap()->QueryObj(pUser->GetPosX(), pUser->GetPosY(), OBJ_NPC, idNpc, IPP_OF(pNpc)))
					return ;
				if(!pNpc->IsTrunkNpc())
					return ;
				if(!pNpc->IsOwnerOf(pUser))
				{
					pUser->SendSysMsg(STR_CHECKIN_DISABLE);
					return ;
				}
				if(pItem->TestSpecialFlag(2) || pItem->TestSpecialFlag(1))
					return ;
				if(pNpc->QueryTrunk()->IsPackageFull())
					return ;

				DEBUG_TRY
				pUser->PopItem(idItem, SYNCHRO_TRUE, UPDATE_FALSE);
				pNpc->QueryTrunk()->QueryPackage()->CheckIn(pUser, pItem, UPDATE_TRUE);
				DEBUG_CATCH("MSGPACKAGE_CHECKIN")

				if(pNpc->IsShelfNpc())
					pNpc->SendSelfToBlock();

				if (pItem->IsNonsuchItem())
				{
					::MyLogSave("gmlog/Trunk", "%s(%u) checkin item:[id=%u, type=%u], dur=%d, max_dur=%d",
							pUser->GetName(),
							pUser->GetID(),
							pItem->GetID(),
							pItem->GetInt(ITEMDATA_TYPE),
							pItem->GetInt(ITEMDATA_AMOUNT),
							pItem->GetInt(ITEMDATA_AMOUNTLIMIT));
				}
			}
			break;
		case	MSGPACKAGE_CHECKOUT:
			{
				OBJID	idNpc = m_pInfo->id;
				OBJID	idItem = m_pInfo->idItem;
				CNpc* pNpc = NULL;
				if(!pUser->GetMap()->QueryObj(pUser->GetPosX(), pUser->GetPosY(), OBJ_NPC, idNpc, IPP_OF(pNpc)))
					return ;
				if(!pNpc->IsTrunkNpc())
					return ;
				if(!pNpc->IsOwnerOf(pUser, CNpc::MATE_ENABLE))
					return ;
				if(pUser->IsBackPackFull())
					return ;

				DEBUG_TRY
				CItemPtr pItem = pNpc->QueryTrunk()->QueryPackage()->CheckOut(pUser, idItem, UPDATE_FALSE);
				if(!pItem)
					return ;
				pUser->AddItem(pItem, SYNCHRO_TRUE);

				if (pItem->IsNonsuchItem())
				{
					::MyLogSave("gmlog/Trunk", "%s(%u) checkout item:[id=%u, type=%u], dur=%d, max_dur=%d",
							pUser->GetName(),
							pUser->GetID(),
							pItem->GetID(),
							pItem->GetInt(ITEMDATA_TYPE),
							pItem->GetInt(ITEMDATA_AMOUNT),
							pItem->GetInt(ITEMDATA_AMOUNTLIMIT));
				}
				DEBUG_CATCH("MSGPACKAGE_CHECKOUT")

				if(pNpc->IsShelfNpc())
					pNpc->SendSelfToBlock();
			}
			break;
		default:
			ASSERT(!"action");
		}
		break;
	case	MSGPACKAGE_TYPE_CHEST:
		switch(m_pInfo->ucAction)
		{
		case	MSGPACKAGE_QUERYLIST:
			((IUserChest*)pUser)->SendChestInfo(m_pInfo->id);
			break;
		case	MSGPACKAGE_CHECKIN:
			((IUserChest*)pUser)->CheckInChest(m_pInfo->id, m_pInfo->idItem);
			break;
		case	MSGPACKAGE_CHECKOUT:
			((IUserChest*)pUser)->CheckOutChest(m_pInfo->id, m_pInfo->idItem);
			break;
		default:
			ASSERT(!"action");
		}
		break;
	default:
		LOGERROR("MsgPackage ucType:%d, ucAction:%d at %s, %d", m_pInfo->ucType, m_pInfo->ucAction, __FILE__, __LINE__);
	}
	DEBUG_CATCH3("MsgPackage. type[%d], subtype[%d]", m_pInfo->ucType, m_pInfo->ucAction)
}
