#include "ZoomNum.h"
#include "GamedataSet.h"

CZoomNum::CZoomNum()
{
}

CZoomNum::~CZoomNum()
{
	for(int i = 0; i < (int)m_setInfo.size(); i++)
	{
		ZoomNumInfo* pInfo = &m_setInfo[i];
		if(pInfo->pImgString)
			delete pInfo->pImgString;
		pInfo->pImgString = NULL;
	}
	m_setInfo.clear();
}
void CZoomNum::Add(int nNum, int nType, DWORD dwColor, DWORD dwDelay, int nRandOffset)
{
	ZoomNumInfo info;
	info.nNum = nNum;
	info.dwTimeBegin = ::TimeGet();
	if(nRandOffset != 0)
		info.nOffsetX = ::RandGet(nRandOffset*2)-nRandOffset;
	else
		info.nOffsetX = 0;
	info.dwColor = dwColor;
	info.dwDelay = dwDelay;
	info.pImgString = new CImageStringEx;
	if(!info.pImgString)
		return;
	if(nNum <= 0)
		nType = 1;

	vector<string> setEffect;
	char szNum[256] = "";
	if(nType == 1)
		sprintf(szNum, "%d", nNum);
	else
		sprintf(szNum, "%d%c%u", (DWORD)nNum / (DWORD)nType, 'X', nType);
	int nLen = strlen(szNum);
	for(int i = 0; i < nLen; i++)
	{
		char szEffect[256] = "";
		sprintf(szEffect, "CountB%c", szNum[i]);
		if((DWORD)nType > 1 && i >= nLen - 2)
			sprintf(szEffect, "CountK%c", szNum[i]);
		setEffect.push_back(string(szEffect));
	}
	info.pImgString->SetString(szNum, setEffect);
	info.pImgString->SetCharWidth(64);
	m_setInfo.push_back(info);
}
void CZoomNum::Show(CMyPos posShow)
{
	for(int i = m_setInfo.size()-1; i >= 0; i--)
	{
		ZoomNumInfo* pInfo = &m_setInfo[i];
		DWORD dwCurrentTime = ::TimeGet();
		if(dwCurrentTime < pInfo->dwTimeBegin + pInfo->dwDelay)
			continue;
		int nLen = strlen(pInfo->pImgString->GetString());
		DWORD dwTimePass = dwCurrentTime - pInfo->dwTimeBegin - pInfo->dwDelay;
		if(dwTimePass <= 250)
		{
			float fPass = dwTimePass * 1.0f / 250.0f;
			float fScale = 1.0f - (1.0f - 0.3f) * fPass;
			pInfo->pImgString->SetScale(fScale, fScale);
			int nX = (int)(posShow.x - pInfo->pImgString->GetCharWidth() * fScale * nLen / 2.0f + pInfo->nOffsetX);
			pInfo->pImgString->SetShowPos(nX, posShow.y);
			int nAlpha = (int)(223.0f * fPass) + 32;
			pInfo->pImgString->SetColor((pInfo->dwColor & 0x00FFFFFF) + (nAlpha << 24));
			pInfo->pImgString->Show();
			fScale = fScale * 1.3f;
			pInfo->pImgString->SetScale(fScale, fScale);
			nX = (int)(posShow.x - pInfo->pImgString->GetCharWidth() * fScale * nLen / 2.0f + pInfo->nOffsetX);
			int nY = (int)(posShow.y - pInfo->pImgString->GetCharWidth() * 0.3f / 3.0f);
			pInfo->pImgString->SetShowPos(nX, nY);
			nAlpha = (int)(nAlpha * 0.3f);
			pInfo->pImgString->SetColor((pInfo->dwColor & 0x00FFFFFF) + (nAlpha << 24));
			pInfo->pImgString->Show();
		}
		else if(dwTimePass <= 1450)
		{
			float fPass = (dwTimePass * 1.0f - 250.0f) / 1200.0f;
			float fScale = 0.3f;
			pInfo->pImgString->SetScale(fScale, fScale);
			int nX = (int)(posShow.x - pInfo->pImgString->GetCharWidth() * fScale * nLen / 2.0f + pInfo->nOffsetX);
			int nY = (int)(posShow.y - fPass * 80.0f);
			pInfo->pImgString->SetShowPos(nX, nY);
			int nAlpha = (int)((1.0f - fPass) * 255.0f);
			pInfo->pImgString->SetColor((pInfo->dwColor & 0x00FFFFFF) + (nAlpha << 24));
			pInfo->pImgString->Show();
		}
	}
}
void CZoomNum::Process(void)
{
	for(int i = m_setInfo.size()-1; i >= 0; i--)
	{
		ZoomNumInfo* pInfo = &m_setInfo[i];
		DWORD dwCurrentTime = ::TimeGet();
		if(dwCurrentTime < pInfo->dwTimeBegin + pInfo->dwDelay)
			continue;
		if(dwCurrentTime - pInfo->dwTimeBegin - pInfo->dwDelay <= 1450)
			continue;
		if(pInfo->pImgString)
			delete pInfo->pImgString;
		m_setInfo.erase(m_setInfo.begin()+i);
	}
}
