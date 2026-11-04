#if !defined(AFX_DLGEMBED_H__806211B4_80E3_406D_9455_3CD1085724CC__INCLUDED_)
#define AFX_DLGEMBED_H__806211B4_80E3_406D_9455_3CD1085724CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyImage.h"

class CDlgEmbed : public CDialog
{
public:
	void OnCloseB();
	void SetPickUpState ( BOOL bPickUp, int nIconRealID );
	int GetMouseMoveId ( OBJID* pidItem );
	void TackOffGem ();
	void Show();
	CDlgEmbed(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	int					m_nPntX ;
	BOOL				m_bPickUp ;
	int					m_nIconRealID ;
	OBJID				m_GoodID ;
	OBJID				m_Gem1ID ;
	OBJID				m_Gem2ID ;
	BOOL				m_bGem1Old ;
	BOOL				m_bGem2Old ;
	int					m_nGoodIndex ;
	int					m_nGem1Index ;
	int					m_nGem2Index ;
	int					m_nCurGem ;
	BOOL				m_bTackOffWait ;
	int					m_nTackOffGem ;

	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;
	enum { IDD = IDD_DIALOG_EMBED };
	CMyButton	m_EmbedBtnCloses;
	CMyImage	m_EmbedImaGood;
	CMyImage	m_EmbedImaGem2;
	CMyImage	m_EmbedImaGem1;
	CMyButton	m_EmbedBtnClose;
	CMyButton	m_EmbedBtnTackOff;
	CMyButton	m_EmbedBtnEmbed;
	CMyButton	m_BtnHelp;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnEmbedImaGood();
	afx_msg void OnEmbedClose();
	afx_msg void OnEmbedImaGem1();
	afx_msg void OnEmbedImaGem2();
	afx_msg void OnDoubleclickedEmbedImaGood();
	afx_msg void OnDoubleclickedEmbedImaGem2();
	afx_msg void OnDoubleclickedEmbedImaGem1();
	afx_msg void OnEmbedBtnEmbed();
	afx_msg void OnEmbedBtnTackOff();
	afx_msg void OnBtnHelp();
	DECLARE_MESSAGE_MAP()

	friend struct DlgEmbedProbe ;
};

#endif
