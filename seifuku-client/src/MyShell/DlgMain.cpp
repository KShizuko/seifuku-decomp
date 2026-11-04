#include "stdafx.h"
#include "MyShell.h"
#include "DlgMain.h"
#include "GameMsg.h"

#include "Hero.h"
#include "3DGameMap.h"
#include "GameMsg.h"
#include "NetMsg.h"
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _SOUND_DISABLE
	#define DXPlaySound
#endif

static DWORD HashName(const char* pszName)
{
	return ::Str2ID(pszName);
}

CDlgMain::CDlgMain(CWnd* pParent )
	: CDialog(CDlgMain::IDD, pParent)
{
	m_uCurDlgID = -1 ;
	m_btCurTip = 0xFF ;
	m_bItemAct = false ;
	m_bNoMana = true ;
	m_bLevRead = false ;
	m_bFactionRead = false ;
	m_bPkFlash = false ;
	m_bCreate = false ;
	m_strOwnerIni[0] = '\0' ;
	m_Hbr = ::CreateSolidBrush ( 0 ) ;
}


void CDlgMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN_BLESSFORCE, m_BlessForcePrg);
	DDX_Control(pDX, IDC_MAIN_FORCE, m_ForcePrg);
	DDX_Control(pDX, IDC_MAIN_FACTION, m_FactionBtn);
	DDX_Control(pDX, IDC_MAIN_GOODBOX, m_GoodBox);
	DDX_Control(pDX, IDC_MAIN_EXP, m_ExpPrg);
	DDX_Control(pDX, IDC_MAIN_XP, m_XpPrg);
	DDX_Control(pDX, IDC_MAIN_MP, m_MpPrg);
	DDX_Control(pDX, IDC_MAIN_HP, m_HpPrg);
	DDX_Control(pDX, IDC_MAIN_TALK, m_TalkChk);
	DDX_Control(pDX, IDC_MAIN_MAP, m_MapChk);
	DDX_Control(pDX, IDC_MAIN_RUN, m_RunChk);
	DDX_Control(pDX, IDC_MAIN_SCREENMOVE, m_ScreenMoveChk);
	DDX_Control(pDX, IDC_MAIN_STAND, m_StandChk);
	DDX_Control(pDX, IDC_MAIN_PK, m_PkBtn);
	DDX_Control(pDX, IDC_MAIN_QUERY, m_QueryBtn);
	DDX_Control(pDX, IDC_MAIN_LEVWORD, m_LevWordBtn);
	DDX_Control(pDX, IDC_MAIN_GOOD, m_GoodBtn);
	DDX_Control(pDX, IDC_MAIN_SETUP, m_SetupBtn);
	DDX_Control(pDX, IDC_MAIN_ACT, m_ActBtn);
	DDX_Control(pDX, IDC_MAIN_CHAT, m_ChatBtn);
	DDX_Control(pDX, IDC_MAIN_GROUP, m_GroupBtn);
	DDX_Control(pDX, IDC_MAIN_FRIEND, m_FriendBtn);
	DDX_Control(pDX, IDC_MAIN_MAGIC, m_MagicImg);
}


