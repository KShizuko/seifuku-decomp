#include "NetMsg.h"

HANDLE	CNetMsg::s_hHeap	=NULL;
UINT	CNetMsg::s_uNumAllocsInHeap	=0;

CNetMsg::CNetMsg()
{
	Init();
}

CNetMsg::~CNetMsg()
{

}

void * CNetMsg::operator new(size_t size)
{
	if (!s_hHeap)
	{
		s_hHeap	=::HeapCreate(0, 0, 0);
		if (!s_hHeap)
			return NULL;
	}

	void* p	=::HeapAlloc(s_hHeap, HEAP_ZERO_MEMORY, size);

	if (p)
	{
		s_uNumAllocsInHeap++;
	}

	return p;
}

void CNetMsg::operator delete(void* p)
{
	if (!s_hHeap || s_uNumAllocsInHeap <= 0)
		return;

	if (::HeapFree(s_hHeap, 0, p))
	{
		s_uNumAllocsInHeap--;
	}

	if (s_uNumAllocsInHeap <= 0)
	{
		if (::HeapDestroy(s_hHeap))
			s_hHeap	=NULL;
	}
}

BOOL CNetMsg::IsValidPt(void* p)
{
	if (!s_hHeap || !p)
		return false;

	if ((DWORD)p < 0x00010000 || (DWORD)p >= 0x7FFEFFFF)
		return false;

	if (::IsBadCodePtr((FARPROC)p))
		return false;

	return ::HeapValidate(s_hHeap, 0, p);
}

void CNetMsg::Init()
{
	memset(m_bufMsg, 0L, sizeof(m_bufMsg));
	m_unMsgSize	=0;
	m_unMsgType	=_MSG_NONE;
}

void CNetMsg::operator = (CNetMsg& msg)
{
	memcpy(m_bufMsg, msg.GetBuf(), _MAX_MSGSIZE);
	m_dwAnyData	=msg.m_dwAnyData;
}

BOOL CNetMsg::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if(!pbufMsg)
		return false;

	if((unsigned short)dwMsgSize != CNetMsg::GetSize(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_NONE == CNetMsg::GetType(pbufMsg, dwMsgSize))
		return false;

	memcpy(this->m_bufMsg, pbufMsg, dwMsgSize);
	return true;
}

void CNetMsg::Process(CGameSocket* pSocket)
{
}

unsigned short	CNetMsg::GetType(char* pbufMsg, DWORD dwMsgSize)
{
	if(!pbufMsg || (int)dwMsgSize > CNetMsg::GetMaxSize())
		return 0;

	if(dwMsgSize <= 2*sizeof(unsigned short))
		return 0;

	unsigned short* punPointer	=(unsigned short*)pbufMsg;
	return punPointer[1];
}

unsigned short	CNetMsg::GetSize(char* pbufMsg, DWORD dwMsgSize)
{
	if(!pbufMsg || (int)dwMsgSize > CNetMsg::GetMaxSize())
		return 0;

	if(dwMsgSize <= 2*sizeof(unsigned short))
		return 0;

	return	*((unsigned short*)pbufMsg);
}