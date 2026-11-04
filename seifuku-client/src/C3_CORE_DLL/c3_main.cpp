#include	 "c3_main.h"
#include	 "c3_texture.h"
#include     "c3_datafile.h"

LPDIRECT3D9								g_D3D = 0;
LPDIRECT3DDEVICE9						g_D3DDevice = 0;
D3DCAPS9								g_D3DCaps;
C3_CORE_DLL_API D3DDISPLAYMODE			g_DisplayMode;
C3_CORE_DLL_API HWND					g_hWnd;
D3DVIEWPORT9							g_Viewport;
C3_CORE_DLL_API D3DXMATRIX				g_ViewMatrix;
C3_CORE_DLL_API D3DXMATRIX				g_InvViewMatrix;
C3_CORE_DLL_API D3DXMATRIX				g_IDMatrix;
C3_CORE_DLL_API D3DXMATRIX				g_ProjectMatrix;
C3_CORE_DLL_API D3DPRESENT_PARAMETERS	g_Present;

static char								s_szDeviceString[64];
static DWORD							s_dwGraphicDetail = 0;

C3_CORE_DLL_API DWORD					g_dwTriTotal = 0;

BOOL APIENTRY DllMain ( HANDLE hModule, 
                        DWORD  ul_reason_for_call, 
                        LPVOID lpReserved )
{
    switch ( ul_reason_for_call )
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

C3_CORE_DLL_API
int Init3D ( HINSTANCE hInst,
			 const char *lpTitle,
			 DWORD dwWidth,
			 DWORD dwHeight,
			 BOOL bWindowed,
			 WNDPROC proc,
			 DWORD dwBackCount )
{
	WNDCLASS wc;
	ZeroMemory ( &wc, sizeof ( wc ) );
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon ( hInst, IDI_APPLICATION );
	wc.hCursor = LoadCursor ( NULL, IDC_ARROW );
	wc.hbrBackground = ( HBRUSH ) GetStockObject ( BLACK_BRUSH );
	wc.lpszClassName = lpTitle;
	RegisterClass ( &wc );

	DEVMODE mode;
	mode.dmSize = sizeof ( mode );
	EnumDisplaySettings ( NULL, ENUM_CURRENT_SETTINGS, &mode );

	HWND hWnd = CreateWindow ( lpTitle,
							   lpTitle,
							   WS_POPUP,
							   ( mode.dmPelsWidth - dwWidth ) / 2,
							   ( mode.dmPelsHeight - dwHeight ) / 2,
							   dwWidth,
							   dwHeight,
							   GetDesktopWindow (),
							   NULL,
							   hInst,
							   NULL );
	if ( !hWnd )
		return 0;

	return Init3DEx ( hWnd,
					  dwWidth,
					  dwHeight,
					  bWindowed,
					  dwBackCount );
}

static const char*	s_lpszNoMultiSample[] = { "RADEON 9250" };
static const DWORD	s_dwMultiSample[3][4] = { { 0, 4, 8, 8 }, { 0, 2, 4, 4 }, { 0, 0, 2, 2 } };
static D3DMULTISAMPLE_TYPE ChooseMultiSampleType ( D3DFORMAT fmtBackBuffer, D3DFORMAT fmtDepthStencil )
{
	if ( strcmp ( s_szDeviceString, "" ) != 0 )
	{
		for ( int i = 0; i < ( int ) ( sizeof ( s_lpszNoMultiSample ) / sizeof ( s_lpszNoMultiSample[0] ) ); i++ )
		{
			if ( strstr ( s_szDeviceString, s_lpszNoMultiSample[i] ) == s_szDeviceString )
				return D3DMULTISAMPLE_NONE;
		}
		for ( int n = 0; n < 3; n++ )
		{
			D3DMULTISAMPLE_TYPE type = ( D3DMULTISAMPLE_TYPE ) s_dwMultiSample[n][3];
			if ( SUCCEEDED ( g_D3D->CheckDeviceMultiSampleType ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, fmtBackBuffer, TRUE, type, NULL ) ) &&
				 SUCCEEDED ( g_D3D->CheckDeviceMultiSampleType ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, fmtDepthStencil, TRUE, type, NULL ) ) )
				return ( D3DMULTISAMPLE_TYPE ) s_dwMultiSample[n][s_dwGraphicDetail];
		}
	}
	return D3DMULTISAMPLE_NONE;
}

