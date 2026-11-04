#include	<windows.h>
#include	<mmsystem.h>
#include	"ND_Bitmap.h"
#include	"c3_capscreen.h"
#include	"c3_datafile.h"
#include	"BaseFunc.h"
#include	"AlphaMask.h"
#include	"MyHeap.h"
#include	"3DObjX.h"
#include	"MyBitmapX.h"
#include	"Ani.h"
#include	 <direct.h>
#include	 <io.h>

#include <deque>
#include <map>
#include <set>
using namespace std;

int	_SCR_WIDTH	=800;
int	_SCR_HEIGHT	=600;

struct CMyFont
{
	BOOL				m_bDataFont;
	C3Font*				m_pFont;
	int					m_nSize;
	map<DWORD, DWORD>	m_mapChar;
	BYTE*				m_pData;
	int		LoadDbcsSpec	();
	BYTE*	GetCharData		(DWORD dwCode);
};

deque<CMyFont*> s_FontSet;
set<WORD> s_setDbcsSmall;
static const char* s_pszDbcsSmallFile = "ini/DbcsSmall.txt";
C3Camera*   s_pCamera = NULL;
C3Camera*   s_pProjectCamera = NULL;
BOOL		s_bProjectCamera = false;
static CMySize s_sizeCamera;
int s_nShowMode		=modeNone;
float s_fZoom = 1.0;
static int			s_nObliqueAngle = -30;
static BOOL			s_bLight = false;
static D3DXVECTOR3	s_vLight ( 0.0f, 0.0f, 0.0f );
static DWORD		s_dwLightDiffuse = 0xffffffff;
static DWORD		s_dwLightAmbient = 0xff888888;

static CMyFont* CreateFont ( const char* pszFontName, int nSize, int bAntiAlias );

void AniFileSetDestroy ( void );

CMyHeap CMyBitmapX::s_heap("CMyBitmapX");

CMyBitmapX::CMyBitmapX ( void )
{
	m_sprite = 0;
	m_pMask = NULL;
	m_lock.pBits = NULL;
}

