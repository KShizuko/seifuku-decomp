#if !defined(AFX_3DEFFECT_H__E643C394_B705_4DAF_A473_BA9A68205F64__INCLUDED_)
#define AFX_3DEFFECT_H__E643C394_B705_4DAF_A473_BA9A68205F64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include <BaseFunc.h>
#include "ND_Bitmap.h"
#include "c3_shape.h"
#include "c3_ptcl.h"

const int _EFFECT_PART_MAX_	 = 32;

class I3DEffect
{
public:
	virtual DWORD	Release ( void ) = 0;
	virtual BOOL	Load ( char *lpFilename ) = 0;
	virtual void	Unload ( void ) = 0;
	virtual void	Bind ( I3DObj *lpObj ) = 0;
	virtual void	Draw ( BOOL bNextFrame = true, int nAsb=5, int nAdb=6 ) = 0;
	virtual void	SetTexture ( I3DTexture* pTexture ) = 0;
	virtual void	ResetShape ( void ) = 0;

	virtual void	Move ( float x, float y, float z ) = 0;
	virtual void	Rotate ( float x, float y, float z ) = 0;
	virtual void	Scale ( float x, float y, float z ) = 0;
	virtual void	BindMatrix ( D3DXMATRIXA16 *lpMatrix ) = 0;
	virtual void	SetColor ( DWORD dwColor ) = 0;

	virtual DWORD	GetPtclAmount () = 0;
	virtual void	SetPtclCurFrame ( DWORD dwIndex, DWORD dwFrame ) = 0;
	virtual DWORD	GetPtclFrames ( DWORD dwIndex ) = 0;
	virtual DWORD	GetPhyAmount () = 0;
	virtual void	SetPhyCurFrame ( DWORD dwIndex, DWORD dwFrame ) = 0;
	virtual DWORD	GetPhyFrames ( DWORD dwIndex ) = 0;
	virtual DWORD	GetFrames () = 0;
	virtual void	SetFrames ( DWORD dwFrame ) = 0;

	virtual DWORD	GetShapeAmount () = 0;

	virtual ShapeBackupInfo*	CreateShapeInfo	(void) = 0;
	virtual void	BackupShapeInfo		(ShapeBackupInfo* pBuf) = 0;
	virtual void	RestoreShapeInfo	(ShapeBackupInfo* pBuf) = 0;

	virtual PTCL_INFO*	CreatePtclInfo	(void) = 0;
	virtual void	SetPtclInfo			(PTCL_INFO* pInfo) = 0;

	virtual PTCL3_INFO*	CreatePtcl3Info	(void) = 0;
	virtual void	SetPtcl3Info		(PTCL3_INFO* pInfo) = 0;
	virtual void	SetPtcl3Gravity		(float x, float y, float z) = 0;
	virtual void	SetPtcl3ExternalSpeed (float x, float y, float z) = 0;
	virtual void	SetStepCount		(DWORD dwStep) = 0;
};

extern "C" GRAPHIC_API I3DEffect* EffectCreate ( char* pszFileName );
extern "C" GRAPHIC_API void DestroyShapeInfo ( ShapeBackupInfo*& pBuf, int nShapes );
extern "C" GRAPHIC_API void DestroyPtclInfo ( PTCL_INFO*& pInfo );
extern "C" GRAPHIC_API void DestroyPtcl3Info ( PTCL3_INFO*& pInfo );

#endif
