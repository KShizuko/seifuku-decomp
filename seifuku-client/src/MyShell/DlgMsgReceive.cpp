#include "stdafx.h"
#include "MyShell.h"
#include "DlgMsgReceive.h"

#include "GameMsg.h"
#include "Hero.h"
#include "Friend.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgMsgReceive::CDlgMsgReceive(CWnd* pParent )
	: CDialog(CDlgMsgReceive::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_HbrBlack = ::CreateSolidBrush ( 0 ) ;
	m_nCurIndex = 0 ;
}


void CDlgMsgReceive::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSGRECEIVE_DELALL, m_MsgReceiveDelall);
	DDX_Control(pDX, IDC_MSGRECEIVE_PRE, m_MsgReceivePre);
	DDX_Control(pDX, IDC_MSGRECEIVE_DATA, m_MsgReceiveData);
	DDX_Control(pDX, IDC_MSGRECEIVE_NAME, m_MsgReceiveName);
	DDX_Control(pDX, IDC_MSGRECEIVE_CLOSEB, m_MsgReceiveClose);
	DDX_Control(pDX, IDC_MSGRECEIVE_BTN_CLOSES, m_MsgReceiveCloses);
	DDX_Control(pDX, IDC_MSGRECEIVE_NEXT, m_MsgReceiveNext);
	DDX_Control(pDX, IDC_MSGRECEIVE_REVERT, m_MsgReceiveRevert);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
}


BEGIN_MESSAGE_MAP(CDlgMsgReceive, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_MSGRECEIVE_BTN_CLOSES, OnMsgreceiveCloseb)
	ON_BN_CLICKED(IDC_MSGRECEIVE_CLOSEB, OnMsgreceiveCloseb)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_MSGRECEIVE_REVERT, OnMsgreceiveRevert)
	ON_BN_CLICKED(IDC_MSGRECEIVE_NEXT, OnMsgreceiveNext)
	ON_BN_CLICKED(IDC_MSGRECEIVE_PRE, OnMsgreceivePre)
	ON_BN_CLICKED(IDC_MSGRECEIVE_DELALL, OnMsgreceiveDelall)
END_MESSAGE_MAP()

LRESULT CDlgMsgReceive::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

HBRUSH CDlgMsgReceive::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_HbrBlack ;
}

BOOL CDlgMsgReceive::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgMsgReceive::GetWindowRect ( rect ) ;

	m_MsgReceiveClose.Init ( rect.left, rect.top, "Button3" ) ;
	m_MsgReceiveCloses.Init ( rect.left, rect.top, "Button5" ) ;
	m_MsgReceiveRevert.Init ( rect.left, rect.top, "Button332" ) ;
	m_MsgReceiveNext.Init ( rect.left, rect.top, "Button335" ) ;
	m_MsgReceivePre.Init ( rect.left, rect.top, "Button334" ) ;
	m_MsgReceiveDelall.Init ( rect.left, rect.top, "Button373" ) ;
	m_BtnHelp.Init ( rect.left, rect.top, "Button4" ) ;

	m_MsgReceiveName.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;

	m_MsgReceiveData.Init ( rect.left, rect.top, NULL, 0x000000, 0xffffff ) ;
	m_MsgReceiveData.SetMultiLine ( true ) ;
	m_MsgReceiveData.SetLimitText ( 256 ) ;
	m_MsgReceiveData.SetOffset ( CPoint ( 2, 3 ) ) ;
	return TRUE;
}

void CDlgMsgReceive::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;

}

void CDlgMsgReceive::Show()
{
	if ( m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog39",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			ShowAni->Show ( 0,
							m_Pnt.x,
							m_Pnt.y ) ;
		}
		else
			return ;

		m_MsgReceiveClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MsgReceiveCloses.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MsgReceiveRevert.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MsgReceiveNext.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MsgReceivePre.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MsgReceiveDelall.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_MsgReceiveName.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MsgReceiveData.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}


void CDlgMsgReceive::OnMsgreceiveCloseb()
{
	CDlgMsgReceive::ShowWindow ( SW_HIDE ) ;
}

