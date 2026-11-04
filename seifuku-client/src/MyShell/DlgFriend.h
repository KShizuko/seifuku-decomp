#if !defined(AFX_DLGFRIEND_H__22F52277_0D73_4DFE_8755_92780539974B__INCLUDED_)
#define AFX_DLGFRIEND_H__22F52277_0D73_4DFE_8755_92780539974B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyList.h"
#include "DlgFrdState.h"
#include "Hero.h"

class CDlgFriend : public CDialog
{
public:
	CDlgFriend(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	BOOL				m_bInitSuc ;
	CPoint				m_Pnt ;
	int 				m_nPntX ;
	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;

public:
	CDlgFrdState		m_DlgFrdState ;
	int					m_nCurIndex ;

	void SetFriendState();
	void SetFriendList ();
	void Show ();
	enum { IDD = IDD_DIALOG_FRIEND };
	CMyButton	m_FriendBtnResume;
	CMyButton	m_FriendBtnAdd ;
	CMyButton	m_FriendBtnClose ;
	CMyButton	m_BtnHelp ;
	CMyList		m_FriendLstData ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnFriendBtnClose();
	afx_msg void OnBtnHelp();
	afx_msg void OnFriendBtnAdd();
	afx_msg void OnFriendBtnResume();
	DECLARE_MESSAGE_MAP()

	friend struct DlgFriendProbe ;
};

#endif