BEGIN_MESSAGE_MAP(CDlgMain, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_MAIN_GOODBOX, OnMainGoodbox)
	ON_BN_CLICKED(IDC_MAIN_QUERY, OnMainQuery)
	ON_BN_CLICKED(IDC_MAIN_LEVWORD, OnMainLevword)
	ON_BN_CLICKED(IDC_MAIN_GOOD, OnMainGood)
	ON_BN_CLICKED(IDC_MAIN_SETUP, OnMainSetup)
	ON_BN_CLICKED(IDC_MAIN_ACT, OnMainAct)
	ON_BN_CLICKED(IDC_MAIN_CHAT, OnMainChat)
	ON_BN_CLICKED(IDC_MAIN_GROUP, OnMainGroup)
	ON_BN_CLICKED(IDC_MAIN_RUN, OnMainRun)
	ON_BN_CLICKED(IDC_MAIN_PK, OnMainPk)
	ON_BN_CLICKED(IDC_MAIN_TALK, OnMainTalk)
	ON_BN_CLICKED(IDC_MAIN_MAP, OnMainMap)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_MAIN_SCREENMOVE, OnMainScreenmove)
	ON_BN_CLICKED(IDC_MAIN_MAGIC, OnMainMagic)
	ON_BN_CLICKED(IDC_MAIN_FACTION, OnMainFaction)
	ON_BN_CLICKED(IDC_MAIN_FRIEND, OnMainFriend)
	ON_BN_CLICKED(IDC_MAIN_STAND, OnMainStand)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BOOL CDlgMain::OnInitDialog()
{
	CDialog::OnInitDialog();
	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgMain::GetWindowRect ( rect ) ;

	m_GoodBox.MoveWindow ( 91, 101, 409, 40, true ) ;
	if ( !m_GoodBox.Init ( rect.left, rect.top, 1, GRID_ICONQUICK_NUM, DLG_MAIN ) )
		return false ;

	if ( !m_HpPrg.Init ( rect.left, rect.top, "Progress40", 0, 100, MY_PROGRESS_V ) )
		return false ;
	m_HpPrg.SetShowMode ( 1 ) ;
	m_HpPrg.SetWidthAndHeight ( 36, 74, 86 ) ;
	if ( !m_MpPrg.Init ( rect.left, rect.top, "Progress41", 0, 100, MY_PROGRESS_V ) )
		return false ;
	m_MpPrg.SetWidthAndHeight ( 34, 74, 34 ) ;
	if ( !m_XpPrg.Init ( rect.left, rect.top, "Progress42", 0, 100, MY_PROGRESS_V ) )
		return false ;
	m_XpPrg.SetWidthAndHeight ( 92, 92, 92 ) ;
	if ( !m_ForcePrg.Init ( rect.left, rect.top, "Progress46", 0, 100, MY_PROGRESS_V ) )
		return false ;
	m_ForcePrg.SetWidthAndHeight ( 8, 70, 8 ) ;
	if ( !m_BlessForcePrg.Init ( rect.left, rect.top, "Progress47", 0, 100, MY_PROGRESS_V ) )
		return false ;
	m_BlessForcePrg.SetWidthAndHeight ( 8, 35, 8 ) ;
	if ( !m_ExpPrg.Init ( rect.left, rect.top, "Progress44", 0, 100, MY_PROGRESS_HLEFT ) )
		return false ;

	if ( !m_QueryBtn.Init ( rect.left, rect.top, "Button40" ) )
		return false ;
	if ( !m_FriendBtn.Init ( rect.left, rect.top, "Button41" ) )
		return false ;
	if ( !m_GoodBtn.Init ( rect.left, rect.top, "Button42" ) )
		return false ;
	if ( !m_SetupBtn.Init ( rect.left, rect.top, "Button43" ) )
		return false ;
	if ( !m_ActBtn.Init ( rect.left, rect.top, "Button44" ) )
		return false ;
	if ( !m_ChatBtn.Init ( rect.left, rect.top, "Button45" ) )
		return false ;
	if ( !m_GroupBtn.Init ( rect.left, rect.top, "Button46" ) )
		return false ;
	if ( !m_PkBtn.Init ( rect.left, rect.top, "Button47" ) )
		return false ;
	if ( !m_FactionBtn.Init ( rect.left, rect.top, "Check62" ) )
		return false ;
	if ( !m_LevWordBtn.Init ( rect.left, rect.top, "Button410" ) )
		return false ;

	if ( !m_RunChk.Init ( rect.left, rect.top, "Check40", 2 ) )
		return false ;
	if ( !m_MapChk.Init ( rect.left, rect.top, "Check43", 2 ) )
		return false ;
	if ( !m_ScreenMoveChk.Init ( rect.left, rect.top, "Check46", 2 ) )
		return false ;
	if ( !m_StandChk.Init ( rect.left, rect.top, "Button411", 2 ) )
		return false ;

	if ( !m_MagicImg.Init ( rect.left, rect.top, "Magic0", 3, -1 ) )
		return false ;

	m_DlgLevWord.Create ( IDD_DIALOG_LEVWORD, CDlgMain::GetParent () ) ;
	m_DlgLevWord.GetWindowRect ( rect ) ;
	m_DlgLevWord.MoveWindow ( ( CMyBitmap::GetScreenWidth () - rect.Width () ) / 2,
		( CMyBitmap::GetScreenHeight () - rect.Height () ) / 2, rect.Width (), rect.Height () ) ;

	m_DlgMagic.Create ( IDD_DIALOG_MAGIC, CDlgMain::GetParent () ) ;
	m_DlgXp.Create ( IDD_DIALOG_XP, CDlgMain::GetParent () ) ;
	m_DlgKongfu.Create ( IDD_DIALOG_KONGFU, CDlgMain::GetParent () ) ;

	m_DlgTalk.Create ( IDD_DIALOG_TALK, CDlgMain::GetParent () ) ;
	m_DlgTalk.GetWindowRect ( rect ) ;
	m_DlgTalk.MoveWindow ( 82, CMyBitmap::GetScreenHeight () - 71, rect.Width (), rect.Height (), false ) ;
	m_DlgTalk.ShowWindow ( SW_SHOW ) ;

	m_DlgQuery.Create ( IDD_DIALOG_QUERY, CDlgMain::GetParent () ) ;
	m_DlgQuery.GetWindowRect ( rect ) ;
	m_DlgQuery.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;

	m_DlgFriend.Create ( IDD_DIALOG_FRIEND, CDlgMain::GetParent () ) ;
	m_DlgFriend.GetWindowRect ( rect ) ;
	m_DlgFriend.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;

	m_DlgInsHelp.Create ( IDD_DIALOG_INSHELP, CDlgMain::GetParent () ) ;
	m_DlgInsHelp.GetWindowRect ( rect ) ;
	m_DlgInsHelp.MoveWindow ( ( CMyBitmap::GetScreenWidth () - rect.Width () ) / 2,
		( CMyBitmap::GetScreenHeight () - rect.Height () ) / 2, rect.Width (), rect.Height () ) ;

	m_DlgMpcGoods.Create ( IDD_DIALOG_MPCGOODS, CDlgMain::GetParent () ) ;
	m_DlgMpcGoods.GetWindowRect ( rect ) ;
	m_DlgMpcGoods.MoveWindow ( CMyBitmap::GetScreenWidth () - rect.Width (), 0, rect.Width (), rect.Height () ) ;

	m_DlgTrade.Create ( IDD_DIALOG_TRADE, CDlgMain::GetParent () ) ;
	m_DlgTrade.GetWindowRect ( rect ) ;
	m_DlgTrade.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;

	m_DlgSetup.Create ( IDD_DIALOG_SETUP, CDlgMain::GetParent () ) ;
	m_DlgSetup.GetWindowRect ( rect ) ;
	m_DlgSetup.MoveWindow ( ( CMyBitmap::GetScreenWidth () - rect.Width () ) / 2,
		( CMyBitmap::GetScreenHeight () - rect.Height () ) / 2, rect.Width (), rect.Height () ) ;

	m_DlgHotkey.Create ( IDD_DIALOG_HOTKEY, CDlgMain::GetParent () ) ;
	m_DlgHotkey.GetWindowRect ( rect ) ;
	m_DlgHotkey.MoveWindow ( ( CMyBitmap::GetScreenWidth () - rect.Width () ) / 2,
		( CMyBitmap::GetScreenHeight () - rect.Height () ) / 2, rect.Width (), rect.Height () ) ;

	m_DlgAct.Create ( IDD_DIALOG_ACT, CDlgMain::GetParent () ) ;
	m_DlgAct.GetWindowRect ( rect ) ;
	m_DlgAct.MoveWindow ( CMyBitmap::GetScreenWidth () / 2 - rect.Width () / 2, CMyBitmap::GetScreenHeight () - 140, rect.Width (), rect.Height () ) ;

	m_DlgGroup.Create ( IDD_DIALOG_GROUP, CDlgMain::GetParent () ) ;
	m_DlgGroup.GetWindowRect ( rect ) ;
	m_DlgGroup.MoveWindow ( ( CMyBitmap::GetScreenWidth () - rect.Width () ) / 2 - 20, 0, rect.Width (), rect.Height () ) ;

	m_DlgBag.Create ( IDD_DIALOG_BAG, CDlgMain::GetParent () ) ;
	m_DlgBag.GetWindowRect ( rect ) ;
	m_DlgBag.MoveWindow ( ( CMyBitmap::GetScreenWidth () - rect.Width () ) / 2, 0, rect.Width (), rect.Height () ) ;

	m_DlgNpcBuy.Create ( IDD_DIALOG_NPCBUY, CDlgMain::GetParent () ) ;
	m_DlgNpcBuy.GetWindowRect ( rect ) ;
	m_DlgNpcBuy.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;

	m_DlgDepot.Create ( IDD_DIALOG_DEPOT, CDlgMain::GetParent () ) ;
	m_DlgDepot.GetWindowRect ( rect ) ;
	m_DlgDepot.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;

	m_DlgPk.Create ( IDD_DIALOG_PK, CDlgMain::GetParent () ) ;
	m_DlgPk.MoveWindow ( CMyBitmap::GetScreenWidth () - 290, CMyBitmap::GetScreenHeight () - 98, 200, 50 ) ;

	m_DlgImprove.Create ( IDD_DIALOG_IMPROVE, CDlgMain::GetParent () ) ;
	m_DlgImprove.GetWindowRect ( rect ) ;
	m_DlgImprove.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;

	m_DlgEnchant.Create ( IDD_DIALOG_ENCHANT, CDlgMain::GetParent () ) ;
	m_DlgEnchant.GetWindowRect ( rect ) ;
	m_DlgEnchant.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;

	m_DlgNpcFace.Create ( IDD_DIALOG_NPCFACE, CDlgMain::GetParent () ) ;
	m_DlgNpcFace.GetWindowRect ( rect ) ;
	m_DlgNpcFace.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;

	m_DlgQuery_Faction.Create ( IDD_DIALOG_QUERY_FACTION, CDlgMain::GetParent () ) ;
	m_DlgQuery_Faction.GetWindowRect ( rect ) ;
	m_DlgQuery_Faction.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;

	m_DlgEmbed.Create ( IDD_DIALOG_EMBED, CDlgMain::GetParent () ) ;
	m_DlgEmbed.GetWindowRect ( rect ) ;
	m_DlgEmbed.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;

	m_DlgCompose.Create ( IDD_DIALOG_EXP, CDlgMain::GetParent () ) ;
	m_DlgCompose.GetWindowRect ( rect ) ;
	m_DlgCompose.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;

	m_DlgFuse.Create ( IDD_DIALOG_FUSE, CDlgMain::GetParent () ) ;
	m_DlgFuse.GetWindowRect ( rect ) ;
	m_DlgFuse.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;
	m_DlgEffigy.Create ( IDD_DIALOG_EFFIGY, CDlgMain::GetParent () ) ;
	m_DlgEffigy.GetWindowRect ( rect ) ;
	m_DlgEffigy.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;

	m_DlgMsgSend.Create ( IDD_DIALOG_MSGSEND, CDlgMain::GetParent () ) ;
	m_DlgMsgSend.GetWindowRect ( rect ) ;
	m_DlgMsgSend.MoveWindow ( ( CMyBitmap::GetScreenWidth () - rect.Width () ) / 2,
		( CMyBitmap::GetScreenHeight () - rect.Height () ) / 2, rect.Width (), rect.Height () ) ;

	m_DlgMsgReceive.Create ( IDD_DIALOG_MSGRECEIVE, CDlgMain::GetParent () ) ;
	m_DlgMsgReceive.GetWindowRect ( rect ) ;
	m_DlgMsgReceive.MoveWindow ( ( CMyBitmap::GetScreenWidth () - rect.Width () ) / 2,
		( CMyBitmap::GetScreenHeight () - rect.Height () ) / 2, rect.Width (), rect.Height () ) ;

	m_DlgMpcBooth.Create ( IDD_DIALOG_MPCBOOTH, CDlgMain::GetParent () ) ;
	m_DlgMpcBooth.GetWindowRect ( rect ) ;
	m_DlgMpcBooth.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;

	m_DlgNpcBooth.Create ( IDD_DIALOG_NPCBOOTH, CDlgMain::GetParent () ) ;
	m_DlgNpcBooth.GetWindowRect ( rect ) ;
	m_DlgNpcBooth.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;

	m_DlgHawk.Create ( IDD_DIALOG_HAWK, CDlgMain::GetParent () ) ;
	m_DlgHawk.GetWindowRect ( rect ) ;
	m_DlgHawk.MoveWindow ( 0, ( CMyBitmap::GetScreenHeight () - rect.Height () ) / 2 + 80, rect.Width (), rect.Height () ) ;

	m_DlgChat.Create ( IDD_DIALOG_CHAT, CDlgMain::GetParent () ) ;
	m_DlgChat.GetWindowRect ( rect ) ;
	m_DlgChat.MoveWindow ( ( CMyBitmap::GetScreenWidth () - rect.Width () ) / 2,
		( CMyBitmap::GetScreenHeight () - rect.Height () ) / 2, rect.Width (), rect.Height () ) ;

	m_DlgBlack.Create ( IDD_DIALOG_BLACK, CDlgMain::GetParent () ) ;
	m_DlgBlack.GetWindowRect ( rect ) ;
	m_DlgBlack.MoveWindow ( ( CMyBitmap::GetScreenWidth () - rect.Width () ) / 2,
		( CMyBitmap::GetScreenHeight () - rect.Height () ) / 2, rect.Width (), rect.Height () ) ;

	m_DlgLay.Create ( IDD_DIALOG_LAY, CDlgMain::GetParent () ) ;
	m_DlgLay.GetWindowRect ( rect ) ;
	m_DlgLay.MoveWindow ( 0, 65, rect.Width (), rect.Height () ) ;

	m_DlgOther.Create ( IDD_DIALOG_OTHER, CDlgMain::GetParent () ) ;
	m_DlgOther.GetWindowRect ( rect ) ;
	m_DlgOther.MoveWindow ( ( CMyBitmap::GetScreenWidth () - rect.Width () ) / 2,
		( CMyBitmap::GetScreenHeight () - rect.Height () ) / 2, rect.Width (), rect.Height () ) ;

	m_DlgNpcEquip.Create ( IDD_DIALOG_NPCEQUIP, CDlgMain::GetParent () ) ;
	m_DlgNpcEquip.GetWindowRect ( rect ) ;
	m_DlgNpcEquip.MoveWindow ( CMyBitmap::GetScreenWidth () - rect.Width (), 65, rect.Width (), rect.Height () ) ;

	m_DlgDice.Create ( IDD_DIALOG_DICE, CDlgMain::GetParent () ) ;
	rect.SetRect ( 0, 0, 333, 392 ) ;
	m_DlgDice.MoveWindow ( CMyBitmap::GetScreenWidth () - rect.Width (), 0, rect.Width (), rect.Height () ) ;

	m_DlgDiceList.Create ( IDD_DIALOG_DICELIST, CDlgMain::GetParent () ) ;
	m_DlgDiceList.GetWindowRect ( rect ) ;
	m_DlgDiceList.MoveWindow ( CMyBitmap::GetScreenWidth () - rect.Width () - 355, 0, rect.Width (), rect.Height () ) ;

	m_DlgEnemy.Create ( IDD_DIALOG_ENEMY, CDlgMain::GetParent () ) ;
	m_DlgEnemy.GetWindowRect ( rect ) ;
	m_DlgEnemy.MoveWindow ( rect.Width (), 65, rect.Width (), rect.Height () ) ;

	m_DlgExeConfirm.Create ( IDD_DIALOG_EXECONFIRM, CDlgMain::GetParent () ) ;
	m_DlgExeConfirm.GetWindowRect ( rect ) ;
	m_DlgExeConfirm.MoveWindow ( CMyBitmap::GetScreenWidth () / 2 - rect.Width () / 2,
		CMyBitmap::GetScreenHeight () / 2 - rect.Height () / 2, rect.Width (), rect.Height (), true ) ;
	m_DlgExercise.Create ( IDD_DIALOG_EXERCISE, CDlgMain::GetParent () ) ;
	m_DlgExercise.GetWindowRect ( rect ) ;
	m_DlgExercise.MoveWindow ( CMyBitmap::GetScreenWidth () / 2 - rect.Width () / 2, 100, rect.Width (), rect.Height (), false ) ;

	CRect Drect ;
	CDlgMain::GetWindowRect ( Drect ) ;
	m_ExpPrg.GetWindowRect ( rect ) ;
	m_rcTip[0] = CRect ( rect.left - Drect.left, rect.top - Drect.top - 2, rect.right - Drect.left, rect.bottom - Drect.top + 2 ) ;
	m_RunChk.GetWindowRect ( rect ) ;
	m_rcTip[1] = CRect ( rect.left - Drect.left, rect.top - Drect.top, rect.right - Drect.left, rect.bottom - Drect.top ) ;
	m_rcTip[2] = CRect ( 0, 0, 0, 0 ) ;
	m_MapChk.GetWindowRect ( rect ) ;
	m_rcTip[3] = CRect ( rect.left - Drect.left, rect.top - Drect.top, rect.right - Drect.left, rect.bottom - Drect.top ) ;
	m_ScreenMoveChk.GetWindowRect ( rect ) ;
	m_rcTip[4] = CRect ( rect.left - Drect.left, rect.top - Drect.top, rect.right - Drect.left, rect.bottom - Drect.top ) ;
	m_StandChk.GetWindowRect ( rect ) ;
	m_rcTip[5] = CRect ( rect.left - Drect.left, rect.top - Drect.top, rect.right - Drect.left, rect.bottom - Drect.top ) ;

	CFile file ;
	file.Open ( "Ini/ProgressXp.rgn", CFile::modeRead ) ;
	if ( ( HANDLE ) file != NULL )
	{
		DWORD dwSize ;
		file.Read ( &dwSize, 4 ) ;
		BYTE* pData = new BYTE[dwSize] ;
		file.Read ( pData, dwSize ) ;
		file.Close () ;
		m_pRgnTip[0] = CRgn::FromHandle ( ::ExtCreateRegion ( NULL, dwSize, ( RGNDATA* ) pData ) ) ;
		delete [] pData ;
	}
	file.Open ( "Ini/ProgressMp.rgn", CFile::modeRead ) ;
	if ( ( HANDLE ) file != NULL )
	{
		DWORD dwSize ;
		file.Read ( &dwSize, 4 ) ;
		BYTE* pData = new BYTE[dwSize] ;
		file.Read ( pData, dwSize ) ;
		file.Close () ;
		m_pRgnTip[1] = CRgn::FromHandle ( ::ExtCreateRegion ( NULL, dwSize, ( RGNDATA* ) pData ) ) ;
		delete [] pData ;
	}
	file.Open ( "Ini/ProgressHp.rgn", CFile::modeRead ) ;
	if ( ( HANDLE ) file != NULL )
	{
		DWORD dwSize ;
		file.Read ( &dwSize, 4 ) ;
		BYTE* pData = new BYTE[dwSize] ;
		file.Read ( pData, dwSize ) ;
		file.Close () ;
		m_pRgnTip[2] = CRgn::FromHandle ( ::ExtCreateRegion ( NULL, dwSize, ( RGNDATA* ) pData ) ) ;
		delete [] pData ;
	}

	m_bCreate = true ;
	return TRUE;
}

