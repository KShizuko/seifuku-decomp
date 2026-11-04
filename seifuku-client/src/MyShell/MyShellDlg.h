#if !defined(AFX_MYSHELLDLG_H__61E22FCF_BADA_42B3_A353_20C540D23727__INCLUDED_)
#define AFX_MYSHELLDLG_H__61E22FCF_BADA_42B3_A353_20C540D23727__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Item.h"
#include "Ani.h"

#include "DlgLogin.h"
#include "DlgRoleChoose.h"
#include "DlgRoleCreate.h"
#include "DlgMain.h"
#include "DlgConfirm.h"
#include "DlgTask.h"
#include "DlgTitleBar.h"
#include "MySlider.h"
#include "DlgHelp.h"
#include "DlgLog.h"
#include "DlgLogo.h"

class CPlayer ;
class CMutexGuard ;
class CMyShellDlg : public CDialog
{
public:
	void RealExit( UINT uType );
	void ConfirmedExit();
	BOOL ProcessHotkey ( int nKey );
	void MoveControl();
	void MoveDlg();
	void SetQUseItemNum();
	void SetScreenMode( BYTE btType, BOOL bBegin = true );
	void OnIconQUse ( LPARAM lParam );
	void ShowMessage( CItem* pItem, IAni* pAni, int nItemNum = 0 , BOOL bShowDur = true);
	void ShowHelpDlg( LPARAM lParam );
	void OnIconBuy(LPARAM lParam );
	void OnIconUse ( LPARAM lParam );
	void PickMapItem ( OBJID ItemID, int nPosx, int nPosy);
	void DropMapItem( int nPosx, int nPosy );
	void OnExit ( UINT uType = 0 );
	void OnMpcGoodsClose();
	void OnIconTrade( LPARAM lParam );
	void OnIconDrop( LPARAM lParam );
	void ShowPickUpIcon();
	void OnIconPickUp( LPARAM lParam );
	void ShowMsgDlg ();
	void SetMsgDlgPnt ( LPARAM lParam, int nWidth );
	void CreateRole( BYTE btRoleType ) ;
	void StartGame();
	void FlashInsHelp();
	CDlgLogin	m_DlgLogin ;
	CDlgRoleChoose m_DlgRoleChoose ;
	CDlgRoleCreate m_DlgRoleCreate ;
	CDlgMain	m_DlgMain ;
	CDlgConfirm	m_DlgConfirm ;
	CDlgTask	m_DlgTask ;
	CDlgTitleBar m_DlgTitleBar ;
	CDlgHelp	m_DlgHelp ;
	CDlgLog		m_DlgLog ;
	BOOL		m_bLogin ;
	BOOL		m_bGameStarted ;
	BOOL		m_bWait ;
	BOOL		m_bRoleCreate ;
	BOOL		m_bMsgDlg ;
	int			m_nMsgDlgWidth ;
	CPoint		m_MsgDlgPnt ;
	CPoint		m_StaticPoint[9] ;
	UINT		m_uMsgDlgType ;
	DWORD		m_dwMsgDlgOwner ;
	BYTE		m_btStaticID ;
	BOOL		m_bIconFix ;
	CRect		m_InitDlgRect ;
	BOOL		m_bDlgMove ;
	int			m_nScreenMode ;
	DEVMODE		m_OldDeviceMode ;
	BOOL		m_bPickUp ;
	CPoint		m_IconMousePnt ;
	CPoint		m_IconShowPnt ;
	OBJID		m_IconTypeID ;
	OBJID		m_IconRealID ;
	DWORD		m_IconIndex ;
	UINT		m_PickUpDlgID ;
	BYTE		m_btIconType ;
	BYTE		m_btIconActType ;
	OBJID		m_MagicID ;
	OBJID		m_DisMagicID ;
	OBJID		m_XpID ;
	UINT		m_uCurSkill ;
	CFont*		m_pGameFont ;
	char		m_strOwnerIni[128] ;
	DWORD		m_dwScale ;
	int			m_nFacWordValue ;
	int			m_nImptIndex ;
	int			m_nDropIndex ;
	CPoint		m_DropPnt ;
	BOOL		m_bExpShow ;
	BOOL		m_bExit ;
	BOOL		m_bInitSuc ;
	OBJID		m_idActiveNpc ;
	BOOL		m_bMouseInShell ;

	CMyShellDlg(CWnd* pParent = NULL);

	DWORD m_dwGameLoop;

	enum {CMD_NORMAL=0, CMD_MARRIAGE, CMD_DIVORCE,
			CMD_ATK, CMD_FRDMAKE, CMD_FRDBREAK,
				CMD_TEMJOIN, CMD_TEMINVITE,
					CMD_FACJOIN, CMD_FACINVITE,
						CMD_TALK,
							CMD_FREEZE, CMD_UNFREEZE, CMD_SELFUNFREEZE,
								CMD_TRADE, CMD_LAY, CMD_MPCCOURT, CMD_DELNPC,
									CMD_NPCEQUIP};
	UINT m_uCmd;

	HCURSOR		m_hCursorH;
	BOOL		m_bCursorNpc ;
	BOOL		m_bCursorPk ;
	BOOL		m_bCursorPick ;
	HBRUSH		m_Hbr ;
	CMutexGuard* m_pMutexGuard ;
	CMyButton	m_BtnSysHide ;
	CMyButton	m_BtnMsgHide ;
	CMySlider	m_SliLine ;
private:
	enum { IDD = IDD_MYSHELL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void CheckCursor () ;
protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT OnMyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBtnSyshide();
	afx_msg void OnBtnMsghide();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()

	friend struct MyShellDlgProbe ;
};

#endif
