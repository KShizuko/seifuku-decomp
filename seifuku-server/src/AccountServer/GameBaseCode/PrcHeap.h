#if !defined(AFX_OBJHEAP_H__5F62F501_11AD_4962_B7A1_BED361A28533__INCLUDED_)
#define AFX_OBJHEAP_H__5F62F501_11AD_4962_B7A1_BED361A28533__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class CPrcHeap  
{
public:
	CPrcHeap();
	virtual ~CPrcHeap();

	void	Init	(void);

	void*	New		(size_t size);
	void	Delete	(void* pObj);
	BOOL	IsSafe	(void* pObj);

private:
	HANDLE		m_hHeap;

};

BOOL IsValidPtr(void* p);

extern CPrcHeap g_heapSystem;

#endif
