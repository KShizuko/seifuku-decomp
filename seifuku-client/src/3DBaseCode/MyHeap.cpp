#include "MyHeap.h"

CMyHeap::CMyHeap(const char* pszName)
{
	m_hHeap = HeapCreate(0, 0, 0);
	m_uNumAllocsInHeap = 0;
	m_uMaxAllocsInHeap = 0;
	if(pszName)
		m_strName = pszName;
}

CMyHeap::~CMyHeap()
{
	if(m_hHeap)
	{
		HeapDestroy(m_hHeap);
		DebugMsg("heap of class %s destroied, with max alloc(%ld).", m_strName.c_str(), m_uMaxAllocsInHeap);
	}
}
void* CMyHeap::Alloc(SIZE_T uSize)
{
	IF_NOT(m_hHeap)
		return NULL;

	if(uSize == 0)
		return NULL;

	void* p = HeapAlloc(m_hHeap, HEAP_ZERO_MEMORY, uSize);

	if(p)
	{
		InterlockedIncrement(&m_uNumAllocsInHeap);
		if(m_uNumAllocsInHeap > m_uMaxAllocsInHeap)
			InterlockedExchange(&m_uMaxAllocsInHeap, m_uNumAllocsInHeap);
	}
	return p;
}
void CMyHeap::Free(void* p)
{
	IF_NOT(p)
		return;

	IF_NOT(m_hHeap)
		return;

	if(HeapFree(m_hHeap, 0, p))
		InterlockedDecrement(&m_uNumAllocsInHeap);
}
