#if !defined(AFX_GAME3DEFFECTEX_H__2DCBEF81_684D_4AF9_AB9B_029B8E3C6E21__INCLUDED_)
#define AFX_GAME3DEFFECTEX_H__2DCBEF81_684D_4AF9_AB9B_029B8E3C6E21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif
#include "BaseFunc.h"
#include "d3dx8compat.h"
#include "3DEffect.h"
#include "GameDataSet.h"

#ifdef	ROLE3D_EXPORTS
#define ROLE3D_API __declspec ( dllexport )
#else
#define ROLE3D_API __declspec ( dllimport )
#endif

struct Game3DEffectIdInfo
{
	OBJID idEffect;
	OBJID idTexture;
	PTCL_INFO* pPtclInfo;
	PTCL3_INFO* pPtcl3Info;
	ShapeBackupInfo* pShapeInfoBuf;
	int nShapes;
	BOOL bReady;
	Game3DEffectIdInfo();
	~Game3DEffectIdInfo();
};

enum
{
	_GAME3DEFFECT_STATE_NEW		= 0,
	_GAME3DEFFECT_STATE_DELAY	= 1,
	_GAME3DEFFECT_STATE_PLAYING	= 2,
	_GAME3DEFFECT_STATE_LOOPGAP	= 3,
	_GAME3DEFFECT_STATE_ENDED	= 4,
};

typedef struct
{
	Game3DEffectIdInfo* pIDInfo;
	int		nInfoAmount;
	char	szIndex[64];
	DWORD	dwTimeDelay;
	DWORD	dwCurrentFrameIndex;
	DWORD	dwTimeBegin;
	DWORD	dwFrameInterval;
	DWORD	dwLoopTime;
	DWORD	dwLoopInterval;
	int		nOffsetX;
	int		nOffsetY;
	int		nOffsetZ;
	int		nState;
	bool	bAutoDelete;
	int		nASB[_MAXPART_MY3DEFFECT];
	int		nADB[_MAXPART_MY3DEFFECT];
	int		nScale[_MAXPART_MY3DEFFECT];
	float	fPosX, fPosY, fPosZ;
	float	fScaleX, fScaleY, fScaleZ;
	float	fRotateX, fRotateY, fRotateZ;
	DWORD	dwColor;
	float	fGravity[3];
	float	fExternalSpeed[3];
	DWORD	dwPauseTime;
	int		nId;
	DWORD	dwStepCount;

}Game3DEffectExInfo;

#include <deque>
using namespace std;
typedef deque<Game3DEffectExInfo*>DEQUE_3DEFFECTEXINFO;

class IGame3DEffectHandler
{
public:
	virtual void	OnStart		( int nId ) = 0;
	virtual void	OnLoop		( int nId ) = 0;
	virtual void	OnLoopGap	( int nId ) = 0;
	virtual void	OnEnd		( int nId ) = 0;
	virtual void	OnDelete	( int nId ) = 0;
};

class IGame3DEffect
{
public:
	virtual DWORD	Release		( void ) = 0;
	virtual BOOL	Process		( void ) = 0;
	virtual void	Clear		( void ) = 0;
	virtual BOOL	Show		( CMyPos posWorld, int nId = -1 ) = 0;
	virtual int		Add			( const char* pszIndex, bool bAutoDelete = true ) = 0;
	virtual void	Delete		( int nId ) = 0;
	virtual void	DeleteByName( const char* pszIndex ) = 0;
	virtual void	Pause		( int nId = -1 ) = 0;
	virtual void	Resume		( int nId = -1 ) = 0;
	virtual void	SkipFrames	( int nFrames, int nId = -1 ) = 0;
	virtual void	SkipTime	( DWORD dwTime, int nId = -1 ) = 0;
	virtual void	Rotate		( float x, float y, float z, int nIndex = -1, bool bById = true ) = 0;
	virtual void	Scale		( float x, float y, float z, int nIndex = -1, bool bById = true ) = 0;
	virtual void	SetPos		( float x, float y, float z, int nIndex = -1, bool bById = true ) = 0;
	virtual void	SetDir		( int nAngle, int nId = -1 ) = 0;
	virtual void	BindMatrix	( D3DXMATRIXA16* pMatrix, int nId = -1 ) = 0;
	virtual void	Reset		( void ) = 0;
	virtual void	SetOffset3	( int nX, int nY, int nZ, int nId = -1 ) = 0;
	virtual void	SetOffset	( int nOffset ) = 0;
	virtual void	SetHeight	( float fHeight ) = 0;
	virtual void	SetOblique	( BOOL bOblique ) = 0;
	virtual void	SetColor	( DWORD dwColor, int nIndex = -1, bool bById = true ) = 0;
	virtual void	SetBlend	( int nASB, int nADB, int nId = -1, int nPart = -1 ) = 0;
	virtual DWORD	GetFrames	( int nId = -1 ) = 0;
	virtual BOOL	TestEffect	( const char* pszIndex ) = 0;
	virtual int		GetId		( const char* pszIndex ) = 0;
	virtual void	EverPlay	( int nId = -1 ) = 0;
	virtual void	SetHandler	( IGame3DEffectHandler* pHandler ) = 0;
	virtual void	SetPtcl3Gravity		( float x, float y, float z, int nId = -1 ) = 0;
	virtual void	SetPtcl3ExternalSpeed ( float x, float y, float z, int nId = -1 ) = 0;
	virtual void	SetTexture	( OBJID idTexture, int nId = -1, int nPart = -1 ) = 0;
	virtual BOOL	Reload		( void ) = 0;
	virtual ~IGame3DEffect() {}
};

