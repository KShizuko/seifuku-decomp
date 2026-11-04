#include <math.h>
#include "AllMsg.h"
#include "User.h"
#include "MapGroup.h"
char	szStatisticTable[]	= "cq_statistic";

bool CUser::LoadStatistic()
{
	m_pStatisticSet	= CSingleObjSet2<CUserStatistic>::CreateNew(true);
	if (!m_pStatisticSet)
		return false;
	char szSQL[256] = "";	sprintf(szSQL, "SELECT * FROM cq_statistic WHERE player_id=%u", GetID());
	if (!m_pStatisticSet->Create(szSQL, Database()))	{ SAFE_RELEASE(m_pStatisticSet);	return false; }
	return true;
}

void CUser::SaveStatistic()
{
	if (!m_pStatisticSet)
		return;
	for (int i = 0; i < m_pStatisticSet->GetAmount(); i++)
	{
		CUserStatistic* pData = m_pStatisticSet->GetObjByIndex(i);
		if (pData)
			pData->Update();
	}
}

int CUser::GetStatistic(int idEvent, int idType)
{
	if (!m_pStatisticSet)
		return 0;
	for (int i = 0; i < m_pStatisticSet->GetAmount(); i++)
	{
		CUserStatistic* pData = m_pStatisticSet->GetObjByIndex(i);
		if (pData && pData->GetInt(USERSTATISTICDATA_EVENT) == idEvent && pData->GetInt(USERSTATISTICDATA_TYPE) == idType)
			return pData->GetInt(USERSTATISTICDATA_DATA);
	}
	return 0;
}

void CUser::SetStatistic(int idEvent, int idType, int nData, bool bStamp, bool bUpdate, bool bSynchro)
{
	if (!m_pStatisticSet)
		return;

	for (int i = 0; i < m_pStatisticSet->GetAmount(); i++)
	{
		CUserStatistic* pData = m_pStatisticSet->GetObjByIndex(i);
		if (pData && pData->GetInt(USERSTATISTICDATA_EVENT) == idEvent && pData->GetInt(USERSTATISTICDATA_TYPE) == idType)
		{
			pData->SetInt(USERSTATISTICDATA_DATA, nData);
			if (bStamp)
				pData->SetInt(USERSTATISTICDATA_TIMESTAMP, time(NULL));
			if (bSynchro)
			{
				CMsgUserAttrib	msg;
				if (msg.Create(GetID(), (idType << 16) + idEvent + 0x100, nData))
					this->SendMsg(&msg);
			}
			if (bUpdate)
				pData->Update();
			return;
		}
	}

	CUserStatistic* pData = CUserStatistic::CreateNew();
	IF_NOT(pData)
		return;

	IF_NOT(pData->Create(GameDataDefault()->GetUserStatistic(), ID_NONE))
	{
		SAFE_RELEASE(pData);
		return;
	}
	pData->SetStr(USERSTATISTICDATA_NAME, GetName(), _MAX_NAMESIZE);
	pData->SetInt(USERSTATISTICDATA_PLAYERID, GetID());
	pData->SetInt(USERSTATISTICDATA_EVENT, idEvent);
	pData->SetInt(USERSTATISTICDATA_TYPE, idType);
	pData->SetInt(USERSTATISTICDATA_DATA, nData);
	pData->SetInt(USERSTATISTICDATA_TIMESTAMP, time(NULL));
	IF_NOT(pData->InsertRecord())
	{
		SAFE_RELEASE(pData);
		return;
	}
	m_pStatisticSet->AddObj(pData);

	if (bSynchro)
	{
		CMsgUserAttrib	msg;
		if (msg.Create(GetID(), (idType << 16) + idEvent + 0x100, nData))
			this->SendMsg(&msg);
	}
}

DWORD CUser::GetStatisticTimestamp(int idEvent, int idType)
{
	if (!m_pStatisticSet)
		return 0;
	for (int i = 0; i < m_pStatisticSet->GetAmount(); i++)
	{
		CUserStatistic* pData = m_pStatisticSet->GetObjByIndex(i);
		if (pData && pData->GetInt(USERSTATISTICDATA_EVENT) == idEvent && pData->GetInt(USERSTATISTICDATA_TYPE) == idType)
			return pData->GetInt(USERSTATISTICDATA_TIMESTAMP);
	}
	return 0;
}

