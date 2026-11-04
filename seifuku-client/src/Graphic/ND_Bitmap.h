#ifndef		_BITMAP_H
#define		_BITMAP_H

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<assert.h>
#include	<math.h>

#include	<windows.h>
#include	<wingdi.h>

#include	"c3_main.h"
#include	"c3_sprite.h"
#include	"c3_phy.h"
#include	"c3_camera.h"
#include    <basefunc.h>
#include	"c3_font.h"

#ifdef	GRAPHIC_EXPORTS
#define GRAPHIC_API __declspec ( dllexport )
#else
#define GRAPHIC_API __declspec ( dllimport )
#endif

const int _COLOR_WHITE		=0x00ffffff;
const int _COLOR_GRAY		=0x007f7f7f;
const int _COLOR_DARKGRAY	=0x003f3f3f;
const int _COLOR_BLACK		=0x00000000;
const int _COLOR_RED		=0x00ff0000;
const int _COLOR_GREEN		=0x0000ff00;
const int _COLOR_BLUE		=0x000000ff;
const int _COLOR_YELLOW		=0x00ffff00;
const int _COLOR_TRANS		=0x00ff00ff;

const DWORD _SHOWWAY_NORMAL		 =0;
const DWORD _SHOWWAY_ADDITIVE	 =1;

enum {	modeNone=0,
		modeSprite,
		modeMixSprite,
		modeLine,
		mode3DLine,
		mode5,
		modeMesh,
		modeShape,
		modePtcl,
		modeText
	};

class IDataIcon;

inline CMyPos& TextPosNull ( void )
{
	static CMyPos s_posNull = { 0, 0 };
	return s_posNull;
}

struct IDirect3DDevice8;

#define LINE_VERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )

class GRAPHIC_API CMyBitmap
{
public:
	struct LineVertex
	{
		float		x, y, z;
		float		rhw;
		D3DCOLOR	color;
	};

public:
	CMyBitmap () {}
	CMyBitmap ( const CMyBitmap& ) {}
	CMyBitmap& operator= ( const CMyBitmap& ) { return *this; }

public:
	virtual DWORD	Release	( void ) = 0;
	virtual void	Show	( int x, int y, int alpha =0,DWORD dwShowWay= _SHOWWAY_NORMAL) = 0;
	virtual void	ShowEx	( int x, int y, RECT *lpSrc, DWORD dwWidth, DWORD dwHeight, int alpha = 0,DWORD dwShowWay = _SHOWWAY_NORMAL, int nRotateAngle = 0 ) = 0;
	virtual void	ShowBlend ( int x, int y, RECT *lpSrc, DWORD dwWidth, DWORD dwHeight, int alpha, int nRotateAngle, DWORD dwSrcBlend, DWORD dwDestBlend ) = 0;
	virtual void	GetSize ( int& nWidth, int& nHeight ) = 0;
	virtual DWORD	GetSize ( void ) = 0;

	virtual int		GetWidth	( void ) = 0;
	virtual int		GetHeight	( void ) = 0;

	virtual void	SetColor	( BYTE a, BYTE r, BYTE g, BYTE b ) = 0;
	virtual int		IsbTransparentFormat() = 0;
	virtual BOOL	IsTransparent(CMyPos posPoint) = 0;
	virtual BOOL	LockRect ( RECT rect ) = 0;
	virtual void	SetPixel ( int x, int y, DWORD dwColor ) = 0;
	virtual void	Unlock ( void ) = 0;

public:
	static BOOL	GameFontCreate();
	static void GameFontDestroy();

	static DWORD GetFontSize(void);
	static void  GetFontSize(CMySize& infoSize);
	static int   GetStrShowLen(const char* pszString, int nBegin, int nEnd);
	static bool  CheckStringPixels(const char* pszString, DWORD dwPixels, const char* pszFont, int nFontSize);

