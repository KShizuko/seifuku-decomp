#if !defined(AFX_DLGENEMY_H__E4498A99_19F7_4CEE_9D80_068A76748954__INCLUDED_)
#define AFX_DLGENEMY_H__E4498A99_19F7_4CEE_9D80_068A76748954__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyList.h"
#include "DlgEnmState.h"

class CDlgEnemy : public CDialog
{
public:
	void Show();
	void SetEnemyState();
	void SetEnemyList ();

	CDlgEnemy(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	int 				m_nPntX ;
	DWORD				m_dwLastShowTime ;
	float				m_fFrameStep ;
	int					m_nFrameMode ;
public:
	CDlgEnmState		m_DlgEnmState ;
	int					m_nCurrentQueryIndex ;

	enum { IDD = IDD_DIALOG_ENEMY };
	CMyButton	m_EnemyBtnResume;
	CMyList	m_EnemyLstData;
	CMyButton	m_EnemyBtnClose;
	CMyButton	m_BtnHelp;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnEnemyBtnClose();
	afx_msg void OnBtnHelp();
	afx_msg void OnEnemyBtnResume();
	DECLARE_MESSAGE_MAP()

	friend struct DlgEnemyProbe ;
};

#endif
