#ifndef _MYBITMAPX_H
#define _MYBITMAPX_H

#include "ND_Bitmap.h"
#include "AlphaMask.h"
#include "MyHeap.h"

class CMyBitmapX : public CMyBitmap
{
public:
	CMyBitmapX();
	virtual ~CMyBitmapX();

	static void* operator new(size_t uSize) throw()	{ return s_heap.Alloc(uSize); }
	static void	 operator delete(void* p)	{ s_heap.Free(p); }
	static CMyHeap	s_heap;

	BOOL	Load	( char *pszBmpFile );
	void	Destroy ( void );

	virtual DWORD	Release	( void );
	virtual void	Show	( int x, int y, int alpha, DWORD dwShowWay );
	virtual void	ShowEx	( int x, int y, RECT *lpSrc, DWORD dwWidth, DWORD dwHeight, int alpha, DWORD dwShowWay, int nRotateAngle );
	virtual void	GetSize ( int& nWidth, int& nHeight );
	virtual DWORD	GetSize ( void );
	virtual int		GetWidth	( void );
	virtual int		GetHeight	( void );
	virtual void	SetColor	( BYTE a, BYTE r, BYTE g, BYTE b );
	virtual int		IsbTransparentFormat();
	virtual BOOL	IsTransparent(CMyPos posPoint);
	virtual void	ShowBlend ( int x, int y, RECT *lpSrc, DWORD dwWidth, DWORD dwHeight, int alpha, int nRotateAngle, DWORD dwSrcBlend, DWORD dwDestBlend );
	virtual BOOL	LockRect ( RECT rect );
	virtual void	SetPixel ( int x, int y, DWORD dwColor );
	virtual void	Unlock ( void );

public:
	 C3Sprite	*m_sprite;
	 CAlphaMask *m_pMask;
	 D3DLOCKED_RECT	m_lock;
	 RECT		m_rect;
};

#endif
