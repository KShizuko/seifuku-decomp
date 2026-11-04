#include "c3_font.h"
#include <map>
#include <vector>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

const int SIZE_FONTBASE = 16;

int GetFontsize(int nSize)
{
	if(nSize <= 16)
		return 16;
	if(nSize <= 32)
		return 32;
	if(nSize <= 64)
		return 64;
	return 128;
}

static FT_Library	s_ftLibrary = 0;

static void Font_DoneLibrary ( void )
{
	FT_Done_FreeType ( s_ftLibrary );
	s_ftLibrary = 0;
}

static BOOL Font_MonoBit ( const BYTE *lpRow, int x )
{
	return ( lpRow[x / 8] & ( 0x80 >> ( x % 8 ) ) ) != 0;
}

struct C3FontFile
{
	std::string		strName;
	std::string		strFile;
};
static std::vector<C3FontFile>	s_fontFiles;
static BOOL						s_bFontFiles = false;

static void Font_ReadInstalledFonts ( void )
{
	OSVERSIONINFOA osv;
	osv.dwOSVersionInfoSize = sizeof ( osv );
	GetVersionExA ( &osv );

	HKEY hKey = 0;
	RegOpenKeyExA ( HKEY_LOCAL_MACHINE,
					osv.dwPlatformId == VER_PLATFORM_WIN32_NT ? "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"
															 : "Software\\Microsoft\\Windows\\CurrentVersion\\Fonts",
					0, KEY_QUERY_VALUE, &hKey );
	if ( !hKey )
		return;

	DWORD dwIndex = 0;
	char szName[128];
	BYTE szFile[260];
	DWORD dwName = 0x80, dwFile = 0x104;
	while ( RegEnumValueA ( hKey, dwIndex, szName, &dwName, NULL, NULL, szFile, &dwFile ) != ERROR_NO_MORE_ITEMS )
	{
		szFile[dwFile] = 0;
		C3FontFile entry;
		entry.strName = szName;
		entry.strFile = ( const char* )szFile;
		s_fontFiles.push_back ( entry );
		dwIndex++;
		dwName = 0x80;
		dwFile = 0x104;
	}
	RegCloseKey ( hKey );
}

static BOOL Font_ResolveFile ( LPCSTR lpFontName, char *lpFile )
{
	if ( !s_bFontFiles )
	{
		s_bFontFiles = true;
		Font_ReadInstalledFonts ();
	}

	int nLength = strlen ( lpFontName );
	if ( nLength > 4 && _strnicmp ( lpFontName + nLength - 4, ".tt", 3 ) == 0 )
	{
		strcpy ( lpFile, lpFontName );
		return true;
	}

	for ( DWORD i = 0; i < s_fontFiles.size (); i++ )
	{
		if ( _strnicmp ( s_fontFiles[i].strName.c_str (), lpFontName, nLength ) == 0 )
		{
			char szFile[80];
			strncpy ( szFile, s_fontFiles[i].strFile.c_str (), 80 );
			GetWindowsDirectoryA ( lpFile, 0x104 );
			strcat ( lpFile, "\\Fonts\\" );
			strcat ( lpFile, szFile );
			return true;
		}
	}
	return false;
}

C3_CORE_DLL_API
void Char_Clear ( C3Char *lpChar )
{
	lpChar->Char[0] = '\0';
	lpChar->Char[1] = '\0';
	lpChar->lpTex = 0;
}

C3_CORE_DLL_API
void Font_Clear ( C3Font *lpFont )
{
	lpFont->nSize = 0;
	lpFont->nRealSize = 0;
	lpFont->atlases.lpLast = lpFont->atlases.lpFirst;
	lpFont->nCursorX = 0;
	lpFont->nCursorY = 0;
	lpFont->bAntiAlias = 0;
	memset ( lpFont->lpChar, 0, sizeof ( C3Char* ) * 256 * 256 );
}

