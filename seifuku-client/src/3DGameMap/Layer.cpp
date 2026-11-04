#pragma warning(disable:4786)
#include "Layer.h"

HANDLE	CLayer::s_hHeap	=NULL;
UINT	CLayer::s_uNumAllocsInHeap	=0;

void * CLayer::operator new(size_t size)
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

void CLayer::operator delete(void* p)
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

BOOL CLayer::IsValidPt(void* p)
{
	if (!s_hHeap || !p)
		return false;

	if ((DWORD)p < 0x00010000 || (DWORD)p >= 0x7FFEFFFF)
		return false;

	if (::IsBadCodePtr((FARPROC)p))
		return false;

	return ::HeapValidate(s_hHeap, 0, p);
}

CLayer::CLayer()
{
	usTerrain = 0;
	sAltitude = 0;
	usMask = 0;
}

CLayer::~CLayer()
{}