extern "C" ROLE3D_API IGame3DEffect* Game3DEffectCreate ( IGame3DEffectHandler* pHandler );

class CGame3DEffectEx2 : public IGame3DEffect
{
public:
	CGame3DEffectEx2(IGame3DEffectHandler* pHandler);
	virtual ~CGame3DEffectEx2();
	void*	operator new(size_t size) throw();
	void	operator delete(void* p);

public:
	virtual DWORD	Release		( void );
	virtual BOOL	Process		( void );
	virtual void	Clear		( void );
	virtual BOOL	Show		( CMyPos posWorld, int nId = -1 );
	virtual int		Add			( const char* pszIndex, bool bAutoDelete = true );
	virtual void	Delete		( int nId );
	virtual void	DeleteByName( const char* pszIndex );
	virtual void	Pause		( int nId = -1 );
	virtual void	Resume		( int nId = -1 );
	virtual void	SkipFrames	( int nFrames, int nId = -1 );
	virtual void	SkipTime	( DWORD dwTime, int nId = -1 );
	virtual void	Rotate		( float x, float y, float z, int nIndex = -1, bool bById = true );
	virtual void	Scale		( float x, float y, float z, int nIndex = -1, bool bById = true );
	virtual void	SetPos		( float x, float y, float z, int nIndex = -1, bool bById = true );
	virtual void	SetDir		( int nAngle, int nId = -1 );
	virtual void	BindMatrix	( D3DXMATRIXA16* pMatrix, int nId = -1 );
	virtual void	Reset		( void );
	virtual void	SetOffset3	( int nX, int nY, int nZ, int nId = -1 );
	virtual void	SetOffset	( int nOffset ){m_nOffsetY = nOffset;}
	virtual void	SetHeight	( float fHeight );
	virtual void	SetOblique	( BOOL bOblique ){m_bOblique = bOblique;}
	virtual void	SetColor	( DWORD dwColor, int nIndex = -1, bool bById = true );
	virtual void	SetBlend	( int nASB, int nADB, int nId = -1, int nPart = -1 );
	virtual DWORD	GetFrames	( int nId = -1 );
	virtual BOOL	TestEffect	( const char* pszIndex );
	virtual int		GetId		( const char* pszIndex );
	virtual void	EverPlay	( int nId = -1 );
	virtual void	SetHandler	( IGame3DEffectHandler* pHandler ){m_pHandler = pHandler;}
	virtual void	SetPtcl3Gravity		( float x, float y, float z, int nId = -1 );
	virtual void	SetPtcl3ExternalSpeed ( float x, float y, float z, int nId = -1 );
	virtual void	SetTexture	( OBJID idTexture, int nId = -1, int nPart = -1 );
	virtual BOOL	Reload		( void );

	void	ShowPart	( class C3DRolePart* pPart );
private:
	int		m_nOffsetY;
	float	m_Height;
	BOOL	m_bOblique;
	int		m_nUnused;
	float	m_fX, m_fY, m_fZ;
	float	m_fPosX, m_fPosY, m_fPosZ;
	float	m_fScaleX, m_fScaleY, m_fScaleZ;
private:
	void Show(Game3DEffectExInfo* pInfo, CMyPos posWorld);
	void ShowPart(Game3DEffectExInfo* pInfo, class C3DRolePart* pPart);
	BOOL Process(Game3DEffectExInfo* pInfo);
	void DeleteInfo(Game3DEffectExInfo* pInfo);
	static BOOL IsLoaded(Game3DEffectExInfo* pInfo);
	void BuildInfo(Game3DEffectExInfo* pInfo, const CMy3DEffectInfo* pEffectInfo);
private:
	DEQUE_3DEFFECTEXINFO m_setInfo;
	IGame3DEffectHandler* m_pHandler;

};

extern int g_nAdjustOblique;
extern int g_nAdjustSimpleObj;
extern int g_nAdjustEffect;
extern int g_nAdjustEffectOrder;
const int _ADJUST_UNSET = 0x05BAACFB;

#endif
