#include "DisdainDataSet.h"
#include "I_mydb.h"
#include "basefunc.h"

char	szDisdainTable[]	= _TBL_DISDAIN;

CDisdainDataSet::CDisdainDataSet()
{
}

CDisdainDataSet::~CDisdainDataSet()
{
	for(DISDAIN_MAP::iterator iter = m_map.begin(); iter != m_map.end(); iter++)
	{
		if(iter->second)
			iter->second->Release();
	}
}

bool CDisdainDataSet::Init(IDatabase* pDb)
{
	CHECKF(pDb);

	char szSQL[1024];
	sprintf(szSQL, "SELECT * FROM %s", _TBL_DISDAIN);
	IRecordset* pRes = pDb->CreateNewRecordset(szSQL, true);
	if(!pRes)
		return true;

	for(int i = 0; i < (int)pRes->RecordCount(); i++)
	{
		__int64	i64Key	= (__int64)pRes->GetInt(DISDAINDATA_DELTA_LEV) * 0x100000000 + (DWORD)pRes->GetInt(DISDAINDATA_TYPE);
		CDisdainData* pData = CDisdainData::CreateNew();
		if(!pData || !pData->Create(pRes))
			return false;

		m_map[i64Key]	= pData;
		pRes->MoveNext();
	}
	pRes->Release();
	return true;
}

CDisdainData* CDisdainDataSet::Query(int nType, int nDeltaLev)
{
	__int64	i64Key	= (__int64)nDeltaLev * 0x100000000 + (DWORD)nType;
	return m_map[i64Key];
}
