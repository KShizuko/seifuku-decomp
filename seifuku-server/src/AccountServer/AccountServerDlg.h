#if !defined(AFX_ACCOUNTSERVERDLG_H__1C382F39_7153_496B_9F90_3CF520BB02F8__INCLUDED_)
#define AFX_ACCOUNTSERVERDLG_H__1C382F39_7153_496B_9F90_3CF520BB02F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class CAccountServerDlg : public CDialog
{
public:
	CAccountServerDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_ACCOUNTSERVER_DIALOG };
	CString	m_sText;
	CString	m_sState2;
	CString	m_sState;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	long m_nLoginAcceptP5;
	long m_nPointFeeP5;
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnCloseButtom();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
};

#endif
