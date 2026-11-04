#if !defined(AFX_DLGMAIN_H__7BCFAE77_739A_4B63_8203_299C48F01BF0__INCLUDED_)
#define AFX_DLGMAIN_H__7BCFAE77_739A_4B63_8203_299C48F01BF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyButton.h"
#include "MyList.h"
#include "MyProgress.h"
#include "MySlider.h"
#include "MyStatic.h"
#include "MyGrid.h"
#include "MyCheck.h"
#include "MyImage.h"

#include "DlgQuery.h"
#include "DlgFriend.h"
#include "DlgMpcGoods.h"
#include "DlgTrade.h"
#include "DlgSetup.h"
#include "DlgAct.h"
#include "DlgGroup.h"
#include "DlgBag.h"
#include "DlgTalk.h"
#include "DlgNpcBuy.h"
#include "DlgDepot.h"
#include "DlgMagic.h"
#include "DlgXp.h"
#include "DlgKongfu.h"
#include "DlgPk.h"
#include "DlgImprove.h"
#include "DlgEnchant.h"
#include "DlgNpcFace.h"
#include "DlgQuery_Faction.h"
#include "DlgEmbed.h"
#include "DlgHotkey.h"
#include "DlgMsgSend.h"
#include "DlgMsgReceive.h"
#include "DlgMpcBooth.h"
#include "DlgNpcBooth.h"
#include "DlgHawk.h"
#include "DlgChat.h"
#include "DlgBlack.h"
#include "DlgLevWord.h"
#include "DlgEffigy.h"
#include "DlgLay.h"
#include "DlgOther.h"
#include "DlgNpcEquip.h"
#include "DlgDice.h"
#include "DlgDiceList.h"
#include "DlgEnemy.h"
#include "DlgInsHelp.h"
#include "DlgCompose.h"
#include "DlgFuse.h"
#include "DlgExeConfirm.h"
#include "DlgExercise.h"

class CDlgMain : public CDialog
{
public:
	void CloseCurDlg ();
	BOOL IsCurDlgShow ();
	void ShowChildDlg ();
	void Show ();
	CDlgMain(CWnd* pParent = NULL);
	BOOL		m_bNoMana ;

	BOOL		m_bLevRead ;

	BOOL		m_bFactionRead ;

	BOOL		m_bPkFlash ;
private:
	POINT		m_DlgPnt ;
	HBRUSH		m_Hbr ;
public:
	BOOL			m_bCreate ;
private:
	CRect		m_rcTip[6] ;
	CRgn*		m_pRgnTip[3] ;
	BYTE		m_btCurTip ;
	CPoint		m_ActPnt ;
	BOOL		m_bItemAct ;
	int			m_nActAmount ;
	DWORD		m_dwLastActTime ;

	BOOL		m_bXpAct ;
	DWORD		m_dwLastXpTime ;

	char		m_strOwnerIni[128] ;

	int			m_nUnknown1 ;
public:
	UINT			m_uCurDlgID ;

