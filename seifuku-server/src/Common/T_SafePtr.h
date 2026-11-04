#ifndef	T_SAFEPTR
#define T_SAFEPTR

#pragma warning(disable:4786)
#include "common.h"

#define LF
#define SHOWINC
#define SHOWDEC

template < class T, unsigned long MYID >
class	TSafePtr
{
public:
	TSafePtr() { m_ptr = NULL; m_nCount++; SHOWINC; }
	TSafePtr(T * ptr) { 
		if(ptr != m_pDefault)	m_ptr = ptr; else m_ptr = NULL;
		m_nCount++; SHOWINC; 
	}
	TSafePtr(T & ref) {
		if(ref.m_ptr != m_pDefault)	m_ptr = ref.m_ptr; else m_ptr = NULL;
		m_nCount++; SHOWINC; 
	}
	~TSafePtr() { 
		m_nCount--; SHOWDEC; 
		if(!m_nCount && m_pDefault){ 
#ifdef	SAFEPTR_COMSTYLE
			m_pDefault->ReleaseByOwner();
#else
			delete m_pDefault; 
#endif
			m_pDefault = NULL; } 
	}
public:
	operator T * () { Valid(); return m_ptr; }
	int operator == (T * ptr) { Valid(); return (m_ptr == ptr); }
	T * operator ->() { if(Valid(false)) return m_ptr; else return Default(); }
private:
	bool	Valid(bool bEnableNull = true);
	T *		Default();
protected:
	T *	m_ptr;
	static T *	m_pDefault;
	static int	m_nCount;
};

template < class T, unsigned long MYID >
T * TSafePtr<T, MYID>::m_pDefault = NULL;

template < class T, unsigned long MYID >
int	TSafePtr<T, MYID>::m_nCount = 0;

template < class T, unsigned long MYID >
bool	TSafePtr<T, MYID>::Valid(bool bEnableNull )
{
	if(bEnableNull && !m_ptr)
		return true;

	if(m_ptr)
	{
		try{
			unsigned long nID = m_ptr->GetObjType();
			if(nID == MYID)
				return true;
			else
			{
				LOGERROR("ERROR: TSafePointer<%d>::operator->() type dismatch to %d." LF, MYID, nID);
				MYASSERT(!"TSafePtr");
			}
		}catch(...){
			m_ptr = NULL;
			LOGERROR("CATCH ERROR: TSafePointer<%d>::operator->() can't access object. follow access." LF, MYID);
			MYASSERT(!"TSafePtr");
		}
	}
	else
	{
		LOGERROR("ERROR: TSafePointer<%d>::operator->() null pointer access.uninitial pointer." LF, MYID);
		MYASSERT(!"TSafePtr");
	}

	return false;
}

template < class T, unsigned long MYID >
T *		TSafePtr<T, MYID>::Default()
{
	try
	{
#ifdef	SAFEPTR_COMSTYLE
			m_pDefault->ReleaseByOwner();
#else
		delete m_pDefault;
#endif
	}catch(...){
		LOGERROR("CATCH ERROR: TSafePointer<%d>::operator->() can't access default object." LF, MYID);
		MYASSERT(!"TSafePtr");
	}

#ifdef	SAFEPTR_COMSTYLE
			m_pDefault = T::CreateNew();
#else
	m_pDefault = new T;
#endif

	return m_pDefault;
}

#endif
