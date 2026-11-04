#include "stdafx.h"
#include "MyShell.h"
#include "DlgDiceList.h"

#include "Hero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgDiceList::CDlgDiceList(CWnd* pParent )
	: CDialog(CDlgDiceList::IDD, pParent)
{
	m_bShow = false ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
}


void CDlgDiceList::DoDataExchange(CDataExchange* pDX)
{
 	CDialog::DoDataExchange(pDX);
 	DDX_Control(pDX, IDC_DICELIST_LIST, m_DiceListList);
}


BEGIN_MESSAGE_MAP(CDlgDiceList, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
END_MESSAGE_MAP()

LRESULT CDlgDiceList::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
		m_bShow = wParam ;
	else if ( message == WM_COMMAND )
	{
		if ( wParam == 1 || wParam == 2 )
			return true ;
	}
	else if ( message == WM_DESTROY )
		::DeleteObject ( m_Hbr ) ;
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

HBRUSH CDlgDiceList::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;

	return m_Hbr ;
}

BOOL CDlgDiceList::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgDiceList::GetWindowRect ( rect ) ;

	m_DiceListList.Init ( rect.left, rect.top, NULL, SIMPLE_LIST ) ;
	m_DiceListList.SetFontColor ( 0xffff00 ) ;

	return TRUE;
}

void CDlgDiceList::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgDiceList::Show()
{
	if ( m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog44",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0,
							m_Pnt.x,
							m_Pnt.y + 2 ) ;
		else
			return ;

		m_DiceListList.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgDiceList::SetDiceList()
{
	m_DiceListList.ResetContent () ;
	int nAmount = g_objHero.m_objDiceManager.GetPlayerListAmount () ;
	for ( int i = 0; i < nAmount; i++ )
	{
		DicePlayer* pDicePlayer = g_objHero.m_objDiceManager.GetPlayerListByIndex ( i ) ;
		if ( pDicePlayer )
		{
			char strText[32] ;
			sprintf ( strText, "%s  %d", pDicePlayer->szName, pDicePlayer->dwBonus ) ;
			m_DiceListList.AddString ( strText ) ;
		}
	}
}
