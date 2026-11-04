#pragma warning(disable:4786)
#include "../NDSound/NDsound.h"

#include "MapSound.h"
#include "Nd_Bitmap.h"
#include "3DGameMap.h"
#include "Hero.h"

CMapSound::CMapSound()
{
	m_nType = MAP_SOUND;
	m_pSound = NULL;
}
CMapSound::~CMapSound()
{
	m_bFocus = false;
	if(m_pSound)
		DXClose2DSound(m_pSound);
}

void CMapSound::Show(void* pInfo)
{
#ifdef _ARHUN_DEBUG
	if(!s_bStroke)
		return;
	static CMyBitmap* s_pObjBmp = NULL;
	if(!s_pObjBmp)
	{
		s_pObjBmp = new CMyBitmap;
		s_pObjBmp->Load("sound.tga");
	}

	CMySize sizeBmp;
	s_pObjBmp->GetSize(sizeBmp.iWidth, sizeBmp.iHeight);

	CMyPos posScr;
	g_objGameMap.World2Screen(m_posWorld.x, m_posWorld.y, posScr.x, posScr.y);

	CMyPos posMouse;
	::MouseCheck(posMouse.x, posMouse.y);
	if((posMouse.x > posScr.x - sizeBmp.iWidth/2) && 
	   (posMouse.y > posScr.y - sizeBmp.iHeight/2) &&
	   (posMouse.x < posScr.x - sizeBmp.iWidth/2+sizeBmp.iWidth) &&
	   (posMouse.y < posScr.y - sizeBmp.iHeight/2+sizeBmp.iHeight))
	{
		m_bFocus = true;
	}
	else
	{
		m_bFocus = false;
	}
	s_pObjBmp->Show(posScr.x - sizeBmp.iWidth/2, posScr.y - sizeBmp.iHeight/2);
#endif

}
void CMapSound::Process(void* pInfo)
{
#ifdef _ARHUN_DEBUG
	CMyPos posCell;
	g_objGameMap.World2Cell(m_posWorld.x, m_posWorld.y, posCell.x, posCell.y);
	if(g_objGameMap.IsPosVisible(posCell))
	{
		CInteractiveLayer* pLayer = g_objGameMap.GetInteractiveLayer();
		if(pLayer)
			pLayer->ApplyShow(this);
		
	}
	else
	{
		m_bFocus = false;
	}
#endif
}
BOOL CMapSound::IsFocus()
{
	return m_bFocus;
}
void CMapSound::GetWorldPos(CMyPos& posWorld)
{
	posWorld = m_posWorld;
}
void CMapSound::SetWorldPos(CMyPos posWorld)
{
	m_posWorld = posWorld;
	if(m_pSound)
		DXSet2DSoundPos(m_pSound, posWorld.x, posWorld.y);
	g_objGameMap.World2Cell(posWorld.x, posWorld.y, m_posCell.x, m_posCell.y);
	m_posCell.x -= 30;
	m_posCell.x -= 30;
}
void   CMapSound::LoadDataObj(FILE* fp)
{
	if(!fp)
		return;
	fread(m_szFile, sizeof(char), _MAX_PATH, fp);
	fread(&m_posWorld, sizeof(CMyPos), 1, fp);
	fread(&m_nRange, sizeof(int), 1, fp);
	fread(&m_nVolume, sizeof(int), 1, fp);
}
void   CMapSound::LoadTextObj(FILE* fp)
{
	if(!fp)
		return;
	fscanf(fp, "SoundFile=%s\n", m_szFile);
	fscanf(fp, "PosWorld=[%d,%d]\n", &m_posWorld.x, &m_posWorld.y);
	fscanf(fp, "Range=%d\n", &m_nRange);
	fscanf(fp, "Volume=%d\n", &m_nVolume);
}
void   CMapSound::SaveDataObj(FILE* fp)
{
	if(!fp)
		return;
	fwrite(m_szFile, sizeof(char), _MAX_PATH, fp);
	fwrite(&m_posWorld, sizeof(CMyPos), 1, fp);
	fwrite(&m_nRange, sizeof(int), 1, fp);
	fwrite(&m_nVolume, sizeof(int), 1, fp);
}
void   CMapSound::SaveTextObj(FILE* fp)
{
	if(!fp)
		return;
	fprintf(fp, "SoundFile=%s\n", m_szFile);
	fprintf(fp, "PosWorld=[%d,%d]\n", m_posWorld.x, m_posWorld.y);
	fprintf(fp, "Range=%d\n", m_nRange);
	fprintf(fp, "Volume=%d\n", m_nVolume);
}
BOOL CMapSound::Create(CMyPos posWorld, int nRange, int nVolume, char* pszFile)
{
	if(!pszFile)
		return false;

	this->SetWorldPos(posWorld);
	m_nRange = nRange;
	m_nVolume = nVolume;
	strcpy(m_szFile, pszFile);
	DXPlaySound(m_szFile);

	return true;
}
CMapSound* CMapSound::CreateNew(CMyPos posWorld, int nRange, int nVolume, char* pszFile)
{
	CMapSound* pSound = new CMapSound;
	MYASSERT(pSound);
	if(!pSound->Create(posWorld, nRange, nVolume, pszFile))
	{
		SAFE_DELETE(pSound);
		return NULL;
	}
	return pSound;
}
void CMapSound::Play()
{
	if(m_pSound)
		DXClose2DSound(m_pSound);
	m_pSound = (CSound*)DXPlaySound(m_szFile, m_posWorld.x, m_posWorld.y,
										m_nRange, -1, m_nVolume);
}