CMyBitmapX::~CMyBitmapX ( void )
{
	Destroy ();
}
DWORD CMyBitmapX::Release ( void )
{
	delete this;
	return 0;
}
void CMyBitmapX::Destroy ( void )
{
	Sprite_Unload ( &m_sprite );
	SAFE_DELETE(m_pMask);
}
BOOL CMyBitmapX::Load(char* pszBmpFile)
{
	IF_NOT(pszBmpFile)
		return false;
	SAFE_DELETE(m_pMask);
	char szBmpFile[256] = "";
	strcpy(szBmpFile, pszBmpFile);
	strupr(szBmpFile);
	if(strstr(szBmpFile, ".MSK"))
	{
		m_pMask = new CAlphaMask;
		IF_NOT(m_pMask)
			return false;
		char szPicFileName[256];
		strcpy(szPicFileName, pszBmpFile);
		int nLength = strlen(pszBmpFile);
		szPicFileName[nLength-3] = 'D';
		szPicFileName[nLength-2] = 'D';
		szPicFileName[nLength-1] = 'S';
		if(!Sprite_Load ( &m_sprite, szPicFileName, D3DPOOL_MANAGED, 0, 0 ))
		{
			int nLength = strlen(pszBmpFile);
			szPicFileName[nLength-3] = 'T';
			szPicFileName[nLength-2] = 'G';
			szPicFileName[nLength-1] = 'A';
			if(!Sprite_Load ( &m_sprite, szPicFileName, D3DPOOL_MANAGED, 0, 0 ))
				return false;
		}
		CMySize sizeInfo;
		this->GetSize(sizeInfo.iWidth, sizeInfo.iHeight);
		if(!m_pMask->Load(sizeInfo, pszBmpFile))
		{
			this->Destroy();
			return false;
		}
		return true;
	}

	BOOL bReturn = Sprite_Load( &m_sprite, pszBmpFile, D3DPOOL_MANAGED, 0, 0 );
	return bReturn;
}
void CMyBitmapX::Show ( int x, int y, int alpha, DWORD dwShowWay )
{
	if(s_fZoom == 1.0)
	{
		if (s_nShowMode != modeSprite)
			CMyBitmap::PrepareSprite();
		if (alpha != 0)
			Sprite_SetColor ( m_sprite, alpha, 255, 255, 255 );
		Sprite_SetCoor ( m_sprite, 0, x, y );
		Sprite_Draw ( m_sprite,dwShowWay );
	}
	else
	{
		CMySize sizeBitmap = {0, 0};
		this->GetSize(sizeBitmap.iWidth, sizeBitmap.iHeight);
		sizeBitmap.iHeight *= s_fZoom;
		sizeBitmap.iWidth *= s_fZoom;
		this->ShowEx(x, y, NULL, sizeBitmap.iWidth, sizeBitmap.iHeight, dwShowWay, _SHOWWAY_NORMAL, 0);
	}
}
void CMyBitmapX::ShowEx ( int x, int y, RECT *lpSrc, DWORD dwWidth, DWORD dwHeight, int alpha, DWORD dwShowWay, int nRotateAngle )
{
	int nWidth = 0, nHeight = 0;
	if(!lpSrc && (nWidth == dwWidth) && (nHeight == dwHeight) && (s_fZoom == 1.0))
	{
		if (s_nShowMode != modeSprite)
			CMyBitmap::PrepareSprite();
		if (alpha != 0)
			Sprite_SetColor ( m_sprite, alpha, 255, 255, 255 );
		Sprite_SetCoor ( m_sprite, 0, x, y );
		if (nRotateAngle != 0)
			Sprite_Rotate ( m_sprite, nRotateAngle );
		Sprite_Draw ( m_sprite,dwShowWay );
		return;
	}
	if (s_nShowMode != modeSprite)
		CMyBitmap::PrepareSprite();
	if (alpha != 0)
		Sprite_SetColor ( m_sprite, alpha, 255, 255, 255 );
	Sprite_SetCoor ( m_sprite, lpSrc, x, y, dwWidth, dwHeight );
	if (nRotateAngle != 0)
		Sprite_Rotate ( m_sprite, nRotateAngle );
	Sprite_Draw ( m_sprite,dwShowWay );
}
void CMyBitmapX::ShowBlend ( int x, int y, RECT *lpSrc, DWORD dwWidth, DWORD dwHeight, int alpha, int nRotateAngle, DWORD dwSrcBlend, DWORD dwDestBlend )
{
	if(!lpSrc && (dwWidth == 0) && (dwHeight == 0) && (s_fZoom == 1.0))
	{
		if (s_nShowMode != modeSprite)
			CMyBitmap::PrepareSprite();
		if (alpha != 0)
			Sprite_SetColor ( m_sprite, alpha, 255, 255, 255 );
		Sprite_SetCoor ( m_sprite, 0, x, y );
		if (nRotateAngle != 0)
			Sprite_Rotate ( m_sprite, nRotateAngle );
		Sprite_DrawEx ( m_sprite, dwSrcBlend, dwDestBlend );
		return;
	}
	if (s_nShowMode != modeSprite)
		CMyBitmap::PrepareSprite();
	if (alpha != 0)
		Sprite_SetColor ( m_sprite, alpha, 255, 255, 255 );
	Sprite_SetCoor ( m_sprite, lpSrc, x, y, dwWidth, dwHeight );
	if (nRotateAngle != 0)
		Sprite_Rotate ( m_sprite, nRotateAngle );
	Sprite_DrawEx ( m_sprite, dwSrcBlend, dwDestBlend );
}
BOOL CMyBitmapX::LockRect ( RECT rect )
{
	if (!m_sprite->lpTex || m_lock.pBits)
		return false;
	m_rect = rect;
	if (Texture_LockRect ( m_sprite->lpTex, rect, m_lock ) && m_lock.pBits)
		return true;
	return false;
}
void CMyBitmapX::SetPixel ( int x, int y, DWORD dwColor )
{
	if (!m_sprite->lpTex || !m_lock.pBits)
		return;
	if (x < m_rect.left || y < m_rect.top || x >= m_rect.right || y >= m_rect.bottom)
		return;
	*( DWORD* )( ( BYTE* )m_lock.pBits + ( y - m_rect.top ) * m_lock.Pitch + ( x - m_rect.left ) * 4 ) = dwColor;
}
void CMyBitmapX::Unlock ( void )
{
	if (m_sprite->lpTex && m_lock.pBits)
	{
		Texture_Unlock ( m_sprite->lpTex );
		m_lock.pBits = NULL;
	}
}
void CMyBitmapX::GetSize ( int& nWidth, int& nHeight)
{
	nWidth = m_sprite->lpTex->Info.Width;
	nHeight = m_sprite->lpTex->Info.Height;
}
DWORD CMyBitmapX::GetSize ( void )
{
	return m_sprite->lpTex->Info.Height * m_sprite->lpTex->Info.Width;
}
int	CMyBitmapX::GetWidth ( void )
{
	return m_sprite->lpTex->Info.Width;;
}
int	CMyBitmapX::GetHeight ( void )
{
	return m_sprite->lpTex->Info.Height;
}
void CMyBitmapX::SetColor ( BYTE a, BYTE r, BYTE g, BYTE b )
{
	Sprite_SetColor ( m_sprite, a, r, g, b );
}
int CMyBitmapX::IsbTransparentFormat()
{
	if (!m_sprite->lpTex)
		return 0;
	int nType = 0;
	switch (m_sprite->lpTex->Info.Format)
	{
	case D3DFMT_A8R8G8B8:
	case D3DFMT_R5G6B5:
	case D3DFMT_A4R4G4B4:
	case D3DFMT_DXT3:
		nType = 1;
		break;
	}
	if (m_pMask)
		nType = 2;
	return nType;
}
BOOL CMyBitmapX::IsTransparent(CMyPos posPoint)
{
	if(!m_pMask)
		return true;
	else
		return m_pMask->IsTransparent(posPoint);
}
CMyBitmap* MyBitmapCreate ( const char* pszBmpFile )
{
	CMyBitmapX* pBmp = new CMyBitmapX;
	if (!pBmp)
		return NULL;
	if (pszBmpFile)
	{
		char szBmpFile[256] = "";
		strcpy(szBmpFile, pszBmpFile);
		::ReplaceString(szBmpFile, '\\', '/');
		if (!pBmp->Load(szBmpFile))
		{
			SAFE_DELETE(pBmp);
			return NULL;
		}
	}
	return pBmp;
}
CMyBitmap* BmpCreate ( void )
{
	return new CMyBitmapX;
}
BOOL CMyBitmap::GameCameraCreate()
{
	if(s_pCamera)
		CMyBitmap::GameCameraDestroy();
	s_pCamera = new C3Camera;
	if(!s_pCamera)
		return false;
	Camera_Clear ( s_pCamera );
	s_pCamera->lpFrom = new D3DXVECTOR3[1];
	s_pCamera->lpFrom[0].x = _SCR_WIDTH/2;
	s_pCamera->lpFrom[0].y = -1000;
	s_pCamera->lpFrom[0].z = _SCR_HEIGHT/2;

	s_pCamera->lpTo = new D3DXVECTOR3[1];
	s_pCamera->lpTo[0].x = _SCR_WIDTH/2;
	s_pCamera->lpTo[0].y = 0;
	s_pCamera->lpTo[0].z = _SCR_HEIGHT/2;

	s_pCamera->fNear = 1.0f;
	s_pCamera->fFar = 10000.0f;

	s_pProjectCamera = new C3Camera;
	Camera_Clear ( s_pProjectCamera );
	s_pProjectCamera->lpFrom = new D3DXVECTOR3[1];
	s_pProjectCamera->lpFrom[0].x = 0;
	s_pProjectCamera->lpFrom[0].y = -800;
	s_pProjectCamera->lpFrom[0].z = tan ( D3DXToRadian ( 30 ) ) * s_pProjectCamera->lpFrom[0].y;

	s_pProjectCamera->lpTo = new D3DXVECTOR3[1];
	s_pProjectCamera->lpTo[0].x = 0;
	s_pProjectCamera->lpTo[0].y = 0;
	s_pProjectCamera->lpTo[0].z = 0;

	s_pProjectCamera->fNear = 100.0f;
	s_pProjectCamera->fFar = 8000.0f;
	s_pProjectCamera->fFov = D3DXToRadian ( 45 );
	return true;
}
void CMyBitmap::GameCameraDestroy()
{
	if ( s_pCamera )
	{
		SafeDeleteEx ( s_pCamera->lpFrom );
		SafeDeleteEx ( s_pCamera->lpTo );
		SafeDelete   ( s_pCamera );
		s_pCamera  = NULL;
	}
	if ( s_pProjectCamera )
	{
		SafeDeleteEx ( s_pProjectCamera->lpFrom );
		SafeDeleteEx ( s_pProjectCamera->lpTo );
		SafeDelete   ( s_pProjectCamera );
		s_pProjectCamera  = NULL;
	}
}
void CMyBitmap::GameCameraBuild(CMySize sizeScr)
{
	if(s_bProjectCamera)
	{
		if(s_pProjectCamera)
		{
			Camera_BuildView ( s_pProjectCamera, true );
			Camera_BuildProject ( s_pProjectCamera, true );
		}
		return;
	}
	if(s_pCamera)
	{
		s_sizeCamera = sizeScr;
		Camera_BuildView ( s_pCamera, true );
		Camera_BuildOrtho ( s_pCamera, sizeScr.iWidth, sizeScr.iHeight, true );
	}
}
void CMyBitmap::GameCameraBuild()
{
	if(s_bProjectCamera)
	{
		if(s_pProjectCamera)
		{
			Camera_BuildView ( s_pProjectCamera, true );
			Camera_BuildProject ( s_pProjectCamera, true );
		}
		return;
	}
	if(s_pCamera)
	{
		Camera_BuildView ( s_pCamera, true );
		Camera_BuildOrtho ( s_pCamera, s_sizeCamera.iWidth, s_sizeCamera.iHeight, true );
	}
}
void CMyBitmap::GameCameraGetPos(float& fX, float& fY, float& fZ, DWORD dwMask)
{
	C3Camera* pCamera = s_bProjectCamera ? s_pProjectCamera : s_pCamera;
	if (dwMask & 2)
	{
		fX = pCamera->lpTo[0].x;
		fY = pCamera->lpTo[0].y;
		fZ = pCamera->lpTo[0].z;
	}
	else
	{
		fX = pCamera->lpFrom[0].x;
		fY = pCamera->lpFrom[0].y;
		fZ = pCamera->lpFrom[0].z;
	}
}
void CMyBitmap::GameCameraSet(float fX, float fZ, float fY, DWORD dwMask)
{
	if(s_bProjectCamera)
	{
		if(!s_pProjectCamera)
			return;
		if (dwMask & 1)
		{
			s_pProjectCamera->lpFrom[0].x = fX;
			s_pProjectCamera->lpFrom[0].y = fY;
			s_pProjectCamera->lpFrom[0].z = fZ;
		}
		if (dwMask & 2)
		{
			s_pProjectCamera->lpTo[0].x = fX;
			s_pProjectCamera->lpTo[0].y = fY;
			s_pProjectCamera->lpTo[0].z = fZ;
		}
	}
	else
	{
		if(!s_pCamera)
			return;
		if (dwMask & 1)
		{
			s_pCamera->lpFrom[0].x = fX;
			s_pCamera->lpFrom[0].y = fY;
			s_pCamera->lpFrom[0].z = fZ;
		}
		if (dwMask & 2)
		{
			s_pCamera->lpTo[0].x = fX;
			s_pCamera->lpTo[0].y = 0;
			s_pCamera->lpTo[0].z = fZ;
		}
	}
	CMyBitmap::GameCameraBuild();
}
void CMyBitmap::GameCameraMove(BOOL bForward)
{
	if(!s_bProjectCamera || !s_pProjectCamera)
		return;
	D3DXVECTOR3 vDir = s_pProjectCamera->lpTo[0] - s_pProjectCamera->lpFrom[0];
	D3DXVec3Normalize ( &vDir, &vDir );
	vDir.x *= 40.0f;
	vDir.y *= 40.0f;
	vDir.z = 0;
	if (bForward)
	{
		s_pProjectCamera->lpFrom[0] += vDir;
		s_pProjectCamera->lpTo[0] += vDir;
	}
	else
	{
		s_pProjectCamera->lpFrom[0] -= vDir;
		s_pProjectCamera->lpTo[0] -= vDir;
	}
	Camera_BuildView ( s_pProjectCamera, true );
}
void CMyBitmap::GameCameraRotate(int nAngle)
{
	if(!s_bProjectCamera || !s_pProjectCamera)
		return;
	D3DXVECTOR3 vDir = s_pProjectCamera->lpFrom[0] - s_pProjectCamera->lpTo[0];
	D3DXMATRIX matrix;
	D3DXMatrixIdentity ( &matrix );
	D3DXMatrixRotationZ ( &matrix, D3DXToRadian ( nAngle ) );
	D3DXVECTOR4 vOut;
	D3DXVec3Transform ( &vOut, &vDir, &matrix );
	vDir.x = vOut.x;
	vDir.y = vOut.y;
	vDir.z = vOut.z;
	s_pProjectCamera->lpFrom[0] = s_pProjectCamera->lpTo[0] + vDir;
	Camera_BuildView ( s_pProjectCamera, true );
}
void CMyBitmap::GameCameraZoom(BOOL bIn)
{
	if(!s_bProjectCamera)
		return;
	D3DXVECTOR3 vDir = s_pProjectCamera->lpTo[0] - s_pProjectCamera->lpFrom[0];
	float fLength = sqrtf ( vDir.x * vDir.x + vDir.y * vDir.y + vDir.z * vDir.z );
	if (bIn)
		fLength -= 50.0f;
	else
		fLength += 50.0f;
	if (fLength < 300.0f)
		fLength = 300.0f;
	D3DXVec3Normalize ( &vDir, &vDir );
	vDir.x *= fLength;
	vDir.y *= fLength;
	vDir.z *= fLength;
	s_pProjectCamera->lpFrom[0] = s_pProjectCamera->lpTo[0] - vDir;
	CMyBitmap::GameCameraBuild();
}
void CMyBitmap::GameCameraSetFov(float fFov)
{
	if(!s_bProjectCamera)
		return;
	s_pProjectCamera->fFov = D3DX_PI / fFov;
	CMyBitmap::GameCameraBuild();
}
void CMyBitmap::GameCameraSetType(BOOL bProject)
{
	s_bProjectCamera = bProject;
	CMyBitmap::GameCameraBuild();
	D3DXVECTOR3 vLight = s_vLight;
	if (bProject)
	{
		D3DXVec3Normalize ( &vLight, &vLight );
	}
	else
	{
		D3DXMATRIX matrix;
		D3DXMatrixRotationX ( &matrix, D3DXToRadian ( s_nObliqueAngle ) );
		D3DXVec3Normalize ( &vLight, &vLight );
		D3DXVec3TransformNormal ( &vLight, &vLight, &matrix );
	}
	::Phy_SetLight ( s_bLight, vLight.x, vLight.y, vLight.z, s_dwLightDiffuse, s_dwLightAmbient, 0 );
}
BOOL CMyBitmap::GameCameraIsProject()
{
	return s_bProjectCamera;
}
BOOL CMyBitmap::GameFontCreate()
{
	char szFile[] = "ini/font.ini";
	FILE* fpFont = fopen(szFile, "r");
	if (!fpFont)
	{
		::ErrorMsg("%s open error.", szFile);
		return false;
	}
	char szFontName[256 + 1] = "";
	int nRead = fread(szFontName, 1, 256, fpFont);
	szFontName[nRead] = 0;
	fclose(fpFont);
	char* szFound = strrchr(szFontName, ' ');
	IF_NOT(szFound)
	{
		::LogMsg("Wrong Font.ini settings!");
		return false;
	}
	*szFound = 0;
	int nFontSize = atoi(szFound + 1);
	if (nFontSize == 0)
	{
		nFontSize = 12;
		::LogMsg("Wrong Font.ini settings!");
	}
	CMyFont* pMyFont = ::CreateFont(szFontName, nFontSize, 0);
	if (!pMyFont)
	{
		::LogMsg("Can not find font: %s %d", szFontName, nFontSize);
		LOGFONT lf;
		memset(&lf, 0, sizeof(lf));
		::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		pMyFont = ::CreateFont(lf.lfFaceName, nFontSize, 0);
		IF_NOT(pMyFont)
		{
			::LogMsg("Fail to create DEFAULT_GUI_FONT font %s", lf.lfFaceName);
			char szFontFile[MAX_PATH];
			::GetWindowsDirectory(szFontFile, MAX_PATH);
			strcat(szFontFile, "\\Fonts\\simsun.ttc");
			pMyFont = ::CreateFont(szFontFile, nFontSize, 0);
			IF_NOT(pMyFont)
			{
				::LogMsg("Fail to create simsun.ttc!!");
				::GetWindowsDirectory(szFontFile, MAX_PATH);
				strcat(szFontFile, "\\Fonts\\simsun.ttf");
				pMyFont = ::CreateFont(szFontFile, nFontSize, 0);
				IF_NOT(pMyFont)
				{
					::LogMsg("Fail to create simsun.ttf!!");
					return false;
				}
			}
		}
	}
	s_FontSet.push_back(pMyFont);
	FILE* fp = fopen(s_pszDbcsSmallFile, "rb");
	if (!fp)
		return true;
	int nAmount = _filelength(_fileno(fp)) / 2;
	WORD* pwChar = new WORD[nAmount];
	if (!pwChar)
	{
		fclose(fp);
		return true;
	}
	fread(pwChar, 2, nAmount, fp);
	for (int i = 0; i < nAmount; i++)
	{
		s_setDbcsSmall.insert(pwChar[i]);
		::DebugMsg("dbcs small char: %x", pwChar[i]);
	}
	delete[] pwChar;
	fclose(fp);
	return true;
}
void CMyBitmap::GameFontDestroy()
{
	int nAmount = s_FontSet.size();
	for (int i = 0; i < nAmount; i++)
	{
		CMyFont* pMyFont = s_FontSet[i];
		if (!pMyFont)
			continue;
		delete[] pMyFont->m_pData;
		Font_Release(&pMyFont->m_pFont);
		pMyFont->m_mapChar.clear();
		delete pMyFont;
	}
	s_FontSet.clear();
	s_setDbcsSmall.clear();
}
static CMyFont* CreateFont ( const char* pszFontName, int nSize, int bAntiAlias )
{
	CMyFont* pMyFont = new CMyFont;
	pMyFont->m_bDataFont = false;
	pMyFont->m_pFont = NULL;
	pMyFont->m_nSize = 0;
	pMyFont->m_pData = NULL;
	if (!pszFontName)
	{
		delete pMyFont;
		return NULL;
	}
	if (Font_Create(&pMyFont->m_pFont, pszFontName, nSize, bAntiAlias, 1))
	{
		pMyFont->LoadDbcsSpec();
		pMyFont->m_bDataFont = false;
		pMyFont->m_nSize = nSize;
		return pMyFont;
	}
	FILE* fp = fopen(pszFontName, "rb");
	if (!fp)
	{
		Font_Release(&pMyFont->m_pFont);
		delete pMyFont;
		return NULL;
	}
	struct { char szMagic[8]; DWORD dwFormat; DWORD dwCount; DWORD dwSize; } header;
	fread(&header, 0x14, 1, fp);
	int nData = header.dwSize * header.dwSize * header.dwCount / 2;
	pMyFont->m_pData = new BYTE[nData];
	if (!pMyFont->m_pData)
	{
		fclose(fp);
		Font_Release(&pMyFont->m_pFont);
		delete pMyFont;
		return NULL;
	}
	fread(pMyFont->m_pData, nData, 1, fp);
	for (DWORD i = 0; i < header.dwCount; i++)
	{
		DWORD dwCode = 0, dwOffset = 0;
		fread(&dwCode, 4, 1, fp);
		fread(&dwOffset, 4, 1, fp);
		pMyFont->m_mapChar[dwCode] = dwOffset - 0x14;
	}
	fclose(fp);
	pMyFont->m_bDataFont = true;
	pMyFont->m_nSize = nSize;
	if (!Font_Create(&pMyFont->m_pFont, pszFontName, header.dwSize, 0, 0))
	{
		delete[] pMyFont->m_pData;
		Font_Release(&pMyFont->m_pFont);
		delete pMyFont;
		return NULL;
	}
	return pMyFont;
}
int CMyFont::LoadDbcsSpec()
{
	FILE* fp = fopen("ini/DbcsSpec.txt", "rb");
	if (!fp)
		return -1;
	int nAmount = _filelength(_fileno(fp)) / 2;
	WORD* pwChar = new WORD[nAmount];
	if (!pwChar)
	{
		fclose(fp);
		return -1;
	}
	fread(pwChar, 2, nAmount, fp);
	int nCount = 0;
	for (int i = 0; i < nAmount; i++)
	{
		char szFile[256] = "";
		sprintf(szFile, "data/DbcsSpec/%x%x.dds", (char)LOBYTE(pwChar[i]), HIBYTE(pwChar[i]));
		if (Font_CreateDbcsSpec(&m_pFont, pwChar[i], szFile))
			nCount++;
	}
	delete[] pwChar;
	fclose(fp);
	return nCount;
}
BYTE* CMyFont::GetCharData(DWORD dwCode)
{
	if (!m_bDataFont)
		return NULL;
	map<DWORD, DWORD>::iterator it = m_mapChar.find(dwCode & 0xFFFF);
	if (it == m_mapChar.end())
		return NULL;
	return m_pData + it->second;
}
DWORD CMyBitmap::GetFontSize(void)
{
	MYASSERT(s_FontSet.size() > 0);
	return s_FontSet[0]->m_nSize;
}
void CMyBitmap::GetFontSize(CMySize& infoSize)
{
	DWORD dwSize = GetFontSize();
	infoSize.iHeight = dwSize;
	infoSize.iWidth  = dwSize/2;
}
int CMyBitmap::GetStrShowLen(const char* pszString, int nBegin, int nEnd)
{
	if (!pszString)
		return 0;
	int nWidth = 0;
	for (int i = nBegin; i < nEnd; i++)
	{
		BYTE c = pszString[i];
		if (c == 0)
			break;
		if (::IsDBCSLeadByte(c))
		{
			if (i + 1 >= nEnd)
				break;
			WORD wChar = MAKEWORD(c, pszString[i + 1]);
			if (s_setDbcsSmall.count(wChar))
				nWidth += 1;
			else
				nWidth += 2;
			i++;
		}
		else
			nWidth += 1;
	}
	return nWidth;
}
bool CMyBitmap::CheckStringPixels(const char* pszString, DWORD dwPixels, const char* pszFont, int nFontSize)
{
	if (!pszString)
		return false;
	if (!pszFont)
		return false;
	CMyFont *pMyFont = NULL;
	int nAmount = s_FontSet.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CMyFont* pFont = s_FontSet[i];
		if(pFont && strcmp(pFont->m_pFont->szName, pszFont) == 0 && pFont->m_nSize == nFontSize)
		{
			pMyFont = pFont;
			break;
		}
	}
	if(!pMyFont)
	{
		pMyFont = ::CreateFont(pszFont, nFontSize, 0);
		if (!pMyFont)
		{
			if (s_FontSet.size() == 0)
				return false;
			pMyFont = s_FontSet[0];
		}
		else
			s_FontSet.push_back(pMyFont);
	}
	int nStrLen = strlen(pszString);
	int n = 0;
	for ( n = 0; n < nStrLen; n++ )
	{
		if ( (BYTE)pszString[n] == 0 )
			break;
		if ( !Font_CheckPixel ( pMyFont->m_pFont, pszString + n, dwPixels ) )
			break;
		if ( ::IsDBCSLeadByte( pszString[n] ) )
			n++;
	}
	return n >= nStrLen;
}
void CMyBitmap::ShowString ( int iPosX, int iPosY, DWORD color, const char* pszString, const char* pszFont, int nFontSize, BOOL bAlpha, RENDER_TEXT_STYLE style, DWORD color2, CMyPos& pos )
{
	if (!pszString)
		return;
	if (s_nShowMode != modeText)
	{
		Font_Prepare();
		s_nShowMode	=modeText;
	}
	if(bAlpha == false)
	{
		color |= 0xff000000;
		color2 |= 0xff000000;
	}
	CMyFont *pMyFont = NULL;
	if (!pszFont)
	{
		pMyFont	= s_FontSet[0];
	}
	else
	{
		int nAmount = s_FontSet.size();
		for(int i = 0; i < nAmount; i ++)
		{
			CMyFont* pFont = s_FontSet[i];
			if(pFont && strcmp(pFont->m_pFont->szName, pszFont) == 0 && pFont->m_nSize == nFontSize && (pFont->m_pFont->bAntiAlias != 0) == (bAlpha == 1))
			{
				pMyFont = pFont;
				break;
			}
		}
		if(!pMyFont)
		{
			pMyFont = ::CreateFont(pszFont, nFontSize, bAlpha);
			if (!pMyFont)
			{
				if (s_FontSet.size() == 0)
					return;
				pMyFont = s_FontSet[0];
			}
			else
				s_FontSet.push_back(pMyFont);
		}
	}
	int step=iPosX;
	char ss[2];
	int nStrLen = strlen(pszString);
	Font_DrawBegin();
	for ( int n = 0; n < nStrLen; n++ )
	{
		if ( pszString[n] == 0x0a )
		{
			step = iPosX;
			iPosY += pMyFont->m_nSize + pMyFont->m_nSize / 4;
			continue;
		}
		ss[0] = pszString[n];
		ss[1] = pszString[n + 1];
		int nAdvance = 0;
		if (pMyFont->m_bDataFont)
		{
			DWORD dwCode = ::IsDBCSLeadByte(pszString[n]) ? (((BYTE)pszString[n + 1] << 8) + (BYTE)pszString[n]) : (BYTE)pszString[n];
			BYTE* pData = pMyFont->GetCharData(dwCode);
			if (pData)
			{
				Font_Draw ( pMyFont->m_pFont, (float)step, (float)iPosY, color, ss, pData );
				nAdvance = pMyFont->m_nSize;
			}
		}
		else
		{
			Font_Draw ( pMyFont->m_pFont, (float)step, (float)iPosY, color, ss, style, color2, pos.x, pos.y );
			nAdvance = pMyFont->m_nSize;
		}
		if ( ::IsDBCSLeadByte( pszString[n] ) )
		{
			if (s_setDbcsSmall.count(MAKEWORD(pszString[n], pszString[n + 1])))
				step += ( int )( ( ( float )nAdvance + 1.0f ) * 0.5f );
			else
				step += nAdvance;
			n++;
		}
		else
			step += ( int )( ( ( float )nAdvance + 1.0f ) * 0.5f );
	}
	Font_DrawEnd();
}
void CMyBitmap::ShowTString ( int iPosX, int iPosY, DWORD color, const char *pszString, const char* pszFont, int nFontSize, BOOL bAlpha, RENDER_TEXT_STYLE style, DWORD color2, CMyPos& pos )
{
	if (!pszString)
		return;
	::RepairString((char*)pszString);
	if (strlen(pszString) == 0)
		return;
	if (s_nShowMode != modeText)
	{
		Font_Prepare();
		s_nShowMode	=modeText;
	}
	CMyFont *pMyFont = NULL;
	if (!pszFont)
	{
		pMyFont	= s_FontSet[0];
	}
	else
	{
		int nAmount = s_FontSet.size();
		for(int i = 0; i < nAmount; i ++)
		{
			CMyFont* pFont = s_FontSet[i];
			if(pFont && strcmp(pFont->m_pFont->szName, pszFont) == 0 && pFont->m_nSize == nFontSize && (pFont->m_pFont->bAntiAlias != 0) == (bAlpha == 1))
			{
				pMyFont = pFont;
				break;
			}
		}
		if(!pMyFont)
		{
			pMyFont = ::CreateFont(pszFont, nFontSize, bAlpha);
			if (!pMyFont)
			{
				if (s_FontSet.size() == 0)
					return;
				pMyFont = s_FontSet[0];
			}
			else
				s_FontSet.push_back(pMyFont);
		}
	}
	if(bAlpha == false)
		color |= 0xff000000;
	int step=iPosY;
	char ss[2];
	Font_DrawBegin();
	for ( DWORD n = 0; n < strlen(pszString); n++ )
	{
		ss[0] = pszString[n];
		ss[1] = pszString[n + 1];
		Font_Draw ( pMyFont->m_pFont, (float)iPosX, (float)step, color, ss, style, color2, pos.x, pos.y );
		step += pMyFont->m_pFont->nRealSize;
		if ( ( BYTE )pszString[n] > 0x80 && ( BYTE )ss[0] < 0xfe )
			n++;
	}
	Font_DrawEnd();
}
class IDataIcon
{
public:
	virtual IAni*	GetIcon ( int nIndex ) = 0;
};
void CMyBitmap::ShowEmotionString (int iPosX, int iPosY, DWORD color, const char* pszString, IDataIcon* pDataIcon, const char* pszFont, int nFontSize, int nImgSize, int nOffsetX, int nOffsetY, RENDER_TEXT_STYLE style, DWORD color2, CMyPos& pos )
{
	if (!pszString)
		return;
	if ( s_nShowMode != modeText )
	{
		Font_Prepare();
		s_nShowMode	=modeText;
	}
	color |= 0xff000000;
	CMyFont *pMyFont = NULL;
	if (!pszFont)
	{
		pMyFont	= s_FontSet[0];
	}
	else
	{
		int nAmount = s_FontSet.size();
		for(int i = 0; i < nAmount; i ++)
		{
			CMyFont* pFont = s_FontSet[i];
			if(pFont && strcmp(pFont->m_pFont->szName, pszFont) == 0 && pFont->m_nSize == nFontSize && pFont->m_pFont->bAntiAlias == 0)
			{
				pMyFont = pFont;
				break;
			}
		}
		if(!pMyFont)
		{
			pMyFont = ::CreateFont(pszFont, nFontSize, 0);
			if (!pMyFont)
			{
				if (s_FontSet.size() == 0)
					return;
				pMyFont = s_FontSet[0];
			}
			else
				s_FontSet.push_back(pMyFont);
		}
	}
	int step = iPosX;
	char ss[2];
	int nStrLen = strlen(pszString);
	Font_DrawBegin();
	for ( int n = 0; n < nStrLen; n++ )
	{
		if ( pszString[n] == 0x0a )
		{
			step = iPosX;
			iPosY += pMyFont->m_nSize + pMyFont->m_nSize / 4;
			continue;
		}
		if ( pszString[n] == '#' &&
			n + 2 < nStrLen &&
			pszString[n + 1] >= '0' &&
			pszString[n + 1] <= '9' &&
			pszString[n + 2] >= '0' &&
			pszString[n + 2] <= '9')
		{
			if (pDataIcon)
			{
				char strIndex[16] = "";
				strncpy( strIndex, pszString + n + 1, 2 );
				int nIndex = atoi( strIndex );
				IAni* pShowAni = pDataIcon->GetIcon(nIndex);
				if ( pShowAni != NULL)
				{
					int nFrameAmount = pShowAni->GetFrameAmount();
					DWORD dwFrame = ::timeGetTime() / 200 % nFrameAmount;
					pShowAni->ShowEx( dwFrame, step + nOffsetX, iPosY + nOffsetY - 4, NULL, nImgSize, nImgSize, 0, _SHOWWAY_NORMAL );
					if ( s_nShowMode != modeText )
					{
						Font_Prepare();
						s_nShowMode	=modeText;
					}
				}
			}
			n += 2;
			if (nImgSize != 0)
				step += nImgSize;
			else
				step += pMyFont->m_nSize - ( int )( ( ( float )pMyFont->m_nSize + 1.0f ) * -0.5f );
			continue;
		}
		ss[0] = pszString[n];
		ss[1] = pszString[n + 1];
		int nAdvance = 0;
		if (pMyFont->m_bDataFont)
		{
			DWORD dwCode = ::IsDBCSLeadByte(pszString[n]) ? (((BYTE)pszString[n + 1] << 8) + (BYTE)pszString[n]) : (BYTE)pszString[n];
			BYTE* pData = pMyFont->GetCharData(dwCode);
			if (pData)
			{
				Font_Draw ( pMyFont->m_pFont, (float)step, (float)iPosY, color, ss, pData );
				nAdvance = pMyFont->m_nSize;
			}
		}
		else
		{
			Font_Draw ( pMyFont->m_pFont, (float)step, (float)iPosY, color, ss, style, color2, pos.x, pos.y );
			nAdvance = pMyFont->m_nSize;
		}
		if ( ::IsDBCSLeadByte( pszString[n] ) )
		{
			if (s_setDbcsSmall.count(MAKEWORD(pszString[n], pszString[n + 1])))
				step += ( int )( ( ( float )nAdvance + 1.0f ) * 0.5f );
			else
				step += nAdvance;
			n ++;
		}
		else
			step += ( int )( ( ( float )nAdvance + 1.0f ) * 0.5f );
	}
	Font_DrawEnd();
}
void CMyBitmap::SetTextCornerColor ( DWORD dwTopLeft, DWORD dwBottomLeft, DWORD dwTopRight, DWORD dwBottomRight )
{
	Font_SetTextCornerColor ( dwTopLeft, dwBottomLeft, dwTopRight, dwBottomRight );
}
void CMyBitmap::PrepareLine ( void )
{
	if (s_nShowMode == modeLine)
		return;
	SetRenderState ( D3DRS_ZENABLE, false );
	SetRenderState ( D3DRS_ZWRITEENABLE, false );
	SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_FLAT );
	SetRenderState ( D3DRS_DITHERENABLE, false );
	SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
	SetRenderState ( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1 );
	SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
	SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
	SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
	SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
	g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	g_D3DDevice->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_NONE );
	g_D3DDevice->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
	g_D3DDevice->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

	s_nShowMode	=modeLine;
}
void CMyBitmap::Prepare3DLine ( void )
{
	if (s_nShowMode == mode3DLine)
		return;
	Phy_Prepare ();
	SetRenderState ( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
	s_nShowMode	=mode3DLine;
}
void CMyBitmap::ShowLine ( LineVertex *line, DWORD num )
{
	if (s_nShowMode != modeLine)
		CMyBitmap::PrepareLine();
	g_D3DDevice->SetFVF ( LINE_VERTEX );
	g_D3DDevice->DrawPrimitiveUP ( D3DPT_LINELIST,
							 num,
							 line,
							 sizeof ( LineVertex ) );
}
void CMyBitmap::ShowLine ( int nPosX1, int nPosY1, int nPosX2, int nPosY2, DWORD dwColor)
{
	static 	LineVertex s_Line[2];
	s_Line[0].x = nPosX1;
	s_Line[0].y = nPosY1;
	s_Line[0].z = 0;
	s_Line[0].rhw = 1.0f;
	s_Line[0].color = dwColor;
 	s_Line[1].x = nPosX2;
	s_Line[1].y = nPosY2;
	s_Line[1].z = 0;
	s_Line[1].rhw = 1.0f;
	s_Line[1].color = dwColor;
	ShowLine  ( s_Line, 1 );
}
void CMyBitmap::ShowRect ( int x1, int y1, int x2, int y2, DWORD color )
{
	static LineVertex s_line[8];
	s_line[0].x = x1;
	s_line[0].y = y1;
	s_line[0].z = 0;
	s_line[0].rhw = 1.0f;
	s_line[0].color = color;
	s_line[1].x = x1;
	s_line[1].y = y2;
	s_line[1].z = 0;
	s_line[1].rhw = 1.0f;
	s_line[1].color = color;
	s_line[2].x = x1;
	s_line[2].y = y2;
	s_line[2].z = 0;
	s_line[2].rhw = 1.0f;
	s_line[2].color = color;
	s_line[3].x = x2;
	s_line[3].y = y2;
	s_line[3].z = 0;
	s_line[3].rhw = 1.0f;
	s_line[3].color = color;
	s_line[4].x = x2;
	s_line[4].y = y2;
	s_line[4].z = 0;
	s_line[4].rhw = 1.0f;
	s_line[4].color = color;
	s_line[5].x = x2;
	s_line[5].y = y1;
	s_line[5].z = 0;
	s_line[5].rhw = 1.0f;
	s_line[5].color = color;
	s_line[6].x = x2;
	s_line[6].y = y1;
	s_line[6].z = 0;
	s_line[6].rhw = 1.0f;
	s_line[6].color = color;
	s_line[7].x = x1;
	s_line[7].y = y1;
	s_line[7].z = 0;
	s_line[7].rhw = 1.0f;
	s_line[7].color = color;
	ShowLine  ( s_line, 4 );
}
void CMyBitmap::ShowBlock( int x1, int y1, int x2, int y2, DWORD color1, DWORD color2, DWORD color3, DWORD color4)
{
	static LineVertex s_vec[4];
	s_vec[0].x = x1;
	s_vec[0].y = y1;
	s_vec[0].z = 0;
	s_vec[0].rhw = 1;
	s_vec[0].color = color1;
	s_vec[1].x = x1;
	s_vec[1].y = y2;
	s_vec[1].z = 0;
	s_vec[1].rhw = 1;
	s_vec[1].color = color2;
	s_vec[2].x = x2;
	s_vec[2].y = y1;
	s_vec[2].z = 0;
	s_vec[2].rhw = 1;
	s_vec[2].color = color3;
	s_vec[3].x = x2;
	s_vec[3].y = y2;
	s_vec[3].z = 0;
	s_vec[3].rhw = 1;
	s_vec[3].color = color4;
	if (s_nShowMode != modeLine)
		CMyBitmap::PrepareLine();
	g_D3DDevice->SetFVF ( LINE_VERTEX );
	g_D3DDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP,
							 2,
							 s_vec,
							 sizeof ( LineVertex ) );
}
void CMyBitmap::ShowBlock( CMyPos posA, CMyPos posB, CMyPos posC, CMyPos posD, DWORD color )
{
	static LineVertex s_vec[4];
	s_vec[0].x = posA.x;
	s_vec[0].y = posA.y;
	s_vec[0].z = 0;
	s_vec[0].rhw = 1.0f;
	s_vec[0].color = color;
	s_vec[1].x = posB.x;
	s_vec[1].y = posB.y;
	s_vec[1].z = 0;
	s_vec[1].rhw = 1.0f;
	s_vec[1].color = color;
	s_vec[2].x = posC.x;
	s_vec[2].y = posC.y;
	s_vec[2].z = 0;
	s_vec[2].rhw = 1.0f;
	s_vec[2].color = color;
	s_vec[3].x = posD.x;
	s_vec[3].y = posD.y;
	s_vec[3].z = 0;
	s_vec[3].rhw = 1.0f;
	s_vec[3].color = color;
	if (s_nShowMode != modeLine)
		CMyBitmap::PrepareLine();
	g_D3DDevice->SetFVF ( LINE_VERTEX );
	g_D3DDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP,
							 2,
							 s_vec,
							 sizeof ( LineVertex ) );
}
void CMyBitmap::ShowBlock ( int x1, int y1, int x2, int y2, DWORD color )
{
	static LineVertex s_vec[4];
	s_vec[0].x = x1;
	s_vec[0].y = y1;
	s_vec[0].z = 0;
	s_vec[0].rhw = 1.0f;
	s_vec[0].color = color;
	s_vec[1].x = x1;
	s_vec[1].y = y2;
	s_vec[1].z = 0;
	s_vec[1].rhw = 1.0f;
	s_vec[1].color = color;
	s_vec[2].x = x2;
	s_vec[2].y = y1;
	s_vec[2].z = 0;
	s_vec[2].rhw = 1.0f;
	s_vec[2].color = color;
	s_vec[3].x = x2;
	s_vec[3].y = y2;
	s_vec[3].z = 0;
	s_vec[3].rhw = 1.0f;
	s_vec[3].color = color;
	if (s_nShowMode != modeLine)
		CMyBitmap::PrepareLine();
	g_D3DDevice->SetFVF ( LINE_VERTEX );
	g_D3DDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP,
							 2,
							 s_vec,
							 sizeof ( LineVertex ) );
}
void CMyBitmap::Show3DBox( D3DXVECTOR3& vMin, D3DXVECTOR3& vMax, DWORD color )
{
	CMyBitmap::Prepare3DLine();

	PhyOutVertex vertex[8];
	ZeroMemory ( vertex, sizeof ( vertex ) );
	for ( int n = 0; n < 8; n++ )
	{
		vertex[n].x = ( n & 4 ) ? vMin.x : vMax.x;
		vertex[n].y = ( n & 2 ) ? vMin.y : vMax.y;
		vertex[n].z = ( n & 1 ) ? vMin.z : vMax.z;
		vertex[n].color = color;
	}
	vertex[0].u = 1.0f;	vertex[0].v = 1.0f;
	vertex[1].u = 1.0f;	vertex[1].v = 1.0f;
	vertex[6].u = 1.0f;	vertex[6].v = 1.0f;
	vertex[7].u = 1.0f;	vertex[7].v = 1.0f;

	WORD index[36] = {	0, 1, 2,	1, 3, 2,
						0, 4, 1,	4, 5, 1,
						7, 5, 4,	7, 4, 6,
						3, 7, 6,	3, 6, 2,
						5, 7, 3,	5, 3, 1,
						6, 4, 2,	4, 0, 2 };

	D3DMATERIAL9 material;
	ZeroMemory ( &material, sizeof ( material ) );
	g_D3DDevice->SetMaterial ( &material );
	g_D3DDevice->SetTexture ( 0, 0 );

	D3DXMATRIX matrix;
	D3DXMatrixIdentity ( &matrix );
	g_D3DDevice->SetTransform ( D3DTS_WORLD, &matrix );
	g_D3DDevice->SetFVF ( PHY_OUT_VERTEX );
	g_D3DDevice->DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST,
										  0,
										  8,
										  12,
										  index,
										  D3DFMT_INDEX16,
										  vertex,
										  sizeof ( PhyOutVertex ) );
}
static DWORD	s_dwCryptTable[0x500];
static BYTE		s_bCryptTable = 0;

