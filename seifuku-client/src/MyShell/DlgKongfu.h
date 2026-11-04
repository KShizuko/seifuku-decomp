#if !defined(AFX_DLGKONGFU_H__2B36748F_2B85_4C0D_B1A6_CFC0CCA7554D__INCLUDED_)
#define AFX_DLGKONGFU_H__2B36748F_2B85_4C0D_B1A6_CFC0CCA7554D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyImage.h"
#include "MyButton.h"

class CDlgKongfu : public CDialog
{
public:
	void InitKongfuDialog();
	void Show();
	CDlgKongfu(CWnd* pParent = NULL);
	BOOL		m_bShow ;
private:
	HBRUSH		m_Hbr ;
	CPoint		m_Pnt ;
	DWORD		m_KongfuIdType[50] ;
	int			m_nCurBase ;
	enum { IDD = IDD_DIALOG_KONGFU };
	CMyButton	m_KongfuBtnU;
	CMyButton	m_KongfuBtnD;
	CMyImage	m_KongfuImg1;
	CMyImage	m_KongfuImg2;
	CMyImage	m_KongfuImg3;
	CMyImage	m_KongfuImg4;
	CMyImage	m_KongfuImg5;
	CMyImage	m_KongfuImg6;
	CMyImage	m_KongfuImg7;
	CMyImage	m_KongfuImg8;
	CMyImage	m_KongfuImg9;
	CMyImage	m_KongfuImg10;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnKongfuImg1();
	afx_msg void OnKongfuImg2();
	afx_msg void OnKongfuImg3();
	afx_msg void OnKongfuImg4();
	afx_msg void OnKongfuImg5();
	afx_msg void OnKongfuImg6();
	afx_msg void OnKongfuImg7();
	afx_msg void OnKongfuImg8();
	afx_msg void OnKongfuImg9();
	afx_msg void OnKongfuImg10();
	afx_msg void OnKongfuBtnUp();
	afx_msg void OnKongfuBtnDown();
	DECLARE_MESSAGE_MAP()

	friend struct DlgKongfuProbe ;
};

#endif