static void TexList_PushBack ( C3TexList *lpList, LPDIRECT3DTEXTURE9 lpTex )
{
	if ( lpList->lpLast == lpList->lpEnd )
	{
		int nSize = lpList->lpFirst ? lpList->lpLast - lpList->lpFirst : 0;
		int nGrow = nSize < 2 ? 1 : nSize;
		LPDIRECT3DTEXTURE9 *lpNew = new LPDIRECT3DTEXTURE9[nSize + nGrow];
		for ( int i = 0; i < nSize; i++ )
			lpNew[i] = lpList->lpFirst[i];
		delete [] lpList->lpFirst;
		lpList->lpFirst = lpNew;
		lpList->lpLast = lpNew + nSize;
		lpList->lpEnd = lpNew + nSize + nGrow;
	}
	*lpList->lpLast++ = lpTex;
}

static void TexList_Free ( C3TexList *lpList )
{
	delete [] lpList->lpFirst;
	lpList->lpFirst = 0;
	lpList->lpLast = 0;
	lpList->lpEnd = 0;
}

static C3Font *Font_New ( void )
{
	C3Font *lpFont = new C3Font;
	lpFont->atlases.lpFirst = 0;
	lpFont->atlases.lpLast = 0;
	lpFont->atlases.lpEnd = 0;
	return lpFont;
}

C3_CORE_DLL_API
BOOL Font_Create ( C3Font **lpFont, LPCSTR lpFontName, int nSize, int bAntiAlias, int bEngine )
{
	if ( !bEngine )
	{
		*lpFont = Font_New ();
		Font_Clear ( *lpFont );
		( *lpFont )->lpFace = 0;
	}
	else
	{
		char szFile[0x108];
		ZeroMemory ( szFile, sizeof ( szFile ) );
		if ( !Font_ResolveFile ( lpFontName, szFile ) )
			return false;

		FT_Face face = 0;
		static BOOL s_bLibrary = false;
		if ( !s_bLibrary )
		{
			s_bLibrary = true;
			s_ftLibrary = 0;
			FT_Init_FreeType ( &s_ftLibrary );
			atexit ( Font_DoneLibrary );
		}
		if ( FT_New_Face ( s_ftLibrary, szFile, 0, &face ) != 0 ||
			 FT_Set_Char_Size ( face, 0, nSize << 6, 0, 0 ) != 0 )
		{
			if ( face )
				operator delete ( face );
			return false;
		}

		*lpFont = Font_New ();
		Font_Clear ( *lpFont );
		( *lpFont )->bAntiAlias = ( bAntiAlias == 1 );
		( *lpFont )->lpFace = face;
	}

	( *lpFont )->nSize = GetFontsize(nSize);
	( *lpFont )->nRealSize = nSize;
	strcpy(( *lpFont )->szName, lpFontName);

	return true;
}

C3_CORE_DLL_API
BOOL Font_CreateDbcsSpec ( C3Font **lpFont, WORD wChar, LPCSTR lpFileName )
{
	if ( !lpFont || !lpFileName )
		return false;

	C3Char **lpSlot = &( *lpFont )->lpChar[LOBYTE ( wChar )][HIBYTE ( wChar )];
	if ( *lpSlot && ( *lpSlot )->lpTex )
	{
		( *lpSlot )->lpTex->Release ();
		( *lpSlot )->lpTex = 0;
	}

	*lpSlot = new C3Char;
	if ( *lpSlot )
	{
		if ( FAILED ( D3DXCreateTextureFromFileEx ( g_D3DDevice,
													lpFileName,
													D3DX_DEFAULT,
													D3DX_DEFAULT,
													1,
													0,
													D3DFMT_A4R4G4B4,
													D3DPOOL_MANAGED,
													D3DX_FILTER_POINT,
													D3DX_FILTER_POINT,
													0,
													NULL,
													NULL,
													&( *lpSlot )->lpTex ) ) )
		{
			if ( *lpSlot )
			{
				delete *lpSlot;
				*lpSlot = 0;
			}
		}
	}
	return *lpSlot != 0;
}

