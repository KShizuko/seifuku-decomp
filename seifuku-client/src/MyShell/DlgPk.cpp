#include "stdafx.h"
#include "MyShell.h"
#include "DlgPk.h"

#include "Hero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgPk::CDlgPk(CWnd* pParent )
	: CDialog(CDlgPk::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
}


void CDlgPk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PK_ARRESTMENT, m_PkArre);
	DDX_Control ( pDX, IDC_PK_FREE, m_PkFree ) ;
	DDX_Control ( pDX, IDC_PK_GROUP, m_PkGroup ) ;
	DDX_Control ( pDX, IDC_PK_SAFE, m_PkSafe ) ;
}


BEGIN_MESSAGE_MAP(CDlgPk, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_PK_FREE, OnPkFree)
	ON_BN_CLICKED(IDC_PK_GROUP, OnPkGroup)
	ON_BN_CLICKED(IDC_PK_SAFE, OnPkSafe)
	ON_BN_CLICKED(IDC_PK_ARRESTMENT, OnPkArrestment)
END_MESSAGE_MAP()

LRESULT CDlgPk::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
	}
	else if ( message == WM_COMMAND )
	{
		if ( wParam == 1 || wParam == 2 )
		{
			return true ;
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

HBRUSH CDlgPk::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgPk::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgPk::GetWindowRect ( rect ) ;

	m_PkFree.Init ( rect.left, rect.top, "Button47" ) ;
	m_PkGroup.Init ( rect.left, rect.top, "Button49" ) ;
	m_PkSafe.Init ( rect.left, rect.top, "Button48" ) ;
	m_PkArre.Init ( rect.left, rect.top, "Button412" ) ;

	return TRUE;
}

void CDlgPk::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;

}

void CDlgPk::Show()
{
	if ( m_bShow )
	{
		m_PkFree.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_PkGroup.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_PkSafe.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_PkArre.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgPk::OnPkFree()
{
	g_objHero.SetPkMode ( PKMODE_FREE ) ;
	CDlgPk::ShowWindow ( SW_HIDE ) ;
}

void CDlgPk::OnPkGroup()
{
	g_objHero.SetPkMode ( PKMODE_MONSTERONLY ) ;
	CDlgPk::ShowWindow ( SW_HIDE ) ;
}

void CDlgPk::OnPkSafe()
{
	g_objHero.SetPkMode ( PKMODE_TMEXCLUDE ) ;
	CDlgPk::ShowWindow ( SW_HIDE ) ;
}

void CDlgPk::OnPkArrestment()
{
	g_objHero.SetPkMode ( PKMODE_ARRESTMENT ) ;
	CDlgPk::ShowWindow ( SW_HIDE ) ;
}
