#if !defined(AFX_DLGOTHER_H__E1A93628_95E0_466C_A1B1_198D41DE3933__INCLUDED_)
#define AFX_DLGOTHER_H__E1A93628_95E0_466C_A1B1_198D41DE3933__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"

class CDlgOther : public CDialog
{
public:
	void Show();
	CDlgOther(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	enum { IDD = IDD_DIALOG_OTHER };
	CMyButton	m_OtherBtnLay;
	CMyButton	m_OtherBtnDel;
	CMyButton	m_BtnCloseB;
	CMyButton	m_BtnClose;
	CMyButton	m_BtnHelp;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnBtnCloseb();
	afx_msg void OnBtnClose();
	afx_msg void OnBtnHelp();
	afx_msg void OnOtherBtnLay();
	afx_msg void OnOtherBtnDel();
	DECLARE_MESSAGE_MAP()

	friend struct DlgOtherProbe ;
};

#endif