static void PrepareCryptTable ( void )
{
	if ( s_bCryptTable )
		return;
	DWORD dwSeed = 0x100001;
	for ( DWORD dwIndex1 = 0; dwIndex1 < 0x100; dwIndex1++ )
	{
		DWORD dwIndex2 = dwIndex1;
		for ( int i = 0; i < 5; i++, dwIndex2 += 0x100 )
		{
			dwSeed = ( dwSeed * 125 + 3 ) % 0x2AAAAB;
			DWORD dwTemp1 = ( dwSeed & 0xFFFF ) << 16;
			dwSeed = ( dwSeed * 125 + 3 ) % 0x2AAAAB;
			DWORD dwTemp2 = dwSeed & 0xFFFF;
			s_dwCryptTable[dwIndex2] = dwTemp1 | dwTemp2;
		}
	}
	s_bCryptTable = 1;
}

static DWORD HashString ( const char *lpString, DWORD dwOffset )
{
	DWORD dwSeed1 = 0x7FED7FED;
	DWORD dwSeed2 = 0xEEEEEEEE;
	for ( const BYTE *p = ( const BYTE* )lpString; *p; p++ )
	{
		DWORD ch = toupper ( *p );
		dwSeed1 = s_dwCryptTable[ch + dwOffset] ^ ( dwSeed1 + dwSeed2 );
		dwSeed2 = ch + dwSeed1 + dwSeed2 + ( dwSeed2 << 5 ) + 3;
	}
	return dwSeed1;
}

