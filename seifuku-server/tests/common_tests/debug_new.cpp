#include <new>

void* __cdecl operator new(size_t nSize, const char* , int )
{
	return ::operator new(nSize);
}

void __cdecl operator delete(void* p, const char* , int )
{
	::operator delete(p);
}