void CUser::SetStatisticTimestamp(int idEvent, int idType, DWORD dwTime)
{
	if (!m_pStatisticSet)
		return;
	for (int i = 0; i < m_pStatisticSet->GetAmount(); i++)
	{
		CUserStatistic* pData = m_pStatisticSet->GetObjByIndex(i);
		if (pData && pData->GetInt(USERSTATISTICDATA_EVENT) == idEvent && pData->GetInt(USERSTATISTICDATA_TYPE) == idType)
		{
			pData->SetInt(USERSTATISTICDATA_TIMESTAMP, dwTime);
			pData->Update();
			return;
		}
	}
}

bool CUser::IsTimeCardOn()
{
	if (QueryStatus(0x10000000))
		return true;
	return GetTimeCardLeft(0, false) > 0;
}

bool CUser::IsTimeCardStatus()
{
	return QueryStatus(0x8000000) != NULL;
}

int CUser::GetTimeCardStatusRemain()
{
	IStatus* pStatus = QueryStatus(0x8000000);
	if (pStatus)
		return pStatus->GetRemain();
	return 0;
}

__int64 CUser::GetDigits(__int64 n, int nFirst, int nLast)
{
	if ((DWORD)nFirst > (DWORD)nLast)
	{
		int nSwap = nFirst;
		nFirst	= nLast;
		nLast	= nSwap;
	}
	__int64 i64Div	= (__int64)pow(10.0, (double)(DWORD)nFirst);
	__int64 i64Mod	= (__int64)pow(10.0, (double)(DWORD)(nLast - nFirst + 1));
	return (n / i64Div) % i64Mod;
}

time_t CUser::GetGodStatusTime()
{
	DWORD dwGod = m_data.m_Info.dwGodStatus;
	if (dwGod == 0)
		return 0;

	time_t	tNow = time(NULL);
	tm		tmTime;
	memcpy(&tmTime, localtime(&tNow), sizeof(tm));
	tmTime.tm_sec	= 0;
	tmTime.tm_min	= 0;
	tmTime.tm_hour	= dwGod % 100;
	tmTime.tm_mday	= (int)GetDigits(dwGod, 2, 3);
	tmTime.tm_mon	= (int)GetDigits(dwGod, 4, 5) - 1;
	tmTime.tm_year	= (int)GetDigits(dwGod, 6, 7) + 100;
	tmTime.tm_wday	= 0;
	tmTime.tm_yday	= 0;
	return mktime(&tmTime);
}

void CUser::SetGodStatusTime(time_t tTime)
{
	tm* pTm = localtime(&tTime);
	DWORD dwGod = (((m_data.m_Info.dwGodStatus / 100000000) * 100 + pTm->tm_year) * 100 + pTm->tm_mon) * 100 + pTm->tm_mday;
	dwGod	= dwGod * 100 + pTm->tm_hour - 99990000;
	m_data.SetGodStatus(dwGod, true);
}

int CUser::GetTimeCardLeft(int nUnit, bool bRound)
{
	time_t tNow = time(NULL);
	time_t tEnd = GetGodStatusTime();
	if (tEnd == 0 || tEnd < tNow)
		return 0;

	int nSecs = tEnd - tNow;
	switch(nUnit)
	{
	case	0:
		return nSecs;
	case	1:
		return bRound ? (nSecs + 30) / 60 : nSecs / 60;
	case	2:
		return bRound ? (nSecs + 1800) / 3600 : nSecs / 3600;
	}
	return bRound ? (nSecs + 43200) / 86400 : nSecs / 86400;
}

void CUser::GetBlessTimeStr(char* pszBuf)
{
	if (!pszBuf)
		return;
	DWORD dwGod = m_data.m_Info.dwGodStatus;
	sprintf(pszBuf, "%04d:%02d-%02d:%02d", (int)GetDigits(dwGod, 6, 7) + 2000, (int)GetDigits(dwGod, 4, 5),
			(int)GetDigits(dwGod, 2, 3), (int)GetDigits(dwGod, 0, 1));
}

