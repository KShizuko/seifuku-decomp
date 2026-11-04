#if !defined(AFX_FACTSET_H__C444973E_E282_4312_81CF_A23CFDA0660B__INCLUDED_)
#define AFX_FACTSET_H__C444973E_E282_4312_81CF_A23CFDA0660B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif
#include "mycom.h"
#include "T_SingleMap.h"
#include "GameObjSet.h"
#include "VarType.h"
#include "I_AiCenter.h"
#include "TickOver.h"
#include "autoptr.h"
#include "myheap.h"
#include "basetype.h"
#include "Fact.h"
#include "t_myqueue.h"
enum FACTDATA{
		FACTDATA_ID_,
		FACTDATA_OWNERID,
		FACTDATA_FACT,
		FACTDATA_PRIORITY,
		FACTDATA_DATETIME,
		FACTDATA_DURABLE,
};
class IDatabase;
class CSymbolTable;
class CFactObj
{
	COM_STYLE(CFactObj)
public:
	CFactObj()	{}
	MYHEAP_DECLARATION(s_heap)
public:
	bool		Create(CSymbolTable* pSymbolTable, OBJID id, OBJID idRecord, StringRef strFact, int nPriority, DWORD dwDateTime, int nDurable);
	bool		Create(CSymbolTable* pSymbolTable, OBJID id, StringRef strFact, int nPriority, int nNow, int nDurable, CFact* pModelFact);
	bool		Create(CSymbolTable* pSymbolTable, OBJID id, CFactRef cFact, int nPriority, int nNow, int nDurable);
public:
	OBJID		GetID()				{ return m_id; }
public:
	OBJID		GetRecordID()		{ return m_idRecord; }
	OBJID		GetType()			const	{ return m_cFact.GetType(); }
	VarTypeSetRef	GetParamSet()	const	{ return m_cFact.GetParamSet(); }
	int			GetPriority()		{ return m_nPriority; }
	DWORD		GetDateTime()		{ return m_tDurable.GetBegin(); }
	void		GetFactString(String& strFact, CSymbolTable* pSymbolTable);
	bool		IsLikeModel(CFactRef cModelFact);
	bool		IsDirty()			{ return m_bDirty; }
	bool		IsAlive(int nNow)	{ return m_tDurable.GetDurable(nNow) > 0; }
	bool		IsLogicNot()		{ return m_cFact.IsLogicNot(); }
	CFactRef	QueryFact()			{ return m_cFact; }
	bool		IsValid();
public:
	void		SetRecordID(OBJID id)		{ MYASSERT(m_idRecord == ID_NONE); m_idRecord = id; }

public:
	String		Dump()	const		{
										String	str("CFactObj:");
										str += ::DumpString(m_cFact);
										str	+= " life:";
										str += String().FormatLong(m_tDurable.GetEnd());

										return str;
									}

protected:
	OBJID		m_id;
	OBJID		m_idRecord;
	bool		m_bDirty;

	CFact		m_cFact;
	int			m_nPriority;
	CTickOver	m_tDurable;
};
template<>
struct dump_traits<CFactObj> { static String Dump(const CFactObj& obj)		{ return obj.Dump(); } };
class IDatabase;
class CAiCenter;
class CFactSet  
{
	COM_STYLE(CFactSet)
protected:
	CFactSet();
	 ~CFactSet();
public:
	bool	Create(CAiCenter* pOwner, IDatabase* pDb, int nNow);
	bool	Persistent(IDatabase *pDb, int nNow);
public:
	bool	AddFact(StringRef strFact, int nPriority, int nNow, int nDurable);
protected:
	void	ClearAlikeFact(CFactRef cModelFact);
	bool	AddFact(OBJID idRecord, StringRef strFact, int nPriority, DWORD dwDateTime, int nDurable);
public:
	bool	AddFactQueue(CFactRef cFact, int nPriority, int nNow, int nDurable);
	void	CombineQueue();
public:
	CSymbolTable*	QuerySymbolTable();
public:
	typedef		CSingleMap<CFactObj>	FACT_SET;
	typedef		FACT_SET::Iterator		Iterator;
	Iterator	NewEnum		() 						{ return m_setFact->NewEnum(); }
	int			Size		() const				{ return m_setFact->Size(); }
	void		DelObj		(Iterator& iter)		{ m_setFact->DelObj(iter); }
	void		DelObj		(OBJID idFact)			{ m_setFact->DelObj(idFact); }
protected:
	CAiCenter*		m_pOwner;
	CAutoPtr<FACT_SET>		m_setFact;
protected:
public:
	OBJID		SpawnFactID()						{ ++m_idLast; MYASSERT(m_idLast != ID_NONE); return m_idLast; }

private:
	OBJID	m_idLast;
};

#endif
