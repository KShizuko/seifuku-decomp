#include "AllMsg.h"
#include "WorldKernel.h"
using namespace world_kernel;
#include "userlist.h"

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

BOOL CMsgFriend::Create(int nAction, OBJID idFriend, DWORD dwData1, DWORD dwData2, LPCTSTR szName, bool bOnline )
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

	CPlayer* pUser = UserList()->GetPlayer(this->GetTransData());
	if (!pUser)
		return ;

	switch(m_pInfo->ucAction)
	{
	case _FRIEND_ONLINE:
		{
			pUser->SendMsg(this);
		}
		break;
	case _ENEMY_ONLINE:
		{
			pUser->SendMsg(this);
		}
		break;
	}
}
