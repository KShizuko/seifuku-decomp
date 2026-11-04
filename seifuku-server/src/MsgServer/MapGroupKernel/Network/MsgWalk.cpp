#include "AllMsg.h"
#include "mapgroup.h"
#include "transformation.h"
#include "I_Role.h"

CMsgWalk::CMsgWalk()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgWalk::~CMsgWalk()
{

}

BOOL CMsgWalk::Create(OBJID idPlayer, int nDir, int nMode )
{
	if (idPlayer == ID_NONE)
		return false;

	this->Init();

	m_unMsgSize	=sizeof(MSG_Info);
	m_unMsgType	=_MSG_WALK;

	m_pInfo->idUser		=idPlayer;
	m_pInfo->ucDir		=nDir;
	m_pInfo->ucMode		=nMode;

	return true;
}

BOOL CMsgWalk::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_WALK != this->GetType())
		return false;

	return true;
}

void CMsgWalk::Process(void *pInfo)
{
	DEBUG_TRY
#ifdef _MSGDEBUG
	::LogMsg("Process CMsgWalk, idUser:%u, data:%u",
					m_pInfo->idUser,
					m_pInfo->dwData);
#endif

	if(!this->IsNpcMsg())
	{
		OBJID idUser = m_pInfo->idUser;
		if(!(idUser >= 700001 && idUser <= 799999) && !(idUser >= 1000000 && idUser <= 3999999999u))
		{
			IRole* pRole	= RoleManager()->QueryRole(this, idUser);
			CUser* pUser	= NULL;
			if(pRole && pRole->QueryObj(OBJ_USER, IPP_OF(pUser)))
			{
				pUser->SetCheatMark(7, 3);
				pUser->SetCheatKickout(RandGet(300) + 30);
			}
			return ;
		}
	}

	IRole* pRole	= RoleManager()->QueryRole(this, m_pInfo->idUser);
	if(!pRole)
	{
		if(!this->IsNpcMsg())
			LOGERROR("CMsgWalk: player not found, SOCKET_ID[%u],NPC_ID[%u]", GetSocketID(), GetNpcID());
		return ;
	}

	CUser* pUser = NULL;
	if(pRole->QueryObj(OBJ_USER, IPP_OF(pUser)))
	{
		if(!pRole->IsAlive() && !pUser->IsGhost())
		{
			pRole->SendSysMsg(STR_DIE);
			return ;
		}
		if (pUser->QueryTransformation() && !pUser->QueryTransformation()->IsMoveEnable())
		{
			pUser->SendSysMsg(STR_YOUR_CANNOT_WALK);
			pUser->KickBack();
			return;
		}
	}
	else
	{
		CNpc* pNpc = NULL;
		if(pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)))
			return ;
		if(!pRole->IsAlive())
			return ;
	}

	DEBUG_TRY
	if(m_pInfo->ucMode != MOVEMODE_SHIFT)
		pRole->ProcessOnMove(m_pInfo->ucMode);
	DEBUG_CATCH("ProcessOnMove")

	DEBUG_TRY
	pRole->BroadcastRoomMsg(this, INCLUDE_SELF);
	pRole->MoveToward(m_pInfo->ucDir % 8);
	DEBUG_CATCH("MoveToward")

	DEBUG_CATCH("CMsgWalk::Process")
}
