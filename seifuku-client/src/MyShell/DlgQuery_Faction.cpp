#include "stdafx.h"
#include "MyShell.h"
#include "DlgQuery_Faction.h"

#include "Hero.h"
#include "GamePlayerSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgQuery_Faction::CDlgQuery_Faction(CWnd* pParent )
	: CDialog(CDlgQuery_Faction::IDD, pParent)
{
	m_Hbr = CreateSolidBrush ( 0x2132B ) ;
	m_HbrEdit = CreateSolidBrush ( 0 ) ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_fFrameStep = 0.0f ;
	m_nFrameMode = 0 ;
	m_nPntX = 0 ;
	m_nShowType = 0 ;
	m_nRank = 0 ;
	m_bShow = false ;
	m_nCurBase = 0 ;
	m_nMemberAmount = 0 ;
	for ( int i = 0; i < 800; i++ )
	{
		m_strMember[i][0] = NULL ;
		m_bMemberOnline[i] = 0 ;
	}
}


void CDlgQuery_Faction::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_DEL, m_BtnDel);
	DDX_Control(pDX, IDC_FACTION_RIGHT, m_BtnRight);
	DDX_Control(pDX, IDC_FACTION_LEFT, m_BtnLeft);
	DDX_Control(pDX, IDC_FACTION_LST_LIST, m_LstMember);
	DDX_Control(pDX, IDC_BTN_OK, m_BtnOk);
	DDX_Control(pDX, IDC_BTN_CANCEL, m_BtnCancel);
	DDX_Control(pDX, IDC_FACTION_EDT_DATA, m_EdtWord);
	DDX_Control(pDX, IDC_FACTION_CLOSEB, m_BtnCloseB);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
	DDX_Control(pDX, IDC_FACTION_BTN_STATE, m_BtnState);
	DDX_Control(pDX, IDC_FACTION_BTN_FRIEND, m_BtnFriend);
	DDX_Control(pDX, IDC_FACTION_BTN_ENEMY, m_BtnEnemy);
	DDX_Control(pDX, IDC_FACTION_BTN_WORD, m_BtnWord);
	DDX_Control(pDX, IDC_FACTION_CLOSE, m_BtnClose);
	DDX_Control(pDX, IDC_STATIC8, m_StaProffer);
	DDX_Control(pDX, IDC_STATIC12, m_StaTitle);
	DDX_Control(pDX, IDC_STATIC11, m_StaName);
	DDX_Control(pDX, IDC_STATIC13, m_StaLeader);
	DDX_Control(pDX, IDC_STATIC55, m_StaFund);
	DDX_Control(pDX, IDC_STATIC66, m_StaPopulation);
	DDX_Control(pDX, IDC_STATIC1, m_StaLine1);
	DDX_Control(pDX, IDC_STATIC2, m_StaLine2);
	DDX_Control(pDX, IDC_STATIC3, m_StaLine3);
	DDX_Control(pDX, IDC_STATIC4, m_StaLine4);
	DDX_Control(pDX, IDC_STATIC5, m_StaLine5);
	DDX_Control(pDX, IDC_FACTION_BTN_JOIN, m_BtnJoin);
	DDX_Control(pDX, IDC_FACTION_BTN_OUT, m_BtnOut);
	DDX_Control(pDX, IDC_FACTION_BTN_SUB, m_BtnSub);
}

BEGIN_MESSAGE_MAP(CDlgQuery_Faction, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_FACTION_BTN_OUT, OnFactionBtnOut)
	ON_BN_CLICKED(IDC_FACTION_BTN_INVITE, OnFactionBtnInvite)
	ON_BN_CLICKED(IDC_FACTION_BTN_JOIN, OnFactionBtnJoin)
	ON_BN_CLICKED(IDC_FACTION_CLOSE, OnFactionClose)
	ON_BN_CLICKED(IDC_FACTION_BTN_SUB, OnFactionBtnSub)
	ON_BN_CLICKED(IDC_FACTION_BTN_WORD, OnFactionBtnWord)
	ON_BN_CLICKED(IDC_FACTION_BTN_STATE, OnFactionBtnState)
	ON_BN_CLICKED(IDC_FACTION_BTN_FRIEND, OnFactionBtnFriend)
	ON_BN_CLICKED(IDC_FACTION_BTN_ENEMY, OnFactionBtnEnemy)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_FACTION_CLOSEB, OnFactionCloseb)
	ON_BN_CLICKED(IDC_BTN_OK, OnBtnOk)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBtnCancel)
	ON_BN_CLICKED(IDC_FACTION_LEFT, OnFactionLeft)
	ON_BN_CLICKED(IDC_FACTION_RIGHT, OnFactionRight)
	ON_BN_CLICKED(IDC_BTN_DEL, OnFactionBtnDel)
