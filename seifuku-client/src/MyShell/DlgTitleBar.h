#if !defined(AFX_DLGTITLEBAR_H__60E1197D_AD00_428D_98F3_81251D8EA922__INCLUDED_)
#define AFX_DLGTITLEBAR_H__60E1197D_AD00_428D_98F3_81251D8EA922__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class CDlgTitleBar : public CDialog
{
public:
	void ResetBk( int nMode );
	CPoint GetOffset();
	CDlgTitleBar(CWnd* pParent = NULL);
	BOOL	m_bLBtnDown ;
	CRect   m_InitDlgRect ;
	CPoint	m_InitDlgPnt ;
	CPoint  m_MouseMovePnt ;
	HBRUSH	m_Hbr ;
	HBITMAP	m_hBmp ;
	CWnd	*m_pParentWnd ;
	enum { IDD = IDD_DIALOG_TITLEBAR };
	CButton	m_5;
	CButton	m_BtnMin;
	CButton	m_BtnClose;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBtnClose();
	afx_msg void OnBtnMin();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
};

#endif