static DWORD HashStringMod ( const char *lpString, DWORD dwSize )
{
	return HashString ( lpString, 0 ) & ( dwSize - 1 );
}
BOOL CMyBitmap::Init3D ( HINSTANCE hInst,
						 const char *lpTitle,
						 DWORD dwWidth,
						 DWORD dwHeight,
						 BOOL bWindowed,
						 WNDPROC proc,
						 DWORD dwBackCount )
{
	if ( !::Init3D ( hInst, lpTitle, dwWidth, dwHeight, bWindowed, proc, dwBackCount ) )
		return false;
	CMyBitmap::GameCameraCreate();
	_SCR_WIDTH = dwWidth;
	_SCR_HEIGHT = dwHeight;
	CMySize sizeScr = {_SCR_WIDTH, _SCR_HEIGHT};
	CMyBitmap::GameCameraBuild(sizeScr);
	return true;
}
BOOL CMyBitmap::Init3DEx ( HWND hWnd,
						  DWORD dwWidth,
						  DWORD dwHeight,
						  BOOL bWindowed,
						  DWORD dwBackCount )
{
	int rval = ::Init3DEx ( hWnd, dwWidth, dwHeight, bWindowed, dwBackCount );
	switch(rval)
	{
	case 0:
		{
			::ErrorMsg("dx error. please install dx8.1a");
			return false;
		}
	case -1:
		{
			::ErrorMsg("dx error. please check your video card driver.");
			return false;
		}
	case -2:
		{
			::ErrorMsg("dx error. please switch to 16bit color mode.");
			return false;
		}
	case -3:
		{
			::ErrorMsg("alpha blend not support.");
			break;
		}
	case 1:
		{
			break;
		}
	default:
		{
			::ErrorMsg("unknow dx error");
			return false;
		}
	}
	FILE* fp = fopen("ini/graphic.ini", "r");
	if (fp)
	{
		if (fscanf(fp, "%u %f %f %f %x %x %d\n", &s_bLight, &s_vLight.x, &s_vLight.y, &s_vLight.z, &s_dwLightDiffuse, &s_dwLightAmbient, &s_nObliqueAngle) == 7)
		{
			D3DXVECTOR3 vLight = s_vLight;
			D3DXMATRIX matrix;
			D3DXMatrixRotationX ( &matrix, D3DXToRadian ( s_nObliqueAngle ) );
			D3DXVec3Normalize ( &vLight, &vLight );
			D3DXVec3TransformNormal ( &vLight, &vLight, &matrix );
			::Phy_SetLight ( s_bLight, vLight.x, vLight.y, vLight.z, s_dwLightDiffuse, s_dwLightAmbient, 0 );
		}
		fclose(fp);
	}
	CMyBitmap::GameCameraCreate();
	_SCR_WIDTH = dwWidth;
	_SCR_HEIGHT = dwHeight;
	CMySize sizeScr = {_SCR_WIDTH, _SCR_HEIGHT};
	CMyBitmap::GameCameraBuild(sizeScr);
	PrepareCryptTable();
	return true;
}
void CMyBitmap::Quit3D (void)
{
	AniFileSetDestroy();
	CMyBitmap::GameCameraDestroy();
	CMyBitmap::GameFontDestroy();
	::Quit3D();
}
BOOL CMyBitmap::IfDeviceLost ( void )
{
	return ::IfDeviceLost ();
}
BOOL CMyBitmap::ResetDevice ( void )
{
	return ::ResetDevice ();
}
IDirect3DDevice8* CMyBitmap::QueryDevice ( void )
{
	return ( IDirect3DDevice8* )g_D3DDevice;
}
BOOL CMyBitmap::Begin3D ( void )
{
	return ::Begin3D ();
}
BOOL CMyBitmap::End3D ( void )
{
	g_dwTriTotal = 0;
	return ::End3D ();
}
BOOL CMyBitmap::ClearBuffer ( BOOL bZBuffer, BOOL bTarget, D3DCOLOR color )
{
	return ::ClearBuffer ( bZBuffer, bTarget, color );
}
BOOL CMyBitmap::Flip ( const RECT *lpSrcRect, const RECT *lpDestRect, HWND hWnd )
{
	return ::Flip ( lpSrcRect, lpDestRect, hWnd );
}
DWORD CMyBitmap::GetShowTriTotal ( void )
{
	return g_dwTriTotal;
}
DWORD CMyBitmap::CalcRate ( void )
{
	return ::CalcRate ();
}
void CMyBitmap::PrepareSprite ( void )
{
	if (s_nShowMode == modeSprite)
		return;
	Sprite_Prepare ();
	s_nShowMode	=modeSprite;
}
void CMyBitmap::PrepareMixSprite ( void )
{
	if (s_nShowMode == modeMixSprite)
		return;
	Sprite_Prepare ();
	s_nShowMode	=modeMixSprite;
}
void CMyBitmap::PrepareModle ( void )
{
	if (s_nShowMode == modeMesh)
		return;
	Phy_Prepare ();
	s_nShowMode	=modeMesh;
}
void CMyBitmap::SetLight ( BOOL bEnable, float x, float y, float z, DWORD dwDiffuse, DWORD dwAmbient, float fReserved )
{
	::Phy_SetLight ( bEnable, x, y, z, dwDiffuse, dwAmbient, fReserved );
}
void CMyBitmap::ScreenShot(const char* pszFileName)
{
	char szFileName[256] = "";
	if (!pszFileName)
	{
		sprintf(szFileName, "ScreenShot/%u.jpg", ::timeGetTime());
	}
	else
		strcpy(szFileName, pszFileName);
	::CapScreen(szFileName);
}
void CMyBitmap::PackFileOpen ( const char* pszFileName )
{
	::DataFile_Open ( pszFileName );
}
void CMyBitmap::PackFileClose ( void )
{
	::DataFile_Close ();
}
void CMyBitmap::Common_Translate ( D3DXMATRIX *matrix, float x, float y, float z )
{
	::Common_Translate ( matrix, x, y, z );
}
void CMyBitmap::Common_Rotate ( D3DXMATRIX *matrix, float x, float y, float z )
{
	::Common_Rotate ( matrix, x, y, z );
}
void CMyBitmap::Common_Scale ( D3DXMATRIX *matrix, float x, float y, float z )
{
	::Common_Scale ( matrix, x, y, z );
}
void CMyBitmap::Motion_GetMatrix ( C3Motion *lpMotion, DWORD dwBone, D3DXMATRIXA16 *lpMatrix )
{
	::Motion_GetMatrix ( lpMotion, dwBone, lpMatrix );
}
void CMyBitmap::Motion_GetMatrix_Blend ( C3MotionTracker tracker, DWORD dwBone, D3DXMATRIXA16 *lpMatrix )
{
	::Motion_GetMatrix_Blend ( tracker, dwBone, lpMatrix );
}
void CMyBitmap::ScreenToWorld(int nScrX, int nScrY, float& fX, float& fY, float& fZ)
{
	if (!s_bProjectCamera)
		return;
	D3DXVECTOR3 vScr ( nScrX, nScrY, 0.0f );
	D3DXVECTOR3 vWorld;
	D3DXVec3Unproject ( &vWorld, &vScr, &g_Viewport, &g_ProjectMatrix, &g_ViewMatrix, NULL );
	fX = vWorld.x;
	fY = vWorld.y;
	fZ = vWorld.z;
}
void CMyBitmap::WorldToScreen(float fX, float fY, float fZ, int& nScrX, int& nScrY)
{
	D3DXVECTOR3 vWorld ( fX, fY, fZ );
	D3DXVECTOR3 vScr;
	D3DXMATRIX matrix;
	D3DXMatrixIdentity ( &matrix );
	D3DXVec3Project ( &vScr, &vWorld, &g_Viewport, &g_ProjectMatrix, &g_ViewMatrix, &matrix );
	nScrX = ( int )( vScr.x + 0.5f );
	nScrY = ( int )( vScr.y + 0.5f );
}
void CMyBitmap::ScrRayBuild(int nScrX, int nScrY, D3DXVECTOR3& vFrom, D3DXVECTOR3& vDir)
{
	D3DXVECTOR3 vCameraFrom, vCameraTo;
	CMyBitmap::GameCameraGetPos ( vCameraFrom.x, vCameraFrom.y, vCameraFrom.z, 1 );
	CMyBitmap::GameCameraGetPos ( vCameraTo.x, vCameraTo.y, vCameraTo.z, 2 );
	if (s_bProjectCamera)
	{
		CMyBitmap::ScreenToWorld ( nScrX, nScrY, vCameraTo.x, vCameraTo.y, vCameraTo.z );
	}
	else
	{
		vCameraTo.x += nScrX - _SCR_WIDTH/2;
		vCameraTo.z += nScrY - _SCR_HEIGHT/2;
	}
	D3DXVECTOR3 vRay = vCameraTo - vCameraFrom;
	D3DXVec3Normalize ( &vRay, &vRay );
	vFrom = vCameraFrom;
	vDir = vRay;
}
void CMyBitmap::GetScreenSize(int& nWidth, int& nHeight)
{
	nWidth=_SCR_WIDTH, nHeight=_SCR_HEIGHT;
}
int CMyBitmap::GetObliqueAngle()
{
	return s_nObliqueAngle;
}
int CMyBitmap::GetScreenWidth()
{
	return _SCR_WIDTH;
}
int CMyBitmap::GetScreenHeight()
{
	return _SCR_HEIGHT;
}
BOOL CMyBitmap::HintGraphicDetail ( DWORD dwDetail )
{
	return ::HintGraphicDetail ( dwDetail );
}
void CMyBitmap::SetReflectFluidSpeed ( float fSpeedU, float fSpeedV )
{
	::Phy_SetFluidReflectSpeed ( fSpeedU, fSpeedV );
}
bool CMyBitmap::IsDBCSLeadByte	(const char* buf, int nOffset)
{
	if (!buf || nOffset < 0)
		return false;
	int nLen = strlen(buf);
	if (nOffset >= nLen)
		return false;
	BYTE btChar = buf[nOffset];
	if (!::IsDBCSLeadByte(btChar))
		return false;
	int idxLastDbcs = 0;
	for (int i=0; i<nOffset; i++)
	{
		if (::IsDBCSLeadByte(buf[i]))
		{
			idxLastDbcs = i;
			i++;
		}
	}
	if (idxLastDbcs < nOffset - 1 || nOffset == 0)
		return ::IsDBCSLeadByte(btChar) ? true : false;
	else
		return false;
}

