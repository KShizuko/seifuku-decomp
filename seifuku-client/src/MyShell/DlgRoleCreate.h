#if !defined(AFX_DLGROLECREATE_H__D27F8954_E6C4_407D_9E3D_A4ADF0879630__INCLUDED_)
#define AFX_DLGROLECREATE_H__D27F8954_E6C4_407D_9E3D_A4ADF0879630__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyImage.h"
#include "MyStatic.h"
#include "MyButton.h"
#include "MyEdit.h"
#include "3DSimpleRole.h"
#include "DlgProIntro.h"

class CDlgRoleCreate : public CDialog
{
public:
	void SetAcnAndPsw( const char* strAccount, const char* strPassword );
	void Show();
	void SetRoleType( BYTE btRoleType );
	void ReInit();
	CDlgRoleCreate(CWnd* pParent = NULL);
	BOOL				m_bShow ;
	CDlgProIntro		m_DlgProIntro ;
private:
	HBRUSH				m_Hbr ;
	HBRUSH				m_EdtHbr ;
	BOOL				m_bInitSuc ;
	BYTE				m_btRoleType ;
	char				m_strRoleType[64] ;
	char				m_strRoleName[16] ;
	char				m_strAccount[16] ;
	char				m_strPassword[16] ;
	C3DSimpleRole		m_ShowRoleA ;
	C3DSimpleRole		m_ShowRoleB ;
	BOOL				m_b1024 ;
	int					m_nRoleSelIndex ;

public:
	enum { IDD = IDD_DIALOG_ROLECREATE };
	CMyImage			m_Img2;
	CMyImage			m_Img1;
	CMyButton			m_RoleCreateExitBtn;
	CMyStatic			m_RoleCreateDataSta ;
	CMyEdit				m_RoleCreateNameEdt ;
	CMyButton			m_RoleCreateOkBtn ;
	CMyButton			m_RoleCreateReturnBtn ;
	CMyButton			m_HelpBtn ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnRolecreateBtnOk();
	afx_msg void OnRolecreateBtnExit();
	afx_msg void OnRolechooseMimg1();
	afx_msg void OnRolechooseMimg2();
	afx_msg void OnRolecreateBtnReturn();
	afx_msg void OnBtnHelp();
	DECLARE_MESSAGE_MAP()

	friend struct DlgRoleCreateProbe ;
};

#endif
