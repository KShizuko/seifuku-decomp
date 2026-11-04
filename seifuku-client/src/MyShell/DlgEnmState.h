#if !defined(AFX_DLGENMSTATE_H__CED952E6_768B_4945_88F5_8C93B1831769__INCLUDED_)
#define AFX_DLGENMSTATE_H__CED952E6_768B_4945_88F5_8C93B1831769__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyImage.h"
#include "MyStatic.h"
#include "MyButton.h"

class CDlgEnmState : public CDialog
{
public:
	void Show();
	void SetEnemyNull( char* strName );
	void SetEnemyState( char* strEnemyName );
	void DelEnemy();

	CDlgEnmState(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	enum { IDD = IDD_DIALOG_ENMSTATE };
	CMyImage	m_EnmStateImaface;
	CMyStatic	m_Sta6;
	CMyStatic	m_Sta5;
	CMyStatic	m_Sta1;
	CMyStatic	m_Sta2;
	CMyStatic	m_Sta3;
	CMyStatic	m_Sta4;
	CMyStatic	m_Sta7;
	CMyButton	m_EnmStateBtnDel;
	CMyButton	m_EnmStateBtnClose;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnEnmstateBtnClose();
	afx_msg void OnEnmstateBtnDel();
	DECLARE_MESSAGE_MAP()

	friend struct DlgEnmStateProbe ;
};

#endif
