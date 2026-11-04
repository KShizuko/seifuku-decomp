#include "c3_scene.h"
#include "c3_main.h"
#include "c3_camera.h"
#include "c3_omni.h"

C3_CORE_DLL_API
void Scene_Clear ( C3Scene *lpScene )
{
	lpScene->lpName = 0;

	lpScene->dwVecCount = 0;
	lpScene->lpVB = 0;
	lpScene->dwTriCount = 0;
	lpScene->lpIB = 0;

	lpScene->lpTexName = 0;
	lpScene->nTex = -1;

	lpScene->lplTexName = 0;
	lpScene->nlTex = -1;

	lpScene->dwFrameCount = 0;
	lpScene->lpFrame = 0;
	lpScene->nFrame = 0;

	D3DXMatrixIdentity ( &lpScene->matrix );
}
C3_CORE_DLL_API
BOOL Scene_Load ( C3Scene **lpScene,
				  char *lpName,
				  DWORD dwIndex )
{
	FILE *file = fopen ( lpName, "rb" );
	if ( !file )
		return false;

	*lpScene = new C3Scene;
	Scene_Clear ( *lpScene );

	ChunkHeader chunk;
	DWORD add = 0;

	while ( 1 )
	{
		fread ( &chunk, sizeof ( ChunkHeader ), 1, file );
		if ( feof ( file ) )
		{
			fclose ( file );
			return false;
		}
		if ( chunk.byChunkID[0] == 'S' &&
			 chunk.byChunkID[1] == 'C' &&
			 chunk.byChunkID[2] == 'E' &&
			 chunk.byChunkID[3] == 'N' )
		{
			if ( add < dwIndex )
			{
				add++;
				fseek ( file, chunk.dwChunkSize, SEEK_CUR );
				continue;
			}

			DWORD temp;
			fread ( &temp, sizeof ( unsigned long ), 1, file );
			( *lpScene )->lpName = new char[temp + 1];
			fread ( ( *lpScene )->lpName, 1, temp, file );
			( *lpScene )->lpName[temp] = '\0';

			fread ( &( *lpScene )->dwVecCount, sizeof ( DWORD ), 1, file );
			( *lpScene )->lpVB = new SceneVertex[( *lpScene )->dwVecCount];
			fread ( ( *lpScene )->lpVB,
					sizeof ( SceneVertex ),
					( *lpScene )->dwVecCount,
					file );

			fread ( &( *lpScene )->dwTriCount, sizeof ( DWORD ), 1, file );
			( *lpScene )->lpIB = new WORD[( *lpScene )->dwTriCount * 3];
			fread ( ( *lpScene )->lpIB,
					sizeof ( WORD ),
					( *lpScene )->dwTriCount * 3,
					file );

			fread ( &temp, sizeof ( DWORD ), 1, file );
			( *lpScene )->lpTexName = new char[temp + 1];
			fread ( ( *lpScene )->lpTexName, 1, temp, file );
			( *lpScene )->lpTexName[temp] = '\0';
			C3Texture *tex;
			( *lpScene )->nTex = Texture_Load ( &tex, ( *lpScene )->lpTexName );
			if ( ( *lpScene )->nTex == -1 )
				return false;

			fread ( &temp, sizeof ( DWORD ), 1, file );
			if ( temp > 0 )
			{
				( *lpScene )->lplTexName = new char[temp + 1];
				fread ( ( *lpScene )->lplTexName, 1, temp, file );
				( *lpScene )->lplTexName[temp] = '\0';
				C3Texture *ligtex;
				ligtex = new C3Texture;
				( *lpScene )->nlTex = Texture_Load ( &ligtex, ( *lpScene )->lplTexName );
				if ( ( *lpScene )->nlTex == -1 )
					return false;
			}
			fread ( &( *lpScene )->dwFrameCount, sizeof ( DWORD ), 1, file );
			( *lpScene )->lpFrame = new D3DXMATRIX[( *lpScene )->dwFrameCount];
			fread ( ( *lpScene )->lpFrame, sizeof ( D3DXMATRIX ), ( *lpScene )->dwFrameCount, file );
			break;
		}
		else
			fseek ( file, chunk.dwChunkSize, SEEK_CUR );
	};
	fclose ( file );
	return true;
}
C3_CORE_DLL_API
BOOL Scene_Save ( char *lpName, C3Scene *lpScene, BOOL bNew )
{
	FILE *file = fopen ( lpName, bNew ? "w+b" : "r+b" );
	if ( !file )
		return false;
	fseek ( file, 0, SEEK_END );

	ChunkHeader chunk;
	chunk.byChunkID[0] = 'S';
	chunk.byChunkID[1] = 'C';
	chunk.byChunkID[2] = 'E';
	chunk.byChunkID[3] = 'N';
	chunk.dwChunkSize = 0;
	fwrite ( &chunk, sizeof ( chunk ), 1, file );

	DWORD length = strlen ( lpScene->lpName );
	fwrite ( &length, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( lpScene->lpName, sizeof ( char ), length, file );
	chunk.dwChunkSize += sizeof ( char ) * length;

	fwrite ( &lpScene->dwVecCount, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );
	fwrite ( lpScene->lpVB, sizeof ( SceneVertex ), lpScene->dwVecCount, file );
	chunk.dwChunkSize += sizeof ( SceneVertex ) * lpScene->dwVecCount;

	fwrite ( &lpScene->dwTriCount, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );
	fwrite ( lpScene->lpIB, sizeof ( WORD ), lpScene->dwTriCount * 3, file );
	chunk.dwChunkSize += sizeof ( WORD ) * lpScene->dwTriCount * 3;

	length = strlen ( lpScene->lpTexName );
	fwrite ( &length, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( lpScene->lpTexName, sizeof ( char ), length, file );
	chunk.dwChunkSize += sizeof ( char ) * length;

	if ( lpScene->lplTexName )
		length = strlen ( lpScene->lplTexName );
	else
		length = 0;
	fwrite ( &length, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	if ( length > 0 )
	{
		fwrite ( lpScene->lplTexName, sizeof ( char ), length, file );
		chunk.dwChunkSize += sizeof ( char ) * length;
	}

	fwrite ( &lpScene->dwFrameCount, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );
	fwrite ( lpScene->lpFrame, sizeof ( D3DXMATRIX ), lpScene->dwFrameCount, file );
	chunk.dwChunkSize += sizeof ( D3DXMATRIX ) * lpScene->dwFrameCount;

	fseek ( file, -( int )( chunk.dwChunkSize + sizeof ( chunk ) ), SEEK_CUR );
	fwrite ( &chunk, sizeof ( chunk ), 1, file );
	fseek ( file, 0, SEEK_END );

	fclose ( file );
	return true;
}
C3_CORE_DLL_API
void Scene_Unload ( C3Scene **lpScene )
{
	SafeDeleteEx ( ( *lpScene )->lpName );
	SafeDeleteEx ( ( *lpScene )->lpVB );
	SafeDeleteEx ( ( *lpScene )->lpIB );

	SafeDeleteEx ( ( *lpScene )->lpTexName );
	if ( ( *lpScene )->nTex > -1 )
		Texture_Unload ( &g_lpTex[( *lpScene )->nTex] );

	SafeDelete ( *lpScene );
}
C3_CORE_DLL_API
BOOL Scene_Optimize ( C3Scene *lpScene )
{
	SceneVertex *vertex = new SceneVertex[lpScene->dwVecCount];
	DWORD veccount = 0;
	WORD *tri = new WORD[lpScene->dwTriCount * 3];

	int index;
	for ( DWORD f = 0; f < lpScene->dwTriCount; f++ )
	{
		for ( DWORD i = 0; i < 3; i++ )
		{
			index = lpScene->lpIB[f * 3 + i];

			DWORD c;
			for ( c = 0; c < veccount; c++ )
			{
				if ( memcmp ( &lpScene->lpVB[index],
							  &vertex[c],
							  sizeof ( SceneVertex ) ) == 0 )
					break;
			}
			if ( c == veccount )
			{
				CopyMemory ( &vertex[veccount],
							 &lpScene->lpVB[index],
							 sizeof ( SceneVertex ) );
				tri[f * 3 + i] = ( WORD )veccount;

				veccount++;
			}
			else
			{
				tri[f * 3 + i] = ( WORD )c;
			}
		}
	}

	SafeDeleteEx ( lpScene->lpVB );

	lpScene->dwVecCount = veccount;
	lpScene->lpVB = new SceneVertex[lpScene->dwVecCount];
	CopyMemory ( lpScene->lpVB, vertex, sizeof ( SceneVertex ) * veccount );

	CopyMemory ( lpScene->lpIB, tri, sizeof ( WORD ) * lpScene->dwTriCount * 3 );

	SafeDeleteEx ( vertex );
	SafeDeleteEx ( tri );
	return true;
}

C3_CORE_DLL_API
void Scene_Prepare ( void )
{
	SetRenderState ( D3DRS_ZENABLE, true );
	SetRenderState ( D3DRS_ZWRITEENABLE, true );
	SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_FLAT );
	SetRenderState ( D3DRS_DITHERENABLE, true );
	SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );
	SetRenderState ( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR2 );
	SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
	g_D3DDevice->SetSoftwareVertexProcessing ( false );

	SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

	SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
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

	g_D3DDevice->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_D3DDevice->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	g_D3DDevice->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
}
C3_CORE_DLL_API
BOOL Scene_Draw ( C3Scene *lpScene )
{
	if ( g_lpTex[lpScene->nTex]->Info.Format == D3DFMT_A8R8G8B8 ||
		 g_lpTex[lpScene->nTex]->Info.Format == D3DFMT_A1R5G5B5 ||
		 g_lpTex[lpScene->nTex]->Info.Format == D3DFMT_A4R4G4B4 ||
		 g_lpTex[lpScene->nTex]->Info.Format == D3DFMT_A8 ||
		 g_lpTex[lpScene->nTex]->Info.Format == D3DFMT_A8R3G3B2 )
	{
		SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
		SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

        SetRenderState ( D3DRS_ALPHATESTENABLE, true );
        SetRenderState ( D3DRS_ALPHAREF, 0x08 );
        SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	}
	else
	{
		SetRenderState ( D3DRS_ALPHABLENDENABLE, false );
        SetRenderState ( D3DRS_ALPHATESTENABLE, false );
	}
	if ( FAILED ( g_D3DDevice->SetTexture ( 0, g_lpTex[lpScene->nTex]->lpTex ) ) )
		return false;

	if ( lpScene->nlTex > -1 )
	{
		if ( FAILED ( g_D3DDevice->SetTexture ( 1, g_lpTex[lpScene->nlTex]->lpTex ) ) )
			return false;

		SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
		SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		SetTextureStageState ( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	}
	else
	{
		if ( FAILED ( g_D3DDevice->SetTexture ( 1, 0 ) ) )
			return false;

		SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		SetTextureStageState ( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	}

	if ( FAILED ( g_D3DDevice->SetTransform ( D3DTS_WORLD,
											  &lpScene->lpFrame[lpScene->nFrame] ) ) )
		return false;
	if ( FAILED ( g_D3DDevice->MultiplyTransform ( D3DTS_WORLD,
												   &lpScene->matrix ) ) )
		return false;
	if ( FAILED ( g_D3DDevice->SetFVF ( SCENE_VERTEX ) ) )
		return false;
	if ( FAILED ( g_D3DDevice->DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST,
														0,
														lpScene->dwVecCount,
														lpScene->dwTriCount,
														lpScene->lpIB,
														D3DFMT_INDEX16,
														lpScene->lpVB,
														sizeof ( SceneVertex ) ) ) )
		return false;
	D3DXMatrixIdentity ( &lpScene->matrix );
	return true;
}

C3_CORE_DLL_API
void Scene_NextFrame ( C3Scene *lpScene, int nStep )
{
	lpScene->nFrame = ( lpScene->nFrame + nStep ) % lpScene->dwFrameCount;
}

C3_CORE_DLL_API
void Scene_Muliply ( C3Scene *lpScene, D3DXMATRIX *matrix )
{
	D3DXMatrixMultiply ( &lpScene->matrix,
						 &lpScene->matrix,
						 matrix );
}
