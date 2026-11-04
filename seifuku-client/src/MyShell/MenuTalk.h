#if !defined(AFX_MENUTALK_H__3F17A627_804B_44B0_AEDB_E3CDADB762A1__INCLUDED_)
#define AFX_MENUTALK_H__3F17A627_804B_44B0_AEDB_E3CDADB762A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
const int MENU_TALK_CLICK = 5000 ;
const int G_CLICK	=	MENU_TALK_CLICK + 1 ;
const int S_CLICK	=	MENU_TALK_CLICK + 2 ;
const int D_CLICK	=	MENU_TALK_CLICK + 3 ;
const int B_CLICK	=	MENU_TALK_CLICK + 4 ;
const int H_CLICK	=	MENU_TALK_CLICK + 5 ;
const int GM_CLICK	=	MENU_TALK_CLICK + 6 ;

class CMenuTalk : public CDialog
{
public:
	void SetParentWnd( CWnd* pParentWnd );
	void Show();
	CMenuTalk(CWnd* pParent = NULL);

	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	CWnd*				m_ParentWnd ;
	enum { IDD = IDD_MENU_TALK };
	CMyButton	m_TalkBtnGM ;
	CMyButton	m_TalkBtnG ;
	CMyButton	m_TalkBtnS ;
	CMyButton	m_TalkBtnD ;
	CMyButton	m_TalkBtnB ;
	CMyButton	m_TalkBtnH ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTalkBtnG();
	afx_msg void OnTalkBtnS();
	afx_msg void OnTalkBtnD();
	afx_msg void OnTalkBtnB();
	afx_msg void OnTalkBtnH();
	afx_msg void OnTalkBtnGM();
	DECLARE_MESSAGE_MAP()

	friend struct MenuTalkProbe ;
};

#endif
