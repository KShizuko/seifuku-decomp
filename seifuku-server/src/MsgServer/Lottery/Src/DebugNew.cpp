#ifdef	_DEBUG
#include "common.h"
#undef	new

void* _cdecl operator new(size_t nSize, LPCSTR lpszFileName, int nLine)		{ return ::operator new(nSize); }
void _cdecl operator delete(void* p, LPCSTR lpszFileName, int nLine)		{ ::operator delete(p); }
void* _cdecl operator new[](size_t nSize, LPCSTR lpszFileName, int nLine)	{ return ::operator new[](nSize); }
void _cdecl operator delete[](void* p, LPCSTR lpszFileName, int nLine)		{ ::operator delete[](p); }
#endif
