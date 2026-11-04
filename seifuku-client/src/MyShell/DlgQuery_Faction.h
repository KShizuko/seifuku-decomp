#if !defined(AFX_DLGQUERY_FACTION_H__SFK_4267__INCLUDED_)
#define AFX_DLGQUERY_FACTION_H__SFK_4267__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyStatic.h"
#include "MyEdit.h"
#include "MyList.h"
#include "DlgFacState.h"

class CDlgQuery_Faction : public CDialog
{
public:
	void SetFacState ();
	void SetMemberList ();
	void SetFaction ();
	void Show ();
	CDlgQuery_Faction(CWnd* pParent = NULL);
	BOOL			m_bShow ;
	char			m_strCurName[16] ;
private:
	HBRUSH			m_Hbr ;
	HBRUSH			m_HbrEdit ;
	CPoint			m_Pnt ;
	int				m_nPntX ;
	int				m_nShowType ;
	int				m_nRank ;
	int				m_nCurBase ;

	DWORD			m_dwLastShowTime ;
	float			m_fFrameStep ;
	int				m_nFrameMode ;

	char			m_strMember[800][32] ;
	BOOL			m_bMemberOnline[800] ;
	int				m_nMemberAmount ;
	CDlgFacState	m_DlgFacState ;
	enum { IDD = IDD_DIALOG_QUERY_FACTION };
	CMyButton	m_BtnDel;
	CMyButton	m_BtnRight;
	CMyButton	m_BtnLeft;
	CMyList		m_LstMember;
	CMyButton	m_BtnOk;
	CMyButton	m_BtnCancel;
	CMyEdit		m_EdtWord;
	CMyButton	m_BtnCloseB;
	CMyButton	m_BtnHelp;
	CMyButton	m_BtnState;
	CMyButton	m_BtnFriend;
	CMyButton	m_BtnEnemy;
	CMyButton	m_BtnWord;
	CMyButton	m_BtnClose;
	CMyStatic	m_StaProffer;
	CMyStatic	m_StaTitle;
	CMyStatic	m_StaName;
	CMyStatic	m_StaLeader;
	CMyStatic	m_StaFund;
	CMyStatic	m_StaPopulation;
	CMyStatic	m_StaLine1;
	CMyStatic	m_StaLine2;
	CMyStatic	m_StaLine3;
	CMyStatic	m_StaLine4;
	CMyStatic	m_StaLine5;
	CMyButton	m_BtnJoin;
	CMyButton	m_BtnOut;
	CMyButton	m_BtnSub;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	void SetLine ( int nIndex, const char* strText );
	void SetWord ();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnFactionBtnOut();
	afx_msg void OnFactionBtnInvite();
	afx_msg void OnFactionBtnJoin();
	afx_msg void OnFactionClose();
	afx_msg void OnFactionBtnSub();
	afx_msg void OnFactionBtnWord();
	afx_msg void OnFactionBtnState();
	afx_msg void OnFactionBtnFriend();
	afx_msg void OnFactionBtnEnemy();
	afx_msg void OnBtnHelp();
	afx_msg void OnFactionCloseb();
	afx_msg void OnBtnOk();
	afx_msg void OnBtnCancel();
	afx_msg void OnFactionLeft();
	afx_msg void OnFactionRight();
	afx_msg void OnFactionBtnDel();
	DECLARE_MESSAGE_MAP()

	friend struct DlgQuery_FactionProbe ;
	friend class CMyShellDlg ;
	friend class CDlgMain ;
};

#endif
