#if !defined(AFX_DLGBAG_H__SFK_4267_OWN__INCLUDED_)
#define AFX_DLGBAG_H__SFK_4267_OWN__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyGrid.h"
#include "MyButton.h"

class CDlgBag : public CDialog
{
public:
	void SetPackage();
	void SetPage( int nPage );
	void Show();
	CDlgBag(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	BYTE				m_btFrame ;
	enum { IDD = IDD_DIALOG_BAG };
	CMyGrid		m_Grid;
	CMyButton	m_BtnClose;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnBtnClose();
	DECLARE_MESSAGE_MAP()

	friend struct DlgBagProbe ;
	friend class CMyShellDlg ;
};

#endif