HBRUSH CDlgMain::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr;
}

void CDlgMain::Show()
{
	CDlgMain::SetProgressRange () ;

	if ( !m_DlgXp.m_bShow )
	{
		if ( g_objHero.TestStatus ( USERSTATUS_FLY ) )
			m_DlgXp.SetFly ( true ) ;
	}
	if ( m_DlgXp.m_bShow )
	{
		if ( !g_objHero.TestStatus ( USERSTATUS_FLY ) && m_DlgXp.m_bFly )
			m_DlgXp.SetFly ( false ) ;
	}

	if ( g_objHero.m_objBoothManager.BeShowCloseDialog () )
	{
		if ( !m_DlgXp.m_bShow || ( m_DlgXp.m_bTransform && m_DlgXp.m_btBoothType != 2 ) )
		{
			m_DlgKongfu.ShowWindow ( SW_HIDE ) ;
			m_DlgMagic.ShowWindow ( SW_HIDE ) ;
			m_DlgXp.SetBooth ( false ) ;
		}
	}
	if ( !g_objHero.m_objBoothManager.BeShowOpenDialog () && !g_objHero.m_objBoothManager.BeShowCloseDialog () )
	{
		if ( m_DlgXp.m_bShow && m_DlgXp.m_btBoothType != 0 )
		{
			m_DlgXp.m_btBoothType = 0 ;
			m_DlgXp.ShowWindow ( SW_HIDE ) ;
		}
	}

	if ( !m_bCreate )
		return ;

	IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
												  "Progress45",
												  EXIGENCE_IMMEDIATE ) ;
	if ( ShowAni != NULL )
		ShowAni->Show ( 0, m_DlgPnt.x, m_DlgPnt.y ) ;
	m_HpPrg.Show ( m_DlgPnt.x, m_DlgPnt.y, g_objHero.GetData ( CPlayer::_PLAYER_LIFE ), g_objHero.GetData ( CPlayer::_PLAYER_CURRENTLIFE ) ) ;
	m_MpPrg.Show ( m_DlgPnt.x, m_DlgPnt.y, g_objHero.GetMana (), g_objHero.GetCurMana () ) ;
	m_ForcePrg.Show ( m_DlgPnt.x, m_DlgPnt.y, g_objHero.GetPhysicalForce (), g_objHero.GetPhysicalForce () ) ;
	if ( g_objHero.IsBlessed () && g_objHero.GetPhysicalForce () >= 100 )
	{
		m_BlessForcePrg.ResetValue ( 0, 50 ) ;
		m_BlessForcePrg.Show ( m_DlgPnt.x, m_DlgPnt.y, g_objHero.GetPhysicalForce () - 100, g_objHero.GetPhysicalForce () - 100 ) ;
	}

	ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
											"Dialog4",
											EXIGENCE_IMMEDIATE ) ;
	if ( ShowAni == NULL )
		return ;
	ShowAni->ShowEx ( 0, m_DlgPnt.x, m_DlgPnt.y - 3, CRect ( 0, 112, 256, 256 ), 256, 144 ) ;
	ShowAni->ShowEx ( 0, m_DlgPnt.x + 256, m_DlgPnt.y + 88, CRect ( 0, 0, 256, 54 ), 256, 54 ) ;
	ShowAni->ShowEx ( 1, m_DlgPnt.x + 512, m_DlgPnt.y + 88, CRect ( 0, 0, 256, 54 ), 256, 54 ) ;
	ShowAni->ShowEx ( 1, m_DlgPnt.x + 768, m_DlgPnt.y + 88, CRect ( 0, 64, 256, 118 ), 256, 54 ) ;
	m_XpPrg.Show ( m_DlgPnt.x, m_DlgPnt.y, g_objHero.GetXP (), g_objHero.GetXP () ) ;
	m_ExpPrg.Show ( m_DlgPnt.x, m_DlgPnt.y - 1, g_objHero.GetExp (), g_objHero.GetExp () ) ;
	m_GoodBox.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;

	m_QueryBtn.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;
	m_LevWordBtn.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;
	m_GoodBtn.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;
	m_SetupBtn.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;
	m_ActBtn.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;
	m_ChatBtn.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;
	m_GroupBtn.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;
	if ( m_bPkFlash )
	{
		static DWORD dwPkTime = TimeGet () ;
		if ( dwPkTime == 0 )
			dwPkTime = TimeGet () ;
		if ( ( ( TimeGet () - dwPkTime ) / 500 ) % 2 == 1 )
			m_PkBtn.SetCurFrame ( 1 ) ;
		else
			m_PkBtn.SetCurFrame ( 0 ) ;
		if ( TimeGet () - dwPkTime > 30000 )
		{
			m_bPkFlash = false ;
			m_PkBtn.SetCurFrame ( 0 ) ;
			dwPkTime = 0 ;
		}
	}
	m_PkBtn.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;
	if ( m_bFactionRead )
	{
		static DWORD dwFactionTime = TimeGet () ;
		if ( ( ( TimeGet () - dwFactionTime ) / 500 ) % 2 == 1 )
			m_FactionBtn.SetCurFrame ( 2 ) ;
		else
			m_FactionBtn.SetCurFrame ( 1 ) ;
	}
	m_FactionBtn.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;
	if ( m_bLevRead )
	{
		static DWORD dwLevTime = TimeGet () ;
		if ( ( ( TimeGet () - dwLevTime ) / 500 ) % 2 == 1 )
			m_FriendBtn.SetCurFrame ( 2 ) ;
		else
			m_FriendBtn.SetCurFrame ( 1 ) ;
	}
	m_FriendBtn.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;

	m_RunChk.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;
	m_MapChk.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;
	m_ScreenMoveChk.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;
	m_StandChk.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;
	m_MagicImg.Show ( m_DlgPnt.x, m_DlgPnt.y ) ;

	ShowChildDlg () ;

	if ( m_bItemAct )
	{
		if ( m_dwLastActTime == 0 )
		{
			IAni *pActAni = GameDataSetQuery()->GetDataAni ( "ani/effect.ani",
														  "ItemUse",
														  EXIGENCE_IMMEDIATE ) ;
			pActAni->Show ( 0, m_ActPnt.x, m_ActPnt.y ) ;
			m_dwLastActTime = TimeGet () ;
		}
		else
		{
			int nFrame = ( TimeGet () - m_dwLastActTime ) / 50 ;
			if ( nFrame < m_nActAmount )
			{
				IAni *pActAni = GameDataSetQuery()->GetDataAni ( "ani/effect.ani",
															  "ItemUse",
															  EXIGENCE_IMMEDIATE ) ;
				pActAni->Show ( nFrame, m_ActPnt.x, m_ActPnt.y ) ;
			}
			else
				m_bItemAct = false ;
		}
	}

	if ( m_bXpAct )
	{
		if ( m_dwLastXpTime == 0 )
		{
			m_XpPrg.SetShowMode ( 2 ) ;
			m_dwLastXpTime = TimeGet () ;
			return ;
		}
		if ( m_dwLastXpTime + 500 <= TimeGet () )
		{
			m_XpPrg.SetShowMode ( 0 ) ;
			m_bXpAct = false ;
		}
	}
}

