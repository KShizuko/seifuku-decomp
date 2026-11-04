#ifndef _c3_main_h_
#define _c3_main_h_

#ifdef	C3_CORE_DLL_EXPORTS
#define C3_CORE_DLL_API __declspec ( dllexport )
#else
#define C3_CORE_DLL_API __declspec ( dllimport )
#endif

#include <d3d9.h>
#include "d3dx8compat.h"

#include "c3_common.h"

const char		C3_VERSION[64] = "MAXFILE C3 00001";

#ifdef	C3_CORE_DLL_EXPORTS
#define C3_CORE_DLL_D3DDATA
#else
#define C3_CORE_DLL_D3DDATA __declspec ( dllimport )
#pragma comment(linker, "/alternatename:__imp_?g_D3D@@3PAUIDirect3D9@@A=__imp_?g_D3D@@3PAUIDirect3D8@@A")
#pragma comment(linker, "/alternatename:__imp_?g_D3DDevice@@3PAUIDirect3DDevice9@@A=__imp_?g_D3DDevice@@3PAUIDirect3DDevice8@@A")
#pragma comment(linker, "/alternatename:__imp_?g_D3DCaps@@3U_D3DCAPS9@@A=__imp_?g_D3DCaps@@3U_D3DCAPS8@@A")
#pragma comment(linker, "/alternatename:__imp_?g_Viewport@@3U_D3DVIEWPORT9@@A=__imp_?g_Viewport@@3U_D3DVIEWPORT8@@A")
#endif

extern C3_CORE_DLL_D3DDATA LPDIRECT3D9			g_D3D;
extern C3_CORE_DLL_D3DDATA LPDIRECT3DDEVICE9	g_D3DDevice;
extern C3_CORE_DLL_D3DDATA D3DCAPS9				g_D3DCaps;
extern C3_CORE_DLL_API D3DDISPLAYMODE			g_DisplayMode;
extern C3_CORE_DLL_API HWND						g_hWnd;
extern C3_CORE_DLL_D3DDATA D3DVIEWPORT9			g_Viewport;
extern C3_CORE_DLL_API D3DXMATRIX				g_ViewMatrix;
extern C3_CORE_DLL_API D3DXMATRIX				g_InvViewMatrix;
extern C3_CORE_DLL_API D3DXMATRIX				g_IDMatrix;
extern C3_CORE_DLL_API D3DXMATRIX				g_ProjectMatrix;
extern C3_CORE_DLL_API D3DPRESENT_PARAMETERS	g_Present;

extern C3_CORE_DLL_API DWORD					g_dwTriTotal;

C3_CORE_DLL_API
int Init3D ( HINSTANCE hInst,
		     const char *lpTitle,
		     DWORD dwWidth,
		     DWORD dwHeight,
		     BOOL bWindowed,
		     WNDPROC proc,
		     DWORD dwBackCount );
C3_CORE_DLL_API
int Init3DEx ( HWND hWnd,
			   DWORD dwWidth,
			   DWORD dwHeight,
			   BOOL bWindowed,
			   DWORD dwBackCount );
C3_CORE_DLL_API
void Quit3D ( void );

C3_CORE_DLL_API
BOOL Begin3D ( void );
C3_CORE_DLL_API
BOOL End3D ( void );

C3_CORE_DLL_API
int IfDeviceLost ( void );
C3_CORE_DLL_API
BOOL ResetDevice ( void );

C3_CORE_DLL_API
BOOL ClearBuffer ( BOOL bZBuffer, BOOL bTarget, D3DCOLOR color );
C3_CORE_DLL_API
BOOL Flip ( const RECT *lpSrc, const RECT *lpDest, HWND hWnd );

C3_CORE_DLL_API
void SetRenderState ( D3DRENDERSTATETYPE state, DWORD dwValue );
C3_CORE_DLL_API
void SetTextureStageState ( DWORD dwStage,
							D3DTEXTURESTAGESTATETYPE type,
							DWORD dwValue );

C3_CORE_DLL_API
int HintGraphicDetail ( DWORD dwDetail );

C3_CORE_DLL_API
DWORD CalcRate ( void );
C3_CORE_DLL_API
BOOL LimitRate ( DWORD dwRate );

#endif
