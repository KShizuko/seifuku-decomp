#if !defined(AFX_3DSIMPLEOBJ_H__EACBD297_70EA_458F_BC6E_9C6F7771C3C5__INCLUDED_)
#define AFX_3DSIMPLEOBJ_H__EACBD297_70EA_458F_BC6E_9C6F7771C3C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif
#include "3DRolePart.h"

class I3DSimpleObj
{
public:
	virtual DWORD	Release		( void ) = 0;
	virtual BOOL	Create		( OBJID idType ) = 0;
	virtual BOOL	Draw2BG		( int nMapX, int nMapY, int nAsb = 5, int nAdb = 6 ) = 0;
	virtual BOOL	HitTest		( int nHitScreenX, int nHitScreenY, int nMapX, int nMapY ) = 0;
	virtual void	SetPos		( int nX, int nY, int nHeight, int nRotate, float fScale ) = 0;
	virtual void	SetPosH		( float fPosH ) = 0;
	virtual void	SetObliqueAngle ( int nAngle ) = 0;
	virtual void	Rotate		( float x, float y, float z ) = 0;
	virtual void	SetLightOffset ( int type, float x, float y, float z, float a, float r, float g, float b ) = 0;
	virtual void	SetOblique	( BOOL bOblique ) = 0;
	virtual void	SetShadowType ( int nShadowType ) = 0;
	virtual void	SetMotionById ( OBJID idMotion ) = 0;
	virtual void	SetARGB		( float alpha, float red, float green, float blue ) = 0;
	virtual void	SetTexture	( I3DTexture* pTexture, I3DTexture* pMixTexture, int nMixOpt ) = 0;
	virtual void	SetFrame	( DWORD dwFrame ) = 0;
	virtual void	SetMotion	( I3DMotion* pMotion ) = 0;
	virtual void	AddEffect	( const char* pszIndex ) = 0;
	virtual void	Scale		( float x, float y, float z ) = 0;
	virtual BOOL	Reload		( void ) = 0;
	virtual ~I3DSimpleObj() {}
};

extern "C" ROLE3D_API I3DSimpleObj* Simple3DObjCreate ( OBJID idType );
extern "C" ROLE3D_API I3DSimpleObj* Simple3DObjCreate2 ( OBJID idMesh, OBJID idTexture, OBJID idMixTex, int nUnused, int nAsb, int nAdb );

class C3DSimpleObj : public I3DSimpleObj
{
public:
	C3DSimpleObj();
	virtual ~C3DSimpleObj();
public:
	int			m_nPosX;
	int			m_nPosY;
	int			m_nPosH;
	int			m_nRotate;
	int			m_nObliqueAngle;
	D3DXVECTOR3	m_vScale;
	D3DXVECTOR3	m_vRotate;
	D3DXVECTOR3	m_LightOffset;
	int			m_nShadowType;
	float		m_a;
	float		m_r;
	float		m_g;
	float		m_b;
	BOOL		m_bOblique;
	float		m_fPosH;
	C3DRolePart*	m_p3DRolePart;
	OBJID			m_idType;
public:
	virtual DWORD	Release		( void );
	virtual BOOL	Create		( OBJID idType );
	BOOL			Create2		( OBJID idMesh, OBJID idTexture, OBJID idMixTex, int nUnused, int nAsb, int nAdb );
	virtual BOOL	Draw2BG		( int nMapX, int nMapY, int nAsb = 5, int nAdb = 6 );
	virtual BOOL	HitTest		( int nHitScreenX, int nHitScreenY, int nMapX, int nMapY );
	virtual void	SetPos		( int nX, int nY, int nHeight, int nRotate, float fScale );
	virtual void	SetPosH		( float fPosH ){m_fPosH = fPosH;}
	virtual void	SetObliqueAngle ( int nAngle ){m_nObliqueAngle = nAngle;}
	virtual void	Rotate		( float x, float y, float z );
	virtual void	SetLightOffset ( int type, float x, float y, float z, float a, float r, float g, float b );
	virtual void	SetOblique	( BOOL bOblique ){m_bOblique = bOblique;}
	virtual void	SetShadowType ( int nShadowType ){m_nShadowType = nShadowType;}
	virtual void	SetMotionById ( OBJID idMotion );
	virtual void	SetARGB		( float alpha, float red, float green, float blue );
	virtual void	SetTexture	( I3DTexture* pTexture, I3DTexture* pMixTexture, int nMixOpt );
	virtual void	SetFrame	( DWORD dwFrame );
	virtual void	SetMotion	( I3DMotion* pMotion );
	virtual void	AddEffect	( const char* pszIndex );
	virtual void	Scale		( float x, float y, float z );
	virtual BOOL	Reload		( void );
};
#endif
