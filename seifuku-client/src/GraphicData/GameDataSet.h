#ifndef _GRAPHICDATA_GAMEDATASET_H
#define _GRAPHICDATA_GAMEDATASET_H

#include <windows.h>
#include "BaseFunc.h"
#include "ND_Bitmap.h"
#include "Ani.h"
#include "3DEffect.h"

#ifdef	GRAPHICDATA_EXPORTS
#define GRAPHICDATA_API __declspec ( dllexport )
#else
#define GRAPHICDATA_API __declspec ( dllimport )
#endif

enum {EXIGENCE_NORMAL = 0, EXIGENCE_ABOVENORMAL, EXIGENCE_IMMEDIATE};

struct C3DObjInfo
{
	OBJID		id;
	I3DObj*		p3DObj;
	I3DMotion*	pDefaultMotion;
	DWORD		timeBirth;
	DWORD		dwLife;

	~C3DObjInfo();
};
const int _MAXPART_SIMPLEOBJ	= 4;
typedef struct
{
	OBJID	idType;
	int		nParts;
	OBJID	idMesh[_MAXPART_SIMPLEOBJ];
	OBJID	idTexture[_MAXPART_SIMPLEOBJ];
}C3DSimpleObjInfo;
const int _MAXPART_MY3DEFFECT	= 16;
typedef struct
{
	char szIndex[32];
	int nAmount;
	OBJID idEffect[_MAXPART_MY3DEFFECT];
	OBJID idTexture[_MAXPART_MY3DEFFECT];
	int nASB[_MAXPART_MY3DEFFECT];
	int nADB[_MAXPART_MY3DEFFECT];
	int nScale[_MAXPART_MY3DEFFECT];
	int nDelay, nLoopTime, nFrameInterval, nLoopInterval;
	int nOffsetX, nOffsetY;
	int nOffsetZ;
}CMy3DEffectInfo;

const int _MAXPART_ROLEPART	= 4;
typedef struct
{
	int				nPart;
	OBJID			idMesh[_MAXPART_ROLEPART];
	OBJID			idTexture[_MAXPART_ROLEPART];
	OBJID			idMixTex[_MAXPART_ROLEPART];
	int				nMixOpt[_MAXPART_ROLEPART];
	D3DMATERIAL8*	pMaterial[_MAXPART_ROLEPART];
	int				nAsb[_MAXPART_ROLEPART];
	int				nAdb[_MAXPART_ROLEPART];
}RolePartInfo;

typedef struct
{
	OBJID			idType;
	RolePartInfo	part;
}C3DSimpleObjExInfo;

class IDataIcon
{
public:
	virtual IAni*	GetIcon ( int nIndex ) = 0;
};

class IGameDataSet
{
public:
	virtual void		Init		( BOOL bBin ) = 0;
	virtual void		InitStrRes	( void ) = 0;
	virtual void		Destroy		( void ) = 0;
	virtual void		Process		( void ) = 0;
	virtual IAni*		GetDataAni	( const char* pszFileName, const char* pszTitle, int nExigence = EXIGENCE_NORMAL, DWORD dwLife = 30*1000 ) = 0;
	virtual I3DObj*		Get3DObj	( OBJID id, int bLoadAtOnce = 0, DWORD dwLife = 0 ) = 0;
	virtual I3DTexture*	Get3DTexture( OBJID id, int bLoadAtOnce = 0, DWORD dwLife = 0 ) = 0;
	virtual I3DEffect*	Get3DEffect	( OBJID id, int bLoadAtOnce = 0, DWORD dwLife = 0 ) = 0;
	virtual I3DMotion*	Get3DMotion	( const char* pszKind, unsigned __int64 id, int bLoadAtOnce = 0, DWORD dwLife = 0 ) = 0;
	virtual C3DObjInfo*	Get3DObjInfo( OBJID id ) = 0;
	virtual void*		Slot10		( DWORD dw ) = 0;
	virtual I3DEffect*	CreateNew3DEffect ( OBJID id ) = 0;
	virtual const char*	GetSound	( OBJID idSound ) = 0;
	virtual const char*	GetVersionInfo ( void ) = 0;
	virtual const char*	GetMotionRes ( const char* pszKind, OBJID id ) = 0;
	virtual const char*	GetStr		( OBJID idRes ) = 0;
	virtual C3DSimpleObjInfo*	Get3DSimpleObjInfo ( OBJID id ) = 0;
	virtual C3DSimpleObjExInfo*	Get3DSimpleObjExInfo ( OBJID id ) = 0;
	virtual CMy3DEffectInfo*	GetMy3DEffectInfo ( const char* pszIndex ) = 0;
	virtual CMy3DEffectInfo*	FindMy3DEffectInfo ( const char* pszIndex ) = 0;
	virtual void		AddMy3DEffectInfo ( CMy3DEffectInfo* pInfo ) = 0;
	virtual RolePartInfo*	GetRolePartInfo ( const char* pszKind, OBJID id ) = 0;
	virtual BOOL		IsHiddenPhy	( const char* pszPhyName ) = 0;
	virtual void*		QueryInterface ( int nInterface ) = 0;
	virtual int			GetEmotionAmount ( void ) = 0;
	virtual const char*	GetEmotionShutCut ( int nIndex ) = 0;
	virtual void		BuildBin	( void ) = 0;
	virtual DWORD		GetActionTransTime ( DWORD idAction, DWORD idTrans ) = 0;
	virtual void		SetLoadAtOnce ( DWORD dwMilliseconds ) = 0;
	virtual OBJID		GetMeshLod ( OBJID idMesh, int nLevel ) = 0;
	virtual BOOL		Reload ( BOOL bBin, DWORD dwFlags ) = 0;
};

extern "C" GRAPHICDATA_API IGameDataSet* GameDataSetQuery ( void );
extern "C" GRAPHICDATA_API void GameDataSetDestroy ( void );

#endif
