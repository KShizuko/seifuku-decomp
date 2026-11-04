#if !defined(AFX_DLGNPCBOOTH_H__363C2C8D_01F3_4E85_AE19_AA22476C951D__INCLUDED_)
#define AFX_DLGNPCBOOTH_H__363C2C8D_01F3_4E85_AE19_AA22476C951D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyImage.h"
#include "MyStatic.h"

class CDlgNpcBooth : public CDialog
{
public:
	int GetMouseMoveIndex();
	void ClearNpcBooth();
	void SetNpcBooth ();
	void Show();
	CDlgNpcBooth(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	void SetMoneyTypeID ( int nIndex, const char* pszID ) ;
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	int 				m_nPntX ;
	int					m_nCurBase ;

	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;
	enum { IDD = IDD_DIALOG_NPCBOOTH };
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
	CMyImage	m_NpcBoothImaGood1;
	CMyImage	m_NpcBoothImaGood2;
	CMyImage	m_NpcBoothImaGood3;
	CMyImage	m_NpcBoothImaGood4;
	CMyImage	m_NpcBoothImaGood5;
	CMyImage	m_NpcBoothImaGood6;
	CMyButton	m_NpcBoothBtnCloseB;
	CMyButton	m_NpcBoothBtnClose;
	CMyButton	m_NpcBoothBtnUp;
	CMyButton	m_NpcBoothBtnDown;
	CMyButton	m_BtnBuy1;
	CMyButton	m_BtnBuy2;
	CMyButton	m_BtnBuy3;
	CMyButton	m_BtnBuy4;
	CMyButton	m_BtnBuy5;
	CMyButton	m_BtnBuy6;
	CMyButton	m_BtnHelp;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnNpcboothBtnClose();
	afx_msg void OnNpcboothBtnCloseb();
	afx_msg void OnBtnHelp();
	afx_msg void OnNpcboothBtnUp();
	afx_msg void OnNpcboothBtnDown();
	afx_msg void OnNpcboothBtnBuy1();
	afx_msg void OnNpcboothBtnBuy2();
	afx_msg void OnNpcboothBtnBuy3();
	afx_msg void OnNpcboothBtnBuy4();
	afx_msg void OnNpcboothBtnBuy5();
	afx_msg void OnNpcboothBtnBuy6();
	DECLARE_MESSAGE_MAP()

	friend struct DlgNpcBoothProbe ;
};

#endif
