#include "stdafx.h"
#include "MyShell.h"
#include "DlgOther.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgOther::CDlgOther(CWnd* pParent )
	: CDialog(CDlgOther::IDD, pParent)
{
	m_bShow = false ;
	m_Pnt = CPoint (0, 0 ) ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
}


void CDlgOther::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OTHER_BTN_LAY, m_OtherBtnLay);
	DDX_Control(pDX, IDC_OTHER_BTN_DEL, m_OtherBtnDel);
	DDX_Control(pDX, IDC_BTN_CLOSEB, m_BtnCloseB);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_BtnClose);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
}


BEGIN_MESSAGE_MAP(CDlgOther, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BTN_CLOSEB, OnBtnCloseb)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBtnClose)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_OTHER_BTN_LAY, OnOtherBtnLay)
	ON_BN_CLICKED(IDC_OTHER_BTN_DEL, OnOtherBtnDel)
END_MESSAGE_MAP()

LRESULT CDlgOther::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

HBRUSH CDlgOther::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;

	return m_Hbr ;
}

BOOL CDlgOther::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgOther::GetWindowRect ( rect ) ;

	m_OtherBtnLay.Init ( rect.left, rect.top, "Button401" ) ;
	m_OtherBtnDel.Init ( rect.left, rect.top, "Button402" ) ;
	m_BtnCloseB.Init ( rect.left, rect.top, "Button3" ) ;
	m_BtnClose.Init ( rect.left, rect.top, "Button5" ) ;
	m_BtnHelp.Init ( rect.left, rect.top, "Button4" ) ;

	return TRUE;
}

void CDlgOther::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgOther::Show()
{
	if ( m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog40",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0,
							m_Pnt.x,
							m_Pnt.y ) ;
		else
			return ;

		m_OtherBtnLay.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_OtherBtnDel.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnCloseB.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgOther::OnBtnCloseb()
{
	CDlgOther::ShowWindow ( SW_HIDE ) ;
}

void CDlgOther::OnBtnClose()
{
	CDlgOther::ShowWindow ( SW_HIDE ) ;
}

void CDlgOther::OnBtnHelp()
{
	CDlgOther::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_OTHER ) ;
}

void CDlgOther::OnOtherBtnLay()
{
}

void CDlgOther::OnOtherBtnDel()
{
	CDlgOther::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_OTHER_DELNPC ) ;
}
