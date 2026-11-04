#ifndef _c3_sprite_h_
#define _c3_sprite_h_

#ifdef	C3_CORE_DLL_EXPORTS
#define C3_CORE_DLL_API __declspec ( dllexport )
#else
#define C3_CORE_DLL_API __declspec ( dllimport )
#endif

#include "c3_common.h"
#include "c3_texture.h"
#include "c3_main.h"

#define SPRITE_VERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
struct SpriteVertex
{
	float x, y;
	float z;
	float rhw;
	DWORD color;
	float u, v;
};

typedef struct
{
	SpriteVertex		vertex[4];
	C3Texture			*lpTex;
} C3Sprite;

C3_CORE_DLL_API
void Sprite_Clear ( C3Sprite *lpSprite );
C3_CORE_DLL_API
BOOL Sprite_Load ( C3Sprite **lpSprite,
				   const char *lpName,
				   D3DPOOL pool = D3DPOOL_MANAGED,
				   BOOL bDuplicate = true,
				   D3DCOLOR colorkey = 0 );
C3_CORE_DLL_API
BOOL Sprite_Create ( C3Sprite **lpSprite,
				     DWORD dwWidth,
					 DWORD dwHeight,
					 DWORD dwMipLevels,
					 D3DFORMAT format,
					 D3DPOOL pool );
C3_CORE_DLL_API
void Sprite_Unload ( C3Sprite **lpSprite );

C3_CORE_DLL_API
void Sprite_SetCoor ( C3Sprite *lpSprite,
					  RECT *lpSrc,
					  int nX,
					  int nY,
					  DWORD dwWidth = 0,
					  DWORD dwHeight = 0 );
C3_CORE_DLL_API
void Sprite_SetColor ( C3Sprite *lpSprite, BYTE a, BYTE r, BYTE g, BYTE b );

C3_CORE_DLL_API
void Sprite_Mirror ( C3Sprite *lpSprite );

C3_CORE_DLL_API
void Sprite_Rotate ( C3Sprite *lpSprite, int nAngle );

C3_CORE_DLL_API
void Sprite_Prepare ( void );
C3_CORE_DLL_API
BOOL Sprite_Draw ( C3Sprite *lpSprite, DWORD dwShowWay = 0 );
C3_CORE_DLL_API
BOOL Sprite_DrawEx ( C3Sprite *lpSprite, DWORD dwSrcBlend, DWORD dwDestBlend );

C3_CORE_DLL_API
BOOL Sprite_Lock ( C3Sprite *lpSprite, RECT *lpRect, D3DLOCKED_RECT *lpReturn );
C3_CORE_DLL_API
BOOL Sprite_Unlock ( C3Sprite *lpSprite );

#endif
