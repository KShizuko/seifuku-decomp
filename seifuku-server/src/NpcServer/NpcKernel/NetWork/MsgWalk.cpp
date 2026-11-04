#include "AllMsg.h"
#include "npcworld.h"

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

	if(_MSG_ACTION != this->GetType())
		return false;

	return true;
}

void CMsgWalk::Process(void *pInfo)
{
	if(IsSceneID(m_pInfo->idUser))
	{
		MYASSERT(!"CMsgWalk");
	}
	else if(IsNpcID(m_pInfo->idUser))
	{
	}
	else
	{
		CUserPtr pUser = UserManager()->QueryUser(m_pInfo->idUser);
		if(pUser)
		{
			pUser->MoveForward(m_pInfo->ucDir);

#ifdef	PALED_DEBUG_X
			MSGBUF	szMsg;
			sprintf(szMsg, "MSGWALK: player, %s(%d,%d)", pUser->GetName(), pUser->GetPosX(), pUser->GetPosY());
			LOGWARNING(szMsg);
#endif
		}
		else
		{
#ifdef	PALED_DEBUG_X
			MSGBUF	szMsg;
			sprintf(szMsg, "MSGWALK: AGENT, %d(%d)", m_pInfo->idUser, m_pInfo->ucDir);
			LOGWARNING(szMsg);
#endif
		}
	}
}
