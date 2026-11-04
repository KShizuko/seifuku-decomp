#if !defined(AFX_DLGFRDSTATE_H__3C1A823C_9F04_4FF2_86F1_CAB3E058B08F__INCLUDED_)
#define AFX_DLGFRDSTATE_H__3C1A823C_9F04_4FF2_86F1_CAB3E058B08F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyImage.h"
#include "MyStatic.h"
#include "MyButton.h"

class CDlgFrdState : public CDialog
{
public:
	CDlgFrdState(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
public:
	void SetFriendNull( char* strName );
	void DeleteFriend();
	void SetFriendState( char* strFriendName );
	void Show();
	enum { IDD = IDD_DIALOG_FRDSTATE };
	CMyImage	m_FrdStateImaface;
	CMyStatic	m_Sta6;
	CMyStatic	m_Sta5;
	CMyStatic	m_Sta1;
	CMyStatic	m_Sta2;
	CMyStatic	m_Sta3;
	CMyStatic	m_Sta4;
	CMyStatic	m_Sta7;
	CMyButton	m_FrdStateBtnDel;
	CMyButton	m_FrdStateBtnClose;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnFrdstateBtnClose();
	afx_msg void OnFrdstateBtnDel();
	afx_msg void OnBtnHelp();
	DECLARE_MESSAGE_MAP()

	friend struct DlgFrdStateProbe ;
};

#endif