	CDlgQuery			m_DlgQuery ;
	CDlgFriend			m_DlgFriend ;
	CDlgMpcGoods		m_DlgMpcGoods ;
	CDlgTrade			m_DlgTrade ;
	CDlgSetup			m_DlgSetup ;
	CDlgAct				m_DlgAct ;
	CDlgGroup			m_DlgGroup ;
	CDlgBag				m_DlgBag ;
	CDlgTalk			m_DlgTalk ;
	CDlgNpcBuy			m_DlgNpcBuy ;
	CDlgDepot			m_DlgDepot ;
	CDlgMagic			m_DlgMagic ;
	CDlgXp				m_DlgXp ;
	CDlgKongfu			m_DlgKongfu ;
	CDlgPk				m_DlgPk ;
	CDlgImprove			m_DlgImprove ;
	CDlgEnchant			m_DlgEnchant ;
	CDlgNpcFace			m_DlgNpcFace ;
	CDlgQuery_Faction	m_DlgQuery_Faction ;
	CDlgEmbed			m_DlgEmbed ;
	CDlgHotkey			m_DlgHotkey ;
	CDlgMsgSend			m_DlgMsgSend ;
	CDlgMsgReceive		m_DlgMsgReceive ;
	CDlgMpcBooth		m_DlgMpcBooth ;
	CDlgNpcBooth		m_DlgNpcBooth ;
	CDlgHawk			m_DlgHawk ;
	CDlgChat			m_DlgChat ;
	CDlgBlack			m_DlgBlack ;
	CDlgLevWord			m_DlgLevWord ;
	CDlgEffigy			m_DlgEffigy ;
	CDlgLay				m_DlgLay ;
	CDlgOther			m_DlgOther ;
	CDlgNpcEquip		m_DlgNpcEquip ;
	CDlgDice			m_DlgDice ;
	CDlgDiceList		m_DlgDiceList ;
	CDlgEnemy			m_DlgEnemy ;
	CDlgInsHelp			m_DlgInsHelp ;
	CDlgCompose			m_DlgCompose ;
	CDlgFuse			m_DlgFuse ;
	CDlgExeConfirm		m_DlgExeConfirm ;
	CDlgExercise		m_DlgExercise ;

public:
	void OpenEmbed();
	void OpenNpcFace();
	void OpenEnchant();
	void OpenImprove();
	void HeroDie();
	void HeroReborn();
	void PlayXp();
	void OnXpFull();
	void SetRecord( const char* strIniFile );
	void OpenDepot( int nNpcType, OBJID idNpc );
	void PlayItemQUse( int nGridIndex );
	void OnShellEnter();
	void OpenNpcShop ( int nNpcType, OBJID idNpc );
	enum { IDD = IDD_MAIN_DIALOG };
	CMyProgress	m_BlessForcePrg ;
	CMyProgress	m_ForcePrg ;
	CMyButton	m_FactionBtn ;
	CMyGrid		m_GoodBox ;
	CMyProgress	m_ExpPrg ;
	CMyProgress	m_XpPrg ;
	CMyProgress	m_MpPrg ;
	CMyProgress	m_HpPrg ;
	CMyCheck	m_TalkChk ;
	CMyCheck	m_MapChk ;
	CMyCheck	m_RunChk ;
	CMyCheck	m_ScreenMoveChk ;
	CMyCheck	m_StandChk ;
	CMyButton	m_PkBtn ;
	CMyButton	m_QueryBtn ;
	CMyButton	m_LevWordBtn ;
	CMyButton	m_GoodBtn ;
	CMyButton	m_SetupBtn ;
	CMyButton	m_ActBtn ;
	CMyButton	m_ChatBtn ;
	CMyButton	m_GroupBtn ;
	CMyButton	m_FriendBtn ;
	CMyImage	m_MagicImg ;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void OpenCompose();
	void CloseOtherDlg( BOOL bCloseHelp );
	void SetProgressRange();
	void WriteRecord();
	void OpenDice( BOOL bOpen );
	void OpenLay();
	void OpenEffigy();
	void OpenBooth( BOOL bOwner );

	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMainQuery();
	afx_msg void OnMainGood();
	afx_msg void OnMainSetup();
	afx_msg void OnMainChat();
	afx_msg void OnMainGroup();
	afx_msg void OnMainMagic();
	afx_msg void OnMainFriend();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMainFaction();
	afx_msg void OnMainLevword();
	afx_msg void OnMainAct();
	afx_msg void OnMainRun();
	afx_msg void OnMainPk();
	afx_msg void OnMainTalk();
	afx_msg void OnMainMap();
	afx_msg void OnMainStand();
	afx_msg void OnMainGoodbox();
	afx_msg void OnMainScreenmove();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

	friend struct DlgMainProbe ;
};

#endif