END_MESSAGE_MAP()

LRESULT CDlgQuery_Faction::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_SHOWWINDOW:
		m_bShow = wParam ;
		if ( wParam )
		{
			CRect rect ;
			CDlgQuery_Faction::GetWindowRect ( rect ) ;
			if ( m_nFrameMode == 0 )
			{
				DXPlaySound ( "Sound/DlgOpen.wav" ) ;
				m_dwLastShowTime = 0 ;
				m_nFrameMode = 1 ;
				CDlgQuery_Faction::MoveWindow ( -rect.Width(), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
			}
			else if ( m_nFrameMode == 2 )
			{
				g_objHero.QuerySynicateInfo () ;
				CDlgQuery_Faction::MoveWindow ( 0, m_Pnt.y, rect.Width(), rect.Height(), true ) ;
				m_nFrameMode = 0 ;
			}
		}
		else
		{
			DXPlaySound ( "Sound/DlgClose.wav" ) ;
		}
		break ;
	case WM_DESTROY:
		DeleteObject ( m_Hbr ) ;
		DeleteObject ( m_HbrEdit ) ;
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
	case LB_SETSEL:
		if ( wParam == 0 )
		{
			char strText[52] ;
			m_LstMember.GetText ( lParam, strText ) ;
			for ( int i = 18; i >= 0; i-- )
			{
				if ( strText[i] != ' ' )
				{
					strText[i + 1] = NULL ;
					break ;
				}
			}
			if ( strText )
			{
				m_LstMember.GetText ( lParam, m_strCurName ) ;
				g_objHero.QuerySynMember ( strText ) ;
			}
		}
		else if ( wParam == -1 )
		{
			if ( m_DlgFacState.m_bShow )
				m_DlgFacState.ShowWindow ( SW_HIDE ) ;
		}
		break ;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

HBRUSH CDlgQuery_Faction::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	if ( nCtlColor == CTLCOLOR_EDIT )
		return m_HbrEdit ;
	return m_Hbr ;
}

BOOL CDlgQuery_Faction::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgQuery_Faction::GetWindowRect ( rect ) ;
	m_fFrameStep = ( float ) rect.Width () / 20 ;

	if ( !m_StaName.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaTitle.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaProffer.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaLeader.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaFund.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaPopulation.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaLine1.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaLine2.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaLine3.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaLine4.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaLine5.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;

	if ( !m_BtnJoin.Init ( rect.left, rect.top, "Button82" ) )
		return false ;
	if ( !m_BtnOut.Init ( rect.left, rect.top, "Button81" ) )
		return false ;
	if ( !m_BtnSub.Init ( rect.left, rect.top, "Button80" ) )
		return false ;
	if ( !m_BtnClose.Init ( rect.left, rect.top, "Button5" ) )
		return false ;
	if ( !m_BtnState.Init ( rect.left, rect.top, "Button83" ) )
		return false ;
	if ( !m_BtnWord.Init ( rect.left, rect.top, "Button84" ) )
		return false ;
	if ( !m_BtnFriend.Init ( rect.left, rect.top, "Button85" ) )
		return false ;
	if ( !m_BtnEnemy.Init ( rect.left, rect.top, "Button86" ) )
		return false ;
	if ( !m_BtnHelp.Init ( rect.left, rect.top, "Button4" ) )
		return false ;
	if ( !m_BtnCloseB.Init ( rect.left, rect.top, "Button3" ) )
		return false ;

	m_BtnOk.Init ( rect.left, rect.top, "Button0" ) ;
	m_BtnOk.EnableWindow ( false ) ;
	m_BtnCancel.Init ( rect.left, rect.top, "Button7" ) ;
	m_BtnCancel.EnableWindow ( false ) ;
	m_BtnLeft.Init ( rect.left, rect.top, "Button382" ) ;
	m_BtnLeft.EnableWindow ( false ) ;
	m_BtnRight.Init ( rect.left, rect.top, "Button383" ) ;
	m_BtnRight.EnableWindow ( false ) ;
	m_BtnDel.Init ( rect.left, rect.top, "Button810" ) ;
	m_BtnDel.EnableWindow ( false ) ;

	m_EdtWord.Init ( rect.left, rect.top, NULL, 0, 0xffffff ) ;
	m_EdtWord.SetMultiLine ( true ) ;
	m_EdtWord.SetLimitText ( 256 ) ;
	m_EdtWord.SetOffset ( CPoint ( 2, 3 ) ) ;
	m_EdtWord.EnableWindow ( false ) ;

	m_LstMember.Init ( rect.left, rect.top, NULL, 2 ) ;
	m_LstMember.SetFontColor ( 0xffffff ) ;
	m_LstMember.SetParentCob ( this ) ;

	m_DlgFacState.Create ( IDD_DIALOG_FACSTATE, this->GetParent () ) ;
	CRect stateRect ;
	m_DlgFacState.GetWindowRect ( stateRect ) ;
	m_DlgFacState.MoveWindow ( rect.Width () - 10, 65, stateRect.Width (), stateRect.Height (), true ) ;

	return TRUE;
}