#define	_BMP_TEX(pBmp)	( ( ( CMyBitmapX* )( pBmp ) )->m_sprite->lpTex->lpTex )

const DWORD _MULTIBMP_MAX	= 4096;
class CMultiBmpX : public IMultiBmp
{
public:
	CMultiBmpX();
	virtual ~CMultiBmpX();
	static void* operator new(size_t uSize) throw()	{ return s_heap.Alloc(uSize); }
	static void	 operator delete(void* p)	{ s_heap.Free(p); }
	static CMyHeap	s_heap;

	virtual DWORD	Release	( void );
	virtual void	Add	( int x, int y, int nWidth, int nHeight, float u0, float v0, float u1, float v1, float u2, float v2, float u3, float v3 );
	virtual void	Clear	( void )	{ m_dwCount = 0; }
	virtual void	Show	( CMyBitmap* pBmp );
public:
	DWORD			m_dwCount;
	SpriteVertex*	m_pVertex;
};

CMyHeap CMultiBmpX::s_heap("CMultiBmpX");

CMultiBmpX::CMultiBmpX ()
{
	m_dwCount	= 0;
	m_pVertex	= new SpriteVertex[_MULTIBMP_MAX * 6];
}

CMultiBmpX::~CMultiBmpX ()
{
	if ( m_pVertex )
	{
		delete [] m_pVertex;
		m_pVertex = NULL;
	}
}
DWORD CMultiBmpX::Release ( void )
{
	delete this;
	return 0;
}
void CMultiBmpX::Add ( int x, int y, int nWidth, int nHeight, float u0, float v0, float u1, float v1, float u2, float v2, float u3, float v3 )
{
	if ( m_dwCount >= _MULTIBMP_MAX )
		return;
	SpriteVertex* p = m_pVertex + m_dwCount * 6;
	m_dwCount++;

	float fLeft = ( float )x, fTop = ( float )y;
	float fRight = ( float )( x + nWidth ), fBottom = ( float )( y + nHeight );
	p[0].x = fLeft;		p[0].y = fTop;		p[0].u = u0;	p[0].v = v0;
	p[1].x = fRight;	p[1].y = fTop;		p[1].u = u1;	p[1].v = v1;
	p[2].x = fLeft;		p[2].y = fBottom;	p[2].u = u2;	p[2].v = v2;
	p[3].x = fRight;	p[3].y = fTop;		p[3].u = u1;	p[3].v = v1;
	p[4].x = fRight;	p[4].y = fBottom;	p[4].u = u3;	p[4].v = v3;
	p[5].x = fLeft;		p[5].y = fBottom;	p[5].u = u2;	p[5].v = v2;
	for ( int i = 0; i < 6; i++ )
	{
		p[i].z		= 0;
		p[i].rhw	= 1.0f;
		p[i].color	= 0xffffffff;
	}
}
void CMultiBmpX::Show ( CMyBitmap* pBmp )
{
	if ( !pBmp || m_dwCount == 0 )
		return;
	CMyBitmap::PrepareSprite ();
	SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
	SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	g_D3DDevice->SetTexture ( 0, _BMP_TEX ( pBmp ) );
	g_D3DDevice->SetFVF ( SPRITE_VERTEX );
	g_D3DDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, m_dwCount * 2, m_pVertex, sizeof ( SpriteVertex ) );
}
IMultiBmp* MultiBmpCreate ( void )
{
	return new CMultiBmpX;
}

class CMultiRectX : public IMultiRect
{
public:
	CMultiRectX()	{ m_dwCount = 0; }
	virtual ~CMultiRectX()	{}
	static void* operator new(size_t uSize) throw()	{ return s_heap.Alloc(uSize); }
	static void	 operator delete(void* p)	{ s_heap.Free(p); }
	static CMyHeap	s_heap;

	virtual DWORD	Release	( void );
	virtual void	Add	( D3DXVECTOR3* pPos, float* pUV );
	virtual void	Clear	( void )	{ m_dwCount = 0; }
	virtual void	Show	( CMyBitmap* pBmp, D3DMATERIAL8* lpMaterial, bool bTwoSide, DWORD dwAsb, DWORD dwAdb );
public:
	DWORD			m_dwCount;
	PhyOutVertex	m_vertex[512 * 6];
};

CMyHeap CMultiRectX::s_heap("CMultiRectX");
DWORD CMultiRectX::Release ( void )
{
	delete this;
	return 0;
}
void CMultiRectX::Add ( D3DXVECTOR3* pPos, float* pUV )
{
	if ( m_dwCount >= sizeof ( m_vertex ) / 6 )
		return;

	D3DXVECTOR3 v1, v2, normal;
	v1.x = pPos[1].x - pPos[0].x;	v1.y = pPos[1].y - pPos[0].y;	v1.z = pPos[1].z - pPos[0].z;
	v2.x = pPos[2].x - pPos[1].x;	v2.y = pPos[2].y - pPos[1].y;	v2.z = pPos[2].z - pPos[1].z;
	D3DXVec3Cross ( &normal, &v2, &v1 );
	D3DXVec3Normalize ( &normal, &normal );

	int nIndex[6] = { 0, 1, 2, 1, 3, 2 };
	for ( int i = 0; i < 6; i++ )
	{
		int n = nIndex[i];
		PhyOutVertex vertex;
		vertex.x	= pPos[n].x;
		vertex.y	= pPos[n].y;
		vertex.z	= pPos[n].z;
		vertex.nx	= normal.x;
		vertex.ny	= normal.y;
		vertex.nz	= normal.z;
		vertex.color = 0xffffffff;
		vertex.u	= pUV[n * 2];
		vertex.v	= pUV[n * 2 + 1];
		m_vertex[m_dwCount * 6 + i] = vertex;
	}
	m_dwCount++;
}
void CMultiRectX::Show ( CMyBitmap* pBmp, D3DMATERIAL8* lpMaterial, bool bTwoSide, DWORD dwAsb, DWORD dwAdb )
{
	if ( !pBmp || m_dwCount == 0 )
		return;
	CMyBitmap::PrepareModle ();
	Phy_Draw ( m_vertex, m_dwCount * 2, ( ( CMyBitmapX* )pBmp )->m_sprite->lpTex, lpMaterial, dwAsb, dwAdb, bTwoSide, true );
}
IMultiRect* MultiRectCreate ( void )
{
	return new CMultiRectX;
}

class CPuzzleCellX : public IPuzzleCell
{
public:
	CPuzzleCellX();
	virtual ~CPuzzleCellX()	{}
	static void* operator new(size_t uSize) throw()	{ return s_heap.Alloc(uSize); }
	static void	 operator delete(void* p)	{ s_heap.Free(p); }
	static CMyHeap	s_heap;

	virtual DWORD	Release	( void );
	virtual void	SetColor	( int x, int y, DWORD dwColor, int nLayer );
	virtual void	Show	( int x, int y, int nWidth, int nHeight, CMyBitmap* pBmp, CMyBitmap* pMixBmp, CMyBitmap* pBmp2, CMyBitmap* pBmp3 );
public:
	DWORD	m_dwColor[4][15];
};

CMyHeap CPuzzleCellX::s_heap("CPuzzleCellX");

static SpriteVertex	s_CellVertex[15];
static WORD			s_CellIndex[48] = {	0, 1, 4,	0, 4, 3,	1, 2, 4,	2, 5, 4,
										3, 4, 6,	4, 7, 6,	4, 5, 8,	4, 8, 7,
										6, 7, 10,	6, 10, 9,	7, 8, 10,	10, 8, 11,
										9, 10, 12,	10, 13, 12,	10, 11, 14,	10, 14, 13 };

CPuzzleCellX::CPuzzleCellX ()
{
	memset ( m_dwColor, 0xff, sizeof ( m_dwColor ) );
}
DWORD CPuzzleCellX::Release ( void )
{
	delete this;
	return 0;
}
void CPuzzleCellX::SetColor ( int x, int y, DWORD dwColor, int nLayer )
{
	if ( nLayer < 1 )
		nLayer = 1;
	else if ( nLayer > 3 )
		nLayer = 3;
	m_dwColor[nLayer - 1][( y % 5 ) * 3 + x % 3] = dwColor;
}
void CPuzzleCellX::Show ( int x, int y, int nWidth, int nHeight, CMyBitmap* pBmp, CMyBitmap* pMixBmp, CMyBitmap* pBmp2, CMyBitmap* pBmp3 )
{
	if ( !pBmp )
		return;
	CMyBitmap::PrepareMixSprite ();
	if ( nWidth <= 0 )
		nWidth = pBmp->GetWidth ();
	if ( nHeight <= 0 )
		nHeight = pBmp->GetHeight ();

	int i;
	for ( i = 0; i < 15; i++ )
	{
		int nCol = i % 3, nRow = i / 3;
		s_CellVertex[i].x		= ( float )x + ( float )( nCol * nWidth ) * 0.5f;
		s_CellVertex[i].y		= ( float )y + ( float )( nRow * nHeight ) * 0.25f;
		s_CellVertex[i].z		= 0;
		s_CellVertex[i].rhw		= 1.0f;
		s_CellVertex[i].color	= m_dwColor[0][i];
		s_CellVertex[i].u		= ( float )nCol * 0.5f;
		s_CellVertex[i].v		= ( float )nRow * 0.25f;
	}

	D3DMATERIAL9 material;
	ZeroMemory ( &material, sizeof ( material ) );
	material.Diffuse.r = material.Diffuse.g = material.Diffuse.b = material.Diffuse.a = 1.0f;
	material.Ambient.r = material.Ambient.g = material.Ambient.b = material.Ambient.a = 1.0f;
	g_D3DDevice->SetMaterial ( &material );
	g_D3DDevice->SetFVF ( SPRITE_VERTEX );

	if ( !pMixBmp )
	{
		g_D3DDevice->SetTexture ( 0, _BMP_TEX ( pBmp ) );
		g_D3DDevice->SetTexture ( 1, NULL );
		g_D3DDevice->SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		g_D3DDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	}
	else
	{
		g_D3DDevice->SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
		g_D3DDevice->SetRenderState ( D3DRS_ALPHATESTENABLE, TRUE );
		g_D3DDevice->SetRenderState ( D3DRS_ALPHAREF, 8 );
		g_D3DDevice->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		g_D3DDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
		g_D3DDevice->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		g_D3DDevice->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		g_D3DDevice->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
		g_D3DDevice->SetTextureStageState ( 1, D3DTSS_TEXCOORDINDEX, 0 );
		g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		g_D3DDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
		g_D3DDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );
		g_D3DDevice->SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_BLENDDIFFUSEALPHA );
		g_D3DDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_D3DDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		g_D3DDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		g_D3DDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_D3DDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		g_D3DDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

		g_D3DDevice->SetTexture ( 0, _BMP_TEX ( pBmp ) );
		g_D3DDevice->SetTexture ( 1, _BMP_TEX ( pMixBmp ) );
		g_D3DDevice->SetTexture ( 2, NULL );
	}
	g_D3DDevice->DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST, 0, 15, 16, s_CellIndex, D3DFMT_INDEX16, s_CellVertex, sizeof ( SpriteVertex ) );

	if ( pBmp2 )
	{
		for ( i = 0; i < 15; i++ )
			s_CellVertex[i].color = m_dwColor[1][i];
		g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		g_D3DDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_D3DDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		g_D3DDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_BLENDDIFFUSEALPHA );
		g_D3DDevice->SetTexture ( 0, _BMP_TEX ( pBmp2 ) );
		g_D3DDevice->SetTexture ( 1, NULL );
		g_D3DDevice->DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST, 0, 15, 16, s_CellIndex, D3DFMT_INDEX16, s_CellVertex, sizeof ( SpriteVertex ) );
	}
	if ( pBmp3 )
	{
		for ( i = 0; i < 15; i++ )
			s_CellVertex[i].color = m_dwColor[2][i];
		g_D3DDevice->SetTexture ( 0, _BMP_TEX ( pBmp3 ) );
		g_D3DDevice->SetTexture ( 1, NULL );
		g_D3DDevice->DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST, 0, 15, 16, s_CellIndex, D3DFMT_INDEX16, s_CellVertex, sizeof ( SpriteVertex ) );
	}
}
IPuzzleCell* PuzzelCellCreate ( void )
{
	return new CPuzzleCellX;
}

