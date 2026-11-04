#include "c3_ptcl.h"
#include "c3_datafile.h"
#include "TqPackage.h"

C3_CORE_DLL_API int g_nPtclCount = 0;

C3_CORE_DLL_API
void Ptcl_Clear ( C3Ptcl *lpPtcl )
{
	lpPtcl->lpName = 0;
	lpPtcl->lpVB = 0;
	lpPtcl->nTex = -1;
	lpPtcl->lpTexName = 0;
	lpPtcl->dwCount = 0;
	lpPtcl->dwRow = 1;

	lpPtcl->lpPtcl = 0;
	lpPtcl->nFrame = 0;
	lpPtcl->dwFrames = 0;

	D3DXMatrixIdentity ( &lpPtcl->matrix );
}

C3_CORE_DLL_API
BOOL Ptcl_Load ( C3Ptcl **lpPtcl,
				 void *file,
				 BOOL bTex )
{
	*lpPtcl = new C3Ptcl;
	Ptcl_Clear ( *lpPtcl );

	DWORD temp;
	TQFRead ( &temp, sizeof ( unsigned long ), 1, file );
	( *lpPtcl )->lpName = new char[temp + 1];
	TQFRead ( ( *lpPtcl )->lpName, 1, temp, file );
	( *lpPtcl )->lpName[temp] = '\0';

	TQFRead ( &temp, sizeof ( DWORD ), 1, file );
	( *lpPtcl )->lpTexName = new char[temp + 1];
	TQFRead ( ( *lpPtcl )->lpTexName, 1, temp, file );
	( *lpPtcl )->lpTexName[temp] = '\0';
	if ( bTex )
	{
		C3Texture *tex;
		( *lpPtcl )->nTex = Texture_Load ( &tex, ( *lpPtcl )->lpTexName, 1 );
	}

	TQFRead ( &( *lpPtcl )->dwRow, sizeof ( DWORD ), 1, file );

	TQFRead ( &( *lpPtcl )->dwCount, sizeof ( DWORD ) , 1, file );
	if ( FAILED ( g_D3DDevice->CreateVertexBuffer ( ( *lpPtcl )->dwCount * 4 * sizeof ( PtclVertex ),
													D3DUSAGE_WRITEONLY,
													PTCL_VERTEX,
													D3DPOOL_MANAGED,
													&( *lpPtcl )->lpVB,
													NULL ) ) )
		return false;

	TQFRead ( &( *lpPtcl )->dwFrames, sizeof ( DWORD ), 1, file );
	( *lpPtcl )->lpPtcl = new PtclFrame[( *lpPtcl )->dwFrames];

	for ( DWORD n = 0; n < ( *lpPtcl )->dwFrames; n++ )
	{
		TQFRead ( &( *lpPtcl )->lpPtcl[n].dwCount, sizeof ( DWORD ), 1, file );

		if ( ( *lpPtcl )->lpPtcl[n].dwCount > 0 )
		{
			( *lpPtcl )->lpPtcl[n].lpPos = new D3DXVECTOR3[( *lpPtcl )->lpPtcl[n].dwCount];
			TQFRead ( ( *lpPtcl )->lpPtcl[n].lpPos,
					sizeof ( D3DXVECTOR3 ),
					( *lpPtcl )->lpPtcl[n].dwCount,
					file );

			( *lpPtcl )->lpPtcl[n].lpAge = new float[( *lpPtcl )->lpPtcl[n].dwCount];
			TQFRead ( ( *lpPtcl )->lpPtcl[n].lpAge,
					sizeof ( float ),
					( *lpPtcl )->lpPtcl[n].dwCount,
					file );

			( *lpPtcl )->lpPtcl[n].lpSize = new float[( *lpPtcl )->lpPtcl[n].dwCount];
			TQFRead ( ( *lpPtcl )->lpPtcl[n].lpSize,
					sizeof ( float ),
					( *lpPtcl )->lpPtcl[n].dwCount,
					file );

			TQFRead ( &( *lpPtcl )->lpPtcl[n].matrix,
					sizeof ( D3DXMATRIX ),
					1,
					file );
		}
		else
		{
			( *lpPtcl )->lpPtcl[n].lpPos = NULL;
			( *lpPtcl )->lpPtcl[n].lpAge = NULL;
			( *lpPtcl )->lpPtcl[n].lpSize = NULL;
		}
	}
	g_nPtclCount++;
	return true;
}
C3_CORE_DLL_API
BOOL Ptcl_Save ( char *lpName, C3Ptcl *lpPtcl, BOOL bNew )
{
	FILE *file = fopen ( lpName, bNew ? "w+b" : "r+b" );
	if ( !file )
		return false;
	fseek ( file, 0, SEEK_END );

	ChunkHeader chunk;
	chunk.byChunkID[0] = 'P';
	chunk.byChunkID[1] = 'T';
	chunk.byChunkID[2] = 'C';
	chunk.byChunkID[3] = 'L';
	chunk.dwChunkSize = 0;
	fwrite ( &chunk, sizeof ( chunk ), 1, file );

	DWORD length = strlen ( lpPtcl->lpName );
	fwrite ( &length, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( lpPtcl->lpName, sizeof ( char ), length, file );
	chunk.dwChunkSize += sizeof ( char ) * length;

	length = strlen ( lpPtcl->lpTexName );
	fwrite ( &length, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( lpPtcl->lpTexName, sizeof ( char ), length, file );
	chunk.dwChunkSize += sizeof ( char ) * length;

	fwrite ( &lpPtcl->dwRow, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( &lpPtcl->dwCount, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( &lpPtcl->dwFrames, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );
	
	for ( DWORD n = 0; n < lpPtcl->dwFrames; n++ )
	{
		fwrite ( &lpPtcl->lpPtcl[n].dwCount, sizeof ( DWORD ), 1, file );
		chunk.dwChunkSize += sizeof ( DWORD );

		if ( lpPtcl->lpPtcl[n].dwCount > 0 )
		{
			fwrite ( lpPtcl->lpPtcl[n].lpPos,
					 sizeof ( D3DXVECTOR3 ),
					 lpPtcl->lpPtcl[n].dwCount,
					 file );
			chunk.dwChunkSize += sizeof ( D3DXVECTOR3 ) * lpPtcl->lpPtcl[n].dwCount;

			fwrite ( lpPtcl->lpPtcl[n].lpAge,
					 sizeof ( float ),
					 lpPtcl->lpPtcl[n].dwCount,
					 file );
			chunk.dwChunkSize += sizeof ( float ) * lpPtcl->lpPtcl[n].dwCount;

			fwrite ( lpPtcl->lpPtcl[n].lpSize,
					 sizeof ( float ),
					 lpPtcl->lpPtcl[n].dwCount,
					 file );
			chunk.dwChunkSize += sizeof ( float ) * lpPtcl->lpPtcl[n].dwCount;

			fwrite ( &lpPtcl->lpPtcl[n].matrix,
					 sizeof ( D3DXMATRIX ),
					 1,
					 file );
			chunk.dwChunkSize += sizeof ( D3DXMATRIX );
		}
	}

	fseek ( file, -( int )( chunk.dwChunkSize + sizeof ( chunk ) ), SEEK_CUR );
	fwrite ( &chunk, sizeof ( chunk ), 1, file );
	fseek ( file, 0, SEEK_END );
	
	fclose ( file );
	return true;
}
C3_CORE_DLL_API
void Ptcl_Unload ( C3Ptcl **lpPtcl )
{
	int nDel = 0;
	if(( *lpPtcl ))
	{
		int nAmount = ( *lpPtcl )->dwFrames;
		for(int i = 0; i < nAmount; i ++)
		{
			PtclFrame* pFrame = ( *lpPtcl )->lpPtcl + i;
			SafeDeleteEx(pFrame->lpAge);
			SafeDeleteEx(pFrame->lpPos);
			SafeDeleteEx(pFrame->lpSize);
		}
	}
	SafeDeleteEx ( ( *lpPtcl )->lpName );
	SafeRelease ( ( *lpPtcl )->lpVB );
	SafeDeleteEx ( ( *lpPtcl )->lpTexName );

	( *lpPtcl )->nTex	=-1;
	SafeDeleteEx(( *lpPtcl )->lpPtcl->lpPos);
	SafeDeleteEx(( *lpPtcl )->lpPtcl->lpAge);
	SafeDeleteEx(( *lpPtcl )->lpPtcl->lpSize);

	SafeDeleteEx(( *lpPtcl )->lpPtcl);

	SafeDelete ( *lpPtcl );

	g_nPtclCount--;
}

C3_CORE_DLL_API
void Ptcl_Prepare ( void )
{
	g_D3DDevice->SetSoftwareVertexProcessing ( true );

	SetRenderState ( D3DRS_ZENABLE, true );
	SetRenderState ( D3DRS_ZWRITEENABLE, false );
	SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	SetRenderState ( D3DRS_DITHERENABLE, false );
	SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
	SetRenderState ( D3DRS_LIGHTING, false );
	SetRenderState ( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1 );
	SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );

	SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

	SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

	g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
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
static WORD						g_wPtclIndices[256 * 6];
static BOOL						g_bPtclIndices = FALSE;
static LPDIRECT3DINDEXBUFFER9	g_lpPtclIB = NULL;
static BOOL						g_bPtclIB = FALSE;

static WORD* Ptcl_GetIndices ( void )
{
	if ( !g_bPtclIndices )
	{
		g_bPtclIndices = TRUE;
		WORD *p = g_wPtclIndices;
		for ( WORD n = 0; n < 256; n++ )
		{
			*p++ = ( WORD )( n * 4 );
			*p++ = ( WORD )( n * 4 + 1 );
			*p++ = ( WORD )( n * 4 + 2 );
			*p++ = ( WORD )( n * 4 + 2 );
			*p++ = ( WORD )( n * 4 + 1 );
			*p++ = ( WORD )( n * 4 + 3 );
		}
	}
	return g_wPtclIndices;
}

static LPDIRECT3DINDEXBUFFER9 Ptcl_GetIndexBuffer ( void )
{
	if ( !g_bPtclIB )
	{
		if ( FAILED ( g_D3DDevice->CreateIndexBuffer ( sizeof ( g_wPtclIndices ), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_lpPtclIB, NULL ) ) )
			return NULL;
		void *p = NULL;
		if ( SUCCEEDED ( g_lpPtclIB->Lock ( 0, 0, &p, D3DLOCK_NOSYSLOCK ) ) )
		{
			memcpy ( p, Ptcl_GetIndices (), sizeof ( g_wPtclIndices ) );
			if ( SUCCEEDED ( g_lpPtclIB->Unlock () ) )
			{
				g_bPtclIB = TRUE;
				return g_lpPtclIB;
			}
		}
		SafeRelease ( g_lpPtclIB );
		return NULL;
	}
	return g_lpPtclIB;
}

static BOOL Ptcl_DrawQuads ( LPDIRECT3DVERTEXBUFFER9 lpVB, DWORD dwCount )
{
	g_D3DDevice->SetStreamSource ( 0, lpVB, 0, sizeof ( PtclVertex ) );
	int nChunks = dwCount >> 8;
	int nRest = dwCount & 0xFF;
	int n;
	for ( n = 0; n < nChunks; n++ )
	{
		g_D3DDevice->SetIndices ( Ptcl_GetIndexBuffer () );
		if ( FAILED ( g_D3DDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, n * 0x400, 0, 0x400, 0, 0x200 ) ) )
			return false;
	}
	if ( nRest > 0 )
	{
		g_D3DDevice->SetIndices ( Ptcl_GetIndexBuffer () );
		if ( FAILED ( g_D3DDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, n << 10, 0, nRest * 4, 0, nRest * 2 ) ) )
			return false;
	}
	return true;
}

C3_CORE_DLL_API
BOOL Ptcl_Draw ( C3Ptcl *lpPtcl, DWORD nAsb, DWORD nAdb )
{
	PtclVertex *vertex;
	PtclVertex *pb = NULL;
	PtclFrame *frame = &lpPtcl->lpPtcl[lpPtcl->nFrame];

	if ( frame->dwCount == 0 )
		return true;

	if ( lpPtcl->nTex < 0 )
		return true;

	DWORD segcount = lpPtcl->dwRow * lpPtcl->dwRow;
	float segsize = 1.0f / lpPtcl->dwRow;

	D3DXMATRIX inv;
	D3DXMatrixMultiply ( &inv, &frame->matrix, &lpPtcl->matrix );
	D3DXMATRIX matrix = inv;
	D3DXVECTOR3 unit ( 1.0f, 1.0f, 1.0f );
	D3DXVec3TransformNormal ( &unit, &unit, &matrix );
	float scale = D3DXVec3Length ( &unit ) * 0.577350557f;
	D3DXMatrixMultiply ( &inv, &matrix, &g_ViewMatrix );

	if ( FAILED ( lpPtcl->lpVB->Lock ( 0, 0, ( void** )&pb, D3DLOCK_NOSYSLOCK ) ) )
		return false;

	for ( DWORD n = 0; n < frame->dwCount; n++ )
	{
		DWORD index = ( DWORD )( frame->lpAge[n] * segcount );

		float u = index % lpPtcl->dwRow * segsize;
		float v = index / lpPtcl->dwRow * segsize;
		float size = frame->lpSize[n] * scale;

		D3DXVECTOR3 vec = frame->lpPos[n];
		D3DXVec3TransformCoord ( &vec, &vec, &inv );

		vertex = &pb[n * 4];

		vertex->x = vec.x - size;
		vertex->y = vec.y - size;
		vertex->z = vec.z;

		vertex->u = u;
		vertex->v = v + segsize;
		vertex->color = D3DCOLOR_RGBA ( 255, 255, 255, 255 );

		vertex = &pb[n * 4 + 1];

		vertex->x = vec.x + size;
		vertex->y = vec.y - size;
		vertex->z = vec.z;

		vertex->u = u + segsize;
		vertex->v = v + segsize;
		vertex->color = D3DCOLOR_RGBA ( 255, 255, 255, 255 );

		vertex = &pb[n * 4 + 2];

		vertex->x = vec.x - size;
		vertex->y = vec.y + size;
		vertex->z = vec.z;

		vertex->u = u;
		vertex->v = v;
		vertex->color = D3DCOLOR_RGBA ( 255, 255, 255, 255 );

		vertex = &pb[n * 4 + 3];

		vertex->x = vec.x + size;
		vertex->y = vec.y + size;
		vertex->z = vec.z;

		vertex->u = u + segsize;
		vertex->v = v;
		vertex->color = D3DCOLOR_RGBA ( 255, 255, 255, 255 );
	}

	if ( FAILED ( lpPtcl->lpVB->Unlock () ) )
		return false;

	SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
	SetRenderState ( D3DRS_SRCBLEND, nAsb);
	SetRenderState ( D3DRS_DESTBLEND, nAdb);

	SetRenderState ( D3DRS_ALPHATESTENABLE, true );
	SetRenderState ( D3DRS_ALPHAREF, 0x08 );
	SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	if ( FAILED ( g_D3DDevice->SetTexture ( 0, g_lpTex[lpPtcl->nTex]->lpTex ) ) )
		return false;

	if ( FAILED ( g_D3DDevice->SetTransform ( D3DTS_WORLD, &g_InvViewMatrix ) ) )
		return false;

	if ( FAILED ( g_D3DDevice->SetFVF ( PTCL_VERTEX ) ) )
		return false;

	return Ptcl_DrawQuads ( lpPtcl->lpVB, frame->dwCount );
}
C3_CORE_DLL_API
void Ptcl_NextFrame ( C3Ptcl *lpPtcl, int nStep )
{
	lpPtcl->nFrame = ( lpPtcl->nFrame + nStep ) % lpPtcl->dwFrames;
}
C3_CORE_DLL_API
void Ptcl_SetFrame ( C3Ptcl *lpPtcl, DWORD dwFrame )
{
	lpPtcl->nFrame = dwFrame % lpPtcl->dwFrames;
}
C3_CORE_DLL_API
void Ptcl_Muliply ( C3Ptcl *lpPtcl, D3DXMATRIX *matrix )
{
	D3DXMatrixMultiply ( &lpPtcl->matrix,
						 &lpPtcl->matrix,
						 matrix );
}
C3_CORE_DLL_API
void Ptcl_ClearMatrix ( C3Ptcl *lpPtcl )
{
	D3DXMatrixIdentity ( &lpPtcl->matrix );
}
C3_CORE_DLL_API
void Ptcl_ChangeTexture ( C3Ptcl *lpPtcl, int nTexID )
{
	lpPtcl->nTex = nTexID;
}

C3_CORE_DLL_API
void Ptcl_Clear ( C3PtclEx *lpPtcl )
{
	lpPtcl->lpName = 0;
	lpPtcl->lpVB = 0;
	lpPtcl->nTex = -1;
	lpPtcl->lpTexName = 0;
	lpPtcl->dwCount = 0;
	lpPtcl->dwRow = 1;

	lpPtcl->lpPtcl = 0;
	lpPtcl->nFrame = 0;
	lpPtcl->dwFrames = 0;

	D3DXMatrixIdentity ( &lpPtcl->matrix );

	lpPtcl->lpInfo = 0;
}

C3_CORE_DLL_API
BOOL Ptcl_Load ( C3PtclEx **lpPtcl,
				 void *file,
				 BOOL bTex )
{
	*lpPtcl = new C3PtclEx;
	Ptcl_Clear ( *lpPtcl );

	DWORD temp;
	TQFRead ( &temp, sizeof ( unsigned long ), 1, file );
	( *lpPtcl )->lpName = new char[temp + 1];
	TQFRead ( ( *lpPtcl )->lpName, 1, temp, file );
	( *lpPtcl )->lpName[temp] = '\0';

	TQFRead ( &temp, sizeof ( DWORD ), 1, file );
	( *lpPtcl )->lpTexName = new char[temp + 1];
	TQFRead ( ( *lpPtcl )->lpTexName, 1, temp, file );
	( *lpPtcl )->lpTexName[temp] = '\0';
	if ( bTex )
	{
		C3Texture *tex;
		( *lpPtcl )->nTex = Texture_Load ( &tex, ( *lpPtcl )->lpTexName, 1 );
		if ( ( *lpPtcl )->nTex == -1 )
			return false;
	}

	TQFRead ( &( *lpPtcl )->dwRow, sizeof ( DWORD ), 1, file );

	TQFRead ( &( *lpPtcl )->dwCount, sizeof ( DWORD ) , 1, file );
	if ( FAILED ( g_D3DDevice->CreateVertexBuffer ( ( *lpPtcl )->dwCount * 4 * sizeof ( PtclVertex ),
													D3DUSAGE_WRITEONLY,
													PTCL_VERTEX,
													D3DPOOL_MANAGED,
													&( *lpPtcl )->lpVB,
													NULL ) ) )
		return false;

	TQFRead ( &( *lpPtcl )->dwFrames, sizeof ( DWORD ), 1, file );
	( *lpPtcl )->lpPtcl = new PtclFrameEx[( *lpPtcl )->dwFrames];

	for ( DWORD n = 0; n < ( *lpPtcl )->dwFrames; n++ )
	{
		TQFRead ( &( *lpPtcl )->lpPtcl[n].dwCount, sizeof ( DWORD ), 1, file );

		if ( ( *lpPtcl )->lpPtcl[n].dwCount > 0 )
		{
			( *lpPtcl )->lpPtcl[n].lpID = new WORD[( *lpPtcl )->lpPtcl[n].dwCount];
			TQFRead ( ( *lpPtcl )->lpPtcl[n].lpID,
					sizeof ( WORD ),
					( *lpPtcl )->lpPtcl[n].dwCount,
					file );

			( *lpPtcl )->lpPtcl[n].lpPos = new D3DXVECTOR3[( *lpPtcl )->lpPtcl[n].dwCount];
			TQFRead ( ( *lpPtcl )->lpPtcl[n].lpPos,
					sizeof ( D3DXVECTOR3 ),
					( *lpPtcl )->lpPtcl[n].dwCount,
					file );

			( *lpPtcl )->lpPtcl[n].lpAge = new float[( *lpPtcl )->lpPtcl[n].dwCount];
			TQFRead ( ( *lpPtcl )->lpPtcl[n].lpAge,
					sizeof ( float ),
					( *lpPtcl )->lpPtcl[n].dwCount,
					file );

			( *lpPtcl )->lpPtcl[n].lpSize = new float[( *lpPtcl )->lpPtcl[n].dwCount];
			TQFRead ( ( *lpPtcl )->lpPtcl[n].lpSize,
					sizeof ( float ),
					( *lpPtcl )->lpPtcl[n].dwCount,
					file );

			TQFRead ( &( *lpPtcl )->lpPtcl[n].matrix,
					sizeof ( D3DXMATRIX ),
					1,
					file );
		}
		else
		{
			( *lpPtcl )->lpPtcl[n].lpID = NULL;
			( *lpPtcl )->lpPtcl[n].lpPos = NULL;
			( *lpPtcl )->lpPtcl[n].lpAge = NULL;
			( *lpPtcl )->lpPtcl[n].lpSize = NULL;
		}
	}
	return true;
}

C3_CORE_DLL_API
void Ptcl_Unload ( C3PtclEx **lpPtcl )
{
	if(( *lpPtcl ))
	{
		int nAmount = ( *lpPtcl )->dwFrames;
		for(int i = 0; i < nAmount; i ++)
		{
			PtclFrameEx* pFrame = ( *lpPtcl )->lpPtcl + i;
			SafeDeleteEx(pFrame->lpAge);
			SafeDeleteEx(pFrame->lpPos);
			SafeDeleteEx(pFrame->lpSize);
			SafeDeleteEx(pFrame->lpID);
		}
	}
	SafeDeleteEx ( ( *lpPtcl )->lpName );
	SafeRelease ( ( *lpPtcl )->lpVB );
	SafeDeleteEx ( ( *lpPtcl )->lpTexName );

	( *lpPtcl )->nTex	=-1;
	SafeDeleteEx(( *lpPtcl )->lpPtcl->lpPos);
	SafeDeleteEx(( *lpPtcl )->lpPtcl->lpAge);
	SafeDeleteEx(( *lpPtcl )->lpPtcl->lpSize);

	SafeDeleteEx(( *lpPtcl )->lpPtcl);

	SafeDelete ( *lpPtcl );
}

C3_CORE_DLL_API
BOOL Ptcl_Draw ( C3PtclEx *lpPtcl, DWORD nAsb, DWORD nAdb )
{
	PtclVertex *vertex;
	PtclVertex *pb = NULL;
	PtclFrameEx *frame = &lpPtcl->lpPtcl[lpPtcl->nFrame];

	if ( lpPtcl->nTex < 0 )
		return true;

	if ( frame->dwCount == 0 )
	{
		lpPtcl->lpInfo->erase ( lpPtcl->lpInfo->begin (), lpPtcl->lpInfo->end () );
		return true;
	}

	DWORD segcount = lpPtcl->dwRow * lpPtcl->dwRow;
	float segsize = 1.0f / lpPtcl->dwRow;

	D3DXMATRIX inv;
	D3DXMatrixMultiply ( &inv, &frame->matrix, &lpPtcl->matrix );
	D3DXMATRIX matrix = inv;
	D3DXVECTOR3 unit ( 1.0f, 1.0f, 1.0f );
	D3DXVec3TransformNormal ( &unit, &unit, &matrix );
	float scale = D3DXVec3Length ( &unit ) * 0.577350557f;
	D3DXMatrixMultiply ( &inv, &matrix, &g_ViewMatrix );

	if ( FAILED ( lpPtcl->lpVB->Lock ( 0, 0, ( void** )&pb, D3DLOCK_NOSYSLOCK ) ) )
		return false;

	for ( DWORD n = 0; n < frame->dwCount; n++ )
	{
		DWORD index = ( DWORD )( frame->lpAge[n] * segcount );

		float u = index % lpPtcl->dwRow * segsize;
		float v = index / lpPtcl->dwRow * segsize;

		D3DXMATRIX mat = inv;
		int id = frame->lpID[n];
		PTCL_INFO::iterator it = lpPtcl->lpInfo->find ( id );
		if ( it != lpPtcl->lpInfo->end () )
		{
			D3DXMatrixMultiply ( &mat, &it->second.matrix, &g_ViewMatrix );
			it->second.bUse = true;
		}
		else
		{
			PTCL_MATRIX info;
			info.matrix = matrix;
			( *lpPtcl->lpInfo )[id] = info;
		}

		float size = frame->lpSize[n] * scale;
		D3DXVECTOR3 vec = frame->lpPos[n];
		D3DXVec3TransformCoord ( &vec, &vec, &mat );

		vertex = &pb[n * 4];

		vertex->x = vec.x - size;
		vertex->y = vec.y - size;
		vertex->z = vec.z;

		vertex->u = u;
		vertex->v = v + segsize;
		vertex->color = D3DCOLOR_RGBA ( 255, 255, 255, 255 );

		vertex = &pb[n * 4 + 1];

		vertex->x = vec.x + size;
		vertex->y = vec.y - size;
		vertex->z = vec.z;

		vertex->u = u + segsize;
		vertex->v = v + segsize;
		vertex->color = D3DCOLOR_RGBA ( 255, 255, 255, 255 );

		vertex = &pb[n * 4 + 2];

		vertex->x = vec.x - size;
		vertex->y = vec.y + size;
		vertex->z = vec.z;

		vertex->u = u;
		vertex->v = v;
		vertex->color = D3DCOLOR_RGBA ( 255, 255, 255, 255 );

		vertex = &pb[n * 4 + 3];

		vertex->x = vec.x + size;
		vertex->y = vec.y + size;
		vertex->z = vec.z;

		vertex->u = u + segsize;
		vertex->v = v;
		vertex->color = D3DCOLOR_RGBA ( 255, 255, 255, 255 );
	}

	if ( FAILED ( lpPtcl->lpVB->Unlock () ) )
		return false;

	PTCL_INFO::iterator it = lpPtcl->lpInfo->begin ();
	while ( it != lpPtcl->lpInfo->end () )
	{
		if ( !it->second.bUse )
			lpPtcl->lpInfo->erase ( it++ );
		else
		{
			it->second.bUse = false;
			++it;
		}
	}

	SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
	SetRenderState ( D3DRS_SRCBLEND, nAsb);
	SetRenderState ( D3DRS_DESTBLEND, nAdb);

	SetRenderState ( D3DRS_ALPHATESTENABLE, true );
	SetRenderState ( D3DRS_ALPHAREF, 0x08 );
	SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	g_D3DDevice->SetRenderState ( D3DRS_LIGHTING, false );

	if ( FAILED ( g_D3DDevice->SetTexture ( 0, g_lpTex[lpPtcl->nTex]->lpTex ) ) )
		return false;

	if ( FAILED ( g_D3DDevice->SetTransform ( D3DTS_WORLD, &g_InvViewMatrix ) ) )
		return false;

	if ( FAILED ( g_D3DDevice->SetFVF ( PTCL_VERTEX ) ) )
		return false;

	return Ptcl_DrawQuads ( lpPtcl->lpVB, frame->dwCount );
}
C3_CORE_DLL_API
void Ptcl_SetFrame ( C3PtclEx *lpPtcl, DWORD dwFrame )
{
	lpPtcl->nFrame = dwFrame % lpPtcl->dwFrames;
}
C3_CORE_DLL_API
void Ptcl_Muliply ( C3PtclEx *lpPtcl, D3DXMATRIX *matrix )
{
	D3DXMatrixMultiply ( &lpPtcl->matrix,
						 &lpPtcl->matrix,
						 matrix );
}
C3_CORE_DLL_API
void Ptcl_ClearMatrix ( C3PtclEx *lpPtcl )
{
	D3DXMatrixIdentity ( &lpPtcl->matrix );
}
C3_CORE_DLL_API
void Ptcl_ChangeTexture ( C3PtclEx *lpPtcl, int nTexID )
{
	lpPtcl->nTex = nTexID;
}

C3_CORE_DLL_API
PTCL_INFO* Ptcl_CreateInfo ( int nCount )
{
	if ( nCount > 0 )
		return new PTCL_INFO[nCount];
	return 0;
}

C3_CORE_DLL_API
void Ptcl_DestroyInfo ( PTCL_INFO *lpInfo )
{
	if ( lpInfo )
		delete [] lpInfo;
}

C3_CORE_DLL_API
void Ptcl_Clear ( C3Ptcl3 *lpPtcl )
{
	Ptcl_Clear ( ( C3PtclEx* )lpPtcl );
	lpPtcl->bAlignSpeed = 0;
	lpPtcl->bBornMatrix = 0;
	lpPtcl->fSpin = 0.0f;
	lpPtcl->fSpinRandom = 0.0f;
	lpPtcl->fAlphaStart = 1.0f;
	lpPtcl->fAlphaMid = 1.0f;
	lpPtcl->fAlphaEnd = 1.0f;
	lpPtcl->dwFadeInFrames = 0;
	lpPtcl->dwFadeOutFrame = ( DWORD )-1;
	lpPtcl->fAgeAlpha0 = 1.0f;
	lpPtcl->fAgeAlpha1 = 1.0f;
	lpPtcl->fAgeAlpha2 = 1.0f;
	lpPtcl->fAgeFadeIn = 0.0f;
	lpPtcl->fAgeFadeOut = 1.0f;
	lpPtcl->vGravity = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	lpPtcl->vSpeed = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
}

C3_CORE_DLL_API
BOOL Ptcl_Load ( C3Ptcl3 **lpPtcl,
				 void *file,
				 BOOL bTex )
{
	*lpPtcl = new C3Ptcl3;
	Ptcl_Clear ( *lpPtcl );

	DWORD temp;
	TQFRead ( &temp, sizeof ( unsigned long ), 1, file );
	( *lpPtcl )->lpName = new char[temp + 1];
	TQFRead ( ( *lpPtcl )->lpName, 1, temp, file );
	( *lpPtcl )->lpName[temp] = '\0';

	TQFRead ( &temp, sizeof ( DWORD ), 1, file );
	( *lpPtcl )->lpTexName = new char[temp + 1];
	TQFRead ( ( *lpPtcl )->lpTexName, 1, temp, file );
	( *lpPtcl )->lpTexName[temp] = '\0';
	if ( bTex )
	{
		C3Texture *tex;
		( *lpPtcl )->nTex = Texture_Load ( &tex, ( *lpPtcl )->lpTexName, 1 );
		if ( ( *lpPtcl )->nTex == -1 )
			return false;
	}

	TQFRead ( &( *lpPtcl )->dwRow, sizeof ( DWORD ), 1, file );

	TQFRead ( &( *lpPtcl )->bAlignSpeed, 1, 1, file );
	TQFRead ( &( *lpPtcl )->bBornMatrix, 1, 1, file );
	TQFRead ( &( *lpPtcl )->fSpin, sizeof ( float ), 1, file );
	TQFRead ( &( *lpPtcl )->fSpinRandom, sizeof ( float ), 1, file );
	TQFRead ( &( *lpPtcl )->fAlphaStart, sizeof ( float ), 1, file );
	TQFRead ( &( *lpPtcl )->fAlphaMid, sizeof ( float ), 1, file );
	TQFRead ( &( *lpPtcl )->fAlphaEnd, sizeof ( float ), 1, file );
	TQFRead ( &( *lpPtcl )->dwFadeInFrames, sizeof ( DWORD ), 1, file );
	TQFRead ( &( *lpPtcl )->dwFadeOutFrame, sizeof ( DWORD ), 1, file );
	TQFRead ( &( *lpPtcl )->fAgeAlpha0, sizeof ( float ), 1, file );
	TQFRead ( &( *lpPtcl )->fAgeAlpha1, sizeof ( float ), 1, file );
	TQFRead ( &( *lpPtcl )->fAgeAlpha2, sizeof ( float ), 1, file );
	TQFRead ( &( *lpPtcl )->fAgeFadeIn, sizeof ( float ), 1, file );
	TQFRead ( &( *lpPtcl )->fAgeFadeOut, sizeof ( float ), 1, file );

	TQFRead ( &( *lpPtcl )->dwCount, sizeof ( DWORD ) , 1, file );
	if ( FAILED ( g_D3DDevice->CreateVertexBuffer ( ( *lpPtcl )->dwCount * 4 * sizeof ( PtclVertex ),
													D3DUSAGE_WRITEONLY,
													PTCL_VERTEX,
													D3DPOOL_MANAGED,
													&( *lpPtcl )->lpVB,
													NULL ) ) )
		return false;

	TQFRead ( &( *lpPtcl )->dwFrames, sizeof ( DWORD ), 1, file );
	( *lpPtcl )->lpPtcl = new PtclFrameEx[( *lpPtcl )->dwFrames];

	for ( DWORD n = 0; n < ( *lpPtcl )->dwFrames; n++ )
	{
		TQFRead ( &( *lpPtcl )->lpPtcl[n].dwCount, sizeof ( DWORD ), 1, file );

		if ( ( *lpPtcl )->lpPtcl[n].dwCount > 0 )
		{
			( *lpPtcl )->lpPtcl[n].lpID = new WORD[( *lpPtcl )->lpPtcl[n].dwCount];
			TQFRead ( ( *lpPtcl )->lpPtcl[n].lpID,
					sizeof ( WORD ),
					( *lpPtcl )->lpPtcl[n].dwCount,
					file );

			( *lpPtcl )->lpPtcl[n].lpPos = new D3DXVECTOR3[( *lpPtcl )->lpPtcl[n].dwCount];
			TQFRead ( ( *lpPtcl )->lpPtcl[n].lpPos,
					sizeof ( D3DXVECTOR3 ),
					( *lpPtcl )->lpPtcl[n].dwCount,
					file );

			( *lpPtcl )->lpPtcl[n].lpAge = new float[( *lpPtcl )->lpPtcl[n].dwCount];
			TQFRead ( ( *lpPtcl )->lpPtcl[n].lpAge,
					sizeof ( float ),
					( *lpPtcl )->lpPtcl[n].dwCount,
					file );

			( *lpPtcl )->lpPtcl[n].lpSize = new float[( *lpPtcl )->lpPtcl[n].dwCount];
			TQFRead ( ( *lpPtcl )->lpPtcl[n].lpSize,
					sizeof ( float ),
					( *lpPtcl )->lpPtcl[n].dwCount,
					file );

			TQFRead ( &( *lpPtcl )->lpPtcl[n].matrix,
					sizeof ( D3DXMATRIX ),
					1,
					file );
		}
		else
		{
			( *lpPtcl )->lpPtcl[n].lpID = NULL;
			( *lpPtcl )->lpPtcl[n].lpPos = NULL;
			( *lpPtcl )->lpPtcl[n].lpAge = NULL;
			( *lpPtcl )->lpPtcl[n].lpSize = NULL;
		}
	}
	return true;
}

C3_CORE_DLL_API
void Ptcl_Unload ( C3Ptcl3 **lpPtcl )
{
	Ptcl_Unload ( ( C3PtclEx** )lpPtcl );
}

C3_CORE_DLL_API
BOOL Ptcl_Draw ( C3Ptcl3 *lpPtcl, DWORD nAsb, DWORD nAdb )
{
	PtclVertex *pb = NULL;
	PtclFrameEx *frame = &lpPtcl->lpPtcl[lpPtcl->nFrame];

	if ( lpPtcl->nTex < 0 )
		return true;

	if ( frame->dwCount == 0 )
	{
		lpPtcl->lpInfo->erase ( lpPtcl->lpInfo->begin (), lpPtcl->lpInfo->end () );
		return true;
	}

	DWORD segcount = lpPtcl->dwRow * lpPtcl->dwRow;
	float segsize = 1.0f / lpPtcl->dwRow;

	D3DXMATRIX matrix;
	D3DXMatrixMultiply ( &matrix, &frame->matrix, &lpPtcl->matrix );
	D3DXMATRIX mat = matrix;
	D3DXVECTOR3 unit ( 1.0f, 1.0f, 1.0f );
	D3DXVec3TransformNormal ( &unit, &unit, &mat );
	float scale = D3DXVec3Length ( &unit ) * 0.577350557f;

	D3DXMATRIX world;
	D3DXMatrixIdentity ( &world );
	D3DXVECTOR3 camera ( g_InvViewMatrix._41, g_InvViewMatrix._42, g_InvViewMatrix._43 );
	DWORD dwNow = timeGetTime ();

	float alpha;
	DWORD dwFrame = ( DWORD )lpPtcl->nFrame;
	if ( dwFrame < lpPtcl->dwFadeInFrames )
	{
		float t = ( float )( int )dwFrame / ( float )lpPtcl->dwFadeInFrames;
		alpha = ( 1.0f - t ) * lpPtcl->fAlphaStart + t * lpPtcl->fAlphaMid;
	}
	else if ( dwFrame < lpPtcl->dwFadeOutFrame )
		alpha = lpPtcl->fAlphaMid;
	else
	{
		float t = ( float )( dwFrame - lpPtcl->dwFadeOutFrame ) / ( float )( lpPtcl->dwFrames - lpPtcl->dwFadeOutFrame );
		alpha = ( 1.0f - t ) * lpPtcl->fAlphaMid + t * lpPtcl->fAlphaEnd;
	}
	if ( alpha < 0.0f )
		alpha = 0.0f;
	else if ( alpha > 1.0f )
		alpha = 1.0f;

	if ( FAILED ( lpPtcl->lpVB->Lock ( 0, 0, ( void** )&pb, D3DLOCK_NOSYSLOCK ) ) )
		return false;

	DWORD dwDrawn = 0;
	for ( DWORD n = 0; n < frame->dwCount; n++ )
	{
		float age = frame->lpAge[n];
		DWORD index = ( DWORD )( age * segcount );

		float u = index % lpPtcl->dwRow * segsize;
		float v = index / lpPtcl->dwRow * segsize;

		D3DXVECTOR3 vec = frame->lpPos[n];
		int id = frame->lpID[n];
		float spin;
		D3DXVECTOR3 dir;
		PTCL3_INFO::iterator it = lpPtcl->lpInfo->find ( id );
		if ( it != lpPtcl->lpInfo->end () )
		{
			float dt = ( float )( dwNow - it->second.dwTime ) * 0.001f;
			it->second.bUse = true;
			D3DXMATRIX m = lpPtcl->bBornMatrix ? it->second.matrix : mat;
			m._41 += dt * lpPtcl->vSpeed.x;
			m._42 += dt * lpPtcl->vSpeed.y;
			m._43 += dt * lpPtcl->vSpeed.z;
			float half = dt * dt * 0.5f;
			m._41 += half * lpPtcl->vGravity.x;
			m._42 += half * lpPtcl->vGravity.y;
			m._43 += half * lpPtcl->vGravity.z;
			D3DXVec3TransformCoord ( &vec, &vec, &m );
			if ( lpPtcl->bAlignSpeed )
			{
				D3DXVECTOR3 moved = vec - it->second.pos;
				D3DXVec3Normalize ( &dir, &moved );
			}
			it->second.pos = vec;
			spin = it->second.fSpin;
		}
		else
		{
			D3DXVec3TransformCoord ( &vec, &vec, &mat );
			spin = ( float )rand () * 3.051851e-05f * ( lpPtcl->fSpinRandom - ( -lpPtcl->fSpinRandom ) ) - lpPtcl->fSpinRandom + lpPtcl->fSpin;
			PTCL3_MATRIX info;
			if ( lpPtcl->bBornMatrix )
				info.matrix = mat;
			info.pos = vec;
			info.dwTime = dwNow;
			info.fSpin = spin;
			( *lpPtcl->lpInfo )[id] = info;
			if ( lpPtcl->bAlignSpeed )
				continue;
		}

		float angle = ( float )lpPtcl->nFrame * spin * 3.14159274f / 180.0f;
		float c = ( float )cos ( angle );
		float s = ( float )sin ( angle );

		D3DXVECTOR3 up, at, right;
		if ( lpPtcl->bAlignSpeed )
		{
			D3DXVECTOR3 look = camera - vec;
			D3DXVec3Normalize ( &at, &look );
			D3DXVec3Cross ( &right, &dir, &at );
			D3DXVec3Cross ( &at, &right, &dir );
			up = dir;
		}
		else
		{
			up = D3DXVECTOR3 ( g_ViewMatrix._12, g_ViewMatrix._22, g_ViewMatrix._32 );
			at = D3DXVECTOR3 ( -g_ViewMatrix._13, -g_ViewMatrix._23, -g_ViewMatrix._33 );
			D3DXVec3Cross ( &right, &up, &at );
		}
		world._11 = right.x; world._12 = right.y; world._13 = right.z;
		world._21 = up.x;    world._22 = up.y;    world._23 = up.z;
		world._31 = at.x;    world._32 = at.y;    world._33 = at.z;

		float size = frame->lpSize[n] * scale;
		float b = ( s + c ) * size;
		float a = ( c - s ) * size;

		float fade;
		if ( age < lpPtcl->fAgeFadeIn )
		{
			float t = age / lpPtcl->fAgeFadeIn;
			fade = ( 1.0f - t ) * lpPtcl->fAgeAlpha0 + t * lpPtcl->fAgeAlpha1;
		}
		else if ( age < lpPtcl->fAgeFadeOut )
			fade = lpPtcl->fAgeAlpha1;
		else
		{
			float t = ( age - lpPtcl->fAgeFadeOut ) / ( 1.0f - lpPtcl->fAgeFadeOut );
			fade = ( 1.0f - t ) * lpPtcl->fAgeAlpha1 + t * lpPtcl->fAgeAlpha2;
		}
		fade *= alpha;
		DWORD grey;
		if ( fade > 1.0f )
			grey = 0xFF;
		else if ( fade > 0.0f )
			grey = ( DWORD )( fade * 255.0f );
		else
			grey = 0;
		DWORD color = ( grey << 24 ) | ( grey << 16 ) | ( grey << 8 ) | grey;

		PtclVertex *vertex = &pb[dwDrawn * 4];
		D3DXVECTOR3 corner;

		corner = D3DXVECTOR3 ( -a, -b, 0.0f );
		D3DXVec3TransformCoord ( &corner, &corner, &world );
		vertex[0].x = vec.x + corner.x;
		vertex[0].y = vec.y + corner.y;
		vertex[0].z = vec.z + corner.z;
		vertex[0].u = u;
		vertex[0].v = v + segsize;
		vertex[0].color = color;

		vertex[3].x = vec.x - corner.x;
		vertex[3].y = vec.y - corner.y;
		vertex[3].z = vec.z - corner.z;
		vertex[3].v = v;
		vertex[3].u = u + segsize;
		vertex[3].color = color;

		corner = D3DXVECTOR3 ( b, -a, 0.0f );
		D3DXVec3TransformCoord ( &corner, &corner, &world );
		vertex[1].x = vec.x + corner.x;
		vertex[1].y = vec.y + corner.y;
		vertex[1].z = vec.z + corner.z;
		vertex[1].v = v + segsize;
		vertex[1].u = u + segsize;
		vertex[1].color = color;

		vertex[2].x = vec.x - corner.x;
		vertex[2].y = vec.y - corner.y;
		vertex[2].z = vec.z - corner.z;
		vertex[2].u = u;
		vertex[2].v = v;
		vertex[2].color = color;

		dwDrawn++;
	}

	if ( FAILED ( lpPtcl->lpVB->Unlock () ) )
		return false;

	PTCL3_INFO::iterator it = lpPtcl->lpInfo->begin ();
	while ( it != lpPtcl->lpInfo->end () )
	{
		if ( !it->second.bUse )
			lpPtcl->lpInfo->erase ( it++ );
		else
		{
			it->second.bUse = false;
			++it;
		}
	}

	SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
	SetRenderState ( D3DRS_SRCBLEND, nAsb);
	SetRenderState ( D3DRS_DESTBLEND, nAdb);

	SetRenderState ( D3DRS_ALPHATESTENABLE, true );
	SetRenderState ( D3DRS_ALPHAREF, 0x08 );
	SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	g_D3DDevice->SetRenderState ( D3DRS_LIGHTING, false );

	if ( FAILED ( g_D3DDevice->SetTexture ( 0, g_lpTex[lpPtcl->nTex]->lpTex ) ) )
		return false;

	D3DXMatrixIdentity ( &world );
	if ( FAILED ( g_D3DDevice->SetTransform ( D3DTS_WORLD, &world ) ) )
		return false;

	if ( FAILED ( g_D3DDevice->SetFVF ( PTCL_VERTEX ) ) )
		return false;

	return Ptcl_DrawQuads ( lpPtcl->lpVB, dwDrawn );
}

C3_CORE_DLL_API
PTCL3_INFO* Ptcl3_CreateInfo ( int nCount )
{
	if ( nCount > 0 )
		return new PTCL3_INFO[nCount];
	return 0;
}
C3_CORE_DLL_API
void Ptcl3_DestroyInfo ( PTCL3_INFO *lpInfo )
{
	if ( lpInfo )
		delete [] lpInfo;
}

C3_CORE_DLL_API
void Ptcl3_SetGravity ( C3Ptcl3 *lpPtcl, D3DXVECTOR3 vGravity )
{
	lpPtcl->vGravity = vGravity;
}
C3_CORE_DLL_API
void Ptrl3_SetExternalSpeed ( C3Ptcl3 *lpPtcl, D3DXVECTOR3 vSpeed )
{
	lpPtcl->vSpeed = vSpeed;
}
