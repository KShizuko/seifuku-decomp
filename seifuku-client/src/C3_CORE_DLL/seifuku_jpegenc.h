#pragma once

#include <windows.h>

BOOL Seifuku_JpegEncode24 ( const char *lpJpegName, DWORD dwWidth, DWORD dwHeight, const unsigned char *lpRows, BOOL bBgr, DWORD dwQuality );
