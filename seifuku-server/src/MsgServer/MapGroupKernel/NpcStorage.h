#if !defined(AFX_NPCSTORAGE_H__09B6F357_449A_4DDC_95BD_7A73C05D8104__INCLUDED_)
#define AFX_NPCSTORAGE_H__09B6F357_449A_4DDC_95BD_7A73C05D8104__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Item.h"
#include "Myheap.h"
#include "Package.h"

class CPackage;
class CNpcTrunk  
{
protected:
	CNpcTrunk();
	virtual ~CNpcTrunk();
public:
	static CNpcTrunk* CreateNew()			{ return new CNpcTrunk; }
	ULONG	Release()						{ delete this; return 0; }

public:
	bool	Create(PROCESS_ID, OBJID idRecordNpc, int nSize);
	CPackage*	QueryPackage()									{ CHECKF(m_pPackage); return m_pPackage; }
	bool	IsPackageFull()										{ return QueryPackage()->GetAmount() >= m_nSize; }
	bool	IsEmpty()											{ return QueryPackage()->GetAmount() == 0; }

protected:
	CPackage*	m_pPackage;
	int			m_nSize;

private:
	PROCESS_ID	m_idProcess;

	MYHEAP_DECLARATION(s_heap)
};

#endif
