#ifdef	MULTITHREAD_SAFE
	#include <afxmt.h>
	#define	LOCKOBJ		CSingleLock xLock(&m_xCtrl, true)
	#include <winsock2.h>
#else
	#define	LOCKOBJ		
#endif

extern	long	s_nDatabaseTimeSum;
#define	LOCKADD_TIMESUM

#include <windows.h>
#include <stdio.h>
#include <time.h>

#include "mydb.h"
#include "basefunc.h"
#include "MyRecord.h"
#include "common.h"

#ifdef	NO_MUTEX
	#define		RELEASE_MUTEX(x)
#else
	#define		RELEASE_MUTEX(x)		::ReleaseMutex(x)
#endif

CMyHeap	CMyRecord::s_heapString("CMyRecord::s_heapString");
#ifdef	MULTITHREAD_SAFE
	CCriticalSection	CMyRecord::m_xCtrl;
#endif

MYHEAP_IMPLEMENTATION(CMyRecord,s_heap)
bool CMyRecord::FormatField(char* szFormat, const CMyField& objField)
{
	switch (objField.m_ulType)
	{
	case FIELD_TYPE_STRING:
	case FIELD_TYPE_VAR_STRING:
		if (strlen(objField.m_szVal) == NULL)
			return false;
		else
		{
			char	szEscape[4096] = "";
			mysql_escape_string(szEscape, objField.m_szVal, strlen(objField.m_szVal));
			sprintf(szFormat, "'%s'",szEscape);
		}
		break;

	case FIELD_TYPE_TINY:
	case FIELD_TYPE_SHORT:
		sprintf(szFormat, "%d",objField.m_iVal);
		break;

	case FIELD_TYPE_FLOAT:
		sprintf(szFormat, "%.2f",objField.m_fVal);
		break;

	case FIELD_TYPE_DOUBLE:
		sprintf(szFormat, "%.2f",objField.m_dVal);
		break;

	case FIELD_TYPE_LONG:
		if ((objField.m_Field.flags & UNSIGNED_FLAG) != 0)
			sprintf(szFormat, "%u", objField.m_ulVal);
		else
			sprintf(szFormat, "%ld", objField.m_lVal);
		break;

	case FIELD_TYPE_LONGLONG:
		sprintf(szFormat, "%I64d", objField.m_i64Val);
		break;

	default:
		::LogSave("Error: unknow type in CMyRecord::FieldToStr()");
		return false;
	}

	return true;
}

void CMyRecord::BuildUpdateCondition(char* szCondition, int nBufSize)
{
	if(!szCondition)
		return;

	switch ( m_nEditMode )
	{
	case modeEdit:
		sprintf(szCondition, "%s=%s", m_szKey, m_szKeyData);
		break;
	case modeAddNew:
	case modeDelete:
		sprintf(szCondition, "%s=%u", m_szKey, m_objFields[m_uiKeyIndex].m_ulVal);
		break;
	}
}

void CMyRecord::BuildUpdateOpration(char* szOpration, int nBufSize)
{
	if(!szOpration)
		return;

	szOpration[0]='\0';

	char szFormat[1024] = "";

	BOOL bFirst = true;
	
	switch ( m_nEditMode )
	{	
	case modeEdit:
	case modeAddNew:
		for (UINT i=0; i<m_uiFieldsCount; i++ )
		{	
			if ( m_pbDirty[i] )
			{
				if (FormatField(szFormat, m_objFields[i]))
				{	
					if (!bFirst)
						strcat(szOpration,",");
					else
						bFirst = false;

					strcat(szOpration, m_Fields[i].name);
					strcat(szOpration, "=");
					strcat(szOpration, szFormat);
					if((int)strlen(szOpration) >= nBufSize)
						::LogSave("ERROR: BuildUpdateOpration() overflow!!!");
				}

			}
		}
		break;
	}
}

