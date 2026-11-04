#include "stdafx.h"
#include "MyShell.h"
#include "DlgMsgSend.h"

#include "Hero.h"
#include "GameMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgMsgSend::CDlgMsgSend(CWnd* pParent )
	: CDialog(CDlgMsgSend::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_HbrBlack = ::CreateSolidBrush ( 0 ) ;
	m_FriendID = 0 ;
}


void CDlgMsgSend::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSGSEND_DATA, m_MsgSendData);
	DDX_Control(pDX, IDC_MSGSEND_SEND, m_MsgSendSend);
	DDX_Control(pDX, IDC_MSGSEND_NAME, m_MsgSendName);
	DDX_Control(pDX, IDC_MSGSEND_CLOSEB, m_MsgSendClose);
	DDX_Control(pDX, IDC_MSGSEND_BTN_CLOSES, m_MsgSendCloses);
	DDX_Control(pDX, IDC_MSGSEND_ALLMSG, m_MsgSendAllMsg);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
}


BEGIN_MESSAGE_MAP(CDlgMsgSend, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_MSGSEND_BTN_CLOSES, OnMsgsendCloseb)
	ON_BN_CLICKED(IDC_MSGSEND_CLOSEB, OnMsgsendCloseb)
	ON_BN_CLICKED(IDC_MSGSEND_ALLMSG, OnMsgsendAllmsg)
	ON_BN_CLICKED(IDC_MSGSEND_SEND, OnMsgsendSend)
END_MESSAGE_MAP()

LRESULT CDlgMsgSend::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

HBRUSH CDlgMsgSend::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_HbrBlack ;
}

BOOL CDlgMsgSend::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgMsgSend::GetWindowRect ( rect ) ;

	m_MsgSendSend.Init ( rect.left, rect.top, "Button331" ) ;
	m_MsgSendClose.Init ( rect.left, rect.top, "Button3" ) ;
	m_MsgSendCloses.Init ( rect.left, rect.top, "Button5" ) ;
	m_MsgSendAllMsg.Init ( rect.left, rect.top, "Button330" ) ;
	m_BtnHelp.Init ( rect.left, rect.top, "Button4" ) ;

	m_MsgSendName.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;

	m_MsgSendData.Init ( rect.left, rect.top ) ;
	m_MsgSendData.SetMultiLine ( true ) ;
	m_MsgSendData.SetLimitText ( 256 ) ;
	m_MsgSendData.SetOffset ( CPoint ( 2, 3 ) ) ;

	return TRUE;
}

void CDlgMsgSend::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgMsgSend::Show()
{
	if ( m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog33",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			ShowAni->Show ( 0,
							m_Pnt.x,
							m_Pnt.y ) ;
		}
		else
			return ;

		m_MsgSendSend.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MsgSendClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MsgSendCloses.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MsgSendAllMsg.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_MsgSendName.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_MsgSendData.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgMsgSend::OnBtnHelp()
{
	CDlgMsgSend::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_MSGSEND ) ;
}

void CDlgMsgSend::OnMsgsendCloseb()
{
	CDlgMsgSend::ShowWindow ( SW_HIDE ) ;
}

void CDlgMsgSend::OnMsgsendAllmsg()
{
	CDlgMsgSend::ShowWindow ( SW_HIDE ) ;
	CDlgMsgSend::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSGSEND_VIEW ) ;
}

void CDlgMsgSend::OnMsgsendSend()
{
	char strText[256] ;
	m_MsgSendData.GetWindowText ( strText, 256 ) ;
	if ( strText )
	{
		g_objHero.LeaveWords2Friend ( m_FriendID, strText ) ;
		m_MsgSendData.SetWindowText ( NULL ) ;
		CDlgMsgSend::ShowWindow ( SW_HIDE ) ;
	}
}

void CDlgMsgSend::SetTargetFriend(const char* strName)
{
	if ( strName == NULL )
		return ;
	CFriend* pFriend = g_objHero.GetFriendByName( strName ) ;
	if ( pFriend )
	{
		m_MsgSendName.SetWindowText ( pFriend->GetName () ) ;
		m_FriendID = pFriend->GetID () ;
	}
	if ( g_objGameMsg.m_objChatLog.GetLeaveWordLineAmount() > 0 )
		m_MsgSendAllMsg.EnableWindow ( true ) ;
	else
		m_MsgSendAllMsg.EnableWindow ( false ) ;
}
