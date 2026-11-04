#include "stdafx.h"
#include "MyShell.h"
#include "DlgNpcBuy.h"

#include "Hero.h"
#include "Shop.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgNpcBuy::CDlgNpcBuy(CWnd* pParent )
	: CDialog(CDlgNpcBuy::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_bInitSuc = false ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_fFrameStep = 0.0f ;
	m_nFrameMode = 0 ;
	m_nPntX = 0 ;
	m_nCurrentAmount = SHOP_AMOUNT ;
}


void CDlgNpcBuy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NPCBUY_GRI_SALE, m_NpcBuySaleGri);
	DDX_Control(pDX, IDC_NPCBUY_BTN_CLOSE, m_NpcBuyCloseBtn);
	DDX_Control(pDX, IDC_NPCBUY_BTN_FIX, m_NpcBuyFixBtn);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
	DDX_Control(pDX, IDC_NPCBUY_BTN_UP, m_NpcButUpBtn );
	DDX_Control(pDX, IDC_NPCBUY_BTN_DOWN,m_NpcButDownBtn ) ;
}


BEGIN_MESSAGE_MAP(CDlgNpcBuy, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_NPCBUY_BTN_CLOSE, OnNpcbuyBtnClose)
	ON_BN_DOUBLECLICKED(IDC_NPCBUY_GRI_SALE, OnDoubleclickedNpcbuyGriSale)
	ON_BN_CLICKED(IDC_NPCBUY_BTN_FIX, OnNpcbuyBtnFix)
	ON_BN_CLICKED(IDC_NPCBUY_BTN_UP, OnNpcbuyBtnUp)
	ON_BN_CLICKED(IDC_NPCBUY_BTN_DOWN, OnNpcbuyBtnDown)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
END_MESSAGE_MAP()

BOOL CDlgNpcBuy::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgNpcBuy::GetWindowRect ( rect ) ;
	m_fFrameStep = ( float ) rect.Width () / 20 ;

	m_NpcBuySaleGri.MoveWindow ( 24, 18, 215, 172, true ) ;
	if ( ! m_NpcBuySaleGri.Init ( rect.left, rect.top, 4, 5, DLG_NPCBUY ) )
		return false ;

	if ( ! m_NpcBuyCloseBtn.Init ( rect.left, rect.top, "Button121" ) )
		return false ;
	if ( ! m_NpcBuyFixBtn.Init ( rect.left, rect.top, "Button110" ) )
		return false ;
	if ( ! m_BtnHelp.Init ( rect.left, rect.top, "Button444" ) )
		return false ;
	if ( ! m_NpcButUpBtn.Init ( rect.left, rect.top, "Button203" ) )
		return false ;
	if ( ! m_NpcButDownBtn.Init ( rect.left, rect.top, "Button204" ) )
		return false ;

	m_bInitSuc = true ;
	return TRUE;
}

HBRUSH CDlgNpcBuy::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

void CDlgNpcBuy::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_nPntX = x ;
	m_Pnt.y = y ;
}

LRESULT CDlgNpcBuy::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		if ( !wParam )
		{
			DXPlaySound ( "Sound/DlgClose.wav" ) ;
			m_bShow = wParam ;
		}
		else
		{
			CRect rect ;
			CDlgNpcBuy::GetWindowRect ( rect ) ;
			if ( m_nFrameMode == 0 )
			{
				DXPlaySound ( "Sound/DlgOpen.wav" ) ;
				m_nFrameMode = 1 ;
				m_bShow = wParam ;
				m_dwLastShowTime = 0 ;
				CDlgNpcBuy::MoveWindow ( -rect.Width(), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
			}
			else if ( m_nFrameMode == 2 )
			{
				CDlgNpcBuy::MoveWindow ( 0, m_Pnt.y, rect.Width(), rect.Height(), true ) ;
				m_nFrameMode = 0 ;
			}
		}
	}
	else if ( message == WM_COMMAND )
	{
		if ( wParam == 1 )
			return true ;
		else if ( wParam == 2 )
		{
			CDlgNpcBuy::OnNpcbuyBtnClose () ;
		}
	}
	else if ( message == WM_DESTROY )
	{
		::DeleteObject ( m_Hbr ) ;
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

void CDlgNpcBuy::Show()
{
	if ( !m_bInitSuc )
		return ;
	if ( m_bShow )
	{
		if ( m_nFrameMode == 1 )
		{
			if ( m_dwLastShowTime == 0 )
			{
				m_dwLastShowTime = TimeGet() ;
				m_Pnt.x = m_nPntX ;
			}
			else
			{
				if ( TimeGet() - m_dwLastShowTime <= 300 )
				{
					m_Pnt.x = m_nPntX ;
					m_Pnt.x += ( TimeGet() - m_dwLastShowTime ) / 15 * m_fFrameStep ;
				}
				else
				{
					m_Pnt.x = 0;
					m_nFrameMode = 2 ;
					CDlgNpcBuy::WindowProc ( WM_SHOWWINDOW, true, NULL ) ;
				}
			}
		}

		IAni* ShowAni ;
		if ( g_objShop.GetMoneyType () == 0 )
			ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog11",
													  EXIGENCE_IMMEDIATE ) ;
		else
			ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog58",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			ShowAni->Show( 0,
						   m_Pnt.x ,
						   m_Pnt.y - 1 );
		}
		else
			return ;

		m_NpcBuySaleGri.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_NpcBuyCloseBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_NpcButUpBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_NpcButDownBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( g_objShop.GetMoneyType () == 0 )
			m_NpcBuyFixBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgNpcBuy::OnNpcbuyBtnClose()
{
	CDlgNpcBuy::ShowWindow ( SW_HIDE ) ;
	CloseShop() ;
	CDlgNpcBuy::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_NPCBUY_CLOSE ) ;
}

