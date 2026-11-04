#if !defined(AFX_DLGQUERY_WEAPON_H__SEIFUKU_134__INCLUDED_)
#define AFX_DLGQUERY_WEAPON_H__SEIFUKU_134__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyImage.h"
#include "WeaponSkill.h"

class CDlgQuery_Weapon : public CDialog
{
public:
	void Show ( int xPos, int yPos );
	void ShowSkill ();
	void SetSkill ( int nIndex, int nType, DWORD dwLevel );
	void ClearSkill ();
	void Reset ();
	CDlgQuery_Weapon(CWnd* pParent = NULL);
private:
	HBRUSH			m_Hbr ;
	CPoint			m_DlgPnt ;
	int				m_nWeaponClass ;
	CWeaponSkill*	m_pCurSkill ;
	BOOL			m_bShowInfo ;
	int				m_nTopIndex ;

public:
	enum { IDD = IDD_DIALOG_QUERY_WEAPON };
	CMyButton	m_BtnUp;
	CMyButton	m_BtnDown;
	CMyButton	m_BtnS;
	CMyButton	m_BtnD;
	CMyButton	m_BtnO;
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
	afx_msg void OnWeaponBtnS();
	afx_msg void OnWeaponBtnD();
	afx_msg void OnWeaponBtnO();
	afx_msg void OnWeaponIma1();
	afx_msg void OnWeaponIma11();
	afx_msg void OnWeaponIma2();
	afx_msg void OnWeaponIma22();
	afx_msg void OnWeaponIma3();
	afx_msg void OnWeaponIma33();
	afx_msg void OnWeaponIma4();
	afx_msg void OnWeaponIma44();
	afx_msg void OnWeaponIma5();
	afx_msg void OnWeaponIma55();
	afx_msg void OnWeaponIma6();
	afx_msg void OnWeaponIma66();
	afx_msg void OnWeaponBtnUp();
	afx_msg void OnWeaponBtnDown();
	DECLARE_MESSAGE_MAP()

	friend struct DlgQuery_WeaponProbe ;
};

#endif
