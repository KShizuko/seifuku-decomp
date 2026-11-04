#if !defined(AFX_DLGNPCEQUIP_H__46045E5D_C390_4D48_9FB2_AC710C9C9CFC__INCLUDED_)
#define AFX_DLGNPCEQUIP_H__46045E5D_C390_4D48_9FB2_AC710C9C9CFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyImage.h"
#include "MyStatic.h"

class CDlgNpcEquip : public CDialog
{
public:
	void Show();
	CDlgNpcEquip(CWnd* pParent = NULL);
	BOOL	m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint		m_Pnt ;
	int 		m_nPntX ;
	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;
public:
	int GetMouseMovePosition();
	void SetNpcEquip();
	enum { IDD = IDD_DIALOG_NPCEQUIP };
	CMyImage	m_ImgGarment;
	CMyImage	m_ImgTreasure;
	CMyStatic	m_StaName;
	CMyStatic	m_StaFaction;
	CMyImage	m_ImgBody;
	CMyImage	m_ImgHead;
	CMyImage	m_ImgNeck;
	CMyImage	m_ImgGear;
	CMyImage	m_ImgLHand;
	CMyImage	m_ImgRHand;
	CMyImage	m_ImgShoe;
	CMyButton	m_BtnCloseB;
	CMyButton	m_BtnClose;
	CMyButton	m_BtnHelp;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnNpcequipClose();
	afx_msg void OnNpcequipCloseb();
	afx_msg void OnBtnHelp();
	DECLARE_MESSAGE_MAP()

	friend struct DlgNpcEquipProbe ;
};

#endif
