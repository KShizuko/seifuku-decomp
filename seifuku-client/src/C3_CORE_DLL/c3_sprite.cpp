#include "c3_sprite.h"
#include "c3_main.h"

C3_CORE_DLL_API
void Sprite_Clear ( C3Sprite *lpSprite )
{
	for ( int v = 0; v < 4; v++ )
	{
		ZeroMemory ( &lpSprite->vertex[v], sizeof ( SpriteVertex ) );

		lpSprite->vertex[v].rhw = 1.0f;
		lpSprite->vertex[v].color = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
	}
	lpSprite->vertex[0].u = 0.0f;
	lpSprite->vertex[0].v = 0.0f;

	lpSprite->vertex[1].u = 0.0f;
	lpSprite->vertex[1].v = 1.0f;

	lpSprite->vertex[2].u = 1.0f;
	lpSprite->vertex[2].v = 0.0f;

	lpSprite->vertex[3].u = 1.0f;
	lpSprite->vertex[3].v = 1.0f;

	lpSprite->lpTex = 0;
}
C3_CORE_DLL_API
BOOL Sprite_Load ( C3Sprite **lpSprite,
				   const char *lpName,
				   D3DPOOL pool,
				   BOOL bDuplicate,
				   D3DCOLOR colorkey )
{
	*lpSprite = new C3Sprite;
	Sprite_Clear ( *lpSprite );

	if ( Texture_Load ( &( *lpSprite )->lpTex, lpName, 1, pool, bDuplicate, colorkey, D3DX_FILTER_POINT ) == -1 )
		return false;

	Sprite_SetCoor ( *lpSprite, 0, 0, 0 );

	return true;
}
C3_CORE_DLL_API
BOOL Sprite_Create ( C3Sprite **lpSprite,
				     DWORD dwWidth,
					 DWORD dwHeight,
					 DWORD dwMipLevels,
					 D3DFORMAT format,
					 D3DPOOL pool )
{
	*lpSprite = new C3Sprite;
	Sprite_Clear ( *lpSprite );

	if ( !Texture_Create ( &( *lpSprite )->lpTex, dwWidth, dwHeight, dwMipLevels, format, pool, 0, false ) )
		return false;
	return true;
}
C3_CORE_DLL_API
void Sprite_Unload ( C3Sprite **lpSprite )
{
	if ( !( *lpSprite ) )
		return;
	Texture_Unload ( &( *lpSprite )->lpTex );
	SafeDelete ( *lpSprite );
}

C3_CORE_DLL_API
void Sprite_Mirror ( C3Sprite *lpSprite )
{
	float u, v;

	u = lpSprite->vertex[0].u;
	v = lpSprite->vertex[0].v;

	lpSprite->vertex[0].u = lpSprite->vertex[2].u;
	lpSprite->vertex[0].v = lpSprite->vertex[2].v;

	lpSprite->vertex[2].u = u;
	lpSprite->vertex[2].v = v;

	u = lpSprite->vertex[1].u;
	v = lpSprite->vertex[1].v;

	lpSprite->vertex[1].u = lpSprite->vertex[3].u;
	lpSprite->vertex[1].v = lpSprite->vertex[3].v;

	lpSprite->vertex[3].u = u;
	lpSprite->vertex[3].v = v;
}

