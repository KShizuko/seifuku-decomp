#ifndef _c3_omni_h_
#define _c3_omni_h_

#ifdef	C3_CORE_DLL_EXPORTS
#define C3_CORE_DLL_API __declspec ( dllexport )
#else
#define C3_CORE_DLL_API __declspec ( dllimport )
#endif

#include "c3_common.h"

typedef struct
{
	char				*lpName;
	D3DXVECTOR3			pos;
	D3DXCOLOR			color;
	float				fRadius;
	float				fAttenuation;
} C3Omni;
C3_CORE_DLL_API
void Omni_Clear ( C3Omni *lpOmni );

C3_CORE_DLL_API
BOOL Omni_Load ( C3Omni **lpOmni,
				 char *lpName,
				 DWORD dwIndex );
C3_CORE_DLL_API
BOOL Omni_Save ( char *lpName, C3Omni *lpOmni, BOOL bNew );
C3_CORE_DLL_API
void Omni_Unload ( C3Omni **lpOmni );

#endif
