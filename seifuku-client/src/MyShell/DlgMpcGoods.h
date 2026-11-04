#if !defined(AFX_DLGMPCGOODS_H__93A5C16D_1A12_482A_8C65_E2BB4658DAEE__INCLUDED_)
#define AFX_DLGMPCGOODS_H__93A5C16D_1A12_482A_8C65_E2BB4658DAEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyGrid.h"
#include "MyStatic.h"
#include "MyButton.h"

class CDlgMpcGoods : public CDialog
{
public:
	void Show();
	CDlgMpcGoods(CWnd* pParent = NULL);
	BOOL	m_bShow ;
private:
	HBRUSH		m_Hbr ;
	BOOL		m_bInitSuc ;
	CPoint		m_Pnt ;
	int 		m_nPntX ;
	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;

public:
	void SetGridContain ();
	enum { IDD = IDD_DIALOG_MPCGOODS };
	CMyButton	m_MpcGoodsEmoneyBtn ;
	CMyStatic	m_MpcGoodsEmoneySta ;
	CMyGrid		m_MpcGoodsGri;
	CMyStatic	m_MpcGoodsMoneySta ;
	CMyButton	m_MpcGoodsCloseBtn ;
	CMyButton	m_MpcGoodsMoneyBtn ;
	CMyButton	m_BtnHelp ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMpcgoodsBtnClose();
	afx_msg void OnMpcgoodsBtnMoney();
	afx_msg void OnBtnHelp();
	afx_msg void OnMpcgoodsBtnEmoney();
	DECLARE_MESSAGE_MAP()

	friend struct DlgMpcGoodsProbe ;
};

#endif
