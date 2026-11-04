#if !defined(AFX_ALPHAMASK_H__4E68F95F_86D7_465A_84E8_8704C8ADBA42__INCLUDED_)
#define AFX_ALPHAMASK_H__4E68F95F_86D7_465A_84E8_8704C8ADBA42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"
#include "MyHeap.h"

class CAlphaMask
{
public:
	CAlphaMask();
	virtual ~CAlphaMask();
	static void* operator new(size_t uSize) throw()	{ return s_heap.Alloc(uSize); }
	static void	 operator delete(void* p)	{ s_heap.Free(p); }

private:
	BYTE*	m_pBuffer;
	CMySize	m_sizeInfo;
public:
	static CMyHeap	s_heap;
private:

public:
	BOOL IsTransparent(CMyPos posPoint);
	BOOL Load(CMySize sizeBmp, char* pszFileName);

	friend struct AlphaMaskProbe;
};

#endif
