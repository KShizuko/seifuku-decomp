#include "allmsg.h"
#include "Npc.h"
#include "NpcManager.h"
#include "GameMap.h"
#include "MapGroup.h"

CMsgNpcInfoEx::CMsgNpcInfoEx()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;

	m_StrPacker.SetBuf(m_pInfo->szBuf, _MAX_MSGSIZE-sizeof(MSG_Info)+1);
}

CMsgNpcInfoEx::~CMsgNpcInfoEx()
{

}

BOOL CMsgNpcInfoEx::Create(CNpc* pNpc)
{
	this->Init();

	m_pInfo->id			= pNpc->GetID();
	m_pInfo->usType		= pNpc->GetType();
	m_pInfo->usSort		= pNpc->GetSort();
	m_pInfo->usLook		= pNpc->GetLookFace();
	m_pInfo->usCellX	= pNpc->GetPosX();
	m_pInfo->usCellY	= pNpc->GetPosY();
	m_pInfo->dwMaxLife	= pNpc->GetMaxLife();
	m_pInfo->dwLife		= pNpc->GetLife();

	if(pNpc->IsSynFlag())
	{
		m_StrPacker.AddString(pNpc->GetName());
	}

	m_unMsgType	=_MSG_NPCINFOEX;
	m_unMsgSize	=sizeof(MSG_Info)-1+m_StrPacker.GetSize();

	return true;
}

BOOL CMsgNpcInfoEx::Create(OBJID id, DWORD dwMaxLife, DWORD dwLife, int nType, int nSort,
							int nLookFace, int nCellX, int nCellY, const char* pszName )
{
	this->Init();

	m_pInfo->id			= id;
	m_pInfo->dwMaxLife	= dwMaxLife;
	m_pInfo->dwLife		= dwLife;
	m_pInfo->usType		= (unsigned short)nType;
	m_pInfo->usSort		= (unsigned short)nSort;
	m_pInfo->usLook		= (unsigned short)nLookFace;
	m_pInfo->usCellX	= (unsigned short)nCellX;
	m_pInfo->usCellY	= (unsigned short)nCellY;

	if(pszName)
	{
		m_StrPacker.AddString(pszName);
	}

	m_unMsgType	=_MSG_NPCINFOEX;
	m_unMsgSize	=sizeof(MSG_Info)-1+m_StrPacker.GetSize();

	return true;
}

void CMsgNpcInfoEx::Process(void* pInfo)
{
#ifdef _MYDEBUG
	::LogSave("Process CMsgNpcInfoEx: ID:0x:%x, Type:%d, LookType:%d, CellX:%d, CellY:%d, Name:%s",
				m_pInfo->id	, m_pInfo->ucType,
				m_pInfo->usLook, m_pInfo->usCellX, 
				m_pInfo->usCellY, m_pInfo->szName);

#endif
}
