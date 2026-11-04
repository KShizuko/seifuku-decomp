#include "stdafx.h"
#include "MyShell.h"
#include "DlgGroup.h"

#include "Hero.h"
#include "GamePlayerSet.h"
#include "GameMsg.h"
#include "3DGameMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgGroup::CDlgGroup(CWnd* pParent )
	: CDialog(CDlgGroup::IDD, pParent)
{
	m_bShow = false ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_uTeamMode = TEAM_MODE_NONE ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_bInitSuc = false ;
	m_uFocusPlayer = 0 ;
}


void CDlgGroup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GROUP_CHK_3, m_GroupBtn3);
	DDX_Control(pDX, IDC_GROUP_IMG1, m_GroupImg1);
	DDX_Control(pDX, IDC_GROUP_IMG2, m_GroupImg2);
	DDX_Control(pDX, IDC_GROUP_IMG3, m_GroupImg3);
	DDX_Control(pDX, IDC_GROUP_IMG4, m_GroupImg4);
	DDX_Control(pDX, IDC_GROUP_IMG5, m_GroupImg5);
	DDX_Control(pDX, IDC_GROUP_BTN_1, m_GroupBtn1);
	DDX_Control(pDX, IDC_GROUP_BTN_2, m_GroupBtn2);
	DDX_Control(pDX, IDC_GROUP_BTN_4, m_GroupBtn4);
	DDX_Control(pDX, IDC_GROUP_BTN_CLOSE, m_GroupCloseBtn);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
}


BEGIN_MESSAGE_MAP(CDlgGroup, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_GROUP_BTN_CLOSE, OnGroupBtnClose)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_GROUP_BTN_1, OnGroupBtn1)
	ON_BN_CLICKED(IDC_GROUP_BTN_2, OnGroupBtn2)
	ON_BN_CLICKED(IDC_GROUP_CHK_3, OnGroupChk3)
	ON_BN_CLICKED(IDC_GROUP_BTN_4, OnGroupBtn4)
	ON_BN_CLICKED(IDC_GROUP_IMG1, OnGroupImg1)
	ON_BN_CLICKED(IDC_GROUP_IMG2, OnGroupImg2)
	ON_BN_CLICKED(IDC_GROUP_IMG3, OnGroupImg3)
	ON_BN_CLICKED(IDC_GROUP_IMG4, OnGroupImg4)
	ON_BN_CLICKED(IDC_GROUP_IMG5, OnGroupImg5)
END_MESSAGE_MAP()

