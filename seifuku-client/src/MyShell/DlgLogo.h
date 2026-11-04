#if !defined(AFX_DLGLOGO_H__72D2DD02_CA8F_4BE6_BBF7_5AE4F03CAB56__INCLUDED_)
#define AFX_DLGLOGO_H__72D2DD02_CA8F_4BE6_BBF7_5AE4F03CAB56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class CDlgLogo : public CDialog
{
public:
	CDlgLogo(CWnd* pParent = NULL);
	HBRUSH			m_Hbr ;
	HBITMAP			m_hBmp ;
	enum { IDD = IDD_DIALOG_LOGO };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:

	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
};

#endif
