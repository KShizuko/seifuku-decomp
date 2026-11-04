#if !defined(AFX_DLGMSGSEND_H__E0DC562A_93D9_42F1_86A7_1922BA5B68A9__INCLUDED_)
#define AFX_DLGMSGSEND_H__E0DC562A_93D9_42F1_86A7_1922BA5B68A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyStatic.h"
#include "MyEdit.h"

class CDlgMsgSend : public CDialog
{
public:
	void Show ();
	CDlgMsgSend(CWnd* pParent = NULL);
	BOOL	m_bShow ;
private:
	HBRUSH				m_Hbr ;
	HBRUSH				m_HbrBlack ;
	CPoint				m_Pnt ;
	OBJID				m_FriendID ;
public:
	void SetTargetFriend( const char* strName );
	enum { IDD = IDD_DIALOG_MSGSEND };
	CMyEdit		m_MsgSendData;
	CMyButton	m_MsgSendSend;
	CMyStatic	m_MsgSendName;
	CMyButton	m_MsgSendClose;
	CMyButton	m_MsgSendCloses;
	CMyButton	m_MsgSendAllMsg;
	CMyButton	m_BtnHelp;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnBtnHelp();
	afx_msg void OnMsgsendCloseb();
	afx_msg void OnMsgsendSend();
	afx_msg void OnMsgsendAllmsg();
	DECLARE_MESSAGE_MAP()

	friend struct DlgMsgSendProbe ;
};

#endif
