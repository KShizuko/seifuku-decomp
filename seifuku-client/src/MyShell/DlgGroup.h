#if !defined(AFX_DLGGROUP_H__11D93147_8947_47E5_B993_3A2FE54D533E__INCLUDED_)
#define AFX_DLGGROUP_H__11D93147_8947_47E5_B993_3A2FE54D533E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

const	int	TEAM_MODE_NONE		=	0 ;
const	int	TEAM_MODE_LEADER	=	1 ;
const	int	TEAM_MODE_MEMBER	=	2 ;

#include "MyImage.h"
#include "MyButton.h"
#include "MenuGroup.h"

class CDlgGroup : public CDialog
{
public:
	void SetGroupContain();
	OBJID GetGroupImgID(int nIndex) {return m_PlayerID[nIndex];}
	void Show ();
	CDlgGroup(CWnd* pParent = NULL);
	BOOL		m_bShow ;
	UINT		m_uTeamMode ;
private:
	HBRUSH		m_Hbr ;
	BOOL		m_bInitSuc ;
	CPoint		m_Pnt ;
	OBJID		m_PlayerID[5] ;
	UINT		m_uKickPlayer ;
	UINT		m_uFocusPlayer ;
public:
	CMenuGroup  m_MenuGroup ;
	enum { IDD = IDD_DIALOG_GROUP };
	CMyButton	m_GroupBtn3;
	CMyImage		m_GroupImg1 ;
	CMyImage		m_GroupImg2 ;
	CMyImage		m_GroupImg3 ;
	CMyImage		m_GroupImg4 ;
	CMyImage		m_GroupImg5 ;
	CMyButton		m_GroupBtn1 ;
	CMyButton		m_GroupBtn2 ;
	CMyButton		m_GroupBtn4 ;
	CMyButton		m_GroupCloseBtn ;
	CMyButton		m_BtnHelp ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void SetFocusPlayer(int nIndex);

	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnGroupBtnClose();
	afx_msg void OnGroupBtn1();
	afx_msg void OnGroupBtn2();
	afx_msg void OnGroupChk3();
	afx_msg void OnGroupBtn4();
	afx_msg void OnGroupImg1();
	afx_msg void OnGroupImg2();
	afx_msg void OnGroupImg3();
	afx_msg void OnGroupImg4();
	afx_msg void OnGroupImg5();
	afx_msg void OnBtnHelp();
	DECLARE_MESSAGE_MAP()

	friend struct DlgGroupProbe ;
};

#endif