void CDlgMain::ShowChildDlg()
{
	switch ( m_uCurDlgID )
	{
	case DLG_QUERY:
		if ( m_DlgQuery.m_bShow )
			m_DlgQuery.Show () ;
		break ;
	case DLG_FRIEND:
		if ( m_DlgFriend.m_bShow )
			m_DlgFriend.Show () ;
		if ( m_DlgEnemy.m_bShow )
			m_DlgEnemy.Show () ;
		break ;
	case DLG_QUERY_FACTION:
		if ( m_DlgQuery_Faction.m_bShow )
			m_DlgQuery_Faction.Show () ;
		break ;
	case DLG_NPCBUY:
		if ( m_DlgNpcBuy.m_bShow )
			m_DlgNpcBuy.Show () ;
		break ;
	case DLG_TRADE:
		if ( m_DlgTrade.m_bShow )
			m_DlgTrade.Show () ;
		break ;
	case DLG_DEPOT:
		if ( m_DlgDepot.m_bShow )
			m_DlgDepot.Show () ;
		break ;
	case DLG_IMPROVE:
		if ( m_DlgImprove.m_bShow )
			m_DlgImprove.Show () ;
		break ;
	case DLG_NPCFACE:
		if ( m_DlgNpcFace.m_bShow )
			m_DlgNpcFace.Show () ;
		break ;
	case DLG_EMBED:
		if ( m_DlgEmbed.m_bShow )
			m_DlgEmbed.Show () ;
		break ;
	case DLG_MPCBOOTH:
		if ( m_DlgMpcBooth.m_bShow )
			m_DlgMpcBooth.Show () ;
		break ;
	case DLG_NPCBOOTH:
		if ( m_DlgNpcBooth.m_bShow )
			m_DlgNpcBooth.Show () ;
		break ;
	case DLG_EFFIGY:
		if ( m_DlgEffigy.m_bShow )
			m_DlgEffigy.Show () ;
		break ;
	case DLG_LAY:
		if ( m_DlgLay.m_bShow )
			m_DlgLay.Show () ;
		break ;
	case DLG_EXP:
		if ( m_DlgCompose.m_bShow )
			m_DlgCompose.Show () ;
		break ;
	case DLG_FUSE:
		if ( m_DlgFuse.m_bShow )
			m_DlgFuse.Show () ;
		break ;
	case DLG_ENCHANT:
		if ( m_DlgEnchant.m_bShow )
			m_DlgEnchant.Show () ;
		break ;
	default:
		break ;
	}
	if ( m_DlgMpcGoods.m_bShow )
		m_DlgMpcGoods.Show () ;
	else if ( m_DlgNpcEquip.m_bShow )
		m_DlgNpcEquip.Show () ;
	if ( m_DlgSetup.m_bShow )
		m_DlgSetup.Show () ;
	if ( m_DlgHotkey.m_bShow )
		m_DlgHotkey.Show () ;
	if ( m_DlgAct.m_bShow )
		m_DlgAct.Show () ;
	if ( m_DlgGroup.m_bShow )
		m_DlgGroup.Show () ;
	if ( m_DlgBag.m_bShow )
		m_DlgBag.Show () ;
	if ( m_DlgTalk.m_bShow )
		m_DlgTalk.Show () ;
	if ( m_DlgMagic.m_bShow )
		m_DlgMagic.Show () ;
	if ( m_DlgXp.m_bShow )
		m_DlgXp.Show () ;
	if ( m_DlgKongfu.m_bShow )
		m_DlgKongfu.Show () ;
	if ( m_DlgPk.m_bShow )
		m_DlgPk.Show () ;
	if ( m_DlgMsgSend.m_bShow )
		m_DlgMsgSend.Show () ;
	if ( m_DlgMsgReceive.m_bShow )
		m_DlgMsgReceive.Show () ;
	if ( m_DlgHawk.m_bShow )
		m_DlgHawk.Show () ;
	if ( m_DlgChat.m_bShow )
		m_DlgChat.Show () ;
	if ( m_DlgBlack.m_bShow )
		m_DlgBlack.Show () ;
	if ( m_DlgLevWord.m_bShow )
		m_DlgLevWord.Show () ;
	if ( m_DlgOther.m_bShow )
		m_DlgOther.Show () ;
	if ( m_DlgDice.m_bShow )
		m_DlgDice.Show () ;
	if ( m_DlgDiceList.m_bShow )
		m_DlgDiceList.Show () ;
	if ( m_DlgInsHelp.m_bShow )
		m_DlgInsHelp.Show () ;
	if ( m_DlgExeConfirm.m_bShow )
		m_DlgExeConfirm.Show () ;
	if ( m_DlgExercise.m_bShow )
		m_DlgExercise.Show () ;
}

void CDlgMain::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_DlgPnt.x = x ;
 	m_DlgPnt.y = y ;
}

void CDlgMain::OnMainQuery()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	if ( m_uCurDlgID == -1 || m_uCurDlgID == DLG_FRIEND || m_uCurDlgID == DLG_QUERY_FACTION || m_uCurDlgID == DLG_QUERY_SKILL )
	{
		CDlgMain::CloseCurDlg () ;
		m_DlgQuery.ShowWindow ( SW_SHOW ) ;
		m_uCurDlgID = DLG_QUERY ;
	}
	else if ( m_uCurDlgID == DLG_QUERY )
	{
		if ( !m_DlgQuery.m_bShow )
			m_DlgQuery.ShowWindow ( SW_SHOW ) ;
		else if ( m_DlgQuery.m_bShow )
		{
			m_DlgQuery.ShowWindow ( SW_HIDE ) ;
			m_uCurDlgID = -1 ;
		}
	}
}

void CDlgMain::OnMainFriend()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	if ( m_uCurDlgID == -1 || m_uCurDlgID == DLG_QUERY || m_uCurDlgID == DLG_QUERY_FACTION || m_uCurDlgID == DLG_QUERY_SKILL || m_uCurDlgID == DLG_EFFIGY )
	{
		CDlgMain::CloseCurDlg () ;
		m_DlgFriend.ShowWindow ( SW_SHOW ) ;
		m_uCurDlgID = DLG_FRIEND ;
		if ( m_bLevRead )
		{
			CDlgMain::CloseOtherDlg ( true ) ;
			m_DlgMsgReceive.ShowWindow ( SW_SHOW ) ;
			m_bLevRead = false ;
		}
	}
	else if ( m_uCurDlgID == DLG_FRIEND )
	{
		if ( !m_DlgFriend.m_bShow )
		{
			if ( !m_DlgEnemy.m_bShow )
				m_DlgFriend.ShowWindow ( SW_SHOW ) ;
			else
				m_DlgEnemy.ShowWindow ( SW_HIDE ) ;
		}
		else if ( m_DlgFriend.m_bShow )
		{
			m_DlgFriend.ShowWindow ( SW_HIDE ) ;
			if ( m_DlgFriend.m_bShow )
				m_DlgEnemy.ShowWindow ( SW_HIDE ) ;
			m_uCurDlgID = -1 ;
		}
	}
}

void CDlgMain::OnMainLevword()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	if ( m_DlgLevWord.m_bShow )
		m_DlgLevWord.ShowWindow ( SW_HIDE ) ;
	else
	{
		CDlgMain::CloseOtherDlg ( true ) ;
		m_DlgLevWord.ShowWindow ( SW_SHOW ) ;
	}
}

void CDlgMain::OnMainGood()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	if ( !m_DlgMpcGoods.m_bShow )
	{
		if ( m_DlgNpcEquip.m_bShow )
			m_DlgNpcEquip.ShowWindow ( SW_HIDE ) ;
		if ( m_DlgDice.m_bShow )
			g_objHero.m_objDiceManager.Close ( true ) ;
		m_DlgMpcGoods.ShowWindow ( SW_SHOW ) ;
		return ;
	}
	else
	{
		m_DlgMpcGoods.ShowWindow ( SW_HIDE ) ;
		return ;
	}
}

void CDlgMain::OnMainSetup()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	if ( !m_DlgSetup.m_bShow )
	{
		if ( m_DlgHotkey.m_bShow )
			m_DlgHotkey.ShowWindow ( SW_HIDE ) ;
		CDlgMain::CloseOtherDlg ( true ) ;
		m_DlgSetup.ShowWindow ( SW_SHOW ) ;
		return ;
	}
	else
	{
		m_DlgSetup.OnSetupBtnReturn () ;
		return ;
	}
}

void CDlgMain::OnMainAct()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	if ( !m_DlgAct.m_bShow )
		m_DlgAct.ShowWindow ( SW_SHOW ) ;
	else
		m_DlgAct.ShowWindow ( SW_HIDE ) ;
}

void CDlgMain::OnMainChat()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	CDlgMain::GetParent()->PostMessage( WM_MY_MESSAGE, ON_MAIN_TRADE ) ;
}

void CDlgMain::OnMainGroup()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	if ( !m_DlgGroup.m_bShow )
	{
		if ( m_DlgBag.m_bShow )
			m_DlgBag.ShowWindow ( SW_HIDE ) ;
		m_DlgGroup.ShowWindow ( SW_SHOW ) ;
		return ;
	}
	else
	{
		m_DlgGroup.ShowWindow ( SW_HIDE ) ;
		return ;
	}
}

