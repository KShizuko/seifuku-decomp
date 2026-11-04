#include "stdafx.h"
#include "MyShell.h"
#include "DlgTrade.h"
#include "GamePlayerSet.h"

#include "Hero.h"
#include "GameMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgTrade::CDlgTrade(CWnd* pParent )
	: CDialog(CDlgTrade::IDD, pParent)
{
	m_bShow = false ;
	m_bInitSuc = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_HbrStatic = ::CreateSolidBrush ( 0x2132B ) ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_nMoney = 0 ;
	m_nEmoney = 0 ;
	m_nChange = 0 ;
	m_nEmoneyChange = 0 ;
	m_bMoneyAdd = false ;
	m_nNpcPage = 0 ;
	m_nMpcPage = 0 ;
	m_nNpcNameLen = 0 ;
}


void CDlgTrade::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRADE_STA_NPCEMONEY, m_TradeStaNpcEmoney);
	DDX_Control(pDX, IDC_TRADE_EDT_MPCEMONEY, m_TradeEdtMpcEmoney);
	DDX_Control(pDX, IDC_TRADE_STA_NPCLEVEL, m_TradeStaNpcLevel);
	DDX_Control(pDX, IDC_TRADE_STA_NPCALLPAGE, m_TradeStaNpcAllPage);
	DDX_Control(pDX, IDC_TRADE_STA_MPCALLPAGE, m_TradeStaMpcAllPage);
	DDX_Control(pDX, IDC_TRADE_BTN_NPCU, m_TradeBtnNpcU);
	DDX_Control(pDX, IDC_TRADE_BTN_NPCD, m_TradeBtnNpcD);
	DDX_Control(pDX, IDC_TRADE_BTN_MPCU, m_TradeBtnMpcU);
	DDX_Control(pDX, IDC_TRADE_BTN_MPCD, m_TradeBtnMpcD);
	DDX_Control(pDX, IDC_TRADE_CLOSEB, m_TradeCloseB);
	DDX_Control(pDX, IDC_TRADE_GRI_NPC, m_TradeGriNpc);
	DDX_Control(pDX, IDC_TRADE_GRI_MPC, m_TradeGriMpc);
	DDX_Control(pDX, IDC_TRADE_STA_NPCNAME, m_TradeStaNpcName);
	DDX_Control(pDX, IDC_TRADE_STA_NPCMONEY, m_TradeStaNpcMoney);
	DDX_Control(pDX, IDC_TRADE_STA_MPCPAGE, m_TradeStaMpcPage);
	DDX_Control(pDX, IDC_TRADE_STA_NPCPAGE, m_TradeStaNpcPage);
	DDX_Control(pDX, IDC_TRADE_EDT_MPCMONEY, m_TradeEdtMpcMoney);
	DDX_Control(pDX, IDC_TRADE_STA_NPCAGREE, m_TradeStaNpcAgree);
	DDX_Control(pDX, IDC_TRADE_BTN_MPCAGREE, m_TradeBtnMpcAgree);
	DDX_Control(pDX, IDC_TRADE_BTN_MONEYMPCOK, m_TradeBtnMoneyMpcOk);
	DDX_Control(pDX, IDC_TRADE_BTN_CLOSE, m_TradeBtnClose);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
}


BEGIN_MESSAGE_MAP(CDlgTrade, CDialog)
	ON_BN_CLICKED(IDC_TRADE_BTN_CLOSE, OnTradeBtnClose)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_TRADE_BTN_MONEYMPCOK, OnTradeBtnMoneympcok)
	ON_BN_CLICKED(IDC_TRADE_BTN_MPCAGREE, OnTradeBtnMpcagree)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_TRADE_CLOSEB, OnTradeCloseb)
	ON_BN_CLICKED(IDC_TRADE_BTN_MPCD, OnTradeBtnMpcD)
	ON_BN_CLICKED(IDC_TRADE_BTN_MPCU, OnTradeBtnMpcU)
	ON_BN_CLICKED(IDC_TRADE_BTN_NPCD, OnTradeBtnNpcD)
	ON_BN_CLICKED(IDC_TRADE_BTN_NPCU, OnTradeBtnNpcU)
	ON_EN_CHANGE(IDC_TRADE_EDT_MPCMONEY, OnChangeTradeEdtMpcmoney)
	ON_EN_CHANGE(IDC_TRADE_EDT_MPCEMONEY, OnChangeTradeEdtMpcemoney)