typedef BOOL ( WINAPI *ENUMDISPLAYDEVICESA ) ( LPCSTR, DWORD, void*, DWORD );
static BOOL EnumDisplayDevicesDyn ( DWORD iDevNum, void *lpDisplayDevice )
{
	static ENUMDISPLAYDEVICESA s_pfnEnumDisplayDevices = ( ENUMDISPLAYDEVICESA ) GetProcAddress ( LoadLibraryA ( "User32.dll" ), "EnumDisplayDevicesA" );
	if ( !s_pfnEnumDisplayDevices )
		return 0;
	return s_pfnEnumDisplayDevices ( NULL, iDevNum, lpDisplayDevice, 0 );
}

struct DISPLAY_DEVICE_4351
{
	DWORD	cb;
	CHAR	DeviceName[32];
	CHAR	DeviceString[128];
	DWORD	StateFlags;
};
static void GetDisplayDeviceString ( void )
{
	DISPLAY_DEVICE_4351 dd;
	ZeroMemory ( &dd, sizeof ( dd ) );
	dd.cb = sizeof ( dd );
	for ( DWORD i = 0; EnumDisplayDevicesDyn ( i, &dd ); i++ )
	{
		if ( dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE )
		{
			strcpy ( s_szDeviceString, dd.DeviceString );
			return;
		}
	}
	strcpy ( s_szDeviceString, "" );
}

static BOOL D3D8RefusesWindowedFormat ( BOOL bWindowed, D3DFORMAT fmtBackBuffer )
{
	if ( !bWindowed )
		return false;
	D3DDISPLAYMODE mode;
	if ( FAILED ( g_D3D->GetAdapterDisplayMode ( D3DADAPTER_DEFAULT, &mode ) ) )
		return false;
	BOOL b16Back = ( fmtBackBuffer == D3DFMT_R5G6B5 || fmtBackBuffer == D3DFMT_X1R5G5B5 || fmtBackBuffer == D3DFMT_A1R5G5B5 );
	BOOL b16Desk = ( mode.Format == D3DFMT_R5G6B5 || mode.Format == D3DFMT_X1R5G5B5 || mode.Format == D3DFMT_A1R5G5B5 );
	return b16Back != b16Desk;
}

