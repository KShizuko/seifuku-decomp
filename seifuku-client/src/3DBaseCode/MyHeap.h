#if !defined(AFX_MYHEAP_H__SFK_4294_INCLUDED_)
#define AFX_MYHEAP_H__SFK_4294_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"
#include <string>

class CMyHeap
{
	friend struct MyHeapProbe;
public:
	CMyHeap(const char* pszName);
	virtual ~CMyHeap();

	void*	Alloc(SIZE_T uSize);
	void	Free(void* p);
	LONG	GetAllocCount() const	{ return m_uNumAllocsInHeap; }

private:
	HANDLE	m_hHeap;
	LONG	m_uNumAllocsInHeap;
	LONG	m_uMaxAllocsInHeap;
	std::string	m_strName;
};

#endif