END_MESSAGE_MAP()

LRESULT CDlgTrade::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		if ( wParam == false )
		{
			m_bShow = false ;
			this->ResetTrade () ;
			CDlgTrade::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_TRADE_CLOSE ) ;
		}
		else
		{
			m_bShow = true ;
			m_nChange = 0 ;
			char strLevel[16] ;
			sprintf ( strLevel, "%s%d", GameDataSetQuery()->GetStr ( 10442 ), g_objHero.GetPlayerTradeLevel () ) ;
			m_TradeStaNpcName.SetWindowText( g_objHero.GetPlayerTradeName () ) ;
			m_nNpcNameLen = strlen ( g_objHero.GetPlayerTradeName () ) ;
			m_TradeStaNpcLevel.SetWindowText( strLevel ) ;
		}
	}
	else if ( message == WM_COMMAND )
	{
		if ( wParam == 1 )
		{
			return true ;
		}
		else if ( wParam == 2 )
		{
			return true ;
		}
	}
	else if ( message == WM_DESTROY )
	{
		::DeleteObject ( m_Hbr ) ;
		::DeleteObject ( m_HbrStatic ) ;
	}

	else if ( message == WM_ERASEBKGND )
		return true ;
	else if ( message == WM_NCPAINT )
		return false ;
	else if ( message == WM_PAINT )
	{
		PAINTSTRUCT ps ;
		::BeginPaint ( m_hWnd, &ps ) ;
		::EndPaint ( m_hWnd, &ps ) ;
		return false ;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CDlgTrade::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgTrade::GetWindowRect ( rect ) ;

	if ( ! m_TradeGriNpc.Init ( rect.left, rect.top, 2, 3, DLG_TRADENPC ) )
		return false ;
	m_TradeGriNpc.SetEnable ( false ) ;
	if ( ! m_TradeGriMpc.Init ( rect.left, rect.top, 2, 3, DLG_TRADEMPC ) )
		return false ;
	m_TradeGriMpc.SetPickEnable ( false ) ;

	if ( ! m_TradeEdtMpcMoney.Init ( rect.left, rect.top, NULL, 0x000000, 0xffffff ) )
		return false ;
	m_TradeEdtMpcMoney.SetOffset ( CPoint ( 1, 0 ) ) ;
	m_TradeEdtMpcMoney.SetLimitText ( 11 ) ;
	if ( ! m_TradeEdtMpcEmoney.Init ( rect.left, rect.top, NULL, 0x000000, 0xffffff ) )
		return false ;
	m_TradeEdtMpcEmoney.SetOffset ( CPoint ( 1, 0 ) ) ;
	m_TradeEdtMpcEmoney.SetLimitText ( 11 ) ;

	m_TradeStaNpcName.Init ( rect.left, rect.top, 0, NULL, "NULL" );
	m_TradeStaNpcName.SetFontColor ( 0xffffff ) ;
	m_TradeStaNpcLevel.Init ( rect.left, rect.top, 0, NULL, "NULL" );
	m_TradeStaNpcLevel.SetFontColor ( 0xffff00 ) ;
	m_TradeStaNpcMoney.Init ( rect.left, rect.top, 0, NULL, "NULL" );
	m_TradeStaNpcMoney.SetFontColor ( 0xffffff ) ;
	m_TradeStaNpcEmoney.Init ( rect.left, rect.top, 0, NULL, "NULL" );
	m_TradeStaNpcEmoney.SetFontColor ( 0xffffff ) ;
	m_TradeStaNpcPage.Init ( rect.left, rect.top, 0, NULL, "NULL" );
	m_TradeStaNpcPage.SetFontColor ( 0xffffff ) ;
	m_TradeStaMpcPage.Init ( rect.left, rect.top, 0, NULL, "NULL" );
	m_TradeStaMpcPage.SetFontColor ( 0xffffff ) ;
	m_TradeStaNpcAllPage.Init ( rect.left, rect.top, 0, NULL, "NULL" );
	m_TradeStaNpcAllPage.SetFontColor ( 0xffffff ) ;
	m_TradeStaMpcAllPage.Init ( rect.left, rect.top, 0, NULL, "NULL" );
	m_TradeStaMpcAllPage.SetFontColor ( 0xffffff ) ;
	if ( ! m_TradeStaNpcAgree.Init ( rect.left, rect.top, 1, "Button8", "NULL" ) )
		return false ;

	if ( ! m_TradeBtnMpcAgree.Init ( rect.left, rect.top, "Button8" ) )
		return false ;
	if ( ! m_TradeBtnMoneyMpcOk.Init ( rect.left, rect.top, "Button9" ) )
		return false ;
	if ( ! m_TradeBtnClose.Init ( rect.left, rect.top, "Button5" ) )
		return false ;
	if ( ! m_BtnHelp.Init ( rect.left, rect.top, "Button4" ) )
		return false ;
	if ( ! m_TradeCloseB.Init ( rect.left, rect.top, "Button3" ) )
		return false ;
	m_TradeBtnNpcU.Init ( rect.left, rect.top, "Button203" ) ;
	m_TradeBtnNpcD.Init ( rect.left, rect.top, "Button204" ) ;
	m_TradeBtnMpcU.Init ( rect.left, rect.top, "Button203" ) ;
	m_TradeBtnMpcD.Init ( rect.left, rect.top, "Button204" ) ;

	m_bInitSuc = true ;
	return TRUE;
}

