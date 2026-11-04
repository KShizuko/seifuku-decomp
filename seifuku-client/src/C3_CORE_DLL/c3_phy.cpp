#include "c3_phy.h"
#include "c3_main.h"
#include "c3_datafile.h"
#include "TqPackage.h"
#include <vector>
#include <stdlib.h>

C3_CORE_DLL_API int g_nMotionCount = 0;
C3_CORE_DLL_API int g_nPhyCount = 0;

C3_CORE_DLL_API
void Motion_Clear ( C3Motion *lpMotion )
{
	lpMotion->dwBoneCount = 0;
	lpMotion->dwFrames = 0;
	lpMotion->nFrame = 0;

	lpMotion->dwKeyFrames = 0;
	lpMotion->lpKeyFrame = 0;

	lpMotion->matrix = 0;
	lpMotion->nFrame = 0;
	lpMotion->dwMorphCount = 0;
	lpMotion->lpMorph = 0;
	lpMotion->dwCacheID = 0;
}

C3_CORE_DLL_API
BOOL Motion_Load ( C3Motion **lpMotion, void *file )
{
	if ( *lpMotion )
		Motion_Unload ( lpMotion );

	*lpMotion = new C3Motion;
	Motion_Clear ( *lpMotion );

	TQFRead ( &( *lpMotion )->dwBoneCount, sizeof ( DWORD ), 1, file );

	TQFRead ( &( *lpMotion )->dwFrames, sizeof ( DWORD ), 1, file );

	( *lpMotion )->matrix = new D3DXMATRIXA16[( *lpMotion )->dwBoneCount];
	for ( DWORD n = 0; n < ( *lpMotion )->dwBoneCount; n++ )
		D3DXMatrixIdentity ( &( *lpMotion )->matrix[n] );

	char kf[4];
	TQFRead ( kf, sizeof ( char ), 4, file );
	if ( kf[0] == 'K' &&
		kf[1] == 'K' &&
		kf[2] == 'E' &&
		kf[3] == 'Y' )
	{
		TQFRead ( &( *lpMotion )->dwKeyFrames, sizeof ( DWORD ), 1, file );
		( *lpMotion )->lpKeyFrame = new C3KeyFrame[( *lpMotion )->dwKeyFrames];

		for ( DWORD kk = 0; kk < ( *lpMotion )->dwKeyFrames; kk++ )
		{
			TQFRead ( &( *lpMotion )->lpKeyFrame[kk].pos,
				sizeof ( DWORD ),
				1,
				file );

			( *lpMotion )->lpKeyFrame[kk].matrix = new D3DXMATRIXA16[( *lpMotion )->dwBoneCount];
			TQFRead ( ( *lpMotion )->lpKeyFrame[kk].matrix,
				sizeof ( D3DXMATRIX ),
				( *lpMotion )->dwBoneCount,
				file );
		}
	}
	else if (	kf[0] == 'Z' &&
		kf[1] == 'K' &&
		kf[2] == 'E' &&
		kf[3] == 'Y' )
	{
		TQFRead ( &( *lpMotion )->dwKeyFrames, sizeof ( DWORD ), 1, file );
		( *lpMotion )->lpKeyFrame = new C3KeyFrame[( *lpMotion )->dwKeyFrames];

		for ( DWORD kk = 0; kk < ( *lpMotion )->dwKeyFrames; kk++ )
		{
			WORD wPos = 0;
			TQFRead ( &wPos,
				sizeof ( WORD ),
				1,
				file );

			( *lpMotion )->lpKeyFrame[kk].pos = ( DWORD )wPos;

			( *lpMotion )->lpKeyFrame[kk].matrix = new D3DXMATRIXA16[( *lpMotion )->dwBoneCount];


			struct DIV_INFO {
				D3DXQUATERNION	quaternion;
				float x, y, z;
			};

			DIV_INFO mx[256];
			TQFRead ( &mx, sizeof ( DIV_INFO ), ( *lpMotion )->dwBoneCount, file );

			for ( DWORD bb = 0; bb < ( *lpMotion )->dwBoneCount; bb++ )
			{
				::D3DXMatrixIdentity( &( *lpMotion )->lpKeyFrame[kk].matrix[bb] );

				::D3DXMatrixRotationQuaternion( &( *lpMotion )->lpKeyFrame[kk].matrix[bb], &mx[bb].quaternion );

				( *lpMotion )->lpKeyFrame[kk].matrix[bb]._41 = mx[bb].x;
				( *lpMotion )->lpKeyFrame[kk].matrix[bb]._42 = mx[bb].y;
				( *lpMotion )->lpKeyFrame[kk].matrix[bb]._43 = mx[bb].z;
				( *lpMotion )->lpKeyFrame[kk].matrix[bb]._44 = 1.0f;
			}
		}
	}
	else if (	kf[0] == 'X' &&
		kf[1] == 'K' &&
		kf[2] == 'E' &&
		kf[3] == 'Y' )
	{
		TQFRead ( &( *lpMotion )->dwKeyFrames, sizeof ( DWORD ), 1, file );
		( *lpMotion )->lpKeyFrame = new C3KeyFrame[( *lpMotion )->dwKeyFrames];

		for ( DWORD kk = 0; kk < ( *lpMotion )->dwKeyFrames; kk++ )
		{
			WORD wPos = 0;
			TQFRead ( &wPos,
				sizeof ( WORD ),
				1,
				file );

			( *lpMotion )->lpKeyFrame[kk].pos = ( DWORD )wPos;

			( *lpMotion )->lpKeyFrame[kk].matrix = new D3DXMATRIXA16[( *lpMotion )->dwBoneCount];

			struct ROW_INFO {
				float row[4][3];
			};

			ROW_INFO mx[256];
			TQFRead ( &mx, sizeof ( ROW_INFO ), ( *lpMotion )->dwBoneCount, file );

			for ( DWORD bb = 0; bb < ( *lpMotion )->dwBoneCount; bb++ )
			{
				D3DXMATRIXA16 *m = &( *lpMotion )->lpKeyFrame[kk].matrix[bb];
				m->_11 = mx[bb].row[0][0]; m->_12 = mx[bb].row[0][1]; m->_13 = mx[bb].row[0][2]; m->_14 = 0.0f;
				m->_21 = mx[bb].row[1][0]; m->_22 = mx[bb].row[1][1]; m->_23 = mx[bb].row[1][2]; m->_24 = 0.0f;
				m->_31 = mx[bb].row[2][0]; m->_32 = mx[bb].row[2][1]; m->_33 = mx[bb].row[2][2]; m->_34 = 0.0f;
				m->_41 = mx[bb].row[3][0]; m->_42 = mx[bb].row[3][1]; m->_43 = mx[bb].row[3][2]; m->_44 = 1.0f;
			}
		}
	}
	else
	{
		TqFSeek ( file, -4, TQSEEK_CUR );

		( *lpMotion )->dwKeyFrames = ( *lpMotion )->dwFrames;
		( *lpMotion )->lpKeyFrame = new C3KeyFrame[( *lpMotion )->dwFrames];

		for ( DWORD kk = 0; kk < ( *lpMotion )->dwFrames; kk++ )
		{
			( *lpMotion )->lpKeyFrame[kk].pos = kk;
			( *lpMotion )->lpKeyFrame[kk].matrix = new D3DXMATRIXA16[( *lpMotion )->dwBoneCount];
		}
		for ( DWORD bb = 0; bb < ( *lpMotion )->dwBoneCount; bb++ )
		{
			for ( DWORD kk = 0; kk < ( *lpMotion )->dwFrames; kk++ )
			{
				TQFRead ( ( *lpMotion )->lpKeyFrame[kk].matrix[bb],
					sizeof ( D3DXMATRIX ),
					1,
					file );
			}
		}
	}

	TQFRead ( &( *lpMotion )->dwMorphCount, sizeof ( DWORD ), 1, file );

	if ( ( *lpMotion )->dwMorphCount > 0 )
	{
		( *lpMotion )->lpMorph = new float[( *lpMotion )->dwMorphCount * ( *lpMotion )->dwFrames];
		TQFRead ( ( *lpMotion )->lpMorph,
				sizeof ( float ),
				( *lpMotion )->dwMorphCount * ( *lpMotion )->dwFrames,
				file );
	}
	g_nMotionCount++;
	return true;
}


