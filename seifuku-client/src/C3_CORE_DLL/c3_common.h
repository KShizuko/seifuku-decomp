#ifndef _c3_common_h_
#define	_c3_common_h_

#ifdef	C3_CORE_DLL_EXPORTS
#define C3_CORE_DLL_API __declspec ( dllexport )
#else
#define C3_CORE_DLL_API __declspec ( dllimport )
#endif

#ifndef	STRICT
#define	STRICT
#endif

#ifndef	WIN32_LEAN_AND_MEAN
#define	WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdio.h>
#include <math.h>
#include <process.h>
#include <signal.h>

#include <d3d9.h>
#include "d3dx8compat.h"

#ifdef _DEBUG
	#define	Assert(Expression) ( ( Expression ) ? ( void ) NULL : ( void ) AssertDialog ( #Expression, __FILE__, __LINE__ ) )
#else
	#define	Assert(Expression) ( void ) NULL
#endif

C3_CORE_DLL_API
void AssertDialog ( char cExpression[], char cFile[], int nLine );
C3_CORE_DLL_API
void ErrorMessage ( char cMessage[] );

#define		SafeRelease(COM)			{ if ( COM ) { COM->Release (); COM = 0; } }
#define		SafeDelete(Pointer)			{ if ( Pointer ) { delete Pointer, Pointer = 0; } }
#define		SafeDeleteEx(Pointer)		{ if ( Pointer ) { delete [] Pointer, Pointer = 0; } }

C3_CORE_DLL_API
int Random ( int nMin, int nMax );
C3_CORE_DLL_API
int FloatCmp ( float f0, float f1, float fDim = 0.0001f );

C3_CORE_DLL_API
void CutString ( char *lpString, DWORD dwLevel );

struct ChunkHeader
{
	BYTE byChunkID[4];
	DWORD dwChunkSize;
};

C3_CORE_DLL_API
void* Common_OpenRes ( LPCSTR name, DWORD &size, int bPack );
C3_CORE_DLL_API
void Common_CloseRes ( void *file );
C3_CORE_DLL_API
void Common_GetChunk ( void *file, ChunkHeader *chunk );
C3_CORE_DLL_API
DWORD Common_ReadRes ( void *file, void *buffer, DWORD size );
C3_CORE_DLL_API
void Common_SeekRes ( void *file, int seek, int origin );
C3_CORE_DLL_API
BOOL Common_IsEofRes ( void *file );
C3_CORE_DLL_API
long Common_GetSeek ( void *file );
C3_CORE_DLL_API
void Common_Translate ( D3DXMATRIX *matrix, float x, float y, float z );
C3_CORE_DLL_API
void Common_Rotate ( D3DXMATRIX *matrix, float x, float y, float z );
C3_CORE_DLL_API
void Common_Scale ( D3DXMATRIX *matrix, float x, float y, float z );
C3_CORE_DLL_API
void Common_Shadow ( D3DXMATRIX *matrix,
					 D3DXVECTOR4 *lightpos,
					 D3DXVECTOR3 *planepoint,
					 D3DXVECTOR3 *planenor );
C3_CORE_DLL_API
void BuildRay ( int nX,
				int nY,
				D3DXVECTOR3 *lpOrig,
				D3DXVECTOR3 *lpDir );

C3_CORE_DLL_API
BOOL IntersectPlane ( const D3DXVECTOR3 &Org,
					  const D3DXVECTOR3 &Dir,
					  const D3DXVECTOR3 &PlaneNor,
					  const D3DXVECTOR3 &PlaneVec,
					  D3DXVECTOR3 &Result );

C3_CORE_DLL_API
BOOL IntersectTriangle ( const D3DXVECTOR3 &Org,
						 const D3DXVECTOR3 &Dir,
						 const D3DXVECTOR3 &V0,
						 const D3DXVECTOR3 &V1,
						 const D3DXVECTOR3 &V2,
						 D3DXVECTOR3 &Result );

#endif
