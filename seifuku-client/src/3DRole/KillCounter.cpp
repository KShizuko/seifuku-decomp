#include "KillCounter.h"
#include "3DGameMap.h"
#include "NDsound.h"

BOOL g_bKoShowRecord = true;

CKillCounter::CKillCounter()
	: m_objDigits(NULL), m_objTitle(NULL), m_objLastDigit(NULL), m_objString(NULL)
{
	m_dwColor		= 0xffffffff;
	m_nStatus		= 0;
	strcpy(m_szLastNumber, "0");
	memset(m_szNumber, 0, sizeof(m_szNumber));
	m_dwStringTime	= 0;
	memset(m_szString, 0, sizeof(m_szString));
	m_strEffectPrefix = "CountB";
	m_objDigits		= ::Game3DEffectCreate(NULL);
	m_objString		= ::Game3DEffectCreate(NULL);
	m_objTitle		= ::Game3DEffectCreate(NULL);
	m_objLastDigit	= ::Game3DEffectCreate(NULL);
}

CKillCounter::~CKillCounter()
{
	if(m_objLastDigit)	{ try { m_objLastDigit->Release(); } catch (...) {} m_objLastDigit = NULL; }
	if(m_objTitle)		{ try { m_objTitle->Release(); } catch (...) {} m_objTitle = NULL; }
	if(m_objString)		{ try { m_objString->Release(); } catch (...) {} m_objString = NULL; }
	if(m_objDigits)		{ try { m_objDigits->Release(); } catch (...) {} m_objDigits = NULL; }
}

void CKillCounter::ShowEffect(int x, int y, IGame3DEffect* pEffect)
{
	pEffect->SetPos(0.0f, -200.0f, 0.0f, -1, false);
	pEffect->SetOblique(false);
	pEffect->Process();

	int nWorldX, nWorldY;
	g_objGameMap.Screen2World(x, y, nWorldX, nWorldY);
	CMyPos posBg;
	g_objGameMap.World2Bg(nWorldX, nWorldY, posBg.x, posBg.y);
	pEffect->Show(posBg);
}

void CKillCounter::Add(DWORD dwNumber)
{
	if (m_nStatus == 1)
	{
		m_setNumber.push_back(dwNumber);
	}
	else
	{
		char szNumber[256] = "";
		sprintf(szNumber, "%u", dwNumber);
		this->AddString(szNumber);
	}
}

void CKillCounter::AddString(const char* pszNumber)
{
	if (!pszNumber || strlen(pszNumber) >= 16)
		return;

	char szEffect[256] = "";
	sprintf(szEffect, "%sZ", m_strEffectPrefix.c_str());
	m_objTitle->Clear();
	m_objTitle->Add(szEffect);

	int nNumber = atoi(pszNumber);
	int nLastDigit = nNumber%10;
	m_objLastDigit->Clear();
	char szDigit[256] = "";
	sprintf(szDigit, "%s%d", m_strEffectPrefix.c_str(), nLastDigit);
	m_objLastDigit->Add(szDigit);

	strcpy(m_szNumber, pszNumber);
	m_nStatus		= 1;
	m_dwBeginTime	= ::TimeGet();

	if (nNumber > 0 && (nNumber == 20 || nNumber == 50 || nNumber%100 == 0))
	{
		::DXPlaySound("sound/KOflash.wav");
		m_objZoomIn.Add(pszNumber);
	}
}

void CKillCounter::Flash(const char* pszNumber)
{
	::DXPlaySound("sound/KOflash.wav");
	m_objZoomIn.Add(pszNumber);
}

void CKillCounter::SetString(const char* pszString)
{
	if (!pszString)
		return;

	m_objString->Clear();
	strcpy(m_szString, pszString);
	for (unsigned int i = 0; i < strlen(m_szString); i++)
	{
		int nChar = m_szString[i];
		if (nChar >= '0' && nChar <= '9')
		{
			char szEffect[256] = "";
			sprintf(szEffect, "CountK%c", nChar);
			m_objString->Add(szEffect);
		}
	}
	m_dwStringTime = ::TimeGet();
}

