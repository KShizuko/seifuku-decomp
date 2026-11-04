#include "stdafx.h"
#include "MyShell.h"
#include "MenuGroup.h"

#include "Hero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMenuGroup::CMenuGroup(CWnd* pParent )
	: CDialog(CMenuGroup::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_ParentWnd = NULL ;
	m_bCreate = false ;
}


void CMenuGroup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GROUP_CHK_STOP, m_GroupChkStop);
	DDX_Control(pDX, IDC_GROUP_CHK_MONEY, m_GroupChkMoney);
	DDX_Control(pDX, IDC_GROUP_CHK_ITEM, m_GroupChkItem);
}


BEGIN_MESSAGE_MAP(CMenuGroup, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_GROUP_CHK_STOP, OnGroupChkStop)
	ON_BN_CLICKED(IDC_GROUP_CHK_MONEY, OnGroupChkMoney)
	ON_BN_CLICKED(IDC_GROUP_CHK_ITEM, OnGroupChkItem)
END_MESSAGE_MAP()

LRESULT CMenuGroup::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
		if ( m_bShow && m_bCreate )
		{
			m_bCreate = false ;
			m_GroupChkMoney.SetCheckValue ( 1 ) ;
			g_objHero.SetTeamMoneyAccess ( true ) ;
			m_GroupChkItem.SetCheckValue ( 0 ) ;
			g_objHero.SetTeamItemAccess ( false ) ;
		}
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

	return CDialog::WindowProc(message, wParam, lParam);
}

HBRUSH CMenuGroup::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CMenuGroup::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CMenuGroup::GetWindowRect ( rect ) ;

	m_GroupChkStop.Init ( rect.left, rect.top, "Check190", 2, 0 ) ;
	m_GroupChkMoney.Init ( rect.left, rect.top, "Check191", 2, 0 ) ;
	m_GroupChkItem.Init ( rect.left, rect.top, "Check192", 2, 0 ) ;

	return TRUE;
}

void CMenuGroup::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CMenuGroup::Show()
{
	if ( m_bShow )
	{
		m_GroupChkStop.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_GroupChkMoney.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_GroupChkItem.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CMenuGroup::OnGroupChkStop()
{
	if ( m_ParentWnd )
		m_ParentWnd->PostMessage ( WM_MY_MESSAGE, MENU_GROUP_CLICK, GEAR_CLICK ) ;
}

void CMenuGroup::OnGroupChkMoney()
{
	if ( m_ParentWnd )
		m_ParentWnd->PostMessage ( WM_MY_MESSAGE, MENU_GROUP_CLICK, MONEY_CLICK ) ;
}

void CMenuGroup::OnGroupChkItem()
{
	if ( m_ParentWnd )
		m_ParentWnd->PostMessage ( WM_MY_MESSAGE, MENU_GROUP_CLICK, ITEM_CLICK ) ;
}

void CMenuGroup::SetParentWnd(CWnd *pParentWnd)
{
	m_ParentWnd = pParentWnd ;
}
