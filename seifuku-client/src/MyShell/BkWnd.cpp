#include "stdafx.h"
#include "MyShell.h"
#include "BkWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBkWnd::CBkWnd()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CBkWnd::~CBkWnd()
{
}

BEGIN_MESSAGE_MAP(CBkWnd, CWnd)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

void CBkWnd::Create()
{
	CRect rect = CRect( 0, 0, 1024, 768 ) ;
	CreateEx( NULL,
			  AfxRegisterWndClass(0,AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
			  "MyWnd",
			  WS_POPUP,
			  rect,
			  NULL,
			  NULL,
			  NULL);
}

void CBkWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);

	m_MyShellDlg.Create( IDD_MYSHELL_DIALOG, this ) ;
	if ( m_MyShellDlg.m_hWnd && m_MyShellDlg.m_bInitSuc )
	{
		m_MyShellDlg.ShowWindow( SW_SHOW ) ;
		m_MyShellDlg.SetFocus() ;
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
}
