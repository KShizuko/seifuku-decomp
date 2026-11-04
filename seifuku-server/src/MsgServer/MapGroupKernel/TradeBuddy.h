#if !defined(SFK_TRADEBUDDY_H__INCLUDED_)
#define SFK_TRADEBUDDY_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "GameObj.h"
#include "Myheap.h"

class IRecordset;
class IDatabase;
class CTradeBuddy : public CGameObj
{
protected:
	CTradeBuddy();
public:
	virtual ~CTradeBuddy();

public:
	static CTradeBuddy*	CreateNew()			{ return new CTradeBuddy; }
	bool	Create			(IRecordset* pRes);
	bool	LoadInfo		(IRecordset* pRes);
	bool	DeleteRecord	(IDatabase* pDb);

public:
	OBJID	GetID			();
	OBJID	GetBuddyID		();
	LPCTSTR	GetName			();
	DWORD	GetDate			();
	void	ClearDate		();

protected:
	OBJID	m_id;
	OBJID	m_idUser;
	OBJID	m_idBuddy;
	char	m_szName[_MAX_NAMESIZE];
	DWORD	m_dwDate;

protected:
public:	MYHEAP_DECLARATION(s_heap)
};

#endif
