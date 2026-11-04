#include "stdafx.h"
#include "MyShell.h"
#include "DlgEnmState.h"

#include "Hero.h"
#include "Enemy.h"
#include "3dRoleData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgEnmState::CDlgEnmState(CWnd* pParent )
	: CDialog(CDlgEnmState::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
}


void CDlgEnmState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ENMSTATE_IMAFACE, m_EnmStateImaface);
	DDX_Control(pDX, IDC_STA6, m_Sta6);
	DDX_Control(pDX, IDC_STA5, m_Sta5);
	DDX_Control(pDX, IDC_STA1, m_Sta1);
	DDX_Control(pDX, IDC_STA2, m_Sta2);
	DDX_Control(pDX, IDC_STA3, m_Sta3);
	DDX_Control(pDX, IDC_STA4, m_Sta4);
	DDX_Control(pDX, IDC_STA7, m_Sta7);
	DDX_Control(pDX, IDC_ENMSTATE_BTN_DEL, m_EnmStateBtnDel);
	DDX_Control(pDX, IDC_ENMSTATE_BTN_CLOSE, m_EnmStateBtnClose);
}


BEGIN_MESSAGE_MAP(CDlgEnmState, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_ENMSTATE_BTN_CLOSE, OnEnmstateBtnClose)
	ON_BN_CLICKED(IDC_ENMSTATE_BTN_DEL, OnEnmstateBtnDel)
END_MESSAGE_MAP()

LRESULT CDlgEnmState::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
		if ( wParam )
		{
			DXPlaySound("Sound/BtnClick.wav");
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

HBRUSH CDlgEnmState::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgEnmState::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgEnmState::GetWindowRect ( rect ) ;

	m_EnmStateImaface.Init ( rect.left, rect.top, NULL, FACE_IMAGE ) ;

	m_Sta1.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_Sta2.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_Sta3.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_Sta4.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_Sta5.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_Sta6.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_Sta7.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_EnmStateBtnDel.Init ( rect.left, rect.top, "Button250" ) ;
	m_EnmStateBtnClose.Init ( rect.left, rect.top, "Button5" ) ;

	return TRUE;
}

void CDlgEnmState::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgEnmState::Show()
{
	if ( m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog47",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			ShowAni->Show ( 0,
							m_Pnt.x,
							m_Pnt.y + 1 ) ;
		}
		else
			return ;

		m_EnmStateImaface.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_Sta1.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_Sta2.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_Sta3.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_Sta4.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_Sta5.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_Sta6.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_Sta7.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_EnmStateBtnDel.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_EnmStateBtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgEnmState::OnEnmstateBtnClose()
{
	CDlgEnmState::ShowWindow ( SW_HIDE ) ;
}

void CDlgEnmState::OnEnmstateBtnDel()
{
	CDlgEnmState::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ENMSTATE_DEL ) ;
}

void CDlgEnmState::SetEnemyState(char* strEnemyName)
{
	CEnemy* pEnemy = g_objHero.GetEnemyByName ( strEnemyName ) ;
	if ( pEnemy != NULL )
	{
		m_EnmStateImaface.RemoveImage () ;

		char strText[16] ;
		itoa ( pEnemy->GetFace (), strText, 10 ) ;
		m_EnmStateImaface.InsertImage ( strText, 0, false ) ;
		m_Sta1.SetWindowText ( pEnemy->GetName () ) ;

		itoa ( pEnemy->GetLevel (), strText, 10 ) ;
		m_Sta2.SetWindowText ( strText ) ;
		m_Sta3.SetWindowText ( pEnemy->GetProfessionalName () ) ;
		itoa ( pEnemy->GetPK (), strText, 10 ) ;
		m_Sta4.SetWindowText ( strText ) ;

		strcpy ( strText, pEnemy->GetSyndicateName () ) ;
		m_Sta5.SetWindowText ( strText ) ;
		strcpy ( strText, pEnemy->GetRankName () ) ;
		m_Sta6.SetWindowText ( strText ) ;
		m_Sta7.SetWindowText ( pEnemy->GetMate () ) ;
	}
}

void CDlgEnmState::DelEnemy()
{
	char strName[16] ;
	m_Sta1.GetWindowText ( strName, 16 ) ;
	if ( strName )
	{
		CEnemy* pEnemy = g_objHero.GetEnemyByName ( strName ) ;
		if ( pEnemy != NULL )
			g_objHero.DelEnemy ( pEnemy->GetID (), true ) ;
	}
	CDlgEnmState::ShowWindow ( SW_HIDE ) ;
}

void CDlgEnmState::SetEnemyNull(char *strName)
{
	m_EnmStateImaface.RemoveImage () ;
	m_Sta1.SetWindowText ( strName ) ;
	m_Sta2.SetWindowText ( NULL ) ;
	m_Sta3.SetWindowText ( NULL ) ;
	m_Sta4.SetWindowText ( NULL ) ;
	m_Sta5.SetWindowText ( NULL ) ;
	m_Sta6.SetWindowText ( NULL ) ;
	m_Sta7.SetWindowText ( NULL ) ;	
}