void CDlgQuery_Faction::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_nPntX = x ;
	m_Pnt.y = y ;
}

void CDlgQuery_Faction::Show()
{
	if ( m_bShow )
	{
		switch ( m_nFrameMode )
		{
		case 0:
			break ;
		case 1:
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
					CDlgQuery_Faction::WindowProc ( WM_SHOWWINDOW, true, NULL ) ;
				}
			}
			break ;
		case 2:
			break ;
		}

		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														  "Dialog8",
														  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0, m_Pnt.x, m_Pnt.y ) ;
		else
			return ;

		switch ( m_nShowType )
		{
		case 0:
			ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni, "Dialog28", EXIGENCE_IMMEDIATE ) ;
			if ( ShowAni == NULL )
				return ;
			ShowAni->Show ( 0, m_Pnt.x + 20, m_Pnt.y + 48 ) ;
			m_StaName.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaTitle.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaProffer.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaLeader.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaFund.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaPopulation.Show ( m_Pnt.x, m_Pnt.y ) ;
			break ;
		case 1:
			m_EdtWord.Show ( m_Pnt.x, m_Pnt.y ) ;
			break ;
		case 2:
			ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni, "Image80", EXIGENCE_IMMEDIATE ) ;
			if ( ShowAni == NULL )
				return ;
			ShowAni->Show ( 0, m_Pnt.x + 34, m_Pnt.y + 57 ) ;
			m_EdtWord.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_BtnOk.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_BtnCancel.Show ( m_Pnt.x, m_Pnt.y ) ;
			break ;
		case 3:
		case 4:
			m_StaLine1.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaLine2.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaLine3.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaLine4.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaLine5.Show ( m_Pnt.x, m_Pnt.y ) ;
			break ;
		case 5:
			ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni, "Image81", EXIGENCE_IMMEDIATE ) ;
			if ( ShowAni == NULL )
				return ;
			ShowAni->Show ( 0, m_Pnt.x + 20, m_Pnt.y + 48 ) ;
			m_LstMember.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_BtnLeft.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_BtnRight.Show ( m_Pnt.x, m_Pnt.y ) ;
			if ( m_nRank == 100 )
				m_BtnDel.Show ( m_Pnt.x, m_Pnt.y ) ;
			break ;
		}

		m_BtnState.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_nRank != 0 )
			m_BtnOut.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_nRank != 80 && m_nRank != 60 )
			m_BtnJoin.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnSub.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnWord.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnFriend.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnEnemy.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnCloseB.Show ( m_Pnt.x, m_Pnt.y ) ;

		if ( m_DlgFacState.m_bShow )
			m_DlgFacState.Show () ;
	}
}

