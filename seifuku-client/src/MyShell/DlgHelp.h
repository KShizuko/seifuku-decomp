#if !defined(AFX_DLGHELP_H__87DF2D19_8209_4C10_BFFB_AF6ACACDE440__INCLUDED_)
#define AFX_DLGHELP_H__87DF2D19_8209_4C10_BFFB_AF6ACACDE440__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "WebBrowser.h"

class CDlgHelp : public CDialog
{
public:
	void ResetBk ( int nMode );
	void OpenHelpDlg( char* strFileName );
	CDlgHelp(CWnd* pParent = NULL);
	CWebBrowser*	m_pWeb ;
	HBRUSH			m_Hbr ;
	HBITMAP			m_hBmpBk ;
	HBITMAP			m_hBmpClose ;
	HBITMAP			m_hBmpMore ;
	enum { IDD = IDD_DIALOG_HELP };
	CButton	m_HelpMore;
	CButton	m_HelpClose;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:

	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnButtonClose();
	afx_msg void OnHelpMore();
	DECLARE_MESSAGE_MAP()
};

#endif
