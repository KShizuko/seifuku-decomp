#include <windows.h>
#include "AllMsg.h"
#include "GameMap.h"
#include "MsgWeather.h"
#include "Weather.h"
#include "math.h"

MYHEAP_IMPLEMENTATION(CWeather,s_heap)
CWeather::CWeather(IWeatherOwner* pOwner) : m_pOwner(pOwner)
{

	m_nDefaultType		= WEATHER_NONE;
	m_nDefaultIntensity	= 0;
	m_nDefaultDir		= 0;
	m_nDefaultColor		= 0x888888;
	m_nDefaultSpeedSecs	= 0;

	m_nCurrType			= 0;
	m_nCurrIntensity	= 0;
	m_nCurrDir			= 0;
	m_nCurrColor		= 0;
	m_nCurrParticle		= 0;

	m_nTargType			= 0;
	m_nTargIntensity	= 0;
	m_nTargDir			= 0;
	m_nTargColor		= 0;

	m_nIncrIntensity	= 0;
	m_nKeepSecs			= 0;
	m_nSpeedSecs		= 0;
}

CWeather::~CWeather()
{
}

bool CWeather::Create(int nType, int nIntensity, int nDir, DWORD nColor, DWORD nSpeedSecs)
{
	ASSERT(nType > WEATHER_NONE && nType < WEATHER_ALL);
	ASSERT(nIntensity >= 0 && nIntensity < MAX_WEATHER_INTENSITY);
	ASSERT(nDir >= 0 && nDir < MAX_WEATHER_DIR);

	if(nType == WEATHER_FINE || nIntensity == 0)
	{
		nType		= WEATHER_FINE;
		nIntensity	= 0;
	}

	m_nDefaultType		= nType;
	m_nDefaultIntensity	= nIntensity;
	m_nDefaultDir		= nDir;
	m_nDefaultColor		= nColor;
	m_nDefaultSpeedSecs	= nSpeedSecs;

	m_nCurrType			= nType;
	m_nCurrIntensity	= nIntensity;
	m_nCurrDir			= nDir;
	m_nCurrColor		= nColor;
	m_nCurrParticle		= GetParticle(m_nCurrIntensity);

	m_nTargType			= nType;
	m_nTargIntensity	= nIntensity;
	m_nTargDir			= nDir;
	m_nTargColor		= nColor;

	m_nIncrIntensity	= 0;
	m_nKeepSecs			= ::RandGet(MAX_KEEP_SECS - MIN_KEEP_SECS + 1) + MIN_KEEP_SECS;
	if(nType == WEATHER_CLOUDY)
		m_nKeepSecs	*= 5;
	m_nSpeedSecs		= WEATHER_NORMAL_SPEED;

	return true;
}

bool CWeather::SetNewWeather(int nType, int nIntensity, int nDir, DWORD nColor, DWORD nKeepSecs, DWORD nSpeedSecs)
{
	ASSERT(nType > WEATHER_NONE && nType < WEATHER_ALL);
	ASSERT(nIntensity >= 0 && nIntensity < MAX_WEATHER_INTENSITY);
	ASSERT(nDir >= 0 && nDir < MAX_WEATHER_DIR);

	if(nType == WEATHER_FINE || nIntensity == 0)
	{
		nType		= WEATHER_FINE;
		nIntensity	= 0;
	}

	if(nKeepSecs == 0)
	{
		m_nDefaultType		= nType;
		m_nDefaultIntensity	= nIntensity;
		m_nDefaultDir		= nDir;
		m_nDefaultColor		= nColor;
		m_nDefaultSpeedSecs	= nSpeedSecs;
	}

	m_nTargType			= nType;
	m_nTargIntensity	= nIntensity;
	m_nTargDir			= nDir;
	m_nTargColor		= nColor;

	if(m_nCurrType == WEATHER_FINE || m_nTargType == WEATHER_FINE
						|| m_nCurrType == m_nTargType && m_nCurrDir == m_nTargDir && m_nCurrColor == m_nTargColor)
		m_nIncrIntensity	= m_nTargIntensity - m_nCurrIntensity;
	else
		m_nIncrIntensity	= (0 - m_nCurrIntensity);

	if(nKeepSecs == 0)
	{
		m_nKeepSecs		= ::RandGet(MAX_KEEP_SECS - MIN_KEEP_SECS + 1) + MIN_KEEP_SECS;
		if(nType == WEATHER_CLOUDY)
			m_nKeepSecs	*= 5;
	}
	else
		m_nKeepSecs		= nKeepSecs;

	m_nSpeedSecs	= nSpeedSecs;

	return true;
}

