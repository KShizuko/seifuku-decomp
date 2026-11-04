#include "stdafx.h"
#include "MyShell.h"
#include "DlgHawk.h"

#include "Hero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgHawk::CDlgHawk(CWnd* pParent )
	: CDialog(CDlgHawk::IDD, pParent)
{
	m_bShow = false ;
	m_Pnt = CPoint (0 ) ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_HbrBlack = ::CreateSolidBrush ( 0 ) ;
}


void CDlgHawk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HAWK_EDT_DATA, m_EdtData);
	DDX_Control(pDX, IDC_HAWK_BTN_CLOSES, m_BtnClose);
	DDX_Control(pDX, IDC_HAWK_BTN_AGREE, m_BtnAgree);
}


BEGIN_MESSAGE_MAP(CDlgHawk, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_HAWK_BTN_CLOSES, OnHawkBtnCloses)
	ON_BN_CLICKED(IDC_HAWK_BTN_AGREE, OnHawkBtnAgree)
END_MESSAGE_MAP()

LRESULT CDlgHawk::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_SHOWWINDOW:
		m_bShow = wParam ;
		if ( m_bShow )
		{
			m_EdtData.SetWindowText ( NULL ) ;
		}
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
		::DeleteObject ( m_HbrBlack ) ;
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

HBRUSH CDlgHawk::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;

	return m_HbrBlack ;
}

BOOL CDlgHawk::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgHawk::GetWindowRect ( rect ) ;

	m_BtnClose.Init ( rect.left, rect.top, "Button5" ) ;
	m_BtnAgree.Init ( rect.left, rect.top, "Button270" ) ;

	m_EdtData.Init ( rect.left, rect.top ) ;
	m_EdtData.SetLimitText ( 64 ) ;

	return TRUE;
}

void CDlgHawk::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;

}

void CDlgHawk::Show()
{
	if ( m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog35",
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
		m_BtnAgree.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_EdtData.Show ( m_Pnt.x, m_Pnt.y ) ;

	}
}

void CDlgHawk::OnHawkBtnCloses()
{
	CDlgHawk::ShowWindow ( SW_HIDE ) ;
}

void CDlgHawk::OnHawkBtnAgree()
{
	if ( m_EdtData.GetWindowTextLength () > 0 )
	{
		char strText[64] ;
		m_EdtData.GetWindowText ( strText, 64 ) ;
		g_objHero.m_objBoothManager.SetCryOut ( strText ) ;
		CDlgHawk::ShowWindow ( SW_HIDE ) ;
	}
}
