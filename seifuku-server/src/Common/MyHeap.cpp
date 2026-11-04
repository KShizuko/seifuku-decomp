#include "windows.h"
#include "MyHeap.h"
#include "common.h"
#include "logfile.h"

CMyHeap::CMyHeap(LPCTSTR pszClassName) : m_strClassName(pszClassName)
{
	m_lNumAlloc	= 0;
	m_hHeap	=::HeapCreate(0, 0, 0);
}

CMyHeap::~CMyHeap()
{
	if (m_hHeap)
	{
		::HeapDestroy(m_hHeap);
	}
	if(m_lNumAlloc != 0)
	{
		if(!m_strClassName.empty())
			LOGWARNING("CMyHeap::~CMyHeap() [%s] has [%d] memory blocks not freed", m_strClassName.c_str(), m_lNumAlloc);
		else
			LOGWARNING("CMyHeap::~CMyHeap() has [%d] memory blocks not freed", m_lNumAlloc);
	}
}

void * CMyHeap::Alloc(size_t size)
{
	CHECKF(m_hHeap);
	if(size == 0)
		return false;

	void* p	=::HeapAlloc(m_hHeap, HEAP_ZERO_MEMORY, size);
	if (p)
	{
		InterlockedIncrement(&m_lNumAlloc);
	}
	
	return p;
}

void CMyHeap::Free(void* p)
{
	if(!p)
		return ;

	CHECK(m_hHeap);
	IF_NOT(m_lNumAlloc > 0)
		LOGWARNING("CMyHeap::Free() [%s] bad memory block count %d!", m_strClassName.c_str(), m_lNumAlloc);

	if (::HeapFree(m_hHeap, 0, p))
	{
		InterlockedDecrement(&m_lNumAlloc);
	}
}

BOOL CMyHeap::IsValidPt(void* p)
{
	if (!m_hHeap || !p)
		return false;
	
	if ((DWORD)p < 0x00010000 || (DWORD)p >= 0x7FFEFFFF)
		return false;
	
	if (::IsBadCodePtr((FARPROC)p))
		return false;
	
	return ::HeapValidate(m_hHeap, 0, p);
}