void CDlgQuery_Faction::OnFactionBtnOut()
{
	DWORD dwRank = g_objHero.GetSyndicateRank () ;
	if ( dwRank == 50 || dwRank == 70 || dwRank == 90 )
	{
		if ( g_objHero.GetSyndicateProffer () < 20000 )
		{
			AfxMessageBox ( GameDataSetQuery()->GetStr ( 10341 ) ) ;
			return ;
		}
		CDlgQuery_Faction::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FACTION_OUT ) ;
	}
	else if ( dwRank == 100 || dwRank == 80 || dwRank == 60 )
	{
		m_nCurBase = 0 ;
		m_nMemberAmount = 0 ;
		g_objHero.QuerySynMemberList ( m_nCurBase ) ;
		m_nShowType = 5 ;
		m_LstMember.EnableWindow ( true ) ;
		m_BtnRight.EnableWindow ( true ) ;
		m_BtnLeft.EnableWindow ( true ) ;
		m_BtnDel.EnableWindow ( true ) ;
		if ( m_EdtWord.IsWindowEnabled () )
			m_EdtWord.EnableWindow ( false ) ;
	}
}

void CDlgQuery_Faction::OnFactionBtnInvite()
{
	if ( g_objHero.GetSyndicateRank () == 100 )
		CDlgQuery_Faction::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FACTION_INVITE ) ;
}

void CDlgQuery_Faction::OnFactionBtnJoin()
{
	if ( g_objHero.GetSydicateID () == 0 )
	{
		CDlgQuery_Faction::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FACTION_JOIN ) ;
		return ;
	}
	DWORD dwRank = g_objHero.GetSyndicateRank () ;
	if ( dwRank == 100 || dwRank == 80 || dwRank == 60 )
	{
		m_nShowType = 2 ;
		m_EdtWord.SetWindowText ( NULL ) ;
		m_EdtWord.EnableWindow ( true ) ;
		m_EdtWord.SetFocus () ;
		m_BtnOk.EnableWindow ( true ) ;
		m_BtnCancel.EnableWindow ( true ) ;
		m_LstMember.EnableWindow ( false ) ;
		m_DlgFacState.ShowWindow ( SW_HIDE ) ;
	}
	else
	{
		m_nCurBase = 0 ;
		m_nMemberAmount = 0 ;
		g_objHero.QuerySynMemberList ( m_nCurBase ) ;
		m_nShowType = 5 ;
		m_LstMember.EnableWindow ( true ) ;
		m_BtnRight.EnableWindow ( true ) ;
		m_BtnLeft.EnableWindow ( true ) ;
		m_BtnDel.EnableWindow ( false ) ;
	}
}

