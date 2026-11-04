#if !defined(AFX_DLGXP_H__F6B281AF_5C64_48B6_AB7F_DFAC3A3CEC07__INCLUDED_)
#define AFX_DLGXP_H__F6B281AF_5C64_48B6_AB7F_DFAC3A3CEC07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyImage.h"
#include "MyButton.h"

class CDlgXp : public CDialog
{
public:
	void SetFly( BOOL bFly );
	void SetTransform ( BOOL bTrans );
	void SetBooth ( BOOL bOpen );
	void SetReborn(BOOL bDie=true);
	void InitXpDialog();
	void Show();
	CDlgXp(CWnd* pParent = NULL);
	BOOL		m_bShow ;
	BYTE		m_btBoothType ;
	BOOL		m_bDie ;
	BOOL		m_bTransform ;
	BOOL		m_bFly ;
private:
	HBRUSH		m_Hbr ;
	CPoint		m_Pnt ;
	DWORD		m_XpIdType[50] ;
	int			m_nCurBase ;
	enum { IDD = IDD_DIALOG_XP };
	CMyButton	m_XpBtnUp;
	CMyButton	m_XpBtnDown;
	CButton		m_XpBtn;
	CMyImage	m_XpImg10;
	CMyImage	m_XpImg9;
	CMyImage	m_XpImg8;
	CMyImage	m_XpImg7;
	CMyImage	m_XpImg6;
	CMyImage	m_XpImg5;
	CMyImage	m_XpImg4;
	CMyImage	m_XpImg3;
	CMyImage	m_XpImg2;
	CMyImage	m_XpImg1;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnXpImg1();
	afx_msg void OnXpImg2();
	afx_msg void OnXpImg3();
	afx_msg void OnXpImg4();
	afx_msg void OnXpImg5();
	afx_msg void OnXpImg6();
	afx_msg void OnXpImg7();
	afx_msg void OnXpImg8();
	afx_msg void OnXpImg9();
	afx_msg void OnXpImg10();
	afx_msg void OnXpBtnUp();
	afx_msg void OnXpBtnDown();
	DECLARE_MESSAGE_MAP()

	friend struct DlgXpProbe ;
};

#endif
