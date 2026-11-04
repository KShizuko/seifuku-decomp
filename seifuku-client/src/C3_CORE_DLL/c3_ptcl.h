#ifndef _c3_ptcl_h_
#define _c3_ptcl_h_

#ifdef	C3_CORE_DLL_EXPORTS
#define C3_CORE_DLL_API __declspec ( dllexport )
#else
#define C3_CORE_DLL_API __declspec ( dllimport )
#endif

#include "c3_main.h"
#include "c3_sprite.h"
#include "c3_phy.h"
#include <map>

#define PTCL_VERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
struct PtclVertex
{
	float x, y, z;
	DWORD color;
	float u, v;
};

struct	PtclFrame
{
	DWORD						dwCount;
	D3DXVECTOR3					*lpPos;
	float						*lpAge;
	float						*lpSize;
	D3DXMATRIX					matrix;
};

struct C3Ptcl
{
	char						*lpName;
	LPDIRECT3DVERTEXBUFFER9		lpVB;
	int							nTex;
	char						*lpTexName;
	DWORD						dwCount;
	DWORD						dwRow;

	PtclFrame					*lpPtcl;
	int							nFrame;
	DWORD						dwFrames;

	D3DXMATRIX					matrix;
};
extern C3_CORE_DLL_API int g_nPtclCount;

C3_CORE_DLL_API
void Ptcl_Clear ( C3Ptcl *lpPtcl );
C3_CORE_DLL_API
BOOL Ptcl_Load ( C3Ptcl **lpPtcl,
				 void *file,
				 BOOL bTex = false );
C3_CORE_DLL_API
BOOL Ptcl_Save ( char *lpName, C3Ptcl *lpPtcl, BOOL bNew );
C3_CORE_DLL_API
void Ptcl_Unload ( C3Ptcl **lpPtcl );

C3_CORE_DLL_API
void Ptcl_Prepare ( void );
C3_CORE_DLL_API
BOOL Ptcl_Draw ( C3Ptcl *lpPtcl, DWORD nAsb=5, DWORD nAdb=6);

C3_CORE_DLL_API
void Ptcl_NextFrame ( C3Ptcl *lpPtcl, int nStep );
C3_CORE_DLL_API
void Ptcl_SetFrame ( C3Ptcl *lpPtcl, DWORD dwFrame );
C3_CORE_DLL_API
void Ptcl_Muliply ( C3Ptcl *lpPtcl, D3DXMATRIX *matrix );
C3_CORE_DLL_API
void Ptcl_ClearMatrix ( C3Ptcl *lpPtcl );

C3_CORE_DLL_API
void Ptcl_ChangeTexture ( C3Ptcl *lpPtcl, int nTexID );

struct PtclFrameEx
{
	DWORD						dwCount;
	WORD						*lpID;
	D3DXVECTOR3					*lpPos;
	float						*lpAge;
	float						*lpSize;
	D3DXMATRIX					matrix;
};

struct PTCL_MATRIX
{
	D3DXMATRIX					matrix;
	BOOL						bUse;

	PTCL_MATRIX ()	{ bUse = true; }
};

typedef std::map<int, PTCL_MATRIX, std::less<int>, std::allocator<PTCL_MATRIX> > PTCL_INFO;

struct C3PtclEx
{
	char						*lpName;
	LPDIRECT3DVERTEXBUFFER9		lpVB;
	int							nTex;
	char						*lpTexName;
	DWORD						dwCount;
	DWORD						dwRow;

	PtclFrameEx					*lpPtcl;
	int							nFrame;
	DWORD						dwFrames;

	D3DXMATRIX					matrix;

	PTCL_INFO					*lpInfo;
};

C3_CORE_DLL_API
void Ptcl_Clear ( C3PtclEx *lpPtcl );
C3_CORE_DLL_API
BOOL Ptcl_Load ( C3PtclEx **lpPtcl,
				 void *file,
				 BOOL bTex = false );
C3_CORE_DLL_API
void Ptcl_Unload ( C3PtclEx **lpPtcl );
C3_CORE_DLL_API
BOOL Ptcl_Draw ( C3PtclEx *lpPtcl, DWORD nAsb = D3DBLEND_SRCALPHA, DWORD nAdb = D3DBLEND_INVSRCALPHA );
C3_CORE_DLL_API
void Ptcl_SetFrame ( C3PtclEx *lpPtcl, DWORD dwFrame );
C3_CORE_DLL_API
void Ptcl_Muliply ( C3PtclEx *lpPtcl, D3DXMATRIX *matrix );
C3_CORE_DLL_API
void Ptcl_ClearMatrix ( C3PtclEx *lpPtcl );
C3_CORE_DLL_API
void Ptcl_ChangeTexture ( C3PtclEx *lpPtcl, int nTexID );

C3_CORE_DLL_API
PTCL_INFO* Ptcl_CreateInfo ( int nCount );
C3_CORE_DLL_API
void Ptcl_DestroyInfo ( PTCL_INFO *lpInfo );

struct PTCL3_MATRIX
{
	D3DXMATRIX					matrix;
	BOOL						bUse;
	D3DXVECTOR3					pos;
	DWORD						dwTime;
	float						fSpin;

	PTCL3_MATRIX ()	{ bUse = true; dwTime = 0; fSpin = 0.0f; }
};

typedef std::map<int, PTCL3_MATRIX, std::less<int>, std::allocator<PTCL3_MATRIX> > PTCL3_INFO;

struct C3Ptcl3
{
	char						*lpName;
	LPDIRECT3DVERTEXBUFFER9		lpVB;
	int							nTex;
	char						*lpTexName;
	DWORD						dwCount;
	DWORD						dwRow;

	PtclFrameEx					*lpPtcl;
	int							nFrame;
	DWORD						dwFrames;

	D3DXMATRIX					matrix;

	PTCL3_INFO					*lpInfo;

	BYTE						bAlignSpeed;
	BYTE						bBornMatrix;
	float						fSpin;
	float						fSpinRandom;
	float						fAlphaStart;
	float						fAlphaMid;
	float						fAlphaEnd;
	DWORD						dwFadeInFrames;
	DWORD						dwFadeOutFrame;
	float						fAgeAlpha0;
	float						fAgeAlpha1;
	float						fAgeAlpha2;
	float						fAgeFadeIn;
	float						fAgeFadeOut;
	D3DXVECTOR3					vGravity;
	D3DXVECTOR3					vSpeed;
};

C3_CORE_DLL_API
void Ptcl_Clear ( C3Ptcl3 *lpPtcl );
C3_CORE_DLL_API
BOOL Ptcl_Load ( C3Ptcl3 **lpPtcl,
				 void *file,
				 BOOL bTex = false );
C3_CORE_DLL_API
void Ptcl_Unload ( C3Ptcl3 **lpPtcl );
C3_CORE_DLL_API
BOOL Ptcl_Draw ( C3Ptcl3 *lpPtcl, DWORD nAsb = D3DBLEND_SRCALPHA, DWORD nAdb = D3DBLEND_INVSRCALPHA );
C3_CORE_DLL_API
PTCL3_INFO* Ptcl3_CreateInfo ( int nCount );
C3_CORE_DLL_API
void Ptcl3_DestroyInfo ( PTCL3_INFO *lpInfo );
C3_CORE_DLL_API
void Ptcl3_SetGravity ( C3Ptcl3 *lpPtcl, D3DXVECTOR3 vGravity );
C3_CORE_DLL_API
void Ptrl3_SetExternalSpeed ( C3Ptcl3 *lpPtcl, D3DXVECTOR3 vSpeed );

#endif