void CWeather::OnTimer()
{
	if(!m_tLoop.ToNextTime(1))
		return;

	int nOldType		= m_nCurrType;
	int	nOldIntensity	= m_nCurrIntensity;
	int	nOldParticle	= m_nCurrParticle;

	if(m_nCurrType == m_nTargType && m_nTargType == m_nDefaultType && m_nDefaultType == WEATHER_FINE)
		return;

	if(m_nCurrType == WEATHER_FINE || m_nTargType == WEATHER_FINE
						|| m_nCurrType == m_nTargType && m_nCurrDir == m_nTargDir && m_nCurrColor == m_nTargColor)
	{
		if(m_nCurrType == m_nTargType && (m_nKeepSecs == 0 || --m_nKeepSecs <= 0))
		{
			int	nType		= (::RandGet(100) < WERTHER_FINE_PERCENT) ? WEATHER_FINE : m_nDefaultType;

			int	nIntensity	= m_nDefaultIntensity - MAX_WEATHER_INTENSITY/4 + ::RandGet(MAX_WEATHER_INTENSITY/2);
			if(nIntensity < 1)
				nIntensity	= 1;
			else if(nIntensity >= MAX_WEATHER_INTENSITY)
				nIntensity	= MAX_WEATHER_INTENSITY - 1;

			int	nDir		= (nType != nOldType && ::RandGet(100) < WERTHER_CHANGE_DIR_PERCENT) ? ::RandGet(MAX_WEATHER_DIR) : m_nTargDir;
			int	nKeepSecs	= ::RandGet(MAX_KEEP_SECS - MIN_KEEP_SECS + 1) + MIN_KEEP_SECS;
			if(nType == WEATHER_CLOUDY)
				nKeepSecs	*= 5;

			SetNewWeather(nType, nIntensity, nDir, m_nDefaultColor, nKeepSecs, m_nDefaultSpeedSecs);

			return;
		}

		if(m_nCurrType == WEATHER_FINE && m_nTargType != WEATHER_FINE)
		{
			m_nCurrType			= m_nTargType;
			m_nCurrIntensity	= 0;
			m_nCurrDir			= m_nTargDir;
			m_nCurrColor		= m_nTargColor;

			m_nIncrIntensity	= m_nTargIntensity - 0;
		}

		if(m_nCurrType != WEATHER_FINE && m_nIncrIntensity && m_nCurrIntensity != m_nTargIntensity )
		{
			m_nCurrIntensity = m_nTargIntensity;

			if(m_nIncrIntensity > 0 && m_nCurrIntensity > m_nTargIntensity 
										|| m_nIncrIntensity < 0 && m_nCurrIntensity < m_nTargIntensity)
				m_nCurrIntensity = m_nTargIntensity;
		}
	}
	else
	{
		ASSERT(m_nIncrIntensity <= 0);
		if(m_nIncrIntensity && m_nCurrIntensity != 0 )
		{
			const int times = 1;
			m_nCurrIntensity = 0;

			if(m_nCurrIntensity < 0)
				m_nCurrIntensity = 0;
		}
	}

	if(m_nCurrIntensity != nOldIntensity)
		m_nCurrParticle = GetParticle(m_nCurrIntensity);

	if(m_nCurrType != nOldType || m_nCurrParticle != nOldParticle)
	{
		CMsgWeather	cMsg;
		if(cMsg.Create(m_nCurrType, m_nCurrParticle, m_nCurrDir, m_nCurrColor))
			m_pOwner->BroadcastMsg(&cMsg);
#ifdef	PALED_DEBUG
		MSGBUF	szMsg;
		sprintf(szMsg, " 类型：%d，粒子数：%d，方向：%d，颜色：0x%06X", 
								m_nCurrType, m_nCurrParticle, m_nCurrDir, m_nCurrColor);
		CMsgTalk	msg;
		msg.Create("[天气]", "大家", szMsg);
		m_pOwner->BroadcastMsg(&msg);
#endif
	}

	if(m_nCurrType != WEATHER_FINE && m_nCurrIntensity == 0 && m_nIncrIntensity <= 0)
	{
		m_nCurrType			= WEATHER_FINE;
		m_nCurrIntensity	= 0;
		m_nCurrDir			= 0;
		m_nCurrColor		= 0;
		m_nCurrParticle		= 0;

		m_nIncrIntensity	= 0;
	}
}

int CWeather::GetParticle(int nIntensity)
{
	int nParticle;
	nParticle	= nIntensity + (int)sqrt(MAX_WEATHER_INTENSITY);
	if(nParticle >= MAX_WEATHER_INTENSITY)
		nParticle = MAX_WEATHER_INTENSITY - 1;
	nParticle = nParticle*nParticle / MAX_WEATHER_INTENSITY;

	switch(m_nCurrType)
	{
	case	WEATHER_FINE:
		break;
	case	WEATHER_RAINY:
		nParticle	= (nParticle + 1) / 2;
		break;
	case	WEATHER_SNOWY:
		nParticle	= (nParticle + 3) / 4;
		break;
	case	WEATHER_SANDS:
		nParticle	= (nParticle + 1) / 2;
		break;
	case	WEATHER_LEAF:
	case	WEATHER_BAMBOO:
	case	WEATHER_FLOWER:
	case	WEATHER_FLYING:
	case	WEATHER_DANDELION:
		nParticle	= (nParticle + 24) / 25;
		break;
	case	WEATHER_WORM:
		nParticle	= (nParticle + 29) / 30;
		break;
	case	WEATHER_CLOUDY:
		nParticle	= (nParticle + 33) / 34;
		break;
	default:
		ASSERT(!"switch(nType)");
		nParticle = 0;
	}
	return nParticle;
}

bool CWeather::SendWeather(CUser* pUser)
{
	CHECKF(pUser);

	CMsgWeather	msg;
	IF_OK(msg.Create(GetType(), GetParticle(), GetDir(), GetColor()))
		pUser->SendMsg(&msg);

	return true;
}

bool CWeather::SendNoWeather(CUser* pUser)
{
	CHECKF(pUser);

	CMsgWeather	msg;
	IF_OK(msg.Create(WEATHER_FINE, 0, 0, 0))
		pUser->SendMsg(&msg);

	return true;
}
