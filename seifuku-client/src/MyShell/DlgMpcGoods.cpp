#include "stdafx.h"
#include "MyShell.h"
#include "DlgMpcGoods.h"

#include "Hero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int		MPCGOODS_PAGE_AMOUNT = 40 ;

CDlgMpcGoods::CDlgMpcGoods(CWnd* pParent )
	: CDialog(CDlgMpcGoods::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_bInitSuc = false ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_fFrameStep = 0.0f ;
	m_nFrameMode = 0 ;
	m_nPntX = 0 ;
}


void CDlgMpcGoods::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MPCGOODS_BTN_EMONEY, m_MpcGoodsEmoneyBtn);
	DDX_Control(pDX, IDC_MPCGOODS_STA_EMONEY, m_MpcGoodsEmoneySta);
	DDX_Control(pDX, IDC_MPCGOODS_GOODBOX, m_MpcGoodsGri);
	DDX_Control(pDX, IDC_MPCGOODS_STA_MONEY, m_MpcGoodsMoneySta);
	DDX_Control(pDX, IDC_MPCGOODS_BTN_CLOSE, m_MpcGoodsCloseBtn);
	DDX_Control(pDX, IDC_MPCGOODS_BTN_MONEY, m_MpcGoodsMoneyBtn);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
}


BEGIN_MESSAGE_MAP(CDlgMpcGoods, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_MPCGOODS_BTN_CLOSE, OnMpcgoodsBtnClose)
	ON_BN_CLICKED(IDC_MPCGOODS_BTN_MONEY, OnMpcgoodsBtnMoney)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_MPCGOODS_BTN_EMONEY, OnMpcgoodsBtnEmoney)
END_MESSAGE_MAP()

LRESULT CDlgMpcGoods::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
		if ( wParam == false )
		{
			DXPlaySound ( "Sound/DlgClose.wav" ) ;
		}
		else
		{
			CRect rect ;
			CDlgMpcGoods::GetWindowRect( rect ) ;
			if ( m_nFrameMode == 0 )
			{
				DXPlaySound ( "Sound/DlgOpen.wav" ) ;
				m_dwLastShowTime = 0 ;
				m_nFrameMode = 1 ;
				CDlgMpcGoods::SetGridContain () ;
				CDlgMpcGoods::MoveWindow ( CMyBitmap::GetScreenWidth (), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
			}
			else if ( m_nFrameMode == 2 )
			{
				CDlgMpcGoods::MoveWindow ( CMyBitmap::GetScreenWidth () - rect.Width(), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
				m_nFrameMode = 0 ;
			}
		}
	}
	else if ( message == WM_COMMAND )
	{
		if ( wParam == 1 )
			return true ;
		else if (wParam == 2)
		{
			return true;
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

HBRUSH CDlgMpcGoods::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgMpcGoods::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgMpcGoods::GetWindowRect ( rect ) ;
	m_fFrameStep = ( float ) rect.Width () / 20 ;

	m_MpcGoodsGri.MoveWindow ( 26, 28, 215, 344, true ) ;
	if ( ! m_MpcGoodsGri.Init ( rect.left, rect.top, 8, 5, DLG_MPCGOODS ) )
		return false ;

	if ( ! m_MpcGoodsMoneySta.Init( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	m_MpcGoodsMoneySta.SetFontColor( 0xffffff ) ;
	if ( ! m_MpcGoodsEmoneySta.Init( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	m_MpcGoodsEmoneySta.SetFontColor( 0xffffff ) ;

	if ( ! m_MpcGoodsCloseBtn.Init ( rect.left, rect.top, "Button121" ) )
		return false ;
	if ( ! m_MpcGoodsMoneyBtn.Init ( rect.left, rect.top, "Button120" ) )
		return false ;
	if ( ! m_MpcGoodsEmoneyBtn.Init ( rect.left, rect.top, "EmoneyNavigation" ) )
		return false ;
	if ( ! m_BtnHelp.Init ( rect.left, rect.top, "Button4" ) )
		return false ;

	m_bInitSuc = true ;
	return TRUE;
}

void CDlgMpcGoods::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_nPntX = x ;
	m_Pnt.y = y ;

}

void CDlgMpcGoods::Show()
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
					m_Pnt.x -= ( TimeGet() - m_dwLastShowTime ) / 15 * m_fFrameStep;
				}
				else
				{
					CRect rect ;
					CDlgMpcGoods::GetWindowRect( rect ) ;
					m_Pnt.x = CMyBitmap::GetScreenWidth () -  rect.Width();
					m_nFrameMode = 2 ;
					CDlgMpcGoods::WindowProc ( WM_SHOWWINDOW, true, NULL ) ;
				}
			}
		}

		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog12",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			ShowAni->Show ( 0, m_Pnt.x, m_Pnt.y );
		}
		else
			return ;

		m_MpcGoodsGri.Show ( m_Pnt.x, m_Pnt.y ) ;
		static DWORD dwShowTimes = 0;

		if ((++dwShowTimes)%32 == 1)
		{
			char szMsg[256] = "";
			sprintf(szMsg, "%u", g_objHero.GetMoney());
			m_MpcGoodsMoneySta.SetWindowText (szMsg) ;
			sprintf(szMsg, "%u", g_objHero.GetEmoney());
			m_MpcGoodsEmoneySta.SetWindowText (szMsg) ;
		}

		m_MpcGoodsMoneySta.Show( m_Pnt.x, m_Pnt.y ) ;
		m_MpcGoodsEmoneySta.Show( m_Pnt.x, m_Pnt.y ) ;

		m_MpcGoodsCloseBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MpcGoodsMoneyBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MpcGoodsEmoneyBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;

	}
}

void CDlgMpcGoods::SetGridContain()
{
	m_MpcGoodsGri.RemoveIcon ( 0, 0, true ) ;

	int nAmount = g_objHero.GetItemAmount();
	for ( int i = 0; i < nAmount; i++ )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( i ) ;
		if ( pItem != NULL )
		{
			OBJID ItemTypeId = pItem->GetTypeID() ;
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
			int nIndex = m_MpcGoodsGri.InsertIcon( 1, 1, ItemTypeId, i, ICON_TYPE_ITEM, btActType ) ;
			m_MpcGoodsGri.SetIconRealID( nIndex, pItem->GetID () ) ;
		}
	}

	DWORD dwMoney = g_objHero.GetMoney () ;
	char strText[16] = "" ;
	itoa ( dwMoney, strText, 10 ) ;
	m_MpcGoodsMoneySta.SetWindowText ( strText ) ;
	itoa ( g_objHero.GetEmoney (), strText, 10 ) ;
	m_MpcGoodsEmoneySta.SetWindowText ( strText ) ;
}

void CDlgMpcGoods::OnMpcgoodsBtnClose()
{
	CDlgMpcGoods::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MPCGOODS_CLOSE ) ;
}

void CDlgMpcGoods::OnMpcgoodsBtnMoney()
{
	CDlgMpcGoods::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MPCGOODS_MONEYDROP ) ;
}

void CDlgMpcGoods::OnBtnHelp()
{
	CDlgMpcGoods::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_MPCGOODS ) ;
}

void CDlgMpcGoods::OnMpcgoodsBtnEmoney()
{
	CDlgMpcGoods::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EMONEY_NAVIGATION ) ;
}