void CDlgQuery_Faction::SetFaction()
{
	m_nShowType = 0 ;
	if ( g_objHero.GetSydicateID () == 0 )
	{
		m_nRank = 0 ;
		m_BtnEnemy.EnableWindow ( false ) ;
		m_BtnFriend.EnableWindow ( false ) ;
		m_BtnState.EnableWindow ( false ) ;
		m_BtnSub.EnableWindow ( false ) ;
		m_BtnWord.EnableWindow ( false ) ;
	}
	else
	{
		m_nRank = g_objHero.GetSyndicateRank () ;
		m_BtnEnemy.EnableWindow ( true ) ;
		m_BtnFriend.EnableWindow ( true ) ;
		m_BtnState.EnableWindow ( true ) ;
		m_BtnSub.EnableWindow ( true ) ;
		m_BtnWord.EnableWindow ( true ) ;
	}

	switch ( m_nRank )
	{
	case 0:
		m_BtnOut.EnableWindow ( false ) ;
		m_BtnJoin.ChangeImage ( "Button82" ) ;
		if ( m_EdtWord.IsWindowEnabled () )
			m_EdtWord.EnableWindow ( false ) ;
		if ( m_LstMember.IsWindowEnabled () )
			m_LstMember.EnableWindow ( false ) ;
		m_BtnRight.EnableWindow ( false ) ;
		m_BtnLeft.EnableWindow ( false ) ;
		m_BtnDel.EnableWindow ( false ) ;
		break ;
	case 50:
	case 70:
	case 90:
		m_BtnOut.ChangeImage ( "Button81" ) ;
		m_BtnJoin.ChangeImage ( "Button89" ) ;
		m_BtnOut.EnableWindow ( true ) ;
		if ( m_EdtWord.IsWindowEnabled () )
			m_EdtWord.EnableWindow ( false ) ;
		break ;
	case 60:
		m_BtnOut.ChangeImage ( "Button89" ) ;
		m_BtnJoin.EnableWindow ( false ) ;
		break ;
	case 80:
		m_BtnJoin.EnableWindow ( false ) ;
		m_BtnOut.ChangeImage ( "Button89" ) ;
		break ;
	case 100:
		m_BtnOut.ChangeImage ( "Button87" ) ;
		m_BtnJoin.ChangeImage ( "Button88" ) ;
		m_BtnOut.EnableWindow ( true ) ;
		break ;
	}

	char strText[32] ;
	strcpy ( strText, g_objHero.GetSyndicate () ) ;
	if ( strText )
		m_StaName.SetWindowText ( strText ) ;
	strcpy ( strText, g_objHero.GetSyndicateTitle () ) ;
	if ( strText )
		m_StaTitle.SetWindowText ( strText ) ;
	if ( g_objHero.GetSydicateID () )
	{
		itoa ( g_objHero.GetSyndicateProffer (), strText, 10 ) ;
		if ( strText )
			m_StaProffer.SetWindowText ( strText ) ;
	}
	else
		m_StaProffer.SetWindowText ( NULL ) ;
	strcpy ( strText, g_objHero.GetSyndicateLeaderName () ) ;
	if ( strText )
		m_StaLeader.SetWindowText ( strText ) ;
	itoa ( g_objHero.GetSyndicateFund (), strText, 10 ) ;
	if ( strText )
		m_StaFund.SetWindowText ( strText ) ;
	itoa ( g_objHero.GetSyndicatePopulation (), strText, 10 ) ;
	if ( strText )
		m_StaPopulation.SetWindowText ( strText ) ;

	switch ( m_nShowType )
	{
	case 1:
		CDlgQuery_Faction::OnFactionBtnWord () ;
		break ;
	case 2:
		CDlgQuery_Faction::OnFactionBtnJoin () ;
		break ;
	case 3:
		CDlgQuery_Faction::OnFactionBtnFriend () ;
		break ;
	case 4:
		CDlgQuery_Faction::OnFactionBtnEnemy () ;
		break ;
	default:
		CDlgQuery_Faction::OnFactionBtnState () ;
		break ;
	}
}

void CDlgQuery_Faction::OnFactionClose()
{
	m_DlgFacState.ShowWindow ( SW_HIDE ) ;
	CDlgQuery_Faction::ShowWindow ( SW_HIDE ) ;
}

void CDlgQuery_Faction::OnFactionCloseb()
{
	CDlgQuery_Faction::OnFactionClose () ;
}

void CDlgQuery_Faction::OnFactionBtnSub()
{
	if ( g_objHero.GetSyndicateRank () != 0 )
		CDlgQuery_Faction::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FACTION_SUB ) ;
}

void CDlgQuery_Faction::OnFactionBtnWord()
{
	CDlgQuery_Faction::SetWord () ;
	m_nShowType = 1 ;
	if ( m_EdtWord.IsWindowEnabled () )
		m_EdtWord.EnableWindow ( false ) ;
	if ( m_BtnOk.IsWindowEnabled () )
		m_BtnOk.EnableWindow ( false ) ;
	if ( m_BtnCancel.IsWindowEnabled () )
		m_BtnCancel.EnableWindow ( false ) ;
	if ( m_LstMember.IsWindowEnabled () )
		m_LstMember.EnableWindow ( false ) ;
	m_BtnRight.EnableWindow ( false ) ;
	m_BtnLeft.EnableWindow ( false ) ;
	m_BtnDel.EnableWindow ( false ) ;
}

void CDlgQuery_Faction::OnFactionBtnState()
{
	m_nShowType = 0 ;
	if ( m_EdtWord.IsWindowEnabled () )
		m_EdtWord.EnableWindow ( false ) ;
	if ( m_BtnOk.IsWindowEnabled () )
		m_BtnOk.EnableWindow ( false ) ;
	if ( m_BtnCancel.IsWindowEnabled () )
		m_BtnCancel.EnableWindow ( false ) ;
	if ( m_LstMember.IsWindowEnabled () )
		m_LstMember.EnableWindow ( false ) ;
	m_BtnRight.EnableWindow ( false ) ;
	m_BtnLeft.EnableWindow ( false ) ;
	m_BtnDel.EnableWindow ( false ) ;
}

