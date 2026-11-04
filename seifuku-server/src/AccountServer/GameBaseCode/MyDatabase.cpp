#include <windows.h>
#include <stdio.h>
#include <time.h>

#include "mydb.h"
#include "basefunc.h"
#include "myrecordset.h"
#include "myrecord.h"

#ifdef	MULTITHREAD_SAFE
#define	LOCKADD_TIMESUM
#else
extern	long	s_nDatabaseTimeSum;
#define	LOCKADD_TIMESUM		::InterlockedExchangeAdd(&s_nDatabaseTimeSum, clock() - tStart)
#endif

CMyDatabase		g_db;

#ifdef	NO_MUTEX
	#define		RELEASE_MUTEX(x)
#else
	#define		RELEASE_MUTEX(x)		::ReleaseMutex(x)
#endif

IRecordset* CMyDatabase::CreateNewRecordset(LPCTSTR szSQL) 
{ 
	CMyRecordset* pRes = (CMyRecordset::CreateNew(this)); 
	if(pRes)
	{
		if(pRes->Open(szSQL))
			return (IRecordset*)pRes;

		delete pRes;
	}

	return NULL;
}

CMyDatabase::CMyDatabase()
{	
	m_hdbc = NULL;
	m_bIsOpen = FALSE;
	m_nRefCount = 0;
	m_szDBName[0]	= 0;
}

CMyDatabase::~CMyDatabase()
{	
	
}

bool CMyDatabase::CheckSQL(const char* pszSQL)
{
	if (!pszSQL)
		return false;

	if (strstr(pszSQL, "UPDATE"))
	{
		if (!strstr(pszSQL, "WHERE") || !strstr(pszSQL, "LIMIT"))
		{
			::LogSave("Invalid update SQL [%s].", pszSQL);
			return false;
		}
	}

	if (strstr(pszSQL, "DELETE"))
	{
		if (!strstr(pszSQL, "WHERE"))
		{
			::LogSave("Invalid SQL delete [%s].", pszSQL);
			return false;
		}
	}

	return true;
}

bool CMyDatabase::Open(char * szHost, char * szUser, char * szPasswd,char * szDB, 
			unsigned int uiPort, char * szSocket, unsigned int uiFlag )
{	
	if (m_bIsOpen)
	{	if (m_nRefCount == 0 )
			Close();
		else 
		{
			g_nMyDBError = 1000;
			return FALSE;
		}
	}

	m_hdbc = mysql_init(NULL);
	if (m_hdbc == NULL) 
	{	g_nMyDBError = 1001;
		return FALSE;
	}

	if (szDB != NULL)
		strcpy(m_szDBName ,szDB);

	mysql_options(m_hdbc, MYSQL_SET_CHARSET_NAME, "latin1");
	mysql_options(m_hdbc, MYSQL_INIT_COMMAND, "SET SESSION sql_mode=''");
	my_bool bReconnect = 1;
	mysql_options(m_hdbc, MYSQL_OPT_RECONNECT, &bReconnect);

	if ( mysql_real_connect(m_hdbc, szHost, szUser, szPasswd,
			szDB, uiPort, szSocket , uiFlag) == NULL )
	{	g_nMyDBError = 1002;
		return FALSE;
	}
	m_bIsOpen = TRUE;
	return TRUE;
}

void CMyDatabase::Close()
{	
	if ( m_hdbc != NULL )	
		mysql_close( m_hdbc );
	m_bIsOpen = false;
	m_hdbc = NULL;

	if(m_nRefCount)
		::OutputDebugString("*DATABASE* warning: Found unclosed CRecordset.");
}

bool CMyDatabase::ExecuteSQL(char * szSQL)
{	
	if (!CMyDatabase::CheckSQL(szSQL))
		return false;

	try {
		if (m_hdbc == NULL) 
		{	
			g_nMyDBError = 1003;
			return false;
		}

		time_t	tStart = clock();
#ifdef	NO_MUTEX
		if(true)
#else
		if (WAIT_OBJECT_0 ==::WaitForSingleObject(m_hDbMutex, _MAX_DBACCESSTIME))
#endif
		{
			if (mysql_query(m_hdbc, szSQL) != NULL) 
			{	
				LOCKADD_TIMESUM;
				RELEASE_MUTEX(m_hDbMutex);
				g_nMyDBError = 1004;				
				return false;
			}
			{
				LOCKADD_TIMESUM;
				RELEASE_MUTEX(m_hDbMutex);
				return true;
			}
		}
		else
		{
			::LogSave("*DATABASE* WARNING: ExecuteSQL() overtime");
			return false;
		}
	}
	catch (...)
	{
		::LogSave("MyDataBase Exe Sql error: %s", szSQL);
		return false;
	}
}

int CMyDatabase::GetInsertId()
{	
	if (m_hdbc)
		return (int)mysql_insert_id(m_hdbc);
	else
		return 0;
}

void CMyDatabase::GetDBErrorMsg(char * szErrorMsg)
{	
	strcpy(szErrorMsg, mysql_error(m_hdbc));
}

DWORD g_dwServerID	=1;
DWORD g_dwMaxPlayer =800;

BOOL CMyDatabase::Init(const char* pszDBServer, const char* pszDBUser, const char* pszPassword, const char* pszDBName)
{
	char szDBServer[256], szDBName[256], szDBUser[256], szPassword[256];

#ifdef _COLOR_ENABLE
	if (pszDBServer)
		strcpy(szDBServer, pszDBServer);
	else
		strcpy(szDBServer,	"192.168.1.200");

	if (pszDBName)
		strcpy(szDBName, pszDBName);
	else
		strcpy(szDBName,	"hl2");

#else
	if (pszDBServer)
		strcpy(szDBServer, pszDBServer);
	else
		strcpy(szDBServer,	"61.131.96.97");

	if (pszDBName)
		strcpy(szDBName, pszDBName);
	else
		strcpy(szDBName,	"games");

#endif

	if (pszDBUser)
		strcpy(szDBUser, pszDBUser);
	else
		strcpy(szDBUser,	"blacknull");

	if (pszPassword)
		strcpy(szPassword, pszPassword);
	else
		strcpy(szPassword,	"monstermaster");

	{
		FILE* fp	=fopen("gameserver.cfg", "r");
		if (fp)
		{
			fscanf(fp, "%u %u %s %s %s %s\n", &g_dwMaxPlayer, &g_dwServerID, szDBServer, szDBUser, szPassword, szDBName);
			fclose(fp);
		}
	}

	if(Open(szDBServer, szDBUser, szPassword, szDBName))
	{
		m_hDbMutex	=::CreateMutex(NULL, false, NULL);
		if (m_hDbMutex)
			return true;
		else
		{
			::MessageBox(NULL, "Cannot create m_hDbMutex.", "Error",MB_OK|MB_ICONERROR);
			return false;
		}
	}
	else
	{
		::MessageBox(NULL, "Cannot open the database object.", "Error",MB_OK|MB_ICONERROR);
		return false;
	}
}

void CMyDatabase::Destroy	(void)
{
	if (m_hDbMutex)
	{
		::CloseHandle(m_hDbMutex);
		m_hDbMutex	=NULL;
	}
	
	Close();
}

IDatabase* CreateDatabase(const char* szDBServer , const char* szLoginName , const char* szPassword , const char* szDBName )
{
	CMyDatabase* pDb = new CMyDatabase;
	if(!pDb)
		return NULL;;

	if(pDb->Init(szDBServer, szLoginName, szPassword, szDBName))
		return pDb;

	delete pDb;
	return NULL;
}
