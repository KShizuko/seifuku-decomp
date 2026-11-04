#pragma warning(disable:4786)
#include	"c3_main.h"
#include	"c3_datafile.h"

#include    <io.h>

C3_CORE_DLL_API
void AssertDialog ( char cExpression[], char cFile[], int nLine )
{
    char OutputMessage[256];
	wsprintf ( OutputMessage, "Assert ( %s );\nFile: %s\nLine: %d", cExpression, cFile, nLine );
	ErrorMessage ( OutputMessage );
}
C3_CORE_DLL_API
void ErrorMessage ( char cMessage[] )
{
	switch ( MessageBox ( g_hWnd, cMessage, "ERROR MESSAGE", MB_ABORTRETRYIGNORE ) )
	{
	case IDABORT:
		raise ( SIGABRT );
		exit ( 3 );
		break;
	case IDRETRY:
		_asm int 3;
		break;
	case IDIGNORE:
		ShowWindow ( GetActiveWindow (), SW_SHOW );
		break;
	};
}

C3_CORE_DLL_API
int Random ( int nMin, int nMax )
{
	int ra = rand ();
	return ra % ( nMax + 1 - nMin ) + nMin;
}
C3_CORE_DLL_API
int FloatCmp ( float f0, float f1, float fDim )
{
	if ( fabs ( f0 - f1 ) < fDim )
		return 0;
	if ( f0 > f1 )
		return 1;
	return -1;
}

C3_CORE_DLL_API
void CutString ( char *lpString, DWORD dwLevel )
{
	if ( dwLevel == 0 )
		return;

	DWORD nowS = 0;
	DWORD l;
	for ( l = 0; l < strlen ( lpString ); l++ )
	{
		if ( lpString[l] == '\\' )
		{
			nowS++;
			if ( nowS == dwLevel )
			{
				l++;
				break;
			}
		}
	}
	if ( nowS < dwLevel )
		return;

	DWORD n;
	for ( n = 0; n < strlen ( lpString ) - l; n++ )
		lpString[n] = lpString[n + l];

	lpString[n] = '\0';
}

C3_CORE_DLL_API
void Common_Translate ( D3DXMATRIX *matrix, float x, float y, float z )
{
	matrix->_41 += x;
	matrix->_42 += y;
	matrix->_43 += z;
}
C3_CORE_DLL_API
void Common_Rotate ( D3DXMATRIX *matrix, float x, float y, float z )
{
	D3DXMATRIX m;

	D3DXMatrixRotationX ( &m, x );
	D3DXMatrixMultiply ( matrix,
					     matrix,
						 &m );

	D3DXMatrixRotationY ( &m, y );
	D3DXMatrixMultiply ( matrix,
					     matrix,
						 &m );

	D3DXMatrixRotationZ ( &m, z );
	D3DXMatrixMultiply ( matrix,
					     matrix,
						 &m );
}
C3_CORE_DLL_API
void Common_Scale ( D3DXMATRIX *matrix, float x, float y, float z )
{
	matrix->_11 *= x; matrix->_21 *= x; matrix->_31 *= x; matrix->_41 *= x;
	matrix->_12 *= y; matrix->_22 *= y; matrix->_32 *= y; matrix->_42 *= y;
	matrix->_13 *= z; matrix->_23 *= z; matrix->_33 *= z; matrix->_43 *= z;
}
C3_CORE_DLL_API
void Common_Shadow ( D3DXMATRIX *matrix,
					 D3DXVECTOR4 *lightpos,
					 D3DXVECTOR3 *planepoint,
					 D3DXVECTOR3 *planenor )
{
	D3DXPLANE plane;
	D3DXPlaneFromPointNormal ( &plane, planepoint, planenor );

	D3DXMATRIX m;
	D3DXMatrixShadow ( &m, lightpos, &plane );
	D3DXMatrixMultiply ( matrix,
					     matrix,
						 &m );
}

