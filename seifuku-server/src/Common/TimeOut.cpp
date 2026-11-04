#include "windows.h"
#include "TimeOut.h"

bool CTimeOut::ToNextTick(int nSecs)
{
	if(IsTimeOut(nSecs))
	{
		if(time(NULL) >= m_tUpdateTime + nSecs*2)
			return Update(),true;
		else
			return (m_tUpdateTime+=nSecs),true;
	}
	else
		return false;
}

bool CTimeOutMS::ToNextTick(int nMilliSecs)
{
	if(IsTimeOut(nMilliSecs))
	{
		if(::TimeGet() >= m_tUpdateTime + nMilliSecs*2)
			return Update(),true;
		else
			return (m_tUpdateTime+=nMilliSecs),true;
	}
	else
		return false;
}