C3_CORE_DLL_API
void Font_Release ( C3Font **lpFont )
{
	if (!lpFont || !(*lpFont))
		return;

	FT_Done_Face ( ( FT_Face )( *lpFont )->lpFace );

	for (int j=0; j<256; j++)
	{
		for (int i=0; i<256; i++)
		{
			if ((*lpFont)->lpChar[i][j])
			{
				delete (*lpFont)->lpChar[i][j];
				(*lpFont)->lpChar[i][j] = 0;
			}
		}
	}

	LPDIRECT3DTEXTURE9 *lpTex;
	for ( lpTex = ( *lpFont )->atlases.lpFirst; lpTex && lpTex != ( *lpFont )->atlases.lpLast; lpTex++ )
	{
		if ( *lpTex )
			( *lpTex )->Release ();
	}
	TexList_Free ( &( *lpFont )->atlases );

	SafeDelete(*lpFont);
}

static LPDIRECT3DTEXTURE9								s_lpLastTex = 0;
static BOOL												s_bBatch = false;
static std::map<LPDIRECT3DTEXTURE9, std::vector<FontVertex> >	s_mapBatch;

C3_CORE_DLL_API
void Font_Prepare ( void )
{
	SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
	SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	SetRenderState ( D3DRS_ZENABLE, false );
	SetRenderState ( D3DRS_ZWRITEENABLE, false );
	SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
	SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
	SetRenderState ( D3DRS_DITHERENABLE, false );
	SetRenderState ( D3DRS_LIGHTING, false );
	SetRenderState ( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1 );
	SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );

	SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );

	SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

	g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

	SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	g_D3DDevice->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_NONE );
	g_D3DDevice->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
	g_D3DDevice->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

	s_lpLastTex = 0;
}

C3_CORE_DLL_API
BOOL Font_CheckPixel ( C3Font *lpFont, LPCSTR lpChar, DWORD dwCount )
{
	FT_Face face = ( FT_Face )lpFont->lpFace;
	if ( !face )
		return false;

	DWORD wChar = 0;
	MultiByteToWideChar ( CP_ACP, 0, lpChar, 2, ( LPWSTR )&wChar, 1 );
	FT_UInt nIndex = FT_Get_Char_Index ( face, wChar & 0xFFFF );
	if ( FT_Load_Glyph ( face, nIndex, 0 ) != 0 )
		return false;
	FT_GlyphSlot slot = face->glyph;
	if ( FT_Render_Glyph ( slot, FT_RENDER_MODE_MONO ) != 0 )
		return false;

	FT_Bitmap bitmap = slot->bitmap;
	DWORD dwPixels = 0;
	const BYTE *lpRow = bitmap.buffer;
	for ( int y = 0; y < bitmap.rows; y++ )
	{
		for ( int x = 0; x < bitmap.width; x++ )
		{
			if ( Font_MonoBit ( lpRow, x ) )
				dwPixels++;
		}
		lpRow += bitmap.pitch;
	}
	return dwCount < dwPixels;
}

static LPDIRECT3DTEXTURE9 Font_CreateAtlas ( BOOL bAntiAlias )
{
	LPDIRECT3DTEXTURE9 lpTex = 0;
	D3DXCreateTexture ( g_D3DDevice, 256, 256, 1, 0, bAntiAlias ? D3DFMT_A8R8G8B8 : D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &lpTex );
	return lpTex;
}