static DWORD s_dwTargetAmount = 0;

class CTargetBmpX : public ITargetBmp
{
public:
	CTargetBmpX ( DWORD dwWidth, DWORD dwHeight );
	virtual ~CTargetBmpX();

	virtual DWORD	Release	( void );
	virtual BOOL	Begin	( void );
	virtual void	End		( void );
	virtual void	Show	( int x, int y, int nWidth, int nHeight );
	virtual void	SetAlpha	( BYTE btIndex, BYTE btAlpha );
	virtual void	ShowAlpha	( int x, int y, int nWidth, int nHeight, DWORD dwColor );
	virtual C3Texture*	GetTexture	( void )	{ return m_pTexture; }		virtual int		GetTextureID	( void )	{ return m_pTexture ? m_pTexture->nID : -1; }
public:
	DWORD				m_dwAlpha[25];
	SpriteVertex		m_vertex[25];
	WORD				m_wIndex[96];
	C3Texture*			m_pTexture;
	LPDIRECT3DSURFACE9	m_pOldTarget;
	LPDIRECT3DSURFACE9	m_pOldDepth;
};

CTargetBmpX::CTargetBmpX ( DWORD dwWidth, DWORD dwHeight )
{
	m_pTexture = NULL;
	if ( !Texture_Create ( &m_pTexture, dwWidth, dwHeight, 1, D3DFMT_R8G8B8, D3DPOOL_DEFAULT, D3DUSAGE_RENDERTARGET, true ) )
		ErrorMsg ( "Create TargetTexture Error" );
	m_pOldTarget	= NULL;
	m_pOldDepth		= NULL;

	s_dwTargetAmount++;
	DebugMsg ( "Target Texture Amount %u", s_dwTargetAmount );

	for ( int b = 0; b < 4; b++ )
	{
		for ( int c = 0; c < 4; c++ )
		{
			WORD* p = m_wIndex + ( b * 4 + c ) * 3;
			p[0]		= ( WORD )( c + b * 5 );
			p[1]		= ( WORD )( c + b * 5 + 5 );
			p[2]		= ( WORD )( c + b * 5 + 1 );
			p[48 + 0]	= ( WORD )( c + b * 5 + 1 );
			p[48 + 1]	= ( WORD )( c + b * 5 + 5 );
			p[48 + 2]	= ( WORD )( c + b * 5 + 6 );
		}
	}
}

CTargetBmpX::~CTargetBmpX ()
{
	if ( m_pTexture )
		Texture_Unload ( &m_pTexture );
	m_pTexture = NULL;
	s_dwTargetAmount--;
	DebugMsg ( "Target Texture Amount %u", s_dwTargetAmount );
}
DWORD CTargetBmpX::Release ( void )
{
	delete this;
	return 0;
}
BOOL CTargetBmpX::Begin ( void )
{
	if ( FAILED ( g_D3DDevice->GetRenderTarget ( 0, &m_pOldTarget ) ) )
		ErrorMsg ( "TargetTexture GetRenderTarget Error" );
	if ( FAILED ( g_D3DDevice->GetDepthStencilSurface ( &m_pOldDepth ) ) )
		ErrorMsg ( "TargetTexture GetDepthStencilSurface Error" );

	LPDIRECT3DSURFACE9 pSurface = NULL;
	if ( !m_pTexture || FAILED ( m_pTexture->lpTex->GetSurfaceLevel ( 0, &pSurface ) ) )
		ErrorMsg ( "TargetTexture GetSurfaceLevel Error" );
	if ( FAILED ( g_D3DDevice->SetRenderTarget ( 0, pSurface ) ) )
		ErrorMsg ( "TargetTexture SetRenderTarget Error" );
	g_D3DDevice->SetDepthStencilSurface ( NULL );
	if ( pSurface )
	{
		pSurface->Release ();
		pSurface = NULL;
	}
	g_D3DDevice->BeginScene ();
	return true;
}
void CTargetBmpX::End ( void )
{
	g_D3DDevice->EndScene ();
	if ( FAILED ( g_D3DDevice->SetRenderTarget ( 0, m_pOldTarget ) ) )
		ErrorMsg ( "TargetTexture SetRenderTarget Error" );
	g_D3DDevice->SetDepthStencilSurface ( m_pOldDepth );
	if ( m_pOldTarget )
	{
		m_pOldTarget->Release ();
		m_pOldTarget = NULL;
	}
	if ( m_pOldDepth )
	{
		m_pOldDepth->Release ();
		m_pOldDepth = NULL;
	}
}
void CTargetBmpX::Show ( int x, int y, int nWidth, int nHeight )
{
	static SpriteVertex s_vertex[4];
	s_vertex[0].x = ( float )x - 0.5f;
	s_vertex[0].y = ( float )y - 0.5f;
	s_vertex[0].z = 0;
	s_vertex[0].rhw = 1.0f;
	s_vertex[0].color = 0xffffffff;
	s_vertex[0].u = 0;
	s_vertex[0].v = 0;
	s_vertex[1].x = ( float )x - 0.5f;
	s_vertex[1].y = ( float )( y + nHeight ) - 0.5f;
	s_vertex[1].z = 0;
	s_vertex[1].rhw = 1.0f;
	s_vertex[1].color = 0xffffffff;
	s_vertex[1].u = 0;
	s_vertex[1].v = 1.0f;
	s_vertex[2].x = ( float )( x + nWidth ) - 0.5f;
	s_vertex[2].y = ( float )y - 0.5f;
	s_vertex[2].z = 0;
	s_vertex[2].rhw = 1.0f;
	s_vertex[2].color = 0xffffffff;
	s_vertex[2].u = 1.0f;
	s_vertex[2].v = 0;
	s_vertex[3].x = ( float )( x + nWidth ) - 0.5f;
	s_vertex[3].y = ( float )( y + nHeight ) - 0.5f;
	s_vertex[3].z = 0;
	s_vertex[3].rhw = 1.0f;
	s_vertex[3].color = 0xffffffff;
	s_vertex[3].u = 1.0f;
	s_vertex[3].v = 1.0f;

	if ( s_nShowMode != modeSprite )
		CMyBitmap::PrepareSprite ();
	SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
	SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	SetRenderState ( D3DRS_ALPHATESTENABLE, true );
	SetRenderState ( D3DRS_ALPHAREF, 0 );
	SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	if ( m_pTexture )
		g_D3DDevice->SetTexture ( 0, m_pTexture->lpTex );
	else
		g_D3DDevice->SetTexture ( 0, NULL );
	g_D3DDevice->SetFVF ( SPRITE_VERTEX );
	g_D3DDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, s_vertex, sizeof ( SpriteVertex ) );

	CMyBitmap::ShowRect ( x, y, x + nWidth, y + nHeight, 0xffffffff );
}
void CTargetBmpX::SetAlpha ( BYTE btIndex, BYTE btAlpha )
{
	if ( btIndex < 25 )
		m_dwAlpha[btIndex] = btAlpha;
}
void CTargetBmpX::ShowAlpha ( int x, int y, int nWidth, int nHeight, DWORD dwColor )
{
	dwColor &= 0xffffff;
	for ( int i = 0; i < 25; i++ )
	{
		int nCol = i % 5, nRow = i / 5;
		m_vertex[i].color	= ( m_dwAlpha[i] << 24 ) + dwColor;
		m_vertex[i].rhw		= 1.0f;
		m_vertex[i].u		= ( float )nCol * 0.25f;
		m_vertex[i].v		= ( float )nRow * 0.25f;
		m_vertex[i].z		= 0;
		m_vertex[i].x		= ( float )( x + nCol * nWidth * 64 / 256 ) - 0.5f;
		m_vertex[i].y		= ( float )( y + nRow * nHeight * 64 / 256 ) - 0.5f;
	}

	g_D3DDevice->SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	g_D3DDevice->SetRenderState ( D3DRS_ALPHATESTENABLE, TRUE );
	g_D3DDevice->SetRenderState ( D3DRS_ALPHAREF, 0 );
	g_D3DDevice->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	g_D3DDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
	g_D3DDevice->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_D3DDevice->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

	if ( m_pTexture )
		g_D3DDevice->SetTexture ( 0, m_pTexture->lpTex );
	else
		g_D3DDevice->SetTexture ( 0, NULL );
	g_D3DDevice->SetTexture ( 1, NULL );
	g_D3DDevice->SetFVF ( SPRITE_VERTEX );
	g_D3DDevice->DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST, 0, 25, 32, m_wIndex, D3DFMT_INDEX16, m_vertex, sizeof ( SpriteVertex ) );
}
ITargetBmp* TargetBmpCreate ( DWORD dwWidth, DWORD dwHeight )
{
	return new CTargetBmpX ( dwWidth, dwHeight );
}

class CPuzzleTriangleX : public IPuzzleTriangle
{
public:
	CPuzzleTriangleX();
	virtual ~CPuzzleTriangleX()	{}
	static void* operator new(size_t uSize) throw()	{ return s_heap.Alloc(uSize); }
	static void	 operator delete(void* p)	{ s_heap.Free(p); }
	static CMyHeap	s_heap;

	virtual DWORD	Release	( void );
	virtual void	SetBlend	( DWORD dwSrcBlend, DWORD dwDestBlend, DWORD dwColorOp )	{ m_dwSrcBlend = dwSrcBlend; m_dwDestBlend = dwDestBlend; m_dwColorOp = dwColorOp; }
	virtual void	SetVertex	( int nIndex, int x, int y, DWORD dwColor, float u, float v );
	virtual void	Show	( CMyBitmap* pBmp );
public:
	SpriteVertex	m_vertex[3];
	DWORD			m_dwSrcBlend;
	DWORD			m_dwDestBlend;
	DWORD			m_dwColorOp;
};

CMyHeap CPuzzleTriangleX::s_heap("CPuzzleTriangleX");

CPuzzleTriangleX::CPuzzleTriangleX ()
{
	for ( int i = 0; i < 3; i++ )
	{
		m_vertex[i].x		= 0;
		m_vertex[i].y		= 0;
		m_vertex[i].z		= 0;
		m_vertex[i].rhw		= 1.0f;
		m_vertex[i].color	= 0xffffffff;
		m_vertex[i].u		= 0;
		m_vertex[i].v		= 0;
	}
	m_dwSrcBlend	= D3DBLEND_SRCALPHA;
	m_dwDestBlend	= D3DBLEND_INVSRCALPHA;
	m_dwColorOp		= D3DTOP_MODULATE;
}
DWORD CPuzzleTriangleX::Release ( void )
{
	delete this;
	return 0;
}
void CPuzzleTriangleX::SetVertex ( int nIndex, int x, int y, DWORD dwColor, float u, float v )
{
	nIndex %= 3;
	m_vertex[nIndex].x		= ( float )x;
	m_vertex[nIndex].y		= ( float )y;
	m_vertex[nIndex].color	= dwColor;
	m_vertex[nIndex].u		= u;
	m_vertex[nIndex].v		= v;
}
void CPuzzleTriangleX::Show ( CMyBitmap* pBmp )
{
	if ( !pBmp )
		return;
	CMyBitmap::PrepareMixSprite ();

	D3DMATERIAL9 material;
	ZeroMemory ( &material, sizeof ( material ) );
	material.Diffuse.r = material.Diffuse.g = material.Diffuse.b = material.Diffuse.a = 1.0f;
	material.Ambient.r = material.Ambient.g = material.Ambient.b = material.Ambient.a = 1.0f;
	g_D3DDevice->SetMaterial ( &material );

	g_D3DDevice->SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	g_D3DDevice->SetRenderState ( D3DRS_ALPHATESTENABLE, TRUE );
	g_D3DDevice->SetRenderState ( D3DRS_ALPHAREF, 8 );
	g_D3DDevice->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	g_D3DDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
	g_D3DDevice->SetRenderState ( D3DRS_SRCBLEND, m_dwSrcBlend );
	g_D3DDevice->SetRenderState ( D3DRS_DESTBLEND, m_dwDestBlend );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, m_dwColorOp );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_BLENDDIFFUSEALPHA );

	g_D3DDevice->SetTexture ( 0, _BMP_TEX ( pBmp ) );
	g_D3DDevice->SetTexture ( 1, NULL );
	g_D3DDevice->SetFVF ( SPRITE_VERTEX );
	g_D3DDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, 1, m_vertex, sizeof ( SpriteVertex ) );
}
IPuzzleTriangle* PuzzelTriangleCreate ( void )
{
	return new CPuzzleTriangleX;
}