void CMyRecord::BuildUpdateSQL(char* szSQL, int nBufSize)
{
	if (!szSQL) return;
	if (!m_bCanUpdate) return;

	char	szOpration[1024] = "";
	BuildUpdateOpration(szOpration, 1024);
	char	szCondition[1024] = "";
	BuildUpdateCondition(szCondition, 1024);

	char * psz = szSQL;

	switch (m_nEditMode)
	{
	case modeDelete:
		psz = (char * )SQL_STMT_DELETE;
		break;

	case modeEdit:
		psz = (char * )SQL_STMT_UPDATE;
		break;

	case modeAddNew:
		psz = (char * )SQL_STMT_INSERT;
		break;
	}

	sprintf(szSQL,psz,m_szTableName,
		szOpration,szCondition);
	if((int)strlen(szSQL) >= nBufSize)
		::LogSave("ERROR: BuildUpdateSQL() overflow!!!");
}

CMyRecord::CMyRecord(CMyDatabase  * pMyDatabase )
{	
	m_bIsOpen = false;

	m_pMyDatabase = pMyDatabase;
	if (pMyDatabase == NULL ) return;
	
	if (m_pMyDatabase != NULL && m_pMyDatabase->IsOpen())
	{	m_pMyDatabase->m_nRefCount++;
	}
}

CMyRecord* CMyRecord::CreateNew(CMyDatabase  * pMyDatabase)
{
	if (pMyDatabase == NULL ||
			!pMyDatabase->IsOpen())
		return NULL;

	return (new CMyRecord(pMyDatabase));
}

bool CMyRecord::Open(CMyRecordset  *pMyRecordset, DWORD idNewKey )
{
	if (pMyRecordset == NULL ||	!pMyRecordset->IsOpen())
		return NULL;

	if (!m_pMyDatabase->IsOpen())
		return false;
	if (!( pMyRecordset->IsOpen() && (pMyRecordset->RecordCount() > 0 || pMyRecordset->m_bDefaultValue) ))
		return false;

	m_uiFieldsCount	= pMyRecordset->m_uiFieldsCount;
	strcpy(m_szKey, pMyRecordset->m_szKey);
	m_uiKeyIndex	= pMyRecordset->m_uiKeyIndex;
	m_bEdit			= pMyRecordset->m_bEdit;
	m_bIsOpen			= pMyRecordset->m_bIsOpen;
	m_nEditMode		= pMyRecordset->m_nEditMode;
	strcpy(m_szTableName, pMyRecordset->m_szTableName);
	m_bCanUpdate		= pMyRecordset->m_bCanUpdate;

#ifdef	USE_NEW
	m_pbDirty	= (bool*)s_heapString.Alloc(sizeof(bool) * pMyRecordset->m_uiFieldsCount);
#else
	m_pbDirty	= (bool *)g_heapSystem.New(sizeof(bool) * pMyRecordset->m_uiFieldsCount);
#endif
	memset(m_pbDirty, 0, (sizeof(bool) * pMyRecordset->m_uiFieldsCount));

#ifdef	USE_NEW
	m_Fields		= (MYSQL_FIELD*)s_heapString.Alloc(sizeof(MYSQL_FIELD) * m_uiFieldsCount);
#else
	m_Fields		= (MYSQL_FIELD*)g_heapSystem.New(sizeof(MYSQL_FIELD) * m_uiFieldsCount);
#endif
	for(int i = 0; i < (int)m_uiFieldsCount; i++)
	{
		m_Fields[i]	= pMyRecordset->m_Fields[i];
#ifdef	USE_NEW
		m_Fields[i].name = (char*)s_heapString.Alloc(sizeof(char) * (strlen(pMyRecordset->m_Fields[i].name)+1));
#else
		m_Fields[i].name = (char*)g_heapSystem.New(sizeof(char) * (strlen(pMyRecordset->m_Fields[i].name)+1));
#endif
		strcpy(m_Fields[i].name, pMyRecordset->m_Fields[i].name);
	}

	m_objFields		= new CMyField [m_uiFieldsCount];
	for(int i = 0; i < (int)m_uiFieldsCount; i++)
	{
		pMyRecordset->GetFieldValue(i);
		m_objFields[i]		= pMyRecordset->m_objFields[i];
		m_objFields[i].m_Field = m_Fields[i];
		if (m_objFields[i].m_ulType == FIELD_TYPE_VAR_STRING 
				|| m_objFields[i].m_ulType == FIELD_TYPE_STRING)
		{
#ifdef	USE_NEW
			m_objFields[i].m_szVal = (char *) s_heapString.Alloc(sizeof(char)*(m_Fields[i].length+1));
#else
			m_objFields[i].m_szVal = (char *) g_heapSystem.New(sizeof(char)*(m_Fields[i].length+1));
#endif
			strcpy(m_objFields[i].m_szVal, pMyRecordset->m_objFields[i].m_szVal);
		}
	}

	if(idNewKey != NEWKEY_NONE)
	{
		sprintf(m_szKeyData, "%u", idNewKey);
		m_objFields[m_uiKeyIndex].m_ulVal	= idNewKey;
	}
	else
		FormatField(m_szKeyData, pMyRecordset->m_objFields[pMyRecordset->m_uiKeyIndex]);

	return true;
}