static C3Char *Font_LoadChar ( C3Font *lpFont, LPCSTR lpChar, BOOL bDBCS )
{
	FT_Face face = ( FT_Face )lpFont->lpFace;
	DWORD wChar = 0;
	MultiByteToWideChar ( CP_ACP, 0, lpChar, bDBCS ? 2 : 1, ( LPWSTR )&wChar, 1 );
	FT_UInt nIndex = FT_Get_Char_Index ( face, wChar & 0xFFFF );
	if ( FT_Load_Glyph ( face, nIndex, lpFont->bAntiAlias ? FT_LOAD_NO_BITMAP : FT_LOAD_DEFAULT ) != 0 )
		return 0;
	FT_GlyphSlot slot = face->glyph;
	if ( FT_Render_Glyph ( slot, lpFont->bAntiAlias ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO ) != 0 )
		return 0;
	if ( slot->format != FT_GLYPH_FORMAT_BITMAP )
		return 0;
	FT_Bitmap bitmap = slot->bitmap;

	C3Char *lpGlyph = new C3Char;
	lpGlyph->nTop = lpFont->nRealSize - slot->bitmap_top;

	if ( lpFont->atlases.lpFirst == lpFont->atlases.lpLast )
		TexList_PushBack ( &lpFont->atlases, Font_CreateAtlas ( lpFont->bAntiAlias ) );

	if ( lpFont->nCursorX + bitmap.width >= 256 )
	{
		lpFont->nCursorY += lpFont->nRealSize + 2;
		lpFont->nCursorX = 0;
		if ( lpFont->nCursorY + lpFont->nRealSize >= 256 )
		{
			TexList_PushBack ( &lpFont->atlases, Font_CreateAtlas ( lpFont->bAntiAlias ) );
			lpFont->nCursorY = 0;
		}
	}

	LPDIRECT3DTEXTURE9 lpTex = lpFont->atlases.lpLast[-1];
	D3DSURFACE_DESC desc;
	lpTex->GetLevelDesc ( 0, &desc );

	RECT rect;
	rect.left = lpFont->nCursorX;
	rect.top = lpFont->nCursorY;
	rect.right = lpFont->nCursorX + bitmap.width;
	rect.bottom = lpFont->nCursorY + bitmap.rows;

	D3DLOCKED_RECT lock;
	if ( bitmap.width != 0 && bitmap.rows != 0 && SUCCEEDED ( lpTex->LockRect ( 0, &lock, &rect, 0 ) ) )
	{
		BYTE *pDstRow = ( BYTE* )lock.pBits;
		const BYTE *pSrcRow = bitmap.buffer;
		if ( bitmap.pixel_mode == FT_PIXEL_MODE_MONO )
		{
			if ( desc.Format == D3DFMT_A8R8G8B8 || desc.Format == D3DFMT_X8R8G8B8 )
			{
				for ( int y = 0; y < bitmap.rows; y++ )
				{
					DWORD *pDst = ( DWORD* )pDstRow;
					for ( int x = 0; x < bitmap.width; x++ )
						pDst[x] = Font_MonoBit ( pSrcRow, x ) ? 0xFFFFFFFF : 0;
					pDstRow += lock.Pitch;
					pSrcRow += bitmap.pitch;
				}
			}
			else
			if ( desc.Format == D3DFMT_A4R4G4B4 || desc.Format == D3DFMT_A1R5G5B5 || desc.Format == D3DFMT_X1R5G5B5 ||
				 desc.Format == D3DFMT_A8R3G3B2 || desc.Format == D3DFMT_X4R4G4B4 )
			{
				for ( int y = 0; y < bitmap.rows; y++ )
				{
					WORD *pDst = ( WORD* )pDstRow;
					for ( int x = 0; x < bitmap.width; x++ )
						pDst[x] = Font_MonoBit ( pSrcRow, x ) ? 0xFFFF : 0;
					pDstRow += lock.Pitch;
					pSrcRow += bitmap.pitch;
				}
			}
		}
		else
		{
			for ( int y = 0; y < bitmap.rows; y++ )
			{
				for ( int x = 0; x < bitmap.width; x++ )
				{
					BYTE grey = pSrcRow[x];
					if ( desc.Format == D3DFMT_A8R8G8B8 )
					{
						pDstRow[x * 4 + 0] = 0xFF;
						pDstRow[x * 4 + 1] = 0xFF;
						pDstRow[x * 4 + 2] = 0xFF;
						pDstRow[x * 4 + 3] = grey;
					}
					else
					if ( desc.Format == D3DFMT_A4R4G4B4 )
					{
						pDstRow[x * 2 + 0] = 0xFF;
						pDstRow[x * 2 + 1] = grey | 0x0F;
					}
					else
					if ( desc.Format == D3DFMT_A1R5G5B5 || desc.Format == D3DFMT_X4R4G4B4 ||
						 desc.Format == D3DFMT_X1R5G5B5 || desc.Format == D3DFMT_A8R3G3B2 )
					{
						pDstRow[x * 2 + 0] = grey ? 0xFF : 0;
						pDstRow[x * 2 + 1] = grey ? 0xFF : 0;
					}
				}
				pDstRow += lock.Pitch;
				pSrcRow += bitmap.pitch;
			}
		}
		lpTex->UnlockRect ( 0 );
	}

	lpGlyph->lpTex = lpTex;
	lpGlyph->u0 = ( float )lpFont->nCursorX / 256.0f;
	lpGlyph->v0 = ( float )lpFont->nCursorY / 256.0f;
	lpGlyph->u1 = ( float )( lpFont->nCursorX + bitmap.width ) / 256.0f;
	lpGlyph->v1 = ( float )( lpFont->nCursorY + bitmap.rows ) / 256.0f;
	lpFont->nCursorX += bitmap.width + 2;
	return lpGlyph;
}

