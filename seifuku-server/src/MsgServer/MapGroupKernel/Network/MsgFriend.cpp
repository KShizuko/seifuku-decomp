#include "AllMsg.h"
#include "MsgFriend.h"
#include "MapGroup.h"
#pragma	warning(disable:4786)

CMsgFriend::CMsgFriend()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgFriend::~CMsgFriend()
{

}

BOOL CMsgFriend::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_FRIEND != this->GetType())
		return false;

	return true;
}

BOOL CMsgFriend::Create(int nAction, OBJID idFriend, DWORD dwPeerage, DWORD dwSex, LPCTSTR szName, bool bOnline )
{
	CHECKF(idFriend && nAction && szName);

	this->Init();
	m_pInfo->idFriend		= idFriend;
	m_pInfo->ucAction		= nAction;
	m_pInfo->ucOnlineFlag	= bOnline;
	SafeCopy(m_pInfo->szName, szName, _MAX_NAMESIZE);

	m_unMsgType	=_MSG_FRIEND;
	m_unMsgSize	=sizeof(MSG_Info);

	return true;
}

void CMsgFriend::Process(void *pInfo)
{
#ifdef _MSGDEBUG
	::LogMsg("Process CMsgFriend, id:%u", m_pInfo->id);
#endif

	CUserPtr pUser = UserManager()->GetUser(this);
	if (!pUser)
	{
		if(IsNpcMsg())
			return;

		switch(m_pInfo->ucAction)
		{
		case _FRIEND_GETINFO:
			{
				OBJID idFriend = m_pInfo->idFriend;
				CUser* pTarget = UserManager()->GetUser(idFriend);
				if(pTarget)
				{
					IF_OK(Create(_FRIEND_ONLINE, idFriend, pTarget->GetPeerageRank(), pTarget->GetSex(), pTarget->GetName()))
						SendMsg(this);
				}
			}
			break;
		case _ENEMY_ADD:
			{
				OBJID idFriend = m_pInfo->idFriend;
				CUser* pTarget = UserManager()->GetUser(idFriend);
				if(pTarget)
				{
					IF_OK(Create(_ENEMY_ONLINE, idFriend, pTarget->GetPeerageRank(), pTarget->GetSex(), m_pInfo->szName))
						SendMsg(this);
				}
			}
			break;
		case _FRIEND_BREAK:
			{
				OBJID idFriend = m_pInfo->idFriend;
				CUser* pTarget = UserManager()->GetUser(idFriend);
				if(pTarget)
				{
					pTarget->DelFriend(m_pInfo->idTransmit);
				}
			}
			break;
		}

		return;
	}
	if (!pUser->IsAlive())
	{
		pUser->SendSysMsg(STR_DIE);
		return;
	}

	switch(m_pInfo->ucAction)
	{
	case _FRIEND_APPLY:
		{
			CUser* pTarget = NULL;
			if(!pUser->GetMap()->QueryObj(pUser->GetPosX(), pUser->GetPosY(), OBJ_USER, m_pInfo->idFriend, IPP_OF(pTarget)))
				break;

			if(pUser->GetFriendAmount() >= _MAX_USERFRIENDSIZE)
			{
				pUser->SendSysMsg(STR_FRIEND_LIST_FULL);
				break;
			}

			if(pUser->GetFriend(m_pInfo->idFriend))
			{
				pUser->SendSysMsg(STR_YOUR_FRIEND_ALREADY);
				break;
			}

			if(pTarget->FetchApply(CUser::APPLY_FRIEND) != pUser->GetID())
			{
				pUser->SetApply(CUser::APPLY_FRIEND, m_pInfo->idFriend);

				pTarget->SendSysMsg(STR_TO_MAKE_FRIEND, pUser->GetName());
				pUser->SendSysMsg(STR_MAKE_FRIEND_SENT);
			}
			else
			{
				if (pUser->AddFriend(m_pInfo->idFriend, m_pInfo->szName) &&
						pTarget->AddFriend(pUser->GetID(), pUser->GetName()))
				{
					if(this->Create(_FRIEND_GETINFO, pTarget->GetID(), pTarget->GetPeerageRank(), pTarget->GetSex(), pTarget->GetName(), ONLINE_STATUS))
						pUser->SendMsg(this);

					if(this->Create(_FRIEND_GETINFO, pUser->GetID(), pUser->GetPeerageRank(), pUser->GetSex(), pUser->GetName(), ONLINE_STATUS))
						pTarget->SendMsg(this);

					MSGBUF	szMsg;
					sprintf(szMsg, STR_MAKE_FRIEND, pUser->GetName(), pTarget->GetName());
					pUser->BroadcastRoomMsg(szMsg, true);
				}
				else
				{
					::LogSave("Error: add friend failed");
				}
			}
		}
		break;
	case _FRIEND_ONLINE:
		{
			ASSERT(!"_FRIEND_ONLINE");
		}
		break;
	case _FRIEND_OFFLINE:
		{
			ASSERT(!"_FRIEND_OFFLINE");
		}
		break;

	case _FRIEND_BREAK:
		{
			CUser* pTarget = UserManager()->GetUser(m_pInfo->idFriend);

			CFriendPtr pFriend = pUser->GetFriend(m_pInfo->idFriend);
			if (!pFriend)
			{
				if(pTarget)
					pUser->SendSysMsg(STR_NOT_YOUR_FRIEND, pTarget->GetName());
				break;
			}

			NAMESTR	szFriend = "";
			SafeCopy(szFriend, pFriend->GetFriendName(), _MAX_NAMESIZE);
			if (pUser->DelFriend(m_pInfo->idFriend))
			{
				MSGBUF	szMsg;
				sprintf(szMsg, STR_BREAK_FRIEND, pUser->GetName(), szFriend);
				pUser->BroadcastRoomMsg(szMsg, true);

				pUser->SendMsg(this);
			}

			if (pTarget)
			{
				if(pTarget->DelFriend(pUser->GetID()))
				{
					if(this->Create(_FRIEND_BREAK, pUser->GetID(), pUser->GetPeerageRank(), pUser->GetSex(), pUser->GetName()))
						pTarget->SendMsg(this);
				}
			}
			else
			{
				SQLBUF	szSQL;
				sprintf(szSQL, "DELETE FROM cq_friend WHERE userid=%u && friend=%u", m_pInfo->idFriend, pUser->GetID());
				Database()->ExecuteSQL(szSQL);

				m_pInfo->idTransmit	= pUser->GetID();
				MapGroup(PID)->QueryIntraMsg()->TransmitMsg(this, GetSocketID(), GetNpcID());
			}
		}
		break;

	case _FRIEND_GETINFO:
		{
			ASSERT(!"_FRIEND_GETINFO");
		}
		break;
	case _ENEMY_DEL:
		{
			pUser->QueryEnemy()->Del(m_pInfo->idFriend, UPDATE_TRUE);
		}
		break;
	}
}
