#ifndef _c3_font_h_
#define _c3_font_h_

#include "c3_main.h"
#include "c3_common.h"

#define FONT_VERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
struct FontVertex
{
	float x, y;
	float z;
	float rhw;
	DWORD color;
	float u, v;
};

enum RENDER_TEXT_STYLE
{
	RENDER_TEXT_NORMAL		= 0,
	RENDER_TEXT_SHADOW		= 1,
	RENDER_TEXT_OUTLINE		= 2,
	RENDER_TEXT_STROKE		= 3,
	RENDER_TEXT_GRADIENT	= 4,
};

struct	C3Char
{
	char					Char[2];
	LPDIRECT3DTEXTURE9		lpTex;
	int						nTop;
	float					u0, v0;
	float					u1, v1;
};

C3_CORE_DLL_API
void Char_Clear ( C3Char *lpChar );

struct C3TexList
{
	BYTE					byAlloc;
	LPDIRECT3DTEXTURE9		*lpFirst;
	LPDIRECT3DTEXTURE9		*lpLast;
	LPDIRECT3DTEXTURE9		*lpEnd;
};

struct	C3Font
{
	void		*lpFace;
	char		szName[256];
	int			nSize;
	int			nRealSize;

	C3Char		*lpChar[256][256];

	C3TexList	atlases;
	int			nCursorX;
	int			nCursorY;
	BYTE		bAntiAlias;
};

C3_CORE_DLL_API
void Font_Clear ( C3Font *lpFont );

C3_CORE_DLL_API
BOOL Font_Create ( C3Font **lpFont, LPCSTR lpFontName, int nSize, int bAntiAlias, int bEngine );

C3_CORE_DLL_API
BOOL Font_CreateDbcsSpec ( C3Font **lpFont, WORD wChar, LPCSTR lpFileName );

C3_CORE_DLL_API
void Font_Release ( C3Font **lpFont );

C3_CORE_DLL_API
void Font_Prepare ( void );

C3_CORE_DLL_API
BOOL Font_CheckPixel ( C3Font *lpFont, LPCSTR lpChar, DWORD dwCount );

C3_CORE_DLL_API
int Font_Draw ( C3Font *lpFont,
				float fX,
				float fY,
				D3DCOLOR color,
				LPCSTR lpChar,
				RENDER_TEXT_STYLE style,
				DWORD color2,
				long lX,
				long lY );

C3_CORE_DLL_API
BOOL Font_Draw ( C3Font *lpFont,
				 float fX,
				 float fY,
				 D3DCOLOR color,
				 LPCSTR lpChar,
				 const void *lpData );

C3_CORE_DLL_API
void Font_DrawBegin ( void );
C3_CORE_DLL_API
bool Font_DrawEnd ( void );

C3_CORE_DLL_API
void Font_SetTextCornerColor ( DWORD dwTopLeft, DWORD dwBottomLeft, DWORD dwTopRight, DWORD dwBottomRight );

C3_CORE_DLL_API
void Font_Save ( C3Font *lpFont, LPCSTR lpFileName );

#endif