static FontVertex	s_vec[64];
static DWORD		s_dwCornerColor[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
static const int	s_nOutline[8][2] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }, { -1, -1 }, { 1, -1 }, { -1, 1 }, { 1, 1 } };

static void Font_CopyQuad ( FontVertex *lpDst, const FontVertex *lpSrc, float fX, float fY, DWORD color )
{
	for ( int v = 0; v < 6; v++ )
	{
		lpDst[v] = lpSrc[v];
		lpDst[v].x += fX;
		lpDst[v].y += fY;
		lpDst[v].color = color;
	}
}

C3_CORE_DLL_API
int Font_Draw ( C3Font *lpFont,
				float fX,
				float fY,
				D3DCOLOR color,
				LPCSTR lpChar,
				RENDER_TEXT_STYLE style,
				DWORD color2,
				long lX,
				long lY )
{
	BOOL bDBCS = IsDBCSLeadByte ( lpChar[0] );
	C3Char **lpSlot = bDBCS ?
					  &lpFont->lpChar[( BYTE )lpChar[0]][( BYTE )lpChar[1]] :
					  &lpFont->lpChar[0][( BYTE )lpChar[0]];
	if ( !*lpSlot )
	{
		*lpSlot = Font_LoadChar ( lpFont, lpChar, bDBCS );
		if ( !*lpSlot )
			return 0;
	}
	C3Char *lpGlyph = *lpSlot;

	float fLeft = fX - 0.5f;
	float fTop = ( float )lpGlyph->nTop + fY - 0.5f;
	float fWidth = ( lpGlyph->u1 - lpGlyph->u0 ) * 256.0f;
	float fRight = fLeft + fWidth;
	float fBottom = ( lpGlyph->v1 - lpGlyph->v0 ) * 256.0f + fTop;

	s_vec[0].x = fLeft;		s_vec[0].y = fTop;		s_vec[0].u = lpGlyph->u0;	s_vec[0].v = lpGlyph->v0;
	s_vec[1].x = fLeft;		s_vec[1].y = fBottom;	s_vec[1].u = lpGlyph->u0;	s_vec[1].v = lpGlyph->v1;
	s_vec[2].x = fRight;	s_vec[2].y = fTop;		s_vec[2].u = lpGlyph->u1;	s_vec[2].v = lpGlyph->v0;
	s_vec[3].x = fRight;	s_vec[3].y = fBottom;	s_vec[3].u = lpGlyph->u1;	s_vec[3].v = lpGlyph->v1;
	for ( int v = 0; v < 4; v++ )
	{
		s_vec[v].z = 0;
		s_vec[v].rhw = 1.0f;
		s_vec[v].color = color;
	}
	s_vec[4] = s_vec[2];
	s_vec[5] = s_vec[1];

	int nTriangles;
	switch ( style )
	{
	case RENDER_TEXT_SHADOW:
		Font_CopyQuad ( &s_vec[6], s_vec, 0.0f, 0.0f, color );
		Font_CopyQuad ( s_vec, &s_vec[6], ( float )lX, ( float )lY, color2 );
		nTriangles = 4;
		break;

	case RENDER_TEXT_OUTLINE:
		Font_CopyQuad ( &s_vec[48], s_vec, 0.0f, 0.0f, color );
		for ( int i = 0; i < 8; i++ )
			Font_CopyQuad ( &s_vec[i * 6], &s_vec[48], ( float )s_nOutline[i][0], ( float )s_nOutline[i][1], color2 );
		nTriangles = 18;
		break;

	case RENDER_TEXT_STROKE:
		{
			int sx = lX < 0 ? -1 : 1;
			int sy = lY < 0 ? -1 : 1;
			int adx = lX < 0 ? -lX : lX;
			int ady = lY < 0 ? -lY : lY;
			int n = adx + ady;
			Font_CopyQuad ( &s_vec[n * 6], s_vec, 0.0f, 0.0f, color );
			BOOL bX = adx >= ady;
			if ( n > 0 )
			{
				int ox = sx * adx;
				for ( int k = 0; k < n; k++ )
				{
					int oy = sy * ady;
					Font_CopyQuad ( &s_vec[k * 6], &s_vec[n * 6], ( float )ox, ( float )oy, color2 );
					if ( bX )
					{
						ox -= sx;
						adx--;
						bX = ( ady == 0 );
					}
					else
					{
						ady--;
						bX = ( adx > 0 );
					}
				}
			}
			nTriangles = n * 2 + 2;
		}
		break;

	case RENDER_TEXT_GRADIENT:
		s_vec[0].color = s_dwCornerColor[0];
		s_vec[1].color = s_dwCornerColor[1];
		s_vec[2].color = s_dwCornerColor[2];
		s_vec[3].color = s_dwCornerColor[3];
		s_vec[4].color = s_vec[2].color;
		s_vec[5].color = s_vec[1].color;
		nTriangles = 2;
		break;

	case RENDER_TEXT_NORMAL:
		nTriangles = 2;
		break;

	default:
		return 0;
	}

	if ( !s_bBatch )
	{
		if ( lpGlyph->lpTex != s_lpLastTex )
		{
			g_D3DDevice->SetTexture ( 0, lpGlyph->lpTex );
			s_lpLastTex = lpGlyph->lpTex;
		}
		if ( FAILED ( g_D3DDevice->SetFVF ( FONT_VERTEX ) ) )
			return 0;
		if ( FAILED ( g_D3DDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, nTriangles, s_vec, sizeof ( FontVertex ) ) ) )
			return 0;
		g_dwTriTotal += nTriangles;
	}
	else
	{
		std::vector<FontVertex> &batch = s_mapBatch[lpGlyph->lpTex];
		batch.insert ( batch.end (), s_vec, s_vec + nTriangles * 3 );
	}

	int nAdvance = ( int )fWidth + 1;
	int nHalf = lpFont->nRealSize / 2;
	return ( DWORD )nHalf <= ( DWORD )nAdvance ? nAdvance : nHalf;
}

