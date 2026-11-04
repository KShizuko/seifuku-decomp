#include "stdafx.h"
#include "MyShell.h"
#include "DlgFrdState.h"

#include "Hero.h"
#include "Friend.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgFrdState::CDlgFrdState(CWnd* pParent )
	: CDialog(CDlgFrdState::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
}


void CDlgFrdState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRDSTATE_IMAFACE, m_FrdStateImaface);
	DDX_Control(pDX, IDC_STA6, m_Sta6);
	DDX_Control(pDX, IDC_STA5, m_Sta5);
	DDX_Control(pDX, IDC_STA1, m_Sta1);
	DDX_Control(pDX, IDC_STA2, m_Sta2);
	DDX_Control(pDX, IDC_STA3, m_Sta3);
	DDX_Control(pDX, IDC_STA4, m_Sta4);
	DDX_Control(pDX, IDC_STA7, m_Sta7);
	DDX_Control(pDX, IDC_FRDSTATE_BTN_DEL, m_FrdStateBtnDel);
	DDX_Control(pDX, IDC_FRDSTATE_BTN_CLOSE, m_FrdStateBtnClose);
}


BEGIN_MESSAGE_MAP(CDlgFrdState, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_FRDSTATE_BTN_CLOSE, OnFrdstateBtnClose)
	ON_BN_CLICKED(IDC_FRDSTATE_BTN_DEL, OnFrdstateBtnDel)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
END_MESSAGE_MAP()

LRESULT CDlgFrdState::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
		if ( wParam )
		{
			CDlgFrdState::GetParent()->PostMessage ( WM_MY_MESSAGE, ON_FRDSTATE_SHOW ) ;
			DXPlaySound ( "Sound/BtnClick.wav" ) ;
		}
	}
	else if ( message == WM_COMMAND )
	{
		if ( wParam == 1 )
		{
			return true ;
		}
		else if ( wParam == 2 )
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

HBRUSH CDlgFrdState::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgFrdState::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgFrdState::GetWindowRect ( rect ) ;

	if ( !m_FrdStateImaface.Init ( rect.left, rect.top, NULL, FACE_IMAGE ) )
		return false ;

	m_Sta1.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_Sta2.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_Sta3.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_Sta4.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_Sta5.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_Sta6.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_Sta7.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;

	if ( ! m_FrdStateBtnDel.Init ( rect.left, rect.top, "Button250" ) )
		return false ;
	if ( ! m_FrdStateBtnClose.Init ( rect.left, rect.top, "Button5" ) )
		return false ;

	return TRUE;
}

void CDlgFrdState::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;

}

void CDlgFrdState::Show()
{
	if ( m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog25",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0,
							m_Pnt.x,
							m_Pnt.y + 1 ) ;
		else
			return ;

		m_FrdStateImaface.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_Sta1.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_Sta2.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_Sta3.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_Sta4.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_Sta5.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_Sta6.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_Sta7.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_FrdStateBtnDel.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_FrdStateBtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgFrdState::OnFrdstateBtnClose()
{
	CDlgFrdState::ShowWindow ( SW_HIDE ) ;
}

void CDlgFrdState::OnFrdstateBtnDel()
{
	CDlgFrdState::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FRDSTATE_DEL ) ;
}

void CDlgFrdState::OnBtnHelp()
{
}

void CDlgFrdState::SetFriendState(char* strFriendName)
{
	CFriend* pFriend = g_objHero.GetFriendByName ( strFriendName ) ;
	if ( pFriend != NULL )
	{
		m_FrdStateImaface.RemoveImage () ;
		char strText[16] ;
		itoa ( pFriend->GetFace (), strText, 10 ) ;
		m_FrdStateImaface.InsertImage ( strText ) ;
		m_Sta1.SetWindowText ( pFriend->GetName () ) ;
		itoa ( pFriend->GetLevel (), strText, 10 ) ;
		m_Sta2.SetWindowText ( strText ) ;
		m_Sta3.SetWindowText ( pFriend->GetProfessionalName () ) ;
		itoa ( pFriend->GetPK (), strText, 10 ) ;
		m_Sta4.SetWindowText ( strText ) ;
		strcpy ( strText, pFriend->GetSyndicateName () ) ;
		m_Sta5.SetWindowText ( strText ) ;
		strcpy ( strText, pFriend->GetRankName () ) ;
		m_Sta6.SetWindowText ( strText ) ;
		m_Sta7.SetWindowText ( pFriend->GetMate () ) ;
	}
}

void CDlgFrdState::DeleteFriend()
{
	char strName[16] ;
	m_Sta1.GetWindowText ( strName, 16 ) ;
	if ( strName != NULL )
	{
		CFriend* pFriend = g_objHero.GetFriendByName ( strName ) ;
		if ( pFriend )
			g_objHero.BreakOffFriend ( pFriend->GetID () ) ;
	}
	CDlgFrdState::ShowWindow ( SW_HIDE ) ;
}

void CDlgFrdState::SetFriendNull(char *strName)
{
	m_FrdStateImaface.RemoveImage () ;
	m_Sta1.SetWindowText ( strName ) ;
	m_Sta2.SetWindowText ( NULL ) ;
	m_Sta3.SetWindowText ( NULL ) ;
	m_Sta4.SetWindowText ( NULL ) ;
	m_Sta5.SetWindowText ( NULL ) ;
	m_Sta6.SetWindowText ( NULL ) ;
	m_Sta7.SetWindowText ( NULL ) ;
}