bool CMyRecord::Spawn(CMyRecord *pMyRecord, DWORD idNewKey )
{
	*this	= *pMyRecord;

#ifdef	USE_NEW
	m_pbDirty	= (bool *)s_heapString.Alloc(sizeof(bool) * pMyRecord->m_uiFieldsCount);
#else
	m_pbDirty	= (bool *)g_heapSystem.New(sizeof(bool) * pMyRecord->m_uiFieldsCount);
#endif
	memset(m_pbDirty, 0, (sizeof(bool) * pMyRecord->m_uiFieldsCount));

#ifdef	USE_NEW
	m_Fields		= (MYSQL_FIELD*)s_heapString.Alloc(sizeof(MYSQL_FIELD) * m_uiFieldsCount);
#else
	m_Fields		= (MYSQL_FIELD*)g_heapSystem.New(sizeof(MYSQL_FIELD) * m_uiFieldsCount);
#endif
	for(int i = 0; i < (int)m_uiFieldsCount; i++)
	{
		m_Fields[i]	= pMyRecord->m_Fields[i];
#ifdef	USE_NEW
		m_Fields[i].name = (char*)s_heapString.Alloc(sizeof(char) * (strlen(pMyRecord->m_Fields[i].name)+1));
#else
		m_Fields[i].name	= (char*)g_heapSystem.New(sizeof(char) * (strlen(pMyRecord->m_Fields[i].name)+1));
#endif
		strcpy(m_Fields[i].name, pMyRecord->m_Fields[i].name);
	}

	m_objFields		= new CMyField [m_uiFieldsCount];
	for(int i = 0; i < (int)m_uiFieldsCount; i++)
	{
		m_objFields[i]		= pMyRecord->m_objFields[i];
		m_objFields[i].m_Field	= m_Fields[i];
		if (m_objFields[i].m_ulType == FIELD_TYPE_VAR_STRING 
				|| m_objFields[i].m_ulType == FIELD_TYPE_STRING)
		{
#ifdef	USE_NEW
			m_objFields[i].m_szVal = (char *) s_heapString.Alloc(sizeof(char)*(m_Fields[i].length+1));
#else
			m_objFields[i].m_szVal = (char *) g_heapSystem.New(sizeof(char)*(m_Fields[i].length+1));
#endif
			strcpy(m_objFields[i].m_szVal, pMyRecord->m_objFields[i].m_szVal);
		}
	}

	if(idNewKey != NEWKEY_NONE)
	{
		sprintf(m_szKeyData, "%u", idNewKey);
		m_objFields[m_uiKeyIndex].m_ulVal	= idNewKey;
	}

	return true;
}

