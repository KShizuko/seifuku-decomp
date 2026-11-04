#if !defined(AFX_MYHEAP_H__CFFE0003_9790_4E6B_B0C8_16CF55F89B19__INCLUDED_)
#define AFX_MYHEAP_H__CFFE0003_9790_4E6B_B0C8_16CF55F89B19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>
#include <string>

const int	CLASSNAME_SIZE = 32;

class CMyHeap
{
public:
	CMyHeap(LPCTSTR pszClassName = NULL);
	~CMyHeap();
public:
	void*	Alloc(size_t size);
	void	Free(void* ptr);
	BOOL	IsValidPt(void* p);
protected:
	long			m_lNumAlloc;
	std::string		m_strClassName;
	HANDLE			m_hHeap;
};

#ifdef	_DEBUG

#define	MYHEAP_DECLARATION(VAR)
#define	MYHEAP_IMPLEMENTATION(T,VAR)

#else

#define	MYHEAP_DECLARATION(VAR)					\
	void   operator delete	(void* p);			\
	void*  operator new	(size_t size);			\
public:											\
	static BOOL		IsValidPt(void* p);			\
protected:										\
	static CMyHeap	VAR;

#define	MYHEAP_IMPLEMENTATION(T,VAR)			\
CMyHeap	T::VAR(#T);								\
void * T::operator new(size_t size)				\
	{ return VAR.Alloc(size); }					\
void T::operator delete(void* p)				\
	{ VAR.Free(p); }							\
BOOL T::IsValidPt(void* p)						\
	{ return VAR.IsValidPt(p); }

#endif

#endif
