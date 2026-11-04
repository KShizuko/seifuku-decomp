#if !defined(AFX_DLGNPCFACE_H__F71206F5_1221_4BEE_ADB3_13E6B8E126A0__INCLUDED_)
#define AFX_DLGNPCFACE_H__F71206F5_1221_4BEE_ADB3_13E6B8E126A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyImage.h"
#include "MyButton.h"

class CDlgNpcFace : public CDialog
{
public:
	void Show();
	CDlgNpcFace(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	int 				m_nPntX ;
	int					m_nMaleFaceAmount ;
	int					m_nFemaleFaceAmount ;
	int					m_nCurrentAmount ;
	int					m_uFocusImage ;

	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;
public:
	void InsertNpcFace( int nImgIndex, int nFaceIndex );
	void InitNpcFace( BYTE btType );
	enum { IDD = IDD_DIALOG_NPCFACE };
	CMyButton	m_NpcFaceBtnCloseB;
	CMyButton	m_NpcFaceBtnUp;
	CMyButton	m_NpcFaceBtnDown;
	CMyImage	m_NpcFaceIma6;
	CMyImage	m_NpcFaceIma5;
	CMyImage	m_NpcFaceIma4;
	CMyImage	m_NpcFaceIma3;
	CMyImage	m_NpcFaceIma2;
	CMyImage	m_NpcFaceIma1;
	CMyButton	m_NpcFaceBtnClose;
	CMyButton	m_NpcFaceBtnAgree;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnNpcfaceIma1();
	afx_msg void OnNpcfaceIma2();
	afx_msg void OnNpcfaceIma3();
	afx_msg void OnNpcfaceIma4();
	afx_msg void OnNpcfaceIma5();
	afx_msg void OnNpcfaceIma6();
	afx_msg void OnNpcfaceBtnUp();
	afx_msg void OnNpcfaceBtnDown();
	afx_msg void OnNpcfaceBtnAgree();
	afx_msg void OnNpcfaceBtnClose();
	afx_msg void OnNpcfaceBtnCloseb();
	DECLARE_MESSAGE_MAP()

	friend struct DlgNpcFaceProbe ;
};

#endif