LRESULT CDlgGroup::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
		if ( !m_bShow )
		{
			if ( m_MenuGroup.m_bShow )
				m_MenuGroup.ShowWindow( SW_HIDE ) ;
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
			CDlgGroup::OnGroupBtnClose () ;
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
	else if ( message == WM_MY_MESSAGE )
	{
		if ( wParam == MENU_GROUP_CLICK )
		{
			switch ( lParam )
			{
			case GEAR_CLICK:
				{
					if (m_MenuGroup.m_GroupChkStop.GetCheckValue())
						g_objHero.TeamClose();
					else
						g_objHero.TeamOpen();
				}
				break ;
			case MONEY_CLICK:
				g_objHero.SetTeamMoneyAccess ( m_MenuGroup.m_GroupChkMoney.GetCheckValue () ) ;
				break ;
			case ITEM_CLICK:
				g_objHero.SetTeamItemAccess ( m_MenuGroup.m_GroupChkItem.GetCheckValue () ) ;
				break ;
			}
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CDlgGroup::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgGroup::GetWindowRect ( rect ) ;

	if ( ! m_GroupImg1.Init ( rect.left, rect.top, NULL, FACE_IMAGE) )
		return false ;
	if ( ! m_GroupImg2.Init ( rect.left, rect.top, NULL, FACE_IMAGE) )
		return false ;
	if ( ! m_GroupImg3.Init ( rect.left, rect.top, NULL, FACE_IMAGE) )
		return false ;
	if ( ! m_GroupImg4.Init ( rect.left, rect.top, NULL, FACE_IMAGE) )
		return false ;
	if ( ! m_GroupImg5.Init ( rect.left, rect.top, NULL, FACE_IMAGE) )
		return false ;

	if ( ! m_GroupBtn1.Init ( rect.left, rect.top, "Button190" ) )
		return false ;
	if ( ! m_GroupBtn2.Init ( rect.left, rect.top, "Button191" ) )
		return false ;
	if ( ! m_GroupBtn3.Init ( rect.left, rect.top, "Button197" ) )
		return false ;
	if ( ! m_GroupBtn4.Init ( rect.left, rect.top, "Button195" ) )
		return false ;
	if ( ! m_GroupCloseBtn.Init ( rect.left, rect.top, "Button5" ) )
		return false ;
	if ( ! m_BtnHelp.Init ( rect.left, rect.top, "Button444" ) )
		return false ;

	if ( ! m_MenuGroup.Create ( IDD_MENU_GROUP, this->GetParent () ) )
		return false ;
	m_MenuGroup.SetParentWnd ( this ) ;

	m_bInitSuc = true ;
	return TRUE;
}

HBRUSH CDlgGroup::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

void CDlgGroup::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
	if ( m_bInitSuc )
	{
		CRect rect, Mrect ;
		CDlgGroup::GetWindowRect ( rect ) ;
		m_MenuGroup.GetWindowRect ( Mrect ) ;
		m_MenuGroup.MoveWindow ( m_Pnt.x + 130,
								 m_Pnt.y + rect.Height () + 10,
								 Mrect.Width (),
								 Mrect.Height (), true ) ;
	}
}

void CDlgGroup::Show()
{
	if ( !m_bInitSuc )
		return ;
	if ( m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														  "Dialog19",
														  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			ShowAni->Show( 0,
			m_Pnt.x,
			m_Pnt.y ) ;
		}
		else
			return ;

		m_GroupImg1.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_GroupImg2.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_GroupImg3.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_GroupImg4.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_GroupImg5.Show ( m_Pnt.x, m_Pnt.y ) ;
		switch(m_uFocusPlayer)
		{
		case 1:
			m_GroupImg1.ShowFocus();
			break;
		case 2:
			m_GroupImg2.ShowFocus();
			break;
		case 3:
			m_GroupImg3.ShowFocus();
			break;
		case 4:
			m_GroupImg4.ShowFocus();
			break;
		case 5:
			m_GroupImg5.ShowFocus();
			break;
		default:
			break;
		}

		m_GroupBtn1.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_GroupBtn2.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_uTeamMode == TEAM_MODE_LEADER )
		{
			m_GroupBtn3.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_GroupBtn4.Show ( m_Pnt.x, m_Pnt.y ) ;
		}
		m_GroupCloseBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;

		if ( m_MenuGroup.m_bShow )
			m_MenuGroup.Show () ;
	}
}

void CDlgGroup::OnGroupBtnClose()
{
	CDlgGroup::ShowWindow( SW_HIDE ) ;
}

void CDlgGroup::OnBtnHelp()
{
	CDlgGroup::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_GROUP ) ;
}

void CDlgGroup::SetGroupContain()
{
	if ( m_MenuGroup.m_bShow )
		m_MenuGroup.ShowWindow ( SW_HIDE ) ;

	m_GroupImg1.RemoveImage ( ) ;
	m_GroupImg2.RemoveImage ( ) ;
	m_GroupImg3.RemoveImage ( ) ;
	m_GroupImg4.RemoveImage ( ) ;
	m_GroupImg5.RemoveImage ( ) ;

	int i ;
	for ( i = 0; i < g_objHero.GetTeamMemberAmount (); i++ )
	{
		TeamMemberInfo *pTeamInfo = g_objHero.GetTeamMemberInfo ( i ) ;
		char strFace[16] ;
		itoa ( pTeamInfo->dwFace, strFace, 10 ) ;
		switch ( i )
		{
		case 0:
			m_GroupImg1.InsertImage ( strFace, 0, false ) ;
			break ;
		case 1:
			m_GroupImg2.InsertImage ( strFace, 0, false ) ;
			break ;
		case 2:
			m_GroupImg3.InsertImage ( strFace, 0, false ) ;
			break ;
		case 3:
			m_GroupImg4.InsertImage ( strFace, 0, false ) ;
			break ;
		case 4:
			m_GroupImg5.InsertImage ( strFace, 0, false ) ;
			break ;
		}
		m_PlayerID[i] = pTeamInfo->id ;
	}

	if ( g_objHero.IsTeamLeader () )
	{
		m_uTeamMode = TEAM_MODE_LEADER ;
		m_GroupBtn1.ChangeImage ( "Button192" ) ;
		m_GroupBtn2.ChangeImage ( "Button194" ) ;
		m_GroupBtn4.EnableWindow ( false ) ;
	}
	else if ( i > 0 )
	{
		m_uTeamMode = TEAM_MODE_MEMBER ;
		m_GroupBtn1.ChangeImage ( "Button193" ) ;
		m_GroupBtn2.ChangeImage ( "Button196" ) ;
	}
	else
	{
		m_uTeamMode = TEAM_MODE_NONE ;
		m_GroupBtn1.ChangeImage ( "Button190" ) ;
		m_GroupBtn2.ChangeImage ( "Button191" ) ;
	}
}

