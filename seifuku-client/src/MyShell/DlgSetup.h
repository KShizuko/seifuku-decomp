#if !defined(AFX_DLGSETUP_H__EF87810C_42E8_4B00_981A_0646A6D9AFDB__INCLUDED_)
#define AFX_DLGSETUP_H__EF87810C_42E8_4B00_981A_0646A6D9AFDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyStatic.h"
#include "MySlider.h"
#include "MyCheck.h"

const int	MAX_INTERFACE_NUM = 2 ;

class CDlgSetup : public CDialog
{
public:
	void Show ();
	CDlgSetup(CWnd* pParent = NULL);
	BOOL				m_bShow ;
	int					m_nInterface ;
private:
	HBRUSH				m_Hbr ;
	HBRUSH				m_CtlHbr ;
	BOOL				m_bInitSuc ;
	CPoint				m_Pnt ;
	int					m_nScreenMode ;
public:
	enum { IDD = IDD_DIALOG_SETUP };
	CMyButton	m_SetupBtnRight;
	CMyButton	m_SetupBtnMode;
	CMyButton	m_SetupBtnLeft;
	CMyCheck	m_SetupChkNameopen;
	CMyCheck	m_SetupChkKoShow;
	CMyButton	m_SetupBtnOther;
	CMyButton	m_SetupBtnCloses;
	CMyButton	m_SetupBtnHotkey;
	CMyButton	m_SetupBtnWinb;
	CMyButton	m_SetupBtnFullb;
	CMySlider	m_SetupSliMusic;
	CMySlider	m_SetupSliEffect;
	CMyButton	m_SetupBtnWin;
	CMyButton	m_SetupBtnFull;
	CMyButton		m_SetupBtnExit ;
	CMyButton		m_SetupBtnDelete ;
	CMyButton	m_SetupBtnReturn;
	CMyButton	m_SetupBtnReal;
	CMyButton	m_SetupBtnSimple;
	CMyButton	m_SetupBtnShadowclose;
	CMyButton	m_SetupBtnHelp;
	CMyStatic	m_SetupPic1;
	CMyStatic	m_SetupPic2;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void SetScreenMode(BYTE btMode);
	void ResetInterface();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSetupBtnReturn();
	afx_msg void OnSetupBtnExit();
	afx_msg void OnSetupBtnWin();
	afx_msg void OnSetupBtnFull();
	afx_msg void OnSetupBtnDelete();
	afx_msg void OnSetupBtnReal();
	afx_msg void OnSetupBtnSimple();
	afx_msg void OnSetupBtnShadowclose();
	afx_msg void OnSetupBtnWinb();
	afx_msg void OnSetupBtnFullb();
	afx_msg void OnBtnHelp();
	afx_msg void OnSetupBtnHotkey();
	afx_msg void OnSetupBtnCloses();
	afx_msg void OnSetupBtnOther();
	afx_msg void OnSetupChkNameopen();
	afx_msg void OnSetupChkKoshow();
	afx_msg void OnSetupBtnMode();
	afx_msg void OnSetupBtnLeft();
	afx_msg void OnSetupBtnRight();
	DECLARE_MESSAGE_MAP()

	friend struct DlgSetupProbe ;
};

#endif
