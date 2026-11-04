#if !defined(AFX_FONTSCALE_H__5F2A1C3E_7B44_4D0A_9E61_0C7D2B8A4F13__INCLUDED_)
#define AFX_FONTSCALE_H__5F2A1C3E_7B44_4D0A_9E61_0C7D2B8A4F13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include <map>
class CMyIni;

class CFontScale
{
public:
	CFontScale();
	virtual ~CFontScale();

	void Init ( CWnd* pDlg, int nFontSize );
	void Adjust ( CWnd* pWnd, int nID = 0 );
	void LoadIni ();
	void SetReserved ( int nValue ) { m_nUnused = nValue ; }
	BOOL  IsScale () { return m_bScale ; }
	float GetScaleX () { return m_fScaleX ; }
	float GetScaleY () { return m_fScaleY ; }

	struct FontCell
	{
		int nWidth ;
		int nHeight ;
	} m_FontCell[7] ;
	BOOL	m_bScale ;
	int		m_nUnused ;
	float	m_fScaleX ;
	float	m_fScaleY ;
	CMyIni*	m_pIni ;
	std::map<HWND, int>	m_mapWndID ;
};

#endif
