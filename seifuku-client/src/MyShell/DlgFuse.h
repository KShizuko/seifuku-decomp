#if !defined(AFX_DLGFUSE_H__SEIFUKU_278__INCLUDED_)
#define AFX_DLGFUSE_H__SEIFUKU_278__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyCheck.h"
#include "MyStatic.h"
#include "MyGrid.h"
#include "Hero.h"

const int	FUSE_ITEM_AMOUNT = 20 ;

class CDlgFuse : public CDialog
{
public:
	void Show ();
	CDlgFuse(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	int 				m_nPntX ;

	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;

	FUSE_INFO			m_FuseInfo ;
	BYTE				m_btImage ;
	OBJID				m_idItem ;

public:
	void Open ( BYTE btImage, OBJID idItem ) ;
	void AddItem ( OBJID idItem ) ;
	void DelItem ( OBJID idItem ) ;
	void SetItem () ;
	void OnBtnClose () ;
	enum { IDD = IDD_DIALOG_FUSE };
	CMyStatic	m_StaOdds ;
	CMyStatic	m_StaLevel ;
	CMyGrid		m_GriLay ;
	CMyButton	m_BtnFuse ;
	CMyCheck	m_ChkOk ;
	CMyButton	m_BtnHelp ;
	CMyButton	m_BtnCloseC ;
	CMyButton	m_BtnClose ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnBtnHelp();
	afx_msg void OnChkOk();
	afx_msg void OnBtnFuse();
	afx_msg void OnBtnCloseB();
	DECLARE_MESSAGE_MAP()

	friend struct DlgFuseProbe ;
};

#endif