C3_CORE_DLL_API
int Init3DEx ( HWND hWnd,
		  	   DWORD dwWidth,
			   DWORD dwHeight,
			   BOOL bWindowed,
			   DWORD dwBackCount )
{
	g_DisplayMode.Width = dwWidth;
	g_DisplayMode.Height = dwHeight;
	g_DisplayMode.RefreshRate = 60;

	GetDisplayDeviceString ();

	g_D3D = Direct3DCreate9 ( D3D_SDK_VERSION );
	if ( NULL == g_D3D )
		return 0;

	if ( FAILED ( g_D3D->GetDeviceCaps ( D3DADAPTER_DEFAULT,
										 D3DDEVTYPE_HAL,
										 &g_D3DCaps ) ) )
		return -1;

	if ( FAILED ( g_D3D->CheckDeviceType ( D3DADAPTER_DEFAULT,
										   D3DDEVTYPE_HAL,
										   D3DFMT_R5G6B5,
										   D3DFMT_R5G6B5,
										   bWindowed ) ) )
	{
		if ( FAILED ( g_D3D->CheckDeviceType ( D3DADAPTER_DEFAULT,
											   D3DDEVTYPE_HAL,
											   D3DFMT_X1R5G5B5,
											   D3DFMT_X1R5G5B5,
											   bWindowed ) ) )
		{
			g_D3D->Release ();
			g_D3D = 0;
			return -2;
		}
		else
			g_DisplayMode.Format = D3DFMT_X1R5G5B5;
	}
	else
		g_DisplayMode.Format = D3DFMT_R5G6B5;
    ZeroMemory ( &g_Present, sizeof ( D3DPRESENT_PARAMETERS ) );
    g_Present.Windowed = bWindowed;
	g_Present.BackBufferCount = dwBackCount;
	g_Present.BackBufferWidth = g_DisplayMode.Width;
	g_Present.BackBufferHeight = g_DisplayMode.Height;
	g_Present.hDeviceWindow = g_hWnd;
    g_Present.SwapEffect = D3DSWAPEFFECT_DISCARD;
	if ( bWindowed )
		g_Present.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    g_Present.BackBufferFormat = g_DisplayMode.Format;
    g_Present.EnableAutoDepthStencil = true;
    g_Present.AutoDepthStencilFormat = D3DFMT_D16;
	g_Present.MultiSampleType = ChooseMultiSampleType ( g_DisplayMode.Format, D3DFMT_D16 );

	BOOL bRefused = D3D8RefusesWindowedFormat ( bWindowed, g_Present.BackBufferFormat );
	if ( bRefused || FAILED ( g_D3D->CreateDevice ( D3DADAPTER_DEFAULT,
									    D3DDEVTYPE_HAL,
									    hWnd,
									    D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
									    &g_Present,
									    &g_D3DDevice ) ) )
	{
		if ( bRefused || FAILED ( g_D3D->CreateDevice ( D3DADAPTER_DEFAULT,
											D3DDEVTYPE_HAL,
											hWnd,
											D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
											&g_Present,
											&g_D3DDevice ) ) )
		{
			g_DisplayMode.Format = D3DFMT_A8R8G8B8;
			g_Present.BackBufferFormat = g_DisplayMode.Format;
			g_Present.MultiSampleType = ChooseMultiSampleType ( D3DFMT_A8R8G8B8, g_Present.AutoDepthStencilFormat );
			bRefused = D3D8RefusesWindowedFormat ( bWindowed, g_Present.BackBufferFormat );
			if ( bRefused || FAILED ( g_D3D->CreateDevice ( D3DADAPTER_DEFAULT,
											D3DDEVTYPE_HAL,
											hWnd,
											D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
											&g_Present,
											&g_D3DDevice ) ) )
			{
				if ( bRefused || FAILED ( g_D3D->CreateDevice ( D3DADAPTER_DEFAULT,
												D3DDEVTYPE_HAL,
												hWnd,
												D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
												&g_Present,
												&g_D3DDevice ) ) )
				{
						return -1;
				}
			}
		}
	}

	if ( !( g_D3DCaps.TextureCaps & D3DPTEXTURECAPS_ALPHA ) )
		return -3;

	g_Viewport.X = 0;
	g_Viewport.Y = 0;
	g_Viewport.Width = dwWidth;
	g_Viewport.Height = dwHeight;
	g_Viewport.MinZ = 0.0f;
	g_Viewport.MaxZ = 1.0f;
	if ( FAILED ( g_D3DDevice->SetViewport ( &g_Viewport ) ) )
		return 0;

	SetRenderState ( D3DRS_AMBIENT, 0 );
	SetRenderState ( D3DRS_LIGHTING, true );
	SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );
	SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	SetRenderState ( D3DRS_NORMALIZENORMALS, true );
	SetRenderState ( D3DRS_DITHERENABLE, false );
	SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
	g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	g_D3DDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	for ( int t = 0; t < TEX_MAX; t++ )
		g_lpTex[t] = 0;

	g_hWnd = hWnd;

	D3DXMatrixIdentity ( &g_IDMatrix );

	ShowWindow ( hWnd, SW_SHOW );
	UpdateWindow ( hWnd );

	return 1;
}

C3_CORE_DLL_API
void Quit3D ( void )
{
	SafeRelease ( g_D3DDevice );
	SafeRelease ( g_D3D );
}

