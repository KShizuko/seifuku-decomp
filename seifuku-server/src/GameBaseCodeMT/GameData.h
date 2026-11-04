#if !defined(GAMEDATA_HEAD3)
#define GAMEDATA_HEAD3

#if _MSC_VER > 1000
#pragma once
#endif

#include "I_mydb.h"
#include "autoptr.h"

template<TYPENAME T, char* TBL, char* KEY>
class CGameData : IGameData<T>
{
protected:
	CGameData()											{ m_pRecord = NULL; m_idKey = ID_NONE; }
	virtual ~CGameData()								{ if(m_pRecord) m_pRecord->Release(); }
public:
	static CGameData*	CreateNew()						{ return new CGameData; }
	virtual ULONG		Release()						{ delete this; return 0; }
	IGameData<T>*			QueryInterface()				{ return (IGameData<T>*)this; }
public:
	virtual void		ReleaseByOwner()				{ delete this; }
	OBJID		GetID()									{ return m_idKey; }
public:
	OBJID		GetKey()								{ return m_idKey; }
	int			GetInt(T nDataName)						{ return m_pRecord->GetInt(nDataName); }
	LPCTSTR		GetStr(T nDataName)						{ return m_pRecord->GetStr(nDataName); }
	__int64		GetInt64(T nDataName)					{ return m_pRecord->GetInt64(nDataName); }
	void		SetInt(T nDataName, int nData)			{ m_pRecord->SetInt(nDataName, nData); }
	void		SetStr(T nDataName, LPCTSTR szData, int nSize)		{ m_pRecord->SetStr(nDataName, szData, nSize); }
public:
	bool		Create(OBJID id, IDatabase* pDb);
	bool		Create(IRecordset* pRecordset)
														{ m_pRecord = pRecordset->CreateNewRecord(); 
															if(m_pRecord) m_idKey	= m_pRecord->LoadDWord(KEY); 
															return m_pRecord != NULL; }
	bool		Create(IRecord* pDefaultRes, OBJID id)
														{ m_pRecord = pDefaultRes->CreateNewRecord(id); 
															if(m_pRecord) m_idKey	= m_pRecord->LoadDWord(KEY); 
															return m_pRecord != NULL; }
	bool		Create(IDatabase* pDb, OBJID id)
														{ CHECKF(CreateNewDefault(pDb));
															m_pRecord = (*m_pDefault).CreateNewRecord(id); 
															if(m_pRecord) m_idKey	= m_pRecord->LoadDWord(KEY); 
															return m_pRecord != NULL; }
	bool		Update()								{ return m_pRecord->UpdateRecord(); }
	OBJID		InsertRecord()							{ m_idKey = m_pRecord->InsertRecord(); return m_idKey; }
	bool		DeleteRecord()							{ return m_pRecord->DeleteRecord(); }
	void		ClearUpdateFlags()						{ m_pRecord->ClearUpdateFlags(); }
	IRecord*	GetRecord()								{ MYASSERT(m_pRecord); return m_pRecord; }

protected:
	IRecord*	m_pRecord;
	OBJID		m_idKey;

protected:
	static bool	CreateNewDefault(IDatabase* pDb)		{ char szSQL[256]; sprintf(szSQL, "SELECT * FROM %s WHERE %s=0 LIMIT 1", TBL, KEY);
															if(m_pDefault) return true; m_pDefault = pDb->CreateNewRecordset(szSQL, false); return m_pDefault!=NULL; }
protected:
	static CAutoPtr<IRecordset>	m_pDefault;

};

template<TYPENAME T, char* TBL, char* KEY>
bool CGameData<T,TBL,KEY>::Create(OBJID id, IDatabase* pDb)
{
	CHECKF(pDb);

	char	szSQL[1024];
	sprintf(szSQL, "SELECT * FROM %s WHERE %s=%u LIMIT 1", TBL, KEY, id);
	IRecordset* pRes = pDb->CreateNewRecordset(szSQL, false);
	m_pRecord	= pRes->CreateNewRecord();
	pRes->Release();

	if(m_pRecord)
		m_idKey	= m_pRecord->LoadDWord(KEY);

	return m_pRecord != NULL;
}

template<TYPENAME T, char* TBL, char* KEY>
CAutoPtr<IRecordset>	CGameData<T,TBL,KEY>::m_pDefault;

#endif
