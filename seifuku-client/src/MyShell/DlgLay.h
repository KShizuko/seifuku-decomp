#if !defined(AFX_DLGLAY_H__047CB3D8_492F_40E4_80D7_5E268DC3EC56__INCLUDED_)
#define AFX_DLGLAY_H__047CB3D8_492F_40E4_80D7_5E268DC3EC56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"

class CDlgLay : public CDialog
{
public:
	CDlgLay(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	int 				m_nPntX ;
	int					m_nDir ;

	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;
public:
	void Show();
	enum { IDD = IDD_DIALOG_LAY };
	CMyButton	m_LayRight;
	CMyButton	m_LayLeft;
	CMyButton	m_LayCloseB;
	CMyButton	m_LayBtnCloseS;
	CMyButton	m_BtnOk;
	CMyButton	m_BtnHelp;
	CMyButton	m_BtnCancel;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnLayBtnCloses();
	afx_msg void OnBtnHelp();
	afx_msg void OnLayCloseb();
	afx_msg void OnLayLeft();
	afx_msg void OnLayRight();
	afx_msg void OnBtnOk();
	afx_msg void OnBtnCancel();
	DECLARE_MESSAGE_MAP()

	friend struct DlgLayProbe ;
};

#endif
