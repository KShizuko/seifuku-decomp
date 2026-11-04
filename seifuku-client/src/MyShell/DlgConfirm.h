#if !defined(AFX_DLGCONFIRM_H__0189EE5F_CBB9_4DEF_AAAE_E426A36ECF9D__INCLUDED_)
#define AFX_DLGCONFIRM_H__0189EE5F_CBB9_4DEF_AAAE_E426A36ECF9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyStatic.h"
#include "MyButton.h"
#include "MyEdit.h"

const int TRADE_CONFIRM = 0 ;
const int TEAMJOIN_CONFIRM = 1 ;
const int TEAMINVITE_CONFIRM = 2 ;
const int FACTIONJOIN_CONFIRM = 3 ;
const int FACTIONINVITE_CONFIRM = 4 ;
const int MONEYDROP_CONFIRM = 5 ;
const int FRIENDDEL_CONFIRM = 6 ;
const int TALKDEL_CONFIRM = 7 ;
const int FACTIONOUT_CONFIRM = 8 ;
const int FACTIONSUB_CONFIRM = 9 ;
const int MPCBOOTHADD_CONFIRM = 10 ;
const int MPCBOOTHCHG_CONFIRM = 11 ;
const int FACTIONDEL_CONFIRM = 12 ;
const int BOOTHOPEN_CONFIRM = 13 ;
const int MPCCOURT_CONFIRM = 14 ;
const int IMPTITEM_CONFIRM = 15 ;
const int DROPITEM_CONFIRM = 16 ;
const int TACKOFF_CONFIRM = 17 ;
const int JOINDICE_CONFIRM = 18 ;
const int OUTDICE_CONFIRM = 19 ;
const int ENEMYDEL_CONFIRM = 20 ;
const int RESOURCE_CONFIRM = 21 ;
const int CONFIRM_TYPE_23 = 23 ;
const int CONFIRM_TYPE_24 = 24 ;
const int CONFIRM_TYPE_25 = 25 ;
const int CONFIRM_TYPE_26 = 26 ;
const int CONFIRM_TYPE_27 = 27 ;
const int CONFIRM_TYPE_28 = 28 ;

class CDlgConfirm : public CDialog
{
public:
	BOOL SetConfirmDlg ( UINT uType, const char* strName = NULL, OBJID TargetID = 0 );
	void Show();
	CDlgConfirm(CWnd* pParent = NULL);

	char*	GetConfirmText()	{return m_strText;}
	BOOL				m_bShow ;
private:
	HBRUSH				m_Hbr ;
	BOOL				m_bInitSuc ;
	CPoint				m_Pnt ;
	char				m_strText[128] ;
	int					m_nLine ;
	int					m_nLineNum ;
	int					m_nAniWidth ;
	int					m_nAniHeight ;
	int					m_nWidth ;
	int					m_nHeight ;
	int					m_nLeftLeft ;
	UINT				m_uType ;
	int					m_nEdtNum ;
	int					m_nEdtNum2 ;
	int					m_nSkip ;
	int					m_nSkip2 ;
	OBJID				m_TargetID ;
	DWORD				m_dwShowTime ;
public:
	enum { IDD = IDD_DIALOG_CONFIRM };
	CMyButton			m_ConfirmBtnExercise ;
	CMyStatic			m_ConfirmStaText ;
	CMyEdit				m_ConfirmEdtData2 ;
	CMyStatic			m_ConfirmStaData2 ;
	CMyStatic			m_ConfirmStaData ;
	CMyEdit				m_ConfirmEdtData;
	CMyButton m_ConfirmBtnOk ;
	CMyButton m_ConfirmBtnCancel ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnConfirmBtnOk();
	afx_msg void OnConfirmBtnCancel();
	afx_msg void OnChangeConfirmEdtData();
	afx_msg void OnChangeConfirmEdtData2();
	afx_msg void OnConfirmBtnExercise();
	DECLARE_MESSAGE_MAP()

	friend struct DlgConfirmProbe ;
};

#endif