void CDlgTrade::Show()
{
	if ( !m_bInitSuc )
		return ;
	if ( m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog14",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			ShowAni->Show( 0,
						   m_Pnt.x,
						   m_Pnt.y );
		}
		else
			return ;

		m_TradeGriNpc.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeGriMpc.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_TradeEdtMpcMoney.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeEdtMpcEmoney.Show ( m_Pnt.x, m_Pnt.y ) ;

		CMyBitmap::ShowBlock ( m_Pnt.x + 20,
							   m_Pnt.y + 44,
							   m_Pnt.x + 20 + m_nNpcNameLen * 7,
							   m_Pnt.y + 58,
							   0xFF0000A0 ) ;
		m_TradeStaNpcName.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeStaNpcLevel.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeStaNpcMoney.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeStaNpcEmoney.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeStaNpcAgree.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeStaNpcPage.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeStaMpcPage.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeStaNpcAllPage.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeStaMpcAllPage.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_TradeBtnMpcAgree.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeBtnMoneyMpcOk.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeBtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeCloseB.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeBtnNpcU.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeBtnNpcD.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeBtnMpcU.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TradeBtnMpcD.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgTrade::OnTradeBtnClose()
{
	g_objHero.QuitTrade () ;
	CDlgTrade::ShowWindow ( SW_HIDE ) ;
}