void CMyRecord::Close()
{	
	try {
		if (m_bIsOpen)
		{	
#ifdef	USE_NEW
			s_heapString.Free(m_pbDirty);
#else
			g_heapSystem.Delete(m_pbDirty);
#endif
			for (unsigned int i=0;i<m_uiFieldsCount;i++)
			{
				if (m_objFields[i].m_ulType == FIELD_TYPE_STRING 
							|| m_objFields[i].m_ulType == FIELD_TYPE_VAR_STRING)
#ifdef	USE_NEW
					s_heapString.Free(m_objFields[i].m_szVal);
#else
					g_heapSystem.Delete(m_objFields[i].m_szVal);
#endif

#ifdef	USE_NEW
				s_heapString.Free(m_Fields[i].name);
#else
				g_heapSystem.Delete(m_Fields[i].name);
#endif
			}
			delete [] m_objFields;
#ifdef	USE_NEW
			s_heapString.Free(m_Fields);
#else
			g_heapSystem.Delete(m_Fields);
#endif
			m_pMyDatabase->m_nRefCount --;
		}
		m_bIsOpen = false;
	}
	catch(...)
	{
		char szMsg[]	="Mysql RecordSet close error.";
		::OutputDebugString(szMsg);
	}
}

CMyRecord::~CMyRecord()
{
	if (m_bIsOpen) Close();
}

bool CMyRecord::Query(char * strSQL, MYSQL_RES * * res)
{
	if (!CMyDatabase::CheckSQL(strSQL))
		return false;

	try {
		if (!m_bIsOpen) return false;

		DWORD	tStart = ::TimeGet();
#ifdef	NO_MUTEX
		if(true)
#else
		if (WAIT_OBJECT_0 ==::WaitForSingleObject(m_pMyDatabase->m_hDbMutex, _MAX_DBACCESSTIME))
#endif
		{
			char	szTemp[1024] = "";
			_snprintf(szTemp, sizeof(szTemp), "CMyRecord::Query(%s)", strSQL);
			szTemp[sizeof(szTemp)-1] = 0;
			CTimeCostChk chk(szTemp, 1000);

			MYSQL * hdbc = m_pMyDatabase->m_hdbc;
			if (mysql_query(hdbc, strSQL)!=NULL)
			{
				LOCKADD_TIMESUM;
				RELEASE_MUTEX(m_pMyDatabase->m_hDbMutex);

				::MsgLogSave("gmlog/DbError", "%s", strSQL);
				return false;
			}
			else
			{
				LOCKADD_TIMESUM;
				*res=mysql_store_result(hdbc);
				if (*res == NULL)
				{
					if (mysql_field_count(hdbc) >0 )
					{
						RELEASE_MUTEX(m_pMyDatabase->m_hDbMutex);
						return false;
					}
					else
					{
						RELEASE_MUTEX(m_pMyDatabase->m_hDbMutex);
						return true;		
					}
				}
				else
				{
					RELEASE_MUTEX(m_pMyDatabase->m_hDbMutex);
					return true;
				}
			}
		}
		else
		{
			::LogSave("*DATABASE* WARNING: Qurey() overtime");
			return false;
		}
	}
	catch(...)
	{
		char szMsg[1024];
		sprintf(szMsg, "Mysql Query error: %s", strSQL);
		::OutputDebugString(szMsg);

		return false;
	}
}

void CMyRecord::ClsEditFlag()
{
	for ( int i=0; i<(int)m_uiFieldsCount; i++ )
		m_pbDirty[i] = false;
}

int CMyRecord::UpdateFieldsCount()
{
	int nCount	=0;
	for ( int i=0; i<(int)m_uiFieldsCount; i++ )
	{
		if (m_pbDirty[i])
			nCount++;
	}

	return nCount;
}

