#if !defined(AFX_DLGTALKACT_H__3EEB533D_3E4F_428C_9F15_536BF845EECF__INCLUDED_)
#define AFX_DLGTALKACT_H__3EEB533D_3E4F_428C_9F15_536BF845EECF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
const int ACTION_CHOOSE = 5006 ;

class CDlgTalkAct : public CDialog
{
public:
	void SetParentCWnd ( CWnd* ParentCWnd );
	void Show();
	CDlgTalkAct(CWnd* pParent = NULL);
	BOOL		m_bShow ;
private:
	HBRUSH		m_Hbr ;
	CPoint		m_Pnt ;
	int			m_nSpace ;
	int			m_nCurIndex ;
	int			m_nCurAmount ;
	CWnd*		m_pParentCWnd ;
	enum { IDD = IDD_DIALOG_TALKACT };
	CMyButton	m_TalkActBtnPageUp;
	CMyButton	m_TalkActBtnPageDown;
	CMyButton	m_TalkActBtnHelp;
	CMyButton	m_TalkActBtnCloseB;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTalkActBtnHelp();
	afx_msg void OnTalkActBtnCloseB();
	afx_msg void OnTalkActBtnPageUp();
	afx_msg void OnTalkActBtnPageDown();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

	friend struct DlgTalkActProbe ;
};

#endif