void CDlgMain::CloseCurDlg()
{
	if ( m_uCurDlgID == -1 )
		return ;

	switch ( m_uCurDlgID )
	{
	case DLG_QUERY:
		m_DlgQuery.ShowWindow ( SW_HIDE ) ;
		break ;
	case DLG_FRIEND:
		m_DlgFriend.ShowWindow ( SW_HIDE ) ;
		m_DlgEnemy.ShowWindow ( SW_HIDE ) ;
		break ;
	case DLG_QUERY_FACTION:
		m_DlgQuery_Faction.ShowWindow ( SW_HIDE ) ;
		break ;
	case DLG_NPCBUY:
		m_DlgNpcBuy.ShowWindow ( SW_HIDE ) ;
		break ;
	case DLG_TRADE:
		m_DlgTrade.ShowWindow ( SW_HIDE ) ;
		break ;
	case DLG_DEPOT:
		m_DlgDepot.ShowWindow ( SW_HIDE ) ;
		break ;
	case DLG_IMPROVE:
		m_DlgImprove.ShowWindow ( SW_HIDE ) ;
		break ;
	case DLG_NPCFACE:
		m_DlgNpcFace.ShowWindow ( SW_HIDE ) ;
		break ;
	case DLG_EMBED:
		m_DlgEmbed.ShowWindow ( SW_HIDE ) ;
		break ;
	case DLG_MPCBOOTH:
		m_DlgMpcBooth.ShowWindow ( SW_HIDE ) ;
		break ;
	case DLG_NPCBOOTH:
		m_DlgNpcBooth.ShowWindow ( SW_HIDE ) ;
		break ;
	case DLG_EFFIGY:
		m_DlgEffigy.ShowWindow ( SW_HIDE ) ;
		break ;
	case DLG_LAY:
		m_DlgLay.ShowWindow ( SW_HIDE ) ;
		break ;
	case DLG_EXP:
		m_DlgCompose.ShowWindow ( SW_HIDE ) ;
		break ;
	case DLG_FUSE:
		m_DlgFuse.ShowWindow ( SW_HIDE ) ;
		break ;
	case DLG_ENCHANT:
		m_DlgEnchant.ShowWindow ( SW_HIDE ) ;
		break ;
	default:
		break ;
	}
	m_uCurDlgID = -1 ;
}

BOOL CDlgMain::IsCurDlgShow()
{
	if ( m_uCurDlgID == -1 )
		return false ;
	switch ( m_uCurDlgID )
	{
	case DLG_QUERY:
		if ( m_DlgQuery.m_bShow )
			return true ;
		break ;
	case DLG_FRIEND:
		if ( m_DlgFriend.m_bShow )
			return true ;
		if ( m_DlgEnemy.m_bShow )
			return true ;
		break ;
	case DLG_QUERY_FACTION:
		if ( m_DlgQuery_Faction.m_bShow )
			return true ;
		break ;
	case DLG_NPCBUY:
		if ( m_DlgNpcBuy.m_bShow )
			return true ;
		break ;
	case DLG_TRADE:
		if ( m_DlgTrade.m_bShow )
			return true ;
		break ;
	case DLG_DEPOT:
		if ( m_DlgDepot.m_bShow )
			return true ;
		break ;
	case DLG_IMPROVE:
		if ( m_DlgImprove.m_bShow )
			return true ;
		break ;
	case DLG_NPCFACE:
		if ( m_DlgNpcFace.m_bShow )
			return true ;
		break ;
	case DLG_EMBED:
		if ( m_DlgEmbed.m_bShow )
			return true ;
		break ;
	case DLG_MPCBOOTH:
		if ( m_DlgMpcBooth.m_bShow )
			return true ;
		break ;
	case DLG_NPCBOOTH:
		if ( m_DlgNpcBooth.m_bShow )
			return true ;
		break ;
	case DLG_EFFIGY:
		if ( m_DlgEffigy.m_bShow )
			return true ;
		break ;
	case DLG_LAY:
		if ( m_DlgLay.m_bShow )
			return true ;
		break ;
	case DLG_EXP:
		if ( m_DlgCompose.m_bShow )
			return true ;
		break ;
	case DLG_FUSE:
		if ( m_DlgFuse.m_bShow )
			return true ;
		break ;
	case DLG_ENCHANT:
		if ( m_DlgEnchant.m_bShow )
			return true ;
		break ;
	default:
		break ;
	}
	return false ;
}

void CDlgMain::OnMainPk()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	if ( m_DlgMagic.m_bShow )
	{
		m_DlgMagic.ShowWindow ( SW_HIDE ) ;
		m_DlgXp.ShowWindow ( SW_HIDE ) ;
		m_DlgKongfu.ShowWindow ( SW_HIDE ) ;
		m_DlgPk.ShowWindow ( SW_SHOW ) ;
		if ( m_DlgTalk.m_DlgTalkAct.m_bShow )
			m_DlgTalk.m_DlgTalkAct.ShowWindow ( SW_HIDE ) ;
		return ;
	}
	if ( m_DlgPk.m_bShow )
	{
		m_DlgPk.ShowWindow ( SW_HIDE ) ;
		return ;
	}
	m_DlgPk.ShowWindow ( SW_SHOW ) ;
	if ( m_DlgTalk.m_DlgTalkAct.m_bShow )
		m_DlgTalk.m_DlgTalkAct.ShowWindow ( SW_HIDE ) ;
}

void CDlgMain::OnMainTalk()
{
}

void CDlgMain::OnMainMap()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	if ( g_objGameMap.m_bShowMiniMap )
		g_objGameMap.m_objRadar.SetShowState ( 2 ) ;
	else
		g_objGameMap.m_objRadar.SetShowState ( 1 ) ;
}

void CDlgMain::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( m_btCurTip == 0xFF )
	{
		int i ;
		for ( i = 0; i < 6; i++ )
		{
			if ( m_rcTip[i].PtInRect ( point ) )
			{
				m_btCurTip = ( BYTE ) i ;
				CDlgMain::GetParent()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_STATIC, i ) ;
				break ;
			}
		}
		for ( i = 0; i < 3; i++ )
		{
			if ( m_pRgnTip[i]->PtInRegion ( point ) )
			{
				m_btCurTip = ( BYTE ) ( i + 6 ) ;
				CDlgMain::GetParent()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_STATIC, i + 6 ) ;
				break ;
			}
		}
	}
	else if ( m_btCurTip < 6 )
	{
		if ( !m_rcTip[m_btCurTip].PtInRect ( point ) )
		{
			m_btCurTip = 0xFF ;
			CDlgMain::GetParent()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_HIDE, 0 ) ;
		}
	}
	else
	{
		if ( !m_pRgnTip[m_btCurTip - 6]->PtInRegion ( point ) )
		{
			m_btCurTip = 0xFF ;
			CDlgMain::GetParent()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_HIDE, 0 ) ;
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}

LRESULT CDlgMain::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case  WM_COMMAND:
		{
			if ( wParam == 1 || wParam == 2 )
				return true ;
		}
		break;
	case WM_DESTROY:
		{
			::DeleteObject ( m_Hbr ) ;
			::DeleteObject ( m_pRgnTip[0]->GetSafeHandle () ) ;
			::DeleteObject ( m_pRgnTip[1]->GetSafeHandle () ) ;
			::DeleteObject ( m_pRgnTip[2]->GetSafeHandle () ) ;
			CDlgMain::WriteRecord () ;
		}
		break;
	case WM_ERASEBKGND:
		return true ;
	case WM_NCPAINT:
		return false ;
	case WM_PAINT:
		{
			PAINTSTRUCT ps ;
			::BeginPaint ( m_hWnd, &ps ) ;
			::EndPaint ( m_hWnd, &ps ) ;
		}
		return false ;
	default:
		break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CDlgMain::SetProgressRange()
{
	m_HpPrg.ResetValue ( 0, g_objHero.GetMaxLife () ) ;
	DWORD dwLife = __min ( g_objHero.GetData ( CPlayer::_PLAYER_LIFE ), g_objHero.GetMaxLife () ) ;
	m_MpPrg.ResetValue ( 0, g_objHero.GetMaxMana () ) ;
	DWORD dwMana = __min ( g_objHero.GetMaxMana (), g_objHero.GetMana () ) ;
	if ( m_bNoMana && dwMana != 0 )
	{
		m_HpPrg.SetShowMode ( 0 ) ;
		m_bNoMana = false ;
	}
	m_ForcePrg.ResetValue ( 0, g_objHero.GetMaxPhysicalForce () ) ;
	m_XpPrg.ResetValue ( 0, g_objHero.GetMaxXP () ) ;
	m_ExpPrg.ResetValue ( 0, g_objHero.GetNextLevelExp () ) ;
}

void CDlgMain::OpenNpcShop(int nNpcType, OBJID idNpc)
{
	if ( m_DlgTrade.m_bShow )
		return ;
	CDlgMain::CloseCurDlg () ;
	if ( nNpcType != _ROLE_STORAGE_NPC )
	{
		m_DlgNpcBuy.OpenShop ( idNpc ) ;
		m_DlgNpcBuy.ShowWindow ( SW_SHOW ) ;
		if ( !m_DlgMpcGoods.m_bShow )
		{
			if ( m_DlgNpcEquip.m_bShow )
				m_DlgNpcEquip.ShowWindow ( SW_HIDE ) ;
			m_DlgMpcGoods.ShowWindow ( SW_SHOW ) ;
		}
		m_uCurDlgID = DLG_NPCBUY ;
	}
}

void CDlgMain::OpenDepot(int nNpcType, OBJID idNpc)
{
	if ( m_DlgTrade.m_bShow )
		return ;
	CDlgMain::CloseCurDlg () ;
	m_DlgDepot.OpenDepot ( idNpc ) ;
	if ( m_DlgBag.m_bShow )
		m_DlgBag.ShowWindow ( SW_HIDE ) ;
	m_DlgDepot.ShowWindow ( SW_SHOW ) ;
	if ( !m_DlgMpcGoods.m_bShow )
	{
		if ( m_DlgNpcEquip.m_bShow )
			m_DlgNpcEquip.ShowWindow ( SW_HIDE ) ;
		m_DlgMpcGoods.ShowWindow ( SW_SHOW ) ;
	}
	m_uCurDlgID = DLG_DEPOT ;
}

void CDlgMain::OnShellEnter()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	m_DlgTalk.m_TalkDataCob.SetFocus () ;
}

