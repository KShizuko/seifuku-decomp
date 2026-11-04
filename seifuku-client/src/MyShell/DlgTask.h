#if !defined(AFX_DLGTASK_H__033DA5A9_1DB6_4C5C_89BA_A92D0B03D1E8__INCLUDED_)
#define AFX_DLGTASK_H__033DA5A9_1DB6_4C5C_89BA_A92D0B03D1E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyEdit.h"
#include "MyButton.h"
const int	HEADPIC_WIDTH = 64 ;
const int	HEADPIC_HEIGHT = 64 ;
const int	TEXT_LIMIT = 60 ;
const int	LINK_LIMIT = 30 ;

class CDlgTask : public CDialog
{
public:
	void Show();
	void SetLineIndex( BYTE btType );
	void ResetTaskDialog();
	void InitTaskDialog();
	CDlgTask(CWnd* pParent = NULL);
	BOOL				m_bShow ;
private:
	struct TaskText
	{
		char				strText[512] ;
		int					nLineIndex[8] ;
		int					nLineNum ;
	} m_TaskText[8] ;
	int					m_nTextAmount ;
	struct TaskLink
	{
		char	strText[256] ;
		CRect   Rect ;
		int		nLineIndex[8] ;
		int		nLineNum ;
		int		nIndex ;
	} m_TaskLink[8] ;
	int					m_nLinkAmount ;
	struct TaskEdit
	{
		char	strTitle[32] ;
		DWORD	dwData ;
		int		nIndex ;
	} m_TaskEdit	;
	int					m_nEditAmount ;
	int					m_nEditIndex ;
	int					m_nPicIndex ;
	CMySize				m_PicSize ;
	HBRUSH				m_Hbr ;
	CPoint				m_Pnt ;
	int					m_nCloseIndex ;
	int					m_nDlgHeight ;
	BOOL				m_bMouseMove ;
	BYTE				m_btCurLink ;
	int					m_nAnswerIndex ;
	int					m_nAnswerAmount ;
public:
	enum { IDD = IDD_DIALOG_TASK };
	CMyEdit		m_TaskEdtAnswer ;
	CMyButton	m_TaskBtnAnswer ;
	CMyButton	m_CloseBtn ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTaskBtnAnswer();
	afx_msg void OnBtnClose();
	DECLARE_MESSAGE_MAP()

	friend struct DlgTaskProbe ;
};

#endif
