#ifndef _c3_key_h_
#define _c3_key_h_

#include "c3_main.h"
#include "c3_common.h"

struct C3Frame
{
	int			nFrame;
	float		fParam[1];
	BOOL		bParam[1];
	int			nParam[1];
};
struct C3Key
{
	DWORD		dwAlphas;
	C3Frame*	lpAlphas;

	DWORD		dwDraws;
	C3Frame*	lpDraws;

	DWORD		dwChangeTexs;
	C3Frame*	lpChangeTexs;
};

C3_CORE_DLL_API
void Key_Clear ( C3Key *lpKey );

C3_CORE_DLL_API
BOOL Key_ProcessAlpha ( C3Key *lpKey, DWORD dwFrame, DWORD dwFrames, float* fReturn );
C3_CORE_DLL_API
BOOL Key_ProcessDraw ( C3Key *lpKey, DWORD dwFrame, BOOL* bReturn );
C3_CORE_DLL_API
BOOL Key_ProcessChangeTex ( C3Key *lpKey, DWORD dwFrame, int* nReturn );

#endif
