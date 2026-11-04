#include "allmsg.h"
#include "Npc.h"
#include "NpcWorld.h"
#include "basefunc.h"

CMsgNpc::CMsgNpc()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgNpc::~CMsgNpc()
{

}

BOOL CMsgNpc::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if(!pbufMsg)
		return false;

	memcpy(this->m_bufMsg, pbufMsg, dwMsgSize);
	return true;
}

void CMsgNpc::Process(void* pSocket)
{
#ifdef _MYDEBUG
	::LogSave("Process CMsgNpc, id:%u, usAction:%u", 
		m_pInfo->id,
		m_pInfo->usAction);

#endif

	if (!pSocket)
		return;

	INpc* pNpc	= NpcManager()->QuerySet()->GetObj(m_pInfo->id);
	if (!pNpc)
		return ;

	switch(m_pInfo->usAction)
	{
	case MSGNPC_DELNPC:
		{
		}
		break;

	default:
		::LogSave("ERROR: CMsgNpc::Process() internal exception");
	}
}