bool CMyRecord::AddField(LPCTSTR szField, int nData)
{
	char	szSQL[1024] = "";
	sprintf(szSQL, "UPDATE %s SET %s=%s+%d WHERE %s=%s LIMIT 1",
				m_szTableName, szField, szField, nData, m_szKey, m_szKeyData);

	MYSQL_RES * res;
	bool ret = Query(szSQL, &res);
	if(ret && res)
		mysql_free_result(res);
	return	ret;
}

IRecord* CMyRecord::CreateNewRecord(DWORD idNewKey )
{
	CMyRecord* pRecord = CMyRecord::CreateNew(m_pMyDatabase);
	if(pRecord)
	{
		if(pRecord->Spawn(this, idNewKey))
			return (IRecord*)pRecord;

		delete pRecord;
	}

	return NULL;
}

DWORD CMyRecord::InsertRecord()
{
	if (!m_bIsOpen || !m_bCanUpdate)
		return false;

	m_bEdit		= false;
	m_nEditMode	= modeAddNew;

	bool ret	=true;
	if (this->UpdateFieldsCount() > 0)
	{
		char	szSQL[2048] = "";
		this->BuildUpdateSQL(szSQL, 2048);

		MYSQL_RES * res;
		ret = Query(szSQL, &res);
		if(ret && res)
			mysql_free_result(res);
	}

	this->ClsEditFlag();

	if(ret)
	{
		DWORD key = m_pMyDatabase->GetInsertId();
		if(key != 0)
		{
			sprintf(m_szKeyData, "%u", key);
			m_objFields[m_uiKeyIndex].m_ulVal	= key;
			return key;
		}
		else
			return m_objFields[m_uiKeyIndex].m_ulVal;
	}
	else
		return 0;
}

bool CMyRecord::UpdateRecord(bool bSync )
{
	if (!m_bIsOpen || !m_bCanUpdate)
		return false;

	m_bEdit		= false;
	m_nEditMode	= modeEdit;

	bool ret	=true;
	if (this->UpdateFieldsCount() > 0)
	{
		char	szSQL[2048] = "";
		this->BuildUpdateSQL(szSQL, 2048);
#ifdef	PALED_DEBUG
		LOGWARNING("--- %s", szSQL);
#endif

		if(bSync)
		{
			MYSQL_RES * res;
			ret = Query(szSQL, &res);
			if(ret && res)
				mysql_free_result(res);
		}
		else
			m_pMyDatabase->ExecuteAsyncSQL(szSQL);
	}

	this->ClsEditFlag();

	return	ret;
}

bool CMyRecord::DeleteRecord()
{
	if (!m_bIsOpen || !m_bCanUpdate)
		return false;

	m_bEdit		= false;
	m_nEditMode	= modeDelete;

	bool ret	=true;

	char	szSQL[2048] = "";
	this->BuildUpdateSQL(szSQL, 2048);

	MYSQL_RES * res;
	ret = Query(szSQL, &res);
	if(ret && res)
		mysql_free_result(res);

	this->ClsEditFlag();

	return	ret;
}

int CMyRecord::LoadInt		(LPCTSTR szField)
{
	IF_NOT(szField)
		return INT_NONE;

	for (UINT i=0; i < m_uiFieldsCount; i++)
	{	
		if (stricmp(szField, m_Fields[i].name) == 0)
		{
			return GetInt(i);
		}
	}

	return INT_NONE;
}

__int64 CMyRecord::LoadInt64	(LPCTSTR szField)
{
	IF_NOT(szField)
		return INT_NONE;

	for (UINT i=0; i < m_uiFieldsCount; i++)
	{
		if (stricmp(szField, m_Fields[i].name) == 0)
		{
			return GetInt64(i);
		}
	}

	return INT_NONE;
}

void CMyRecord::SetInt		(LPCTSTR szField, int nData)
{
	IF_NOT(szField)
		return;

	for (UINT i=0; i < m_uiFieldsCount; i++)
	{	
		if (stricmp(szField, m_Fields[i].name) == 0)
		{
			SetInt(i, nData);
			return;
		}
	}
}

