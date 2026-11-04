#if !defined(CONST_GAMEDATA_HEAD3)
#define CONST_GAMEDATA_HEAD3
#if _MSC_VER > 1000
#pragma once
#endif
#include "I_mydb.h"
template<TYPENAME T, char* TBL, T KEY>
class CConstGameData : IGameData<T>
{
protected:
	CConstGameData()											{ m_pRecord = NULL; m_idKey = ID_NONE; }
	virtual ~CConstGameData()								{ if(m_pRecord) m_pRecord->Release(); }
public:
	static CConstGameData*	CreateNew()						{ return new CConstGameData; }
	virtual ULONG		Release()						{ delete this; return 0; }
	IGameData<T>*		QueryInterface()				{ return (IGameData<T>*)this; }
public:
	virtual void		ReleaseByOwner()				{ delete this; }
	OBJID		GetID()									{ return m_idKey; }
public:
	OBJID		GetKey()								{ return m_idKey; }
	int			GetInt(T nDataName)						{ return m_pRecord->GetInt(nDataName); }
	LPCTSTR		GetStr(T nDataName)						{ return m_pRecord->GetStr(nDataName); }
	void		SetInt(T nDataName, int nData)			{ m_pRecord->SetInt(nDataName, nData); }
	void		SetStr(T nDataName, LPCTSTR szData, int nSize)		{ m_pRecord->SetStr(nDataName, szData, nSize); }
public:
	bool		Create(OBJID id, IDatabase* pDb);
	bool		Create(IRecordset* pRecordset)			{ m_pRecord = pRecordset->CreateNewConstRecord(); 
															if(m_pRecord) m_idKey	= m_pRecord->GetInt(KEY); 
															return m_pRecord != NULL; }
	bool		Create(IRecord* pDefaultRes, OBJID id)
														{ m_pRecord = pDefaultRes->CreateNewRecord(id); 
															if(m_pRecord) m_idKey	= m_pRecord->GetInt(KEY); 
															return m_pRecord != NULL; }
	bool		Update()								{ return m_pRecord->UpdateRecord(); }
	OBJID		InsertRecord()							{ m_idKey = m_pRecord->InsertRecord(); return m_idKey; }
	bool		DeleteRecord()							{ return m_pRecord->DeleteRecord(); }
	void		ClearUpdateFlags()						{ m_pRecord->ClearUpdateFlags(); }
	IRecord*	GetRecord()								{ MYASSERT(m_pRecord); return m_pRecord; }
protected:
	IRecord*	m_pRecord;
	OBJID		m_idKey;
};
template<TYPENAME T, char* TBL, T KEY>
bool CConstGameData<T,TBL,KEY>::Create(OBJID id, IDatabase* pDb)
{
	CHECKF(pDb);

	char	szSQL[1024];
	sprintf(szSQL, "SELECT * FROM %s WHERE %s=%u LIMIT 1", TBL, KEY, id);
	IRecordset* pRes = pDb->CreateNewRecordset(szSQL, false);
	m_pRecord	= pRes->CreateNewRecord();
	pRes->Release();

	if(m_pRecord)
		m_idKey	= m_pRecord->GetInt(KEY);

	return m_pRecord != NULL;
}

#endif
