#pragma warning(disable:4786)
#include "c3_texture.h"
#include "c3_main.h"
#include "c3_datafile.h"

C3_CORE_DLL_API DWORD		g_dwTexCount = 0;
C3_CORE_DLL_API C3Texture	*g_lpTex[TEX_MAX];

static CRITICAL_SECTION		g_CriticalSection;
static struct CriticalSectionInit
{
	CriticalSectionInit ()	{ InitializeCriticalSection ( &g_CriticalSection ); }
	~CriticalSectionInit ()	{ DeleteCriticalSection ( &g_CriticalSection ); }
} g_CriticalSectionInit;

C3_CORE_DLL_API
void Texture_Clear ( C3Texture *lpTex )
{
	lpTex->nID = -1;
	lpTex->nDupCount = 0;
	lpTex->lpName = 0;
	lpTex->lpTex = 0;
	ZeroMemory ( &lpTex->Info, sizeof ( D3DXIMAGE_INFO ) );
}
C3_CORE_DLL_API
int Texture_Load ( C3Texture **lpTex,
				   const char *lpName,
				   DWORD dwMipLevels,
				   D3DPOOL pool,
				   BOOL bDuplicate,
				   D3DCOLOR colorkey,
				   DWORD dwFilter )
{
	EnterCriticalSection ( &g_CriticalSection );

	if ( bDuplicate )
	{
		DWORD add = 0;
		for ( int t = 0; t < TEX_MAX; t++ )
		{
			if ( add >= g_dwTexCount )
				break;
			if ( 0 != g_lpTex[t] )
			{
				if ( stricmp ( g_lpTex[t]->lpName, lpName ) == 0 )
				{
					g_lpTex[t]->nDupCount++;
					*lpTex = g_lpTex[t];

					LeaveCriticalSection ( &g_CriticalSection );
					return t;
				}
				add++;
			}
		}
	}
	
	*lpTex = new C3Texture;
	Texture_Clear ( *lpTex );
	DWORD dwSize = 0;
	void* pBuf = NULL;
	if ( !DataFile_UnpackSearch ( lpName ) )
	{
		if ( !DataFile_Load ( lpName, pBuf, dwSize ) )
		{
			LeaveCriticalSection ( &g_CriticalSection );
			return -1;
		}
		if ( FAILED ( D3DXCreateTextureFromFileInMemoryEx ( g_D3DDevice,
															pBuf,
															dwSize,
															D3DX_DEFAULT,
															D3DX_DEFAULT,
															dwMipLevels,
															0,
															D3DFMT_UNKNOWN,
															pool,
															dwFilter,
															dwFilter,
															colorkey,
															&( *lpTex )->Info,
															NULL,
															&( *lpTex )->lpTex ) ) )
		{
			DataFile_Free ( pBuf );
			LeaveCriticalSection ( &g_CriticalSection );
			return -1;
		}
		DataFile_Free ( pBuf );
		D3DSURFACE_DESC desc;
		( *lpTex )->lpTex->GetLevelDesc ( 0, &desc );
		( *lpTex )->Info.Format = desc.Format;

		( *lpTex )->lpName = new char[strlen ( lpName ) + 1];
		strcpy ( ( *lpTex )->lpName, lpName );

		( *lpTex )->nDupCount = 1;
		g_dwTexCount++;

		for ( int t = 0; t < TEX_MAX; t++ )
		{
			if ( g_lpTex[t] == 0 )
			{
				g_lpTex[t] = *lpTex;
				g_lpTex[t]->nID = t;
				LeaveCriticalSection ( &g_CriticalSection );
				return t;
			}
		}
	}
	else
	{
		if ( FAILED ( D3DXCreateTextureFromFileEx ( g_D3DDevice,
													lpName,
													D3DX_DEFAULT,
													D3DX_DEFAULT,
													dwMipLevels,
													0,
													D3DFMT_UNKNOWN,
													pool,
													dwFilter,
													dwFilter,
													colorkey,
													&( *lpTex )->Info,
													NULL,
													&( *lpTex )->lpTex ) ) )
		{
			LeaveCriticalSection ( &g_CriticalSection );
			return -1;
		}

		D3DSURFACE_DESC desc;
		( *lpTex )->lpTex->GetLevelDesc ( 0, &desc );
		( *lpTex )->Info.Format = desc.Format;

		( *lpTex )->lpName = new char[strlen ( lpName ) + 1];
		strcpy ( ( *lpTex )->lpName, lpName );

		( *lpTex )->nDupCount = 1;
		g_dwTexCount++;

		for ( int t = 0; t < TEX_MAX; t++ )
		{
			if ( g_lpTex[t] == 0 )
			{
				g_lpTex[t] = *lpTex;
				g_lpTex[t]->nID = t;
				LeaveCriticalSection ( &g_CriticalSection );
				return t;
			}
		}
	}
	LeaveCriticalSection ( &g_CriticalSection );
	return -1;
}