void CDlgGroup::OnGroupBtn1()
{
	switch ( m_uTeamMode )
	{
	case TEAM_MODE_NONE:
		g_objHero.TeamCreate () ;
		m_MenuGroup.m_bCreate = true ;
		break ;
	case TEAM_MODE_LEADER:
		CDlgGroup::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_GROUP_INVITE ) ;
		break ;
	case TEAM_MODE_MEMBER:
		g_objHero.TeamLeave () ;
		break ;
	}
}

void CDlgGroup::OnGroupBtn2()
{
	switch ( m_uTeamMode )
	{
	case TEAM_MODE_NONE:
		CDlgGroup::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_GROUP_JOIN ) ;
		break ;
	case TEAM_MODE_LEADER:
		g_objHero.TeamDismiss () ;
		break ;
	case TEAM_MODE_MEMBER:
		g_objHero.TeamFollow () ;
 		break ;
	}
}

void CDlgGroup::OnGroupChk3()
{
	if ( m_uTeamMode == TEAM_MODE_LEADER )
	{
		if ( !m_MenuGroup.m_bShow )
			m_MenuGroup.ShowWindow ( SW_SHOW ) ;
		else
			m_MenuGroup.ShowWindow ( SW_HIDE ) ;
	}
}

void CDlgGroup::OnGroupBtn4()
{
	if ( m_uTeamMode == TEAM_MODE_LEADER )
	{
		g_objHero.TeamKickOut ( m_PlayerID[m_uKickPlayer] ) ;
	}
}

void CDlgGroup::OnGroupImg1()
{
	SetFocusPlayer(0);
	if ( m_GroupImg1.GetImageLoadState () )
	{
		m_uKickPlayer = 0 ;
	}
	m_GroupBtn4.EnableWindow ( false ) ;
}

void CDlgGroup::OnGroupImg2()
{
	SetFocusPlayer(1);
	if ( m_GroupImg2.GetImageLoadState () )
	{
		m_uKickPlayer = 1 ;
		m_GroupBtn4.SetCurFrame ( 0 ) ;
		m_GroupBtn4.EnableWindow ( true ) ;
	}
	else
		m_GroupBtn4.EnableWindow ( false ) ;
}

void CDlgGroup::OnGroupImg3()
{
	SetFocusPlayer(2);
	if ( m_GroupImg3.GetImageLoadState () )
	{
		m_uKickPlayer = 2 ;
		m_GroupBtn4.SetCurFrame ( 0 ) ;
		m_GroupBtn4.EnableWindow ( true ) ;
	}
	else
		m_GroupBtn4.EnableWindow ( false ) ;
}

void CDlgGroup::OnGroupImg4()
{
	SetFocusPlayer(3);
	if ( m_GroupImg4.GetImageLoadState () )
	{
		m_uKickPlayer = 3 ;
		m_GroupBtn4.SetCurFrame ( 0 ) ;
		m_GroupBtn4.EnableWindow ( true ) ;
	}
	else
		m_GroupBtn4.EnableWindow ( false ) ;
}

void CDlgGroup::OnGroupImg5()
{
	SetFocusPlayer(4);
	if ( m_GroupImg5.GetImageLoadState () )
	{
		m_uKickPlayer = 4 ;
		m_GroupBtn4.SetCurFrame ( 0 ) ;
		m_GroupBtn4.EnableWindow ( true ) ;
	}
	else
		m_GroupBtn4.EnableWindow ( false ) ;
}

void CDlgGroup::SetFocusPlayer(int nIndex)
{
	m_uFocusPlayer = nIndex + 1 ;
}
