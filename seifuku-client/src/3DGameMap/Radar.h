#if !defined(AFX_RADAR_H__F603F447_D77F_48B4_A419_C0DD98DD7917__INCLUDED_)
#define AFX_RADAR_H__F603F447_D77F_48B4_A419_C0DD98DD7917__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"
#include "Ani.h"
#include "Hero.h"

enum{
		_MODE_128 = 0,
		_MODE_256 = 1,
		_MODE_512 = 2,
	};
class CRadar  
{
public:
	CRadar();
	virtual ~CRadar();

public:
	void CellPos2RaderPos(IAni* pAni, CMyPos posCell, CMyPos& posRader);
	void Show(OBJID idMap);
	void SetMode(int nMode) { m_nMode = nMode; }
	int GetMode() { return m_nMode; }
	void SetShowState(int nState);
	void SetHidePos(CMyPos pos) { m_posHide = pos; }

private:
	BOOL ProcessShowRect();
	void ShowMap128(IAni* pAni);
	void ShowMap256(IAni* pAni);
	static void ShowMap512(IAni* pAni);
	static void ShowMapSmall(IAni* pAni);
	void ShowFrame(CMyBitmap* pBmp, int nX, int nY);
	void ShowHero(IAni* pAni);
	void ShowMarks(IAni* pAni);

private:
	CMyPos	m_posFrame;
	float	m_fScaleX;
	float	m_fScaleY;
	int		m_nMode;
	RECT	m_rcShow;
	RECT	m_rcShowBg;
	CMyPos	m_posHide;
	int		m_nShowState;

	friend struct RadarProbe;
	friend class CGameMap;
public:
	void	SetShowRect(RECT rcShow)	{memcpy(&m_rcShow, &rcShow, sizeof(RECT)); memcpy(&m_rcShowBg, &m_rcShow, sizeof(RECT));}
};

#endif