void CDlgTrade::SetMpcGridContain()
{
	m_TradeGriMpc.RemoveIcon ( 0, 0 , true ) ;
	int nAmount = g_objHero.GetHeroTradeItemAmount () ;
	if ( nAmount == 20 )
	{
		g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10390 ) ) ;
		m_TradeGriMpc.SetEnable ( false ) ;
	}
	if ( nAmount > m_nMpcPage + TRADE_PAGE_AMOUNT )
		nAmount = m_nMpcPage + TRADE_PAGE_AMOUNT ;
	for ( int i = m_nMpcPage; i < nAmount; i++ )
	{
		OBJID ItemTypeId = g_objHero.GetHeroTradeItemByIndex ( i )->GetTypeID () ;
		BYTE btActType = 0 ;
		if ( ItemTypeId < 1000000 && ( ItemTypeId < 700000 || ItemTypeId > 800000 ) )
		{
			switch ( ItemTypeId % 10 )
			{
			case 9:
				btActType = 1 ;
				break ;
			case 8:
				btActType = 2 ;
				break ;
			case 7:
				btActType = 3 ;
				break ;
			case 6:
				btActType = 4 ;
				break ;
			}
		}
		if ( ItemTypeId >= 700001 && ItemTypeId <= 700063 )
			btActType = 5 ;
		if ( ItemTypeId >= 1088000 && ItemTypeId <= 1088002 )
			btActType = 6 ;
		if ( ItemTypeId == 1100003 || ItemTypeId == 1100006 || ItemTypeId == 1100009 )
			btActType = 6 ;
		m_TradeGriMpc.InsertIcon ( 1, 1, ItemTypeId, i - m_nMpcPage, ICON_TYPE_ITEM, btActType ) ;
	}
	char strPage[16] ;
	itoa ( m_nMpcPage / TRADE_PAGE_AMOUNT + 1, strPage, 10 ) ;
	m_TradeStaMpcPage.SetWindowText ( strPage ) ;
	itoa ( ( g_objHero.GetHeroTradeItemAmount () + TRADE_PAGE_AMOUNT - 1 ) / TRADE_PAGE_AMOUNT, strPage, 10 ) ;
	m_TradeStaMpcAllPage.SetWindowText ( strPage ) ;
}

void CDlgTrade::SetNpcGridContain()
{
	m_TradeGriNpc.RemoveIcon ( 0, 0 , true ) ;
	int nAmount = g_objHero.GetPlayerTradeItemAmount () ;
	if ( nAmount > m_nNpcPage + TRADE_PAGE_AMOUNT )
		m_TradeBtnNpcD.SetCurFrame ( 2 ) ;
	if ( nAmount > m_nNpcPage + TRADE_PAGE_AMOUNT )
		nAmount = m_nNpcPage + TRADE_PAGE_AMOUNT ;
	for ( int i = m_nNpcPage; i < nAmount; i++ )
	{
		OBJID ItemTypeId = g_objHero.GetPlayerTradeItemByIndex ( i )->GetTypeID () ;
		BYTE btActType = 0 ;
		if ( ItemTypeId < 1000000 && ( ItemTypeId < 700000 || ItemTypeId > 800000 ) )
		{
			switch ( ItemTypeId % 10 )
			{
			case 9:
				btActType = 1 ;
				break ;
			case 8:
				btActType = 2 ;
				break ;
			case 7:
				btActType = 3 ;
				break ;
			case 6:
				btActType = 4 ;
				break ;
			}
		}
		if ( ItemTypeId >= 700001 && ItemTypeId <= 700063 )
			btActType = 5 ;
		if ( ItemTypeId >= 1088000 && ItemTypeId <= 1088002 )
			btActType = 6 ;
		if ( ItemTypeId == 1100003 || ItemTypeId == 1100006 || ItemTypeId == 1100009 )
			btActType = 6 ;
		m_TradeGriNpc.InsertIcon ( 1, 1, ItemTypeId, i - m_nNpcPage, ICON_TYPE_ITEM, btActType ) ;
	}
	char strPage[16] ;
	itoa ( m_nNpcPage / TRADE_PAGE_AMOUNT + 1, strPage, 10 ) ;
	m_TradeStaNpcPage.SetWindowText ( strPage ) ;
	itoa ( ( g_objHero.GetPlayerTradeItemAmount () + TRADE_PAGE_AMOUNT - 1 ) / TRADE_PAGE_AMOUNT, strPage, 10 ) ;
	m_TradeStaNpcAllPage.SetWindowText ( strPage ) ;
}

HBRUSH CDlgTrade::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	if ( nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_STATIC )
		return m_HbrStatic ;
	return m_Hbr ;
}

