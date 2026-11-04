#if !defined(AFX_DLGINSHELP_H__AA7EFB31_A361_4DA3_B32F_98B8A6F64099__INCLUDED_)
#define AFX_DLGINSHELP_H__AA7EFB31_A361_4DA3_B32F_98B8A6F64099__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"

class CDlgInsHelp : public CDialog
{
public:
	void Show();
	void ShowHelpIcon( BOOL bFlash );
	CDlgInsHelp(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	BOOL				m_bFlash ;
	BOOL				m_bTipShow ;
	int					m_nCurTipIndex ;
public:
	enum { IDD = IDD_DIALOG_INSHELP };
	CMyButton	m_InsHelpBtnCloseB;
	CMyButton	m_InsHelpBtnTip;
	CMyButton	m_InsHelpBtnPageUp;
	CMyButton	m_InsHelpBtnPageDown;
	CMyButton	m_InsHelpBtnHelp;
	CMyButton	m_InsHelpBtnMore;
	CMyButton	m_BtnClose;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnInshelpBtnCloses();
	afx_msg void OnInshelpBtnMore();
	afx_msg void OnInshelpBtnHelp();
	afx_msg void OnInshelpBtnTip();
	afx_msg void OnInshelpBtnPageup();
	afx_msg void OnInshelpBtnPagedown();
	DECLARE_MESSAGE_MAP()

	friend struct DlgInsHelpProbe ;
};

#endif
