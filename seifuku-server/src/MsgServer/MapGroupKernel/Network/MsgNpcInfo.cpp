#include "allmsg.h"
#include "Npc.h"
#include "NpcManager.h"
#include "GameMap.h"
#include "MapGroup.h"

CMsgNpcInfo::CMsgNpcInfo()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;

	m_StrPacker.SetBuf(m_pInfo->szBuf, _MAX_MSGSIZE-sizeof(MSG_Info)+1);
}

CMsgNpcInfo::~CMsgNpcInfo()
{

}

BOOL CMsgNpcInfo::Create(CNpc* pNpc)
{
	this->Init();

	m_pInfo->id			= pNpc->GetID();
	m_pInfo->usType		= pNpc->GetType();
	m_pInfo->usSort		= pNpc->GetSort();
	m_pInfo->usLook		= pNpc->GetLookFace();
	m_pInfo->usCellX	= pNpc->GetPosX();
	m_pInfo->usCellY	= pNpc->GetPosY();

	if(0)
	{
		m_StrPacker.AddString(pNpc->GetName());
	}

	m_unMsgType	=_MSG_NPCINFO;
	m_unMsgSize	=sizeof(MSG_Info)-1+m_StrPacker.GetSize();

	return true;
}

BOOL CMsgNpcInfo::Create(OBJID id, int nType, int nSort, int nLookFace, int nCellX, int nCellY, const char* pszName )
{
	this->Init();

	m_pInfo->id			= id;
	m_pInfo->usType		= (unsigned short)nType;
	m_pInfo->usSort		= (unsigned short)nSort;
	m_pInfo->usLook		= (unsigned short)nLookFace;
	m_pInfo->usCellX	= (unsigned short)nCellX;
	m_pInfo->usCellY	= (unsigned short)nCellY;

	if(pszName)
	{
		m_StrPacker.AddString(pszName);
	}

	m_unMsgType	=_MSG_NPCINFO;
	m_unMsgSize	=sizeof(MSG_Info)-1+m_StrPacker.GetSize();

	return true;
}

void CMsgNpcInfo::Process(void* pInfo)
{
#ifdef _MYDEBUG
	::LogSave("Process CMsgNpcInfo: ID:0x:%x, Type:%d, LookType:%d, CellX:%d, CellY:%d, Name:%s",
				m_pInfo->id	, m_pInfo->ucType,
				m_pInfo->usLook, m_pInfo->usCellX, 
				m_pInfo->usCellY, m_pInfo->szName);

#endif
	CUserPtr pUser = UserManager()->GetUser(this);
	if(!pUser)
		return;

	MSGBUF	szMsg;
	sprintf(szMsg, "%d %d %d %d %d", 
			m_pInfo->usCellX, m_pInfo->usCellY, 
			m_pInfo->usLook, m_pInfo->usType, m_pInfo->usSort);

	OBJID	idTaskItem = pUser->GetTaskItemID();
	if(idTaskItem == ID_NONE)
		return ;
	CItem* pItem = pUser->GetItem(idTaskItem);
	if(!pItem)
		return ;

	OBJID idAction = pUser->GetTaskItemActionID();
	if(idAction == ID_NONE)
		return ;

	GameAction()->ProcessAction(idAction, pUser, NULL, pItem, szMsg);
}
