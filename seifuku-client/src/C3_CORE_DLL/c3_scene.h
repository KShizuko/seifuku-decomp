#ifndef _c3_scene_h_
#define _c3_scene_h_

#ifdef	C3_CORE_DLL_EXPORTS
#define C3_CORE_DLL_API __declspec ( dllexport )
#else
#define C3_CORE_DLL_API __declspec ( dllimport )
#endif

#include "c3_common.h"
#include "c3_texture.h"

#define SCENE_VERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2 )
struct SceneVertex
{
	float	x, y, z;
	float	nx, ny, nz;
	float	u, v;
	float	lu, lv;
};
struct SceneEdge
{
	DWORD	dwStartVec;
	DWORD	dwEndVec;
};

typedef struct
{
	char						*lpName;

	DWORD						dwVecCount;
	SceneVertex					*lpVB;
	DWORD						dwTriCount;
	WORD						*lpIB;

	char						*lpTexName;
	int							nTex;

	char						*lplTexName;
	int							nlTex;

	DWORD						dwFrameCount;
	D3DXMATRIX					*lpFrame;
	int							nFrame;

	D3DXMATRIX					matrix;
} C3Scene;

C3_CORE_DLL_API
void Scene_Clear ( C3Scene *lpScene );
C3_CORE_DLL_API
BOOL Scene_Load ( C3Scene **lpScene,
				  char *lpName,
				  DWORD dwIndex );
C3_CORE_DLL_API
BOOL Scene_Save ( char *lpName, C3Scene *lpScene, BOOL bNew );
C3_CORE_DLL_API
void Scene_Unload ( C3Scene **lpScene );

C3_CORE_DLL_API
BOOL Scene_Optimize ( C3Scene *lpScene );

C3_CORE_DLL_API
void Scene_Prepare ( void );
C3_CORE_DLL_API
BOOL Scene_Draw ( C3Scene *lpScene );

C3_CORE_DLL_API
void Scene_NextFrame ( C3Scene *lpScene, int nStep );

C3_CORE_DLL_API
void Scene_Muliply ( C3Scene *lpScene, D3DXMATRIX *matrix );

#endif