void CDlgMain::PlayItemQUse(int nGridIndex)
{
	IAni *pActAni = GameDataSetQuery()->GetDataAni ( "ani/effect.ani",
												  "ItemUse",
												  EXIGENCE_IMMEDIATE ) ;
	if ( pActAni != NULL )
	{
		CRect Drect, rect ;
		CDlgMain::GetWindowRect ( Drect ) ;
		m_GoodBox.GetWindowRect ( rect ) ;
		m_bItemAct = true ;
		m_dwLastActTime = 0 ;
		m_ActPnt.x = rect.left - Drect.left + m_DlgPnt.x + ( nGridIndex - 1 ) * 41 - 60;
		m_ActPnt.y = m_DlgPnt.y + 30;
		m_nActAmount = pActAni->GetFrameAmount () ;
	}
}

void CDlgMain::OnMainScreenmove()
{
	CDlgMain::GetParent()->PostMessage( WM_MY_MESSAGE, ON_MAIN_SCREENMOVE, m_ScreenMoveChk.GetCheckValue () ) ;
}

void CDlgMain::OnMainMagic()
{
	int nAmount = g_objHero.GetMagicAmount () ;
	if ( nAmount > 0 )
	{
		if ( m_DlgPk.m_bShow )
			m_DlgPk.ShowWindow ( SW_HIDE ) ;
		else if ( m_DlgMagic.m_bShow )
		{
			m_DlgMagic.ShowWindow ( SW_HIDE ) ;
			goto KONGFU ;
		}
		if ( nAmount > 10 )
			m_DlgMagic.MoveWindow ( CMyBitmap::GetScreenWidth () - 518, CMyBitmap::GetScreenHeight () - 123, 518, 50 ) ;
		else
			m_DlgMagic.MoveWindow ( CMyBitmap::GetScreenWidth () - nAmount * 50 - 18, CMyBitmap::GetScreenHeight () - 123, nAmount * 50 + 18, 50 ) ;
		m_DlgMagic.InitMagicDialog () ;
	}
KONGFU:
	nAmount = g_objHero.GetKongfuAmount () ;
	if ( nAmount > 0 )
	{
		if ( m_DlgKongfu.m_bShow )
		{
			m_DlgKongfu.ShowWindow ( SW_HIDE ) ;
			return ;
		}
		if ( nAmount > 10 )
			m_DlgKongfu.MoveWindow ( CMyBitmap::GetScreenWidth () - 518, CMyBitmap::GetScreenHeight () - 223, 518, 50 ) ;
		else
			m_DlgKongfu.MoveWindow ( CMyBitmap::GetScreenWidth () - nAmount * 50 - 18, CMyBitmap::GetScreenHeight () - 223, nAmount * 50 + 18, 50 ) ;
		m_DlgKongfu.InitKongfuDialog () ;
	}
}

void CDlgMain::OnMainFaction()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	if ( m_uCurDlgID == -1 || m_uCurDlgID == DLG_QUERY || m_uCurDlgID == DLG_FRIEND || m_uCurDlgID == DLG_QUERY_SKILL )
	{
		CDlgMain::CloseCurDlg () ;
		m_DlgQuery_Faction.ShowWindow ( SW_SHOW ) ;
		m_uCurDlgID = DLG_QUERY_FACTION ;
		if ( m_bFactionRead )
		{
			m_DlgQuery_Faction.OnFactionBtnWord () ;
			m_bFactionRead = false ;
		}
	}
	else if ( m_uCurDlgID == DLG_QUERY_FACTION )
	{
		if ( !m_DlgQuery_Faction.m_bShow )
		{
			m_DlgQuery_Faction.ShowWindow ( SW_SHOW ) ;
			if ( m_bFactionRead )
			{
				m_DlgQuery_Faction.OnFactionBtnWord () ;
				m_bFactionRead = false ;
			}
		}
		else if ( m_DlgQuery_Faction.m_bShow )
		{
			m_DlgQuery_Faction.ShowWindow ( SW_HIDE ) ;
			m_uCurDlgID = -1 ;
		}
	}
}

void CDlgMain::WriteRecord()
{
	if ( strlen ( g_objHero.GetName () ) == 0 || m_strOwnerIni[0] == '\0' )
		return ;
	if ( strlen ( m_strOwnerIni ) == 0 )
		return ;

	char strAppName[32] ;
	sprintf ( strAppName, "%sRecord", g_objHero.GetName () ) ;

	char* pstrTemp = strchr ( strAppName, '[' ) ;
	if ( pstrTemp != NULL )
	{
		strncpy ( pstrTemp, "(", 1 ) ;
		pstrTemp = NULL ;
	}
	pstrTemp = strchr ( strAppName, ']' ) ;
	if ( pstrTemp != NULL )
	{
		strncpy ( pstrTemp, ")", 1 ) ;
		pstrTemp = NULL ;
	}

	char strHash[64] ;
	_i64toa ( ( unsigned __int64 ) HashName ( g_objHero.GetName () ), strHash, 10 ) ;
	sprintf ( m_strOwnerIni, "Log/%s/%s/Setup.ini", g_szServerName, strHash ) ;

	WritePrivateProfileString ( strAppName, "RecordAmount", "1", m_strOwnerIni ) ;

	char strMode[16];
	itoa ( CRole::s_nShadowType, strMode, 10 );
	WritePrivateProfileString ( strAppName, "ShadowRecord", strMode, m_strOwnerIni ) ;

	itoa ( m_RunChk.GetCheckValue (), strMode, 10 ) ;
	WritePrivateProfileString ( strAppName, "RunModeRecord", strMode, m_strOwnerIni ) ;
	itoa ( m_MapChk.GetCheckValue (), strMode, 10 ) ;
	WritePrivateProfileString ( strAppName, "MapModeRecord", strMode, m_strOwnerIni ) ;
	itoa ( m_TalkChk.GetCheckValue (), strMode, 10 ) ;
	WritePrivateProfileString ( strAppName, "TalkModeRecord", strMode, m_strOwnerIni ) ;
	itoa ( g_objHero.GetPkMode (), strMode, 10 ) ;
	WritePrivateProfileString ( strAppName, "PkModeRecord", strMode, m_strOwnerIni ) ;

	char strTypeID[32] ;
	char strKeyName[16] ;
	int i=0;
	for ( i = 1; i <= GRID_ICONQUICK_NUM; i++ )
	{
		sprintf ( strKeyName, "QItem%dRecord", i ) ;
		if ( m_GoodBox.GetGridAble ( CPoint ( i, 1 ) ) )
		{
			switch ( m_GoodBox.GetIconType ( i - 1 ) )
			{
			case ICON_TYPE_ITEM:
				itoa ( m_GoodBox.GetIconTypeID ( i - 1 ) * 10 + ICON_TYPE_ITEM, strTypeID, 10 ) ;
				break ;
			case ICON_TYPE_ACTION:
				itoa ( m_GoodBox.GetIconTypeID ( i - 1 ) * 10 + ICON_TYPE_ACTION, strTypeID, 10 ) ;
				break ;
			case ICON_TYPE_SKILL:
				itoa ( m_GoodBox.GetIconTypeID ( i - 1 ) * 10 + ICON_TYPE_SKILL, strTypeID, 10 ) ;
				break ;
			case ICON_TYPE_XP:
				itoa ( m_GoodBox.GetIconTypeID ( i - 1 ) * 10 + ICON_TYPE_XP, strTypeID, 10 ) ;
				break ;
			default:
				continue ;
			}
			WritePrivateProfileString ( strAppName, strKeyName, strTypeID, m_strOwnerIni ) ;
		}
		else
			WritePrivateProfileString ( strAppName, strKeyName, "", m_strOwnerIni ) ;
	}

	int nCount = m_DlgTalk.m_DlgListBox.m_ListBox.GetCount () ;

	if ( nCount > 0 )
	{
		char strRecord[17*18] ;
		char strTemp[16] ;
		itoa ( nCount, strTemp, 10 ) ;
		strcpy ( strRecord, strTemp ) ;
		strcat ( strRecord, "|" ) ;
		for ( int i = 0; i < __min(16, nCount); i++ )
		{
			m_DlgTalk.m_DlgListBox.m_ListBox.GetText ( i, strTemp ) ;
			strcat ( strRecord, strTemp ) ;
			strcat ( strRecord, "|" ) ;
		}

		WritePrivateProfileString ( strAppName, "TalkListRecord", strRecord, m_strOwnerIni ) ;
	}
	else
		WritePrivateProfileString ( strAppName, "TalkListRecord", "", m_strOwnerIni ) ;

	char strInt[8] ;
	int nValue = m_DlgSetup.m_SetupSliMusic.GetSliderValue () ;

	itoa ( nValue, strInt, 10 ) ;
	WritePrivateProfileString( strAppName, "MusicRecord", strInt, m_strOwnerIni ) ;

	nValue = m_DlgSetup.m_SetupSliEffect.GetSliderValue () ;
	itoa ( nValue, strInt, 10 ) ;
	WritePrivateProfileString( strAppName, "EffectRecord", strInt, m_strOwnerIni ) ;

	nValue = 0;
	char strText [256] ;
	strcpy ( strText, g_objHero.GetSyndicateAnnounce () ) ;
	if ( strcmp ( strText, "NULL" ) && strText[0] != '\0' )
	{
		for ( i = 0; i < strlen ( strText ); i++ )
			nValue += abs ( strText[i] ) ;
		itoa ( nValue, strInt, 10 ) ;
		WritePrivateProfileString ( strAppName, "FacWordRecord", strInt, m_strOwnerIni ) ;
	}

	itoa ( m_DlgSetup.m_SetupChkNameopen.GetCheckValue (), strInt, 10 ) ;
	WritePrivateProfileString ( strAppName, "ShiftRecord", strInt, m_strOwnerIni ) ;

	itoa ( m_DlgSetup.m_SetupChkKoShow.GetCheckValue (), strInt, 10 ) ;
	WritePrivateProfileString ( strAppName, "KoShowRecord", strInt, m_strOwnerIni ) ;

	for ( i = 0; i < 6; i++ )
		strInt[i] = m_DlgChat.m_nColorLog [i] + 49;
	strInt[6] = '\0' ;
	WritePrivateProfileString ( strAppName, "ChatColorRecord", strInt, m_strOwnerIni ) ;

	itoa ( m_DlgSetup.m_nInterface, strInt, 10 ) ;
	WritePrivateProfileString ( "Interface", "InterfaceRecord", strInt, g_strSetupIni ) ;
}

