#if !defined(AFX_DLGMSGRECEIVE_H__33C5E3E1_31AD_49ED_AC24_CD19B9DABE80__INCLUDED_)
#define AFX_DLGMSGRECEIVE_H__33C5E3E1_31AD_49ED_AC24_CD19B9DABE80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyStatic.h"
#include "MyEdit.h"

class CDlgMsgReceive : public CDialog
{
public:
	void SetReceiveMsg();
	void Show();
	CDlgMsgReceive(CWnd* pParent = NULL);
	BOOL	m_bShow ;
private:
	HBRUSH				m_Hbr ;
	HBRUSH				m_HbrBlack ;
	CPoint				m_Pnt ;
	int					m_nCurIndex ;
	enum { IDD = IDD_DIALOG_MSGRECEIVE };
	CMyButton	m_MsgReceiveDelall;
	CMyButton	m_MsgReceivePre;
	CMyEdit		m_MsgReceiveData;
	CMyStatic	m_MsgReceiveName;
	CMyButton	m_MsgReceiveClose;
	CMyButton	m_MsgReceiveCloses;
	CMyButton	m_MsgReceiveNext;
	CMyButton	m_MsgReceiveRevert ;
	CMyButton	m_BtnHelp ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMsgreceiveCloseb();
	afx_msg void OnBtnHelp();
	afx_msg void OnMsgreceiveRevert();
	afx_msg void OnMsgreceivePre();
	afx_msg void OnMsgreceiveNext();
	afx_msg void OnMsgreceiveDelall();
	DECLARE_MESSAGE_MAP()

	friend struct DlgMsgReceiveProbe ;
};

#endif
