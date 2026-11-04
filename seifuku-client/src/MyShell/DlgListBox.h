#if !defined(AFX_DLGLISTBOX_H__A923459B_E192_4E5E_AEB7_A4589E1E8679__INCLUDED_)
#define AFX_DLGLISTBOX_H__A923459B_E192_4E5E_AEB7_A4589E1E8679__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyList.h"

class CDlgListBox : public CDialog
{
public:
	void Show ();
	CDlgListBox(CWnd* pParent = NULL);
	BOOL		m_bShow ;
private:
	HBRUSH		m_Hbr ;
	BOOL		m_bLoad ;
	CPoint		m_Pnt ;
public:
	enum { IDD = IDD_DIALOG_LISTBOX };
	CMyList			m_ListBox ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	DECLARE_MESSAGE_MAP()

	friend struct DlgListBoxProbe ;
};

#endif