void CDlgQuery_Faction::OnFactionBtnFriend()
{
	m_StaLine1.SetWindowText ( NULL ) ;
	m_StaLine2.SetWindowText ( NULL ) ;
	m_StaLine3.SetWindowText ( NULL ) ;
	m_StaLine4.SetWindowText ( NULL ) ;
	m_StaLine5.SetWindowText ( NULL ) ;
	int nAmount = g_objPlayerSet.GetAllySyndicateAmount () ;
	for ( int i = 0; i < nAmount; i++ )
	{
		char strText[16] ;
		strcpy ( strText, g_objPlayerSet.GetAllySyndicateNameByIndex ( i ) ) ;
		if ( strText )
			CDlgQuery_Faction::SetLine ( i, strText ) ;
	}
	m_nShowType = 3 ;
	if ( m_EdtWord.IsWindowEnabled () )
		m_EdtWord.EnableWindow ( false ) ;
	if ( m_BtnOk.IsWindowEnabled () )
		m_BtnOk.EnableWindow ( false ) ;
	if ( m_BtnCancel.IsWindowEnabled () )
		m_BtnCancel.EnableWindow ( false ) ;
	if ( m_LstMember.IsWindowEnabled () )
		m_LstMember.EnableWindow ( false ) ;
	m_BtnRight.EnableWindow ( false ) ;
	m_BtnLeft.EnableWindow ( false ) ;
	m_BtnDel.EnableWindow ( false ) ;
}

void CDlgQuery_Faction::OnFactionBtnEnemy()
{
	m_StaLine1.SetWindowText ( NULL ) ;
	m_StaLine2.SetWindowText ( NULL ) ;
	m_StaLine3.SetWindowText ( NULL ) ;
	m_StaLine4.SetWindowText ( NULL ) ;
	m_StaLine5.SetWindowText ( NULL ) ;
	int nAmount = g_objPlayerSet.GetAntagonizeSyndicateAmount () ;
	for ( int i = 0; i < nAmount; i++ )
	{
		char strText[16] ;
		strcpy ( strText, g_objPlayerSet.GetAntagonizeNameByIndex ( i ) ) ;
		if ( strText )
			CDlgQuery_Faction::SetLine ( i, strText ) ;
	}
	m_nShowType = 4 ;
	if ( m_EdtWord.IsWindowEnabled () )
		m_EdtWord.EnableWindow ( false ) ;
	if ( m_BtnOk.IsWindowEnabled () )
		m_BtnOk.EnableWindow ( false ) ;
	if ( m_BtnCancel.IsWindowEnabled () )
		m_BtnCancel.EnableWindow ( false ) ;
	if ( m_LstMember.IsWindowEnabled () )
		m_LstMember.EnableWindow ( false ) ;
	m_BtnRight.EnableWindow ( false ) ;
	m_BtnLeft.EnableWindow ( false ) ;
	m_BtnDel.EnableWindow ( false ) ;
}

void CDlgQuery_Faction::SetLine ( int nIndex, const char* strText )
{
	switch ( nIndex )
	{
	case 0:
		m_StaLine1.SetWindowText ( strText ) ;
		break ;
	case 1:
		m_StaLine2.SetWindowText ( strText ) ;
		break ;
	case 2:
		m_StaLine3.SetWindowText ( strText ) ;
		break ;
	case 3:
		m_StaLine4.SetWindowText ( strText ) ;
		break ;
	case 4:
		m_StaLine5.SetWindowText ( strText ) ;
		break ;
	}
}

void CDlgQuery_Faction::OnBtnHelp()
{
	CDlgQuery_Faction::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_QUERY_FACTION ) ;
}

void CDlgQuery_Faction::OnBtnOk()
{
	if ( m_EdtWord.GetWindowTextLength () > 0 )
	{
		char strText[256] ;
		m_EdtWord.GetWindowText ( strText, 256 ) ;
		g_objHero.SendSyndicateAnnounce ( strText ) ;
		CDlgQuery_Faction::OnFactionBtnWord () ;
	}
}