void CDlgMain::SetRecord( const char* strIniFile )
{
	if ( strIniFile != NULL )
		strcpy ( m_strOwnerIni, strIniFile ) ;
	else
		return ;

	char strAppName[32] ;
	sprintf ( strAppName, "%sRecord", g_objHero.GetName () ) ;

	char* pstrTemp = strchr ( strAppName, '[' ) ;
	if ( pstrTemp != NULL )
	{
		strncpy ( pstrTemp, "(", 1 ) ;
		pstrTemp = NULL ;
	}
	pstrTemp = strchr ( strAppName, ']' ) ;
	if ( pstrTemp != NULL )
	{
		strncpy ( pstrTemp, ")", 1 ) ;
		pstrTemp = NULL ;
	}

	int nReturn = GetPrivateProfileInt ( strAppName, "RecordAmount", -1, m_strOwnerIni ) ;
	if ( nReturn != -1 )
	{
		int nMusic = GetPrivateProfileInt ( strAppName, "MusicRecord", -1, m_strOwnerIni ) ;
		if ( nMusic != -1 )
		{
			m_DlgSetup.m_SetupSliMusic.SetSliderValue ( nMusic ) ;
			DXSetMusicVolume ( nMusic ) ;
		}
		else
		{
			m_DlgSetup.m_SetupSliMusic.SetSliderValue ( 80 ) ;
			DXSetMusicVolume ( 80 ) ;
		}
		nMusic = GetPrivateProfileInt ( strAppName, "EffectRecord", -1, m_strOwnerIni ) ;
		if ( nMusic != -1 )
		{
			m_DlgSetup.m_SetupSliEffect.SetSliderValue ( nMusic ) ;
			DXSetSoundVolume ( nMusic ) ;
		}
		else
		{
			m_DlgSetup.m_SetupSliEffect.SetSliderValue ( 80 ) ;
			DXSetSoundVolume ( 80 ) ;
		}

		nReturn = GetPrivateProfileInt ( strAppName, "ShadowRecord", -1, m_strOwnerIni ) ;
		if ( nReturn != -1 )
			CRole::s_nShadowType = nReturn ;

		nReturn = GetPrivateProfileInt ( strAppName, "RunModeRecord", -1, m_strOwnerIni ) ;
		if ( nReturn == 1 )
		{
			m_RunChk.SetCheckValue ( 1 ) ;
			CDlgMain::OnMainRun () ;
		}
		nReturn = GetPrivateProfileInt ( strAppName, "MapModeRecord", -1, m_strOwnerIni ) ;
		if ( nReturn == 1 )
		{
			m_MapChk.SetCheckValue ( 1 ) ;
			CDlgMain::OnMainMap () ;
		}
		nReturn = GetPrivateProfileInt ( strAppName, "TalkModeRecord", -1, m_strOwnerIni ) ;
		if ( nReturn == 0 )
		{
			m_TalkChk.SetCheckValue ( 0 ) ;
			CDlgMain::OnMainTalk () ;
		}

		nReturn = GetPrivateProfileInt ( strAppName, "PkModeRecord", -1, m_strOwnerIni ) ;
		if ( nReturn != -1 )
			g_objHero.SetPkMode ( PKMODE_ARRESTMENT ) ;
		else
			g_objHero.SetPkMode ( PKMODE_ARRESTMENT ) ;

		nReturn = GetPrivateProfileInt ( strAppName, "ShiftRecord", -1, m_strOwnerIni ) ;
		if ( nReturn != -1 && nReturn != 0 )
		{
			m_DlgSetup.m_SetupChkNameopen.SetCheckValue ( 1 ) ;
			m_DlgSetup.OnSetupChkNameopen () ;
		}

		nReturn = GetPrivateProfileInt ( strAppName, "KoShowRecord", -1, m_strOwnerIni ) ;
		if ( nReturn != -1 )
		{
			g_bKoShowRecord = nReturn ;
			if ( nReturn != 0 )
			{
				m_DlgSetup.m_SetupChkKoShow.SetCheckValue ( 1 ) ;
				m_DlgSetup.OnSetupChkKoshow () ;
			}
		}
		else
		{
			m_DlgSetup.m_SetupChkKoShow.SetCheckValue ( 1 ) ;
			m_DlgSetup.OnSetupChkKoshow () ;
		}
		nReturn = GetPrivateProfileInt ( strAppName, "ChatColorRecord", -1, m_strOwnerIni ) ;
		if ( nReturn != -1 )
		{
			int nTemp ;
			char strTemp[16] ;
			nTemp = nReturn / 100000 ;
			if ( nTemp > 0 )
			{
				nReturn = nReturn % 100000 ;
				nTemp = nTemp - 1 ;
				m_DlgChat.m_nColorLog [0] = nTemp ;
				g_objGameMsg.SetChannelColor ( _TXTATR_TALK, m_DlgChat.m_dwColor [nTemp] ) ;
				sprintf ( strTemp, "Image36%d", nTemp ) ;
				m_DlgChat.m_ChatImgGc.InsertImage ( strTemp ) ;
			}
			nTemp = nReturn / 10000 - 1 ;
			m_DlgChat.m_nColorLog [1] = nTemp ;
			g_objGameMsg.SetChannelColor ( _TXTATR_FRIEND, m_DlgChat.m_dwColor [nTemp] ) ;
			sprintf ( strTemp, "Image36%d", nTemp ) ;
			m_DlgChat.m_ChatImgHc.InsertImage ( strTemp ) ;

			nTemp = ( nReturn % 10000 ) / 1000 - 1 ;
			m_DlgChat.m_nColorLog [2] = nTemp ;
			g_objGameMsg.SetChannelColor ( _TXTATR_SYNDICATE, m_DlgChat.m_dwColor [nTemp] ) ;
			sprintf ( strTemp, "Image36%d", nTemp ) ;
			m_DlgChat.m_ChatImgBc.InsertImage ( strTemp ) ;

			nTemp = ( ( nReturn % 10000 ) % 1000 ) / 100 - 1 ;
			m_DlgChat.m_nColorLog [3] = nTemp ;
			g_objGameMsg.SetChannelColor ( _TXTATR_TEAM, m_DlgChat.m_dwColor [nTemp] ) ;
			sprintf ( strTemp, "Image36%d", nTemp ) ;
			m_DlgChat.m_ChatImgDc.InsertImage ( strTemp ) ;

			nTemp = ( ( ( nReturn % 10000 ) % 1000 ) % 100 ) / 10 - 1 ;
			m_DlgChat.m_nColorLog [4] = nTemp ;
			g_objGameMsg.SetChannelColor ( _TXTATR_PRIVATE, m_DlgChat.m_dwColor [nTemp] ) ;
			sprintf ( strTemp, "Image36%d", nTemp ) ;
			m_DlgChat.m_ChatImgSc.InsertImage ( strTemp ) ;

			nTemp = ( ( ( nReturn % 10000 ) % 1000 ) % 100 ) % 10 - 1 ;
			m_DlgChat.m_nColorLog [5] = nTemp ;
			g_objGameMsg.SetChannelColor ( _TXTATR_ACTION, m_DlgChat.m_dwColor [nTemp] ) ;
			sprintf ( strTemp, "Image36%d", nTemp ) ;
			m_DlgChat.m_ChatImgZc.InsertImage ( strTemp ) ;
		}

		int nMode = GetPrivateProfileInt ( "Interface", "InterfaceRecord", -1, g_strSetupIni ) ;
		if ( nMode != -1 )
		{
			m_DlgSetup.m_nInterface = nMode ;
			char strTemp[32] ;
			sprintf ( strTemp, "Button101%d", nMode + 1 ) ;
			m_DlgSetup.m_SetupBtnMode.ChangeImage ( strTemp ) ;
			m_DlgSetup.OnSetupBtnMode () ;
		}

		int nItemTypeID ;
		char strKeyName[16] ;
		for ( int i = 1; i <= GRID_ICONQUICK_NUM; i++ )
		{
			if ( m_GoodBox.GetGridAble ( CPoint ( i, 1 ) ) )
				break ;
			sprintf ( strKeyName, "QItem%dRecord", i ) ;
			nItemTypeID = GetPrivateProfileInt ( strAppName, strKeyName, -1, m_strOwnerIni ) ;
			if ( nItemTypeID != -1 )
			{
				switch ( nItemTypeID % 10 )
				{
				case ICON_TYPE_ITEM:
					{
						for ( int j = 0; j < g_objHero.GetItemAmount (); j++ )
						{
							CItem* pItem = g_objHero.GetItemByIndex ( j ) ;
							if ( pItem != NULL )
							{
								if ( pItem->GetTypeID () == nItemTypeID / 10 )
								{
									m_GoodBox.InsertIcon( 1, i, nItemTypeID / 10, 0, ICON_TYPE_ITEM, 0, true ) ;
									break ;
								}
							}
						}
					}
					break ;
				case ICON_TYPE_ACTION:
					m_GoodBox.InsertIcon( 1, i, nItemTypeID / 10, 0, ICON_TYPE_ACTION, 0, false ) ;
					break ;
				case ICON_TYPE_SKILL:
					m_GoodBox.InsertIcon( 1, i, nItemTypeID / 10, 0, ICON_TYPE_SKILL, 0, false ) ;
					break ;
				case ICON_TYPE_XP:
					m_GoodBox.InsertIcon( 1, i, nItemTypeID / 10, 0, ICON_TYPE_XP, 0, false ) ;
					break ;
				}
			}
		}

		char strRecord[17*18] ;
		GetPrivateProfileString ( strAppName, "TalkListRecord", "", strRecord, 17 * 18, m_strOwnerIni ) ;
		if ( strRecord[0] != NULL && m_DlgTalk.m_DlgListBox.m_ListBox.GetCount () == 0 )
		{
			char* pTemp ;
			char* pLast ;
			char strTemp[16] ;
			int nAmount = 0 ;
			pTemp = strchr ( strRecord, '|' ) ;
			strncpy ( strTemp, strRecord, pTemp - strRecord ) ;
			strTemp[pTemp - strRecord] = NULL ;
			nAmount = atoi ( strTemp ) ;
			for ( int i = 0; i < __min(16, nAmount); i++ )
			{
				pLast = pTemp + 1 ;
				pTemp = strchr ( pTemp + 1, '|' ) ;
				strncpy ( strTemp, pLast, pTemp - pLast ) ;
				strTemp[pTemp - pLast] = NULL ;
				m_DlgTalk.m_DlgListBox.m_ListBox.AddString ( strTemp ) ;
			}
		}
	}
	else
	{
		::WritePrivateProfileStruct( strAppName,
									 NULL,
									 NULL,
								     0,
								     m_strOwnerIni ) ;
		::WritePrivateProfileSection ( strAppName,
									   " RecordAmount=1\0 MusicRecord=\0 EffectRecord=\0 PkModeRecord=\0 RunModeRecord=\0 MapModeRecord=\0 TalkModeRecord=\0 QItem1Record=\0 QItem2Record=\0 QItem3Record=\0 QItem4Record=\0 QItem5Record=\0 QItem6Record=\0 QItem7Record=\0 QItem8Record=\0 QItem9Record=\0 QItem10Record=\0 TalkListRecord=\0 ShadowRecord=\0 LineRecord=\0 LogRecord=\0 FacWordRecord=\0 ShiftRecord=\0 ChatColorRecord=\0 ",
									   m_strOwnerIni ) ;

		m_DlgSetup.m_SetupSliMusic.SetSliderValue ( 80 ) ;
		DXSetMusicVolume ( 80 ) ;
		m_DlgSetup.m_SetupSliEffect.SetSliderValue ( 80 ) ;
		DXSetSoundVolume ( 80 ) ;

		g_objHero.SetPkMode ( PKMODE_ARRESTMENT ) ;
	}
}