C3_CORE_DLL_API
BOOL Motion_Save ( char *lpName, C3Motion *lpMotion, BOOL bNew )
{
	FILE *file = fopen ( lpName, bNew ? "w+b" : "r+b" );
	if ( !file )
		return false;
	fseek ( file, 0, SEEK_END );

	ChunkHeader chunk;
	chunk.byChunkID[0] = 'M';
	chunk.byChunkID[1] = 'O';
	chunk.byChunkID[2] = 'T';
	chunk.byChunkID[3] = 'I';
	chunk.dwChunkSize = 0;
	fwrite ( &chunk, sizeof ( chunk ), 1, file );

	fwrite ( &lpMotion->dwBoneCount, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( &lpMotion->dwFrames, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	char kf[4];
	kf[0] = 'K';
	kf[1] = 'K';
	kf[2] = 'E';
	kf[3] = 'Y';

	fwrite ( kf, sizeof ( char ), 4, file );
	chunk.dwChunkSize += sizeof ( char ) * 4;

	fwrite ( &lpMotion->dwKeyFrames,
			 sizeof ( DWORD ),
			 1,
			 file );
	chunk.dwChunkSize += sizeof ( DWORD );

	for ( DWORD kk = 0; kk < lpMotion->dwKeyFrames; kk++ )
	{
		fwrite ( &lpMotion->lpKeyFrame[kk].pos,
				 sizeof ( DWORD ),
				 1,
				 file );
		chunk.dwChunkSize += sizeof ( DWORD );

		fwrite ( lpMotion->lpKeyFrame[kk].matrix,
				 sizeof ( D3DXMATRIXA16 ),
				 lpMotion->dwBoneCount,
				 file );
		chunk.dwChunkSize += sizeof ( D3DXMATRIX ) * lpMotion->dwBoneCount;
	}

	fwrite ( &lpMotion->dwMorphCount, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( lpMotion->lpMorph,
			 sizeof ( float ),
			 lpMotion->dwMorphCount * lpMotion->dwFrames,
			 file );
	chunk.dwChunkSize += sizeof ( float ) * lpMotion->dwMorphCount * lpMotion->dwFrames;

	fseek ( file, -( int )( chunk.dwChunkSize + sizeof ( chunk ) ), SEEK_CUR );
	fwrite ( &chunk, sizeof ( chunk ), 1, file );
	fseek ( file, 0, SEEK_END );

	fclose ( file );
	return true;
}

C3_CORE_DLL_API
void Motion_Unload ( C3Motion **lpMotion )
{
	for ( DWORD kk = 0; kk < ( *lpMotion )->dwKeyFrames; kk++ )
	{
		SafeDeleteEx ( ( *lpMotion )->lpKeyFrame[kk].matrix );
	}
	SafeDeleteEx ( ( *lpMotion )->lpKeyFrame );

	SafeDeleteEx ( ( *lpMotion )->matrix );
	SafeDeleteEx ( ( *lpMotion )->lpMorph );

	SafeDelete ( *lpMotion );

	g_nMotionCount--;
}

C3_CORE_DLL_API
void Motion_GetMatrix ( C3Motion *lpMotion, DWORD dwBone, D3DXMATRIXA16 
*lpMatrix )
{
	if ( !lpMotion )
		return;

	int sindex = -1;
	int eindex = -1;
	for ( int n = 0; n < ( int )lpMotion->dwKeyFrames; n++ )
	{
		if ( lpMotion->lpKeyFrame[n].pos <= ( DWORD )lpMotion->nFrame )
		{
			if ( sindex == -1 || n > sindex)
				sindex = n;
		}
		if ( lpMotion->lpKeyFrame[n].pos > ( DWORD )lpMotion->nFrame )
		{
			if ( eindex == -1 || n < eindex )
				eindex = n;
		}
	}
	if ( sindex == -1 && eindex > -1 )
	{
		*lpMatrix = lpMotion->lpKeyFrame[eindex].matrix[dwBone];
	}
	else
	if ( sindex > -1 && eindex == -1 )
	{
		*lpMatrix = lpMotion->lpKeyFrame[sindex].matrix[dwBone];
	}
	else
	if ( sindex > -1 && eindex > -1 )
	{
		*lpMatrix = lpMotion->lpKeyFrame[sindex].matrix[dwBone] + 
				   ( ( float )( lpMotion->nFrame - lpMotion->lpKeyFrame[sindex].pos ) / ( float )( lpMotion->lpKeyFrame[eindex].pos - lpMotion->lpKeyFrame[sindex].pos ) ) * 
				   ( lpMotion->lpKeyFrame[eindex].matrix[dwBone] - lpMotion->lpKeyFrame[sindex].matrix[dwBone] );
	}
}

C3_CORE_DLL_API
void Motion_GetMatrix_Blend ( C3MotionTracker tracker, DWORD dwBone, D3DXMATRIXA16 *lpMatrix )
{
	if ( !tracker.lpMotion )
		return;

	if ( !tracker.lpBlendMotion )
	{
		Motion_GetMatrix ( tracker.lpMotion, dwBone, lpMatrix );
		return;
	}

	float weight = tracker.lpBlendMotion->fWeight;

	D3DXMATRIXA16 blend;
	Motion_GetMatrix ( tracker.lpMotion, dwBone, lpMatrix );
	Motion_GetMatrix ( tracker.lpBlendMotion, dwBone, &blend );

	*lpMatrix = ( 1.0f - weight ) * *lpMatrix + blend * weight;
}

C3_CORE_DLL_API
void Phy_Clear ( C3Phy *lpPhy )
{
	lpPhy->lpName = 0;

	lpPhy->dwBlendCount = 0;

	lpPhy->bNormal = false;
	lpPhy->bTwoSide = false;

	lpPhy->dwNVecCount = 0;
	lpPhy->dwAVecCount = 0;
	lpPhy->lpVB = 0;
	lpPhy->vb = 0;

	lpPhy->dwNTriCount = 0;
	lpPhy->dwATriCount = 0;
	lpPhy->lpIB = 0;
	lpPhy->ib = 0;

	lpPhy->lpTexName = 0;
	lpPhy->nTex = -1;
	lpPhy->nTex2 = -1;

	lpPhy->bboxMin.x = lpPhy->bboxMin.y = lpPhy->bboxMin.z = 999999.0f;
	lpPhy->bboxMax.x = lpPhy->bboxMax.y = lpPhy->bboxMax.z = -999999.0f;

	lpPhy->lpMotion = 0;
	lpPhy->lpBlendMotion = 0;

	lpPhy->fA = lpPhy->fR = lpPhy->fG = lpPhy->fB = 1.0f;
	lpPhy->bDraw = true;
	lpPhy->dwTexRow = 1;
	lpPhy->uvstep.x = 0;
	lpPhy->uvstep.y = 0;
	lpPhy->dwStepCount = 0;
	lpPhy->lpCache = 0;
	lpPhy->nBillboard = 0;
	Key_Clear ( &lpPhy->Key );

	D3DXMatrixIdentity ( &lpPhy->InitMatrix );
	D3DXMatrixIdentity ( &lpPhy->matrix );
}

C3_CORE_DLL_API
BOOL Phy_Load ( C3Phy **lpPhy, void *file, BOOL bNormal, BOOL bCalcNormal, BOOL bTex, BOOL bMorph )
{
	*lpPhy = new C3Phy;
	Phy_Clear ( *lpPhy );
	( *lpPhy )->bNormal = bNormal;

	DWORD temp;
	TQFRead ( &temp, sizeof ( unsigned long ), 1, file );
	( *lpPhy )->lpName = new char[temp + 1];
	TQFRead ( ( *lpPhy )->lpName, 1, temp, file );
	( *lpPhy )->lpName[temp] = '\0';

	TQFRead ( &( *lpPhy )->dwBlendCount, sizeof ( DWORD ), 1, file );

	TQFRead ( &( *lpPhy )->dwNVecCount, sizeof ( DWORD ), 1, file );
	TQFRead ( &( *lpPhy )->dwAVecCount, sizeof ( DWORD ), 1, file );

	DWORD dwVecCount = ( *lpPhy )->dwNVecCount + ( *lpPhy )->dwAVecCount;

	if ( FAILED ( g_D3DDevice->CreateVertexBuffer ( dwVecCount * sizeof ( PhyOutVertex ),
													D3DUSAGE_WRITEONLY,
													0,
													D3DPOOL_MANAGED,
													&( *lpPhy )->vb, NULL ) ) )
		return false;

	( *lpPhy )->lpVB = new PhyVertex[dwVecCount];
	ZeroMemory ( ( *lpPhy )->lpVB, sizeof ( PhyVertex ) * dwVecCount );

	DWORD v;
	if ( bNormal )
	{
		if ( !bMorph )
		{
			TQFRead ( ( *lpPhy )->lpVB,
					sizeof ( PhyVertex ),
					dwVecCount,
					file );
		}
		else
		{
			for ( v = 0; v < dwVecCount; v++ )
			{
				TQFRead ( &( *lpPhy )->lpVB[v].pos, sizeof ( D3DXVECTOR3 ), 1, file );
				TqFSeek ( file, sizeof ( D3DXVECTOR3 ) * 3, TQSEEK_CUR );
				TQFRead ( &( *lpPhy )->lpVB[v].u, sizeof ( PhyVertex ) - sizeof ( D3DXVECTOR3 ), 1, file );
			}
		}
	}
	else
	{
		if ( !bMorph )
		{
			for ( v = 0; v < dwVecCount; v++ )
				TQFRead ( &( *lpPhy )->lpVB[v], sizeof ( PhyVertex ) - sizeof ( D3DXVECTOR3 ), 1, file );
		}
		else
		{
			for ( v = 0; v < dwVecCount; v++ )
			{
				TQFRead ( &( *lpPhy )->lpVB[v].pos, sizeof ( D3DXVECTOR3 ), 1, file );
				TqFSeek ( file, sizeof ( D3DXVECTOR3 ) * 3, TQSEEK_CUR );
				TQFRead ( &( *lpPhy )->lpVB[v].u, sizeof ( PhyVertex ) - sizeof ( D3DXVECTOR3 ) * 2, 1, file );
			}
		}
	}

	PhyOutVertex *vertex;
	if ( FAILED ( ( *lpPhy )->vb->Lock ( 0,
										 0,
										 ( void** )&vertex,
										 D3DLOCK_NOSYSLOCK ) ) )
		return false;

	for ( v = 0; v < dwVecCount; v++ )
	{
		vertex[v].x = ( *lpPhy )->lpVB[v].pos.x;
		vertex[v].y = ( *lpPhy )->lpVB[v].pos.y;
		vertex[v].z = ( *lpPhy )->lpVB[v].pos.z;
		vertex[v].nx = ( *lpPhy )->lpVB[v].normal.x;
		vertex[v].ny = ( *lpPhy )->lpVB[v].normal.y;
		vertex[v].nz = ( *lpPhy )->lpVB[v].normal.z;
		vertex[v].color = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
		vertex[v].u = ( *lpPhy )->lpVB[v].u;
		vertex[v].v = ( *lpPhy )->lpVB[v].v;
	}

	TQFRead ( &( *lpPhy )->dwNTriCount, sizeof ( DWORD ), 1, file );
	TQFRead ( &( *lpPhy )->dwATriCount, sizeof ( DWORD ), 1, file );

	DWORD dwTriCount = ( *lpPhy )->dwNTriCount + ( *lpPhy )->dwATriCount;

	if ( FAILED ( g_D3DDevice->CreateIndexBuffer ( dwTriCount * sizeof ( WORD ) * 3,
												   D3DUSAGE_WRITEONLY,
												   D3DFMT_INDEX16,
												   D3DPOOL_MANAGED,
												   &( *lpPhy )->ib, NULL ) ) )
	{
		( *lpPhy )->vb->Unlock ();
		( *lpPhy )->vb->Release ();
		return false;
	}

	WORD *tri;
	if ( FAILED ( ( *lpPhy )->ib->Lock ( 0,
										 0,
										 ( void** )&tri,
										 D3DLOCK_NOSYSLOCK ) ) )
	{
		( *lpPhy )->ib->Release ();
		( *lpPhy )->vb->Unlock ();
		( *lpPhy )->vb->Release ();
		return false;
	}
	TQFRead ( tri, sizeof ( WORD ), dwTriCount * 3, file );

	( *lpPhy )->lpIB = new WORD[dwTriCount * 3];
	CopyMemory ( ( *lpPhy )->lpIB, tri, sizeof ( WORD ) * dwTriCount * 3 );

	if ( !bNormal && bCalcNormal )
	{
		DWORD n;
		for ( n = 0; n < dwTriCount; n++ )
		{
			WORD i0 = tri[n * 3];
			WORD i1 = tri[n * 3 + 1];
			WORD i2 = tri[n * 3 + 2];

			D3DXVECTOR3 v0 ( vertex[i0].x, vertex[i0].y, vertex[i0].z );
			D3DXVECTOR3 v1 ( vertex[i1].x, vertex[i1].y, vertex[i1].z );
			D3DXVECTOR3 v2 ( vertex[i2].x, vertex[i2].y, vertex[i2].z );

			D3DXVECTOR3 e0 = v1 - v0;
			D3DXVECTOR3 e1 = v2 - v1;
			D3DXVECTOR3 normal;
			D3DXVec3Cross ( &normal, &e1, &e0 );
			D3DXVec3Normalize ( &normal, &normal );

			vertex[i0].nx += normal.x; vertex[i0].ny += normal.y; vertex[i0].nz += normal.z;
			vertex[i1].nx += normal.x; vertex[i1].ny += normal.y; vertex[i1].nz += normal.z;
			vertex[i2].nx += normal.x; vertex[i2].ny += normal.y; vertex[i2].nz += normal.z;
		}

		for ( v = 0; v < dwVecCount; v++ )
		{
			D3DXVec3Normalize ( ( D3DXVECTOR3* )&vertex[v].nx, ( D3DXVECTOR3* )&vertex[v].nx );

			std::vector<int> same;
			for ( n = 0; n < v; n++ )
			{
				if ( FloatCmp ( vertex[v].x, vertex[n].x, 0.01f ) == 0 &&
					 FloatCmp ( vertex[v].y, vertex[n].y, 0.01f ) == 0 &&
					 FloatCmp ( vertex[v].z, vertex[n].z, 0.01f ) == 0 )
					same.push_back ( n );
			}
			if ( same.size () == 0 )
				continue;

			D3DXVECTOR3 normal ( vertex[v].nx, vertex[v].ny, vertex[v].nz );
			for ( n = 0; n < same.size (); n++ )
			{
				normal = normal + *( D3DXVECTOR3* )&vertex[same[n]].nx;
				D3DXVec3Normalize ( &normal, &normal );
			}

			same.push_back ( v );
			for ( n = 0; n < same.size (); n++ )
			{
				vertex[same[n]].nx = normal.x;
				vertex[same[n]].ny = normal.y;
				vertex[same[n]].nz = normal.z;
			}
		}

		for ( v = 0; v < dwVecCount; v++ )
		{
			( *lpPhy )->lpVB[v].normal.x = vertex[v].nx;
			( *lpPhy )->lpVB[v].normal.y = vertex[v].ny;
			( *lpPhy )->lpVB[v].normal.z = vertex[v].nz;
		}
		( *lpPhy )->bNormal = true;
	}

	( *lpPhy )->ib->Unlock ();
	( *lpPhy )->vb->Unlock ();

	TQFRead ( &temp, sizeof ( DWORD ), 1, file );
	( *lpPhy )->lpTexName = new char[temp + 1];
	TQFRead ( ( *lpPhy )->lpTexName, 1, temp, file );
	( *lpPhy )->lpTexName[temp] = '\0';
	if ( bTex )
	{
		C3Texture *tex;
		( *lpPhy )->nTex = Texture_Load ( &tex, ( *lpPhy )->lpTexName );
		if ( ( *lpPhy )->nTex == -1 )
			return false;
	}

	TQFRead ( &( *lpPhy )->bboxMin, sizeof ( D3DXVECTOR3 ), 1, file );
	TQFRead ( &( *lpPhy )->bboxMax, sizeof ( D3DXVECTOR3 ), 1, file );

	TQFRead ( &( *lpPhy )->InitMatrix, sizeof ( D3DXMATRIXA16 ), 1, file );

	TQFRead ( &( *lpPhy )->dwTexRow, sizeof ( DWORD ), 1, file );

	TQFRead ( &( *lpPhy )->Key.dwAlphas, sizeof ( DWORD ), 1, file );
	if ( ( *lpPhy )->Key.dwAlphas > 0 )
	{
		( *lpPhy )->Key.lpAlphas = new C3Frame[( *lpPhy )->Key.dwAlphas];
		TQFRead ( ( *lpPhy )->Key.lpAlphas, sizeof ( C3Frame ), ( *lpPhy )->Key.dwAlphas, file );
	}

	TQFRead ( &( *lpPhy )->Key.dwDraws, sizeof ( DWORD ), 1, file );
	if ( ( *lpPhy )->Key.dwDraws > 0 )
	{
		( *lpPhy )->Key.lpDraws = new C3Frame[( *lpPhy )->Key.dwDraws];
		TQFRead ( ( *lpPhy )->Key.lpDraws, sizeof ( C3Frame ), ( *lpPhy )->Key.dwDraws, file );
	}

	TQFRead ( &( *lpPhy )->Key.dwChangeTexs, sizeof ( DWORD ), 1, file );
	if ( ( *lpPhy )->Key.dwChangeTexs > 0 )
	{
		( *lpPhy )->Key.lpChangeTexs = new C3Frame[( *lpPhy )->Key.dwChangeTexs];
		TQFRead ( ( *lpPhy )->Key.lpChangeTexs, sizeof ( C3Frame ), ( *lpPhy )->Key.dwChangeTexs, file );
	}

	char flag[4];
	TQFRead ( flag, sizeof ( char ), 4, file );

	if ( flag[0] == 'S' &&
		 flag[1] == 'T' &&
		 flag[2] == 'E' &&
		 flag[3] == 'P' )
	{
		TQFRead ( &( *lpPhy )->uvstep.x, sizeof ( float ), 1, file );
		TQFRead ( &( *lpPhy )->uvstep.y, sizeof ( float ), 1, file );
	}
	else
		TqFSeek ( file, -4, TQSEEK_CUR ) ;

	TQFRead ( flag, sizeof ( char ), 4, file );

	if ( flag[0] == '2' &&
		 flag[1] == 'S' &&
		 flag[2] == 'I' &&
		 flag[3] == 'D' )
	{
		( *lpPhy )->bTwoSide = true;
		TQFRead ( flag, sizeof ( char ), 4, file );
	}

	if ( flag[0] == 'B' &&
		 flag[1] == 'I' &&
		 flag[2] == 'L' &&
		 flag[3] == 'B' )
	{
		( *lpPhy )->nBillboard = 1;
	}
	else
	if ( flag[0] == 'B' &&
		 flag[1] == 'I' &&
		 flag[2] == 'B' &&
		 flag[3] == '2' )
	{
		( *lpPhy )->nBillboard = 2;
	}
	else
		TqFSeek ( file, -4, TQSEEK_CUR ) ;

	Phy_SetColor ( *lpPhy, 1, 1, 1, 1 );

	( *lpPhy )->bMatrix = false;
	D3DXMatrixIdentity ( &( *lpPhy )->matrixWorld );
	D3DXMatrixIdentity ( &( *lpPhy )->matrixLocal );

	( *lpPhy )->lpCache = new PHY_CACHE;
	return true;
}

C3_CORE_DLL_API
BOOL Phy_Save ( char *lpName, C3Phy *lpPhy, BOOL bNew )
{
	FILE *file = fopen ( lpName, bNew ? "w+b" : "r+b" );
	if ( !file )
		return false;
	fseek ( file, 0, SEEK_END );

	ChunkHeader chunk;
	chunk.byChunkID[0] = 'P';
	chunk.byChunkID[1] = 'H';
	chunk.byChunkID[2] = 'Y';
	chunk.byChunkID[3] = ' ';
	chunk.dwChunkSize = 0;
	fwrite ( &chunk, sizeof ( chunk ), 1, file );

	DWORD length = strlen ( lpPhy->lpName );
	fwrite ( &length, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( lpPhy->lpName, sizeof ( char ), length, file );
	chunk.dwChunkSize += sizeof ( char ) * length;

	fwrite ( &lpPhy->dwBlendCount, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( &lpPhy->dwNVecCount, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );
	fwrite ( &lpPhy->dwAVecCount, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( lpPhy->lpVB,
			 sizeof ( PhyVertex ),
			 lpPhy->dwNVecCount + lpPhy->dwAVecCount,
			 file );
	chunk.dwChunkSize += sizeof ( PhyVertex ) * ( lpPhy->dwNVecCount + 
lpPhy->dwAVecCount );

	fwrite ( &lpPhy->dwNTriCount, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );
	fwrite ( &lpPhy->dwATriCount, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );
	if ( lpPhy->lpIB )
		fwrite ( lpPhy->lpIB, sizeof ( WORD ), ( lpPhy->dwNTriCount + 
lpPhy->dwATriCount ) * 3, file );
	else
	if ( lpPhy->ib )
	{
		WORD *tri;
		if ( FAILED ( lpPhy->ib->Lock ( 0,
										0,
										( void** )&tri,
										D3DLOCK_NOSYSLOCK ) ) )
			return false;
		fwrite ( tri, sizeof ( WORD ), ( lpPhy->dwNTriCount + lpPhy->dwATriCount ) 
* 3, file );
		if ( FAILED ( lpPhy->ib->Unlock () ) )
			return false;
	}
	chunk.dwChunkSize += sizeof ( WORD ) * ( lpPhy->dwNTriCount + 
lpPhy->dwATriCount ) * 3;

	length = strlen ( lpPhy->lpTexName );
	fwrite ( &length, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( lpPhy->lpTexName, sizeof ( char ), length, file );
	chunk.dwChunkSize += sizeof ( char ) * length;

	fwrite ( &lpPhy->bboxMin, sizeof ( D3DXVECTOR3 ), 1, file );
	chunk.dwChunkSize += sizeof ( D3DXVECTOR3 );

	fwrite ( &lpPhy->bboxMax, sizeof ( D3DXVECTOR3 ), 1, file );
	chunk.dwChunkSize += sizeof ( D3DXVECTOR3 );

	fwrite ( &lpPhy->InitMatrix, sizeof ( D3DXMATRIXA16 ), 1, file );
	chunk.dwChunkSize += sizeof ( D3DXMATRIXA16 );

	fwrite ( &lpPhy->dwTexRow, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( &lpPhy->Key.dwAlphas, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( lpPhy->Key.lpAlphas, sizeof ( C3Frame ), lpPhy->Key.dwAlphas, file 
);
	chunk.dwChunkSize += sizeof ( C3Frame ) * lpPhy->Key.dwAlphas;

	fwrite ( &lpPhy->Key.dwDraws, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( lpPhy->Key.lpDraws, sizeof ( C3Frame ), lpPhy->Key.dwDraws, file 
);
	chunk.dwChunkSize += sizeof ( C3Frame ) * lpPhy->Key.dwDraws;

	fwrite ( &lpPhy->Key.dwChangeTexs, sizeof ( DWORD ), 1, file );
	chunk.dwChunkSize += sizeof ( DWORD );

	fwrite ( lpPhy->Key.lpChangeTexs, sizeof ( C3Frame ), 
lpPhy->Key.dwChangeTexs, file );
	chunk.dwChunkSize += sizeof ( C3Frame ) * lpPhy->Key.dwChangeTexs;


	char flag[4];
	flag[0] = 'S';
	flag[1] = 'T';
	flag[2] = 'E';
	flag[3] = 'P';

	fwrite ( flag, sizeof ( char ), 4, file );
	chunk.dwChunkSize += sizeof ( char ) * 4;

	fwrite ( &lpPhy->uvstep.x, sizeof ( float ), 1, file );
	chunk.dwChunkSize += sizeof ( float );

	fwrite ( &lpPhy->uvstep.y, sizeof ( float ), 1, file );
	chunk.dwChunkSize += sizeof ( float );

	fseek ( file, -( int )( chunk.dwChunkSize + sizeof ( chunk ) ), SEEK_CUR );
	fwrite ( &chunk, sizeof ( chunk ), 1, file );
	fseek ( file, 0, SEEK_END );

	fclose ( file );
	return true;
}
C3_CORE_DLL_API
void Phy_Unload ( C3Phy **lpPhy )
{
	SafeDeleteEx ( ( *lpPhy )->lpName );
	SafeDeleteEx ( ( *lpPhy )->lpVB );
	SafeRelease ( ( *lpPhy )->vb );
	SafeDeleteEx ( ( *lpPhy )->lpIB );
	SafeRelease ( ( *lpPhy )->ib );
	SafeDeleteEx ( ( *lpPhy )->lpTexName );


	SafeDeleteEx ( ( *lpPhy )->Key.lpAlphas );
	SafeDeleteEx ( ( *lpPhy )->Key.lpDraws );
	SafeDeleteEx ( ( *lpPhy )->Key.lpChangeTexs );

	PHY_CACHE::iterator it;
	for ( it = ( *lpPhy )->lpCache->begin (); it != ( *lpPhy )->lpCache->end (); ++it )
	{
		for ( DWORD i = 0; i < it->second.size (); i++ )
		{
			if ( it->second[i].lpVertex )
			{
				free ( it->second[i].lpVertex );
				it->second[i].lpVertex = 0;
			}
		}
	}
	SafeDelete ( ( *lpPhy )->lpCache );

	SafeDelete ( *lpPhy );

	g_nPhyCount--;
}
C3_CORE_DLL_API
void Phy_Prepare ( void )
{
	SetRenderState ( D3DRS_ZENABLE, true );
	SetRenderState ( D3DRS_ZWRITEENABLE, true );
	SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
	SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );
	SetRenderState ( D3DRS_DITHERENABLE, false );
	SetRenderState ( D3DRS_ALPHATESTENABLE, true );
	SetRenderState ( D3DRS_ALPHAREF, 0x08 );
	SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

	SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

	if ( g_D3DCaps.MaxAnisotropy >= 2 )
	{
		g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MAXANISOTROPY, 2 );
		g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
		g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
		g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	}
	else
	{
		g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	}

	SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	g_D3DDevice->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_NONE );
	g_D3DDevice->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
	g_D3DDevice->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
}
C3_CORE_DLL_API
BOOL Phy_Calculate ( C3Phy *lpPhy, BOOL bLight, int nType, D3DXVECTOR3 vLight )
{
	float alpha = 1.0f;
	if ( Key_ProcessAlpha ( &lpPhy->Key, lpPhy->lpMotion->nFrame,
lpPhy->lpMotion->dwFrames, &alpha ) )
		lpPhy->fA = alpha;

	BOOL draw = true;
	if ( Key_ProcessDraw ( &lpPhy->Key, lpPhy->lpMotion->nFrame, &draw ) )
		lpPhy->bDraw = draw;

	int tex = -1;
	Key_ProcessChangeTex ( &lpPhy->Key, lpPhy->lpMotion->nFrame, &tex );

	if ( !lpPhy->bDraw )
		return true;


	PhyOutVertex *vertex = 0;
	if ( FAILED ( lpPhy->vb->Lock ( 0,
									0,
									( void** )&vertex,
									D3DLOCK_NOSYSLOCK ) ) )
		return false;

	PhyCacheItem *lpCache = 0;
	if ( lpPhy->lpMotion->dwCacheID != 0 && lpPhy->bMatrix && !lpPhy->lpBlendMotion )
	{
		std::vector<PhyCacheItem> &frames = ( *lpPhy->lpCache )[lpPhy->lpMotion->dwCacheID];
		if ( frames.empty () )
		{
			PhyCacheItem empty;
			empty.lpVertex = 0;
			empty.dwSize = 0;
			frames.reserve ( lpPhy->lpMotion->dwFrames );
			frames.resize ( lpPhy->lpMotion->dwFrames, empty );
		}
		lpCache = &frames[lpPhy->lpMotion->nFrame];
	}

	DWORD color = D3DCOLOR_ARGB ( ( int )( lpPhy->fA * 255.0f ),
								  ( int )( lpPhy->fR * 255.0f ),
								  ( int )( lpPhy->fG * 255.0f ),
								  ( int )( lpPhy->fB * 255.0f ) );
	DWORD dwVecCount = lpPhy->dwNVecCount + lpPhy->dwAVecCount;

	float segsize = 1.0f / lpPhy->dwTexRow;
	float du, dv;
	if ( tex < 0 )
	{
		du = lpPhy->dwStepCount * lpPhy->uvstep.x;
		dv = lpPhy->dwStepCount * lpPhy->uvstep.y;
	}
	else
	{
		du = tex % lpPhy->dwTexRow * segsize;
		dv = tex / lpPhy->dwTexRow * segsize;
	}

	if ( !lpCache || !lpCache->lpVertex )
	{
		static D3DXMATRIX bone[256];

		C3MotionTracker tracker;
		tracker.lpMotion = lpPhy->lpMotion;
		tracker.lpBlendMotion = lpPhy->lpBlendMotion;
		for ( DWORD b = 0; b < lpPhy->lpMotion->dwBoneCount; b++ )
		{
			D3DXMATRIXA16 mm;
			Motion_GetMatrix_Blend ( tracker, b, &mm );

			D3DXMatrixMultiply ( &bone[b],
								 &lpPhy->InitMatrix,
								 &mm );

			if ( !lpPhy->bMatrix )
				D3DXMatrixMultiply ( &bone[b],
									 &bone[b],
									 &lpPhy->lpMotion->matrix[b] );
		}

		D3DXVec3Normalize ( &vLight, &vLight );
		vLight.z = -vLight.z;
		vLight.y = -vLight.y;
		vLight.x = -vLight.x;

		for ( DWORD v = 0; v < dwVecCount; v++ )
		{
			D3DXVECTOR3 mix = lpPhy->lpVB[v].pos;

			D3DXMATRIX mm = lpPhy->lpVB[v].weight[0] * bone[lpPhy->lpVB[v].index[0]] +
							lpPhy->lpVB[v].weight[1] * bone[lpPhy->lpVB[v].index[1]];

			D3DXVec3TransformCoord ( ( D3DXVECTOR3* )&vertex[v].x,
									 &mix,
									 &mm );

			if ( lpPhy->bNormal && bLight )
			{
				D3DXVECTOR3 *normal = ( D3DXVECTOR3* )&vertex[v].nx;
				D3DXVec3TransformNormal ( normal,
										  &lpPhy->lpVB[v].normal,
										  &mm );
			}

			vertex[v].color = color;
			vertex[v].u = du + lpPhy->lpVB[v].u;
			vertex[v].v = dv + lpPhy->lpVB[v].v;
		}

		if ( lpCache && !lpCache->lpVertex )
		{
			lpCache->dwSize = dwVecCount * sizeof ( PhyOutVertex );
			lpCache->lpVertex = ( PhyOutVertex* )malloc ( lpCache->dwSize );
			CopyMemory ( lpCache->lpVertex, vertex, lpCache->dwSize );
		}
	}
	else
	{
		CopyMemory ( vertex, lpCache->lpVertex, lpCache->dwSize );

		if ( lpPhy->uvstep.x == 0.0f && lpPhy->uvstep.y == 0.0f && tex < 0 )
		{
			if ( color != vertex[0].color )
			{
				for ( DWORD v = 0; v < dwVecCount; v++ )
					vertex[v].color = color;
			}
		}
		else
		{
			for ( DWORD v = 0; v < dwVecCount; v++ )
			{
				vertex[v].color = color;
				vertex[v].u = du + lpPhy->lpVB[v].u;
				vertex[v].v = dv + lpPhy->lpVB[v].v;
			}
		}
	}

	if ( FAILED ( lpPhy->vb->Unlock () ) )
		return false;

	return true;
}

static BOOL			s_bLight = false;
static D3DLIGHT9	s_Light;
static struct PhyLightInit
{
	PhyLightInit ()
	{
		ZeroMemory ( &s_Light, sizeof ( s_Light ) );
		s_Light.Type = D3DLIGHT_DIRECTIONAL;
		s_Light.Diffuse.r = s_Light.Diffuse.g = s_Light.Diffuse.b = s_Light.Diffuse.a = 1.0f;
		s_Light.Specular.r = s_Light.Specular.g = s_Light.Specular.b = s_Light.Specular.a = 1.0f;
		s_Light.Ambient.r = s_Light.Ambient.g = s_Light.Ambient.b = 0.4f;
		s_Light.Ambient.a = 1.0f;
		s_Light.Direction.x = s_Light.Direction.y = s_Light.Direction.z = 1.0f;
		s_Light.Range = 1000.0f;
		s_Light.Attenuation0 = 1.0f;
	}
} s_LightInit;

C3_CORE_DLL_API
void Phy_SetLight ( BOOL bEnable, float x, float y, float z, DWORD dwDiffuse, DWORD dwAmbient, float fReserved )
{
	s_bLight = bEnable;

	s_Light.Direction.x = x;
	s_Light.Direction.y = y;
	s_Light.Direction.z = z;
	D3DXVec3Normalize ( ( D3DXVECTOR3* )&s_Light.Direction, ( D3DXVECTOR3* )&s_Light.Direction );

	s_Light.Diffuse.a = 1.0f;
	s_Light.Specular.a = 1.0f;
	s_Light.Diffuse.r = s_Light.Specular.r = ( float )( dwDiffuse & 0xFF0000 ) / 65536.0f / 255.0f;
	s_Light.Diffuse.g = s_Light.Specular.g = ( float )( dwDiffuse & 0xFF00 ) / 256.0f / 255.0f;
	s_Light.Diffuse.b = s_Light.Specular.b = ( float )( dwDiffuse & 0xFF ) / 255.0f;

	s_Light.Ambient.r = ( float )( dwAmbient & 0xFF0000 ) / 65536.0f / 255.0f;
	s_Light.Ambient.g = ( float )( dwAmbient & 0xFF00 ) / 256.0f / 255.0f;
	s_Light.Ambient.b = ( float )( dwAmbient & 0xFF ) / 255.0f;
	s_Light.Ambient.a = 1.0f;
}

static float		s_fFluidSpeedU = 1.0f;
static float		s_fFluidSpeedV = 0.0f;

C3_CORE_DLL_API
void Phy_SetFluidReflectSpeed ( float fSpeedU, float fSpeedV )
{
	s_fFluidSpeedU = fSpeedU;
	s_fFluidSpeedV = fSpeedV;
}

static void Phy_SetLighting ( BOOL bZ, BOOL bSpecular )
{
	if ( s_bLight && bZ )
	{
		g_D3DDevice->SetLight ( 0, &s_Light );
		g_D3DDevice->LightEnable ( 0, true );
		SetRenderState ( D3DRS_LIGHTING, true );
		SetRenderState ( D3DRS_SPECULARENABLE, bSpecular );
		SetRenderState ( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
		SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
		SetRenderState ( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
	}
	else
	{
		SetRenderState ( D3DRS_LIGHTING, false );
	}
}

C3_CORE_DLL_API
BOOL Phy_DrawNormal ( C3Phy *lpPhy, D3DMATERIAL8 material, D3DXMATRIXA16 *lpShadow, BOOL bZ, DWORD nAsb, DWORD nAdb, int nType, float fShadowAlpha )
{
	if ( !lpPhy->bDraw )
		return true;
	if ( lpPhy->dwNTriCount + lpPhy->dwATriCount == 0 )
		return true;
	if ( lpPhy->fA <= 0.0f )
		return true;

	SetRenderState ( D3DRS_ZWRITEENABLE, bZ );
	SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
	SetRenderState ( D3DRS_SRCBLEND, nAsb );
	SetRenderState ( D3DRS_DESTBLEND, nAdb );
	SetRenderState ( D3DRS_CULLMODE, lpPhy->bTwoSide ? D3DCULL_NONE : D3DCULL_CW );

	if ( s_bLight && bZ )
	{
		material.Diffuse.a *= lpPhy->fA;

		if ( FAILED ( g_D3DDevice->SetMaterial ( ( D3DMATERIAL9* )&material ) ) )
			return false;

		D3DLIGHT9 light = s_Light;
		light.Ambient.r *= lpPhy->fR;
		light.Ambient.g *= lpPhy->fG;
		light.Ambient.b *= lpPhy->fB;
		g_D3DDevice->SetLight ( 0, &light );
		g_D3DDevice->LightEnable ( 0, true );
		SetRenderState ( D3DRS_LIGHTING, true );
		SetRenderState ( D3DRS_SPECULARENABLE, material.Power > 0.0f );
		SetRenderState ( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
		SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
		SetRenderState ( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
	}
	else
	{
		SetRenderState ( D3DRS_LIGHTING, false );
	}

	if ( lpPhy->nTex == -1 )
	{
		SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
		g_D3DDevice->SetTexture ( 0, 0 );
	}
	else
	{
		if ( !g_lpTex[lpPhy->nTex] )
			return false;

		if ( lpPhy->nTex2 == -1 )
		{
			if ( nType == 0x13 )
			{
				SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
				SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
				SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
				SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
				SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
			}
			else
			{
				SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
				SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
				SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
				SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
			}
			g_D3DDevice->SetTexture ( 0, g_lpTex[lpPhy->nTex]->lpTex );

			SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
			SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			g_D3DDevice->SetTexture ( 1, 0 );
		}
		else
		{
			switch ( nType )
			{
			case 0x11:
				SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
				SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
				SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
				SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
				g_D3DDevice->SetTexture ( 0, g_lpTex[lpPhy->nTex]->lpTex );

				SetTextureStageState ( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );
				SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
				SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
				SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
				SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
				g_D3DDevice->SetTexture ( 1, g_lpTex[lpPhy->nTex2]->lpTex );
				break;

			case 0x12:
				SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );
				SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
				SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
				SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
				g_D3DDevice->SetTexture ( 0, g_lpTex[lpPhy->nTex2]->lpTex );

				SetTextureStageState ( 1, D3DTSS_TEXCOORDINDEX, 0 );
				SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
				SetTextureStageState ( 1, D3DTSS_COLOROP, 0x12 );
				SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
				SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
				g_D3DDevice->SetTexture ( 1, g_lpTex[lpPhy->nTex]->lpTex );
				break;

			case 0x13:
				SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
				SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
				SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
				SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
				g_D3DDevice->SetTexture ( 0, g_lpTex[lpPhy->nTex]->lpTex );

				SetTextureStageState ( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
				SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
				SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
				SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
				SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
				if ( g_lpTex[lpPhy->nTex2] )
					g_D3DDevice->SetTexture ( 1, g_lpTex[lpPhy->nTex2]->lpTex );
				else
					g_D3DDevice->SetTexture ( 1, g_lpTex[lpPhy->nTex]->lpTex );
				break;

			case 0x15:
			case 0x16:
				SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
				SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
				SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
				g_D3DDevice->SetTexture ( 0, g_lpTex[lpPhy->nTex]->lpTex );

				SetTextureStageState ( 1, D3DTSS_TEXCOORDINDEX, 1 );
				SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
				SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
				SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
				g_D3DDevice->SetTexture ( 1, g_lpTex[lpPhy->nTex2]->lpTex );
				break;

			case 0x17:
				{
					SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
					SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
					SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
					SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
					SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
					SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
					SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
					g_D3DDevice->SetTexture ( 0, g_lpTex[lpPhy->nTex]->lpTex );

					SetTextureStageState ( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );
					SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
					SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
					SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
					SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
					SetTextureStageState ( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
					SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
					SetTextureStageState ( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );

					D3DXMATRIX fluid = g_IDMatrix;
					float fTime = ( float )timeGetTime () * 0.001f;
					fluid._41 = s_fFluidSpeedU * fTime;
					fluid._42 = s_fFluidSpeedV * fTime;
					g_D3DDevice->SetTransform ( D3DTS_TEXTURE1, &fluid );
					g_D3DDevice->SetTexture ( 1, g_lpTex[lpPhy->nTex2]->lpTex );
				}
				break;

			case 2:
			case 4: case 5: case 6: case 7: case 8: case 9: case 0xA: case 0xB:
			case 0xD: case 0xE: case 0xF: case 0x10:
				SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
				SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
				SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
				SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
				g_D3DDevice->SetTexture ( 0, g_lpTex[lpPhy->nTex]->lpTex );

				if ( nType == 0xE )
				{
					float fTime = ( float )timeGetTime () / 4000.0f * 3.1415926f;
					int nAlpha = ( int )( ( sin ( ( double )fTime ) + 1.0 ) * 0.5 * 255.0 );
					SetRenderState ( D3DRS_TEXTUREFACTOR, ( nAlpha << 24 ) | 0xFFFFFF );
				}

				SetTextureStageState ( 1, D3DTSS_TEXCOORDINDEX, 0 );
				SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
				SetTextureStageState ( 1, D3DTSS_COLOROP, nType );
				SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				SetTextureStageState ( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
				SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
				g_D3DDevice->SetTexture ( 1, g_lpTex[lpPhy->nTex2]->lpTex );
				break;

			default:
				SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
				g_D3DDevice->SetTexture ( 1, 0 );
				break;
			}

			SetTextureStageState ( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
			g_D3DDevice->SetTexture ( 2, 0 );
		}
	}

	D3DXMATRIXA16 matrix;
	if ( lpPhy->bMatrix )
		matrix = lpPhy->matrix;
	else
		matrix = g_IDMatrix;
	g_D3DDevice->SetTransform ( D3DTS_WORLD, &matrix );
	if ( FAILED ( g_D3DDevice->SetFVF ( PHY_OUT_VERTEX ) ) )
		return false;

	if ( FAILED ( g_D3DDevice->SetStreamSource ( 0, lpPhy->vb, 0, sizeof ( PhyOutVertex ) ) ) )
		return false;
	if ( FAILED ( g_D3DDevice->SetIndices ( lpPhy->ib ) ) )
		return false;

	if ( FAILED ( g_D3DDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0,
													  0,
													  lpPhy->dwNVecCount + lpPhy->dwAVecCount,
													  0,
													  lpPhy->dwNTriCount + lpPhy->dwATriCount ) ) )
		return false;
	g_dwTriTotal += lpPhy->dwNTriCount + lpPhy->dwATriCount;

	if ( nType == 0x17 )
		SetTextureStageState ( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

	if ( lpShadow )
	{
		SetRenderState ( D3DRS_LIGHTING, true );
		SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESS );
		SetRenderState ( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
		SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
		SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );

		ZeroMemory ( &material, sizeof ( material ) );
		material.Diffuse.a = fShadowAlpha;
		g_D3DDevice->SetMaterial ( ( D3DMATERIAL9* )&material );

		g_D3DDevice->SetTexture ( 0, 0 );
		g_D3DDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
		g_D3DDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

		D3DXMATRIXA16 shadow;
		D3DXMatrixMultiply ( &shadow, &lpPhy->matrixLocal, lpShadow );
		D3DXMatrixMultiply ( &shadow, &shadow, &lpPhy->matrixWorld );
		D3DXMatrixMultiply ( &shadow, &shadow, &g_ViewMatrix );

		D3DXMATRIXA16 flat;
		D3DXMatrixIdentity ( &flat );
		g_D3DDevice->SetTransform ( D3DTS_VIEW, &flat );
		flat._43 = g_ViewMatrix._43 * 1.5f;
		flat._33 = 0.0f;
		D3DXMatrixMultiply ( &shadow, &shadow, &flat );
		g_D3DDevice->SetTransform ( D3DTS_WORLD, &shadow );

		g_D3DDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0,
											0,
											lpPhy->dwNVecCount + lpPhy->dwAVecCount,
											0,
											lpPhy->dwNTriCount + lpPhy->dwATriCount );
		g_dwTriTotal += lpPhy->dwNTriCount + lpPhy->dwATriCount;

		SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
		g_D3DDevice->SetTransform ( D3DTS_VIEW, &g_ViewMatrix );
	}

	if ( nType == 0x14 || nType == 0x16 )
	{
		g_D3DDevice->SetTransform ( D3DTS_WORLD, &matrix );

		ZeroMemory ( &material, sizeof ( material ) );
		material.Diffuse.a = 1.0f;
		material.Diffuse.r = 0;
		material.Diffuse.g = 0;
		material.Diffuse.b = 0;
		g_D3DDevice->SetMaterial ( ( D3DMATERIAL9* )&material );

		SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );
		SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );

		PhyOutVertex *vertex = 0;
		lpPhy->vb->Lock ( 0, 0, ( void** )&vertex, 0 );
		for ( DWORD v = 0; v < lpPhy->dwNVecCount + lpPhy->dwAVecCount; v++ )
		{
			vertex[v].x = vertex[v].nx + vertex[v].nx + vertex[v].x;
			vertex[v].y = vertex[v].ny + vertex[v].ny + vertex[v].y;
			vertex[v].z = vertex[v].nz + vertex[v].nz + vertex[v].z;
		}
		lpPhy->vb->Unlock ();

		g_D3DDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0,
											0,
											lpPhy->dwNVecCount + lpPhy->dwAVecCount,
											0,
											lpPhy->dwNTriCount + lpPhy->dwATriCount );
		g_dwTriTotal += lpPhy->dwNTriCount + lpPhy->dwATriCount;
	}

	SetRenderState ( D3DRS_LIGHTING, false );
	return true;
}
C3_CORE_DLL_API
BOOL Phy_DrawAlpha ( C3Phy *lpPhy, BOOL bZ, DWORD nAsb, DWORD nAdb )
{
	return true;
}

C3_CORE_DLL_API
BOOL Phy_Draw ( PhyOutVertex *lpVertex, DWORD dwTriCount, C3Texture *lpTex, const D3DMATERIAL8 *lpMaterial, DWORD nAsb, DWORD nAdb, BOOL bTwoSide, BOOL bZ )
{
	if ( !lpVertex )
		return false;

	SetRenderState ( D3DRS_ZWRITEENABLE, bZ );
	SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
	SetRenderState ( D3DRS_SRCBLEND, nAsb );
	SetRenderState ( D3DRS_DESTBLEND, nAdb );
	SetRenderState ( D3DRS_CULLMODE, bTwoSide ? D3DCULL_NONE : D3DCULL_CW );

	if ( lpMaterial )
	{
		if ( FAILED ( g_D3DDevice->SetMaterial ( ( const D3DMATERIAL9* )lpMaterial ) ) )
			return false;
	}

	Phy_SetLighting ( bZ, !lpMaterial || lpMaterial->Power > 0.0f );

	if ( !lpTex )
	{
		SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
		g_D3DDevice->SetTexture ( 0, 0 );
	}
	else
	{
		SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
		SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
		SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		g_D3DDevice->SetTexture ( 0, lpTex->lpTex );

		SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		g_D3DDevice->SetTexture ( 1, 0 );
	}

	D3DXMATRIXA16 matrix;
	D3DXMatrixIdentity ( &matrix );
	g_D3DDevice->SetTransform ( D3DTS_WORLD, &matrix );

	if ( FAILED ( g_D3DDevice->SetFVF ( PHY_OUT_VERTEX ) ) )
		return false;

	g_D3DDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, dwTriCount, lpVertex, sizeof ( PhyOutVertex ) );

	SetRenderState ( D3DRS_LIGHTING, false );
	return true;
}

C3_CORE_DLL_API
void Phy_NextFrame ( C3Phy *lpPhy, int nStep )
{
	lpPhy->lpMotion->nFrame = ( lpPhy->lpMotion->nFrame + nStep ) %
lpPhy->lpMotion->dwFrames;
}

C3_CORE_DLL_API
void Phy_SetFrame ( C3Phy *lpPhy, DWORD dwFrame )
{
	if ( lpPhy->lpMotion )
		lpPhy->lpMotion->nFrame = dwFrame % lpPhy->lpMotion->dwFrames;
}

C3_CORE_DLL_API
void Phy_SetBlendFrame ( C3Phy *lpPhy, DWORD dwFrame )
{
	if ( lpPhy->lpBlendMotion )
		lpPhy->lpBlendMotion->nFrame = dwFrame % lpPhy->lpBlendMotion->dwFrames;
}

C3_CORE_DLL_API
void Phy_SetStepCount ( C3Phy *lpPhy, DWORD dwStepCount )
{
	if ( lpPhy )
		lpPhy->dwStepCount = dwStepCount;
}

C3_CORE_DLL_API
void Phy_Muliply ( C3Phy *lpPhy, int nBoneIndex, D3DXMATRIXA16 *matrix )
{
	int start, end;
	if ( nBoneIndex == -1 )
	{
		start = 0;
		end = lpPhy->lpMotion->dwBoneCount;
	}
	else
	{
		start = nBoneIndex;
		end = start + 1;
	}
	for ( int n = start; n < end; n++ )
	{
		D3DXMatrixMultiply ( &lpPhy->lpMotion->matrix[n],
							 &lpPhy->lpMotion->matrix[n],
							 matrix );
	}
}
C3_CORE_DLL_API
void Phy_SetColor ( C3Phy *lpPhy,
					float alpha,
					float red,
					float green,
					float blue )
{
	lpPhy->fA = alpha;
	lpPhy->fR = red;
	lpPhy->fG = green;
	lpPhy->fB = blue;
}

C3_CORE_DLL_API
void Phy_ClearMatrix ( C3Phy *lpPhy )
{
	for ( DWORD n = 0; n < lpPhy->lpMotion->dwBoneCount; n ++ )
		D3DXMatrixIdentity ( &lpPhy->lpMotion->matrix[n] );

	lpPhy->bMatrix = false;
	D3DXMatrixIdentity ( &lpPhy->matrixWorld );
	D3DXMatrixIdentity ( &lpPhy->matrixLocal );
	D3DXMatrixIdentity ( &lpPhy->matrix );
}

C3_CORE_DLL_API
void Phy_ChangeTexture ( C3Phy *lpPhy, int nTexID )
{
	lpPhy->nTex = nTexID;
}

C3_CORE_DLL_API
void Phy_ChangeMixTexture ( C3Phy *lpPhy, int nTexID )
{
	lpPhy->nTex2 = nTexID;
}
