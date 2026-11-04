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

#ifdef	MULTITHREAD_SAFE
	CCriticalSection	CMyField::m_xCtrl;
#endif

MYHEAP_IMPLEMENTATION(CMyField,s_heap)

CMyField::CMyField()
{
	m_ulType = FIELD_TYPE_NULL;
	m_szVal = NULL;
}

CMyField::~CMyField()
{
}
