#include "ImgNumCounter.h"
CImgNumCounter g_objImgNumCounter;

CImgNumCounter::CImgNumCounter()
{
	m_dwColor		= 0xFFFFFFFF;
	m_nNumber		= 0;
	m_nCurrent		= 0;
	m_nStep			= 0;
	m_posShow.y		= 0;
	m_posShow.x		= 0;
	m_dwZoomPercent	= 50;
	m_dwBeginTime	= 0;
	m_dwInterval	= 0;
}

CImgNumCounter::~CImgNumCounter()
{
	this->End();
}

void CImgNumCounter::Start	(int nNumber, int nStep , DWORD dwInterval )
{
	MYASSERT (nNumber > 0);

	m_nCurrent		= 0;
	m_nStep			= nStep;
	m_nNumber		= nNumber;

	m_dwInterval	= dwInterval;
	m_dwBeginTime	= ::TimeGet();
}

void CImgNumCounter::End(void)
{
	m_dwBeginTime	= 0;
	m_nCurrent		= 0;
	int nAmount = m_setNumImg.size();
	for(int i = 0; i < nAmount; i ++)
	{
		SAFE_DELETE(m_setNumImg[i].pImgString);
	}
	m_setNumImg.clear();
}
void CImgNumCounter::Reset (void)
{
	m_dwBeginTime	= ::TimeGet();
}

BOOL CImgNumCounter::Show (void)
{
	if (m_nNumber <= 0 || m_dwBeginTime == 0 || m_dwInterval == 0)
		return true;

	DWORD dwNow = ::TimeGet();
	int nAmount = m_setNumImg.size();
	for(int i = nAmount-1; i >= 0; i --)
	{
		DWORD dwPhase1	= m_dwInterval*2/5;
		DWORD dwPhase2	= m_dwInterval*3/4;
		DWORD dwPhase3	= m_dwInterval*5/4;
		NumImgInfo& info = m_setNumImg[i];
		CImageStringEx* pImgString = info.pImgString;
		if(dwNow < info.dwBeginTime + dwPhase1)
		{
			float fScale = (float)m_dwZoomPercent/100.0f;
			pImgString->SetScale(fScale, fScale);
			int nLen = strlen(pImgString->GetString());
			pImgString->SetShowPos((int)((float)m_posShow.x - pImgString->GetCharWidth()*fScale*((float)nLen/2.0f - 0.3f)), m_posShow.y);
			DWORD dwPercent = (dwNow - info.dwBeginTime)*100/dwPhase1;
			int nAlpha = (int)((double)(DWORD)((m_dwColor >> 24)*dwPercent)/100.0f);
			pImgString->SetColor((m_dwColor & 0x00FFFFFF) + ((DWORD)nAlpha << 24));
			pImgString->Show();
		}
		else if(dwNow < info.dwBeginTime + dwPhase2)
		{
			float fScale = (float)m_dwZoomPercent/100.0f;
			pImgString->SetScale(fScale, fScale);
			int nLen = strlen(pImgString->GetString());
			pImgString->SetShowPos((int)((float)m_posShow.x - pImgString->GetCharWidth()*fScale*((float)nLen/2.0f - 0.3f)), m_posShow.y);
			pImgString->SetColor(m_dwColor);
			pImgString->Show();
		}
		else if(dwNow < info.dwBeginTime + dwPhase3)
		{
			DWORD dwPercent = ((dwNow - (info.dwBeginTime + dwPhase2))*100/(dwPhase3 - dwPhase2)) % 100;
			float fScale = (float)((((double)dwPercent + (double)dwPercent)/100.0f + 1.0f)*((double)m_dwZoomPercent/100.0f));
			pImgString->SetScale(fScale, fScale);
			int nLen = strlen(pImgString->GetString());
			pImgString->SetShowPos((int)((float)m_posShow.x - pImgString->GetCharWidth()*fScale*((float)nLen/2.0f - 0.3f)), m_posShow.y);
			int nAlpha = (int)((double)(100 - dwPercent)/100.0f*(int)(m_dwColor >> 24));
			pImgString->SetColor(((DWORD)nAlpha << 24) + (m_dwColor & 0x00FFFFFF));
			pImgString->Show();
		}
		else
		{
			SAFE_DELETE(info.pImgString);
			m_setNumImg.erase(m_setNumImg.begin()+i);
		}
	}

	int nNumber = m_nStep*(int)((::TimeGet() - m_dwBeginTime)/m_dwInterval) + m_nNumber;
	if(nNumber != m_nCurrent && nNumber >= 0)
	{
		m_nCurrent = nNumber;
		CImageStringEx* pImgString = new CImageStringEx;
		if(pImgString)
		{
			vector<string> setEffect;
			char szNumber[256] = "";
			sprintf(szNumber, "%02d", m_nCurrent);
			int nLen = strlen(szNumber);
			for(int j = 0; j < nLen; j ++)
			{
				char szEffect[256] = "";
				sprintf(szEffect, "CountB%c", szNumber[j]);
				setEffect.push_back(szEffect);
			}
			pImgString->SetString(szNumber, setEffect);
			pImgString->SetCharWidth(64);
			NumImgInfo info;
			info.pImgString		= pImgString;
			info.dwBeginTime	= ::TimeGet();
			m_setNumImg.push_back(info);
		}
	}
	if(nNumber < 0)
	{
		this->End();
		return true;
	}
	return false;
}
