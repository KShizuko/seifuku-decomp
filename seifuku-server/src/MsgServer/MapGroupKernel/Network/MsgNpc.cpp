#include "allmsg.h"
#include "Npc.h"
#include "NpcManager.h"
#include "GameMap.h"
#include "MapGroup.h"

CMsgNpc::CMsgNpc()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgNpc::~CMsgNpc()
{

}

BOOL CMsgNpc::Create(int nAction, OBJID id, int nType , DWORD dwData )
{
	this->Init();

	m_unMsgSize	=sizeof(MSG_Info);
	m_unMsgType	=_MSG_NPC;

	m_pInfo->id			= id;
	m_pInfo->usAction	= (unsigned short)nAction;
	m_pInfo->usType		= nType;
	m_pInfo->dwData		= dwData;

	return true;
}

void CMsgNpc::Process(void* pInfo)
{
#ifdef _MYDEBUG
	::LogSave("Process CMsgName, id:%u, usAction:%u", 
		m_pInfo->id,
		m_pInfo->usAction);

#endif

	if (!pInfo)
		return;

	CUserPtr pUser	= UserManager()->GetUser(this);
	if (!pUser)
		return ;

	if(!pUser->IsAlive())
	{
		pUser->SendSysMsg(STR_DIE);
		return ;
	}

	switch(m_pInfo->usAction)
	{
	case EVENT_BEACTIVED:
		{
			if(!pUser->GetMap()->QueryNpc(pUser->GetPosX(), pUser->GetPosY(), m_pInfo->id, 20))
				return ;

			CNpc* pNpc;
			IRole* pRole = RoleManager()->QuerySet()->GetObj(m_pInfo->id);
			if (pRole && pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pUser->GetMapID() == pNpc->GetMapID())
			{
				pNpc->ActivateNpc(pUser->QueryRole(), 0);
			}
		}
		break;
	case EVENT_DELNPC:
		{
			CNpc* pNpc;
			IRole* pRole = RoleManager()->QuerySet()->GetObj(m_pInfo->id);
			if (pRole && pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pUser->GetMapID() == pNpc->GetMapID())
			{
				if(pNpc->IsDeleted())
					return ;

				if(pNpc->IsDelEnable() && pNpc->IsOwnerOf(pUser))
				{
					if(pNpc->IsRecycleEnable())
					{
						if(pUser->IsBackPackFull())
						{
							pUser->SendSysMsg(STR_YOUR_BAG_FULL);
							return ;
						}

						CItem* pItem = pNpc->Recycle(pUser->GetID());
						if(pItem)
						{
							pUser->AddItem(pItem, SYNCHRO_TRUE);
							pUser->SendSysMsg(STR_GOT_ITEM, pItem->GetStr(ITEMDATA_NAME));
						}
						else
							pUser->SendSysMsg(STR_ITEM_DAMAGED);
					}
					if(!pNpc->DelNpc())
						pUser->SendSysMsg(STR_DELETE_FAILED);
				}
			}
		}
		break;
	case EVENT_CHANGEPOS:
		{
			CNpc* pNpc;
			IRole* pRole = RoleManager()->QuerySet()->GetObj(m_pInfo->id);
			if (pRole && pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pUser->GetMapID() == pNpc->GetMapID())
			{
				int	nLook = ::MaskLook(m_pInfo->usType);
				if(::MaskLook(pNpc->GetLookFace()) == nLook)
					pNpc->ChangeDir(nLook);
				pNpc->TransPos(m_pInfo->usPosX, m_pInfo->usPosY);
			}
		}
		break;

	default:
		::LogSave("ERROR: CMsgNpc::Process() internal exception");
	}
}
