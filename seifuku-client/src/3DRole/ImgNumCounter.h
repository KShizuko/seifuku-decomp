#if !defined(AFX_IMGNUMCOUNTER_H__82CC27B5_6D53_4F12_9107_176944BCD986__INCLUDED_)
#define AFX_IMGNUMCOUNTER_H__82CC27B5_6D53_4F12_9107_176944BCD986__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ImageStringEx.h"
#include <vector>

typedef struct
{
	CImageStringEx*	pImgString;
	DWORD			dwBeginTime;
}NumImgInfo;
typedef vector<NumImgInfo> VEC_NUMIMG;

class CImgNumCounter
{
	friend struct ImgNumCounterProbe;
public:
	CImgNumCounter();
	virtual ~CImgNumCounter();

	void	Start		(int nNumber, int nStep=-1, DWORD dwInterval=1000);
	void	Reset		(void);

	void	Scale		(int nPercent)		{ m_dwZoomPercent = nPercent; }
	void	SetShowPos	(CMyPos posShow)	{ m_posShow = posShow; }
	BOOL	Show		(void);
	void	End			(void);
private:
	int		m_nNumber;
	int		m_nCurrent;
	int		m_nStep;
	DWORD	m_dwBeginTime;
	DWORD	m_dwInterval;
	DWORD	m_dwColor;
	CMyPos	m_posShow;
	DWORD	m_dwZoomPercent;
	VEC_NUMIMG	m_setNumImg;
};

extern CImgNumCounter g_objImgNumCounter;

#endif
