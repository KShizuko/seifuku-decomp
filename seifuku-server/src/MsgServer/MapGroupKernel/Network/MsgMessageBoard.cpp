#include "AllMsg.h"
#ifdef	WORLD_KERNEL
#include "WorldKernel.h"
#include "MessageBoard.h"
using namespace world_kernel;
#include "userlist.h"
#else
#pragma warning(disable:4786)
#include "MapGroup.h"
#include "usermanager.h"
#endif

const int	MSGTITLE_SIZE		= 44;
const int	MSGLIST_SIZE		= 5;

CMsgMessageBoard::CMsgMessageBoard()
{
	Init();
	m_pInfo	=(MSG_Info* )m_bufMsg;

	m_StrPacker.SetBuf(m_pInfo->szBuf, _MAX_MSGSIZE-sizeof(MSG_Info)+1);
}

CMsgMessageBoard::~CMsgMessageBoard()
{

}

void CMsgMessageBoard::Process(void *pInfo)
{

#ifdef _MSGDEBUG
	::LogMsg("Process CMsgMessageBoard, Sender:%s, Receiver:%s, Words:%s", 
						szSender, 
						szReceiver, 
						szWords);
#endif

	if(m_pInfo->usChannel == _TXTATR_MSG_SYN)
	{
		CUser* pUser = UserManager()->GetUser(this);
		if(!pUser)
			return ;
		OBJID idSyn = pUser->GetSynID();
		if(idSyn == ID_NONE)
			return ;

		SetTransData(idSyn);
	}

	MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(this);
}
