#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "mydb.h"
#include "basefunc.h"
#include "myrecordset.h"
#include "myrecord.h"
extern	long	s_nDatabaseTimeSum;
#define	LOCKADD_TIMESUM		::InterlockedExchangeAdd(&s_nDatabaseTimeSum, clock() - tStart)
CMyHeap	CMyDatabase::s_heap("CMyDatabase");
CMyDatabase		g_db;
#ifdef	NO_MUTEX
	#define		RELEASE_MUTEX(x)
#else
	#define		RELEASE_MUTEX(x)		::ReleaseMutex(x)
#endif
IRecordset* CMyDatabase::CreateNewRecordset(LPCTSTR szSQL, bool bNoEmpty )
{
	CMyRecordset* pRes = (CMyRecordset::CreateNew(this));
	if(pRes)
	{
		if(pRes->Open(szSQL))
		{
			if(pRes->RecordCount())
			{
				return (IRecordset*)pRes;
			}
			else if(!bNoEmpty && pRes->GetDefauldField())
			{
				return (IRecordset*)pRes;
			}
		}
		delete pRes;
	}
	return NULL;
}
CMyDatabase::CMyDatabase(int eSupportMode)
{
	m_eSupportMode		= eSupportMode;
	m_pThreadAsyncSql	= NULL;
	m_hdbcAsync			= NULL;
	m_hDbMutex			= NULL;
	m_hdbc = NULL;
	m_bIsOpen = FALSE;
	m_nRefCount = 0;
	m_szDBName[0]	= 0;
}
CMyDatabase::~CMyDatabase()
{
	Destroy();
}
bool CMyDatabase::CheckSQL(const char* pszSQL)
{
	if (!pszSQL)
		return false;
	if (0 == strncmp(pszSQL, "UPDATE", 6))
	{
		if (!strstr(pszSQL, "WHERE") || !strstr(pszSQL, "LIMIT"))
		{
			::LogSave("Invalid update SQL [%s], since not found WHERE or LIMIT.", pszSQL);
			return false;
		}
	}
	if (0 == strncmp(pszSQL, "DELETE", 6))
	{
		if (!strstr(pszSQL, "WHERE"))
		{
			::LogSave("Invalid SQL delete [%s].", pszSQL);
			return false;
		}
	}
	return true;
}
void SfkSetClientOptions(MYSQL* hdbc)
{
	if(!hdbc)
		return;
	mysql_options(hdbc, MYSQL_SET_CHARSET_NAME, "latin1");
	mysql_options(hdbc, MYSQL_INIT_COMMAND, "SET SESSION sql_mode=''");
	my_bool bReconnect = 1;
	mysql_options(hdbc, MYSQL_OPT_RECONNECT, &bReconnect);
}
bool CMyDatabase::Open(char * szHost, char * szUser, char * szPasswd,char * szDB,
			unsigned int uiPort, char * szSocket, unsigned int uiFlag )
{
	IF_NOT(szHost && szUser && szPasswd && szDB)
		return false;

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
	SfkSetClientOptions(m_hdbc);
	if (m_hdbc == NULL)
	{	g_nMyDBError = 1001;
		return FALSE;
	}

	if ( mysql_real_connect(m_hdbc, szHost, szUser, szPasswd,
			szDB, uiPort, szSocket , uiFlag) == NULL )
	{	g_nMyDBError = 1002;
		return FALSE;
	}

	if (SUPPORT_ASYN_DBACCESS == m_eSupportMode)
	{
		m_hdbcAsync = mysql_init(NULL);
		SfkSetClientOptions(m_hdbcAsync);
		if (m_hdbcAsync == NULL)
		{	g_nMyDBError = 1001;
			return FALSE;
		}

		if ( mysql_real_connect(m_hdbcAsync, szHost, szUser, szPasswd,
				szDB, uiPort, szSocket , uiFlag) == NULL )
		{	g_nMyDBError = 1002;
			return FALSE;
		}

		m_pThreadAsyncSql = CMyThread::CreateNew(this, 0, (DWORD)-1);
		if (!m_pThreadAsyncSql)
		{
			::MessageBox(NULL, "Cannot create the async SQL thread.", "Error", MB_OK|MB_ICONERROR);
			return false;
		}
	}

	strcpy(m_szDBName, szDB);
	m_bIsOpen = TRUE;
	return TRUE;
}

