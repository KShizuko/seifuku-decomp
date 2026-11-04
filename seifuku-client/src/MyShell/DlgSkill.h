#if !defined(AFX_DLGSKILL_H__SFK_4351_00446110_INCLUDED_)
#define AFX_DLGSKILL_H__SFK_4351_00446110_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyStatic.h"

class CDlgSkill : public CDialog
{
public:
	void Show();
	CDlgSkill(CWnd* pParent = NULL);

public:
	BOOL				m_bShow ;
	int					m_nFrame ;
private:
	HBRUSH				m_Hbr ;
	BOOL				m_bInitSuc ;
	CPoint				m_Pnt ;
	int					m_nHeight ;
	int					m_nWidth ;
	float				m_fFrameStep ;
public:
	enum { IDD = IDD_DIALOG_SKILL };
	CMyButton			m_BtnSkill1 ;
	CMyButton			m_BtnSkill2 ;
	CMyButton			m_BtnSkill3 ;
	CMyButton			m_BtnSkill4 ;
	CMyButton			m_BtnSkill5 ;
	CMyButton			m_BtnSkill6 ;
	CMyButton			m_BtnSkill7 ;
	CMyButton			m_BtnSkill8 ;
	CMyStatic			m_StaLev1 ;
	CMyStatic			m_StaLev2 ;
	CMyStatic			m_StaLev3 ;
	CMyStatic			m_StaLev4 ;
	CMyStatic			m_StaLev5 ;
	CMyStatic			m_StaLev6 ;
	CMyStatic			m_StaLev7 ;
	CMyStatic			m_StaLev8 ;
	CMyButton			m_BtnClose ;
	CMyButton			m_BtnH ;
	CMyButton			m_BtnUp ;
	CMyButton			m_BtnDown ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	void Slide();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSkillBtnClose();
	DECLARE_MESSAGE_MAP()
};

#endif
