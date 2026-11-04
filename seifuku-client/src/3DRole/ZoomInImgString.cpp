#include "ZoomInImgString.h"

CZoomInImgString::CZoomInImgString()
{
	m_nStep			= 0;
	m_dwBeginTime	= 0;
	m_strEffectPrefix = "CountB";
	m_pImgString	= NULL;
}

CZoomInImgString::~CZoomInImgString()
{
	SAFE_DELETE(m_pImgString);
}

void CZoomInImgString::Add(const char* pszString)
{
	if (!pszString)
		return;

	vector<string> setEffect;
	int nLen = strlen(pszString);
	for (int i = 0; i < nLen; i++)
	{
		if (pszString[i] >= '0' && pszString[i] <= '9')
		{
			char szEffect[256] = "";
			sprintf(szEffect, "%s%c", m_strEffectPrefix.c_str(), pszString[i]);
			setEffect.push_back(szEffect);
		}
	}

	if (!m_pImgString)
		m_pImgString = new CImageStringEx;
	IF_NOT(m_pImgString)
		return;
	m_pImgString->SetString(pszString, setEffect);
	m_pImgString->SetCharWidth(58);
	m_pImgString->SetColor(0xffffffff);
	m_nStep			= 1;
	m_dwBeginTime	= ::TimeGet();
}

void CZoomInImgString::Show(void)
{
	if (!m_pImgString)
		return;

	int nHalfWidth = m_pImgString->GetCharWidth()/2;
	if (m_nStep == 1)
	{
		const int	nDuration	= 250;
		const float	fScaleFrom	= 3.6f;
		DWORD dwTimePass = __max(m_dwBeginTime, ::TimeGet()) - m_dwBeginTime;
		float fPercent = 1.0f;
		if (dwTimePass*1.0f/nDuration <= 1.0f)
			fPercent = dwTimePass*1.0f/nDuration;
		float fScale = (1.0f - fPercent)*(fScaleFrom - 1.0f) + 1.0f;
		m_pImgString->SetScale(fScale, fScale);
		int nAlpha = (int)(255.0f*fPercent);
		m_pImgString->SetColor(((DWORD)nAlpha << 24) + (m_pImgString->GetColor() & 0x00ffffff));
		int nLen = strlen(m_pImgString->GetString());
		int nX = (int)((CMyBitmap::GetScreenWidth() - m_pImgString->GetCharWidth()*(nLen*fScale))/2.0f + nHalfWidth);
		int nY = (int)((CMyBitmap::GetScreenHeight() - m_pImgString->GetCharWidth()*fScale - 100.0f)/2.0f);
		m_pImgString->SetShowPos(nX, nY);
		m_pImgString->Show();
		if (dwTimePass < (DWORD)nDuration)
			return;
		m_nStep = 2;
		m_dwBeginTime = ::TimeGet();
	}
	else if (m_nStep == 2)
	{
		const int	nDuration	= 500;
		const float	fScaleTo	= 1.2f;
		DWORD dwTimePass = __max(m_dwBeginTime, ::TimeGet()) - m_dwBeginTime;
		float fPercent = 1.0f;
		if (dwTimePass*1.0f/nDuration <= 1.0f)
			fPercent = dwTimePass*1.0f/nDuration;
		float fScale = (fScaleTo - 1.0f)*fPercent + 1.0f;
		m_pImgString->SetScale(fScale, fScale);
		int nLen = strlen(m_pImgString->GetString());
		int nX = (int)((CMyBitmap::GetScreenWidth() - m_pImgString->GetCharWidth()*(nLen*fScale))/2.0f + nHalfWidth);
		int nY = (int)((CMyBitmap::GetScreenHeight() - m_pImgString->GetCharWidth()*fScale - 100.0f)/2.0f);
		m_pImgString->SetShowPos(nX, nY);
		m_pImgString->Show();
		if (dwTimePass < (DWORD)nDuration)
			return;
		m_nStep = 3;
		m_dwBeginTime = ::TimeGet();
	}
	else if (m_nStep == 3)
	{
		const int	nDuration	= 100;
		const float	fScaleXTo	= 10.0f;
		const float	fScaleYTo	= 0.3f;
		DWORD dwTimePass = __max(m_dwBeginTime, ::TimeGet()) - m_dwBeginTime;
		float fPercent = 1.0f;
		if (dwTimePass*1.0f/nDuration <= 1.0f)
			fPercent = dwTimePass*1.0f/nDuration;
		float fScaleX = (fScaleXTo - 1.2f)*fPercent + 1.2f;
		float fScaleY = 1.2f - (1.2f - fScaleYTo)*fPercent;
		m_pImgString->SetScale(fScaleX, fScaleY);
		const int nOffset = 260;
		int nLen = strlen(m_pImgString->GetString());
		float fOffset = fPercent*(float)nOffset;
		int nX = (int)((CMyBitmap::GetScreenWidth() - m_pImgString->GetCharWidth()*(nLen*fScaleX))/2.0f + nHalfWidth + fOffset);
		int nY = (int)((CMyBitmap::GetScreenHeight() - m_pImgString->GetCharWidth()*fScaleY - 100.0f)/2.0f);
		m_pImgString->SetShowPos(nX, nY);
		int nAlpha = (int)(255.0f - 255.0f*fPercent);
		m_pImgString->SetColor(((DWORD)nAlpha << 24) + (m_pImgString->GetColor() & 0x00ffffff));
		m_pImgString->Show();
		if (dwTimePass < (DWORD)nDuration)
			return;
		m_nStep = 0;
		if (m_pImgString)
		{
			try
			{
				delete m_pImgString;
			}
			catch(...)
			{
			}
			m_pImgString = NULL;
		}
	}
}