C3_CORE_DLL_API
void Sprite_Rotate ( C3Sprite *lpSprite, int nAngle )
{
	if ( !lpSprite )
		return;

	float x = ( lpSprite->vertex[2].x - lpSprite->vertex[0].x ) / 2.0f + lpSprite->vertex[0].x;
	float y = ( lpSprite->vertex[1].y - lpSprite->vertex[0].y ) / 2.0f + lpSprite->vertex[0].y;

	D3DXMATRIX matRotate, matTranslate;
	D3DXMatrixRotationZ ( &matRotate, ( float )( nAngle % 360 ) * 0.01745329f );
	D3DXMatrixTranslation ( &matTranslate, x, y, 0 );
	D3DXMATRIX matrix;
	D3DXMatrixMultiply ( &matrix, &matRotate, &matTranslate );

	for ( int v = 0; v < 4; v++ )
	{
		D3DXVECTOR2 vec;
		vec.x = lpSprite->vertex[v].x - x;
		vec.y = lpSprite->vertex[v].y - y;
		D3DXVec2TransformCoord ( &vec, &vec, &matrix );
		lpSprite->vertex[v].x = vec.x;
		lpSprite->vertex[v].y = vec.y;
	}
}
C3_CORE_DLL_API
void Sprite_SetCoor ( C3Sprite *lpSprite,
					  RECT *lpSrc,
					  int nX,
					  int nY,
					  DWORD dwWidth,
					  DWORD dwHeight )
{
	float uu = 1.0f / ( float )lpSprite->lpTex->Info.Width;
	float vv = 1.0f / ( float )lpSprite->lpTex->Info.Height;

	uu = 0;
	vv = 0;

	if ( !lpSrc )
	{
		lpSprite->vertex[0].u = 0.0f + uu;
		lpSprite->vertex[0].v = 0.0f + vv;

		lpSprite->vertex[1].u = 0.0f + uu;
		lpSprite->vertex[1].v = 1.0f - vv;

		lpSprite->vertex[2].u = 1.0f - uu;
		lpSprite->vertex[2].v = 0.0f + vv;

		lpSprite->vertex[3].u = 1.0f - uu;
		lpSprite->vertex[3].v = 1.0f - vv;
	}
	else
	{
		lpSprite->vertex[0].u = ( float )lpSrc->left / ( float )lpSprite->lpTex->Info.Width + uu;
		lpSprite->vertex[0].v = ( float )lpSrc->top / ( float )lpSprite->lpTex->Info.Height + vv;

		lpSprite->vertex[1].u = lpSprite->vertex[0].u;
		lpSprite->vertex[1].v = ( float )lpSrc->bottom / ( float )lpSprite->lpTex->Info.Height - vv;

		lpSprite->vertex[2].u = ( float )lpSrc->right / ( float )lpSprite->lpTex->Info.Width - uu;
		lpSprite->vertex[2].v = lpSprite->vertex[0].v;

		lpSprite->vertex[3].u = lpSprite->vertex[2].u;
		lpSprite->vertex[3].v = lpSprite->vertex[1].v;
	}
	float x = ( float )nX - 0.5f;
	float y = ( float )nY - 0.5f;
	lpSprite->vertex[0].x = x;
	lpSprite->vertex[0].y = y;

	lpSprite->vertex[1].x = x;
	lpSprite->vertex[1].y = ( float )( int )( dwHeight == 0 ? lpSprite->lpTex->Info.Height : dwHeight ) + y;

	lpSprite->vertex[2].x = ( float )( int )( dwWidth == 0 ? lpSprite->lpTex->Info.Width : dwWidth ) + x;
	lpSprite->vertex[2].y = y;

	lpSprite->vertex[3].x = ( float )( nX + ( int )( dwWidth == 0 ? lpSprite->lpTex->Info.Width : dwWidth ) ) - 0.5f;
	lpSprite->vertex[3].y = ( float )( nY + ( int )( dwHeight == 0 ? lpSprite->lpTex->Info.Height : dwHeight ) ) - 0.5f;
}
C3_CORE_DLL_API
void Sprite_SetColor ( C3Sprite *lpSprite, BYTE a, BYTE r, BYTE g, BYTE b )
{
	for ( int v = 0; v < 4; v++ )
		lpSprite->vertex[v].color = D3DCOLOR_ARGB ( a, r, g, b );
}
C3_CORE_DLL_API
void Sprite_Prepare ( void )
{
	SetRenderState ( D3DRS_ZENABLE, false );
	SetRenderState ( D3DRS_ZWRITEENABLE, false );
	SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_FLAT );
	SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
	SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
	SetRenderState ( D3DRS_DITHERENABLE, false );
	SetRenderState ( D3DRS_LIGHTING, false );
	SetRenderState ( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1 );
	SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );

	SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

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
}
C3_CORE_DLL_API
BOOL Sprite_Draw ( C3Sprite *lpSprite, DWORD dwShowWay )
{
	switch ( dwShowWay )
	{
	case 0:
		if ( lpSprite->lpTex->Info.Format == D3DFMT_A8R8G8B8 ||
			 lpSprite->lpTex->Info.Format == D3DFMT_A1R5G5B5 ||
			 lpSprite->lpTex->Info.Format == D3DFMT_A4R4G4B4 ||
			 lpSprite->lpTex->Info.Format == D3DFMT_A8 ||
			 lpSprite->lpTex->Info.Format == D3DFMT_A8R3G3B2 ||
			 lpSprite->lpTex->Info.Format == D3DFMT_DXT3 ||
			 ( lpSprite->vertex[0].color >> 24 ) < 255 )
		{
			SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
			SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			SetRenderState ( D3DRS_ALPHATESTENABLE, true );
			SetRenderState ( D3DRS_ALPHAREF, 0x00 );
			SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}
		else
		{
			SetRenderState ( D3DRS_ALPHABLENDENABLE, false );
			SetRenderState ( D3DRS_ALPHATESTENABLE, false );
		}
		break;
	case 1:
		if ( lpSprite->lpTex->Info.Format == D3DFMT_A8R8G8B8 ||
			 lpSprite->lpTex->Info.Format == D3DFMT_A1R5G5B5 ||
			 lpSprite->lpTex->Info.Format == D3DFMT_A4R4G4B4 ||
			 lpSprite->lpTex->Info.Format == D3DFMT_A8 ||
			 lpSprite->lpTex->Info.Format == D3DFMT_A8R3G3B2 ||
			 lpSprite->lpTex->Info.Format == D3DFMT_DXT3 ||
			 ( lpSprite->vertex[0].color >> 24 ) < 255 )
		{
			SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
			SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_ONE );
			SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_ONE );

			SetRenderState ( D3DRS_ALPHATESTENABLE, true );
			SetRenderState ( D3DRS_ALPHAREF, 0x00 );
			SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}
		else
		{
			SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
			SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
			SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_ONE);

			SetRenderState ( D3DRS_ALPHATESTENABLE, false );
		}
		break;
	};

	if ( FAILED ( g_D3DDevice->SetTexture ( 0, lpSprite->lpTex->lpTex ) ) )
		return false;

	if ( FAILED ( g_D3DDevice->SetFVF ( SPRITE_VERTEX ) ) )
		return false;
	if ( FAILED ( g_D3DDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP,
											     2,
											     lpSprite->vertex,
											     sizeof ( SpriteVertex ) ) ) )
		return false;
	return true;
}

