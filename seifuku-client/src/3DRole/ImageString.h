#if !defined(AFX_IMAGESTRING_H__DC21F97B_4D4E_418B_B86A_9DC845158E28__INCLUDED_)
#define AFX_IMAGESTRING_H__DC21F97B_4D4E_418B_B86A_9DC845158E28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"
#include "ImageStringEx.h"

class CImageString
{
	friend struct ImageStringProbe;
public:
	CImageString();
	virtual ~CImageString();

	static CImageString* CreateNew(const char* pszString, CMyPos posShow);

	void	SetScalePercent (DWORD dwPercent);
	void	SetShowPos		(CMyPos posShow);
	void	SetString		(const char* pszString);
	void	SetColor		(DWORD dwColor, DWORD dwAlpha);
	void	Show			(void);

private:
	float	m_fScale;
	CMyPos	m_posShow;
	DWORD	m_dwColor;
	CImageStringEx	m_objImgString;
};

#endif
