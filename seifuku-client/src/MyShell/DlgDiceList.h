#if !defined(AFX_DLGDICELIST_H__FEFEE47F_5172_426C_B142_AA5E7A0D793D__INCLUDED_)
#define AFX_DLGDICELIST_H__FEFEE47F_5172_426C_B142_AA5E7A0D793D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyList.h"

class CDlgDiceList : public CDialog
{
public:
	void SetDiceList();
	void Show();
	CDlgDiceList(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	enum { IDD = IDD_DIALOG_DICELIST };
	CMyList	m_DiceListList;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	DECLARE_MESSAGE_MAP()

	friend struct DlgDiceListProbe ;
};

#endif