void CDlgMsgReceive::OnBtnHelp()
{
	CDlgMsgReceive::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_MSGRECEIVE ) ;
}

void CDlgMsgReceive::OnMsgreceiveRevert()
{
	CDlgMsgReceive::ShowWindow ( SW_HIDE ) ;
	char* pszName = new char[32] ;
	m_MsgReceiveName.GetWindowText ( pszName, 32 ) ;
	CDlgMsgReceive::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSGREC_REVERT, ( LPARAM ) pszName ) ;
}

void CDlgMsgReceive::SetReceiveMsg()
{
	m_nCurIndex = 0 ;
	m_MsgReceivePre.EnableWindow ( false ) ;
	m_MsgReceiveNext.EnableWindow ( false ) ;
	m_MsgReceiveRevert.EnableWindow ( false ) ;
	if ( g_objGameMsg.m_objChatLog.GetLeaveWordLineAmount () > 0 )
	{
		m_MsgReceiveNext.EnableWindow ( true ) ;
		ChatLogLine* pLine = g_objGameMsg.m_objChatLog.GetLeaveWordLineByIndex ( 0 ) ;
		if ( pLine )
		{
			CFriend* pFriend = g_objHero.GetFriendByName ( pLine->szSender ) ;
			if ( pFriend && !pFriend->IsOnline () )
				m_MsgReceiveRevert.EnableWindow ( true ) ;
			m_MsgReceiveName.SetWindowText ( pLine->szSender ) ;
			m_MsgReceiveData.SetWindowText ( pLine->szTxt ) ;
		}
	}
	else
	{
		m_MsgReceiveName.SetWindowText ( NULL ) ;
		m_MsgReceiveData.SetWindowText ( NULL ) ;
	}
}

void CDlgMsgReceive::OnMsgreceivePre()
{
	if ( m_nCurIndex > 0 )
	{
		m_MsgReceiveRevert.EnableWindow ( false ) ;
		if ( !m_MsgReceiveNext.IsWindowEnabled () )
			m_MsgReceiveNext.EnableWindow ( true ) ;
		m_nCurIndex-- ;
		ChatLogLine* pLine = g_objGameMsg.m_objChatLog.GetLeaveWordLineByIndex ( m_nCurIndex ) ;
		if ( pLine )
		{
			CFriend* pFriend = g_objHero.GetFriendByName ( pLine->szSender ) ;
			if ( pFriend && !pFriend->IsOnline () )
				m_MsgReceiveRevert.EnableWindow ( true ) ;
			m_MsgReceiveName.SetWindowText ( pLine->szSender ) ;
			m_MsgReceiveData.SetWindowText ( pLine->szTxt ) ;
		}
	}
	else
		m_MsgReceivePre.EnableWindow ( false ) ;
}

void CDlgMsgReceive::OnMsgreceiveNext()
{
	if ( m_nCurIndex + 1 < g_objGameMsg.m_objChatLog.GetLeaveWordLineAmount () )
	{
		m_MsgReceiveRevert.EnableWindow ( false ) ;
		if ( !m_MsgReceivePre.IsWindowEnabled () )
			m_MsgReceivePre.EnableWindow ( true ) ;
		m_nCurIndex++ ;
		ChatLogLine* pLine = g_objGameMsg.m_objChatLog.GetLeaveWordLineByIndex ( m_nCurIndex ) ;
		if ( pLine )
		{
			CFriend* pFriend = g_objHero.GetFriendByName ( pLine->szSender ) ;
			if ( pFriend && !pFriend->IsOnline () )
				m_MsgReceiveRevert.EnableWindow ( true ) ;
			m_MsgReceiveName.SetWindowText ( pLine->szSender ) ;
			m_MsgReceiveData.SetWindowText ( pLine->szTxt ) ;
		}
	}
	else
		m_MsgReceiveNext.EnableWindow ( false ) ;
}

void CDlgMsgReceive::OnMsgreceiveDelall()
{
	g_objGameMsg.m_objChatLog.DelLeaveWordLogFile () ;
}