C3_CORE_DLL_API
void BuildRay ( int nX,
				int nY,
				D3DXVECTOR3 *lpOrig,
				D3DXVECTOR3 *lpDir )
{
	D3DXMATRIX matrix;
	g_D3DDevice->GetTransform ( D3DTS_PROJECTION, &matrix );

	D3DXVECTOR3 vec;
	vec.x =  ( ( ( 2.0f * nX ) / g_DisplayMode.Width  ) - 1 ) / matrix._11;
	vec.y = -( ( ( 2.0f * nY ) / g_DisplayMode.Height ) - 1 ) / matrix._22;
	vec.z =  1.0f;

	g_D3DDevice->GetTransform ( D3DTS_VIEW, &matrix );
	D3DXMatrixInverse ( &matrix, 0, &matrix );

	lpDir->x = vec.x * matrix._11 + vec.y * matrix._21 + vec.z * matrix._31;
	lpDir->y = vec.x * matrix._12 + vec.y * matrix._22 + vec.z * matrix._32;
	lpDir->z = vec.x * matrix._13 + vec.y * matrix._23 + vec.z * matrix._33;

	lpOrig->x = matrix._41;
	lpOrig->y = matrix._42;
	lpOrig->z = matrix._43;

	D3DXVec3Normalize ( lpDir, lpDir );
}
C3_CORE_DLL_API
BOOL IntersectPlane ( const D3DXVECTOR3 &Org,
					  const D3DXVECTOR3 &Dir,
					  const D3DXVECTOR3 &PlaneNor,
					  const D3DXVECTOR3 &PlaneVec,
					  D3DXVECTOR3 &Result )
{
	D3DXVECTOR3 vec = Org - PlaneVec;
	float dist = D3DXVec3Dot ( &vec, &PlaneNor );

	D3DXVec3Scale ( &vec, &Dir, dist );
	vec = vec + Org;

	D3DXPLANE plane;
	D3DXPlaneFromPointNormal ( &plane, &PlaneVec, &PlaneNor );
	return D3DXPlaneIntersectLine ( &Result, &plane, &Org, &vec ) != NULL;
}

C3_CORE_DLL_API
BOOL IntersectTriangle ( const D3DXVECTOR3 &Org,
						 const D3DXVECTOR3 &Dir,
						 const D3DXVECTOR3 &V0,
						 const D3DXVECTOR3 &V1,
						 const D3DXVECTOR3 &V2,
						 D3DXVECTOR3 &Result )
{
	static D3DXVECTOR3 edge1, edge2, normal, pvec, tvec, qvec;
	static float det, u, v, t;

	edge2 = V2 - V0;
	edge1 = V1 - V0;
	D3DXVec3Cross ( &normal, &edge2, &edge1 );
	det = D3DXVec3Dot ( &normal, &Dir );
	if ( det > -0.0001f )
		return FALSE;

	D3DXVec3Cross ( &pvec, &Dir, &edge1 );
	det = D3DXVec3Dot ( &pvec, &edge2 );
	if ( det < 0.0001f )
		return FALSE;

	tvec = Org - V0;
	u = D3DXVec3Dot ( &tvec, &pvec );
	if ( u < 0.0f || u > det )
		return FALSE;

	D3DXVec3Cross ( &qvec, &tvec, &edge2 );
	v = D3DXVec3Dot ( &qvec, &Dir );
	if ( v < 0.0f || u + v > det )
		return FALSE;

	t = D3DXVec3Dot ( &qvec, &edge1 ) / det;
	if ( t < 0.0f )
		return FALSE;

	float dist = D3DXVec3Dot ( &tvec, &normal );
	if ( dist < 0.0f )
	{
		if ( dist > -1.0f )
			dist = -1.0f;
	}
	else if ( dist < 1.0f )
		dist = 1.0f;

	tvec = Dir * dist;
	tvec += Org;

	D3DXPLANE plane;
	D3DXPlaneFromPointNormal ( &plane, &V0, &normal );
	return D3DXPlaneIntersectLine ( &Result, &plane, &Org, &tvec ) != NULL;
}