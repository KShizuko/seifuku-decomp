#if !defined(AFX_DLGQUERY_ROLE_H__7F2EE870_574F_4B36_890C_111149EC92EF__INCLUDED_)
#define AFX_DLGQUERY_ROLE_H__7F2EE870_574F_4B36_890C_111149EC92EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyStatic.h"
#include "MyButton.h"

class CDlgQuery_Role : public CDialog
{
public:
	void Show ( int xPos, int yPos );
	CDlgQuery_Role(CWnd* pParent = NULL);
	void SetRoleInfo () ;

private:
	void	AllotPoint () ;
	HBRUSH	m_Hbr ;
	BOOL	m_bInitSuc ;
	CPoint	m_DlgPnt ;
	int		m_nShowType ;
public:
	enum { IDD = IDD_DIALOG_QUERY_ROLE };
	CMyButton	m_BtnMore;
	CMyButton	m_BtnBase;
	CMyStatic	m_StaName;
	CMyStatic	m_StaLev;
	CMyStatic	m_StaFamor;
	CMyStatic	m_StaPk;
	CMyStatic	m_StaFere;
	CMyStatic	m_StaSyn;
	CMyStatic	m_StaTitle;
	CMyStatic	m_StaStrong;
	CMyStatic	m_StaSmart;
	CMyStatic	m_StaHa;
	CMyStatic	m_StaEnergy;
	CMyStatic	m_StaPotential;
	CMyStatic	m_StaUnused;
	CMyStatic	m_StaAtk;
	CMyStatic	m_StaDef;
	CMyStatic	m_StaMagicAtk;
	CMyStatic	m_StaMagicDef;
	CMyStatic	m_Sta18;
	CMyStatic	m_Sta19;
	CMyButton	m_BtnAddStrong;
	CMyButton	m_BtnAddSmart;
	CMyButton	m_BtnAddHa;
	CMyButton	m_BtnAddEnergy;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnRoleBtn8();
	afx_msg void OnRoleBtn9();
	afx_msg void OnRoleBtn10();
	afx_msg void OnRoleBtn11();
	afx_msg void OnRoleBtnBase();
	afx_msg void OnRoleBtnMore();
	DECLARE_MESSAGE_MAP()

	friend struct DlgQuery_RoleProbe ;
};

#endif
