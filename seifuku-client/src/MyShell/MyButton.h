#if !defined(AFX_NEWBUTTON_H__210E1717_078A_44B7_8C57_C239445F919F__INCLUDED_)
#define AFX_NEWBUTTON_H__210E1717_078A_44B7_8C57_C239445F919F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class CMyButton : public CButton
{
public:
	CMyButton();

public:
	CPoint m_BtnPnt ;
	CPoint m_IntPnt ;
	CRect m_ShowRect ;
	int m_nWidth ;
	int m_nHeight ;
	UINT m_uFrame ;
	BYTE m_btOwnerID ;
	int m_nButtonID ;
	char m_strButtonID[64] ;
public:

	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void ChangeImage ( char* ButtonID );
	void SetCurFrame(UINT uFrame);
	BOOL Init ( int xPos, int yPos, char *ButtonID, BYTE btOwnerID = 0, int nImageID = 0 );
	void Show ( int x, int y );
	virtual ~CMyButton();

protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	DECLARE_MESSAGE_MAP()
};

#endif
