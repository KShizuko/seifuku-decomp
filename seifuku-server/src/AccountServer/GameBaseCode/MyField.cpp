#ifdef	MULTITHREAD_SAFE
	#include <afxmt.h>
	#define	LOCKOBJ		CSingleLock xLock(&m_xCtrl, true)
	#include <winsock2.h>
#else
	#define	LOCKOBJ		
#endif

#include <windows.h>
#include <stdio.h>
#include <assert.h>

#include "mydb.h"
#include "BaseFunc.h"

HANDLE	CMyField::s_hHeap				=NULL;
UINT	CMyField::s_uNumAllocsInHeap	=0;
#ifdef	MULTITHREAD_SAFE
	CCriticalSection	CMyField::m_xCtrl;
#endif

CMyField::CMyField()
{
	m_ulType = FIELD_TYPE_NULL;
	m_szVal = NULL;
}

void CMyField::Clear()
{

}

CMyField::~CMyField()
{
}

void* CMyField::operator new(size_t size)
{
	LOCKOBJ;

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

void CMyField::operator delete(void* p)
{
	LOCKOBJ;

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

void* CMyField::operator new[](size_t size)
{
	LOCKOBJ;

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

void CMyField::operator delete[](void* p)
{
	LOCKOBJ;

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

BOOL CMyField::IsValidPt(void* p)
{
	LOCKOBJ;

	if (!s_hHeap || !p)
		return false;

	if ((DWORD)p < 0x00010000 || (DWORD)p >= 0x7FFEFFFF)
		return false;

	if (::IsBadCodePtr((FARPROC)p))
		return false;

	return ::HeapValidate(s_hHeap, 0, p);
}

void CMyField::operator = (unsigned char chOp)
{
	if (m_pMyField->m_ulType == FIELD_TYPE_TINY)
		m_pMyField->m_ucVal = chOp;
}

void CMyField::operator = (char chOp)
{
	if (m_pMyField->m_ulType == FIELD_TYPE_TINY)
		m_pMyField->m_cVal = chOp;
}

void CMyField::operator = ( int nOp)
{	
	if (m_pMyField->m_ulType == FIELD_TYPE_LONG)
		m_pMyField->m_iVal = nOp;
}

void CMyField::operator = ( long lOp)
{	
	if (m_pMyField->m_ulType == FIELD_TYPE_LONG)
		m_pMyField->m_lVal = lOp;
}

void CMyField::operator = ( float fltOp )
{
	if (m_pMyField->m_ulType == FIELD_TYPE_FLOAT)
		m_pMyField->m_fVal = fltOp;
}

void CMyField::operator = ( double dbOp )
{
	if (m_pMyField->m_ulType == FIELD_TYPE_DOUBLE) 
		m_pMyField->m_dVal = dbOp;
}

void CMyField::operator = ( unsigned long lOp )
{
	if (m_pMyField->m_ulType == FIELD_TYPE_LONG || 
			m_pMyField->m_ulType == FIELD_TYPE_SHORT )
		m_pMyField->m_lVal = lOp;
}

void CMyField::FieldName(char * szFileName)
{	strcpy(szFileName, m_Field.name);
}

unsigned int CMyField::FieldLen() const
{	return m_Field.length;
}

unsigned int CMyField::FieldAttr() const
{	return m_Field.flags;
}

void CMyField::CopyFrom(const char * szField, int nLen)
{	
	try{
		assert(szField != NULL);
		memset(m_szVal,0,nLen);
		strncpy(m_szVal, szField, nLen-1);
	}catch(...){ LogSave("*DATABASE* ERROR: CMyField::CopyFrom() illegal access."); }
}

void CMyField::CopyTo(char * szField, int nLen)
{	
	try{
		assert(szField != NULL);
		memset(szField,0,nLen);
		strncpy(szField, m_szVal, nLen-1);
	}catch(...){ LogSave("*DATABASE* ERROR: CMyField::CopyTo() illegal access."); }
}

#ifdef	EXTERN_C_DEFINE
	void CMyField::operator = (char * szVal)
	{	
		try{
			if (m_pMyField->m_ulType == FIELD_TYPE_STRING)
				strcpy(m_pMyField->m_szVal ,szVal);
		}catch(...){
			LogSave("*DATABASE* ERROR: CMyField::operator char*() illegal access."); 
		}
	}
#else
	void CMyField::operator = (CString strOp)
	{	
		strcpy(m_pMyField->m_szVal, strOp);
	}
#endif
