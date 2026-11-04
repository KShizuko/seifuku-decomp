#if !defined(AFX_DLGIMPROVE_H__4370C54E_32A0_46F7_88CC_0E4A60256C90__INCLUDED_)
#define AFX_DLGIMPROVE_H__4370C54E_32A0_46F7_88CC_0E4A60256C90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyImage.h"
#include "MyButton.h"

class CDlgImprove : public CDialog
{
public:
	CDlgImprove(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	int 				m_nPntX ;
	BOOL				m_bPickUp ;
	int					m_nIconRealID ;
	OBJID				m_GoodID ;
	OBJID				m_GemID ;
	int					m_nGoodIndex ;
	int					m_nGemIndex ;
	int					m_nImproveType ;

	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;

public:
	DWORD GetMouseMoveId();
	void SetPickUpState( BOOL bPickUp, int nIconRealID );
	void Show();
	enum { IDD = IDD_DIALOG_IMPROVE };
	CMyButton	m_ImproveBtnCloseB;
	CMyButton	m_BtnHelp;
	CMyButton	m_ImproveBtnClose;
	CMyImage	m_ImproveImaGood;
	CMyImage	m_ImproveImaGem;
	CMyButton	m_ImproveBtnImp;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnImproveImaGem();
	afx_msg void OnImproveImaGood();
	afx_msg void OnImproveBtnImp();
	afx_msg void OnImproveBtnClose();
	afx_msg void OnDoubleclickedImproveImaGem();
	afx_msg void OnDoubleclickedImproveImaGood();
	afx_msg void OnBtnHelp();
	afx_msg void OnImproveBtnCloseb();
	DECLARE_MESSAGE_MAP()

	friend struct DlgImproveProbe ;
};

#endif
