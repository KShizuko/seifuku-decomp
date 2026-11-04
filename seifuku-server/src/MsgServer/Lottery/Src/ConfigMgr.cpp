#include "ConfigMgr.h"
#include "common.h"

static CConfigMgr*	s_pConfigMgr	= NULL;
CConfigMgr* CConfigMgr::GetInstance()
{
	if (!s_pConfigMgr)
	{
		static CConfigMgr	s_objConfigMgr;
		s_pConfigMgr = &s_objConfigMgr;
	}
	return s_pConfigMgr;
}

CConfigMgr::~CConfigMgr()
{
	for (LOTTERYCONFIG_SET::iterator it = m_setConfig.begin(); it != m_setConfig.end(); it++)
	{
		CLotteryConfigSet* pConfigSet = it->second;
		SAFE_DELETE(pConfigSet);
	}
	m_setConfig.clear();
}

bool CConfigMgr::Create(IDatabase* pDb)
{
	if (!s_pConfigMgr)
		s_pConfigMgr = this;

	IF_NOT(pDb)
		return false;

	IRecordset* pRes = pDb->CreateNewRecordset("SELECT * FROM cq_config where type between 10000 and 11008", true);
	IF_NOT(pRes)
		return false;

	for (int i = 0; i < (int)pRes->RecordCount(); i++)
	{
		ST_LOTTERYCONFIG	info;
		info.id			= pRes->GetInt(0);
		info.dwType		= pRes->GetInt(1);
		info.nData1		= pRes->GetInt(2);
		info.nData2		= pRes->GetInt(3);
		info.nData3		= pRes->GetInt(4);
		info.nData4		= pRes->GetInt(5);
		info.nData5		= pRes->GetInt(6);
		LPCTSTR pszDescribe = pRes->GetStr(7);
		if (pszDescribe)
			info.strDescribe = pszDescribe;

		LOTTERYCONFIG_SET::iterator it = m_setConfig.find(info.nData1);
		if (it != m_setConfig.end())
		{
			CLotteryConfigSet* pLotteryConfig = it->second;
			IF_NOT(pLotteryConfig)
				return false;
			pLotteryConfig->AddConfig(&info);
		}
		else
		{
			CLotteryConfigSet* pLotteryConfig = new CLotteryConfigSet;
			pLotteryConfig->AddConfig(&info);
			m_setConfig[info.nData1] = pLotteryConfig;
		}

		pRes->MoveNext();
	}
	pRes->Release();
	return true;
}

CLotteryConfigSet* CConfigMgr::GetConfigSet(int nLotteryType)
{
	LOTTERYCONFIG_SET::iterator it = m_setConfig.find(nLotteryType);
	if (it != m_setConfig.end())
	{
		CLotteryConfigSet* pConfigSet = it->second;
		IF_NOT(pConfigSet)
			return NULL;
		return pConfigSet;
	}
	return NULL;
}

bool CLotteryConfigSet::AddConfig(ST_LOTTERYCONFIG* pInfo)
{
	m_setConfig[pInfo->dwType] = *pInfo;
	return true;
}

ST_LOTTERYCONFIG* CLotteryConfigSet::GetConfig(DWORD dwType)
{
	CONFIG_SET::iterator it = m_setConfig.find(dwType);
	if (it != m_setConfig.end())
		return &it->second;
	return NULL;
}

bool CLotteryConfigSet::GetGuarantee(int nLotteryType, DWORD* pInfo)
{
	if (m_setConfig.size() == 0)
		return false;

	DWORD	dwFirst		= 0;
	DWORD	dwSecond	= 0;
	int		nSum		= 0;
	for (CONFIG_SET::iterator it = m_setConfig.begin(); it != m_setConfig.end(); it++)
	{
		ST_LOTTERYCONFIG* pInfoConfig = &it->second;
		if (pInfoConfig->nData1 != nLotteryType)
			continue;
		DWORD dwType = pInfoConfig->dwType;
		if (dwType >= 10000 && dwType <= 10999)
		{
			if (dwFirst == 0)
				dwFirst = dwType;
			else
				dwSecond = dwType;
		}
		else
			nSum += pInfoConfig->nData2;
	}

	if (dwFirst == 0 || dwSecond == 0 || nSum == 0)
		return false;
	if (dwFirst > dwSecond)
	{
		DWORD dwTemp = dwFirst;
		dwFirst = dwSecond;
		dwSecond = dwTemp;
	}
	pInfo[0] = dwFirst;
	pInfo[1] = dwSecond;
	pInfo[2] = nSum;
	return true;
}

DWORD CLotteryConfigSet::RollType(int nLotteryType, int nRoll)
{
	if (m_setConfig.size() == 0)
		return 0;

	int nSum = 0;
	for (CONFIG_SET::iterator it = m_setConfig.begin(); it != m_setConfig.end(); it++)
	{
		ST_LOTTERYCONFIG* pInfoConfig = &it->second;
		if (pInfoConfig->nData1 != nLotteryType)
			continue;
		nSum += pInfoConfig->nData2;
		if (nRoll < nSum)
			return pInfoConfig->dwType;
	}
	return 0;
}