C3_CORE_DLL_API
void Font_DrawBegin ( void )
{
}

C3_CORE_DLL_API
bool Font_DrawEnd ( void )
{
	return true;
}

C3_CORE_DLL_API
void Font_SetTextCornerColor ( DWORD dwTopLeft, DWORD dwBottomLeft, DWORD dwTopRight, DWORD dwBottomRight )
{
	s_dwCornerColor[0] = dwTopLeft;
	s_dwCornerColor[1] = dwBottomLeft;
	s_dwCornerColor[2] = dwTopRight;
	s_dwCornerColor[3] = dwBottomRight;
}

C3_CORE_DLL_API
BOOL Font_Draw ( C3Font *lpFont,
				 float fX,
				 float fY,
				 D3DCOLOR color,
				 LPCSTR lpChar,
				 const void *lpData )
{
	if ( !lpFont || !lpChar || !lpData )
		return false;

	BOOL bDBCS = IsDBCSLeadByte ( lpChar[0] );

	C3Char **lpSlot = bDBCS ?
					  &lpFont->lpChar[( BYTE )lpChar[0]][( BYTE )lpChar[1]] :
					  &lpFont->lpChar[0][( BYTE )lpChar[0]];
	if ( !*lpSlot )
	{
		*lpSlot = new C3Char;
		if ( FAILED ( D3DXCreateTexture ( g_D3DDevice,
										  GetFontsize(lpFont->nSize),
										  GetFontsize(lpFont->nSize),
										  1,
										  0,
										  D3DFMT_A4R4G4B4,
										  D3DPOOL_MANAGED,
										  &( *lpSlot )->lpTex ) ) )
			return false;

		D3DLOCKED_RECT lock;
		( *lpSlot )->lpTex->LockRect ( 0, &lock, 0, 0 );

		BYTE *pDstRow = ( BYTE* )lock.pBits;
		WORD *pDst16;
		const BYTE *pSrc = ( const BYTE* )lpData;

		BYTE alpha;
		int nInk = 0;
		for ( int y = 0; y < lpFont->nSize; y++ )
		{
			pDst16 = ( WORD* )pDstRow;
			for ( int x = 0; x < lpFont->nSize; x++ )
			{
				int n = y * lpFont->nSize + x;
				if ( n & 1 )
					alpha = pSrc[n >> 1] >> 4;
				else
					alpha = pSrc[n >> 1] & 0x0f;
				if ( alpha > 0 )
				{
					*pDst16++ = ( alpha << 12 ) | 0x0fff;
					nInk++;
				}
				else
				{
					*pDst16++ = 0x0000;
				}
			}
			pDstRow += lock.Pitch;
		}
		( *lpSlot )->lpTex->UnlockRect ( 0 );
	}

	D3DSURFACE_DESC desc;
	( *lpSlot )->lpTex->GetLevelDesc ( 0, &desc );

	static FontVertex vec[4];

	vec[0].x = fX;
	vec[0].y = fY;
	vec[0].z = 0;
	vec[0].rhw = 1.0f;
	vec[0].color = color;
	vec[0].u = 0.0f;
	vec[0].v = 0.0f;

	vec[1].x = fX;
	vec[1].y = fY + ( float )lpFont->nSize;
	vec[1].z = 0;
	vec[1].rhw = 1.0f;
	vec[1].color = color;
	vec[1].u = 0.0f;
	vec[1].v = ( float )lpFont->nSize / ( float )desc.Height;

	vec[2].x = ( !bDBCS )?
			   fX + ( float )lpFont->nSize / 2 :
			   fX + ( float )lpFont->nSize;
	vec[2].y = fY;
	vec[2].z = 0;
	vec[2].rhw = 1.0f;
	vec[2].color = color;
	vec[2].u = ( !bDBCS )?
			   ( float )lpFont->nSize / ( float )desc.Width / 2 :
			   ( float )lpFont->nSize / ( float )desc.Width;
	vec[2].v = 0.0f;

	vec[3].x = ( !bDBCS )?
			   fX + ( float )lpFont->nSize / 2 :
			   fX + ( float )lpFont->nSize;
	vec[3].y = fY + ( float )lpFont->nSize;
	vec[3].z = 0;
	vec[3].rhw = 1.0f;
	vec[3].color = color;
	vec[3].u = ( !bDBCS )?
			   ( float )lpFont->nSize / ( float )desc.Width / 2 :
			   ( float )lpFont->nSize / ( float )desc.Width;
	vec[3].v = ( float )lpFont->nSize / ( float )desc.Height;

	if ( ( *lpSlot )->lpTex != s_lpLastTex )
	{
		g_D3DDevice->SetTexture ( 0, ( *lpSlot )->lpTex );
		s_lpLastTex = ( *lpSlot )->lpTex;
	}

	if ( FAILED ( g_D3DDevice->SetFVF ( FONT_VERTEX ) ) )
		return false;

	if ( FAILED ( g_D3DDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP,
											     2,
											     vec,
											     sizeof ( FontVertex ) ) ) )
		return false;

	g_dwTriTotal += 2;
	return true;
}