void CKillCounter::ShowString(void)
{
	if (m_dwStringTime == 0)
		return;

	m_objString->Reset();
	const int nTime = 3000;
	const int nIndex = -1;
	if (m_setNumber.size() > 0)
		m_dwStringTime = ::TimeGet();

	DWORD dwTimePass = ::TimeGet() - m_dwStringTime;
	float fPercent = (dwTimePass * 1.0f / 3000.0f > 1.0f) ? 1.0f : dwTimePass * 1.0f / 3000.0f;
	int nAlpha = (int)(255.0f - 255.0f * fPercent);
	DWORD dwColor = (nAlpha << 24) + 0x00ffffff;
	m_objString->SetColor(dwColor, nIndex, false);

	const float fWidth = 53.0f;
	const float fScale = 1.4f;
	int nLen = strlen(m_szString);
	for (int i = 0; i < nLen; i++)
	{
		m_objString->SetPos(i*fWidth*fScale, 0.0f, 0.0f, i, false);
		m_objString->Scale(fScale, 1.0f, fScale, i, false);
	}
	int nShowX = (int)(CMyBitmap::GetScreenWidth() - nLen*fWidth*fScale);
	int nShowY = CMyBitmap::GetScreenHeight() - 180;
	this->ShowEffect(nShowX, nShowY, m_objString);

	if (::TimeGet() >= m_dwStringTime + nTime)
		m_dwStringTime = 0;
}

void CKillCounter::Show(void)
{
	int nShowX, nShowY;
	const float fScale = 0.5f;
	int nX, nY;
	const float fWidth = 26.0f;
	const int nTime = 100;
	if (m_nStatus == 1)
	{
		DWORD dwTimePass = ::TimeGet() - m_dwBeginTime;
		int nLen = strlen(m_szLastNumber);
		for (int i = 0; i < nLen; i++)
		{
			if (m_szLastNumber[i] != m_szNumber[i])
			{
				const float fAngle = 1.570796f;
				m_objDigits->Rotate(0.0f, 0.0f, dwTimePass * fAngle / 100.0f, i, false);
				const float fZoom = 2.0f;
				m_objDigits->Scale(1.0f*fScale, 1.0f*fScale, 1.0f*fScale + fScale*fZoom*dwTimePass/100.0f, i, false);
				DWORD dwAlpha = 255 - dwTimePass*255/nTime;
				DWORD dwColor = (dwAlpha << 24) + (m_dwColor & 0x00ffffff);
				m_objDigits->SetColor(dwColor, i, false);
			}
			else
			{
				m_objDigits->Rotate(0.0f, 0.0f, 0.0f, i, false);
				m_objDigits->Scale(1.0f*fScale, 1.0f*fScale, 1.0f*fScale, i, false);
				m_objDigits->SetColor(m_dwColor, i, false);
			}
			m_objDigits->SetPos(i*fWidth, 0.0f, 0.0f, i, false);
		}
	}
	else if (m_nStatus == 0)
	{
		int nLen = strlen(m_szLastNumber);
		for (int i = 0; i < nLen; i++)
		{
			m_objDigits->Rotate(0.0f, 0.0f, 0.0f, i, false);
			m_objDigits->Scale(1.0f*fScale, 1.0f*fScale, 1.0f*fScale, i, false);
			m_objDigits->SetPos(i*fWidth, 0.0f, 0.0f, i, false);
		}
		m_objDigits->SetColor(m_dwColor, -1, false);
	}

	nX = (int)(CMyBitmap::GetScreenWidth() - fWidth - 4.0f);
	nY = (int)((CMyBitmap::GetScreenHeight() - 85) - fWidth);
	nShowX = (int)(nX - strlen(m_szLastNumber)*fWidth - 10.0f);
	nShowY = nY;
	this->ShowEffect(nShowX, nShowY, m_objDigits);
	m_objTitle->SetColor(m_dwColor, -1, false);
	m_objTitle->Scale(1.0f*fScale*1.2f, 1.0f*fScale*1.2f, 1.0f*fScale*1.2f, -1, false);
	this->ShowEffect(nX, nY, m_objTitle);

	if (m_nStatus == 1)
	{
		DWORD dwTimePass = ::TimeGet() - m_dwBeginTime;
		int nXStart = CMyBitmap::GetScreenWidth();
		int nYStart = nY - 60;
		int nXEnd = (int)(nX - fWidth - 10.0f);
		int nYEnd = nY;
		float fPercent = (dwTimePass * 1.0f / 100.0f > 1.0f) ? 1.0f : dwTimePass * 1.0f / 100.0f;
		nShowX = (int)(nXStart + (nXEnd - nXStart)*fPercent);
		nShowY = (int)(nYStart + (nYEnd - nYStart)*fPercent);
		DWORD dwAlpha = dwTimePass*136/nTime;
		DWORD dwColor = (dwAlpha << 24) + (m_dwColor & 0x00ffffff);
		m_objLastDigit->SetColor(dwColor, -1, false);
		m_objLastDigit->Scale(1.0f*fScale, 1.0f*fScale, 1.0f*fScale, -1, false);
		this->ShowEffect(nShowX, nShowY, m_objLastDigit);
	}

	m_objZoomIn.Show();
	this->ShowString();

	if (m_nStatus == 1 && ::TimeGet() >= m_dwBeginTime + nTime)
	{
		m_nStatus		= 0;
		m_dwBeginTime	= ::TimeGet();
		m_objDigits->Clear();
		strcpy(m_szLastNumber, m_szNumber);
		for (unsigned int i = 0; i < strlen(m_szLastNumber); i++)
		{
			int nChar = m_szLastNumber[i];
			if (nChar >= '0' && nChar <= '9')
			{
				char szEffect[256] = "";
				sprintf(szEffect, "%s%c", m_strEffectPrefix.c_str(), nChar);
				m_objDigits->Add(szEffect);
			}
		}
		m_objDigits->Reset();
		m_objDigits->SetColor(m_dwColor, -1, false);
		if (m_setNumber.size() > 0)
		{
			DWORD dwNumber = m_setNumber.front();
			m_setNumber.pop_front();
			char szNumber[256] = "";
			sprintf(szNumber, "%u", dwNumber);
			this->AddString(szNumber);
		}
	}
}

