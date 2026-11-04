#if !defined(AFX_DLGQUERY_H__F2781CE1_7667_4C8A_A2A8_55B1A147C7B5__INCLUDED_)
#define AFX_DLGQUERY_H__F2781CE1_7667_4C8A_A2A8_55B1A147C7B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyCheck.h"
#include "MyButton.h"
#include "DlgQuery_Role.h"
#include "DlgQuery_Weapon.h"
#include "DlgQuery_Mission.h"
#include "DlgQuery_Skill.h"

class CDlgQuery : public CDialog
{
public:
	void PopupCurChk ( BYTE btCurDlgID );
	void ShowChildDlg ( );
	void HideCurDlg ( BYTE btCurDlgID );
	void ShowCurDlg ( BYTE btCurDlgID );
	void Show();
	CDlgQuery(CWnd* pParent = NULL);
	BOOL		m_bShow ;
private:
	HBRUSH				m_Hbr ;
public:
	BOOL				m_bLoad ;
	int 				m_nPntX ;
	CPoint				m_Pnt ;
	int					m_Height ;
	int					m_Width ;
	CDlgQuery_Role		m_DlgRole;
	CDlgQuery_Weapon	m_DlgWeapon;
	CDlgQuery_Mission	m_DlgMission;
	CDlgQuery_Skill		m_DlgSkill;
	BYTE				m_btCurChildDlgID ;
	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;
	enum { IDD = IDD_DIALOG_QUERY };
	CMyCheck	m_ChkSkill;
	CMyButton	m_QueryCloseBBtn ;
	CMyButton	m_QueryCloseBtn ;
	CMyCheck	m_ChkRole;
	CMyCheck	m_ChkWeapon;
	CMyCheck	m_ChkMission;
	CMyButton	m_BtnHelp ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnQueryClose();
	afx_msg void OnQueryCloseb();
	afx_msg void OnChkMission();
	afx_msg void OnChkWeapon();
	afx_msg void OnQueryFaction();
	afx_msg void OnChkRole();
	afx_msg void OnBtnHelp();
	afx_msg void OnChkSkill();
	DECLARE_MESSAGE_MAP()

	friend struct DlgQueryProbe ;
};

#endif
