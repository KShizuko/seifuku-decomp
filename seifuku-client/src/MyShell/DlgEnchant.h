#if !defined(AFX_DLGENCHANT_H__7C1E5D20_4294_4B6A_8F11_0406BD000105__INCLUDED_)
#define AFX_DLGENCHANT_H__7C1E5D20_4294_4B6A_8F11_0406BD000105__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyImage.h"

class CDlgEnchant : public CDialog
{
public:
	void SetPickUpState ( BOOL bPickUp, int nIconRealID ) ;
	OBJID GetMouseMoveId () ;
	int GetMouseMoveIndex () ;
	void Show();
	CDlgEnchant(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	int					m_nPntX ;
	BOOL				m_bPickUp ;
	int					m_nIconRealID ;
	OBJID				m_GoodID ;
	OBJID				m_GemID ;
	int					m_nGoodIndex ;
	int					m_nGemIndex ;

	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;
public:
	enum { IDD = IDD_DIALOG_ENCHANT };
	CMyButton	m_BtnHelp;
	CMyImage	m_EnchantImaGood;
	CMyImage	m_EnchantImaGem;
	CMyButton	m_EnchantBtnOk;
	CMyButton	m_EnchantBtnClose;
	CMyButton	m_EnchantBtnCloses;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg void OnEnchantBtnClose();
	afx_msg void OnEnchantBtnOk();
	afx_msg void OnBtnHelp();
	afx_msg void OnEnchantBtnCloses();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnchantImaGood();
	afx_msg void OnEnchantImaGem();
	afx_msg void OnMove(int x, int y);
	virtual BOOL OnInitDialog();
	afx_msg void OnDoubleclickedEnchantImaGem();
	afx_msg void OnDoubleclickedEnchantImaGood();
	DECLARE_MESSAGE_MAP()

	friend struct DlgEnchantProbe ;
};

#endif
