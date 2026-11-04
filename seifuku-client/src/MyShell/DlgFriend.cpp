#include "stdafx.h"
#include "MyShell.h"
#include "DlgFriend.h"

#include "Friend.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef _SOUND_DISABLE
	#define DXPlaySound
#endif

CDlgFriend::CDlgFriend(CWnd* pParent )
	: CDialog(CDlgFriend::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_bInitSuc = false ;
	m_fFrameStep = 0.0f ;
	m_nFrameMode = 0 ;
	m_nPntX = 0 ;
	m_nCurIndex = 0 ;
}


void CDlgFriend::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRIEND_BTN_RESUME, m_FriendBtnResume);
	DDX_Control(pDX, IDC_FRIEND_BTN_ADD, m_FriendBtnAdd ) ;
	DDX_Control(pDX, IDC_FRIEND_BTN_CLOSE, m_FriendBtnClose ) ;
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp ) ;
	DDX_Control(pDX, IDC_FRIEND_LST_DATA, m_FriendLstData ) ;
}


BEGIN_MESSAGE_MAP(CDlgFriend, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_FRIEND_BTN_CLOSE, OnFriendBtnClose)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_FRIEND_BTN_ADD, OnFriendBtnAdd)
	ON_BN_CLICKED(IDC_FRIEND_BTN_RESUME, OnFriendBtnResume)
END_MESSAGE_MAP()

LRESULT CDlgFriend::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_SHOWWINDOW:
		m_bShow = wParam ;
		if ( wParam )
		{
			CRect rect ;
			CDlgFriend::GetWindowRect ( rect ) ;
			if ( m_nFrameMode == 0 )
			{
				DXPlaySound ( "Sound/DlgOpen.wav" ) ;
				if ( m_FriendLstData.GetCount () == 0 )
					CDlgFriend::SetFriendList () ;
				m_dwLastShowTime = 0 ;
				m_nFrameMode = 1 ;
				CDlgFriend::MoveWindow ( -rect.Width(), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
			}
			else if ( m_nFrameMode == 2 )
			{
				CDlgFriend::MoveWindow ( 0, m_Pnt.y, rect.Width(), rect.Height(), true ) ;
				m_nFrameMode = 0 ;
			}
		}
		else
		{
			DXPlaySound ( "Sound/DlgClose.wav" ) ;
			if ( m_DlgFrdState.m_bShow )
				m_DlgFrdState.ShowWindow ( SW_HIDE ) ;
		}
		break ;
	case WM_COMMAND:
		if ( wParam == 1 )
		{
			return true ;
		}
		else if ( wParam == 2 )
		{
			CDlgFriend::OnFriendBtnClose () ;
			return true ;
		}
		break ;
	case WM_DESTROY:
		::DeleteObject ( m_Hbr ) ;
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
	case LB_SETSEL:
		{
			if ( wParam == 1 )
			{
				CDlgFriend::GetParent()->PostMessage(WM_MY_MESSAGE, ON_FRIEND_CHAT, lParam);
			}
			else if ( wParam == 2 )
			{
				CDlgFriend::GetParent()->PostMessage(WM_MY_MESSAGE, ON_FRIEND_LEAVE, lParam);
			}
			else if ( wParam == 0 )
			{
				char strFriendName[16] ;
				m_nCurIndex = lParam ;
				m_FriendLstData.GetText ( lParam, strFriendName ) ;
				CFriend* pFriend = g_objHero.GetFriendByName ( strFriendName ) ;
				if ( pFriend )
				{
					g_objHero.QueryFriend ( pFriend->GetID() ) ;
				}
			}
			else if ( wParam == -1 )
			{
				if ( lParam != -1 )
				{
					char strFriendName[16] ;
					m_FriendLstData.GetText ( lParam, strFriendName ) ;
					m_DlgFrdState.SetFriendNull ( strFriendName ) ;
					if ( !m_DlgFrdState.m_bShow )
						m_DlgFrdState.ShowWindow ( SW_SHOW ) ;
				}
				else
				{
					if ( m_DlgFrdState.m_bShow )
						m_DlgFrdState.ShowWindow ( SW_HIDE ) ;
				}
			}
		}
		break ;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

HBRUSH CDlgFriend::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgFriend::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgFriend::GetWindowRect ( rect ) ;
	m_fFrameStep = ( float ) rect.Width () / 20 ;

	if ( ! m_FriendBtnAdd.Init ( rect.left, rect.top, "Button170" ) )
		return false ;
	if ( ! m_FriendBtnClose.Init ( rect.left, rect.top, "Button5" ) )
		return false ;
	if ( ! m_BtnHelp.Init ( rect.left, rect.top, "Button444" ) )
		return false ;
	m_FriendBtnResume.Init ( rect.left, rect.top, "Button175" ) ;

	if ( ! m_FriendLstData.Init ( rect.left, rect.top, NULL, 2 ) )
		return false ;
	m_FriendLstData.SetFontColor ( 0xffff00 ) ;
	m_FriendLstData.SetParentCob ( this ) ;

	m_DlgFrdState.Create ( IDD_DIALOG_FRDSTATE, CDlgFriend::GetParent () ) ;
	CRect Temprect ;
	m_DlgFrdState.GetWindowRect ( Temprect ) ;
	m_DlgFrdState.MoveWindow ( rect.Width () - 2,
							   65,
							   Temprect.Width (),
							   Temprect.Height () ) ;

	m_bInitSuc = true ;
	return TRUE;
}

