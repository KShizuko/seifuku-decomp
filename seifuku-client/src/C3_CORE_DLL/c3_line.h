#ifndef _c3_line_h_
#define _c3_line_h_

#ifdef	C3_CORE_DLL_EXPORTS
#define C3_CORE_DLL_API __declspec ( dllexport )
#else
#define C3_CORE_DLL_API __declspec ( dllimport )
#endif

#include "c3_common.h"

#define LINE_VERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE )
struct LineVertex
{
	float							x, y, z;
	DWORD							color;
};

#define		_LINE_MAX_				256

struct C3Line
{
	float							fA, fR, fG, fB;
	D3DXMATRIX						matrix;
	DWORD							dwCount;
	LineVertex						vertex[_LINE_MAX_];
};

C3_CORE_DLL_API
void Line_SetColor ( C3Line *lpLine,
					 float alpha,
					 float red,
					 float green,
					 float blue );
C3_CORE_DLL_API
void Line_ClearMatrix ( C3Line *lpLine );
C3_CORE_DLL_API
void Line_Clear ( C3Line *lpLine );
C3_CORE_DLL_API
void Line_Prepare ( void );
C3_CORE_DLL_API
void Line_Draw ( C3Line *lpLine );
C3_CORE_DLL_API
void Line_Muliply ( C3Line *lpLine, D3DXMATRIX *matrix );

#endif
