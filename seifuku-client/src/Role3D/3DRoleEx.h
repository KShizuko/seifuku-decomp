#if !defined(AFX_3DROLEEX_H__SFK_4343_INCLUDED_)
#define AFX_3DROLEEX_H__SFK_4343_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif
#include "3DRolePart.h"
#include <map>
#include <string>
#include <deque>

struct RolePartNode
{
	std::string		strBone;
	std::string		strParent;
	C3DRolePart*	pPart;
};
typedef std::map<std::string, RolePartNode*> MAP_ROLEPARTNODE;

class I3DRole
{
public:
	virtual DWORD	Release		( void ) = 0;
	virtual void	Move		( float x, float y, float z, const char* pszKind ) = 0;
	virtual void	Rotate		( float x, float y, float z, const char* pszKind ) = 0;
	virtual void	Scale		( float x, float y, float z, const char* pszKind ) = 0;
	virtual BOOL	HitTest		( int nScrX, int nScrY, int nMapX, int nMapY, const char* pszKind ) = 0;
	virtual BOOL	IsLoaded	( const char* pszKind ) = 0;
	virtual void	Draw		( const char* pszKind, D3DXMATRIXA16* pMatrixBase, D3DXMATRIXA16* pMatrixWorld, bool bClearMatrix ) = 0;
	virtual BOOL	AddPart		( const char* pszKind, const char* pszBone, const char* pszParentKind, OBJID id, const D3DXMATRIXA16* pMatrix = NULL ) = 0;
	virtual BOOL	AddPartMesh	( const char* pszKind, const char* pszPartKind, OBJID id ) = 0;
	virtual BOOL	ReplacePartMesh ( const char* pszKind, const char* pszPartKind, OBJID id ) = 0;
	virtual BOOL	SetPartMatrix ( const char* pszKind, const D3DXMATRIXA16* pMatrix ) = 0;
	virtual void	DelPart		( const char* pszKind ) = 0;
	virtual BOOL	HasPart		( const char* pszKind ) = 0;
	virtual void	SetAttachment ( const char* pszKind, const char* pszBone, const char* pszParentKind ) = 0;
	virtual void	SetMotion	( const char* pszKind, I3DMotion* pMotion ) = 0;
	virtual void	SetDefaultMotion ( const char* pszKind ) = 0;
	virtual void	SetBlendMotion ( const char* pszKind, I3DMotion* pMotion, float fWeight ) = 0;
	virtual void	ClearMatrix	( const char* pszKind ) = 0;
	virtual void	SetLightOffset ( int type, float x, float y, float z, float a, float r, float g, float b ) = 0;
	virtual void	SetBlend	( int nAsb, int nAdb ) = 0;
	virtual void	SetTexture	( I3DTexture* pTexture, I3DTexture* pMixTexture, const char* pszKind ) = 0;
	virtual void	SetDrawType	( int nType, const char* pszKind ) = 0;
	virtual void	SetARGB		( float alpha, float red, float green, float blue ) = 0;
	virtual void	SetFrame	( DWORD dwFrame ) = 0;
	virtual void	SetBlendFrame ( DWORD dwFrame ) = 0;
	virtual void	AddEffect	( const char* pszIndex, const char* pszKind, BOOL bForce ) = 0;
	virtual void	Clear3DEffect ( const char* pszIndex, const char* pszKind ) = 0;
	virtual BOOL	TestEffect	( const char* pszIndex, const char* pszKind ) = 0;
	virtual void	SetLodLevel	( int nLevel ) = 0;
	virtual BOOL	Reload		( void ) = 0;
};

extern "C" ROLE3D_API I3DRole* I3DRoleCreate ( void );
extern "C" ROLE3D_API void LoadAdjustConfig ( void );

class C3DRoleEx : public I3DRole
{
public:
	C3DRoleEx();
	virtual ~C3DRoleEx();
public:
	virtual DWORD	Release		( void );
	virtual void	Move		( float x, float y, float z, const char* pszKind );
	virtual void	Rotate		( float x, float y, float z, const char* pszKind );
	virtual void	Scale		( float x, float y, float z, const char* pszKind );
	virtual BOOL	HitTest		( int nScrX, int nScrY, int nMapX, int nMapY, const char* pszKind );
	virtual BOOL	IsLoaded	( const char* pszKind );
	virtual void	Draw		( const char* pszKind, D3DXMATRIXA16* pMatrixBase, D3DXMATRIXA16* pMatrixWorld, bool bClearMatrix );
	virtual BOOL	AddPart		( const char* pszKind, const char* pszBone, const char* pszParentKind, OBJID id, const D3DXMATRIXA16* pMatrix = NULL );
	virtual BOOL	AddPartMesh	( const char* pszKind, const char* pszPartKind, OBJID id );
	virtual BOOL	ReplacePartMesh ( const char* pszKind, const char* pszPartKind, OBJID id );
	virtual BOOL	SetPartMatrix ( const char* pszKind, const D3DXMATRIXA16* pMatrix );
	virtual void	DelPart		( const char* pszKind );
	virtual BOOL	HasPart		( const char* pszKind );
	virtual void	SetAttachment ( const char* pszKind, const char* pszBone, const char* pszParentKind );
	virtual void	SetMotion	( const char* pszKind, I3DMotion* pMotion );
	virtual void	SetDefaultMotion ( const char* pszKind );
	virtual void	SetBlendMotion ( const char* pszKind, I3DMotion* pMotion, float fWeight );
	virtual void	ClearMatrix	( const char* pszKind );
	virtual void	SetLightOffset ( int type, float x, float y, float z, float a, float r, float g, float b );
	virtual void	SetBlend	( int nAsb, int nAdb ){m_nAsb = nAsb; m_nAdb = nAdb;}
	virtual void	SetTexture	( I3DTexture* pTexture, I3DTexture* pMixTexture, const char* pszKind );
	virtual void	SetDrawType	( int nType, const char* pszKind );
	virtual void	SetARGB		( float alpha, float red, float green, float blue );
	virtual void	SetFrame	( DWORD dwFrame );
	virtual void	SetBlendFrame ( DWORD dwFrame );
	virtual void	AddEffect	( const char* pszIndex, const char* pszKind, BOOL bForce );
	virtual void	Clear3DEffect ( const char* pszIndex, const char* pszKind );
	virtual BOOL	TestEffect	( const char* pszIndex, const char* pszKind );
	virtual void	SetLodLevel	( int nLevel );
	virtual BOOL	Reload		( void );

	int				GetPartAmount ( void ){return m_mapNode.size();}
	C3DRolePart*	GetPart		( const char* pszKind );
private:
	RolePartNode*	GetNode		( const char* pszKind );
	void			CollectTrackers ( RolePartNode* pNode, std::deque<C3MotionTracker>& setTracker );
	void			DeleteNode	( RolePartNode* pNode );
private:
	BYTE			m_ucReserved[2];
	BYTE			m_ucReserved2;
	MAP_ROLEPARTNODE	m_mapNode;
	float			m_a, m_r, m_g, m_b;
	D3DXVECTOR3		m_LightOffset;
	int				m_nShadowType;
	int				m_nAsb;
	int				m_nAdb;
	int				m_nLodLevel;
};

#endif
