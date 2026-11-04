#if !defined(AFX_DLGCOMPOSE_H__SEIFUKU_275__INCLUDED_)
#define AFX_DLGCOMPOSE_H__SEIFUKU_275__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyImage.h"
#include "MyButton.h"

class CDlgCompose : public CDialog
{
public:
	CDlgCompose(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	int 				m_nPntX ;
	BOOL				m_bPickUp ;
	int					m_nIconRealID ;
	OBJID				m_idItem ;
	OBJID				m_idItem1 ;
	OBJID				m_idItem2 ;
	OBJID				m_idGem1 ;
	OBJID				m_idGem2 ;
	OBJID				m_idItemType ;
	BYTE				m_btAddition ;
	BYTE				m_btLevel ;

	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;

public:
	BOOL GetMouseMoveId ( OBJID& idItem ) ;
	void SetPickUpState( BOOL bPickUp, int nIconRealID );
	void Show();
	enum { IDD = IDD_DIALOG_EXP };
	CMyImage	m_ImgItem2;
	CMyImage	m_ImgItem1;
	CMyImage	m_ImgItem;
	CMyImage	m_ImgGem2;
	CMyImage	m_ImgGem1;
	CMyButton	m_BtnOk;
	CMyButton	m_BtnHelp;
	CMyButton	m_BtnClose;
	CMyButton	m_BtnCloseB;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnBtnHelp();
	afx_msg void OnBtnClose();
	afx_msg void OnComposeBtnClose();
	afx_msg void OnComposeImgItem();
	afx_msg void OnComposeImgItem1();
	afx_msg void OnComposeImgItem2();
	afx_msg void OnComposeImgGem1();
	afx_msg void OnComposeImgGem2();
	afx_msg void OnComposeBtnOk();
	DECLARE_MESSAGE_MAP()

	friend struct DlgComposeProbe ;
};

#endif
