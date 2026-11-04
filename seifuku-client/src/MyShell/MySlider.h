#if !defined(AFX_MYSLIDER_H__C9956441_F84B_45CF_A08D_B46B11C7ADB9__INCLUDED_)
#define AFX_MYSLIDER_H__C9956441_F84B_45CF_A08D_B46B11C7ADB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class CMySlider : public CButton
{
public:
	CMySlider();

private:
	private:
	struct MySliderParameter
	{
		UINT		m_uFrame ;
		char		m_strSliderID[64] ;
		CPoint		m_IntPoint ;
		BOOL		m_bBtnDown ;
		WORD		m_wdSliderWidth ;
		WORD		m_wdSliderMaxValue ;
		int			m_nSliderPos ;
		int			m_nSliderCurPos ;
		CPoint		m_MousePoint ;
		BOOL		m_bV ;
	} m_SliPar ;
public:

	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void SetSliderValue ( int nValue );
	BOOL Init( int xPos, int yPos, char *SliderID, BOOL bType = false );
	int GetSliderValue ();
	void SetMaxValue ( int nValue );
	void SetSliderWidth ( int nWidth );
	void Show ( int x, int y );
	virtual ~CMySlider();

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
	friend struct MySliderProbe;
};

#endif