void CMyRecord::SetInt64	(LPCTSTR szField, __int64 nData)
{
	IF_NOT(szField)
		return;

	for (UINT i=0; i < m_uiFieldsCount; i++)
	{
		if (stricmp(szField, m_Fields[i].name) == 0)
		{
			SetInt64(i, nData);
			return;
		}
	}
}

void CMyRecord::LoadString	(char* szData, LPCTSTR szField, int nSize)	
{
	IF_NOT(szField && szData)
		return;

	for (UINT i=0; i < m_uiFieldsCount; i++)
	{	
		if (stricmp(szField, m_Fields[i].name) == 0)
		{
			SafeCopy(szData, GetStr(i), nSize);
			return;
		}
	}
}

void CMyRecord::SetString	(LPCTSTR szField, LPCTSTR szData, int nSize)
{
	IF_NOT(szField && szData)
		return;

	for (UINT i=0; i < m_uiFieldsCount; i++)
	{	
		if (stricmp(szField, m_Fields[i].name) == 0)
		{
			SetStr(i, szData, nSize);
			return;
		}
	}
}

int CMyRecord::GetInt		(int idx)							
{
	IF_NOT(idx>=0 && idx<(int)m_uiFieldsCount)
	{
		LOGERROR("invalid index[%d] in table [%s].", idx, m_szTableName);
		return INT_NONE;
	}

	if (!m_objFields[idx].m_szVal)
		return m_objFields[idx].m_iVal;
	else
		return INT_NONE;
}

__int64 CMyRecord::GetInt64	(int idx)
{
	IF_NOT(idx>=0 && idx<(int)m_uiFieldsCount)
	{
		LOGERROR("invalid index[%d] in table [%s].", idx, m_szTableName);
		return INT_NONE;
	}
	if (!m_objFields[idx].m_szVal)
		return m_objFields[idx].m_i64Val;
	else
		return INT_NONE;
}
LPCTSTR CMyRecord::GetStr		(int idx)							
{
	IF_NOT(idx>=0 && idx<(int)m_uiFieldsCount) 
	{
		LOGERROR("invalid index[%d] in table [%s].", idx, m_szTableName);
		return STR_NONE;
	}

	if (m_objFields[idx].m_szVal)
		return (m_objFields[idx].m_szVal);
	else 
		return STR_NONE;
}

void CMyRecord::SetInt		(int idx, int nData)				
{
	IF_NOT(idx>=0 && idx<(int)m_uiFieldsCount) 
	{
		LOGERROR("Invalid index[%d] in table [%s]", idx, m_szTableName);
		return;
	}

	m_pbDirty[idx] = true;
	if (!m_objFields[idx].m_szVal)
		m_objFields[idx].m_iVal = nData;
	return;
}

void CMyRecord::SetInt64	(int idx, __int64 nData)
{
	IF_NOT(idx>=0 && idx<(int)m_uiFieldsCount)
	{
		LOGERROR("Invalid index[%d] in table [%s]", idx, m_szTableName);
		return;
	}
	m_pbDirty[idx] = true;
	if (!m_objFields[idx].m_szVal)
		m_objFields[idx].m_i64Val = nData;
	return;
}

void CMyRecord::SetStr		(int idx, LPCTSTR szData, int nSize)
{
	IF_NOT(idx>=0 && idx<(int)m_uiFieldsCount) 
	{
		LOGERROR("Invalid index[%d] in table [%s]", idx, m_szTableName);
		return;
	}

	IF_NOT(nSize <= (int)m_Fields[idx].length+1)
	{
		LOGERROR("Invalid size[%d] in field[%s] of table [%s]", nSize, m_Fields[idx].name, m_szTableName);
		return;
	}

	m_pbDirty[idx] = true;
	if (m_objFields[idx].m_szVal)
		SafeCopy(m_objFields[idx].m_szVal, szData, nSize);

	return;
}