void CDlgMain::OnXpFull()
{
	if (m_DlgXp.m_bTransform)
		return;
	m_XpPrg.SetShowMode ( 1 ) ;
	int nAmount = g_objHero.GetXpSkillAmount() ;
	if ( g_objHero.IsTransform () )
	{
		nAmount = 0 ;
		for ( int i = 0; i < g_objHero.GetXpSkillAmount (); i++ )
		{
			CMagic* pMagic = g_objHero.GetXpSkillByIndex ( i ) ;
			if ( pMagic != NULL && pMagic->IsTransformMagic () )
				nAmount ++ ;
		}
	}
	if ( nAmount > 0 )
	{
		if ( nAmount > 10 )
			m_DlgXp.MoveWindow ( CMyBitmap::GetScreenWidth () - 518, CMyBitmap::GetScreenHeight () - 173, 518, 50 ) ;
		else
			m_DlgXp.MoveWindow ( CMyBitmap::GetScreenWidth () - nAmount * 50 - 18,
				CMyBitmap::GetScreenHeight () - 173,
				nAmount * 50 + 18,
				50 ) ;
		m_DlgXp.InitXpDialog ( ) ;
	}
}

void CDlgMain::PlayXp()
{
	m_bXpAct = true ;
	m_dwLastXpTime = 0 ;
}

void CDlgMain::HeroDie()
{
	m_DlgXp.ShowWindow ( SW_HIDE ) ;
	m_DlgKongfu.ShowWindow ( SW_HIDE ) ;
	m_DlgMagic.ShowWindow ( SW_HIDE ) ;
	m_DlgXp.MoveWindow ( CMyBitmap::GetScreenWidth ()  - 68,
						 CMyBitmap::GetScreenHeight () - 173,
						 68,
						 50 ) ;
	m_DlgXp.SetReborn (true) ;
}

void CDlgMain::HeroReborn()
{
	m_DlgXp.SetReborn (false) ;
}

void CDlgMain::OpenImprove()
{
	if ( m_DlgTrade.m_bShow )
		return ;
	CDlgMain::CloseCurDlg () ;
	m_DlgImprove.ShowWindow ( SW_SHOW ) ;
	if ( !m_DlgMpcGoods.m_bShow )
	{
		if ( m_DlgNpcEquip.m_bShow )
			m_DlgNpcEquip.ShowWindow ( SW_HIDE ) ;
		m_DlgMpcGoods.ShowWindow ( SW_SHOW ) ;
	}
	m_uCurDlgID = DLG_IMPROVE ;
}

void CDlgMain::OpenEnchant()
{
	if ( m_DlgTrade.m_bShow )
		return ;
	CDlgMain::CloseCurDlg () ;
	m_DlgEnchant.ShowWindow ( SW_SHOW ) ;
	if ( !m_DlgMpcGoods.m_bShow )
	{
		if ( m_DlgNpcEquip.m_bShow )
			m_DlgNpcEquip.ShowWindow ( SW_HIDE ) ;
		m_DlgMpcGoods.ShowWindow ( SW_SHOW ) ;
	}
	m_uCurDlgID = DLG_ENCHANT ;
}

void CDlgMain::OpenNpcFace()
{
	if ( m_DlgTrade.m_bShow )
		return ;
	CDlgMain::CloseCurDlg () ;
	m_DlgNpcFace.InitNpcFace ( ( BYTE ) g_objHero.GetSex () ) ;
	m_DlgNpcFace.ShowWindow ( SW_SHOW ) ;
	m_uCurDlgID = DLG_NPCFACE ;
}

void CDlgMain::OpenEmbed()
{
	if ( m_DlgEmbed.m_bShow )
		return ;
	CDlgMain::CloseCurDlg () ;
	m_DlgEmbed.ShowWindow ( SW_SHOW ) ;
	if ( !m_DlgMpcGoods.m_bShow )
	{
		if ( m_DlgNpcEquip.m_bShow )
			m_DlgNpcEquip.ShowWindow ( SW_HIDE ) ;
		m_DlgMpcGoods.ShowWindow ( SW_SHOW ) ;
	}
	m_uCurDlgID = DLG_EMBED ;
}

void CDlgMain::OpenCompose()
{
	if ( m_DlgCompose.m_bShow )
		return ;
	CDlgMain::CloseCurDlg () ;
	m_DlgCompose.ShowWindow ( SW_SHOW ) ;
	if ( !m_DlgMpcGoods.m_bShow )
	{
		if ( m_DlgNpcEquip.m_bShow )
			m_DlgNpcEquip.ShowWindow ( SW_HIDE ) ;
		m_DlgMpcGoods.ShowWindow ( SW_SHOW ) ;
	}
	m_uCurDlgID = DLG_EXP ;
}

void CDlgMain::OpenBooth(BOOL bOwner)
{
	if ( bOwner )
	{
		if ( m_DlgMpcBooth.m_bShow || m_DlgTrade.m_bShow )
			return ;
		CDlgMain::CloseCurDlg () ;
		if ( m_DlgBag.m_bShow )
			m_DlgBag.ShowWindow ( SW_HIDE ) ;
		m_DlgMpcBooth.ShowWindow ( SW_SHOW ) ;
		if ( !m_DlgMpcGoods.m_bShow )
		{
			if ( m_DlgNpcEquip.m_bShow )
				m_DlgNpcEquip.ShowWindow ( SW_HIDE ) ;
			m_DlgMpcGoods.ShowWindow ( SW_SHOW ) ;
		}
		m_uCurDlgID = DLG_MPCBOOTH ;
	}
	else
	{
		CDlgMain::CloseCurDlg () ;
		m_DlgNpcBooth.ShowWindow ( SW_SHOW ) ;
		m_uCurDlgID = DLG_NPCBOOTH ;
	}
}

void CDlgMain::OpenEffigy()
{
	CDlgMain::CloseCurDlg () ;
	m_DlgEffigy.ShowWindow ( SW_SHOW ) ;
	m_uCurDlgID = DLG_EFFIGY ;
}

void CDlgMain::OpenLay()
{
	CDlgMain::CloseCurDlg () ;
	m_DlgLay.ShowWindow ( SW_SHOW ) ;
	m_uCurDlgID = DLG_LAY ;
}

void CDlgMain::OnMainStand()
{
	if ( m_StandChk.GetCheckValue () )
		CDlgMain::GetParent()->PostMessage( WM_MY_MESSAGE, ON_MAIN_NPCEQUIP, 1 ) ;
	else
		CDlgMain::GetParent()->PostMessage( WM_MY_MESSAGE, ON_MAIN_NPCEQUIP, 0 ) ;
}

void CDlgMain::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ( m_rcTip[0].PtInRect ( point ) )
		CDlgMain::GetParent()->PostMessage( WM_MY_MESSAGE, ON_MAIN_EXPDOWN, 0 ) ;
	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgMain::OpenDice(BOOL bOpen)
{
	if ( bOpen )
	{
		m_DlgDice.ShowWindow ( SW_SHOW ) ;
		if ( m_DlgMpcGoods.m_bShow )
			m_DlgMpcGoods.ShowWindow ( SW_HIDE ) ;
	}
	else
	{
		m_DlgDice.ResetDice () ;
		m_DlgDice.ShowWindow ( SW_HIDE ) ;
		if ( m_DlgDiceList.m_bShow )
			m_DlgDiceList.ShowWindow ( SW_HIDE ) ;
	}
}

void CDlgMain::CloseOtherDlg( BOOL bCloseHelp )
{
	if ( m_DlgSetup.m_bShow )
		m_DlgSetup.ShowWindow ( SW_HIDE ) ;
	if ( m_DlgMsgSend.m_bShow )
		m_DlgMsgSend.ShowWindow ( SW_HIDE ) ;
	if ( m_DlgMsgReceive.m_bShow )
		m_DlgMsgReceive.ShowWindow ( SW_HIDE ) ;
	if ( m_DlgChat.m_bShow )
		m_DlgChat.ShowWindow ( SW_HIDE ) ;
	if ( m_DlgLevWord.m_bShow )
		m_DlgLevWord.ShowWindow ( SW_HIDE ) ;
	if ( bCloseHelp )
	{
		if ( !m_DlgInsHelp.m_InsHelpBtnHelp.IsWindowEnabled () )
			m_DlgInsHelp.OnInshelpBtnCloses () ;
	}
}

void CDlgMain::OnMainRun()
{
}

void CDlgMain::OnMainGoodbox()
{
}
