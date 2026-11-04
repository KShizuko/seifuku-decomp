#ifndef _3DANIOBJ_H
#define _3DANIOBJ_H

#include "ND_Bitmap.h"

class I3DAniObj
{
public:
	virtual DWORD	Release	( void ) = 0;
	virtual BOOL	Create	( const char* pszFileName ) = 0;
	virtual void	SetFrame	( DWORD dwFrame ) = 0;
	virtual void	Draw	( void ) = 0;
	virtual void	ChangeTexture ( I3DTexture* pTexture, char *objname=NULL ) = 0;
	virtual void	ChangeMixTexture ( I3DTexture* pTexture, char *objname=NULL ) = 0;
	virtual void	SetDrawType ( int nType ) = 0;
	virtual void	SetBlend	( DWORD dwAsb, DWORD dwAdb ) = 0;
	virtual void	SetColor	( DWORD dwColor ) = 0;
	virtual void	SetMaterial ( D3DMATERIAL8 *lpMaterial ) = 0;
	virtual bool	HitTest	( int nScrX, int nScrY, float* pfX, float* pfY, float* pfZ ) = 0;
};

extern "C" GRAPHIC_API I3DAniObj* Scene3DCreate ( const char* pszFileName );

#endif