	static void	ShowString	( int iPosX, int iPosY, DWORD color, const char *pszString, const char* pszFont = NULL, int nFontSize = 0, BOOL bAlpha = false, RENDER_TEXT_STYLE style = RENDER_TEXT_NORMAL, DWORD color2 = 0xFF000000, CMyPos& pos = TextPosNull() );
	static void	ShowTString	( int iPosX, int iPosY, DWORD color, const char *pszString, const char* pszFont = NULL, int nFontSize = 0, BOOL bAlpha = false, RENDER_TEXT_STYLE style = RENDER_TEXT_NORMAL, DWORD color2 = 0xFF000000, CMyPos& pos = TextPosNull() );
	static void ShowEmotionString(int iPosX, int iPosY, DWORD color, const char* pszString, IDataIcon* pDataIcon, const char* pszFont, int nFontSize, int nImgSize, int nOffsetX, int nOffsetY, RENDER_TEXT_STYLE style = RENDER_TEXT_NORMAL, DWORD color2 = 0xFF000000, CMyPos& pos = TextPosNull() );
	static void SetTextCornerColor ( DWORD dwTopLeft, DWORD dwBottomLeft, DWORD dwTopRight, DWORD dwBottomRight );

	static bool IsDBCSLeadByte		(const char* buf, int nOffset);

public:
	static BOOL GameCameraCreate();
	static void GameCameraDestroy();
	static void GameCameraBuild(CMySize sizeScr);
	static void GameCameraBuild();
	static void GameCameraGetPos(float& fX, float& fY, float& fZ, DWORD dwMask);
	static void GameCameraSet(float fX, float fZ, float fY, DWORD dwMask);
	static void GameCameraMove(BOOL bForward);
	static void GameCameraRotate(int nAngle);
	static void GameCameraZoom(BOOL bIn);
	static void GameCameraSetFov(float fFov);
	static void GameCameraSetType(BOOL bProject);
	static BOOL GameCameraIsProject();

public:
	static BOOL Init3D ( HINSTANCE hInst,
						 const char *lpTitle,
						 DWORD dwWidth,
						 DWORD dwHeight,
						 BOOL bWindowed,
						 WNDPROC proc,
						 DWORD dwBackCount );
	static BOOL Init3DEx ( HWND hWnd,
						   DWORD dwWidth,
  						   DWORD dwHeight,
						   BOOL bWindowed,
						   DWORD dwBackCount );

	static void Quit3D	( void );
	static BOOL IfDeviceLost ( void );
	static BOOL ResetDevice ( void );
	static IDirect3DDevice8* QueryDevice ( void );

public:
	static BOOL Begin3D ( void );

	static BOOL End3D ( void );

	static BOOL ClearBuffer ( BOOL bZBuffer, BOOL bTarget, D3DCOLOR color );

	static BOOL Flip ( const RECT *lpSrcRect, const RECT *lpDestRect, HWND hWnd );

	static DWORD GetShowTriTotal ( void );
	static DWORD CalcRate ( void );

	static void PrepareSprite ( void );
	static void PrepareMixSprite ( void );
	static void PrepareModle ( void );

	static void SetLight ( BOOL bEnable, float x, float y, float z, DWORD dwDiffuse, DWORD dwAmbient, float fReserved );

	static void	GetScreenSize(int& nWidth, int& nHeight);
	static int	GetScreenWidth();
	static int	GetScreenHeight();
	static int	GetObliqueAngle();

	static BOOL HintGraphicDetail ( DWORD dwDetail );
	static void SetReflectFluidSpeed ( float fSpeedU, float fSpeedV );

	static void ScreenToWorld(int nScrX, int nScrY, float& fX, float& fY, float& fZ);
	static void WorldToScreen(float fX, float fY, float fZ, int& nScrX, int& nScrY);
	static void ScrRayBuild(int nScrX, int nScrY, D3DXVECTOR3& vFrom, D3DXVECTOR3& vDir);

	static void	PrepareLine ( void );
	static void	Prepare3DLine ( void );
	static void ShowLine ( LineVertex *line, DWORD num );
	static void ShowLine ( int nPosX1, int nPosY1, int nPosX2, int nPosY2, DWORD dwColor);

	static void	ShowRect ( int x1, int y1, int x2, int y2, DWORD color );
	static void ShowBlock( int x1, int y1, int x2, int y2, DWORD color );
	static void ShowBlock( int x1, int y1, int x2, int y2, DWORD color1, DWORD color2, DWORD color3, DWORD color4);
	static void ShowBlock( CMyPos posA, CMyPos posB, CMyPos posC, CMyPos posD, DWORD color );
	static void Show3DBox( D3DXVECTOR3& vMin, D3DXVECTOR3& vMax, DWORD color );

	static void	ScreenShot	( const char* pszFileName );

