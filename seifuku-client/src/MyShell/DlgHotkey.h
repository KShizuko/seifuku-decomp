#if !defined(AFX_DLGHOTKEY_H__SEIFUKU_231__INCLUDED_)
#define AFX_DLGHOTKEY_H__SEIFUKU_231__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyList.h"
#include "MyButton.h"
#include "MyStatic.h"

const int	HOTKEY_PARENT_AMOUNT = 6 ;
const int	HOTKEY_CHILD_AMOUNT = 10 ;
extern int	g_nHotkeyValue[HOTKEY_PARENT_AMOUNT * HOTKEY_CHILD_AMOUNT] ;

class CDlgHotkey : public CDialog
{
public:
	void Show();
	void SetChildList ( int nParent ) ;
	CDlgHotkey(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	HBRUSH				m_HbrBlack ;
	CPoint				m_Pnt ;
	WORD				m_wVirtualKey ;
	WORD				m_wModifiers ;
	char				m_szHotKey[64] ;
	enum { IDD = IDD_DIALOG_HOTKEY };
	CMyButton	m_BtnDel;
	CHotKeyCtrl	m_HotKey;
	CMyStatic	m_StaOld;
	CMyList		m_LstParent;
	CMyList		m_LstChild;
	CMyButton	m_BtnCloseB;
	CMyButton	m_BtnClose;
	CMyButton	m_BtnRevert;
	CMyButton	m_BtnChange;
	CMyButton	m_BtnHelp;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnHotkeyClose();
	afx_msg void OnHotkeyCloseb();
	afx_msg void OnHotkeyBtnChange();
	afx_msg void OnHotkeyBtnSet();
	afx_msg void OnHotkeyBtnRevert();
	afx_msg void OnBtnHelp();
	afx_msg void OnHotkeyBtnDel();
	DECLARE_MESSAGE_MAP()

	friend struct DlgHotkeyProbe ;
	friend class CMyShellDlg ;
};

#endif
