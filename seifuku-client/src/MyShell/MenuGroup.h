#if !defined(AFX_MENUGROUP_H__07ED8FDF_E009_4EA8_9A33_BDAA228D7F2A__INCLUDED_)
#define AFX_MENUGROUP_H__07ED8FDF_E009_4EA8_9A33_BDAA228D7F2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyCheck.h"
#include "MyButton.h"
const int MENU_GROUP_CLICK = 5000 ;
const int GEAR_CLICK	=	MENU_GROUP_CLICK + 1 ;
const int MONEY_CLICK	=	MENU_GROUP_CLICK + 2 ;
const int ITEM_CLICK	=	MENU_GROUP_CLICK + 3 ;

class CMenuGroup : public CDialog
{
public:
	void SetParentWnd ( CWnd* pParentWnd);
	void Show();
	CMenuGroup(CWnd* pParent = NULL);

	BOOL				m_bShow ;
	BOOL				m_bCreate ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	CWnd*				m_ParentWnd ;
public:
	enum { IDD = IDD_MENU_GROUP };
	CMyCheck	m_GroupChkStop;
	CMyCheck	m_GroupChkMoney;
	CMyCheck	m_GroupChkItem;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnGroupChkStop();
	afx_msg void OnGroupChkMoney();
	afx_msg void OnGroupChkItem();
	DECLARE_MESSAGE_MAP()

	friend struct MenuGroupProbe ;
};

#endif
