#ifndef	MYRECORD_H
#define	MYRECORD_H

class CCriticalSection;
class CMyDatabase;

class CMyRecord : public IRecord
{
protected:
	virtual IRecord*	CreateNewRecord(DWORD idNewKey = NEWKEY_NONE);
	virtual	void	Release() { delete this; }

	virtual DWORD	InsertRecord();
	virtual bool	UpdateRecord(bool bSync = true);
	virtual bool	DeleteRecord();

	virtual int		LoadInt		(LPCTSTR szField);
	virtual __int64	LoadInt64	(LPCTSTR szField);
	virtual void	SetInt		(LPCTSTR szField, int nData);
	virtual void	SetInt64	(LPCTSTR szField, __int64 nData);
	virtual UINT	LoadUInt	(LPCTSTR szField)					{ return (UINT)LoadInt(szField); }
	virtual void	SetUInt		(LPCTSTR szField, UINT nData)		{ SetInt(szField, nData); }
	virtual void	LoadString	(char* szData, LPCTSTR szField, int nSize);
	virtual void	SetString	(LPCTSTR szField, LPCTSTR szData, int nSize);

	virtual int		GetInt		(int idx);
	virtual __int64	GetInt64	(int idx);
	virtual LPCTSTR	GetStr		(int idx);
	virtual void	SetInt		(int idx, int nData);
	virtual void	SetInt64	(int idx, __int64 nData);
	virtual void	SetStr		(int idx, LPCTSTR szData, int nSize);
	virtual void	ClearUpdateFlags	()							{ ClsEditFlag(); }

	virtual bool	AddInt		(LPCTSTR szField, int nData)		{ return AddField(szField, nData); }

public:
	CMyRecord(CMyDatabase  * pMyDatabase);
	virtual ~CMyRecord();

public:
#ifdef	MULTITHREAD_SAFE
	static CCriticalSection	m_xCtrl;
#endif
#ifdef	USE_NEW
	static CMyHeap	s_heapString;
#endif
	static CMyRecord* CreateNew(CMyDatabase  *pMyDatabase);

public:
	MYHEAP_DECLARATION(s_heap)

public:
	enum { modeEdit, modeAddNew, modeDelete };

	virtual bool Open( CMyRecordset* pRes, DWORD idNewKey = 0);
	virtual bool Spawn(CMyRecord  *pMyRecord, DWORD idNewKey = 0);
	virtual void Close();

	void ClsEditFlag();

	bool AddField(LPCTSTR szField, int nData);

	void SetDatabase(CMyDatabase* pDb) { m_pMyDatabase = pDb; }

	CMyDatabase	*	m_pMyDatabase;

protected:
	int				UpdateFieldsCount();

	void			BuildUpdateSQL(char* szSQL, int nBufSize);
	void			BuildUpdateCondition(char* szCondition, int nBufSize);
	void			BuildUpdateOpration(char* szOpration, int nBufSize);

	bool			Query(char * szSQL, MYSQL_RES * * res);

	void			GetFieldValue(UINT uiIndex);
	bool			FormatField(char* szFormat, const CMyField& objField);

protected:
	MYSQL_FIELD*	m_Fields;

	unsigned int 	m_uiFieldsCount;

	char	  		m_szKey[16];
	unsigned int 	m_uiKeyIndex;
	char		 	m_szKeyData[32];

	bool	*		m_pbDirty;

	bool 			m_bEdit;
	bool 			m_bIsOpen;

	int				m_nEditMode;

	CMyField	*	m_objFields;

	char 			m_szTableName[32];

	bool			m_bCanUpdate;
};

#endif