void CDlgTrade::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgTrade::SetNpcMoney()
{
	int nNpcMoney = g_objHero.GetPlayerTradeMoney ( ) ;
	char strMoney[64] = "" ;
	itoa ( nNpcMoney, strMoney, 10 ) ;
	strcpy ( strMoney, CMyShellApp::StringChange ( strMoney, 2 ) ) ;
	m_TradeStaNpcMoney.SetWindowText ( strMoney ) ;
}

void CDlgTrade::SetNpcEmoney()
{
	int nNpcEmoney = g_objHero.GetPlayerTradeEmoney ( ) ;
	char strMoney[64] = "" ;
	itoa ( nNpcEmoney, strMoney, 10 ) ;
	strcpy ( strMoney, CMyShellApp::StringChange ( strMoney, 2 ) ) ;
	m_TradeStaNpcEmoney.SetWindowText ( strMoney ) ;
}

void CDlgTrade::OnTradeBtnMpcagree()
{
	if ( !m_bMoneyAdd && m_TradeEdtMpcMoney.GetWindowTextLength () != 0 )
		g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10046 ) ) ;
	else
	{
		g_objHero.AgreeTrade () ;
		m_TradeBtnMpcAgree.EnableWindow(false);
		m_TradeBtnMpcAgree.SetCurFrame(1);
		m_TradeGriMpc.SetEnable( false ) ;
		m_TradeEdtMpcMoney.EnableWindow ( false ) ;
		m_TradeBtnMoneyMpcOk.EnableWindow ( false ) ;
	}
}

void CDlgTrade::ResetTrade()
{
	m_nNpcPage = 0 ;
	m_nMpcPage = 0 ;
	m_TradeStaNpcAgree.RemoveImage () ;
	m_TradeGriNpc.RemoveIcon ( 0, 0,true ) ;
	m_TradeBtnNpcD.SetCurFrame ( 0 ) ;
	m_TradeGriMpc.RemoveIcon ( 0, 0,true ) ;
	m_TradeGriMpc.SetEnable( true ) ;
	m_TradeBtnMpcAgree.EnableWindow(true);
	m_TradeBtnMpcAgree.SetCurFrame(0);
	m_TradeBtnMoneyMpcOk.EnableWindow(true);
	m_TradeBtnMoneyMpcOk.SetCurFrame(0);
	m_TradeEdtMpcMoney.EnableWindow(true) ;
	m_TradeEdtMpcEmoney.EnableWindow(true) ;
	m_nMoney = 0 ;
	m_nEmoney = 0 ;
	m_nChange = 0 ;
	m_nEmoneyChange = 0 ;
	m_TradeEdtMpcMoney.SetWindowText(NULL);
	m_TradeEdtMpcEmoney.SetWindowText(NULL);
	m_TradeStaNpcMoney.SetWindowText (NULL) ;
	m_TradeStaNpcEmoney.SetWindowText (NULL) ;
	m_TradeStaMpcPage.SetWindowText (NULL) ;
	m_TradeStaMpcAllPage.SetWindowText (NULL) ;
	m_TradeStaNpcPage.SetWindowText (NULL) ;
	m_TradeStaNpcAllPage.SetWindowText (NULL) ;
	m_bMoneyAdd = false ;
}

void CDlgTrade::OnTradeCloseb()
{
	CDlgTrade::OnTradeBtnClose () ;
}

void CDlgTrade::OnBtnHelp ()
{
	CDlgTrade::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_TRADE ) ;
}

