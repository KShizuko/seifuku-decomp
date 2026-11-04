#if !defined(AFX_DLGFACSTATE_H__C43FC724_0665_4433_8B60_19691EB69718__INCLUDED_)
#define AFX_DLGFACSTATE_H__C43FC724_0665_4433_8B60_19691EB69718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyStatic.h"

class CDlgFacState : public CDialog
{
public:
	void SetFacState( const char* strMemName );
	void Show();
	CDlgFacState(CWnd* pParent = NULL);
	BOOL				m_bShow	 ;
private:
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	enum { IDD = IDD_DIALOG_FACSTATE };
	CMyStatic	m_StaProffer;
	CMyStatic	m_StaSynJob;
	CMyStatic	m_StaLevel;
	CMyStatic	m_StaName;
	CMyButton	m_BtnClose;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnFacstateBtnClose();
	DECLARE_MESSAGE_MAP()

	friend struct DlgFacStateProbe ;
};

#endif
