#ifndef __MYDB_H__
#define __MYDB_H__

#include "I_mydb.h"
#include "MyHeap.h"
#include "MyLock.h"
#include "MyThread.h"
#pragma warning(disable:4786)
#include <list>
#include <string>

typedef char * HSTMT;
typedef bool ( * ENUMPROC)(char * );

static	int g_nMyDBError;
static	char m_szMyDBErrorMsg[256];

enum { SUPPORT_SYN_DBACCESS = 0, SUPPORT_ASYN_DBACCESS = 1 };

class CMyDatabase : public IDatabase, public IThreadEvent
{
public:
	CMyDatabase(int eSupportMode = SUPPORT_SYN_DBACCESS);
	virtual ~CMyDatabase();
public:
	static CMyDatabase*	CreateNew()	{ return new CMyDatabase; }
protected:
	virtual	void	Release() { delete this; }
	virtual IRecordset*	CreateNewRecordset(LPCTSTR szSQL, bool bNoEmpty = true);
public:
	virtual bool	ExecuteSQL(char* szSQL);
	virtual bool	ExecuteAsyncSQL(char* szSQL);
	virtual int		GetInsertId();
	virtual int		CountRecord	(const char* szSQL);
	virtual bool	GetFieldData(const char* szSQL, char* szBuf, int nBufSize);

protected:
	virtual int		OnThreadCreate()		{ return 0; }
	virtual int		OnThreadDestroy()		{ return 0; }
	virtual int		OnThreadWorkEvent();
	virtual int		OnThreadProcess()		{ return 0; }

public:
	IDatabase*	GetInterface() { return (IDatabase*)this; }

	BOOL Init		(const char* szDBServer = NULL, const char* szLoginName = NULL, const char* szPassword = NULL, const char* szDBName = NULL, int nPort = 0);
	void Destroy	(void);

	bool	ExecuteSQL(char * szSQL, MYSQL_RES ** pRes) { return ExecuteSQL(szSQL); }

	static	bool CheckSQL	(const char* pszSQL);

private:
	bool	IsOpen()		{return m_bIsOpen;}

	bool	EnumDatabases	(ENUMPROC lpEnumFunc);
	bool	EnumTables		(ENUMPROC lpEnumFunc);
	void	GetDBErrorMsg	(char * szErrorMsg);

	bool Open(char * szHost, char * szUser, char * szPasswd,char * szDB,
			unsigned int uiPort = MYSQL_PORT, char * szSocket = NULL,
			unsigned int uiFlag = NULL);
	virtual void Close();

protected:
	int		m_eSupportMode;
	MYSQL*	m_hdbc;
	int		m_nRefCount;
	HANDLE	m_hDbMutex;

	friend class CMyRecordset;
	friend class CMyRecord;

private:
	bool 		m_bIsOpen;
	char		m_szDBName[32];

protected:
	CMyThread*			m_pThreadAsyncSql;
	MYSQL*				m_hdbcAsync;
	CMyCriticalSection	m_xAsyncCtrl;
	typedef std::list<std::string>	ASYNC_SQL_SET;
	ASYNC_SQL_SET		m_setAsyncSql;

protected:
	static CMyHeap	s_heap;
};

struct st_mysql;
void	SfkSetClientOptions(struct st_mysql* hdbc);

#endif
