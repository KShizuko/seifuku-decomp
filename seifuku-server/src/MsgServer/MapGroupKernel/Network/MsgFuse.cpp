#include "AllMsg.h"
#include "MsgFuse.h"
#include "mapgroup.h"
#include "user.h"
#include "npc.h"

CMsgFuse::CMsgFuse()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgFuse::~CMsgFuse()
{
}

BOOL	CMsgFuse::Create		(char* pMsgBuf, DWORD dwSize)
{
	if (!CNetMsg::Create(pMsgBuf, dwSize))
		return false;

	return _MSG_FUSE == this->GetType();
}

void	CMsgFuse::Process		(void* pInfo)
{
	CUserPtr pUser = UserManager()->GetUser(this);
	if (!pUser || !pUser->IsAlive())
		return;

	IRole* pRole = RoleManager()->QuerySet()->GetObj(m_pInfo->idNpc);
	if (!pRole)
		return;

	CNpc* pNpc = NULL;
	if (!pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)))
		return;

	if (pNpc->GetMapID() != pUser->GetMapID())
		return;

	pUser->FuseItem(m_pInfo->idItem, m_pInfo->usAmount, m_pInfo->usType);
}