void CDlgFriend::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_nPntX = x ;
	m_Pnt.y = y ;

}

void CDlgFriend::OnFriendBtnClose()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;

	CDlgFriend::ShowWindow ( SW_HIDE ) ;
}

void CDlgFriend::Show()
{
	if ( !m_bInitSuc )
		return ;
	if ( m_bShow )
	{
		if ( m_nFrameMode == 1 )
		{
			if ( m_dwLastShowTime == 0 )
			{
				m_dwLastShowTime = TimeGet() ;
				m_Pnt.x = m_nPntX ;
			}
			else
			{
				if ( TimeGet() - m_dwLastShowTime <= 300 )
				{
					m_Pnt.x = m_nPntX ;
					m_Pnt.x += ( TimeGet() - m_dwLastShowTime ) / 15 * m_fFrameStep ;
				}
				else
				{
					m_Pnt.x = 0;
					m_nFrameMode = 2 ;
					CDlgFriend::WindowProc ( WM_SHOWWINDOW, true, NULL ) ;
				}
			}
		}

		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														  "Dialog17",
														  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0,
							m_Pnt.x,
							m_Pnt.y ) ;
		else
			return ;

		m_FriendLstData.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_FriendBtnAdd.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_FriendBtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_FriendBtnResume.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_DlgFrdState.m_bShow )
			m_DlgFrdState.Show() ;
	}
}

void CDlgFriend::OnBtnHelp()
{
	CDlgFriend::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_FRIEND ) ;
}

void CDlgFriend::OnFriendBtnAdd()
{
	CDlgFriend::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FRIEND_ADD ) ;
}

void CDlgFriend::SetFriendList()
{
	m_FriendLstData.ResetContent ( ) ;

	int nAmount = g_objHero.GetFriendAmount () ;
	int nOnline = 0 ;
	int i ;
	for ( i = 0; i < nAmount; i++)
	{
		CFriend* pFriend = g_objHero.GetFriendByIndex ( i ) ;
		if ( pFriend && pFriend->IsOnline () )
			nOnline++ ;
	}
	int nOffLine = nOnline ;

	for ( i = 0; i < nAmount; i++)
	{
		CFriend* pFriend	= g_objHero.GetFriendByIndex(i);
		if (pFriend)
		{
			char strListData[64] ;
			strcpy ( strListData, pFriend->GetName () ) ;
			if ( pFriend->IsOnline () )
			{
				m_FriendLstData.InsertString ( 0, strListData ) ;
				m_FriendLstData.SetListRealIndex ( nOnline - 1, 1 ) ;
				nOnline-- ;
			}
			else
			{
				m_FriendLstData.AddString(strListData);
				m_FriendLstData.SetListRealIndex ( nOffLine, -1 ) ;
				nOffLine++ ;
			}
		}
	}
}

void CDlgFriend::SetFriendState()
{
	char strFriendName[16] ;
	m_FriendLstData.GetText ( m_nCurIndex, strFriendName ) ;
	if ( strFriendName != NULL )
	{
		m_DlgFrdState.SetFriendState ( strFriendName ) ;
		if ( !m_DlgFrdState.m_bShow )
			m_DlgFrdState.ShowWindow ( SW_SHOW ) ;
	}
}

void CDlgFriend::OnFriendBtnResume()
{
	CDlgFriend::ShowWindow ( SW_HIDE ) ;
	CDlgFriend::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FRIEND_RESUME, 0 ) ;
}