void CMyDatabase::Close()
{
	if ( m_hdbc != NULL )
	{
		mysql_close( m_hdbc );
		m_hdbc = NULL;
	}

	while (m_setAsyncSql.size() > 0)
		::Sleep(0);

	if ( m_hdbcAsync != NULL )
	{
		mysql_close( m_hdbcAsync );
		m_hdbcAsync = NULL;
	}
	m_bIsOpen = false;

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

		DWORD	tStart = ::TimeGet();
#ifdef	NO_MUTEX
		if(true)
#else
		if (WAIT_OBJECT_0 ==::WaitForSingleObject(m_hDbMutex, _MAX_DBACCESSTIME))
#endif
		{
			char	szCostChk[1024]	= "";
			_snprintf(szCostChk, sizeof(szCostChk), "CMyDatabase::ExecuteSQL(%s)", szSQL);
			szCostChk[sizeof(szCostChk)-1]	= 0;
			CTimeCostChk chk(szCostChk, 1000);

			if (mysql_query(m_hdbc, szSQL) != NULL)
			{
				RELEASE_MUTEX(m_hDbMutex);
				g_nMyDBError = 1004;

				::MsgLogSave("gmlog/DbError", "%s", szSQL);
				return false;
			}
			{
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

bool CMyDatabase::ExecuteAsyncSQL(char* pszSQL)
{
	IF_NOT(SUPPORT_ASYN_DBACCESS == m_eSupportMode)
		return false;

	IF_NOT(this->IsOpen())
		return false;

	IF_NOT(pszSQL && strlen(pszSQL) > 0)
		return false;

	char	szCmd[256]	= "";
	strncpy(szCmd, pszSQL, 6);
	_strlwr(szCmd);
	if (0 == strcmp(szCmd, "select"))
		return false;

	CMySingleLock xLock(&m_xAsyncCtrl, true);
	m_setAsyncSql.push_back(std::string(pszSQL));

	IF_OK(m_pThreadAsyncSql)
		m_pThreadAsyncSql->SetWorkEvent();

	return true;
}

int	CMyDatabase::CountRecord (const char* szSQL)
{
	if (!szSQL)
		return -1;

	if (NULL == strstr(szSQL, "SELECT"))
	{
		::LogSave("Error: invalid count record Sql: %s", szSQL);
		return -1;
	}

	if (m_hdbc == NULL)
	{
		g_nMyDBError = 1003;
		return -1;
	}

#ifdef	NO_MUTEX
	if(true)
#else
	if (WAIT_OBJECT_0 ==::WaitForSingleObject(m_hDbMutex, _MAX_DBACCESSTIME))
#endif
	{
		char	szCostChk[1024]	= "";
		_snprintf(szCostChk, sizeof(szCostChk), "CMyDatabase::CountRecord(%s)", szSQL);
		szCostChk[sizeof(szCostChk)-1]	= 0;
		CTimeCostChk chk(szCostChk, 1000);

		if (mysql_query(m_hdbc, szSQL) != NULL)
		{
			g_nMyDBError = 1004;

			::MsgLogSave("gmlog/DbError", "%s", szSQL);
			return -1;
		}

		MYSQL_RES* res = mysql_store_result(m_hdbc);
		RELEASE_MUTEX(m_hDbMutex);

		if (res == NULL)
			return -1;
		else
		{
			int rval = (int)mysql_num_rows(res);
			mysql_free_result(res);
			return rval;
		}
	}
	else
	{
		::LogSave("*DATABASE* WARNING: ExecuteSQL() overtime");
		return -1;
	}
}

bool CMyDatabase::GetFieldData(const char* szSQL, char* szBuf, int nBufSize)
{
	if (!szSQL || !szBuf)
		return false;

	if (NULL == strstr(szSQL, "select") && NULL == strstr(szSQL, "SELECT"))
	{
		::LogSave("Error: invalid field query sql: %s", szSQL);
		return false;
	}

	if (m_hdbc == NULL)
	{
		g_nMyDBError = 1003;
		return false;
	}

	char	szCostChk[1024]	= "";
	_snprintf(szCostChk, sizeof(szCostChk), "CMyDatabase::GetFieldData(%s)", szSQL);
	szCostChk[sizeof(szCostChk)-1]	= 0;
	CTimeCostChk chk(szCostChk, 1000);

	if (mysql_query(m_hdbc, szSQL) != NULL)
	{
		g_nMyDBError = 1004;

		::MsgLogSave("gmlog/DbError", "%s", szSQL);
		return false;
	}

	MYSQL_RES* res = mysql_store_result(m_hdbc);
	if (res && mysql_num_rows(res) == 1 && mysql_num_fields(res) == 1)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		if (row && strlen(row[0]) < (UINT)nBufSize)
		{
			strcpy(szBuf, row[0]);
			return true;
		}
	}

	return false;
}

int CMyDatabase::GetInsertId()
{
	if (m_hdbc)
		return (int)mysql_insert_id(m_hdbc);
	else
		return 0;
}

DWORD g_dwServerID	=1;
DWORD g_dwMaxPlayer =800;

static bool ReadDatString(FILE* fp, long nOffset, char* pszBuf)
{
	unsigned char	key[16]	= { 0xA4, 0x49, 0x12, 0x11, 0x99, 0x53, 0xF0, 0x5F, 0x32, 0x77, 0xE4, 0x82, 0x2E, 0x6B, 0x10, 0x34 };
	if (!pszBuf || !fp)
		return true;

	fseek(fp, nOffset, SEEK_SET);
	int	nLen	= _getw(fp);
	int	i		= 0;
	for (; i < nLen; i++)
		pszBuf[i]	= (char)(fgetc(fp) ^ key[i % 16]);
	pszBuf[i]	= 0;
	return true;
}

static bool ReadDatFile(const char* pszFile, DWORD* pMaxPlayer, DWORD* pServerID, char* szDBServer, char* szDBUser, char* szPassword, char* szDBName)
{
	FILE* fp	= fopen(pszFile, "rb");
	if (!fp)
		return false;

	*pMaxPlayer	= _getw(fp);
	*pServerID	= _getw(fp);
	bool	bRet	= true;
	bRet	|= ReadDatString(fp, 0x1B, szDBServer);
	bRet	|= ReadDatString(fp, 0x171, szDBUser);
	bRet	|= ReadDatString(fp, 0x309, szPassword);
	bRet	|= ReadDatString(fp, 0x462, szDBName);
	fclose(fp);
	return bRet;
}

BOOL CMyDatabase::Init(const char* pszDBServer, const char* pszDBUser, const char* pszPassword, const char* pszDBName, int nPort)
{
	char szDBServer[256] = "", szDBName[256] = "", szDBUser[256] = "", szPassword[256] = "";

	if (pszDBServer)
		strcpy(szDBServer, pszDBServer);
	else
		strcpy(szDBServer,	"127.0.0.1");

	if (pszDBName)
		strcpy(szDBName, pszDBName);
	else
		strcpy(szDBName,	"cq");

	if (pszDBUser)
		strcpy(szDBUser, pszDBUser);
	else
		strcpy(szDBUser,	"test");

	if (pszPassword)
		strcpy(szPassword, pszPassword);
	else
		strcpy(szPassword,	"test");

	if (0 == nPort)
		nPort	= MYSQL_PORT;

	{
		FILE* fp	=fopen("gameserver.cfg", "r");
		if (fp)
		{
			UINT	nCfgPort	= 0;
			fscanf(fp, "%u %u %s %s %s %s %u\n", &g_dwMaxPlayer, &g_dwServerID, szDBServer, szDBUser, szPassword, szDBName, &nCfgPort);
			fclose(fp);
			nPort	= nCfgPort ? nCfgPort : MYSQL_PORT;
		}
		else if (ReadDatFile("gameserver.dat", &g_dwMaxPlayer, &g_dwServerID, szDBServer, szDBUser, szPassword, szDBName))
		{
			nPort	= MYSQL_PORT;
		}

	}
	if(Open(szDBServer, szDBUser, szPassword, szDBName, nPort, NULL, 0))
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
		char	szMsg[1024]	= "";
		sprintf(szMsg, "database open failed. db server:%s, user:%s, psw:%s, db_name:%s, db port:%d. ", szDBServer, szDBUser, szPassword, szDBName, nPort);
		::MessageBox(NULL, szMsg, "Error",MB_OK|MB_ICONERROR);
		return false;
	}
}

void CMyDatabase::Destroy	(void)
{
	SAFE_DELETE(m_pThreadAsyncSql);

	if (m_hDbMutex)
	{
		::CloseHandle(m_hDbMutex);
		m_hDbMutex	=NULL;
	}

	Close();
}

int CMyDatabase::OnThreadWorkEvent()
{
	if (!m_hdbcAsync)
	{
		::LogSave("Async DB error, not init");
		return 0;
	}

	while (true)
	{
		std::string	strSQL;
		{
			CMySingleLock xLock(&m_xAsyncCtrl, true);
			if (m_setAsyncSql.empty())
				break;
			strSQL	= m_setAsyncSql.front();
			m_setAsyncSql.pop_front();
		}

		if (strSQL.empty() || !CMyDatabase::CheckSQL(strSQL.c_str()))
			continue;

		char	szCostChk[1024]	= "";
		_snprintf(szCostChk, sizeof(szCostChk), "CMyDatabase::OnThreadWorkEvent(%s)", strSQL.c_str());
		szCostChk[sizeof(szCostChk)-1]	= 0;
		CTimeCostChk chk(szCostChk, 1000);

		if (mysql_query(m_hdbcAsync, strSQL.c_str()) != 0)
			::LogSave("Async DB error:%s when execute:%s.", mysql_error(m_hdbcAsync), strSQL.c_str());
	}

	return 1;
}

IDatabase* CreateDatabase(const char* szDBServer , const char* szLoginName , const char* szPassword , const char* szDBName , int nPort , bool bAsync )
{
	CMyDatabase* pDb = new CMyDatabase(bAsync ? SUPPORT_ASYN_DBACCESS : SUPPORT_SYN_DBACCESS);
	if(!pDb)
		return NULL;;

	if(pDb->Init(szDBServer, szLoginName, szPassword, szDBName, nPort))
		return pDb->GetInterface();

	delete pDb;
	return NULL;
}
