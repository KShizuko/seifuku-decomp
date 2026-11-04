#if !defined(AFX_DLGROLECHOOSE_H__F710F48E_D755_4AAC_A795_DA4F0F491E51__INCLUDED_)
#define AFX_DLGROLECHOOSE_H__F710F48E_D755_4AAC_A795_DA4F0F491E51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyImage.h"
#include "MyStatic.h"
#include "MyButton.h"
#include "3DSimpleRole.h"

class CDlgRoleChoose : public CDialog
{
public:
	void CreateRole ( BYTE btRoleType );
	void Show();
	CDlgRoleChoose(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	BOOL				m_bLoad ;
	BYTE				m_btRoleType ;
	C3DSimpleRole		m_ShowRole ;
	BOOL				m_b1024 ;

public:
	void ReInit();
	enum { IDD = IDD_DIALOG_ROLECHOOSE };
	CMyImage		m_RoleChooseMImg1 ;
	CMyImage		m_RoleChooseMImg2 ;
	CMyImage		m_RoleChooseMImg3 ;
	CMyImage		m_RoleChooseMImg4 ;
	CMyImage		m_RoleChooseWImg1 ;
	CMyImage		m_RoleChooseWImg2 ;
	CMyImage		m_RoleChooseWImg3 ;
	CMyImage		m_RoleChooseWImg4 ;
	CMyStatic		m_RoleChooseDataSta ;
	CMyButton		m_RoleChooseReturnBtn ;
	CMyButton		m_RoleChooseOkBtn ;
	CMyButton		m_HelpBtn ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnRolechooseBtnReturn();
	afx_msg void OnBtnHelp();
	afx_msg void OnRolechooseMimg1();
	afx_msg void OnRolechooseMimg2();
	afx_msg void OnRolechooseMimg3();
	afx_msg void OnRolechooseMimg4();
	afx_msg void OnRolechooseWimg1();
	afx_msg void OnRolechooseWimg2();
	afx_msg void OnRolechooseWimg3();
	afx_msg void OnRolechooseWimg4();
	afx_msg void OnRolechooseBtnOk();
	DECLARE_MESSAGE_MAP()

	friend struct DlgRoleChooseProbe ;
};

#endif
