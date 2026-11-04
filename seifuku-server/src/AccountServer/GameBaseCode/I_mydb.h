#ifndef	I_MYDB_H
#define	I_MYDB_H

#define PURE_VIRTUAL_DECLARATION_DB	= 0;
#define PURE_VIRTUAL_DECLARATION_DB_0	= 0;

class	CMyRecordset;
class	CMyRecord;
class	IRecordset;
class	IDatabase
{
public:
	virtual ~IDatabase() {}
	virtual IRecordset*	CreateNewRecordset(LPCTSTR szSQL)		PURE_VIRTUAL_DECLARATION_DB_0
	virtual	void	Release()									PURE_VIRTUAL_DECLARATION_DB
public:
	virtual bool	ExecuteSQL(char* szSQL)				PURE_VIRTUAL_DECLARATION_DB_0
	virtual int		GetInsertId()						PURE_VIRTUAL_DECLARATION_DB_0
};

class	IRecord
{
public:
	virtual ~IRecord() {}
	virtual IRecord*	CreateNewRecord(DWORD idNewKey = 0)		PURE_VIRTUAL_DECLARATION_DB_0
	virtual	void	Release()									PURE_VIRTUAL_DECLARATION_DB

	virtual bool	InsertRecord()								PURE_VIRTUAL_DECLARATION_DB
	virtual bool	UpdateRecord()								PURE_VIRTUAL_DECLARATION_DB
	virtual bool	DeleteRecord()								PURE_VIRTUAL_DECLARATION_DB

	virtual int		LoadInt		(LPCTSTR szField)					PURE_VIRTUAL_DECLARATION_DB_0
	virtual void	SetInt		(LPCTSTR szField, int nData)		PURE_VIRTUAL_DECLARATION_DB
	virtual UINT	LoadUInt	(LPCTSTR szField)					PURE_VIRTUAL_DECLARATION_DB_0
	virtual void	SetUInt		(LPCTSTR szField, UINT nData)		PURE_VIRTUAL_DECLARATION_DB
	virtual void	LoadString	(char* szData, LPCTSTR szField, int nSize)		PURE_VIRTUAL_DECLARATION_DB
	virtual void	SetString	(LPCTSTR szField, LPCTSTR szData, int nSize)	PURE_VIRTUAL_DECLARATION_DB
	virtual DWORD	LoadDWord	(LPCTSTR szField)					{ return LoadUInt(szField); }
	virtual void	SetDWord	(LPCTSTR szField, DWORD nData)		{ SetUInt(szField, nData); }

	virtual bool	AddInt(LPCTSTR szField, int nData)			PURE_VIRTUAL_DECLARATION_DB
public:
};

class	IRecordset : public IRecord
{
public:
	virtual ~IRecordset() {}
	virtual UINT		RecordCount()			PURE_VIRTUAL_DECLARATION_DB_0
	virtual bool		MoveNext()				PURE_VIRTUAL_DECLARATION_DB
};

extern IDatabase*	CreateDatabase(const char* szDBServer = NULL, const char* szLoginName = NULL, const char* szPassword = NULL, const char* szDBName = NULL);

#endif