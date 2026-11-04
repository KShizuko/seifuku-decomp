#if !defined(AFX_BKWND_H__2FBC627D_2672_4E56_B8D3_9F6CA01AAD5A__INCLUDED_)
#define AFX_BKWND_H__2FBC627D_2672_4E56_B8D3_9F6CA01AAD5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyShellDlg.h"

class CBkWnd : public CWnd
{
public:
	CBkWnd();
	CMyShellDlg		m_MyShellDlg ;
public:
public:

public:
	void Create();
	virtual ~CBkWnd();

protected:
	HICON m_hIcon;

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()

	friend struct BkWndProbe ;
};

#endif