void CUser::AddTimeCardHours(int nHours)
{
	time_t	tNow	= time(NULL);
	time_t	tEnd	= GetGodStatusTime();
	int		nAdd	= 0;
	if (tEnd <= 0 || tEnd < tNow)
	{
		tEnd	= (tNow + 3599) / 3600 * 3600;
		nAdd	= tEnd - tNow;
		this->OnTimeCardMapOut();
	}

	int nSecs = nHours * 3600;
	this->SetGodStatusTime(nSecs + tEnd);
	CRole::AttachStatus(this->QueryRole(), 0x10000000, -1, nSecs + nAdd, 0);
}

int CUser::GetMaxEnergy()
{
	int nMax = m_data.m_Info.usMaxEnergy;
	if (this->IsTimeCardOn())
		return nMax * 150 / 100;
	return nMax;
}

void CUser::SetDoubleExp(int nDoubleExp, int nSecs)
{
	if (nDoubleExp < 0)
		nDoubleExp = 0;
	m_nDoubleExp		= nDoubleExp;
	m_tDoubleExp.Startup(nSecs);

	CMsgUserAttrib	msg;
	msg.Create(GetID(), 0x13, nSecs);
	this->SendMsg(&msg);
}

void CUser::CalcLevByExp(__int64 i64Exp, int* pLev, int* pPercent)
{
	int nLev = this->GetLev();
	CLevupexpData* pLevupexp = LevupexpSet()->GetObj(nLev);
	if (!pLevupexp)
	{
		*pLev		= 0;
		*pPercent	= 0;
		return;
	}

	i64Exp += this->GetExp();
	int nAddLev = 0;
	for (int i = nLev; i < g_nMaxLevel; i++)
	{
		CLevupexpData* pData = LevupexpSet()->GetObj(i);
		if (!pData)
			break;
		pLevupexp	= pData;
		if (i64Exp < pData->GetInt64(LEVUPEXPDATA_EXP))
			break;
		i64Exp		-= pData->GetInt64(LEVUPEXPDATA_EXP);
		nAddLev++;
	}
	*pLev = nLev + nAddLev;

	__int64 i64Need = __max(pLevupexp->GetInt64(LEVUPEXPDATA_EXP), (__int64)1);
	*pPercent = (int)(i64Exp * 100.0 / i64Need);
}

__int64 CUser::CalcExpByTime(int nTime)
{
	int nLev = this->GetLev();
	CLevupexpData* pLevupexp = LevupexpSet()->GetObj(nLev);
	if (!pLevupexp)
		return 0;

	__int64 i64Exp	= this->GetExp();
	__int64 i64Need	= __max(pLevupexp->GetInt64(LEVUPEXPDATA_EXP), (__int64)1);
	int		nUpTime	= pLevupexp->GetInt(LEVUPEXPDATA_UP_LEV_TIME);
	int		nTotal	= nTime + (int)(nUpTime * ((double)i64Exp / i64Need));

	__int64 i64Sum = 0;
	for (int i = nLev; i < g_nMaxLevel; i++)
	{
		CLevupexpData* pData = LevupexpSet()->GetObj(i);
		if (!pData)
			break;
		if (nTotal < pData->GetInt(LEVUPEXPDATA_UP_LEV_TIME))
		{
			int nLevTime = __max(pData->GetInt(LEVUPEXPDATA_UP_LEV_TIME), 1);
			i64Sum += (__int64)(pData->GetInt64(LEVUPEXPDATA_EXP) * ((double)(DWORD)nTotal / nLevTime));
			break;
		}
		nTotal -= pData->GetInt(LEVUPEXPDATA_UP_LEV_TIME);
		i64Sum += pData->GetInt64(LEVUPEXPDATA_EXP);
	}

	if (i64Sum < i64Exp)
		i64Sum = i64Exp;
	return i64Sum - i64Exp;
}
