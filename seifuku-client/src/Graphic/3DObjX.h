#ifndef _3DOBJX_H
#define _3DOBJX_H

#include "ND_Bitmap.h"
#include "MyHeap.h"

extern int s_nShowMode;

class C3DObjX : public I3DObj
{
public:
	C3DObjX ();
	virtual ~C3DObjX ();
	static void* operator new(size_t uSize) throw()	{ return s_heap.Alloc(uSize); }
	static void	 operator delete(void* p)	{ s_heap.Free(p); }
	static CMyHeap	s_heap;

	virtual DWORD	Release	( void );
	virtual bool	IsValid ( void );
	virtual BOOL	IsPhyHidden ( DWORD dwIndex )	{return m_bHidden[dwIndex];}
	virtual BOOL	Create	( char *filename );
	virtual void	Destroy ();
	virtual void	Move ( float x, float y, float z );
	virtual void	Rotate	( float x, float y, float z );
	virtual void	Scale	( float x, float y, float z );
	virtual void	Muliply ( D3DXMATRIXA16 *matrix );
	virtual void	SetARGB ( float alpha, float red, float green, float blue );
	virtual int		GetIndexByName ( char *lpName );
	virtual char*	GetPhyName ( DWORD dwIndex );
	virtual DWORD	GetPhyAmount ( void )	{return m_dwPhyNum;}
	virtual void	GetBoundingBox ( DWORD dwIndex, D3DXVECTOR3 *lpMin, D3DXVECTOR3 *lpMax );
	virtual void	SetMotion ( I3DMotion* pMotion );
	virtual void	SetBlendMotion ( I3DMotion* pMotion, float fWeight );
	virtual C3Motion*	GetMotion ( DWORD dwIndex );
	virtual C3Motion*	GetBlendMotion ( DWORD dwIndex );
	virtual D3DXMATRIXA16*	GetBoneMatrix ( DWORD dwIndex, DWORD dwBone );
	virtual D3DXMATRIXA16*	GetPhyMatrix ( DWORD dwIndex );
	virtual void	NextFrame ( int nStep );
	virtual void	SetFrame (DWORD dwFrame);
	virtual void	SetBlendFrame (DWORD dwFrame);
	virtual void	Draw ( int type, float lightx, float lighty, float lightz, float sa, float sr, float sg, float sb, int nAsb, int nAdb );
	virtual void	ChangeTexture ( I3DTexture* pTexture, char *objname );
	virtual void	ChangeMixTexture ( I3DTexture* pTexture, char *objname );
	virtual void	SetDrawType ( int nType )	{if (nType) m_nDrawType = nType;}
	virtual void	SetMaterial ( D3DMATERIAL8 *lpMaterial )	{m_material = *lpMaterial;}
	virtual void	ClearMatrix ( void );
	virtual void	SetPhyMatrix ( D3DXMATRIXA16* pobjLocal, D3DXMATRIXA16* pobjWorld );
	virtual void	SetPhyHidden ( DWORD dwIndex, BOOL bHidden )	{m_bHidden[dwIndex] = bHidden;}

public:
	int				m_nDrawType;
	DWORD			m_dwPhyNum;
	DWORD			m_dwStepCount;
	C3Phy			*m_phy[PHY_MAX];
	BOOL			m_bHidden[PHY_MAX];
	D3DMATERIAL8	m_material;
};

#endif
