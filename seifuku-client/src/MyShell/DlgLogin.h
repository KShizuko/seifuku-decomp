#if !defined(AFX_DLGLOGIN_H__CFBF876A_07F8_4B55_AF0F_F3ABB3ABDD4A__INCLUDED_)
#define AFX_DLGLOGIN_H__CFBF876A_07F8_4B55_AF0F_F3ABB3ABDD4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyEdit.h"
#include "MyStatic.h"
#include "MyImage.h"
#include "ServerStatus.h"
#include "ServerAdv.h"

#include "GameInput.h"

class CDlgLogin : public CDialog
{
public:
	void Show();
	CDlgLogin(CWnd* pParent = NULL);
	void SetAccount ( const char* pszAccount, const char* pszPassword ) ;
	void SetPassword ( const char* pszPassword ) ;
	BOOL		m_bShow ;
private:
	HBRUSH		m_Hbr ;
	HBRUSH		m_HbrEdit ;
	BOOL		m_bLoad ;
	CPoint		m_Pnt ;
	IGameInputStr* m_pGameInputStr ;
	UINT		m_uCurrentEdt ;
	BOOL		m_b1024 ;
	char		m_strBkName[16] ;
	char		m_strGroup[16] ;
	char		m_strServer[16] ;
	char		m_szPassword[32] ;
	int			m_nCurrentFocus ;
	int			m_nCurGroup ;
	int			m_nGroupBase ;
	int			m_nGroupAmount ;
	CString		m_strServerDat ;
	CServerStatus	m_SerSta ;
	CServerAdv		m_ServerAdv ;
	vector<ServerAdvInfo>	m_setAdv ;

public:
	void InsertServer( int nIndex, char* strPicID, char* strText, int nSerSta );
	void ReInit();
	enum { IDD = IDD_DIALOG_LOGIN };
	CMyStatic	m_StaDesc;
	CMyStatic	m_StaServer1;
	CMyStatic	m_StaServer2;
	CMyStatic	m_StaServer3;
	CMyStatic	m_StaServer4;
	CMyStatic	m_StaServer5;
	CMyStatic	m_StaServer6;
	CMyStatic	m_StaServer7;
	CMyStatic	m_StaServer8;
	CMyStatic	m_StaServer9;
	CMyStatic	m_StaServer10;
	CMyStatic	m_StaServer11;
	CMyStatic	m_StaServer12;
	CMyImage	m_LoginRoleImg;
	CMyImage	m_LoginEdtImg;
	CMyButton	m_LoginAddBtn;
	CMyEdit		m_LoginAccountEdt;
	CMyEdit		m_LoginPasswordEdt;
	CMyButton	m_LoginOkBtn ;
	CMyButton	m_LoginExitBtn ;
	CMyButton	m_LoginNewBtn ;
	CMyButton	m_LoginNetBtn ;
	CMyButton	m_LoginRecordBtn ;
	CMyButton	m_LoginHelpBtn ;
	CMyImage	m_ImgGroup1;
	CMyImage	m_ImgGroup2;
	CMyImage	m_ImgGroup3;
	CMyImage	m_ImgGroup4;
	CMyImage	m_ImgServer1;
	CMyImage	m_ImgServer2;
	CMyImage	m_ImgServer3;
	CMyImage	m_ImgServer4;
	CMyImage	m_ImgServer5;
	CMyImage	m_ImgServer6;
	CMyImage	m_ImgServer7;
	CMyImage	m_ImgServer8;
	CMyImage	m_ImgServer9;
	CMyImage	m_ImgServer10;
	CMyImage	m_ImgServer11;
	CMyImage	m_ImgServer12;
	CMyButton	m_BtnLeftPage ;
	CMyButton	m_BtnRightPage ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	void ResetServerStatus();
	void LoginGroup( int nIndex );
	void SetGroupPage();
	void SetServerAdv();
	void ShowAdv();
	void SetServerDesc();

	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLoginBtnOk();
	afx_msg void OnLoginBtnExit();
	afx_msg void OnLoginBtnNew();
	afx_msg void OnBtnHelp();
	afx_msg void OnLoginBtnRecord();
	afx_msg void OnLoginBtnNet();
	afx_msg void OnSetfocusLoginEdtAccount();
	afx_msg void OnSetfocusLoginEdtPassword();
	afx_msg void OnLoginGroup1();
	afx_msg void OnLoginServer1();
	afx_msg void OnLoginServer2();
	afx_msg void OnLoginServer3();
	afx_msg void OnLoginBtnAdd();
	afx_msg void OnLoginGroup2();
	afx_msg void OnLoginGroup3();
	afx_msg void OnLoginGroup4();
	afx_msg void OnLoginServer4();
	afx_msg void OnLoginServer5();
	afx_msg void OnLoginServer6();
	afx_msg void OnLoginServer7();
	afx_msg void OnLoginServer8();
	afx_msg void OnLoginServer9();
	afx_msg void OnLoginServer10();
	afx_msg void OnLoginServer11();
	afx_msg void OnLoginServer12();
	afx_msg void OnBtnLeftpage();
	afx_msg void OnBtnRightpage();
	DECLARE_MESSAGE_MAP()

	friend struct DlgLoginProbe ;
};

#endif
