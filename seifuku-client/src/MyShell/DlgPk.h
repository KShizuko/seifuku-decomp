#if !defined(AFX_DLGPK_H__FA3BAF32_D9BC_493A_8640_C33B0469D863__INCLUDED_)
#define AFX_DLGPK_H__FA3BAF32_D9BC_493A_8640_C33B0469D863__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"

class CDlgPk : public CDialog
{
public:
	void Show();
	CDlgPk(CWnd* pParent = NULL);

	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	enum { IDD = IDD_DIALOG_PK };
	CMyButton	m_PkArre;
	CMyButton	m_PkFree ;
	CMyButton	m_PkGroup ;
	CMyButton	m_PkSafe ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnPkFree();
	afx_msg void OnPkGroup();
	afx_msg void OnPkSafe();
	afx_msg void OnPkArrestment();
	DECLARE_MESSAGE_MAP()

	friend struct DlgPkProbe ;
};

#endif
