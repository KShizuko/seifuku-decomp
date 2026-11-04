#include "stdafx.h"
#include "MyShell.h"
#include "DlgExeConfirm.h"
#include "ND_Bitmap.h"

#include "Hero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgExeConfirm::CDlgExeConfirm(CWnd* pParent )
	: CDialog(CDlgExeConfirm::IDD, pParent)
{
	m_bShow = false ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_Hbr = CreateSolidBrush ( 0 ) ;
	m_bInitSuc = false ;
	m_bShow = false ;
}


void CDlgExeConfirm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXECONFIRM_BTN_OK, m_BtnOk ) ;
	DDX_Control(pDX, IDC_EXECONFIRM_BTN_HELP, m_BtnHelp ) ;
	DDX_Control(pDX, IDC_EXECONFIRM_BTN_CANCEL, m_BtnCancel ) ;
}


BEGIN_MESSAGE_MAP(CDlgExeConfirm, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_EXECONFIRM_BTN_CANCEL, OnBtnCancel)
	ON_BN_CLICKED(IDC_EXECONFIRM_BTN_OK, OnBtnOk)
	ON_BN_CLICKED(IDC_EXECONFIRM_BTN_HELP, OnBtnHelp)
END_MESSAGE_MAP()

HBRUSH CDlgExeConfirm::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgExeConfirm::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgExeConfirm::GetWindowRect ( rect ) ;

	m_BtnOk.Init ( rect.left, rect.top, "ExeConfirm01" ) ;
	m_BtnCancel.Init ( rect.left, rect.top, "ExeCancel" ) ;
	m_BtnHelp.Init ( rect.left, rect.top, "ExerciseHelp01" ) ;

	m_bInitSuc = true ;
	return TRUE;
}

void CDlgExeConfirm::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

LRESULT CDlgExeConfirm::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_SHOWWINDOW:
		m_bShow = wParam ;
		if ( wParam == 0 )
			DXPlaySound ( "Sound/window_close.wav" ) ;
		else
			DXPlaySound ( "Sound/Open.wav" ) ;
		break ;
	case WM_DESTROY:
		DeleteObject ( m_Hbr ) ;
		break ;
	case WM_PAINT:
		{
			PAINTSTRUCT ps ;
			::BeginPaint ( m_hWnd, &ps ) ;
			::EndPaint ( m_hWnd, &ps ) ;
			return false ;
		}
	case WM_ERASEBKGND:
		return true ;
	case WM_NCPAINT:
		return false ;
	case WM_COMMAND:
		if ( wParam == 1 )
		{
			return true ;
		}
		else if ( wParam == 2 )
		{
			CDlgExeConfirm::OnBtnCancel () ;
			return true ;
		}
		break ;
	default:
		break ;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CDlgExeConfirm::OnBtnCancel()
{
	CDlgExeConfirm::ShowWindow ( SW_HIDE ) ;
	::PostCmd ( CMD_EXERCISE_CONFIRM, 6 ) ;
}

void CDlgExeConfirm::OnBtnOk()
{
	g_objHero.RequestTable ( 1 ) ;
	CDlgExeConfirm::ShowWindow ( SW_HIDE ) ;
	::PostCmd ( CMD_EXERCISE_CONFIRM, 5 ) ;
}

void CDlgExeConfirm::Show()
{
	if ( m_bShow && m_bInitSuc )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
															"PublicInfoBG",
															EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			ShowAni->Show ( 0, m_Pnt.x, m_Pnt.y ) ;

			m_BtnOk.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_BtnCancel.Show ( m_Pnt.x, m_Pnt.y ) ;

			CMySize sizeFont ;
			CMyBitmap::GetFontSize ( sizeFont ) ;
			int nOffset = 20 ;
			char szText[256] ;
			int nStr = 11011 ;
			for ( int i = 0; i < 3; i++ )
			{
				strcpy ( szText, GameDataSetQuery()->GetStr ( nStr ) ) ;
				CMyBitmap::ShowString ( m_Pnt.x + 7,
										m_Pnt.y + nOffset,
										0xff8b00,
										szText,
										GameDataSetQuery()->GetStr ( 50000 ),
										16,
										false ) ;
				nStr++ ;
				nOffset += sizeFont.iHeight * 2 ;
			}
			nStr = 11014 ;
			sprintf ( szText, GameDataSetQuery()->GetStr ( nStr ), m_nTime / 60, m_nTime % 60 ) ;
			CMyBitmap::ShowString ( m_Pnt.x + 7,
									m_Pnt.y + nOffset,
									0xff8b00,
									szText,
									GameDataSetQuery()->GetStr ( 50000 ),
									16,
									false ) ;
		}
	}
}

void CDlgExeConfirm::SetTime ( int nTime )
{
	m_nTime = nTime ;
	CDlgExeConfirm::ShowWindow ( SW_SHOW ) ;
}

void CDlgExeConfirm::OnBtnHelp()
{
	CDlgExeConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_EXECONFIRM ) ;
}
