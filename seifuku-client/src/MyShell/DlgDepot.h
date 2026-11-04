#if !defined(AFX_DLGDEPOT_H__DFE57E0E_6953_49F6_9820_AE06E69745A3__INCLUDED_)
#define AFX_DLGDEPOT_H__DFE57E0E_6953_49F6_9820_AE06E69745A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyEdit.h"
#include "MyStatic.h"
#include "MyGrid.h"

const int	DEPOT_PAGE_AMOUNT = 20 ;

class CDlgDepot : public CDialog
{
public:
	void Show ();
	CDlgDepot(CWnd* pParent = NULL);
	BOOL				m_bShow ;
	int					m_nCurPage ;
private:
	HBRUSH				m_Hbr ;
	HBRUSH				m_HbrBlack ;
	int					m_nMoney ;
	int					m_nChange ;
	BOOL				m_bInitSuc ;
	CPoint				m_Pnt ;
	int 				m_nPntX ;

	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;

public:
	void OnDepotClose();
	void CloseDepot ();
	void SetItem ();
	void SetMoney ();
	BOOL OpenDepot ( OBJID DepotID );
	enum { IDD = IDD_DIALOG_DEPOT };
	CMyStatic	m_DepotStaPage ;
	CMyStatic	m_DepotStaAllPage ;
	CMyButton	m_DepotBtnU ;
	CMyButton	m_DepotBtnD ;
	CMyButton	m_DepotCloseB;
	CMyButton	m_DepotBtnAdd ;
	CMyButton	m_DepotBtnGet ;
	CMyButton	m_DepotClose ;
	CMyButton	m_BtnHelp ;
	CMyEdit		m_DepotEdtMoney ;
	CMyStatic	m_DepotStaLay ;
	CMyGrid		m_DepotGriLay ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDepotBtnGet();
	afx_msg void OnDepotBtnAdd();
	afx_msg void OnBtnHelp();
	afx_msg void OnDepotBtnU();
	afx_msg void OnDepotBtnD();
	afx_msg void OnDepotCloseb();
	afx_msg void OnChangeDepotEdtMoney();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()

	friend struct DlgDepotProbe ;
};

#endif