struct TQFontHeader
{
	char	szTag[8];
	DWORD	dwVersion;
	DWORD	dwChars;
	int		nSize;

	TQFontHeader ()
	{
		memset ( szTag, 0, sizeof ( szTag ) );
		strcpy ( szTag, "TQFONT" );
		dwVersion = 0x1000;
		dwChars = 0;
		nSize = 16;
	}
};

static BOOL Font_WriteChar ( C3Font *lpFont, FILE *file, const BYTE *lpChar )
{
	if ( !lpFont || !file || !lpChar )
		return false;

	FT_Face face = ( FT_Face )lpFont->lpFace;
	DWORD wChar = 0;
	MultiByteToWideChar ( CP_ACP, 0, ( LPCSTR )lpChar, 2, ( LPWSTR )&wChar, 1 );
	FT_UInt nIndex = FT_Get_Char_Index ( face, wChar & 0xFFFF );
	if ( FT_Load_Glyph ( face, nIndex, 0 ) != 0 )
		return false;
	FT_GlyphSlot slot = face->glyph;
	if ( FT_Render_Glyph ( slot, FT_RENDER_MODE_MONO ) != 0 )
		return false;
	FT_Bitmap bitmap = slot->bitmap;

	int nBytes = lpFont->nSize * lpFont->nSize / 2;
	BYTE *lpPack = new BYTE[nBytes];
	memset ( lpPack, 0, nBytes );

	int nInk = 0;
	BYTE *lpRow = lpPack;
	const BYTE *lpSrc = bitmap.buffer;
	for ( int y = 0; y < bitmap.rows; y++, lpSrc += bitmap.pitch )
	{
		for ( int x = 0; x < bitmap.width; x++ )
		{
			if ( Font_MonoBit ( lpSrc, x ) )
			{
				nInk++;
				lpRow[x / 2] |= ( x & 1 ) ? 0xF0 : 0x0F;
			}
		}
		lpRow += bitmap.width / 2;
	}

	if ( lpChar[0] != ' ' && nInk <= 0 )
		return false;

	fwrite ( lpPack, 1, nBytes, file );
	delete [] lpPack;
	return true;
}

