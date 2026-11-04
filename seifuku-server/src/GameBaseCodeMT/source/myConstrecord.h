#ifndef	MY_CONST_RECORD_H
#define	MY_CONST_RECORD_H

#include "Array.h"
#include "VarType.h"
class CCriticalSection;

class CMyConstRecord : public IRecord
{
protected:
	virtual IRecord*	CreateNewRecord(DWORD idNewKey = NEWKEY_NONE);
	virtual	void	Release() { delete this; }

	virtual DWORD	InsertRecord();
	virtual bool	UpdateRecord(bool bSync);
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
	virtual void	ClearUpdateFlags	()							{ MYASSERT(!"const"); }

	virtual bool	AddInt		(LPCTSTR szField, int nData)		{ MYASSERT(!"const"); return false; }

public:
	CMyConstRecord(CMyDatabase  * pMyDatabase);
	virtual ~CMyConstRecord();
	bool	Open(CMyRecordset  *pMyRecordset, DWORD idNewKey = NEWKEY_NONE);

public:
#ifdef	MULTITHREAD_SAFE
	static CCriticalSection	m_xCtrl;
#endif
#ifdef	USE_NEW
	static CMyHeap	s_heapString;
#endif
	static CMyConstRecord* CreateNew(CMyDatabase  *pMyDatabase);

public:
	MYHEAP_DECLARATION(s_heap)

protected:
	VarTypeSet	m_setField;
};

#endif