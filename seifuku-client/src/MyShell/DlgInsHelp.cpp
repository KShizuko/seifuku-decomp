#include "stdafx.h"
#include "MyShell.h"
#include "DlgInsHelp.h"

#include "Hero.h"
#include "3DRoleData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgInsHelp::CDlgInsHelp(CWnd* pParent )
	: CDialog(CDlgInsHelp::IDD, pParent)
{
	m_bShow = false ;
	m_Pnt = CPoint (0, 0 ) ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_bFlash = false ;
	m_bTipShow = false ;
	m_nCurTipIndex = 0 ;
}


void CDlgInsHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INSHELP_BTN_CLOSEB, m_InsHelpBtnCloseB);
	DDX_Control(pDX, IDC_INSHELP_BTN_TIP, m_InsHelpBtnTip);
	DDX_Control(pDX, IDC_INSHELP_BTN_PAGEUP, m_InsHelpBtnPageUp);
	DDX_Control(pDX, IDC_INSHELP_BTN_PAGEDOWN, m_InsHelpBtnPageDown);
	DDX_Control(pDX, IDC_INSHELP_BTN_HELP, m_InsHelpBtnHelp);
	DDX_Control(pDX, IDC_INSHELP_BTN_MORE, m_InsHelpBtnMore);
	DDX_Control(pDX, IDC_INSHELP_BTN_CLOSES, m_BtnClose);
}


BEGIN_MESSAGE_MAP(CDlgInsHelp, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_INSHELP_BTN_CLOSES, OnInshelpBtnCloses)
	ON_BN_CLICKED(IDC_INSHELP_BTN_MORE, OnInshelpBtnMore)
	ON_BN_CLICKED(IDC_INSHELP_BTN_HELP, OnInshelpBtnHelp)
	ON_BN_CLICKED(IDC_INSHELP_BTN_CLOSEB, OnInshelpBtnCloses)
	ON_BN_CLICKED(IDC_INSHELP_BTN_TIP, OnInshelpBtnTip)
	ON_BN_CLICKED(IDC_INSHELP_BTN_PAGEUP, OnInshelpBtnPageup)
	ON_BN_CLICKED(IDC_INSHELP_BTN_PAGEDOWN, OnInshelpBtnPagedown)
END_MESSAGE_MAP()

HBRUSH CDlgInsHelp::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgInsHelp::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgInsHelp::GetWindowRect ( rect ) ;

	m_BtnClose.Init ( rect.left, rect.top, "Button5" ) ;
	m_InsHelpBtnCloseB.Init ( rect.left, rect.top, "Button3" ) ;
	m_InsHelpBtnMore.Init ( rect.left, rect.top, "Button491" ) ;
	m_InsHelpBtnHelp.Init ( rect.left, rect.top, "Button490", 32 ) ;
	m_InsHelpBtnTip.Init ( rect.left, rect.top, "Button492" ) ;
	m_InsHelpBtnPageUp.Init ( rect.left, rect.top, "Button271" ) ;
	m_InsHelpBtnPageDown.Init ( rect.left, rect.top, "Button272" ) ;
	m_InsHelpBtnHelp.EnableWindow ( false ) ;

	return TRUE;
}