C3_CORE_DLL_API
void Font_Save ( C3Font *lpFont, LPCSTR lpFileName )
{
	if ( !lpFont || !lpFileName )
		return;

	std::map<int, long> mapOffset;

	FILE *file = fopen ( lpFileName, "wb" );
	if ( !file )
		return;

	TQFontHeader header;
	header.nSize = lpFont->nSize;
	fwrite ( &header, sizeof ( header ), 1, file );

	BYTE code[2];
	for ( int c = 0; c <= 0xA0; c++ )
	{
		long nOffset = ftell ( file );
		code[0] = ( BYTE )c;
		code[1] = 0;
		if ( Font_WriteChar ( lpFont, file, code ) )
			mapOffset[c] = nOffset;
	}

	for ( int lead = 0x81; lead <= 0xFE; lead++ )
	{
		for ( int trail = 0x40; trail <= 0xFE; trail++ )
		{
			if ( trail == 0x7F )
				continue;
			long nOffset = ftell ( file );
			code[0] = ( BYTE )lead;
			code[1] = ( BYTE )trail;
			if ( Font_WriteChar ( lpFont, file, code ) )
				mapOffset[lead + ( trail << 8 )] = nOffset;
		}
	}

	for ( std::map<int, long>::iterator it = mapOffset.begin (); it != mapOffset.end (); ++it )
	{
		int nCode = it->first;
		long nOffset = it->second;
		fwrite ( &nCode, sizeof ( int ), 1, file );
		fwrite ( &nOffset, sizeof ( long ), 1, file );
	}

	fseek ( file, 0, SEEK_SET );
	header.dwChars = ( DWORD )mapOffset.size ();
	fwrite ( &header, sizeof ( header ), 1, file );
	fclose ( file );
}
