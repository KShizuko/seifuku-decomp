#include "stdafx.h"
#include "MyShell.h"
#include "DlgTitleBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgTitleBar::CDlgTitleBar(CWnd* pParent )
	: CDialog(CDlgTitleBar::IDD, pParent)
{
	m_bLBtnDown = false ;
	m_pParentWnd = NULL ;
	m_hBmp = NULL ;
	m_Hbr = NULL ;
}


void CDlgTitleBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_MIN, m_BtnMin);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_BtnClose);
}


BEGIN_MESSAGE_MAP(CDlgTitleBar, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBtnClose)
	ON_BN_CLICKED(IDC_BTN_MIN, OnBtnMin)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CDlgTitleBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	return TRUE;
}

void CDlgTitleBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLBtnDown = true ;
	GetCursorPos ( &m_InitDlgPnt ) ;
	CDlgTitleBar::GetWindowRect ( m_InitDlgRect ) ;
	if ( m_pParentWnd )
	{
		m_pParentWnd->PostMessage( WM_MY_MESSAGE, ON_OWNER_MOVE ) ;
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgTitleBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLBtnDown = false ;
	if ( m_pParentWnd )
	{
		m_pParentWnd->PostMessage( WM_MY_MESSAGE, ON_OWNER_STOP ) ;
	}
	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgTitleBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( m_bLBtnDown )
	{

	}
	CDialog::OnMouseMove(nFlags, point);
}

BOOL CDlgTitleBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor ( ( ( CMyShellApp* ) AfxGetApp () )->m_hCursor[7] ) ;
	return true ;
}

LRESULT CDlgTitleBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_COMMAND )
	{
		if ( wParam == 1 || wParam == 2 )
			return true ;
	}
	else if ( message == 43 )
		return true ;

	return CDialog::WindowProc(message, wParam, lParam);
}

CPoint CDlgTitleBar::GetOffset()
{
	GetCursorPos ( &m_MouseMovePnt ) ;
	CDlgTitleBar::MoveWindow ( m_InitDlgRect.left + ( m_MouseMovePnt.x - m_InitDlgPnt.x ),
							   m_InitDlgRect.top + ( m_MouseMovePnt.y - m_InitDlgPnt.y ),
							   m_InitDlgRect.Width (),
							   m_InitDlgRect.Height () ) ;
	return CPoint ( m_MouseMovePnt.x - m_InitDlgPnt.x, m_MouseMovePnt.y - m_InitDlgPnt.y ) ;
}


HBRUSH CDlgTitleBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

void CDlgTitleBar::OnBtnClose()
{
	if ( m_pParentWnd )
	{
		m_pParentWnd->PostMessage( WM_MY_MESSAGE, ON_EXIT ) ;
	}
}

void CDlgTitleBar::OnBtnMin()
{
	if ( m_pParentWnd )
	{
		m_pParentWnd->PostMessage( WM_MY_MESSAGE, ON_MIN ) ;
	}
}

void CDlgTitleBar::OnDestroy()
{
	CDialog::OnDestroy();
	if ( m_Hbr )
		DeleteObject( m_Hbr ) ;
	if ( m_hBmp )
		DeleteObject( m_hBmp ) ;
}

void CDlgTitleBar::ResetBk(int nMode)
{
	char strTemp[32] ;
	if ( nMode == 0 )
	{
		if ( CMyBitmap::GetScreenWidth () <= 800 )
		{
			strcpy ( strTemp, "Data/Main/TitleBarBk.bmp" ) ;
			m_BtnMin.MoveWindow ( 752, 1, 20, 20, FALSE ) ;
			m_BtnClose.MoveWindow ( 774, 1, 20, 20, FALSE ) ;
		}
		else
		{
			strcpy ( strTemp, "Data/Main/TitleBarBk1024.bmp" ) ;
			m_BtnMin.MoveWindow ( 977, 1, 20, 20, FALSE ) ;
			m_BtnClose.MoveWindow ( 1000, 1, 20, 20, FALSE ) ;
		}
	}
	else
		sprintf ( strTemp, "Data/Main%d/TitleBarBk.bmp", nMode ) ;

	if ( m_hBmp )
		DeleteObject ( m_hBmp ) ;
	m_hBmp =  (HBITMAP )::LoadImage ( AfxGetInstanceHandle(),
									strTemp,
									IMAGE_BITMAP,
									0,
									0,
									LR_LOADFROMFILE ) ;
	if ( m_Hbr )
		DeleteObject ( m_Hbr ) ;
	m_Hbr = CreatePatternBrush ( m_hBmp ) ;
	CDlgTitleBar::RedrawWindow () ;
}