void CDlgQuery_Faction::OnBtnCancel()
{
	m_EdtWord.SetWindowText ( NULL ) ;
	m_EdtWord.SetFocus () ;
}

void CDlgQuery_Faction::SetWord()
{
	if ( strcmp ( g_objHero.GetSyndicateAnnounce (), "NULL" ) == 0 )
		m_EdtWord.SetWindowText ( GameDataSetQuery()->GetStr ( 100158 ) ) ;
	else
		m_EdtWord.SetWindowText ( g_objHero.GetSyndicateAnnounce () ) ;
}

void CDlgQuery_Faction::SetMemberList()
{
	m_LstMember.ResetContent () ;
	int nAmount = g_objHero.GetSynMemberAmount () ;
	for ( int i = 0; i < nAmount; i++ )
	{
		SynMemberInfo* pInfo = g_objHero.GetSynMemberByIndex ( i ) ;
		if ( pInfo )
		{
			char strText[28] ;
			sprintf ( strText, "%-19s%d", pInfo->szName, pInfo->ucRank ) ;
			if ( pInfo->bOnLine )
			{
				m_LstMember.AddString ( strText ) ;
				m_LstMember.SetListRealIndex ( i, 1 ) ;
			}
			else
			{
				m_LstMember.AddString ( strText ) ;
				m_LstMember.SetListRealIndex ( i, -1 ) ;
			}
		}
		m_LstMember.GetText ( i, m_strMember[m_nMemberAmount + i] ) ;
		if ( m_LstMember.GetListRealIndex ( i ) == -1 )
			m_bMemberOnline[m_nMemberAmount + i] = 0 ;
		else
			m_bMemberOnline[m_nMemberAmount + i] = 1 ;
	}
	m_nMemberAmount += nAmount ;
}

void CDlgQuery_Faction::OnFactionLeft()
{
	if ( m_nCurBase > 0 )
	{
		m_nCurBase -= 10 ;
		m_LstMember.ResetContent () ;
		for ( int i = m_nCurBase; i < m_nCurBase + 10; i++ )
		{
			m_LstMember.AddString ( m_strMember[i] ) ;
			if ( m_bMemberOnline[i] )
				m_LstMember.SetListRealIndex ( i - m_nCurBase, 1 ) ;
			else
				m_LstMember.SetListRealIndex ( i - m_nCurBase, -1 ) ;
		}
	}
}

void CDlgQuery_Faction::OnFactionRight()
{
	if ( m_LstMember.GetCount () == 10 )
	{
		m_nCurBase += 10 ;
		if ( m_nMemberAmount > m_nCurBase )
		{
			int nAmount = m_nCurBase + 10 ;
			if ( m_nMemberAmount <= nAmount )
				nAmount = m_nMemberAmount ;
			m_LstMember.ResetContent () ;
			for ( int i = m_nCurBase; i < nAmount; i++ )
			{
				m_LstMember.AddString ( m_strMember[i] ) ;
				if ( m_bMemberOnline[i] )
					m_LstMember.SetListRealIndex ( i - m_nCurBase, 1 ) ;
				else
					m_LstMember.SetListRealIndex ( i - m_nCurBase, -1 ) ;
			}
		}
		else
			g_objHero.QuerySynMemberList ( m_nCurBase ) ;
	}
}

void CDlgQuery_Faction::OnFactionBtnDel()
{
	int nIndex = m_LstMember.GetCurSel () ;
	if ( nIndex != -1 )
	{
		char strText[64] ;
		m_LstMember.GetText ( nIndex, strText ) ;
		for ( int i = 18; i > 0; i-- )
		{
			if ( strText[i] != ' ' )
			{
				strText[i + 1] = NULL ;
				break ;
			}
		}
		strcpy ( m_strCurName, strText ) ;
		CDlgQuery_Faction::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FACTION_KICK ) ;
	}
}

void CDlgQuery_Faction::SetFacState()
{
	if ( m_strCurName )
	{
		for ( int i = 0; i < m_nMemberAmount; i++ )
		{
			if ( strcmp ( m_strCurName, m_strMember[i] ) == 0 )
			{
				m_DlgFacState.SetFacState ( m_strMember[i] ) ;
				return ;
			}
		}
	}
}
