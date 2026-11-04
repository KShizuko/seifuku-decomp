#ifdef	MULTITHREAD_SAFE
	#include <afxmt.h>
	#define	LOCKOBJ		CSingleLock xLock(&m_xCtrl, true)
	#include <winsock2.h>
#else
	#define	LOCKOBJ		
#endif

extern	long	s_nDatabaseTimeSum;
#define	LOCKADD_TIMESUM		::InterlockedExchangeAdd(&s_nDatabaseTimeSum, clock() - tStart)
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "mydb.h"
#include "basefunc.h"
#include "MyConstRecord.h"
#include "common.h"
#ifdef	NO_MUTEX
	#define		RELEASE_MUTEX(x)
#else
	#define		RELEASE_MUTEX(x)		::ReleaseMutex(x)
#endif
CMyHeap	CMyConstRecord::s_heapString("CMyConstRecord::s_heapString");
#ifdef	MULTITHREAD_SAFE
	CCriticalSection	CMyConstRecord::m_xCtrl;
#endif
MYHEAP_IMPLEMENTATION(CMyConstRecord,s_heap)
CMyConstRecord::CMyConstRecord(CMyDatabase  * pMyDatabase )
{	
}
CMyConstRecord* CMyConstRecord::CreateNew(CMyDatabase  * pMyDatabase)
{
	return (new CMyConstRecord(pMyDatabase));
}
bool CMyConstRecord::Open(CMyRecordset  *pMyRecordset, DWORD idNewKey )
{
	int	nFieldsCount	= pMyRecordset->FieldsCount();
	for(int i = 0; i < nFieldsCount; i++)
	{
	}
	for(int i = 0; i < nFieldsCount; i++)
	{
		pMyRecordset->GetFieldValue(i);
		VarType	data;
		if (pMyRecordset->m_objFields[i].m_ulType == FIELD_TYPE_VAR_STRING 
				|| pMyRecordset->m_objFields[i].m_ulType == FIELD_TYPE_STRING)
			data = VarType(pMyRecordset->m_objFields[i].m_szVal);
		else
			data = VarType(pMyRecordset->m_objFields[i].m_iVal);
		m_setField.Push(data);
	}
	return true;
}
CMyConstRecord::~CMyConstRecord()
{
}
IRecord* CMyConstRecord::CreateNewRecord(DWORD idNewKey )
{
	return new CMyConstRecord(NULL);
}
DWORD CMyConstRecord::InsertRecord()
{
	MYASSERT(!"const");
	return	false;
}

bool CMyConstRecord::UpdateRecord(bool bSync)
{
	MYASSERT(!"const");
	return	false;
}

bool CMyConstRecord::DeleteRecord()
{
	MYASSERT(!"const");
	return	false;
}

int CMyConstRecord::LoadInt		(LPCTSTR szField)
{
	MYASSERT(!"no field");
	return 0;
}
__int64 CMyConstRecord::LoadInt64	(LPCTSTR szField)
{
	MYASSERT(!"no field");
	return 0;
}

void CMyConstRecord::SetInt		(LPCTSTR szField, int nData)
{
	MYASSERT(!"const");
}

void CMyConstRecord::SetInt64	(LPCTSTR szField, __int64 nData)
{
	MYASSERT(!"const");
}

void CMyConstRecord::SetInt64	(int idx, __int64 nData)
{
	MYASSERT(!"const");
}

void CMyConstRecord::LoadString	(char* szData, LPCTSTR szField, int nSize)	
{
	MYASSERT(!"no field");
}

void CMyConstRecord::SetString	(LPCTSTR szField, LPCTSTR szData, int nSize)
{
	MYASSERT(!"const");
}

int CMyConstRecord::GetInt		(int idx)							
{
	IF_NOT(idx>=0 && idx<m_setField.Size())
	{
		LOGERROR("invalid index[%d] in table [?s].", idx);
		return INT_NONE;
	}

	return static_cast<int>(m_setField[idx]);
}

__int64 CMyConstRecord::GetInt64	(int idx)
{
	IF_NOT(idx>=0 && idx<m_setField.Size())
	{
		LOGERROR("invalid index[%d] in table [?s].", idx);
		return INT_NONE;
	}

	return static_cast<int>(m_setField[idx]);
}

LPCTSTR CMyConstRecord::GetStr		(int idx)							
{
	IF_NOT(idx>=0 && idx<m_setField.Size())
		return STR_NONE;

	return static_cast<const String&>(m_setField[idx]);
}

void CMyConstRecord::SetInt		(int idx, int nData)				
{
	MYASSERT(!"const");
}

void CMyConstRecord::SetStr		(int idx, LPCTSTR szData, int nSize)
{
	MYASSERT(!"const");
}