int CDlgNpcBuy::OpenShop(OBJID idNpc)
{
	if ( !g_objShop.Open( idNpc ) )
		return 0 ;
	CRect rect ;
	CDlgNpcBuy::GetWindowRect ( rect ) ;
	m_NpcBuySaleGri.Init ( rect.left, rect.top, 4, 5, DLG_NPCBUY );
	DWORD dwItemAmount = g_objShop.GetItemAmount () ;
	int nRealAmount = ( dwItemAmount > SHOP_AMOUNT ) ? SHOP_AMOUNT : dwItemAmount ;
	for ( int i = 0; i < nRealAmount; i++ )
	{
		CItem *pItem = g_objShop.GetItem( i ) ;
		OBJID ItemTypeID = pItem->GetTypeID() ;
		m_NpcBuySaleGri.InsertIcon ( 1, 1, ItemTypeID, i, ICON_TYPE_ITEM ) ;
	}
	return 1 ;
}

void CDlgNpcBuy::CloseShop()
{
	g_objShop.Close() ;
	m_NpcBuySaleGri.RemoveIcon ( 0, 0, true ) ;
	m_nCurrentAmount = SHOP_AMOUNT ;
}

void CDlgNpcBuy::OnDoubleclickedNpcbuyGriSale()
{
}

void CDlgNpcBuy::OnNpcbuyBtnFix()
{
	if ( g_objShop.GetMoneyType () == 0 )
		CDlgNpcBuy::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_NPCBUY_FIX ) ;
}

void CDlgNpcBuy::OnNpcbuyBtnUp()
{
	DXPlaySound ( "Sound/ChangePage.wav" ) ;
	if ( m_nCurrentAmount > SHOP_AMOUNT )
	{
		m_NpcBuySaleGri.RemoveIcon ( 0, 0, true ) ;
		m_nCurrentAmount -= SHOP_AMOUNT ;
		for ( int i = m_nCurrentAmount - SHOP_AMOUNT; i < m_nCurrentAmount; i++ )
		{
			CItem *pItem = g_objShop.GetItem( i ) ;
			OBJID ItemTypeID = pItem->GetTypeID() ;
			m_NpcBuySaleGri.InsertIcon ( 1, 1, ItemTypeID, i, ICON_TYPE_ITEM ) ;
		}
	}
}

void CDlgNpcBuy::OnNpcbuyBtnDown()
{
	DXPlaySound ( "Sound/ChangePage.wav" ) ;
	DWORD dwAmount = g_objShop.GetItemAmount () ;
	if ( dwAmount > m_nCurrentAmount )
	{
		m_NpcBuySaleGri.RemoveIcon ( 0, 0, true ) ;
		m_nCurrentAmount += SHOP_AMOUNT ;
		int nNextAmount = ( dwAmount > m_nCurrentAmount ) ? m_nCurrentAmount : dwAmount ;
		for ( int i = m_nCurrentAmount - SHOP_AMOUNT; i < nNextAmount; i++ )
		{
			CItem *pItem = g_objShop.GetItem( i ) ;
			OBJID ItemTypeID = pItem->GetTypeID() ;
			m_NpcBuySaleGri.InsertIcon ( 1, 1, ItemTypeID, i, ICON_TYPE_ITEM ) ;
		}
	}
}

void CDlgNpcBuy::OnBtnHelp()
{
	if ( g_objShop.GetMoneyType () == 0 )
		CDlgNpcBuy::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_NPCBUY ) ;
	else
		CDlgNpcBuy::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_NPCBUY_EMONEY ) ;
}
