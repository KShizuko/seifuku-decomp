#include "stdafx.h"
#include "MyShell.h"
#include "DlgListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgListBox::CDlgListBox(CWnd* pParent )
	: CDialog(CDlgListBox::IDD, pParent)
{
	m_bLoad = false ;
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0 ) ;
	m_Pnt = CPoint ( 0, 0 ) ;
}


void CDlgListBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTBOX, m_ListBox);
}


BEGIN_MESSAGE_MAP(CDlgListBox, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
END_MESSAGE_MAP()

void CDlgListBox::Show()
{
	if ( m_bLoad && m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni, "Dialog24", 2, 30000 ) ;
		if ( ShowAni )
		{
			ShowAni->Show ( 0, m_Pnt.x - 3, m_Pnt.y - 4, 0, 0 ) ;
			m_ListBox.Show ( m_Pnt.x, m_Pnt.y ) ;
		}
	}
}

LRESULT CDlgListBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		if ( wParam == false )
		{
			m_bShow = false ;
		}
		else
		{
			m_bShow = true ;
			m_ListBox.SetFocus () ;
		}
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

HBRUSH CDlgListBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgListBox::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgListBox::GetWindowRect ( rect ) ;

	if ( ! ( m_ListBox.Init ( rect.left, rect.top, NULL, 1 ) ) )
		return false ;
	m_ListBox.SetFontColor ( 0xffff00 ) ;

	m_bLoad = true ;

	return TRUE;
}

void CDlgListBox::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;

}
