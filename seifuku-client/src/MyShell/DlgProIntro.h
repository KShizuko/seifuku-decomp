#if !defined(AFX_DLGPROINTRO_H__A59147E1_2FCB_4D98_8350_F75E406F1161__INCLUDED_)
#define AFX_DLGPROINTRO_H__A59147E1_2FCB_4D98_8350_F75E406F1161__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyStatic.h"
#include "MyButton.h"

class CDlgProIntro : public CDialog
{
public:
	void Show();
	CDlgProIntro(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
public:
	enum { IDD = IDD_DIALOG_PROINTRO };
	CMyButton	m_BtnClose;
	CMyStatic	m_Sta1;
	CMyStatic	m_Sta2;
	CMyStatic	m_Sta3;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnBtnClose();
	DECLARE_MESSAGE_MAP()

	friend struct DlgProIntroProbe ;
};

#endif
