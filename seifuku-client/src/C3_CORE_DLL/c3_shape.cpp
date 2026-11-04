#include "c3_shape.h"
#include "c3_texture.h"
#include "c3_datafile.h"
#include "TqPackage.h"

C3_CORE_DLL_API int g_nSMotionCount = 0;
C3_CORE_DLL_API int g_nShapeCount = 0;

C3_CORE_DLL_API
void SMotion_Clear ( C3SMotion *lpSMotion )
{
	lpSMotion->dwFrames = 0;
	lpSMotion->lpFrames = 0;
	lpSMotion->nFrame = 0;
}

C3_CORE_DLL_API
BOOL SMotion_Load ( C3SMotion **lpSMotion, void *file )
{
	*lpSMotion = new C3SMotion;
	SMotion_Clear ( *lpSMotion );

	TQFRead ( &( *lpSMotion )->dwFrames, sizeof ( DWORD ), 1, file );

	( *lpSMotion )->lpFrames = new D3DXMATRIX[( *lpSMotion )->dwFrames];
	TQFRead ( ( *lpSMotion )->lpFrames,
			sizeof ( D3DXMATRIX ),
			( *lpSMotion )->dwFrames,
			file );

	D3DXMatrixIdentity ( &( *lpSMotion )->matrix );
	g_nSMotionCount++;
	return true;
}	
C3_CORE_DLL_API
BOOL SMotion_Save ( char *lpName, C3SMotion *lpSMotion, BOOL bNew )
{
	FILE *file = fopen ( lpName, bNew ? "w+b" : "r+b" );
	if ( !file )
		return false;
	fseek ( file, 0, SEEK_END );

	ChunkHeader chunk;
	chunk.byChunkID[0] = 'S';
	chunk.byChunkID[1] = 'M';
	chunk.byChunkID[2] = 'O';
	chunk.byChunkID[3] = 'T';
	chunk.dwChunkSize = 0;
	fwrite ( &chunk, sizeof ( chunk ), 1, file );

	fwrite ( &lpSMotion->dwFrames, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( lpSMotion->lpFrames,
			 sizeof ( D3DXMATRIX ),
			 lpSMotion->dwFrames,
			 file );
	chunk.dwChunkSize += sizeof ( D3DXMATRIX ) * lpSMotion->dwFrames;

	fseek ( file, -( int )( chunk.dwChunkSize + sizeof ( chunk ) ), SEEK_CUR );
	fwrite ( &chunk, sizeof ( chunk ), 1, file );
	fseek ( file, 0, SEEK_END );

	fclose ( file );
	return true;
}
C3_CORE_DLL_API
void SMotion_Unload ( C3SMotion **lpSMotion )
{
	SafeDeleteEx ( ( *lpSMotion )->lpFrames );
	SafeDelete ( *lpSMotion );
	g_nSMotionCount--;
}

C3_CORE_DLL_API
void Shape_Clear ( C3Shape *lpShape )
{
	lpShape->lpName = 0;
	
	lpShape->dwLineCount = 0;
	lpShape->lpLine = 0;

	lpShape->vb = 0;
	lpShape->dwSegment = 0;
	lpShape->dwSegmentMax = 0;
	lpShape->dwSegmentCur = 0;

	lpShape->bFrist = true;

	lpShape->dwSmooth = 0;

	lpShape->lpTexName = 0;
	lpShape->nTex = -1;

	lpShape->lpMotion = 0;
}

C3_CORE_DLL_API
BOOL Shape_Load ( C3Shape **lpShape, void *file, BOOL bTex )
{
	*lpShape = new C3Shape;
	Shape_Clear ( *lpShape );

	DWORD temp;
	TQFRead ( &temp, sizeof ( unsigned long ), 1, file );
	( *lpShape )->lpName = new char[temp + 1];
	TQFRead ( ( *lpShape )->lpName, 1, temp, file );
	( *lpShape )->lpName[temp] = '\0';

	TQFRead ( &( *lpShape )->dwLineCount, sizeof ( DWORD ), 1, file );

	( *lpShape )->lpLine = new C3Line[( *lpShape )->dwLineCount];

	for ( DWORD n = 0; n < ( *lpShape )->dwLineCount; n++ )
	{
		TQFRead ( &( *lpShape )->lpLine[n].dwVecCount,
				sizeof ( DWORD ),
				1,
				file );

		( *lpShape )->lpLine[n].lpVB = new D3DXVECTOR3[( *lpShape )->lpLine[n].dwVecCount];
		TQFRead ( ( *lpShape )->lpLine[n].lpVB,
				sizeof ( D3DXVECTOR3 ),
				( *lpShape )->lpLine[n].dwVecCount,
				file );
	}

	TQFRead ( &temp, sizeof ( DWORD ), 1, file );
	( *lpShape )->lpTexName = new char[temp + 1];
	TQFRead ( ( *lpShape )->lpTexName, 1, temp, file );
	( *lpShape )->lpTexName[temp] = '\0';
	if ( bTex )
	{
		C3Texture *tex;
		( *lpShape )->nTex = Texture_Load ( &tex, ( *lpShape )->lpTexName );
	}

	DWORD segment;
	TQFRead ( &segment, sizeof ( DWORD ), 1, file );
	Shape_SetSegment ( *lpShape, segment );
	g_nShapeCount++;
	return true;
}

C3_CORE_DLL_API
BOOL Shape_Save ( char *lpName, C3Shape *lpShape, BOOL bNew )
{
	FILE *file = fopen ( lpName, bNew ? "w+b" : "r+b" );
	if ( !file )
		return false;
	fseek ( file, 0, SEEK_END );

	ChunkHeader chunk;
	chunk.byChunkID[0] = 'S';
	chunk.byChunkID[1] = 'H';
	chunk.byChunkID[2] = 'A';
	chunk.byChunkID[3] = 'P';
	chunk.dwChunkSize = 0;
	fwrite ( &chunk, sizeof ( chunk ), 1, file );

	DWORD length = strlen ( lpShape->lpName );
	fwrite ( &length, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( lpShape->lpName, sizeof ( char ), length, file );
	chunk.dwChunkSize += sizeof ( char ) * length;

	fwrite ( &lpShape->dwLineCount, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	for ( DWORD n = 0; n < lpShape->dwLineCount; n++ )
	{
		fwrite ( &lpShape->lpLine[n].dwVecCount,
				 sizeof ( DWORD ),
				 1,
				 file );
		chunk.dwChunkSize += sizeof ( DWORD );

		fwrite ( lpShape->lpLine[n].lpVB,
				 sizeof ( D3DXVECTOR3 ),
				 lpShape->lpLine[n].dwVecCount,
				 file );
		chunk.dwChunkSize += sizeof ( D3DXVECTOR3 ) * lpShape->lpLine[n].dwVecCount;
	}

	length = strlen ( lpShape->lpTexName );
	fwrite ( &length, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( lpShape->lpTexName, sizeof ( char ), length, file );
	chunk.dwChunkSize += sizeof ( char ) * length;

	fwrite ( &lpShape->dwSegment, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );


	fseek ( file, -( int )( chunk.dwChunkSize + sizeof ( chunk ) ), SEEK_CUR );
	fwrite ( &chunk, sizeof ( chunk ), 1, file );
	fseek ( file, 0, SEEK_END );

	fclose ( file );
	return true;
}
C3_CORE_DLL_API
void Shape_Unload ( C3Shape **lpShape )
{
	SafeDeleteEx ( ( *lpShape )->lpName );
	SafeDeleteEx ( ( *lpShape )->lpTexName );

	for ( DWORD n = 0; n < ( *lpShape )->dwLineCount; n++ )
	{
		SafeDeleteEx ( ( *lpShape )->lpLine[n].lpVB );
	}

	SafeDeleteEx ( ( *lpShape )->lpLine );
	SafeRelease ( ( *lpShape )->vb );

	SafeDelete ( *lpShape );

	g_nShapeCount--;
}
C3_CORE_DLL_API
void Shape_SetSegment ( C3Shape *lpShape, DWORD dwSegment, DWORD dwSmooth )
{
	dwSegment = dwSegment * 5;
	if ( dwSegment > 800 )
		dwSegment = 800;

	lpShape->dwSegment = dwSegment;
	lpShape->dwSegmentMax = dwSegment + 1;
	lpShape->dwSmooth = 5;
	lpShape->dwSegmentCur = 0;

	SafeRelease ( lpShape->vb );
	g_D3DDevice->CreateVertexBuffer ( lpShape->dwSegmentMax * 2 * sizeof ( ShapeOutVertex ),
									  0,
									  SHAPE_OUT_VERTEX,
									  D3DPOOL_MANAGED,
									  &lpShape->vb,
									  NULL );
}

C3_CORE_DLL_API
BOOL Shape_Draw ( C3Shape *lpShape, BOOL bLocal )
{
	D3DXMATRIX mm;

	if ( bLocal )
		mm = lpShape->lpMotion->lpFrames[lpShape->lpMotion->nFrame];
	else
		D3DXMatrixMultiply ( &mm,
							 &lpShape->lpMotion->lpFrames[lpShape->lpMotion->nFrame],
							 &lpShape->lpMotion->matrix );

	D3DXVECTOR3 vec[2];
	D3DXVec3TransformCoord ( &vec[0],
							 ( D3DXVECTOR3* )&lpShape->lpLine[0].lpVB[0],
							 &mm );
	D3DXVec3TransformCoord ( &vec[1],
							 ( D3DXVECTOR3* )&lpShape->lpLine[0].lpVB[1],
							 &mm );

	ShapeOutVertex *pb = NULL;
	if ( !lpShape->bFrist )
	{
		if ( FAILED ( lpShape->vb->Lock ( 0, 0, ( void** )&pb, D3DLOCK_NOSYSLOCK ) ) )
			return false;

		D3DXVECTOR3 sub;
		D3DXVec3Subtract ( &sub, &vec[0], &vec[1] );
		float len = D3DXVec3Length ( &sub );

		float step = 1.0f / ( float )lpShape->dwSmooth;
		for ( DWORD nn = 1; nn <= lpShape->dwSmooth; nn++ )
		{
			float t = ( float )nn * step;
			D3DXVECTOR3 smooths[2];
			D3DXVec3Lerp ( &smooths[0], &lpShape->last[0], &vec[0], t );
			D3DXVec3Lerp ( &smooths[1], &lpShape->last[1], &vec[1], t );

			D3DXVec3Subtract ( &sub, &smooths[0], &smooths[1] );
			float lennow = D3DXVec3Length ( &sub );
			float k = len / lennow;

			ShapeOutVertex *v = &pb[lpShape->dwSegmentCur * 2];
			v[0].x = k * sub.x + smooths[1].x;
			v[0].y = k * sub.y + smooths[1].y;
			v[0].z = k * sub.z + smooths[1].z;
			v[0].color = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
			v[1].x = smooths[1].x;
			v[1].y = smooths[1].y;
			v[1].z = smooths[1].z;
			v[1].color = D3DCOLOR_ARGB ( 255, 255, 255, 255 );

			lpShape->dwSegmentCur++;
		}
		if ( lpShape->dwSegmentCur == lpShape->dwSegmentMax )
		{
			pb[0].x = vec[0].x; pb[0].y = vec[0].y; pb[0].z = vec[0].z;
			pb[1].x = vec[1].x; pb[1].y = vec[1].y; pb[1].z = vec[1].z;
			pb[1].color = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
			pb[0].color = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
			lpShape->dwSegmentCur = 1;
		}

		float uvstep = 0.95f / ( float )lpShape->dwSegment;
		float u = 0.0f;
		DWORD n;
		for ( n = lpShape->dwSegmentCur; n <= lpShape->dwSegment; n++ )
		{
			pb[n * 2].u = u;		pb[n * 2].v = 0.0f;
			pb[n * 2 + 1].u = u;	pb[n * 2 + 1].v = 1.0f;
			u += uvstep;
		}
		for ( n = 0; n < lpShape->dwSegmentCur; n++ )
		{
			pb[n * 2].u = u;		pb[n * 2].v = 0.0f;
			pb[n * 2 + 1].u = u;	pb[n * 2 + 1].v = 1.0f;
			u += uvstep;
		}

		if ( FAILED ( lpShape->vb->Unlock () ) )
			return false;

		if ( lpShape->nTex == -1 )
		{
			if ( FAILED ( g_D3DDevice->SetTexture ( 0, 0 ) ) )
				return false;
		}
		else
		{
			if ( FAILED ( g_D3DDevice->SetTexture ( 0, g_lpTex[lpShape->nTex]->lpTex ) ) )
				return false;
		}

		g_D3DDevice->SetTransform ( D3DTS_WORLD, bLocal ? &lpShape->lpMotion->matrix : &g_IDMatrix );

		if ( FAILED ( g_D3DDevice->SetFVF ( SHAPE_OUT_VERTEX ) ) )
			return false;

		g_D3DDevice->SetStreamSource ( 0, lpShape->vb, 0, sizeof ( ShapeOutVertex ) );

		if ( lpShape->dwSegmentCur > 1 )
		{
			if ( FAILED ( g_D3DDevice->DrawPrimitive ( D3DPT_TRIANGLESTRIP, 0, lpShape->dwSegmentCur * 2 - 2 ) ) )
				return false;
		}
		if ( lpShape->dwSegment - lpShape->dwSegmentCur > 1 )
		{
			if ( FAILED ( g_D3DDevice->DrawPrimitive ( D3DPT_TRIANGLESTRIP,
														( lpShape->dwSegmentCur + 1 ) * 2,
														( lpShape->dwSegment - lpShape->dwSegmentCur - 1 ) * 2 ) ) )
				return false;
		}
	}
	else
	{
		if ( FAILED ( lpShape->vb->Lock ( 0, 0, ( void** )&pb, D3DLOCK_NOSYSLOCK ) ) )
			return false;
		ZeroMemory ( pb, sizeof ( ShapeOutVertex ) * 2 * lpShape->dwSegmentMax );
		lpShape->bFrist = false;

		pb[0].x = vec[0].x; pb[0].y = vec[0].y; pb[0].z = vec[0].z;
		pb[1].x = vec[1].x; pb[1].y = vec[1].y; pb[1].z = vec[1].z;
		pb[1].color = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
		pb[0].color = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
		pb[1].u = 0.0f;
		pb[0].u = 0.0f;
		pb[0].v = 0.0f;
		pb[1].v = 1.0f;

		lpShape->vb->Unlock ();
		lpShape->dwSegmentCur = 1;
	}

	lpShape->last[0] = vec[0];
	lpShape->last[1] = vec[1];

	return true;
}
C3_CORE_DLL_API
void Shape_Prepare ( DWORD dwAsb, DWORD dwAdb )
{
	g_D3DDevice->SetSoftwareVertexProcessing ( false );
	SetRenderState ( D3DRS_ZENABLE, true );
	SetRenderState ( D3DRS_ZWRITEENABLE, false );
	SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	SetRenderState ( D3DRS_DITHERENABLE, false );
	SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
	SetRenderState ( D3DRS_LIGHTING, false );
	SetRenderState ( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1 );
	SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );

	SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
	SetRenderState ( D3DRS_SRCBLEND, dwAsb );
	SetRenderState ( D3DRS_DESTBLEND, dwAdb );

	SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

	SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

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
}
C3_CORE_DLL_API
void Shape_ChangeTexture ( C3Shape *lpShape, int nTexID )
{
	lpShape->nTex = nTexID;
}
C3_CORE_DLL_API
void Shape_Muliply ( C3Shape *lpShape, D3DXMATRIX *matrix )
{
	D3DXMatrixMultiply ( &lpShape->lpMotion->matrix,
						 &lpShape->lpMotion->matrix,
						 matrix );
}
C3_CORE_DLL_API
void Shape_NextFrame ( C3Shape *lpShape, int nStep )
{
	lpShape->lpMotion->nFrame = ( lpShape->lpMotion->nFrame + nStep ) % lpShape->lpMotion->dwFrames;
}
C3_CORE_DLL_API
void Shape_SetFrame ( C3Shape *lpShape, DWORD dwFrame )
{
	lpShape->lpMotion->nFrame = dwFrame;
}
C3_CORE_DLL_API
void Shape_ClearMatrix ( C3Shape *lpShape )
{
	D3DXMatrixIdentity ( &lpShape->lpMotion->matrix );
}
