#ifndef __MYDB_H__
#define __MYDB_H__

#include "I_mydb.h"

typedef char * HSTMT; 
typedef bool ( * ENUMPROC)(char * ); 

static	int g_nMyDBError;
static	char m_szMyDBErrorMsg[256];
class CMyDatabase : public IDatabase
{
protected:
	virtual IRecordset*	CreateNewRecordset(LPCTSTR szSQL);
	virtual	void	Release() { delete this; }
public:
	virtual bool	ExecuteSQL(char* szSQL);
	virtual int		GetInsertId();

public:
	CMyDatabase();
	virtual ~CMyDatabase();
	IDatabase*	GetInterface() { return (IDatabase*)this; }

	BOOL Init		(const char* szDBServer = NULL, const char* szLoginName = NULL, const char* szPassword = NULL, const char* szDBName = NULL);
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
	MYSQL*	m_hdbc;			
	int		m_nRefCount;
	HANDLE	m_hDbMutex;

	friend class CMyRecordset;
	friend class CMyRecord;

private:				
	bool 		m_bIsOpen;
	char		m_szDBName[32];
};

#endif
