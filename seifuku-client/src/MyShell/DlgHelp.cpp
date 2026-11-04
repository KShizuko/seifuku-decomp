#include "stdafx.h"
#include "MyShell.h"
#include "DlgHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgHelp::CDlgHelp(CWnd* pParent )
	: CDialog(CDlgHelp::IDD, pParent)
{
	m_pWeb = NULL ;
	m_Hbr = NULL ;
	m_hBmpBk = NULL ;
	m_hBmpClose = NULL ;
	m_hBmpMore = NULL ;
}


void CDlgHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HELP_MORE, m_HelpMore);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_HelpClose);
}


BEGIN_MESSAGE_MAP(CDlgHelp, CDialog)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	ON_BN_CLICKED(IDC_HELP_MORE, OnHelpMore)
END_MESSAGE_MAP()

BOOL CDlgHelp::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	m_HelpMore.MoveWindow ( 635, 59, 66, 20, true ) ;
	m_HelpClose.MoveWindow ( 695, 7, 16, 16, true ) ;

	return TRUE;
}

void CDlgHelp::OnDestroy()
{
	CDialog::OnDestroy();

	if ( m_pWeb )
	{
		m_pWeb->DestroyWindow () ;
		delete m_pWeb ;
	}
	if (m_Hbr)
		DeleteObject( m_Hbr ) ;
	if ( m_hBmpBk )
		DeleteObject ( m_hBmpBk ) ;
	if ( m_hBmpClose )
		DeleteObject ( m_hBmpClose ) ;
	if ( m_hBmpMore )
		DeleteObject ( m_hBmpMore ) ;
}

HBRUSH CDlgHelp::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;

	if ( nCtlColor == CTLCOLOR_DLG )
		return m_Hbr ;
	return NULL ;
}

void CDlgHelp::OpenHelpDlg(char *strFileName)
{
	COleVariant varEmpty;

	if ( strFileName != NULL )
	{
		if ( m_pWeb )
		{
			m_pWeb->DestroyWindow () ;
			delete m_pWeb ;
		}
		m_pWeb = new CWebBrowser ;
		CRect rect ;
		CDlgHelp::GetWindowRect ( rect ) ;
		if ( ! m_pWeb->Create( NULL,
							   WS_CHILD | WS_VISIBLE,
							   CRect( 10, 80, rect.Width() - 15, rect.Height () - 10 ),
							   this,
							   100 ) )
			return ;
		char strBuffer[256] = "";
		GetCurrentDirectory ( 256, strBuffer ) ;
		strcat ( strBuffer, "\\Help\\" ) ;
		strcat ( strBuffer, strFileName ) ;
		m_pWeb->Navigate( strBuffer, &varEmpty, &varEmpty,
						&varEmpty, &varEmpty );
		CDlgHelp::ShowWindow ( SW_SHOW ) ;
	}
}

void CDlgHelp::OnButtonClose()
{
	CDlgHelp::ShowWindow ( SW_HIDE ) ;
}

void CDlgHelp::OnHelpMore()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;

	char szUrl[1024] = "" ;
	if ( IniStrGet ( "ini/oem.ini", "OemHelp", "url", szUrl ) )
		::ShellExecute(m_hWnd, NULL, szUrl, NULL, NULL, SW_SHOW );
	else
		::ShellExecute(m_hWnd, NULL, GameDataSetQuery()->GetStr ( 10435 ), NULL, NULL, SW_SHOW );
}

void CDlgHelp::ResetBk(int nMode)
{
	char strName[32] ;
	char strMain[8] ;
	if ( nMode == 0 )
		strcpy ( strMain, "Main" ) ;
	else
		sprintf ( strMain, "Main%d", nMode ) ;

	sprintf ( strName, "Data/%s/HelpBk.bmp", strMain ) ;
	m_hBmpBk =  (HBITMAP )::LoadImage ( AfxGetInstanceHandle(),
									strName,
									IMAGE_BITMAP,
									0,
									0,
									LR_LOADFROMFILE ) ;
	m_Hbr = CreatePatternBrush ( m_hBmpBk ) ;

	sprintf ( strName, "Data/%s/Close.bmp", strMain ) ;
	m_hBmpClose =  (HBITMAP )::LoadImage ( AfxGetInstanceHandle(),
									strName,
									IMAGE_BITMAP,
									0,
									0,
									LR_LOADFROMFILE ) ;
	m_HelpClose.SetBitmap ( m_hBmpClose ) ;

	sprintf ( strName, "Data/%s/HelpMore.bmp", strMain ) ;
	m_hBmpMore =  (HBITMAP )::LoadImage ( AfxGetInstanceHandle(),
									strName,
									IMAGE_BITMAP,
									0,
									0,
									LR_LOADFROMFILE ) ;
	m_HelpMore.SetBitmap ( m_hBmpMore ) ;
	CDlgHelp::RedrawWindow () ;
}