C3_CORE_DLL_API
BOOL Begin3D ( void )
{
	if ( FAILED ( g_D3DDevice->BeginScene () ) )
		return false;
	return true;
}
C3_CORE_DLL_API
BOOL End3D ( void )
{
	if ( FAILED ( g_D3DDevice->EndScene () ) )
		return false;
	return true;
}

C3_CORE_DLL_API
int IfDeviceLost ( void )
{
	HRESULT result;
    if ( FAILED ( result = g_D3DDevice->TestCooperativeLevel () ) )
    {
		if ( result == D3DERR_DEVICELOST )
			return 1;

		if ( result == D3DERR_DEVICENOTRESET )
			return 2;
    }
	return 0;
}
C3_CORE_DLL_API
BOOL ResetDevice ( void )
{
	if ( FAILED ( g_D3DDevice->Reset ( &g_Present ) ) )
		return false;
	SetRenderState ( D3DRS_AMBIENT, 0 );
	return true;
}

C3_CORE_DLL_API
BOOL ClearBuffer ( BOOL bZBuffer, BOOL bTarget, D3DCOLOR color )
{
	DWORD dwFlags = 0;

	if ( bZBuffer )
		dwFlags |= D3DCLEAR_ZBUFFER;
	if ( bTarget )
		dwFlags |= D3DCLEAR_TARGET;

	if ( FAILED ( g_D3DDevice->Clear ( 0, 0, dwFlags, color, 1.0f, 0 ) ) )
		return false;
	return true;
}
C3_CORE_DLL_API
BOOL Flip ( const RECT *lpSrc, const RECT *lpDest, HWND hWnd )
{
	if ( FAILED ( g_D3DDevice->Present ( lpSrc, lpDest, hWnd, 0 ) ) )
		return false;
	return true;
}

C3_CORE_DLL_API
void SetRenderState ( D3DRENDERSTATETYPE state, DWORD dwValue )
{
	DWORD current;
	g_D3DDevice->GetRenderState ( state, &current );
	if ( current != dwValue )
		g_D3DDevice->SetRenderState ( state, dwValue );
}
C3_CORE_DLL_API
void SetTextureStageState ( DWORD dwStage,
							D3DTEXTURESTAGESTATETYPE type,
							DWORD dwValue )
{
	DWORD current;
	g_D3DDevice->GetTextureStageState ( dwStage, type, &current );
	if ( current != dwValue )
		g_D3DDevice->SetTextureStageState ( dwStage, type, dwValue );
}

C3_CORE_DLL_API
int HintGraphicDetail ( DWORD dwDetail )
{
	if ( dwDetail == s_dwGraphicDetail )
		return 0;
	if ( dwDetail >= 3 )
		return 0;
	if ( !g_D3DDevice )
	{
		s_dwGraphicDetail = dwDetail;
		return 0;
	}
	int rval = 0;
	switch ( dwDetail )
	{
	case 0:
		SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, 0 );
		break;
	case 1:
	case 2:
		SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, 1 );
		rval = 1;
		break;
	}
	s_dwGraphicDetail = dwDetail;
	return rval;
}

C3_CORE_DLL_API
DWORD CalcRate ( void )
{
	static DWORD countcur = 0, countold = 0;
	static DWORD timecur = 0, timeold = 0;
	static DWORD rate = 0;

	countcur++;
	timecur = timeGetTime ();

	if ( timecur - timeold > 1000 )
	{
		rate = ( countcur - countold ) * 1000 / ( timecur - timeold );

		timeold = timecur;
		countold = countcur;
	}
	return rate;
}
C3_CORE_DLL_API
BOOL LimitRate ( DWORD dwRate )
{
	static DWORD dwFrame = 0;
	static DWORD dwFrameOld = 0;
	dwFrame = timeGetTime ();
	if ( dwFrame - dwFrameOld >= dwRate )
	{
		dwFrameOld = dwFrame;
		return TRUE;
	}
	return FALSE;
}