void CDlgTrade::OnTradeBtnMoneympcok()
{
	int nMpcMoney = m_nMoney ;
	int nMpcEmoney = m_nEmoney ;
	if ( nMpcMoney > 0 || nMpcEmoney > 0 )
	{
		if ( ( DWORD ) nMpcMoney <= g_objHero.GetMoney () && ( DWORD ) nMpcEmoney <= g_objHero.GetEmoney () )
		{
			m_TradeEdtMpcMoney.EnableWindow ( false ) ;
			m_TradeEdtMpcEmoney.EnableWindow ( false ) ;
			m_TradeBtnMoneyMpcOk.EnableWindow(false);
			m_TradeBtnMoneyMpcOk.SetCurFrame(1);
			g_objHero.AddHeroTradeMoney ( nMpcMoney ) ;
			g_objHero.AddHeroTradeEmoney ( nMpcEmoney ) ;
			m_bMoneyAdd = TRUE ;
		}
		else
		{
			g_objGameMsg.AddMsg((char*)GameDataSetQuery()->GetStr(10045));
		}
	}
}

void CDlgTrade::OnTradeBtnMpcD()
{
	if ( g_objHero.GetHeroTradeItemAmount () >= m_nMpcPage + TRADE_PAGE_AMOUNT )
	{
		m_nMpcPage += TRADE_PAGE_AMOUNT ;
		CDlgTrade::SetMpcGridContain () ;
		char strPage[16] ;
		itoa ( m_nMpcPage / TRADE_PAGE_AMOUNT + 1, strPage, 10 ) ;
		m_TradeStaMpcPage.SetWindowText ( strPage ) ;
	}
}

void CDlgTrade::OnTradeBtnMpcU()
{
	if ( m_nMpcPage > 0 )
	{
		m_nMpcPage -= TRADE_PAGE_AMOUNT ;
		CDlgTrade::SetMpcGridContain () ;
		char strPage[16] ;
		itoa ( m_nMpcPage / TRADE_PAGE_AMOUNT + 1, strPage, 10 ) ;
		m_TradeStaMpcPage.SetWindowText ( strPage ) ;
	}
}

void CDlgTrade::OnTradeBtnNpcD()
{
	if ( g_objHero.GetPlayerTradeItemAmount () > m_nNpcPage + TRADE_PAGE_AMOUNT )
	{
		m_nNpcPage += TRADE_PAGE_AMOUNT ;
		CDlgTrade::SetNpcGridContain () ;
		char strPage[16] ;
		itoa ( m_nNpcPage / TRADE_PAGE_AMOUNT + 1, strPage, 10 ) ;
		m_TradeStaNpcPage.SetWindowText ( strPage ) ;
	}
}

void CDlgTrade::OnTradeBtnNpcU()
{
	if ( m_nNpcPage > 0 )
	{
		m_nNpcPage -= TRADE_PAGE_AMOUNT ;
		CDlgTrade::SetNpcGridContain () ;
		char strPage[16] ;
		itoa ( m_nNpcPage / TRADE_PAGE_AMOUNT + 1, strPage, 10 ) ;
		m_TradeStaNpcPage.SetWindowText ( strPage ) ;
	}
}

