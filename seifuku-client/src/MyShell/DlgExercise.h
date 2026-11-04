#if !defined(AFX_DLGEXERCISE_H__4343_00437110_INCLUDED_)
#define AFX_DLGEXERCISE_H__4343_00437110_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyStatic.h"
#include "MyButton.h"

class CDlgExercise : public CDialog
{
public:
	void SetTime ( int nTime, int nBlessTime, BOOL bBlessed = false ) ;
	int GetTotalTime () ;
	void FlashExp ( int nTable ) ;
	void Show();
	CDlgExercise(CWnd* pParent = NULL);

private:
	void RequestExp () ;
	void ClearExp () ;

	HBRUSH				m_Hbr ;
	bool				m_bInitSuc ;
	CPoint				m_Pnt ;
public:
	BOOL				m_bShow ;
private:
	int					m_nTime ;
	int					m_nPayTime ;
	int					m_nBlessTime ;
public:
	enum { IDD = IDD_DIALOG_EXERCISE };
	CMyStatic			m_StaSettle2 ;
	CMyStatic			m_StaBlessCost ;
	CMyStatic			m_StaBless ;
	CMyStatic			m_StaBlessEmoney ;
	CMyStatic			m_StaSpare ;
	CMyStatic			m_StaSettle1 ;
	CMyStatic			m_StaBalance ;
	CMyStatic			m_StaCost ;
	CMyStatic			m_StaCostBG ;
	CMyStatic			m_StaEmoney ;
	CMyStatic			m_StaEmoney2 ;
	CMyStatic			m_StaTime ;
	CMyStatic			m_StaExp ;
	CMyStatic			m_StaExpTitle ;
	CMyStatic			m_StaClock ;
	CMyButton			m_BtnSpare ;
	CMyButton			m_BtnContinue ;
	CMyButton			m_BtnHelp ;
	CMyButton			m_BtnOk ;
	CMyButton			m_BtnClose ;
	CMyButton			m_BtnPay ;

	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnBtnClose();
	afx_msg void OnBtnOk();
	afx_msg void OnBtnPay();
	afx_msg void OnBtnContinue();
	afx_msg void OnBtnHelp();
	DECLARE_MESSAGE_MAP()

	friend struct DlgExerciseProbe ;
};

#endif