	static void PackFileOpen ( const char* pszFileName );
	static void PackFileClose ( void );

public:
	static void Common_Translate ( D3DXMATRIX *matrix, float x, float y, float z );
	static void Common_Rotate ( D3DXMATRIX *matrix, float x, float y, float z );
	static void Common_Scale ( D3DXMATRIX *matrix, float x, float y, float z );
	static void Motion_GetMatrix ( C3Motion *lpMotion, DWORD dwBone, D3DXMATRIXA16 *lpMatrix );
	static void Motion_GetMatrix_Blend ( C3MotionTracker tracker, DWORD dwBone, D3DXMATRIXA16 *lpMatrix );
};

extern "C" GRAPHIC_API CMyBitmap* MyBitmapCreate ( const char* pszBmpFile );
extern "C" GRAPHIC_API CMyBitmap* BmpCreate ( void );

class I3DTexture
{
public:
	virtual DWORD		Release	( void ) = 0;
	virtual BOOL		Create	( const char* pszFileName ) = 0;
	virtual void		Destroy	( void ) = 0;
	virtual int			GetTextureID ( void ) = 0;
	virtual C3Texture*	GetTexture	( void ) = 0;
	virtual BOOL		LockRect ( RECT rect ) = 0;
	virtual void		SetPixel ( int x, int y, DWORD dwColor ) = 0;
	virtual void		Unlock ( void ) = 0;
};

#define		MOTION_MAX	16
class I3DMotion
{
public:
	virtual DWORD		Release	( void ) = 0;
	virtual BOOL		Create ( char *filename, BOOL bCache ) = 0;
	virtual void		Destroy ( void ) = 0;
	virtual DWORD		GetFrameAmount() = 0;
	virtual DWORD		GetMotionAmount() = 0;
	virtual C3Motion*	GetMotion ( DWORD dwIndex ) = 0;
	virtual D3DXMATRIXA16*	GetMatrix ( DWORD dwIndex, DWORD dwBone ) = 0;
	virtual DWORD		GetKeyFrameAmount ( DWORD dwIndex ) = 0;
	virtual D3DXMATRIXA16*	GetKeyFrameMatrix ( DWORD dwIndex, DWORD dwKeyFrame ) = 0;
};

#define		PHY_MAX		16
class I3DObj
{
public:
	virtual DWORD	Release	( void ) = 0;
	virtual bool	IsValid ( void ) = 0;
	virtual BOOL	IsPhyHidden ( DWORD dwIndex ) = 0;
	virtual BOOL	Create	( char *filename ) = 0;
	virtual void	Destroy () = 0;

	virtual void	Move ( float x, float y, float z ) = 0;
	virtual void	Rotate	( float x, float y, float z ) = 0;
	virtual void	Scale	( float x, float y, float z ) = 0;
	virtual void	Muliply ( D3DXMATRIXA16 *matrix ) = 0;
	virtual void	SetARGB ( float alpha, float red, float green, float blue ) = 0;

	virtual int		GetIndexByName ( char *lpName ) = 0;
	virtual char*	GetPhyName ( DWORD dwIndex ) = 0;
	virtual DWORD	GetPhyAmount ( void ) = 0;
	virtual void	GetBoundingBox ( DWORD dwIndex, D3DXVECTOR3 *lpMin, D3DXVECTOR3 *lpMax ) = 0;

	virtual void	SetMotion ( I3DMotion* pMotion ) = 0;
	virtual void	SetBlendMotion ( I3DMotion* pMotion, float fWeight ) = 0;
	virtual C3Motion*	GetMotion ( DWORD dwIndex ) = 0;
	virtual C3Motion*	GetBlendMotion ( DWORD dwIndex ) = 0;
	virtual D3DXMATRIXA16*	GetBoneMatrix ( DWORD dwIndex, DWORD dwBone ) = 0;
	virtual D3DXMATRIXA16*	GetPhyMatrix ( DWORD dwIndex ) = 0;

	virtual void	NextFrame ( int nStep ) = 0;
	virtual void	SetFrame (DWORD dwFrame) = 0;
	virtual void	SetBlendFrame (DWORD dwFrame) = 0;

	virtual void	Draw ( int type, float lightx, float lighty, float lightz, float sa, float sr, float sg, float sb, int nAsb, int nAdb ) = 0;
	virtual void	ChangeTexture ( I3DTexture* pTexture, char *objname=NULL ) = 0;
	virtual void	ChangeMixTexture ( I3DTexture* pTexture, char *objname=NULL ) = 0;
	virtual void	SetDrawType ( int nType ) = 0;
	virtual void	SetMaterial ( D3DMATERIAL8 *lpMaterial ) = 0;
	virtual void	ClearMatrix ( void ) = 0;
	virtual void	SetPhyMatrix ( D3DXMATRIXA16* pobjLocal, D3DXMATRIXA16* pobjWorld ) = 0;
	virtual void	SetPhyHidden ( DWORD dwIndex, BOOL bHidden ) = 0;
};

