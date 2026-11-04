#ifndef _ANI_H
#define _ANI_H

#include <basefunc.h>
#include "ND_Bitmap.h"

const int _MAX_FRAME	=64;

enum {ANISTATUS_INIT=0, ANISTATUS_LOADED, ANISTATUS_FAILED, ANISTATUS_LOADING};

class IAni
{
public:
	virtual DWORD	Release		(void) = 0;

	virtual BOOL	Create		(const char* pszDataFile, const char* pszIndex, BOOL bHalf = false) = 0;
	virtual BOOL	Show		(UINT uFrame, int x, int y, int alpha=0, DWORD dwShowWay=_SHOWWAY_NORMAL) = 0;
	virtual BOOL	ShowEx		(UINT uFrame, int x, int y, RECT *lpSrc, DWORD dwWidth, DWORD dwHeight, int alpha = 0,DWORD dwShowWay = _SHOWWAY_NORMAL) = 0;

	virtual CMyBitmap*	GetFrame		(UINT uFrameIndex) = 0;
	virtual int			GetFrameAmount	(void) = 0;

	virtual int			GetStatus		(void) = 0;
	virtual void		SetStatus		(int nStatus) = 0;
	virtual DWORD		GetSize			(void) = 0;
	virtual CMySize		GetSize			(UINT uFrame) = 0;

	virtual	BOOL	Load		(void) = 0;
	virtual void	Free		(void) = 0;

	virtual DWORD   GetLife		(void) = 0;
	virtual void	SetLife		(DWORD dwLife) = 0;
	virtual void	ResetLife	(void) = 0;
	virtual DWORD   GetTimeBorn	(void) = 0;

	virtual BOOL	SetAniFilename(const char *pszDataFile, const char *pszIndex) = 0;
};

extern "C" GRAPHIC_API IAni* AniCreate ( const char* pszDataFile, const char* pszIndex, BOOL bHalf );

#endif
