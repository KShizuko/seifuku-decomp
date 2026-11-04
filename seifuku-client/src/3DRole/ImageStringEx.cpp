#include "ImageStringEx.h"
#include "3DGameMap.h"

CMyHeap CImageStringEx::s_heap("CImageStringEx");

CImageStringEx::CImageStringEx()
	: m_objEffect(NULL)
{
	m_posShow.y		= 0;
	m_posShow.x		= 0;
	m_dwColor		= 0xFFFFFFFF;
	m_fScaleY		= 1.0f;
	m_fScaleX		= 1.0f;
	m_nRotateY		= 0;
	m_nRotateZ		= 0;
	m_nRotateX		= 0;
	m_nCharWidth	= 0;
	memset(m_szString, 0, sizeof(m_szString));
	m_objEffect = ::Game3DEffectCreate(NULL);
}

CImageStringEx::~CImageStringEx()
{
	if(m_objEffect)
	{
		try
		{
			m_objEffect->Release();
		}
		catch (...)
		{
		}
		m_objEffect = NULL;
	}
}
void CImageStringEx::ShowEffect(int x, int y, IGame3DEffect* pEffect)
{
	pEffect->SetOblique(false);
	pEffect->SetPos(0.0f, -200.0f, 0.0f, -1, false);
	pEffect->Process();

	int nWorldX, nWorldY;
	g_objGameMap.Screen2World(x, y, nWorldX, nWorldY);
	CMyPos posBg;
	g_objGameMap.World2Bg(nWorldX, nWorldY, posBg.x, posBg.y);
	pEffect->Show(posBg);
}
void CImageStringEx::SetString(const char* pszString, vector<string>& setEffect)
{
	if(!pszString)
		return;
	if(strcmp(pszString, m_szString) == 0)
		return;
	int nLen = strlen(pszString);
	if(nLen >= sizeof(m_szString))
		return;
	if(nLen != (int)setEffect.size())
		return;

	m_objEffect->Clear();
	strcpy(m_szString, pszString);
	m_setId.clear();
	for(int i = 0; i < nLen; i++)
	{
		m_setId.push_back(m_objEffect->Add((char*)setEffect[i].c_str()));
	}
}
void CImageStringEx::Show(void)
{
	m_objEffect->Reset();
	int nLen = strlen(m_szString);
	for(int i = 0; i < nLen; i++)
	{
		m_objEffect->Scale(m_fScaleX, 1.0f, m_fScaleY, i, false);
		float fRadian = D3DX_PI / 180.0f;
		m_objEffect->Rotate(m_nRotateX * fRadian, m_nRotateY * fRadian, m_nRotateZ * fRadian, i, false);
		m_objEffect->SetColor(m_dwColor, m_setId[i], false);
		m_objEffect->SetPos((float)(i * m_nCharWidth) * m_fScaleX, 0.0f, 0.0f, i, false);
	}
	this->ShowEffect(m_posShow.x, m_posShow.y, m_objEffect);
}