C3_CORE_DLL_API
void Texture_Unload ( C3Texture **lpTex )
{
	if ( --( *lpTex )->nDupCount <= 0 )
	{
		EnterCriticalSection ( &g_CriticalSection );

		g_lpTex[( *lpTex )->nID] = 0;

		SafeDeleteEx ( ( *lpTex )->lpName );
		SafeRelease ( ( *lpTex )->lpTex );
		SafeDelete ( *lpTex );

		g_dwTexCount--;

		LeaveCriticalSection ( &g_CriticalSection );
	}
}
C3_CORE_DLL_API
BOOL Texture_Update ( C3Texture *lpSrcTex, C3Texture *lpDestTex )
{
	if ( FAILED ( g_D3DDevice->UpdateTexture ( lpSrcTex->lpTex, lpDestTex->lpTex ) ) )
		return false;
	return true;
}
struct CTextureSectionLock
{
	CTextureSectionLock ( CRITICAL_SECTION *pcs ) : m_pcs ( pcs )	{ EnterCriticalSection ( m_pcs ); }
	~CTextureSectionLock ()											{ LeaveCriticalSection ( m_pcs ); }
	CRITICAL_SECTION	*m_pcs;
};
C3_CORE_DLL_API
BOOL Texture_Create ( C3Texture **lpTex,
					  DWORD dwWidth,
					  DWORD dwHeight,
					  DWORD dwMipLevels,
					  D3DFORMAT format,
					  D3DPOOL pool,
					  DWORD dwUsage,
					  BOOL bAdd )
{
	CTextureSectionLock lock ( &g_CriticalSection );
	*lpTex = new C3Texture;
	Texture_Clear ( *lpTex );

	if ( FAILED ( D3DXCreateTexture ( g_D3DDevice,
									  dwWidth,
									  dwHeight,
									  dwMipLevels,
									  dwUsage,
									  format,
									  pool,
									  &( *lpTex )->lpTex ) ) )
		return false;

	( *lpTex )->Info.Width = dwWidth;
	( *lpTex )->Info.Height = dwHeight;
	( *lpTex )->Info.Format = format;

	if ( bAdd )
	{
		int t;
		for ( t = 0; t < TEX_MAX; t++ )
		{
			if ( g_lpTex[t] == 0 )
			{
				g_lpTex[t] = *lpTex;
				( *lpTex )->nID = t;
				break;
			}
		}
		if ( t == TEX_MAX )
			return false;
	}

	return true;
}

C3_CORE_DLL_API
BOOL Texture_LockRect ( C3Texture *lpTex, RECT rect, D3DLOCKED_RECT &lockedRect )
{
	if ( lpTex->Info.Format != D3DFMT_A8R8G8B8 && lpTex->Info.Format != D3DFMT_X8R8G8B8 )
		return FALSE;
	return lpTex->lpTex->LockRect ( 0, &lockedRect, &rect, 0 ) == D3D_OK;
}

C3_CORE_DLL_API
void Texture_Unlock ( C3Texture *lpTex )
{
	lpTex->lpTex->UnlockRect ( 0 );
}
