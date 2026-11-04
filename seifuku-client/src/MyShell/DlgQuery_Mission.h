#if !defined(AFX_DLGQUERY_MISSION_H__SEIFUKU_135__INCLUDED_)
#define AFX_DLGQUERY_MISSION_H__SEIFUKU_135__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyImage.h"

class CDlgQuery_Mission : public CDialog
{
public:
	int GetMouseMovePosition();
	void SetPickUpState ( BOOL bPickUp, int nIconRealID );
	void FlashEquip();
	BOOL HeroEquipItem( int nPos );
	BOOL IsEquipEnable();
	void Show(int x,int y);
	CDlgQuery_Mission(CWnd* pParent = NULL);
	BOOL	m_bShow ;
private:
	HBRUSH		m_Hbr ;
	BOOL		m_bInitSuc ;
	CPoint		m_Pnt ;
	BOOL		m_bPickUp ;
	int			m_nIconRealID ;
	BOOL		m_bLHandEnable ;
	enum { IDD = IDD_DIALOG_QUERY_MISSION };
	CMyImage	m_ImgGarment;
	CMyImage	m_ImgTreasure;
	CMyImage	m_ImgHead;
	CMyImage	m_ImgNeck;
	CMyImage	m_ImgGear;
	CMyImage	m_ImgBody;
	CMyImage	m_ImgLHand;
	CMyImage	m_ImgRHand;
	CMyImage	m_ImgShoe;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMissionImaHead();
	afx_msg void OnMissionImaNeck();
	afx_msg void OnMissionImaGear();
	afx_msg void OnMissionImaBody();
	afx_msg void OnMissionImaLhand();
	afx_msg void OnMissionImaRhand();
	afx_msg void OnMissionImaFoot();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDoubleclickedMissionImaHead();
	afx_msg void OnDoubleclickedMissionImaLhand();
	afx_msg void OnDoubleclickedMissionImaNeck();
	afx_msg void OnDoubleclickedMissionImaBody();
	afx_msg void OnDoubleclickedMissionImaFoot();
	afx_msg void OnDoubleclickedMissionImaGear();
	afx_msg void OnDoubleclickedMissionImaRhand();
	afx_msg void OnMissionImaTreasure();
	afx_msg void OnDoubleclickedMissionImaTreasure();
	afx_msg void OnMissionImaGarment();
	afx_msg void OnDoubleclickedMissionImaGarment();
	DECLARE_MESSAGE_MAP()

	friend struct DlgQuery_MissionProbe ;
};

#endif
