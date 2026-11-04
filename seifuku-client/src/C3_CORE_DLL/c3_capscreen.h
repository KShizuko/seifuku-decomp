#ifndef _c3_capscreen_h_
#define _c3_capscreen_h_

#include "c3_common.h"
#include "c3_main.h"
typedef enum _IJL_COLOR
{
	IJL_RGB			= 1,
	IJL_BGR			= 2
} IJL_COLOR;

C3_CORE_DLL_API
void CapScreen ( char *lpName );

BOOL JPGEncode ( char *lpJpegName,
				 DWORD dwWidth,
				 DWORD dwHeight,
				 unsigned char *lpBuffer,
				 IJL_COLOR mode = IJL_RGB,
				 DWORD dwQuality = 100 );

#endif