CKoCounter::CKoCounter()
{
	m_dwColor	= 0xffffffff;
	m_nStatus	= 0;
	strcpy(m_szLastNumber, "0");
	memset(m_szNumber, 0, sizeof(m_szNumber));
	m_objDigits		= ::Game3DEffectCreate(NULL);
	m_objTitle		= ::Game3DEffectCreate(NULL);
	m_objLastDigit	= ::Game3DEffectCreate(NULL);
}

CKoCounter::~CKoCounter()
{
	if(m_objDigits)		{ try { m_objDigits->Release(); } catch (...) {} m_objDigits = NULL; }
	if(m_objTitle)		{ try { m_objTitle->Release(); } catch (...) {} m_objTitle = NULL; }
	if(m_objLastDigit)	{ try { m_objLastDigit->Release(); } catch (...) {} m_objLastDigit = NULL; }
}

void CKoCounter::ShowEffect(int x, int y, IGame3DEffect* pEffect)
{
	pEffect->SetPos(0.0f, -200.0f, 0.0f, -1, false);
	pEffect->SetOblique(false);
	pEffect->Process();

	int nWorldX, nWorldY;
	g_objGameMap.Screen2World(x, y, nWorldX, nWorldY);
	CMyPos posBg;
	g_objGameMap.World2Bg(nWorldX, nWorldY, posBg.x, posBg.y);
	pEffect->Show(posBg);
}

void CKoCounter::Add(DWORD dwNumber)
{
	if (m_nStatus == 1)
	{
		m_setNumber.push_back(dwNumber);
	}
	else
	{
		char szNumber[256] = "";
		sprintf(szNumber, "%u", dwNumber);
		this->AddString(szNumber);
	}
}

void CKoCounter::AddString(const char* pszNumber)
{
	if (!pszNumber || strlen(pszNumber) >= 16)
		return;

	m_objTitle->Clear();
	m_objTitle->Add("CountKZ");

	int nNumber = atoi(pszNumber);
	int nLastDigit = nNumber%10;
	m_objLastDigit->Clear();
	char szDigit[256] = "";
	sprintf(szDigit, "CountK%d", nLastDigit);
	m_objLastDigit->Add(szDigit);

	strcpy(m_szNumber, pszNumber);
	m_nStatus		= 1;
	m_dwBeginTime	= ::TimeGet();
}

