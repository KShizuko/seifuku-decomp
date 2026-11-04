#if !defined(AFX_ZOOMNUM_H__43C6571A_40D0_4AC0_8DC8_378B7B5672C5__INCLUDED_)
#define AFX_ZOOMNUM_H__43C6571A_40D0_4AC0_8DC8_378B7B5672C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"
#include "ImageStringEx.h"
struct ZoomNumInfo
{
	ZoomNumInfo()	{ nNum = 0; pImgString = NULL; }
	int nNum;
	DWORD dwTimeBegin;
	DWORD dwColor;
	DWORD dwDelay;
	int	  nOffsetX;
	CImageStringEx* pImgString;
};

#include <deque>
using namespace std;
typedef deque<ZoomNumInfo>DEQUE_ZOOMNUMINFO;

class CZoomNum
{
public:
	CZoomNum();
	virtual ~CZoomNum();

private:
	DEQUE_ZOOMNUMINFO m_setInfo;

public:
	void Add(int nNum, int nType, DWORD dwColor, DWORD dwDelay, int nRandOffset = 0);
	void Show(CMyPos posShow);
	void Process(void);
};

#endif
