#include "ImageString.h"

CImageString::CImageString()
{
	m_fScale = 1.0f;
	m_dwColor = 0xffffffff;
	m_posShow.x = m_posShow.y = 0;
}

CImageString::~CImageString()
{

}

CImageString* CImageString::CreateNew(const char* pszString, CMyPos posShow)
{
	if (!pszString)
		return NULL;

	CImageString* pObj = new CImageString;
	if (!pObj)
		return NULL;

	pObj->SetString(pszString);
	pObj->SetShowPos(posShow);

	return pObj;
}

void CImageString::SetScalePercent(DWORD dwPercent)
{
	m_fScale = dwPercent / 100.0f;
}

void CImageString::SetShowPos(CMyPos posShow)
{
	m_posShow = posShow;
}

void CImageString::SetColor(DWORD dwColor, DWORD dwAlpha)
{
	dwColor &= 0x00ffffff;
	dwAlpha &= 0xff;
	m_dwColor = dwColor + (dwAlpha << 24);
}

void CImageString::SetString	(const char* pszString)
{
	if (!pszString)
		return;
	if (m_objImgString.GetStrLen() > 0 && strcmp(pszString, m_objImgString.GetString()) == 0)
		return;

	vector<string> setEffect;
	int nLen = strlen(pszString);
	for (int i = 0; i < nLen; i++)
	{
		char szEffect[256] = "";
		sprintf(szEffect, "CountB%c", pszString[i]);
		setEffect.push_back(string(szEffect));
	}
	m_objImgString.SetString(pszString, setEffect);
}

void CImageString::Show	(void)
{
	m_objImgString.SetColor(m_dwColor);
	m_objImgString.SetCharWidth(64);
	float fScale = m_fScale * 0.32f;
	m_objImgString.SetScale(fScale, fScale);
	int nX = (int)(m_posShow.x - m_objImgString.GetCharWidth() * fScale / 2.0f);
	int nY = (int)(m_posShow.y - m_objImgString.GetCharWidth() * fScale / 2.0f);
	m_objImgString.SetShowPos(nX, nY);
	m_objImgString.Show();
}
