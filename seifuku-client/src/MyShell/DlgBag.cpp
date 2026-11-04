#include "stdafx.h"
#include "MyShell.h"
#include "DlgBag.h"

#include "Hero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgBag::CDlgBag(CWnd* pParent )
	: CDialog(CDlgBag::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_btFrame = 0 ;
}


void CDlgBag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BAG_GRID, m_Grid);
	DDX_Control(pDX, IDC_BAG_BTN_CLOSE, m_BtnClose);
}


BEGIN_MESSAGE_MAP(CDlgBag, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BAG_BTN_CLOSE, OnBtnClose)
END_MESSAGE_MAP()

LRESULT CDlgBag::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
	}
	else if ( message == WM_COMMAND )
	{
		if ( wParam == 1 || wParam == 2 )
			return true ;
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

HBRUSH CDlgBag::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgBag::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgBag::GetWindowRect ( rect ) ;

	m_BtnClose.Init ( rect.left, rect.top, "Button5" ) ;

	return TRUE;
}

void CDlgBag::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgBag::Show()
{
	if ( m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog53",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( m_btFrame, m_Pnt.x, m_Pnt.y ) ;
		else
			return ;

		m_BtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_Grid.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgBag::OnBtnClose()
{
	CDlgBag::ShowWindow ( SW_HIDE ) ;
	g_objHero.ClosePackage () ;
}

void CDlgBag::SetPage( int nPage )
{
	m_btFrame = nPage - 1 ;

	CRect rect ;
	CDlgBag::GetWindowRect ( rect ) ;

	CRgn rgn ;
	rgn.Attach ( ::CreateRectRgn ( 0, 0, 164, 98 + 42 * m_btFrame ) ) ;
	::SetWindowRgn ( m_hWnd, ( HRGN )rgn, true ) ;

	m_Grid.MoveWindow ( 16, 38, 128, 43 * nPage - 1, true ) ;
	m_Grid.Init ( rect.left, rect.top, nPage, 3, DLG_BAG ) ;
}

void CDlgBag::SetPackage()
{
	m_Grid.RemoveIcon ( 0, 0, true ) ;
	for ( int i = 0; i < g_objHero.GetPackageItemAmount (); i++ )
	{
		CItem* pItem = g_objHero.GetPackageItemByIndex ( i ) ;
		if ( pItem )
			m_Grid.InsertIcon ( 1, 1, pItem->GetTypeID (), i, ICON_TYPE_ITEM, 0, true ) ;
	}
}
