#if !defined(AFX_DLGCHAT_H__852F2B8C_AAA9_40F5_8720_688D087302AA__INCLUDED_)
#define AFX_DLGACT_H__852F2B8C_AAA9_40F5_8720_688D087302AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyCheck.h"
#include "MyImage.h"

class CDlgChat : public CDialog
{
public:
	void ChangeColor ( int nColorIndex );
	void Show();
	CDlgChat(CWnd* pParent = NULL);
	BOOL		m_bShow ;
	int			m_nColorLog[7] ;
	DWORD		m_dwColor[8] ;
private:
	HBRUSH		m_Hbr ;
	CPoint		m_Pnt ;
	unsigned short		m_usCurChannel ;
	DWORD		m_dwCurColor ;
	BOOL		m_bColorList ;
public:
	enum { IDD = IDD_DIALOG_CHAT };
	CMyImage	m_ChatImgCc1;
	CMyImage	m_ChatImgCc2;
	CMyImage	m_ChatImgCc3;
	CMyImage	m_ChatImgCc4;
	CMyImage	m_ChatImgCc5;
	CMyImage	m_ChatImgCc6;
	CMyImage	m_ChatImgCc7;
	CMyImage	m_ChatImgCc8;
	CMyImage	m_ChatImgBc;
	CMyImage	m_ChatImgDc;
	CMyImage	m_ChatImgGc;
	CMyImage	m_ChatImgSc;
	CMyImage	m_ChatImgHc;
	CMyImage	m_ChatImgZc;
	CMyImage	m_ChatImgFc;
	CMyCheck	m_ChatChkSF;
	CMyCheck	m_ChatChkS;
	CMyButton	m_ChatBtnCloseS;
	CMyButton	m_ChatBtnCloseB;
	CMyCheck	m_ChatCHkH;
	CMyCheck	m_ChatCHkG;
	CMyCheck	m_ChatCHkD;
	CMyCheck	m_ChatChkB;
	CMyCheck	m_ChatChkZ;
	CMyCheck	m_ChatChkF;
	CMyButton	m_ChatBtnSift;
	CMyButton	m_ChatBtnRecord;
	CMyButton	m_BtnHelp;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnChatBtnCloses();
	afx_msg void OnBtnHelp();
	afx_msg void OnChatCloseb();
	afx_msg void OnChatBtnRecord();
	afx_msg void OnChatBtnSift();
	afx_msg void OnChatChkG();
	afx_msg void OnChatChkB();
	afx_msg void OnChatChkD();
	afx_msg void OnChatChkH();
	afx_msg void OnChatChkS();
	afx_msg void OnChatChkZ();
	afx_msg void OnChatChkF();
	afx_msg void OnChatChkSf();
	afx_msg void OnChatImgGc();
	afx_msg void OnChatImgDc();
	afx_msg void OnChatImgSc();
	afx_msg void OnChatImgBc();
	afx_msg void OnChatImgHc();
	afx_msg void OnChatImgZc();
	afx_msg void OnChatImgFc();
	afx_msg void OnChatImgCc1();
	afx_msg void OnChatImgCc2();
	afx_msg void OnChatImgCc3();
	afx_msg void OnChatImgCc4();
	afx_msg void OnChatImgCc5();
	afx_msg void OnChatImgCc6();
	afx_msg void OnChatImgCc7();
	afx_msg void OnChatImgCc8();
	DECLARE_MESSAGE_MAP()

	friend struct DlgChatProbe ;
};

#endif
