#if !defined(AFX_MYSCROLLBUTTON_H__D4F2A33E_BAAF_4B8B_B16B_400A6231FA49__INCLUDED_)
#define AFX_MYSCROLLBUTTON_H__D4F2A33E_BAAF_4B8B_B16B_400A6231FA49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif
#define MYSCROLL_UP		0
#define MYSCROLL_DOWN	1
#define MYSCROLL_ROLL	2
#define MYSCROLL_DROP	3
class CMyScrollButton : public CButton
{
public:
	CMyScrollButton();

private:
	CWnd *m_ParentWnd ;
	BYTE m_btScrBtnType ;
	char m_strScrButtonID[64] ;
	CPoint m_ScrBtnPnt ;
	BOOL	m_bDrpValue ;
	UINT	m_uFrame ;
public:

	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void Show ( int xPos, int yPos );
	BOOL Load ( char *ScrButtonID);
	void Init ( CWnd* ListWnd, BYTE btScrBtnType );
	virtual ~CMyScrollButton();

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
	friend struct MyScrollButtonProbe;
};

#endif
