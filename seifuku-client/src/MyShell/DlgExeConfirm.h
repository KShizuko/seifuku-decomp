#if !defined(AFX_DLGEXECONFIRM_H__4343_0040EAC0_INCLUDED_)
#define AFX_DLGEXECONFIRM_H__4343_0040EAC0_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"

class CDlgExeConfirm : public CDialog
{
public:
	void SetTime ( int nTime ) ;
	void Show();
	CDlgExeConfirm(CWnd* pParent = NULL);

private:
	HBRUSH				m_Hbr ;
	bool				m_bInitSuc ;
	CPoint				m_Pnt ;
public:
	BOOL				m_bShow ;
private:
	int					m_nTime ;
public:
	enum { IDD = IDD_DIALOG_EXECONFIRM };
	CMyButton			m_BtnOk ;
	CMyButton			m_BtnHelp ;
	CMyButton			m_BtnCancel ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnBtnCancel();
	afx_msg void OnBtnOk();
	afx_msg void OnBtnHelp();
	DECLARE_MESSAGE_MAP()

	friend struct DlgExeConfirmProbe ;
};

#endif
