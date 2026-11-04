#ifndef _c3_point_h_
#define _c3_point_h_

#ifdef	C3_CORE_DLL_EXPORTS
#define C3_CORE_DLL_API __declspec ( dllexport )
#else
#define C3_CORE_DLL_API __declspec ( dllimport )
#endif

#include "c3_common.h"

#define POINT_VERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE )
struct PointVertex
{
	float							x, y, z;
	DWORD							color;
};

#define		_POINT_MAX_				256

struct C3Point
{
	float							fA, fR, fG, fB;
	D3DXMATRIX						matrix;
	DWORD							dwCount;
	PointVertex						vertex[_POINT_MAX_];
};

C3_CORE_DLL_API
void Point_Prepare ( void );
C3_CORE_DLL_API
void Point_Draw ( C3Point *lpPoint );
C3_CORE_DLL_API
void Point_SetColor ( C3Point *lpPoint,
					  float alpha,
					  float red,
					  float green,
					  float blue );
C3_CORE_DLL_API
void Point_ClearMatrix ( C3Point *lpPoint );
C3_CORE_DLL_API
void Point_Clear ( C3Point *lpPoint );
C3_CORE_DLL_API
void Point_Muliply ( C3Point *lpPoint, D3DXMATRIX *matrix );

#endif
