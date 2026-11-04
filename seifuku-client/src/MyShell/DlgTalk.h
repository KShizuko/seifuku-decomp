#if !defined(AFX_DLGTALK_H__7B85476C_7BD6_463E_A558_EEE57C74B29A__INCLUDED_)
#define AFX_DLGTALK_H__7B85476C_7BD6_463E_A558_EEE57C74B29A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyCombo.h"
#include "MyEdit.h"
#include "MyImage.h"
#include "MyButton.h"
#include "MenuTalk.h"
#include "DlgListBox.h"
#include "DlgTalkAct.h"

#include "Player.h"

const int _MAX_WORDS_PS = 2 ;
class CDlgTalk : public CDialog
{
public:
	void SetTalkTarget( CPlayer* pTalkTarget );
	void SetTalkMode( UINT uTalkMode);
	void Show();
	CDlgTalk(CWnd* pParent = NULL);

	BOOL				m_bShow ;
	UINT				m_uTalkMode ;
	CDlgListBox			m_DlgListBox;
	CDlgTalkAct			m_DlgTalkAct;
	char				m_strTargetName[5][16] ;
	UINT				m_uTargetTalkMode[5] ;
	int					m_nTargetAmount ;
	int					m_nCurTargetIndex ;
private:
	HBRUSH				m_Hbr ;
	HBRUSH				m_HbrEdit ;
	BOOL				m_bInitSuc ;
	CBitmap				m_Bitmap ;
	CPoint				m_Pnt ;
	CListBox			m_ListBox ;
	DWORD				m_dwLastTime ;
	int					m_nListAmount ;
	int					m_nListIndex ;
public:
	CMenuTalk			m_MenuTalk ;
private:
	OBJID				m_TalkPlayerID ;
	UINT				m_uAddBtnMode ;
	UINT				m_uDelIndex ;
public:
	BOOL				m_bShowActionBtn ;
private:
	DWORD				m_dwLastGmTime ;
	CRect				m_rect ;
public:
	void DeleteTalkTarget();
	enum { IDD = IDD_DIALOG_TALK };
	CMyCombo	m_TalkDataCob ;
	CMyEdit		m_TalkNameEdt ;
	CMyImage	m_TalkModeImg ;
	CMyButton	m_TalkSendBtn ;
	CMyButton	m_TalkActionBtn ;
	CMyButton	m_TalkClearBtn ;
	CMyButton	m_TalkAddBtn ;
	CMyButton	m_TalkListBtn ;
	CMyButton	m_TalkLogBtn ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void CycleTalkChannel( int nDir );
	void MoveWindow ( int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE ) ;
	void CycleTalkTarget( int nDir );
	void SetTalkTargetList();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTalkBtnSend();
	afx_msg void OnTalkBtnClear();
	afx_msg void OnTalkImgMode();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdateTalkEdtName();
	afx_msg void OnTalkBtnList();
	afx_msg void OnTalkBtnAdd();
	afx_msg void OnTalkBtnLog();
	afx_msg void OnTalkBtnAction();
	DECLARE_MESSAGE_MAP()

	friend struct DlgTalkProbe ;
};

#endif
