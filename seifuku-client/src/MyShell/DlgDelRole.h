#if !defined(AFX_DLGDELROLE_H__5F2A1C0B_4294_4D0E_9B31_0405BF000283__INCLUDED_)
#define AFX_DLGDELROLE_H__5F2A1C0B_4294_4D0E_9B31_0405BF000283__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class CDlgDelRole : public CDialog
{
public:
	CDlgDelRole(CWnd* pParent = NULL);
	CString	m_strCode ;

	enum { IDD = IDD_DIALOG_DELROLE };
	CStatic	m_StaText ;
	CStatic	m_StaTitle ;
	CEdit	m_Edt ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:

	virtual BOOL OnInitDialog();
	afx_msg void OnDelroleBtnCancel();
	afx_msg void OnDelroleBtnOk();
	DECLARE_MESSAGE_MAP()
};

#endif
