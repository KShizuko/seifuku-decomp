#include <windows.h>
#include "PrcHeap.h"

CPrcHeap g_heapSystem;

CPrcHeap::CPrcHeap()
{
	Init();
}

CPrcHeap::~CPrcHeap()
{
}

void CPrcHeap::Init(void)
{
	m_hHeap	=::GetProcessHeap();
}

void* CPrcHeap::New(size_t size)
{
	if (!m_hHeap)
		return NULL;

	return ::HeapAlloc(m_hHeap, 0, size);
}

void CPrcHeap::Delete(void* pObj)
{
	if (!m_hHeap || !pObj)
		return;
	
	::HeapFree(m_hHeap, 0, pObj);
}

BOOL CPrcHeap::IsSafe(void* pObj)
{
	if (!m_hHeap || !pObj)
		return false;

	if ((DWORD)pObj < 0x00010000 || (DWORD)pObj >= 0x7FFEFFFF)
		return false;

	if (::IsBadCodePtr((FARPROC)pObj))
		return false;

	return ::HeapValidate(m_hHeap, 0, pObj);
}

BOOL IsValidPtr(void* p)
{
	return g_heapSystem.IsSafe(p);	
}