#include	"c3_capscreen.h"
#include	"seifuku_jpegenc.h"

C3_CORE_DLL_API
void CapScreen ( char *lpName )
{
    BITMAPINFO bi;
    ZeroMemory ( &bi.bmiHeader, sizeof ( BITMAPINFOHEADER ) );
    bi.bmiHeader.biSize = sizeof ( BITMAPINFOHEADER );
    bi.bmiHeader.biWidth = g_DisplayMode.Width;
    bi.bmiHeader.biHeight = -1*int(g_DisplayMode.Height);
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biCompression = BI_RGB;
    bi.bmiHeader.biBitCount = 24;

	void *buffer;
	HDC bitmapDC = CreateCompatibleDC ( NULL );
    HBITMAP bitmap = CreateDIBSection ( bitmapDC,
										&bi,
										DIB_RGB_COLORS,
										&buffer,
										0,
										0 );
    SelectObject ( bitmapDC, bitmap );

	HDC wndDC;
	wndDC = GetDC ( g_hWnd );

	BitBlt ( bitmapDC,
			 0,
			 0,
			 g_DisplayMode.Width,
			 g_DisplayMode.Height,
			 wndDC,
			 0,
			 0,
			 SRCCOPY );

	JPGEncode ( lpName,
				g_DisplayMode.Width,
				g_DisplayMode.Height,
				( unsigned char* )buffer,
				IJL_BGR,
				95);

	ReleaseDC ( g_hWnd, wndDC );
	ReleaseDC ( NULL, bitmapDC );

	DeleteObject ( bitmap );
}

BOOL JPGEncode ( char *lpJpegName,
				 DWORD dwWidth,
				 DWORD dwHeight,
				 unsigned char *lpBuffer,
				 _IJL_COLOR mode,
				 DWORD dwQuality )
{
	if ( mode != IJL_RGB && mode != IJL_BGR )
		return false;

	return Seifuku_JpegEncode24 ( lpJpegName, dwWidth, dwHeight, lpBuffer, mode == IJL_BGR, dwQuality );
}