extern "C" GRAPHIC_API I3DTexture* TextureCreate ( const char* pszFileName );
extern "C" GRAPHIC_API I3DTexture* TextureCreateEx ( C3Texture* pTexture );
extern "C" GRAPHIC_API I3DMotion* MotionCreate ( char* pszFileName, BOOL bCache );
extern "C" GRAPHIC_API D3DXMATRIXA16* MotionMatrixGet ( C3Motion* pMotion, int nBone );
extern "C" GRAPHIC_API I3DObj* MeshCreate ( char* pszFileName );

class IMultiBmp
{
public:
	virtual DWORD	Release	( void ) = 0;
	virtual void	Add	( int x, int y, int nWidth, int nHeight, float u0, float v0, float u1, float v1, float u2, float v2, float u3, float v3 ) = 0;
	virtual void	Clear	( void ) = 0;
	virtual void	Show	( CMyBitmap* pBmp ) = 0;
};

class IMultiRect
{
public:
	virtual DWORD	Release	( void ) = 0;
	virtual void	Add	( D3DXVECTOR3* pPos, float* pUV ) = 0;
	virtual void	Clear	( void ) = 0;
	virtual void	Show	( CMyBitmap* pBmp, D3DMATERIAL8* lpMaterial, bool bTwoSide, DWORD dwAsb, DWORD dwAdb ) = 0;
};

class IPuzzleCell
{
public:
	virtual DWORD	Release	( void ) = 0;
	virtual void	SetColor	( int x, int y, DWORD dwColor, int nLayer ) = 0;
	virtual void	Show	( int x, int y, int nWidth, int nHeight, CMyBitmap* pBmp, CMyBitmap* pMixBmp, CMyBitmap* pBmp2, CMyBitmap* pBmp3 ) = 0;
};

class ITargetBmp
{
public:
	virtual DWORD	Release	( void ) = 0;
	virtual BOOL	Begin	( void ) = 0;
	virtual void	End		( void ) = 0;
	virtual void	Show	( int x, int y, int nWidth, int nHeight ) = 0;
	virtual void	SetAlpha	( BYTE btIndex, BYTE btAlpha ) = 0;
	virtual void	ShowAlpha	( int x, int y, int nWidth, int nHeight, DWORD dwColor ) = 0;
	virtual C3Texture*	GetTexture	( void ) = 0;
	virtual int		GetTextureID	( void ) = 0;
};

class IPuzzleTriangle
{
public:
	virtual DWORD	Release	( void ) = 0;
	virtual void	SetBlend	( DWORD dwSrcBlend, DWORD dwDestBlend, DWORD dwColorOp ) = 0;
	virtual void	SetVertex	( int nIndex, int x, int y, DWORD dwColor, float u, float v ) = 0;
	virtual void	Show	( CMyBitmap* pBmp ) = 0;
};

class IPuzzleBlock
{
public:
	virtual DWORD	Release	( void ) = 0;
	virtual void	SetSize	( DWORD dwWidth, DWORD dwHeight ) = 0;
	virtual void	SetBlend	( DWORD dwSrcBlend, DWORD dwDestBlend, DWORD dwColorOp ) = 0;
	virtual void	SetColor	( DWORD x, DWORD y, DWORD dwColor ) = 0;
	virtual void	SetUV	( DWORD x, DWORD y, float u, float v ) = 0;
	virtual void	Show	( int x, int y, CMyBitmap* pBmp ) = 0;
};

extern "C" GRAPHIC_API IMultiBmp* MultiBmpCreate ( void );
extern "C" GRAPHIC_API IMultiRect* MultiRectCreate ( void );
extern "C" GRAPHIC_API IPuzzleCell* PuzzelCellCreate ( void );
extern "C" GRAPHIC_API ITargetBmp* TargetBmpCreate ( DWORD dwWidth, DWORD dwHeight );
extern "C" GRAPHIC_API IPuzzleTriangle* PuzzelTriangleCreate ( void );
extern "C" GRAPHIC_API IPuzzleBlock* PuzzelBlockCreate ( DWORD dwWidth, DWORD dwHeight, DWORD dwCols, DWORD dwRows );

#endif
