#if !defined(AFX_DLGMPCBOOTH_H__A803F2DE_2B13_431F_B74D_491D80E2CD71__INCLUDED_)
#define AFX_DLGMPCBOOTH_H__A803F2DE_2B13_431F_B74D_491D80E2CD71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyImage.h"
#include "MyStatic.h"

class CDlgMpcBooth : public CDialog
{
public:
	int GetMouseMoveIndex();
	void ClearMpcBooteh();
	void SetMpcBooth();
	void SetPickUpState ( BOOL bPickUp, OBJID nIconRealID);
	void Show();
	CDlgMpcBooth(CWnd* pParent = NULL);
	BOOL				m_bShow ;
	OBJID				m_nCurRealID ;
	int					m_nCurChangeIndex ;
private:
	void SetMoneyTypeID ( int nIndex, const char* pszID ) ;
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	int 				m_nPntX ;
	BOOL				m_bPickUp ;
	OBJID				m_nIconRealID ;
	int					m_nCurBase ;

	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;
	enum { IDD = IDD_DIALOG_MPCBOOTH };
	CMyStatic	m_StaMoneyType6;
	CMyStatic	m_StaMoneyType5;
	CMyStatic	m_StaMoneyType4;
	CMyStatic	m_StaMoneyType3;
	CMyStatic	m_StaMoneyType2;
	CMyStatic	m_StaMoneyType1;
	CMyStatic	m_StaPrice1;
	CMyStatic	m_StaPrice2;
	CMyStatic	m_StaPrice3;
	CMyStatic	m_StaPrice4;
	CMyStatic	m_StaPrice5;
	CMyStatic	m_StaPrice6;
	CMyImage	m_MpcBoothImaGood1;
	CMyImage	m_MpcBoothImaGood2;
	CMyImage	m_MpcBoothImaGood3;
	CMyImage	m_MpcBoothImaGood4;
	CMyImage	m_MpcBoothImaGood5;
	CMyImage	m_MpcBoothImaGood6;
	CMyButton	m_BtnChange1;
	CMyButton	m_BtnChange2;
	CMyButton	m_BtnChange3;
	CMyButton	m_BtnChange4;
	CMyButton	m_BtnChange5;
	CMyButton	m_BtnChange6;
	CMyButton	m_BtnGetBack1;
	CMyButton	m_BtnGetBack2;
	CMyButton	m_BtnGetBack3;
	CMyButton	m_BtnGetBack4;
	CMyButton	m_BtnGetBack5;
	CMyButton	m_BtnGetBack6;
	CMyButton	m_MpcBoothBtnCloseB;
	CMyButton	m_MpcBoothBtnUp;
	CMyButton	m_MpcBoothBtnHawk;
	CMyButton	m_MpcBoothBtnDown;
	CMyButton	m_MpcBoothBtnCloseS;
	CMyButton	m_BtnHelp;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMpcboothBtnCloses();
	afx_msg void OnMpcboothBtnCloseb();
	afx_msg void OnBtnHelp();
	afx_msg void OnMpcboothBtnUp();
	afx_msg void OnMpcboothBtnDown();
	afx_msg void OnMpcboothBtnHawk();
	afx_msg void OnMpcboothBtnChange1();
	afx_msg void OnMpcboothBtnChange2();
	afx_msg void OnMpcboothBtnChange3();
	afx_msg void OnMpcboothBtnChange4();
	afx_msg void OnMpcboothBtnChange5();
	afx_msg void OnMpcboothBtnChange6();
	afx_msg void OnMpcboothBtnGetback1();
	afx_msg void OnMpcboothBtnGetback2();
	afx_msg void OnMpcboothBtnGetback3();
	afx_msg void OnMpcboothBtnGetback4();
	afx_msg void OnMpcboothBtnGetback5();
	afx_msg void OnMpcboothBtnGetback6();
	afx_msg void OnMpcboothImaGood1();
	afx_msg void OnMpcboothImaGood2();
	afx_msg void OnMpcboothImaGood3();
	afx_msg void OnMpcboothImaGood4();
	afx_msg void OnMpcboothImaGood5();
	afx_msg void OnMpcboothImaGood6();
	DECLARE_MESSAGE_MAP()

	friend struct DlgMpcBoothProbe ;
};

#endif