void CDlgInsHelp::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgInsHelp::Show()
{
	if ( m_bShow )
	{
		if ( m_InsHelpBtnHelp.IsWindowEnabled () )
		{
			if ( m_bFlash )
			{
				static BOOL bFirst = false ;
				static DWORD dwFlashTime ;
				if ( !bFirst )
				{
					bFirst = true ;
					dwFlashTime = TimeGet () ;
				}
				m_InsHelpBtnHelp.SetCurFrame ( ( ( TimeGet () - dwFlashTime ) / 500 ) & 1 ? 2 : 0 ) ;
			}
			m_InsHelpBtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;
			return ;
		}

		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														  "Dialog49",
														  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			ShowAni->Show ( 0,
							m_Pnt.x,
							m_Pnt.y ) ;
		}
		else
			return ;

		m_BtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_InsHelpBtnCloseB.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_InsHelpBtnMore.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_InsHelpBtnTip.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_InsHelpBtnPageUp.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_InsHelpBtnPageDown.Show ( m_Pnt.x, m_Pnt.y ) ;

		if ( m_bTipShow )
		{
			int nAmount = RoleDataQuery()->GetTipLineAmount ( m_nCurTipIndex ) ;
			for ( int i = 0; i < nAmount; i++ )
			{
				CMyBitmap::ShowString ( m_Pnt.x + 25,
										m_Pnt.y + 45 + i * 16,
										0xffffff,
										RoleDataQuery()->GetTipLine ( m_nCurTipIndex, i ) ) ;
			}
		}
		else
		{
			int nAmount = g_objHero.GetUserHelpInfoLineAmount () ;
			for ( int i = 0; i < nAmount; i++ )
			{
				CMyBitmap::ShowString ( m_Pnt.x + 25,
										m_Pnt.y + 45 + i * 16,
										0xffffff,
										g_objHero.GetUserHelpInfoLineByIndex ( i ) ) ;
			}
		}
	}
}

LRESULT CDlgInsHelp::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_SHOWWINDOW:
		m_bShow = wParam ;
		break ;
	case WM_COMMAND:
		if ( wParam == 1 )
		{
			return true ;
		}
		else if ( wParam == 2 )
		{
			return true ;
		}
		break ;
	case WM_DESTROY:
		::DeleteObject ( m_Hbr ) ;
		break ;
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
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CDlgInsHelp::OnInshelpBtnCloses()
{
	m_bTipShow = false ;
	m_nCurTipIndex = 0 ;
	CDlgInsHelp::ShowWindow ( SW_HIDE ) ;
	m_InsHelpBtnHelp.EnableWindow ( true ) ;
	m_bFlash = false ;
	CDlgInsHelp::MoveWindow ( 110, CMyBitmap::GetScreenHeight () - 105, 38, 32, true ) ;
	CDlgInsHelp::ShowWindow ( SW_SHOW ) ;
}

void CDlgInsHelp::OnInshelpBtnMore()
{
	char szUrl[1024] = "" ;
	if ( IniStrGet ( "ini/oem.ini", "OemHelp", "url", szUrl ) )
		::ShellExecute(m_hWnd, NULL, szUrl, NULL, NULL, SW_SHOW );
	else
		::ShellExecute(m_hWnd, NULL, GameDataSetQuery()->GetStr ( 10367 ), NULL, NULL, SW_SHOW );
}

void CDlgInsHelp::OnInshelpBtnHelp()
{
	m_InsHelpBtnHelp.EnableWindow ( false ) ;
	m_InsHelpBtnHelp.SetCurFrame ( 0 ) ;
	CDlgInsHelp::MoveWindow ( ( CMyBitmap::GetScreenWidth () - 494 ) / 2, ( CMyBitmap::GetScreenHeight () - 288 ) / 2, 494, 288, true ) ;
	CDlgInsHelp::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_INSHELP_SHOW ) ;
}

void CDlgInsHelp::OnInshelpBtnTip()
{
	m_bTipShow = true ;
}

void CDlgInsHelp::OnInshelpBtnPageup()
{
	if ( m_nCurTipIndex > 0 )
		m_nCurTipIndex-- ;
}

void CDlgInsHelp::OnInshelpBtnPagedown()
{
	if ( m_nCurTipIndex < RoleDataQuery()->GetTipsAmount () - 1 )
		m_nCurTipIndex++ ;
}

void CDlgInsHelp::ShowHelpIcon( BOOL bFlash )
{
	m_bFlash = bFlash ;
	m_InsHelpBtnHelp.EnableWindow ( true ) ;
	CDlgInsHelp::MoveWindow ( 110, CMyBitmap::GetScreenHeight () - 105, 38, 32, true ) ;
	CDlgInsHelp::ShowWindow ( SW_SHOW ) ;
}