C3_CORE_DLL_API
BOOL Sprite_DrawEx ( C3Sprite *lpSprite, DWORD dwSrcBlend, DWORD dwDestBlend )
{
	SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
	SetRenderState ( D3DRS_SRCBLEND, dwSrcBlend );
	SetRenderState ( D3DRS_DESTBLEND, dwDestBlend );
	SetRenderState ( D3DRS_ALPHATESTENABLE, true );
	SetRenderState ( D3DRS_ALPHAREF, 0x08 );
	SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	if ( FAILED ( g_D3DDevice->SetTexture ( 0, lpSprite->lpTex->lpTex ) ) )
		return false;
	if ( FAILED ( g_D3DDevice->SetFVF ( SPRITE_VERTEX ) ) )
		return false;
	return SUCCEEDED ( g_D3DDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP,
													  2,
													  lpSprite->vertex,
													  sizeof ( SpriteVertex ) ) );
}

C3_CORE_DLL_API
BOOL Sprite_Lock ( C3Sprite *lpSprite, RECT *lpRect, D3DLOCKED_RECT *lpReturn )
{
	return lpSprite->lpTex->lpTex->LockRect ( 0, lpReturn, lpRect, 0 ) == D3D_OK;
}
C3_CORE_DLL_API
BOOL Sprite_Unlock ( C3Sprite *lpSprite )
{
	return lpSprite->lpTex->lpTex->UnlockRect ( 0 ) == D3D_OK;
}