class CPuzzleBlockX : public IPuzzleBlock
{
public:
	CPuzzleBlockX();
	virtual ~CPuzzleBlockX();
	static void* operator new(size_t uSize) throw()	{ return s_heap.Alloc(uSize); }
	static void	 operator delete(void* p)	{ s_heap.Free(p); }
	static CMyHeap	s_heap;

	BOOL	Create	( DWORD dwWidth, DWORD dwHeight, DWORD dwCols, DWORD dwRows );

	virtual DWORD	Release	( void );
	virtual void	SetSize	( DWORD dwWidth, DWORD dwHeight )	{ m_dwWidth = dwWidth; m_dwHeight = dwHeight; }
	virtual void	SetBlend	( DWORD dwSrcBlend, DWORD dwDestBlend, DWORD dwColorOp )	{ m_dwSrcBlend = dwSrcBlend; m_dwDestBlend = dwDestBlend; m_dwColorOp = dwColorOp; }
	virtual void	SetColor	( DWORD x, DWORD y, DWORD dwColor );
	virtual void	SetUV	( DWORD x, DWORD y, float u, float v );
	virtual void	Show	( int x, int y, CMyBitmap* pBmp );
public:
	SpriteVertex*	m_vertex;
	WORD*			m_bufIndex;
	DWORD			m_dwSrcBlend;
	DWORD			m_dwDestBlend;
	DWORD			m_dwColorOp;
	DWORD			m_dwWidth;
	DWORD			m_dwHeight;
	DWORD			m_dwCols;
	DWORD			m_dwRows;
};

CMyHeap CPuzzleBlockX::s_heap("CPuzzleBlockX");

CPuzzleBlockX::CPuzzleBlockX ()
{
	m_vertex		= NULL;
	m_bufIndex		= NULL;
	m_dwSrcBlend	= D3DBLEND_SRCALPHA;
	m_dwDestBlend	= D3DBLEND_INVSRCALPHA;
	m_dwColorOp		= D3DTOP_MODULATE;
	m_dwWidth		= 0;
	m_dwHeight		= 0;
	m_dwCols		= 0;
	m_dwRows		= 0;
}

CPuzzleBlockX::~CPuzzleBlockX ()
{
	if ( m_vertex )
		delete [] m_vertex;
	if ( m_bufIndex )
		delete [] m_bufIndex;
}
BOOL CPuzzleBlockX::Create ( DWORD dwWidth, DWORD dwHeight, DWORD dwCols, DWORD dwRows )
{
	m_dwWidth	= dwWidth;
	m_dwHeight	= dwHeight;
	m_dwCols	= ( dwCols < 1 ) ? 1 : dwCols;
	m_dwRows	= ( dwRows < 1 ) ? 1 : dwRows;

	m_vertex = new SpriteVertex[( m_dwRows + 1 ) * ( m_dwCols + 1 )];
	IF_NOT ( m_vertex )
		return false;
	DWORD i, j;
	for ( j = 0; j < m_dwRows + 1; j++ )
	{
		for ( i = 0; i < m_dwCols + 1; i++ )
		{
			SpriteVertex* p = m_vertex + j * ( m_dwCols + 1 ) + i;
			p->x		= ( float )( ( double )m_dwWidth * ( double )i / ( int )m_dwCols );
			p->y		= ( float )( ( double )m_dwHeight * ( double )j / ( int )m_dwRows );
			p->z		= 0;
			p->rhw		= 1.0f;
			p->color	= 0xffffffff;
			p->u		= ( float )( ( double )i / ( double )m_dwCols );
			p->v		= ( float )( ( double )j / ( double )m_dwRows );
		}
	}

	m_bufIndex = new WORD[m_dwCols * m_dwRows * 6];
	IF_NOT ( m_bufIndex )
		return false;
	DWORD n = 0;
	for ( j = 0; j < dwRows; j++ )
	{
		for ( i = 0; i < dwCols; i++ )
		{
			DWORD dwBase = ( dwCols + 1 ) * j + i;
			m_bufIndex[n++] = ( WORD )dwBase;
			m_bufIndex[n++] = ( WORD )( dwBase + 1 );
			m_bufIndex[n++] = ( WORD )( dwBase + dwCols + 1 );
			m_bufIndex[n++] = ( WORD )( dwBase + 1 );
			m_bufIndex[n++] = ( WORD )( dwBase + dwCols + 2 );
			m_bufIndex[n++] = ( WORD )( dwBase + dwCols + 1 );
		}
	}
	return true;
}
DWORD CPuzzleBlockX::Release ( void )
{
	delete this;
	return 0;
}
void CPuzzleBlockX::SetColor ( DWORD x, DWORD y, DWORD dwColor )
{
	m_vertex[( y % ( m_dwRows + 1 ) ) * ( m_dwCols + 1 ) + x % ( m_dwCols + 1 )].color = dwColor;
}
void CPuzzleBlockX::SetUV ( DWORD x, DWORD y, float u, float v )
{
	SpriteVertex* p = m_vertex + ( y % ( m_dwRows + 1 ) ) * ( m_dwCols + 1 ) + x % ( m_dwCols + 1 );
	p->u = u;
	p->v = v;
}
void CPuzzleBlockX::Show ( int x, int y, CMyBitmap* pBmp )
{
	if ( !pBmp )
		return;
	CMyBitmap::PrepareMixSprite ();

	for ( DWORD j = 0; j < m_dwRows + 1; j++ )
	{
		for ( DWORD i = 0; i < m_dwCols + 1; i++ )
		{
			SpriteVertex* p = m_vertex + j * ( m_dwCols + 1 ) + i;
			p->x = ( float )( ( double )m_dwWidth * ( int )i / ( int )m_dwCols + x );
			p->y = ( float )( ( double )m_dwHeight * ( double )j / ( int )m_dwRows + y );
		}
	}

	D3DMATERIAL9 material;
	ZeroMemory ( &material, sizeof ( material ) );
	material.Diffuse.r = material.Diffuse.g = material.Diffuse.b = material.Diffuse.a = 1.0f;
	material.Ambient.r = material.Ambient.g = material.Ambient.b = material.Ambient.a = 1.0f;
	g_D3DDevice->SetMaterial ( &material );

	g_D3DDevice->SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	g_D3DDevice->SetRenderState ( D3DRS_ALPHATESTENABLE, TRUE );
	g_D3DDevice->SetRenderState ( D3DRS_ALPHAREF, 8 );
	g_D3DDevice->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	g_D3DDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
	g_D3DDevice->SetRenderState ( D3DRS_SRCBLEND, m_dwSrcBlend );
	g_D3DDevice->SetRenderState ( D3DRS_DESTBLEND, m_dwDestBlend );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, m_dwColorOp );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	g_D3DDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_BLENDDIFFUSEALPHA );

	g_D3DDevice->SetTexture ( 0, _BMP_TEX ( pBmp ) );
	g_D3DDevice->SetTexture ( 1, NULL );
	g_D3DDevice->SetFVF ( SPRITE_VERTEX );
	g_D3DDevice->DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST, 0, m_dwCols * m_dwRows * 6, m_dwCols * m_dwRows * 2,
										  m_bufIndex, D3DFMT_INDEX16, m_vertex, sizeof ( SpriteVertex ) );
}
IPuzzleBlock* PuzzelBlockCreate ( DWORD dwWidth, DWORD dwHeight, DWORD dwCols, DWORD dwRows )
{
	CPuzzleBlockX* pBlock = new CPuzzleBlockX;
	if (!pBlock)
		return NULL;
	if (!pBlock->Create(dwWidth, dwHeight, dwCols, dwRows))
	{
		SAFE_DELETE(pBlock);
		return NULL;
	}
	return pBlock;
}

class C3DTextureX : public I3DTexture
{
public:
	C3DTextureX();
	virtual ~C3DTextureX();
	static void* operator new(size_t uSize) throw()	{ return s_heap.Alloc(uSize); }
	static void	 operator delete(void* p)	{ s_heap.Free(p); }
	static CMyHeap	s_heap;

	virtual DWORD		Release	( void );
	virtual BOOL		Create	( const char* pszFileName );
	virtual void		Destroy	( void );
	virtual int			GetTextureID ( void );
	virtual C3Texture*	GetTexture	( void )	{return m_pTexture;}
	virtual BOOL		LockRect ( RECT rect );
	virtual void		SetPixel ( int x, int y, DWORD dwColor );
	virtual void		Unlock ( void );
public:
	C3Texture*	m_pTexture;
	D3DLOCKED_RECT	m_lock;
	RECT		m_rect;
};

CMyHeap C3DTextureX::s_heap("C3DTextureX");

C3DTextureX::C3DTextureX ()
{
	m_pTexture	=NULL;
	m_lock.pBits = NULL;
}
BOOL C3DTextureX::LockRect ( RECT rect )
{
	if (!m_pTexture || m_lock.pBits)
		return false;
	m_rect = rect;
	if (Texture_LockRect ( m_pTexture, rect, m_lock ) && m_lock.pBits)
		return true;
	return false;
}
void C3DTextureX::SetPixel ( int x, int y, DWORD dwColor )
{
	if (!m_pTexture || !m_lock.pBits)
		return;
	if (x < m_rect.left || y < m_rect.top || x >= m_rect.right || y >= m_rect.bottom)
		return;
	*( DWORD* )( ( BYTE* )m_lock.pBits + ( y - m_rect.top ) * m_lock.Pitch + ( x - m_rect.left ) * 4 ) = dwColor;
}
void C3DTextureX::Unlock ( void )
{
	if (m_pTexture && m_lock.pBits)
	{
		Texture_Unlock ( m_pTexture );
		m_lock.pBits = NULL;
	}
}

C3DTextureX::~C3DTextureX ()
{
	this->Destroy ();
}
DWORD C3DTextureX::Release ( void )
{
	delete this;
	return 0;
}
BOOL C3DTextureX::Create(const char* pszFileName)
{
	if (!pszFileName)
		return false;
	int nReturn = ::Texture_Load(&m_pTexture, pszFileName, 8, D3DPOOL_MANAGED, 0, 0, D3DX_DEFAULT);
	return (nReturn != -1);
}
void C3DTextureX::Destroy(void)
{
	if (m_pTexture)
		::Texture_Unload(&m_pTexture);
}
int C3DTextureX::GetTextureID ( void )
{
	return m_pTexture->nID;
}
I3DTexture* TextureCreate ( const char* pszFileName )
{
	C3DTextureX* pTexture = new C3DTextureX;
	if (!pTexture)
		return NULL;
	if (!pTexture->Create(pszFileName))
	{
		SAFE_DELETE(pTexture);
		return NULL;
	}
	return pTexture;
}
I3DTexture* TextureCreateEx ( C3Texture* pTexture )
{
	if (!pTexture)
		return NULL;
	C3DTextureX* pObj = new C3DTextureX;
	if (!pObj)
		return NULL;
	pObj->m_pTexture = pTexture;
	return pObj;
}

class C3DMotionX : public I3DMotion
{
public:
	C3DMotionX ();
	virtual ~C3DMotionX ();
	static void* operator new(size_t uSize) throw()	{ return s_heap.Alloc(uSize); }
	static void	 operator delete(void* p)	{ s_heap.Free(p); }
	static CMyHeap	s_heap;

	virtual DWORD		Release	( void );
	virtual BOOL		Create ( char *filename, BOOL bCache );
	virtual void		Destroy ( void );
	virtual DWORD		GetFrameAmount();
	virtual DWORD		GetMotionAmount()					{return m_dwMotionNum;}
	virtual C3Motion*	GetMotion ( DWORD dwIndex )			{return m_motion[dwIndex];}
	virtual D3DXMATRIXA16*	GetMatrix ( DWORD dwIndex, DWORD dwBone );
	virtual DWORD		GetKeyFrameAmount ( DWORD dwIndex )	{return m_motion[dwIndex]->dwKeyFrames;}
	virtual D3DXMATRIXA16*	GetKeyFrameMatrix ( DWORD dwIndex, DWORD dwKeyFrame )	{return m_motion[dwIndex]->lpKeyFrame[dwKeyFrame].matrix;}
public:
	DWORD		m_dwMotionNum;
	C3Motion	*m_motion[MOTION_MAX];
};

CMyHeap C3DMotionX::s_heap("C3DMotionX");

