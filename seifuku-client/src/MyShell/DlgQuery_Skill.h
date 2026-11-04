#if !defined(AFX_DLGQUERY_SKILL_H__SEIFUKU_163__INCLUDED_)
#define AFX_DLGQUERY_SKILL_H__SEIFUKU_163__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyImage.h"
#include "Magic.h"

class CDlgQuery_Skill : public CDialog
{
public:
	void Show ( int xPos, int yPos );
	void ShowSkill ();
	void FlashSkillExp ();
	void SetSkill ( int nIndex, int nType, DWORD dwLevel );
	void ClearSkill ();
	void Reset ();
	CDlgQuery_Skill(CWnd* pParent = NULL);
private:
	HBRUSH			m_Hbr ;
	CPoint			m_DlgPnt ;
	int				m_nSkillClass ;
	CMagic*			m_pCurSkill ;
	BOOL			m_bShowInfo ;
	int				m_nTopIndex ;

public:
	enum { IDD = IDD_DIALOG_QUERY_SKILL };
	CMyButton	m_BtnZ;
	CMyButton	m_BtnM;
	CMyButton	m_BtnJ;
	CMyButton	m_BtnUp;
	CMyButton	m_BtnDown;
	CMyImage	m_Ima1;
	CMyImage	m_Ima2;
	CMyImage	m_Ima3;
	CMyImage	m_Ima4;
	CMyImage	m_Ima5;
	CMyImage	m_Ima6;
	CMyImage	m_Ima11;
	CMyImage	m_Ima22;
	CMyImage	m_Ima33;
	CMyImage	m_Ima44;
	CMyImage	m_Ima55;
	CMyImage	m_Ima66;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSkillBtnZ();
	afx_msg void OnSkillBtnM();
	afx_msg void OnSkillBtnJ();
	afx_msg void OnSkillIma1();
	afx_msg void OnSkillIma11();
	afx_msg void OnSkillIma2();
	afx_msg void OnSkillIma22();
	afx_msg void OnSkillIma3();
	afx_msg void OnSkillIma33();
	afx_msg void OnSkillIma4();
	afx_msg void OnSkillIma44();
	afx_msg void OnSkillIma5();
	afx_msg void OnSkillIma55();
	afx_msg void OnSkillIma6();
	afx_msg void OnSkillIma66();
	afx_msg void OnSkillBtnUp();
	afx_msg void OnSkillBtnDown();
	DECLARE_MESSAGE_MAP()

	friend struct DlgQuery_SkillProbe ;
};

#endif
