#include "AllMsg.h"
#include "GameMsg.h"
#include "Hero.h"
#include "Friend.h"
#include "GamePlayerSet.h"
#include "GameDataSet.h"

CMsgFriend::CMsgFriend()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgFriend::~CMsgFriend()
{

}

BOOL CMsgFriend::Create(int nAction, OBJID idFriend, BOOL bOnline, const char* pszName)
{
	MYASSERT (pszName);

	m_pInfo->ucAction		=(unsigned char)nAction;
	m_pInfo->idFriend		=idFriend;
	m_pInfo->ucOnlineFlag	=(unsigned char)bOnline;
	strcpy(m_pInfo->szName, pszName);

	m_pInfo->unMsgType	=_MSG_FRIEND;
	m_pInfo->unMsgSize	=sizeof(MSG_Info);

	return true;
}

BOOL CMsgFriend::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;
	if(_MSG_FRIEND != this->GetType())
		return false;

	return true;
}

void CMsgFriend::Process(void *pInfo)
{
#ifdef _MSGDEBUG
	::LogSave("Process CMsgGoodFriend,  Action:%d, Name:%s",
							m_pInfo->ucAction ,
							m_pInfo->szName);
#endif
	switch(m_pInfo->ucAction)
	{
	case _FRIEND_APPLY:
		{
			char szTemp[256];
			const int idApply = 100101;
			sprintf(szTemp, GameDataSetQuery()->GetStr(idApply),
						m_pInfo->szName);
			g_objGameMsg.AddMsg(szTemp);
		}
		break;
	case _FRIEND_ACCEPT:
		{
			CFriend* pFriend =CFriend::CreateNew(m_pInfo->idFriend,
												 m_pInfo->szName,
												 m_pInfo->ucOnlineFlag);
			if (pFriend)
			{
				g_objHero.AddFriend(pFriend);
				char szTemp[256];
				const int idAccept = 100102;
				sprintf(szTemp, GameDataSetQuery()->GetStr(idAccept),
									g_objHero.GetName(),
									pFriend->GetName());
				g_objGameMsg.AddMsg(szTemp);
			}
		}
		break;
	case _FRIEND_ONLINE:
		{
			CFriend* pFriend = g_objHero.GetFriend(m_pInfo->idFriend);
			if (pFriend)
			{
				pFriend->Online();
				::PostCmd(CMD_FLASHFRIENDLIST);
				const OBJID idFriendCome = 100103;
				g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idFriendCome), pFriend->GetName());
			}
		}
		break;
	case _FRIEND_OFFLINE:
		{
			CFriend* pFriend = g_objHero.GetFriend(m_pInfo->idFriend);
			if (pFriend)
			{
				pFriend->Offline();
				::PostCmd(CMD_FLASHFRIENDLIST);
				const OBJID idFriendLeave = 100104;
				g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idFriendLeave), pFriend->GetName());
			}
		}
		break;
	case _FRIEND_BREAK:
		{
			OBJID idFriend		=m_pInfo->idFriend;
			g_objHero.DelFriend(idFriend);
		}
		break;
	case _FRIEND_GETINFO:
		{
			CFriend* pFriend	=CFriend::CreateNew(m_pInfo->idFriend,
														m_pInfo->szName,
														m_pInfo->ucOnlineFlag);
			if (pFriend)
				g_objHero.AddFriend(pFriend);
		}
		break;
	case _ENEMY_ONLINE:
		{
			CEnemy* pEnemy = g_objHero.GetEnemy(m_pInfo->idFriend);
			if (pEnemy)
			{
				pEnemy->Online();
				::PostCmd(CMD_FLASHENEMYLIST);
				const OBJID idEnemyCome = 100144;
				g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idEnemyCome), pEnemy->GetName());
			}
		}
		break;
	case _ENEMY_OFFLINE:
		{
			CEnemy* pEnemy = g_objHero.GetEnemy(m_pInfo->idFriend);
			if (pEnemy)
			{
				pEnemy->Offline();
				::PostCmd(CMD_FLASHENEMYLIST);
				const OBJID idEnemyLeave = 100145;
				g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idEnemyLeave), pEnemy->GetName());
			}
		}		break;
	case _ENEMY_DEL:
		{
			OBJID idEnemy	=m_pInfo->idFriend;
			g_objHero.DelEnemy(idEnemy, false);
		}
		break;
	case _ENEMY_ADD:
		{
			CEnemy* pEnemy = g_objHero.GetEnemy(m_pInfo->idFriend);
			if(!pEnemy)
			{
				pEnemy	=CEnemy::CreateNew(m_pInfo->idFriend, m_pInfo->szName, m_pInfo->ucOnlineFlag);
				if (pEnemy)
					g_objHero.AddEnemy(pEnemy);
			}
		}
		break;
	}
}
