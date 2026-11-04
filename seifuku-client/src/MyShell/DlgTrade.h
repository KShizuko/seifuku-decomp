#if !defined(AFX_DLGTRADE_H__FDF39A2D_36C3_48F1_85CD_854601D04581__INCLUDED_)
#define AFX_DLGTRADE_H__FDF39A2D_36C3_48F1_85CD_854601D04581__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyGrid.h"
#include "MyStatic.h"
#include "MyButton.h"
#include "MyEdit.h"
#include "MyImage.h"
#include "MyCheck.h"

const int	TRADE_PAGE_AMOUNT = 6 ;

class CDlgTrade : public CDialog
{
public:
	CDlgTrade(CWnd* pParent = NULL);
	BOOL		m_bShow ;
private:
	int			m_nNpcPage ;
	int			m_nMpcPage ;
	HBRUSH		m_Hbr ;
	HBRUSH		m_HbrStatic ;
	int			m_nMoney ;
	int			m_nEmoney ;
	int			m_nChange ;
	int			m_nEmoneyChange ;
	BOOL		m_bInitSuc ;
	CPoint		m_Pnt ;
	BOOL		m_bMoneyAdd ;
	int			m_nNpcNameLen ;
public:
	void ResetTrade();
	void SetNpcMoney ();
	void SetNpcEmoney ();
	void SetNpcGridContain();
	void SetMpcGridContain();
	void Show();
	enum { IDD = IDD_DIALOG_TRADE };
	CMyStatic	m_TradeStaNpcEmoney ;
	CMyEdit		m_TradeEdtMpcEmoney ;
	CMyStatic	m_TradeStaNpcLevel ;
	CMyStatic	m_TradeStaNpcAllPage ;
	CMyStatic	m_TradeStaMpcAllPage ;
	CMyButton	m_TradeBtnNpcU ;
	CMyButton	m_TradeBtnNpcD ;
	CMyButton	m_TradeBtnMpcU ;
	CMyButton	m_TradeBtnMpcD ;
	CMyButton	m_TradeCloseB;
	CMyGrid	m_TradeGriNpc ;
	CMyGrid m_TradeGriMpc ;
	CMyStatic m_TradeStaNpcName ;
	CMyStatic m_TradeStaNpcMoney ;
	CMyStatic m_TradeStaMpcPage ;
	CMyStatic m_TradeStaNpcPage ;
	CMyEdit	  m_TradeEdtMpcMoney ;
	CMyStatic m_TradeStaNpcAgree ;
	CMyButton m_TradeBtnMpcAgree ;
	CMyButton m_TradeBtnMoneyMpcOk ;
	CMyButton m_TradeBtnClose ;
	CMyButton m_BtnHelp ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnTradeBtnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTradeBtnMpcagree();
	afx_msg void OnBtnHelp();
	afx_msg void OnTradeCloseb();
	afx_msg void OnTradeBtnMoneympcok();
	afx_msg void OnTradeBtnMpcD();
	afx_msg void OnTradeBtnMpcU();
	afx_msg void OnTradeBtnNpcD();
	afx_msg void OnTradeBtnNpcU();
	afx_msg void OnChangeTradeEdtMpcmoney();
	afx_msg void OnChangeTradeEdtMpcemoney();
	DECLARE_MESSAGE_MAP()

	friend struct DlgTradeProbe ;
	friend class CMyShellDlg ;
};

#endif
