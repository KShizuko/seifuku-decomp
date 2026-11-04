#if !defined(AFX_ZOOMINIMGSTRING_H__SFK_4294_INCLUDED_)
#define AFX_ZOOMINIMGSTRING_H__SFK_4294_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"
#include "ImageStringEx.h"
#include <string>

class CZoomInImgString
{
	friend struct ZoomInImgStringProbe;
public:
	CZoomInImgString();
	virtual ~CZoomInImgString();

	void	SetEffectPrefix	(const char* pszPrefix)	{ m_strEffectPrefix = pszPrefix; }
	void	Add				(const char* pszString);
	void	Show			(void);

private:
	std::string		m_strEffectPrefix;
	int				m_nStep;
	DWORD			m_dwBeginTime;
	CImageStringEx*	m_pImgString;
};

#endif
