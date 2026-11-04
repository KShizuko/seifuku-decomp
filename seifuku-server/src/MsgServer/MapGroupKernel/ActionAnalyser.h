#if !defined(SFK_ACTIONANALYSER_H__INCLUDED_)
#define SFK_ACTIONANALYSER_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>
#include <stdlib.h>
#include <vector>
#include "BaseFunc.h"

class CActionAnalyser
{
public:
	CActionAnalyser(DWORD dwAmount, int nMinAverage, DWORD dwMinDeviation, DWORD dwMaxInterval)
		: m_dwAmount(dwAmount < 3 ? 3 : dwAmount), m_nMinAverage(nMinAverage), m_dwMinDeviation(dwMinDeviation), m_dwMaxInterval(dwMaxInterval) {}
	virtual ~CActionAnalyser() {}
	static CActionAnalyser* CreateNew(DWORD dwAmount, int nMinAverage, DWORD dwMinDeviation, DWORD dwMaxInterval)
	{ return new CActionAnalyser(dwAmount, nMinAverage, dwMinDeviation, dwMaxInterval); }

public:
	bool AddAction(DWORD dwTime)
	{
		if (dwTime == 0)
			dwTime = ::TimeGet(TIME_MILLISECOND);
		if (m_setTime.size() > 0)
		{
			DWORD dwLast = m_setTime.back();
			if (dwTime <= dwLast || dwTime - dwLast >= m_dwMaxInterval)
			{
				m_setTime.clear();
				return true;
			}
		}
		m_setTime.push_back(dwTime);
		return Check();
	}

protected:
	bool Check()
	{
		DWORD dwAmount = m_dwAmount < 3 ? 3 : m_dwAmount;
		if (m_setTime.size() < dwAmount)
			return true;

		std::vector<int>	setInterval;
		for (int i = 1; i < (int)m_setTime.size(); i++)
			setInterval.push_back(m_setTime[i] - m_setTime[i-1]);

		int nIntervals = (int)setInterval.size();
		int nSum = 0;
		for (int j = 0; j < nIntervals; j++)
			nSum += setInterval[j];
		int nAverage = (DWORD)nSum / (DWORD)nIntervals;
		if (nAverage < m_nMinAverage)
			return false;

		int nMaxDeviation = 0;
		for (int k = 0; k < nIntervals; k++)
		{
			int nDeviation = abs(setInterval[k] - nAverage);
			if (nDeviation >= nMaxDeviation)
				nMaxDeviation = nDeviation;
		}
		if ((DWORD)nMaxDeviation <= m_dwMinDeviation)
			return false;
		return true;
	}

protected:
	std::vector<DWORD>	m_setTime;
	DWORD				m_dwAmount;
	int					m_nMinAverage;
	DWORD				m_dwMinDeviation;
	DWORD				m_dwMaxInterval;
};

#endif
