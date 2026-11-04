#include "3DMapEffect.h"
#include "InteractiveLayer.h"
#include "3DGameMap.h"

BOOL C3DMapEffect::s_bStroke = false;
C3DMapEffect::C3DMapEffect()
{
	m_objEffect = NULL;
	m_bDie = false;
	m_nOffsetY = 0;
	strcpy(m_szIndex, "");
	m_fHorizontal	= 0.0;
	m_fVertical		= 0.0;
	m_bOver			= false;
	m_nDir			= -1;
	m_objEffect = ::Game3DEffectCreate(NULL);
}

C3DMapEffect::~C3DMapEffect()
{
	if(m_objEffect)
	{
		m_objEffect->Release();
	}
}

void C3DMapEffect::Show(void* pInfo)
{
	m_objEffect->SetHeight(m_nOffsetY);

	CMyPos posScr, posWorld;
	if(s_bStroke)
	{
		g_objGameMap.Bg2World(m_posWorld.x, m_posWorld.y, posWorld.x, posWorld.y);
		g_objGameMap.World2Screen(posWorld.x, posWorld.y, posScr.x, posScr.y);
		CMyBitmap::ShowBlock(posScr.x-32, posScr.y-32, posScr.x+32, posScr.y+32, 0xff000000);
	}
	if(!m_bDie)
	{
		float fHorizontal = 0.0f;
		if(m_nDir >= 0)
		{
			m_nDir = m_nDir % 8;
			float fPi = 3.1415925f;
			fHorizontal = (float)(m_nDir * -45 - 45) * fPi / 180.0f;
		}
		fHorizontal = fHorizontal + m_fHorizontal;
		m_objEffect->Rotate(m_fVertical, 0.0, fHorizontal, -1, false);
		m_objEffect->Show(m_posWorld);
	}
}
void C3DMapEffect::SetRotate(float fHorizontal, float fVertical)
{
	m_fHorizontal = fHorizontal;
	m_fVertical = fVertical;
}
void C3DMapEffect::Process(void* pInfo)
{
	BOOL* pbOver = (BOOL*)pInfo;
	if(m_bDie)
	{
		*pbOver = true;
		return;
	}
	*pbOver = m_objEffect->Process();
	m_bOver = *pbOver;
	CMyPos posCell;
	CMyPos posWorld;
	g_objGameMap.Bg2World(m_posWorld.x, m_posWorld.y, posWorld.x, posWorld.y);
	g_objGameMap.World2Cell(posWorld.x, posWorld.y, posCell.x, posCell.y);
	this->SetPos(posCell);
	if((!*pbOver && g_objGameMap.IsPosVisible(posCell)) || (g_objGameMap.IsPosVisible(posCell) && !m_bDelSelf))
	{
		CInteractiveLayer* pLayer = g_objGameMap.GetInteractiveLayer();
		if(pLayer)
			pLayer->ApplyShow(this);
	}
	if(!m_bDelSelf)
		*pbOver = false;
}
C3DMapEffect* C3DMapEffect::CreateNew(CMyPos posWorld, char* pszIndex, BOOL bDelSelf, BOOL bSave, int nDir)
{
	if(!pszIndex)
		return NULL;
	C3DMapEffect* pEffect = new C3DMapEffect;
	if(!pEffect)
		return NULL;
	if(!pEffect->Create(posWorld, pszIndex, bDelSelf, bSave))
	{
		SAFE_DELETE(pEffect);
		return NULL;
	}
	CMyPos posCell;
	g_objGameMap.World2Cell(posWorld.x, posWorld.y, posCell.x, posCell.y);
	pEffect->SetPos(posCell);
	pEffect->SetDir(nDir);
	return pEffect;
}
BOOL C3DMapEffect::Create(CMyPos posWorld, char* pszIndex, BOOL bDelSelf, BOOL bSave)
{
	if(!pszIndex)
		return false;
	int nBgX, nBgY;
	g_objGameMap.World2Bg(posWorld.x, posWorld.y, nBgX, nBgY);
	m_posWorld.x = nBgX;
	m_posWorld.y = nBgY;
	strcpy(m_szIndex, pszIndex);
	m_objEffect->Add(pszIndex);
	m_bDelSelf = bDelSelf;
	m_bSave = bSave;
	if(bSave)
		m_objEffect->EverPlay();
	return true;
}
void C3DMapEffect::GetWorldPos(CMyPos& posWorld)
{
	posWorld = m_posWorld;
}
void C3DMapEffect::SetWorldPos(CMyPos posWorld)
{
	int nBgX, nBgY;
	g_objGameMap.World2Bg(posWorld.x, posWorld.y, nBgX, nBgY);
	m_posWorld.x = nBgX;
	m_posWorld.y = nBgY;
}
void C3DMapEffect::LoadDataObj(FILE* fp)
{
	if(!fp)
		return;
	fread(m_szIndex, sizeof(char), 64, fp);
	fread(&m_posWorld, sizeof(CMyPos), 1, fp);
	this->Create(m_posWorld, m_szIndex);
	m_objEffect->EverPlay();
	m_bDelSelf	= true;
	m_bDie = false;
	m_nOffsetY = 0;
	m_bSave = true;
}
void C3DMapEffect::LoadTextObj(FILE* fp)
{
	if(!fp)
		return;
	fscanf(fp, "Index=%s\n", m_szIndex);
	fscanf(fp, "PosWorld=[%d,%d]\n", &m_posWorld.x, &m_posWorld.y);
	this->Create(m_posWorld, m_szIndex);
	m_objEffect->EverPlay();
	m_bDelSelf	= true;
	m_bDie = false;
	m_nOffsetY = 0;
	m_bSave = true;
}
void C3DMapEffect::SaveDataObj(FILE* fp)
{
	if(!fp)
		return;
	fwrite(m_szIndex, sizeof(char), 64, fp);
	CMyPos posWorld;
	g_objGameMap.Bg2World(m_posWorld.x, m_posWorld.y, posWorld.x, posWorld.y);
	fwrite(&posWorld, sizeof(CMyPos), 1, fp);
}
void C3DMapEffect::SaveTextObj(FILE* fp)
{
	if(!fp)
		return;
	fprintf(fp, "Index=%s\n", m_szIndex);
	CMyPos posWorld;
 	g_objGameMap.Bg2World(m_posWorld.x, m_posWorld.y, posWorld.x, posWorld.y);
	fprintf(fp, "PosWorld=[%d,%d]\n", posWorld.x, posWorld.y);
}
BOOL C3DMapEffect::IsFocus()
{
	CMyPos posScr, posWorld;
	if(s_bStroke)
	{
		g_objGameMap.Bg2World(m_posWorld.x, m_posWorld.y, posWorld.x, posWorld.y);
		g_objGameMap.World2Screen(posWorld.x, posWorld.y, posScr.x, posScr.y);
		CMyPos posMouse;
		::MouseCheck(posMouse.x, posMouse.y);

		if((posMouse.x > posScr.x-32) &&
		   (posMouse.y > posScr.y-32) &&
		   (posMouse.x < posScr.x+32) &&
		   (posMouse.y < posScr.y+32))
		{
			return true;
		}
	}
	return false;
}
void C3DMapEffect::SetDir(int nDir)
{
	m_nDir = nDir;
}