void CKoCounter::Show(void)
{
	int nShowX, nShowY;
	const float fScale = 1.0f;
	int nX, nY;
	const float fWidth = 53.0f;
	const int nTime = 140;
	if (m_nStatus == 1)
	{
		DWORD dwTimePass = ::TimeGet() - m_dwBeginTime;
		int nLen = strlen(m_szLastNumber);
		for (int i = 0; i < nLen; i++)
		{
			if (m_szLastNumber[i] != m_szNumber[i])
			{
				const float fAngle = 1.570796f;
				m_objDigits->Rotate(0.0f, 0.0f, dwTimePass * fAngle / 140.0f, i, false);
				DWORD dwAlpha = 255 - dwTimePass*255/nTime;
				DWORD dwColor = (dwAlpha << 24) + (m_dwColor & 0x00ffffff);
				m_objDigits->SetColor(dwColor, i, false);
			}
			else
			{
				m_objDigits->Rotate(0.0f, 0.0f, 0.0f, i, false);
				m_objDigits->Scale(1.0f*fScale, 1.0f*fScale, 1.0f*fScale, i, false);
				m_objDigits->SetColor(m_dwColor, i, false);
			}
			m_objDigits->SetPos(i*fWidth, 0.0f, 0.0f, i, false);
		}
	}
	else if (m_nStatus == 0)
	{
		int nLen = strlen(m_szLastNumber);
		for (int i = 0; i < nLen; i++)
			m_objDigits->SetPos(i*fWidth, 0.0f, 0.0f, i, false);
		m_objDigits->Scale(1.0f*fScale, 1.0f*fScale, 1.0f*fScale, -1, false);
		m_objDigits->SetColor(m_dwColor, -1, false);
	}

	nX = (int)(CMyBitmap::GetScreenWidth() - fWidth - 4.0f);
	nY = (int)((CMyBitmap::GetScreenHeight() - 85) - fWidth);
	nShowX = (int)(nX - strlen(m_szLastNumber)*fWidth - 10.0f);
	nShowY = nY;
	this->ShowEffect(nShowX, nShowY, m_objDigits);
	m_objTitle->SetColor(m_dwColor, -1, false);
	m_objTitle->SetPos(0.0f, -100.0f, 0.0f, -1, false);
	m_objTitle->Scale(1.0f*fScale*1.2f, 1.0f*fScale*1.2f, 1.0f*fScale*1.2f, -1, false);
	this->ShowEffect(nX, nY, m_objTitle);

	if (m_nStatus == 1)
	{
		DWORD dwTimePass = ::TimeGet() - m_dwBeginTime;
		int nXStart = CMyBitmap::GetScreenWidth();
		int nYStart = nY;
		int nXEnd = (int)(nX - fWidth - 10.0f);
		int nYEnd = nY;
		float fPercent = dwTimePass * 1.0f / 140.0f;
		if (fPercent > 1.0f)
			fPercent = 1.0f;
		nShowX = (int)(nXStart + (nXEnd - nXStart)*fPercent);
		nShowY = (int)(nYStart + (nYEnd - nYStart)*fPercent);
		DWORD dwAlpha = dwTimePass*136/nTime;
		DWORD dwColor = (dwAlpha << 24) + (m_dwColor & 0x00ffffff);
		m_objLastDigit->SetColor(dwColor, -1, false);
		const float fScaleFrom = 2.0f;
		const float fScaleTo = 0.3f;
		float fScaleX = (1.0f - fPercent)*fScaleFrom + 1.0f;
		float fScaleZ = (1.0f - fScaleTo)*fPercent + fScaleTo;
		m_objLastDigit->Scale(1.0f*fScale*fScaleX, 1.0f*fScale, 1.0f*fScale*fScaleZ, -1, false);
		this->ShowEffect(nShowX, nShowY, m_objLastDigit);
	}

	if (m_nStatus == 1 && ::TimeGet() >= m_dwBeginTime + nTime)
	{
		m_nStatus		= 0;
		m_dwBeginTime	= ::TimeGet();
		m_objDigits->Clear();
		strcpy(m_szLastNumber, m_szNumber);
		for (unsigned int i = 0; i < strlen(m_szLastNumber); i++)
		{
			int nChar = m_szLastNumber[i];
			if (nChar >= '0' && nChar <= '9')
			{
				char szEffect[256] = "";
				sprintf(szEffect, "CountK%c", nChar);
				m_objDigits->Add(szEffect);
			}
		}
		m_objDigits->Reset();
		m_objDigits->SetColor(m_dwColor, -1, false);
		if (m_setNumber.size() > 0)
		{
			DWORD dwNumber = m_setNumber.front();
			m_setNumber.pop_front();
			char szNumber[256] = "";
			sprintf(szNumber, "%u", dwNumber);
			this->AddString(szNumber);
		}
	}
}