void CDlgTrade::OnChangeTradeEdtMpcmoney()
{
	char strMoney[12] ;
	m_TradeEdtMpcMoney.GetWindowText ( strMoney, 12 ) ;
	if ( strcmp ( strMoney, "0" ) == 0 )
	{
		m_TradeEdtMpcMoney.SetWindowText ( NULL ) ;
		return ;
	}
	m_nChange++ ;
	if ( m_nChange % 2 == 0 )
		return ;
	int nLen = m_TradeEdtMpcMoney.GetWindowTextLength () ;
	if ( nLen >= 3 )
	{
		char* pszComma = strchr ( strMoney, ',' ) ;
		if ( pszComma )
		{
			char strTemp[12] ;
			char strNum[12] ;
			strncpy ( strTemp, strMoney, pszComma - strMoney ) ;
			strTemp[pszComma - strMoney] = 0 ;
			strcat ( strTemp, pszComma + 1 ) ;
			pszComma = strchr ( strTemp, ',' ) ;
			if ( pszComma )
			{
				strncpy ( strMoney, strTemp, pszComma - strTemp ) ;
				strMoney[pszComma - strTemp] = 0 ;
				strcat ( strMoney, pszComma + 1 ) ;
				strcpy ( strNum, strMoney ) ;
			}
			else
				strcpy ( strNum, strTemp ) ;
			m_nMoney = atoi ( strNum ) ;
			int nNumLen = strlen ( strNum ) ;
			CString strText = strNum ;
			if ( nNumLen > 3 && nNumLen < 7 )
				strText.Insert ( nNumLen - 3, ',' ) ;
			else if ( nNumLen >= 7 )
			{
				strText.Insert ( nNumLen - 6, ',' ) ;
				strText.Insert ( nNumLen - 2, ',' ) ;
			}
			m_TradeEdtMpcMoney.SetWindowText ( strText ) ;
			m_TradeEdtMpcMoney.SetSel ( 12, 12 ) ;
		}
		else
		{
			if ( strlen ( strMoney ) == 4 )
			{
				m_nMoney = atoi ( strMoney ) ;
				CString strText = strMoney ;
				strText.Insert ( 1, ',' ) ;
				m_TradeEdtMpcMoney.SetWindowText ( strText ) ;
				m_TradeEdtMpcMoney.SetSel ( 5, 5 ) ;
			}
			else
			{
				m_nMoney = atoi ( strMoney ) ;
				m_nChange++ ;
			}
		}
	}
	else
	{
		m_nChange = 0 ;
		m_nMoney = atoi ( strMoney ) ;
	}
}

void CDlgTrade::OnChangeTradeEdtMpcemoney()
{
	char strMoney[12] ;
	m_TradeEdtMpcEmoney.GetWindowText ( strMoney, 12 ) ;
	if ( strcmp ( strMoney, "0" ) == 0 )
	{
		m_TradeEdtMpcEmoney.SetWindowText ( NULL ) ;
		return ;
	}
	m_nEmoneyChange++ ;
	if ( m_nEmoneyChange % 2 == 0 )
		return ;
	int nLen = m_TradeEdtMpcEmoney.GetWindowTextLength () ;
	if ( nLen >= 3 )
	{
		char* pszComma = strchr ( strMoney, ',' ) ;
		if ( pszComma )
		{
			char strTemp[12] ;
			char strNum[12] ;
			strncpy ( strTemp, strMoney, pszComma - strMoney ) ;
			strTemp[pszComma - strMoney] = 0 ;
			strcat ( strTemp, pszComma + 1 ) ;
			pszComma = strchr ( strTemp, ',' ) ;
			if ( pszComma )
			{
				strncpy ( strMoney, strTemp, pszComma - strTemp ) ;
				strMoney[pszComma - strTemp] = 0 ;
				strcat ( strMoney, pszComma + 1 ) ;
				strcpy ( strNum, strMoney ) ;
			}
			else
				strcpy ( strNum, strTemp ) ;
			m_nEmoney = atoi ( strNum ) ;
			int nNumLen = strlen ( strNum ) ;
			CString strText = strNum ;
			if ( nNumLen > 3 && nNumLen < 7 )
				strText.Insert ( nNumLen - 3, ',' ) ;
			else if ( nNumLen >= 7 )
			{
				strText.Insert ( nNumLen - 6, ',' ) ;
				strText.Insert ( nNumLen - 2, ',' ) ;
			}
			m_TradeEdtMpcEmoney.SetWindowText ( strText ) ;
			m_TradeEdtMpcEmoney.SetSel ( 12, 12 ) ;
		}
		else
		{
			if ( strlen ( strMoney ) == 4 )
			{
				m_nEmoney = atoi ( strMoney ) ;
				CString strText = strMoney ;
				strText.Insert ( 1, ',' ) ;
				m_TradeEdtMpcEmoney.SetWindowText ( strText ) ;
				m_TradeEdtMpcEmoney.SetSel ( 5, 5 ) ;
			}
			else
			{
				m_nEmoney = atoi ( strMoney ) ;
				m_nEmoneyChange++ ;
			}
		}
	}
	else
	{
		m_nEmoneyChange = 0 ;
		m_nEmoney = atoi ( strMoney ) ;
	}
}
