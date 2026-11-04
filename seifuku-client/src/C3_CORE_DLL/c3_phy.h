#ifndef _c3_phy_h_
#define _c3_phy_h_

#ifdef	C3_CORE_DLL_EXPORTS
#define C3_CORE_DLL_API __declspec ( dllexport )
#else
#define C3_CORE_DLL_API __declspec ( dllimport )
#endif

#include "c3_common.h"
#include "c3_texture.h"
#include "c3_key.h"
#include <map>
#include <vector>

typedef struct _D3DMATERIAL8
{
	D3DCOLORVALUE					Diffuse;
	D3DCOLORVALUE					Ambient;
	D3DCOLORVALUE					Specular;
	D3DCOLORVALUE					Emissive;
	float							Power;
} D3DMATERIAL8;

#define PHY_OUT_VERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
struct PhyOutVertex
{
	float							x, y, z;
	float							nx, ny, nz;
	DWORD							color;
	float							u, v;
};

struct PhyCacheItem
{
	PhyOutVertex					*lpVertex;
	DWORD							dwSize;
};
typedef std::map<DWORD, std::vector<PhyCacheItem> > PHY_CACHE;

#define		_BONE_MAX_				2
struct PhyVertex
{
	D3DXVECTOR3						pos;
	float							u, v;
	DWORD							color;
	DWORD							index[_BONE_MAX_];
	float							weight[_BONE_MAX_];
	D3DXVECTOR3						normal;
};

struct C3KeyFrame
{
	DWORD							pos;
	D3DXMATRIXA16					*matrix;
};
struct C3Motion
{
	DWORD							dwBoneCount;
	DWORD							dwFrames;

	DWORD							dwKeyFrames;
	C3KeyFrame						*lpKeyFrame;

	D3DXMATRIXA16					*matrix;

	DWORD							dwMorphCount;
	float							*lpMorph;
	int								nFrame;

	float							fWeight;

	DWORD							dwCacheID;
};

struct C3MotionTracker
{
	C3Motion						*lpMotion;
	C3Motion						*lpBlendMotion;
};

extern C3_CORE_DLL_API int g_nMotionCount;
extern C3_CORE_DLL_API int g_nPhyCount;

C3_CORE_DLL_API
void Motion_Clear ( C3Motion *lpMotion );
C3_CORE_DLL_API
BOOL Motion_Load ( C3Motion **lpMotion, void *file );
C3_CORE_DLL_API
BOOL Motion_Save ( char *lpName, C3Motion *lpMotion, BOOL bNew );
C3_CORE_DLL_API
void Motion_Unload ( C3Motion **lpMotion );
C3_CORE_DLL_API
void Motion_GetMatrix ( C3Motion *lpMotion, DWORD dwBone, D3DXMATRIXA16 *lpMatrix );
C3_CORE_DLL_API
void Motion_GetMatrix_Blend ( C3MotionTracker tracker, DWORD dwBone, D3DXMATRIXA16 *lpMatrix );

struct C3Phy
{
	char							*lpName;

	DWORD							dwBlendCount;

	BOOL							bNormal;
	BOOL							bTwoSide;

	DWORD							dwNVecCount;
	DWORD							dwAVecCount;
	PhyVertex						*lpVB;
	LPDIRECT3DVERTEXBUFFER9			vb;

	DWORD							dwNTriCount;
	DWORD							dwATriCount;
	WORD							*lpIB;
	LPDIRECT3DINDEXBUFFER9			ib;

	char							*lpTexName;
	int								nTex;
	int								nTex2;
	D3DXVECTOR3						bboxMin, bboxMax;

	C3Motion						*lpMotion;
	C3Motion						*lpBlendMotion;

	float							fA, fR, fG, fB;

	C3Key							Key;
	BOOL							bDraw;

	DWORD							dwTexRow;

	D3DXMATRIXA16					InitMatrix;
	D3DXMATRIXA16					matrix;

	BOOL							bMatrix;
	D3DXMATRIXA16					matrixLocal;
	D3DXMATRIXA16					matrixWorld;

	D3DXVECTOR2						uvstep;
	DWORD							dwStepCount;

	int								nBillboard;
	PHY_CACHE						*lpCache;
};

C3_CORE_DLL_API
void Phy_Clear ( C3Phy *lpPhy );
C3_CORE_DLL_API
BOOL Phy_Load ( C3Phy **lpPhy, void *file, BOOL bNormal, BOOL bCalcNormal, BOOL bTex, BOOL bMorph );
C3_CORE_DLL_API
BOOL Phy_Save ( char *lpName, C3Phy *lpPhy, BOOL bNew );
C3_CORE_DLL_API
void Phy_Unload ( C3Phy **lpPhy );
C3_CORE_DLL_API
void Phy_Prepare ( void );
C3_CORE_DLL_API
BOOL Phy_Calculate ( C3Phy *lpPhy, BOOL bLight, int nType, D3DXVECTOR3 vLight );
C3_CORE_DLL_API
void Phy_SetLight ( BOOL bEnable, float x, float y, float z, DWORD dwDiffuse, DWORD dwAmbient, float fReserved );
C3_CORE_DLL_API
void Phy_SetFluidReflectSpeed ( float fSpeedU, float fSpeedV );
C3_CORE_DLL_API
BOOL Phy_DrawNormal ( C3Phy *lpPhy, D3DMATERIAL8 material, D3DXMATRIXA16 *lpShadow, BOOL bZ, DWORD nAsb, DWORD nAdb, int nType, float fShadowAlpha );
C3_CORE_DLL_API
BOOL Phy_DrawAlpha ( C3Phy *lpPhy, BOOL bZ = false, DWORD nAsb=5, DWORD nAdb=6);
C3_CORE_DLL_API
BOOL Phy_Draw ( PhyOutVertex *lpVertex, DWORD dwTriCount, C3Texture *lpTex, const D3DMATERIAL8 *lpMaterial, DWORD nAsb, DWORD nAdb, BOOL bTwoSide, BOOL bZ );
C3_CORE_DLL_API
void Phy_NextFrame ( C3Phy *lpPhy, int nStep );
C3_CORE_DLL_API
void Phy_SetFrame ( C3Phy *lpPhy, DWORD dwFrame );
C3_CORE_DLL_API
void Phy_SetBlendFrame ( C3Phy *lpPhy, DWORD dwFrame );
C3_CORE_DLL_API
void Phy_SetStepCount ( C3Phy *lpPhy, DWORD dwStepCount );

C3_CORE_DLL_API
void Phy_Muliply ( C3Phy *lpPhy, int nBoneIndex, D3DXMATRIXA16 *matrix );

C3_CORE_DLL_API
void Phy_SetColor ( C3Phy *lpPhy,
					float alpha,
					float red,
					float green,
					float blue );

C3_CORE_DLL_API
void Phy_ClearMatrix ( C3Phy *lpPhy );

C3_CORE_DLL_API
void Phy_ChangeTexture ( C3Phy *lpPhy, int nTexID );
C3_CORE_DLL_API
void Phy_ChangeMixTexture ( C3Phy *lpPhy, int nTexID );
#endif
