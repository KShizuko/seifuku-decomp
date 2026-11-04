#if !defined(AFX_DLGBLACK_H__51FAF5B7_1A75_4DD0_934C_92D672D30652__INCLUDED_)
#define AFX_DLGBLACK_H__51FAF5B7_1A75_4DD0_934C_92D672D30652__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyList.h"
#include "MyEdit.h"

class CDlgBlack : public CDialog
{
public:
	void Show();
	CDlgBlack(CWnd* pParent = NULL);
	BOOL		m_bShow ;
private:
	HBRUSH		m_Hbr ;
	CPoint		m_Pnt ;
public:
	void SetBlack();
	enum { IDD = IDD_DIALOG_BLACK };
	CMyEdit	m_BlackEdtName;
	CMyButton	m_BtnHelp;
	CMyList	m_BlackLstData;
	CMyButton	m_BlackCloseb;
	CMyButton	m_BlackBtnDel;
	CMyButton	m_BlackBtnAdd;
	CMyButton	m_BlackBtnCloses;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnBlackBtnCloses();
	afx_msg void OnBtnHelp();
	afx_msg void OnBlackCloseb();
	afx_msg void OnBlackBtnDel();
	afx_msg void OnBlackBtnAdd();
	DECLARE_MESSAGE_MAP()

	friend struct DlgBlackProbe ;
};

#endif
