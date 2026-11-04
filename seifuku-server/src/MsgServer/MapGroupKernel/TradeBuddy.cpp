#include "windows.h"
#include "define.h"
#include "TradeBuddy.h"
#include "I_mydb.h"
#include "MapGroup.h"

MYHEAP_IMPLEMENTATION(CTradeBuddy,s_heap)

CTradeBuddy::CTradeBuddy()
{
	memset(&m_id, 0, sizeof(OBJID)*3 + _MAX_NAMESIZE + sizeof(DWORD));
	SetObjType(OBJ_TUTOR);
}

CTradeBuddy::~CTradeBuddy()
{
}

bool CTradeBuddy::Create(IRecordset* pRes)
{
	CHECKF(pRes);

	return LoadInfo(pRes);
}

bool CTradeBuddy::LoadInfo(IRecordset* pRes)
{
	if(m_id != ID_NONE)
		return true;

	CHECKF(pRes);

	m_id		= pRes->LoadDWord("id");
	m_idUser	= pRes->LoadDWord("userid");
	m_idBuddy	= pRes->LoadDWord("business");
	pRes->LoadString(m_szName, "name", _MAX_NAMESIZE);
	m_dwDate	= pRes->LoadDWord("date");
	return true;
}

OBJID CTradeBuddy::GetID()
{
	return m_id;
}

OBJID CTradeBuddy::GetBuddyID()
{
	return m_idBuddy;
}

LPCTSTR CTradeBuddy::GetName()
{
	return m_szName;
}

DWORD CTradeBuddy::GetDate()
{
	return m_dwDate;
}

void CTradeBuddy::ClearDate()
{
	m_dwDate	= 0;
}

bool CTradeBuddy::DeleteRecord(IDatabase* pDb)
{
	char	szSQL[1024];
	_snprintf(szSQL, sizeof(szSQL), "delete from cq_business where id = %u", m_id);
	szSQL[sizeof(szSQL)-1]	= 0;
	return pDb->ExecuteSQL(szSQL) != 0;
}