C3DMotionX::C3DMotionX ()
{
	for ( int n = 0; n < MOTION_MAX; n++ )
		m_motion[n] = 0;
	m_dwMotionNum = 0;
}
C3DMotionX::~C3DMotionX ()
{
	Destroy ();
}
DWORD C3DMotionX::Release ( void )
{
	delete this;
	return 0;
}
BOOL C3DMotionX::Create ( char *filename, BOOL bCache )
{
	m_dwMotionNum = 0;
	for ( int n = 0; n < MOTION_MAX; n++ )
		m_motion[n] = 0;
	DWORD dwSize = 0;
	void *f = Common_OpenRes ( filename, dwSize, 1 );
	if ( !f )
		return false;
	ChunkHeader chunk;
	long nStart = Common_GetSeek ( f );
	Common_SeekRes ( f, 16, SEEK_CUR );
	while ( ( DWORD )Common_GetSeek ( f ) < nStart + dwSize )
	{
		Common_GetChunk ( f, &chunk );
		if ( chunk.byChunkID[0] == 'M' &&
			 chunk.byChunkID[1] == 'O' &&
			 chunk.byChunkID[2] == 'T' &&
			 chunk.byChunkID[3] == 'I' )
		{
			if ( !Motion_Load ( &m_motion[m_dwMotionNum], f ) )
				break;
			if ( bCache )
			{
				char szKey[256] = "";
				sprintf ( szKey, "%s%d", filename, m_dwMotionNum );
				m_motion[m_dwMotionNum]->dwCacheID = HashStringMod ( szKey, 0 );
			}
			m_dwMotionNum++;
			if ( m_dwMotionNum >= MOTION_MAX )
				break;
		}
		else
		{
			Common_SeekRes ( f, chunk.dwChunkSize, SEEK_CUR );
		}
	}
	Common_CloseRes ( f );
	return (m_dwMotionNum>0);
}
void C3DMotionX::Destroy ( void )
{
	for ( DWORD n = 0; n < m_dwMotionNum; n++ )
		Motion_Unload ( &m_motion[n] );
	m_dwMotionNum	=0;
}
DWORD C3DMotionX::GetFrameAmount()
{
	DWORD dwMax	=0;
	for ( DWORD n = 0; n < m_dwMotionNum; n++ )
	{
		dwMax	=__max(dwMax, m_motion[n]->dwFrames);
	}
	return dwMax;
}
D3DXMATRIXA16* C3DMotionX::GetMatrix ( DWORD dwIndex, DWORD dwBone )
{
	C3Motion* pMotion = m_motion[dwIndex % m_dwMotionNum];
	return pMotion->matrix + dwBone % pMotion->dwBoneCount;
}
I3DMotion* MotionCreate ( char* pszFileName, BOOL bCache )
{
	C3DMotionX* pMotion = new C3DMotionX;
	if (!pMotion)
		return NULL;
	if (!pMotion->Create(pszFileName, bCache))
	{
		SAFE_DELETE(pMotion);
		return NULL;
	}
	return pMotion;
}
D3DXMATRIXA16* MotionMatrixGet ( C3Motion* pMotion, int nBone )
{
	return pMotion->matrix + nBone;
}

CMyHeap C3DObjX::s_heap("C3DObjX");

C3DObjX::C3DObjX ()
{
	for ( int n = 0; n < PHY_MAX; n++ )
	{
		m_phy[n] = 0;
		m_bHidden[n] = 0;
	}
	m_dwStepCount = 0;
	m_dwPhyNum = 0;
	m_nDrawType = 4;
	ZeroMemory ( &m_material, sizeof ( m_material ) );
	m_material.Diffuse.r = m_material.Diffuse.g = m_material.Diffuse.b = m_material.Diffuse.a = 1.0f;
	m_material.Specular.r = m_material.Specular.g = m_material.Specular.b = m_material.Specular.a = 1.0f;
	m_material.Power = 0;
	m_material.Ambient.r = m_material.Ambient.g = m_material.Ambient.b = m_material.Ambient.a = 0.5f;
}
C3DObjX::~C3DObjX ()
{
	Destroy ();
}
DWORD C3DObjX::Release ( void )
{
	delete this;
	return 0;
}
bool C3DObjX::IsValid ( void )
{
	return m_phy != NULL;
}
BOOL C3DObjX::Create ( char *filename )
{
	m_dwPhyNum = 0;
	for ( int n = 0; n < PHY_MAX; n++ )
		m_phy[n] = 0;
	DWORD dwSize = 0;
	void *f = Common_OpenRes ( filename, dwSize, 1 );
	if ( !f )
		return false;
	ChunkHeader chunk;
	long nStart = Common_GetSeek ( f );
	Common_SeekRes ( f, 16, SEEK_CUR );
	while ( ( DWORD )Common_GetSeek ( f ) < nStart + dwSize )
	{
		Common_GetChunk ( f, &chunk );
		if ( chunk.byChunkID[0] == 'P' &&
			 chunk.byChunkID[1] == 'H' &&
			 chunk.byChunkID[2] == 'Y' &&
			 ( chunk.byChunkID[3] == ' ' || chunk.byChunkID[3] == '2' || chunk.byChunkID[3] == '3' || chunk.byChunkID[3] == '4' ) )
		{
			BOOL bLoad = false;
			if ( chunk.byChunkID[3] == ' ' )
				bLoad = Phy_Load ( &m_phy[m_dwPhyNum], f, false, true, false, true );
			else if ( chunk.byChunkID[3] == '2' )
				bLoad = Phy_Load ( &m_phy[m_dwPhyNum], f, true, false, false, true );
			else if ( chunk.byChunkID[3] == '3' )
				bLoad = Phy_Load ( &m_phy[m_dwPhyNum], f, true, false, false, false );
			else
				bLoad = Phy_Load ( &m_phy[m_dwPhyNum], f, false, true, false, false );
			if ( !bLoad )
				break;
			m_dwPhyNum++;
		}
		else
		{
			Common_SeekRes ( f, chunk.dwChunkSize, SEEK_CUR );
		}
	}
	Common_CloseRes ( f );
	return (m_dwPhyNum>0);
}
void C3DObjX::Destroy ()
{
	for ( DWORD n = 0; n < m_dwPhyNum; n++ )
		Phy_Unload ( &m_phy[n] );
}
void C3DObjX::Move ( float x, float y, float z )
{
	D3DXMATRIXA16 matrix;
	D3DXMatrixIdentity ( &matrix );
	Common_Translate ( &matrix, x, y, z );

	for ( DWORD n = 0; n < m_dwPhyNum; n++ )
		Phy_Muliply ( m_phy[n], -1, &matrix );
}
void C3DObjX::Rotate ( float x, float y, float z )
{
	D3DXMATRIXA16 matrix;
	D3DXMatrixIdentity ( &matrix );
	Common_Rotate ( &matrix, x, y, z );

	for ( DWORD n = 0; n < m_dwPhyNum; n++ )
		Phy_Muliply ( m_phy[n], -1, &matrix );
}
void C3DObjX::Scale ( float x, float y, float z )
{
	D3DXMATRIXA16 matrix;
	D3DXMatrixIdentity ( &matrix );
	Common_Scale ( &matrix, x, y, z );

	for ( DWORD n = 0; n < m_dwPhyNum; n++ )
		Phy_Muliply ( m_phy[n], -1, &matrix );
}
void C3DObjX::Muliply ( D3DXMATRIXA16 *matrix )
{
	for ( DWORD n = 0; n < m_dwPhyNum; n++ )
		Phy_Muliply ( m_phy[n], -1, matrix );
}
void C3DObjX::SetARGB ( float alpha, float red, float green, float blue )
{
	for ( DWORD n = 0; n < m_dwPhyNum; n++ )
		Phy_SetColor ( m_phy[n], alpha, red, green, blue );
}
int C3DObjX::GetIndexByName ( char *lpName )
{
	if ( !lpName )
		return -1;
	for ( DWORD n = 0; n < m_dwPhyNum; n++ )
	{
		if ( stricmp ( m_phy[n]->lpName, lpName ) == 0 )
			return n;
	}
	return -1;
}
char* C3DObjX::GetPhyName ( DWORD dwIndex )
{
	return m_phy[dwIndex % m_dwPhyNum]->lpName;
}
void C3DObjX::GetBoundingBox ( DWORD dwIndex, D3DXVECTOR3 *lpMin, D3DXVECTOR3 *lpMax )
{
	DWORD dwAmount = m_dwPhyNum;
	if ( dwAmount < 1 )
		dwAmount = 1;
	*lpMin = m_phy[dwIndex % dwAmount]->bboxMin;
	*lpMax = m_phy[dwIndex % dwAmount]->bboxMax;
}
void C3DObjX::SetMotion ( I3DMotion* pMotion )
{
	if ( !pMotion )
		return;
	if ( m_dwPhyNum > pMotion->GetMotionAmount() )
		return;
	for ( DWORD n = 0; n < m_dwPhyNum; n++ )
		m_phy[n]->lpMotion = pMotion->GetMotion ( n );
}
void C3DObjX::SetBlendMotion ( I3DMotion* pMotion, float fWeight )
{
	for ( DWORD n = 0; n < m_dwPhyNum; n++ )
	{
		if ( pMotion )
		{
			if ( m_dwPhyNum > pMotion->GetMotionAmount() )
				return;
			m_phy[n]->lpBlendMotion = pMotion->GetMotion ( n );
			m_phy[n]->lpBlendMotion->fWeight = fWeight;
			m_phy[n]->lpBlendMotion->nFrame = 0;
		}
		else
			m_phy[n]->lpBlendMotion = NULL;
	}
}
C3Motion* C3DObjX::GetMotion ( DWORD dwIndex )
{
	if ( m_dwPhyNum == 0 )
		return NULL;
	return m_phy[dwIndex % m_dwPhyNum]->lpMotion;
}
C3Motion* C3DObjX::GetBlendMotion ( DWORD dwIndex )
{
	if ( m_dwPhyNum == 0 )
		return NULL;
	return m_phy[dwIndex % m_dwPhyNum]->lpBlendMotion;
}
D3DXMATRIXA16* C3DObjX::GetBoneMatrix ( DWORD dwIndex, DWORD dwBone )
{
	C3Motion* pMotion = m_phy[dwIndex % m_dwPhyNum]->lpMotion;
	return pMotion->matrix + dwBone % pMotion->dwBoneCount;
}
D3DXMATRIXA16* C3DObjX::GetPhyMatrix ( DWORD dwIndex )
{
	return &m_phy[dwIndex % m_dwPhyNum]->matrix;
}
void C3DObjX::NextFrame ( int nStep )
{
	for ( DWORD n = 0; n < m_dwPhyNum; n++ )
		Phy_NextFrame ( m_phy[n], nStep );
}
void C3DObjX::SetFrame ( DWORD dwFrame )
{
	for ( DWORD n = 0; n < m_dwPhyNum; n++ )
		Phy_SetFrame ( m_phy[n], dwFrame );
}
void C3DObjX::SetBlendFrame ( DWORD dwFrame )
{
	for ( DWORD n = 0; n < m_dwPhyNum; n++ )
		Phy_SetBlendFrame ( m_phy[n], dwFrame );
}
void C3DObjX::Draw ( int type, float lightx, float lighty, float lightz, float sa, float sr, float sg, float sb, int nAsb, int nAdb )
{
	CMyBitmap::PrepareModle();
	D3DXVECTOR3 vLight ( lightx, lighty, lightz );
	for ( DWORD n = 0; n < m_dwPhyNum; n++ )
	{
		if ( m_bHidden[n] )
			continue;
		m_phy[n]->bMatrix = ( type > 0 );
		if ( type == 2 )
		{
			D3DXPLANE plane ( 0.0f, 0.0f, 1.0f, 0.0f );
			D3DXVECTOR4 light ( lightx, lighty, lightz, 1.0f );
			D3DXMATRIXA16 matShadow;
			D3DXMatrixShadow ( &matShadow, &light, &plane );
			Phy_SetStepCount ( m_phy[n], m_dwStepCount );
			Phy_Calculate ( m_phy[n], true, m_nDrawType, vLight );
			Phy_DrawNormal ( m_phy[n], m_material, &matShadow, true, nAsb, nAdb, m_nDrawType, sa );
		}
		else
		{
			Phy_SetStepCount ( m_phy[n], m_dwStepCount );
			Phy_Calculate ( m_phy[n], true, m_nDrawType, vLight );
			Phy_DrawNormal ( m_phy[n], m_material, NULL, true, nAsb, nAdb, m_nDrawType, 0.35f );
		}
	}
	m_dwStepCount++;
}
void C3DObjX::ChangeTexture ( I3DTexture* pTexture, char *objname )
{
	if ( !objname )
	{
		for ( DWORD n = 0; n < m_dwPhyNum; n++ )
			Phy_ChangeTexture ( m_phy[n], pTexture ? pTexture->GetTextureID() : -1 );
		return;
	}
	int nIndex = this->GetIndexByName ( objname );
	if ( nIndex < 0 )
		nIndex = 0;
	Phy_ChangeTexture ( m_phy[nIndex], pTexture ? pTexture->GetTextureID() : -1 );
}
void C3DObjX::ChangeMixTexture ( I3DTexture* pTexture, char *objname )
{
	if ( !objname )
	{
		for ( DWORD n = 0; n < m_dwPhyNum; n++ )
			Phy_ChangeMixTexture ( m_phy[n], pTexture ? pTexture->GetTextureID() : -1 );
		return;
	}
	int nIndex = this->GetIndexByName ( objname );
	if ( nIndex < 0 )
		nIndex = 0;
	Phy_ChangeMixTexture ( m_phy[nIndex], pTexture ? pTexture->GetTextureID() : -1 );
}
void C3DObjX::ClearMatrix ( void )
{
	for ( DWORD n = 0; n < m_dwPhyNum; n++ )
	{
		Phy_SetColor ( m_phy[n], 1.0f, 1.0f, 1.0f, 1.0f );
		Phy_ClearMatrix ( m_phy[n] );
	}
}
void C3DObjX::SetPhyMatrix ( D3DXMATRIXA16* pobjLocal, D3DXMATRIXA16* pobjWorld )
{
	for ( DWORD n = 0; n < m_dwPhyNum; n++ )
	{
		CopyMemory ( &m_phy[n]->matrixLocal, pobjLocal, sizeof ( D3DXMATRIXA16 ) );
		CopyMemory ( &m_phy[n]->matrixWorld, pobjWorld, sizeof ( D3DXMATRIXA16 ) );
	}
}
I3DObj* MeshCreate ( char* pszFileName )
{
	C3DObjX* pObj = new C3DObjX;
	if (!pObj)
		return NULL;
	if (!pObj->Create(pszFileName))
	{
		SAFE_DELETE(pObj);
		return NULL;
	}
	return pObj;
}
