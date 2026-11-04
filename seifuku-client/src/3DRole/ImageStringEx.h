#if !defined(AFX_IMAGESTRINGEX_H__SFK_4294_INCLUDED_)
#define AFX_IMAGESTRINGEX_H__SFK_4294_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"
#include "Game3DEffectEx.h"
#include "MyHeap.h"
#include <string>
#include <vector>

class CImageStringEx
{
	friend struct ImageStringExProbe;
public:
	CImageStringEx();
	virtual ~CImageStringEx();

	static void* operator new(size_t uSize)	{ return s_heap.Alloc(uSize); }
	static void	 operator delete(void* p)	{ s_heap.Free(p); }

	void	SetString	(const char* pszString, vector<string>& setEffect);
	void	Show		(void);

	void	SetShowPos	(CMyPos posShow)			{ m_posShow = posShow; }
	void	SetShowPos	(int x, int y)				{ m_posShow.x = x; m_posShow.y = y; }
	void	SetColor	(DWORD dwColor)				{ m_dwColor = dwColor; }
	void	SetScale	(float fScaleX, float fScaleY)	{ m_fScaleX = fScaleX; m_fScaleY = fScaleY; }
	void	SetRotate	(int nX, int nY, int nZ)	{ m_nRotateX = nX; m_nRotateY = nY; m_nRotateZ = nZ; }
	void	SetCharWidth(int nWidth)				{ m_nCharWidth = nWidth; }
	int		GetCharWidth()							{ return m_nCharWidth; }
	DWORD	GetColor()							{ return m_dwColor; }
	const char* GetString()						{ return m_szString; }
	int		GetStrLen()						{ return strlen(m_szString); }

private:
	void	ShowEffect	(int x, int y, IGame3DEffect* pEffect);

private:
	CHAR	m_szString[32];
	std::vector<int>	m_setId;
	CMyPos	m_posShow;
	DWORD	m_dwColor;
	float	m_fScaleX;
	float	m_fScaleY;
	int		m_nRotateX;
	int		m_nRotateZ;
	int		m_nRotateY;
	int		m_nCharWidth;
	IGame3DEffect*	m_objEffect;

public:
	static CMyHeap	s_heap;
private:
};

#endif
