#ifndef	MYRECORDSET_H
#define	MYRECORDSET_H

class CCriticalSection;
class CMyDatabase;
class IRecordset;
class IRecord;
class CMyRecordset : public IRecordset
{
	friend class CMyRecord;
	friend class CMyConstRecord;
	friend class CMyField;
public:
	CMyRecordset(CMyDatabase  * pMyDatabase);
	virtual ~CMyRecordset();

public:
	virtual UINT		RecordCount();
	virtual bool		MoveNext();

protected:
	virtual IRecord*	CreateNewRecord(DWORD idNewKey = NEWKEY_NONE);
	virtual IRecord*	CreateNewConstRecord(DWORD idNewKey = NEWKEY_NONE);
	virtual	void	Release() { delete this; }

	virtual DWORD	InsertRecord();
	virtual bool	UpdateRecord(bool bSync = true);
	virtual bool	DeleteRecord();

	virtual int		LoadInt		(LPCTSTR szField);
	virtual void	SetInt		(LPCTSTR szField, int nData);
	virtual UINT	LoadUInt	(LPCTSTR szField)					{ return (UINT)LoadInt(szField); }
	virtual void	SetUInt		(LPCTSTR szField, UINT nData)		{ SetInt(szField, nData); }
	virtual void	LoadString	(char* szData, LPCTSTR szField, int nSize);
	virtual void	SetString	(LPCTSTR szField, LPCTSTR szData, int nSize);

	virtual int		GetInt		(int idx);
	virtual LPCTSTR	GetStr		(int idx);
	virtual void	SetInt		(int idx, int nData);
	virtual void	SetStr		(int idx, LPCTSTR szData, int nSize);
	virtual void	ClearUpdateFlags	()							{ ClsEditFlag(); }

	virtual bool	AddInt		(LPCTSTR szField, int nData)		{ return AddField(szField, nData); }

	virtual __int64	LoadInt64	(LPCTSTR szField);
	virtual void	SetInt64	(LPCTSTR szField, __int64 nData);
	virtual __int64	GetInt64	(int idx);
	virtual void	SetInt64	(int idx, __int64 nData);

public:
#ifdef	MULTITHREAD_SAFE
	static CCriticalSection	m_xCtrl;
#endif
#ifdef	USE_NEW
	static CMyHeap	s_heapString;
#endif
	static CMyRecordset* CreateNew(CMyDatabase  *pMyDatabase);

public:
	MYHEAP_DECLARATION(s_heap)
	
public:
	enum { modeEdit, modeAddNew, modeDelete };

	virtual bool Open( const char * lpszSQL);
	virtual void Close();

public:
	int FieldsCount();

	void MoveFirst();

	bool IsOpen();

	void ClsEditFlag();

	bool AddField(LPCTSTR szField, int nData);
	
protected:
	int UpdateFieldsCount();

	void			BuildUpdateSQL();
	void			BuildUpdateCondition();
	void			BuildUpdateOpration();

	bool			Query(char * szSQL, MYSQL_RES * * res);

	void			GetFieldValue(UINT uiIndex);
	void			GetAllField();
public:
	bool			GetDefauldField();

protected:
	CMyDatabase	*	m_pMyDatabase;

	MYSQL_RES	*	m_Recordset;
	MYSQL_FIELD *	m_Fields;
	MYSQL_ROW		m_Row;

	_int64			m_ulRowsCount;
	_int64			m_ulCursor;
	unsigned int 	m_uiFieldsCount;

	char	  		m_szKey[16];
	unsigned int 	m_uiKeyIndex;

	bool	*		m_pbDirty;
	
	bool 			m_bIsOpen;
	bool 			m_bBOF;
	bool 			m_bEOF;
	bool 			m_bEdit;

	int				m_nEditMode;

	CMyField	*	m_objFields;

	char  			m_szSQL[1024];
	char 			m_szTableName[32];
	char 			m_szUpdateSQL[1024];
	char			m_szUpdateOpration[1024];
	char			m_szUpdateCondition[128];

	bool			m_bCanUpdate;

	bool			m_bDefaultValue;
};

#endif