#if !defined(AFX_DLGNPCBUY_H__1D440B00_667A_4B83_96A1_EAE50D717B19__INCLUDED_)
#define AFX_DLGNPCBUY_H__1D440B00_667A_4B83_96A1_EAE50D717B19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyGrid.h"
#include "MyStatic.h"
#include "MyButton.h"

const int SHOP_AMOUNT = 20 ;

class CDlgNpcBuy : public CDialog
{
public:
	void Show ();
	CDlgNpcBuy(CWnd* pParent = NULL);
	BOOL		m_bShow ;
private:
	HBRUSH		m_Hbr ;
	BOOL		m_bInitSuc ;
	CPoint		m_Pnt ;
	int 		m_nPntX ;
	int			m_nCurrentAmount ;
	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;
public:
	void CloseShop();
	int  OpenShop (OBJID idNpc );
	enum { IDD = IDD_DIALOG_NPCBUY };
	CMyGrid		m_NpcBuySaleGri;
	CMyButton	m_NpcBuyCloseBtn ;
	CMyButton	m_NpcBuyFixBtn ;
	CMyButton	m_BtnHelp ;
	CMyButton	m_NpcButUpBtn ;
	CMyButton	m_NpcButDownBtn ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnNpcbuyBtnClose();
	afx_msg void OnDoubleclickedNpcbuyGriSale();
	afx_msg void OnNpcbuyBtnFix();
	afx_msg void OnNpcbuyBtnUp();
	afx_msg void OnNpcbuyBtnDown();
	afx_msg void OnBtnHelp();
	DECLARE_MESSAGE_MAP()

	friend struct DlgNpcBuyProbe ;
};

#endif
