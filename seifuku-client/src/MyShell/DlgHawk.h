#if !defined(AFX_DLGHAWK_H__21A6B2A7_5659_44D2_A72F_2CA0BB6402C4__INCLUDED_)
#define AFX_DLGHAWK_H__21A6B2A7_5659_44D2_A72F_2CA0BB6402C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyEdit.h"

class CDlgHawk : public CDialog
{
public:
	void Show();
	CDlgHawk(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	HBRUSH				m_HbrBlack ;
	CPoint				m_Pnt ;
public:
	enum { IDD = IDD_DIALOG_HAWK };
	CMyEdit	m_EdtData;
	CMyButton	m_BtnClose;
	CMyButton	m_BtnAgree;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnHawkBtnCloses();
	afx_msg void OnHawkBtnAgree();
	DECLARE_MESSAGE_MAP()

	friend struct DlgHawkProbe ;
};

#endif
