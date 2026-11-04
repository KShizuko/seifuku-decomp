#include "stdafx.h"
#include "MyShell.h"
#include "MenuTalk.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMenuTalk::CMenuTalk(CWnd* pParent )
	: CDialog(CMenuTalk::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_ParentWnd = NULL ;
}


void CMenuTalk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TALK_BTN_GM, m_TalkBtnGM ) ;
	DDX_Control(pDX, IDC_TALK_BTN_G, m_TalkBtnG ) ;
	DDX_Control(pDX, IDC_TALK_BTN_S, m_TalkBtnS ) ;
	DDX_Control(pDX, IDC_TALK_BTN_D, m_TalkBtnD ) ;
	DDX_Control(pDX, IDC_TALK_BTN_B, m_TalkBtnB ) ;
	DDX_Control(pDX, IDC_TALK_BTN_H, m_TalkBtnH ) ;
}


BEGIN_MESSAGE_MAP(CMenuTalk, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_TALK_BTN_G, OnTalkBtnG)
	ON_BN_CLICKED(IDC_TALK_BTN_S, OnTalkBtnS)
	ON_BN_CLICKED(IDC_TALK_BTN_D, OnTalkBtnD)
	ON_BN_CLICKED(IDC_TALK_BTN_B, OnTalkBtnB)
	ON_BN_CLICKED(IDC_TALK_BTN_H, OnTalkBtnH)
	ON_BN_CLICKED(IDC_TALK_BTN_GM, OnTalkBtnGM)
END_MESSAGE_MAP()

LRESULT CMenuTalk::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
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

HBRUSH CMenuTalk::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CMenuTalk::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CMenuTalk::GetWindowRect ( rect ) ;

	if ( ! ( m_TalkBtnG.Init ( rect.left, rect.top, "Button50" ) ) )
		return false ;
	if ( ! ( m_TalkBtnS.Init ( rect.left, rect.top, "Button51" ) ) )
		return false ;
	if ( ! ( m_TalkBtnD.Init ( rect.left, rect.top, "Button52" ) ) )
		return false ;
	if ( ! ( m_TalkBtnB.Init ( rect.left, rect.top, "Button53" ) ) )
		return false ;
	if ( ! ( m_TalkBtnH.Init ( rect.left, rect.top, "Button54" ) ) )
		return false ;
	if ( ! ( m_TalkBtnGM.Init ( rect.left, rect.top, "Button55" ) ) )
		return false ;

	return TRUE;
}

void CMenuTalk::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;

}

void CMenuTalk::Show()
{
	if ( m_bShow )
	{
		m_TalkBtnG.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TalkBtnS.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TalkBtnD.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TalkBtnB.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TalkBtnH.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TalkBtnGM.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CMenuTalk::SetParentWnd(CWnd *pParentWnd)
{
	m_ParentWnd = pParentWnd ;
}

void CMenuTalk::OnTalkBtnG()
{
	if ( m_ParentWnd )
	{
		m_ParentWnd->PostMessage ( WM_MY_MESSAGE, MENU_TALK_CLICK, G_CLICK ) ;
	}
	CMenuTalk::ShowWindow ( SW_HIDE ) ;
}

void CMenuTalk::OnTalkBtnS()
{
	if ( m_ParentWnd )
	{
		m_ParentWnd->PostMessage ( WM_MY_MESSAGE, MENU_TALK_CLICK, S_CLICK ) ;
	}
	CMenuTalk::ShowWindow ( SW_HIDE ) ;
}

void CMenuTalk::OnTalkBtnD()
{
	if ( m_ParentWnd )
	{
		m_ParentWnd->PostMessage ( WM_MY_MESSAGE, MENU_TALK_CLICK, D_CLICK ) ;
	}
	CMenuTalk::ShowWindow ( SW_HIDE ) ;
}

void CMenuTalk::OnTalkBtnB()
{
	if ( m_ParentWnd )
	{
		m_ParentWnd->PostMessage ( WM_MY_MESSAGE, MENU_TALK_CLICK, B_CLICK ) ;
	}
	CMenuTalk::ShowWindow ( SW_HIDE ) ;
}

void CMenuTalk::OnTalkBtnH()
{
	if ( m_ParentWnd )
	{
		m_ParentWnd->PostMessage ( WM_MY_MESSAGE, MENU_TALK_CLICK, H_CLICK ) ;
	}
	CMenuTalk::ShowWindow ( SW_HIDE ) ;
}

void CMenuTalk::OnTalkBtnGM()
{
	if ( m_ParentWnd )
	{
		m_ParentWnd->PostMessage ( WM_MY_MESSAGE, MENU_TALK_CLICK, GM_CLICK ) ;
	}
	CMenuTalk::ShowWindow ( SW_HIDE ) ;
}
