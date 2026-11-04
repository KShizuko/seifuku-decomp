#include "stdafx.h"
#include "MyShell.h"
#include "DlgEnemy.h"

#include "Hero.h"
#include "Enemy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgEnemy::CDlgEnemy(CWnd* pParent )
	: CDialog(CDlgEnemy::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_fFrameStep = 0.0f ;
	m_nFrameMode = 0 ;
	m_nPntX = 0 ;
	m_nCurrentQueryIndex = 0 ;
}


void CDlgEnemy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ENEMY_BTN_RESUME, m_EnemyBtnResume);
	DDX_Control(pDX, IDC_ENEMY_LST_DATA, m_EnemyLstData);
	DDX_Control(pDX, IDC_ENEMY_BTN_CLOSE, m_EnemyBtnClose);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
}


BEGIN_MESSAGE_MAP(CDlgEnemy, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_ENEMY_BTN_CLOSE, OnEnemyBtnClose)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_ENEMY_BTN_RESUME, OnEnemyBtnResume)
END_MESSAGE_MAP()

LRESULT CDlgEnemy::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_SHOWWINDOW:
		m_bShow = wParam ;
		if ( wParam )
		{
			CRect rect ;
			CDlgEnemy::GetWindowRect ( rect ) ;
			if ( m_nFrameMode == 0 )
			{
				DXPlaySound ( "Sound/DlgOpen.wav" ) ;
				if ( m_EnemyLstData.GetCount () == 0 )
					CDlgEnemy::SetEnemyList () ;
				m_dwLastShowTime = 0 ;
				m_nFrameMode = 1 ;
				CDlgEnemy::MoveWindow ( -rect.Width(), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
			}
			else if ( m_nFrameMode == 2 )
			{
				CDlgEnemy::MoveWindow ( 0, m_Pnt.y, rect.Width(), rect.Height(), true ) ;
				m_nFrameMode = 0 ;
			}
		}
		else
		{
			DXPlaySound ( "Sound/DlgClose.wav" ) ;
			if ( m_DlgEnmState.m_bShow )
				m_DlgEnmState.ShowWindow ( SW_HIDE ) ;
		}
		break ;
	case WM_COMMAND:
		if ( wParam == 1 )
		{
			return true ;
		}
		else if ( wParam == 2 )
		{
			CDlgEnemy::OnEnemyBtnClose () ;
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
			if ( wParam == 0 )
			{
				char strEnemyName[16] ;
				m_nCurrentQueryIndex = lParam ;
				m_EnemyLstData.GetText ( lParam, strEnemyName ) ;
				CEnemy* pEnemy = g_objHero.GetEnemyByName ( strEnemyName ) ;
				if ( pEnemy )
				{
					g_objHero.QueryEnemy ( pEnemy->GetID() ) ;
				}
			}
			else if ( wParam == -1 )
			{
				if ( lParam != -1 )
				{
					char strEnemyName[16] ;
					m_EnemyLstData.GetText ( lParam, strEnemyName ) ;
					m_DlgEnmState.SetEnemyNull ( strEnemyName ) ;
					if ( !m_DlgEnmState.m_bShow )
						m_DlgEnmState.ShowWindow ( SW_SHOW ) ;
				}
				else
				{
					if ( m_DlgEnmState.m_bShow )
						m_DlgEnmState.ShowWindow ( SW_HIDE ) ;
				}
			}
		}
		break ;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

HBRUSH CDlgEnemy::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgEnemy::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect;
	CDlgEnemy::GetWindowRect(rect);
	m_fFrameStep = ( float ) rect.Width () / 20 ;

	m_EnemyBtnClose.Init ( rect.left, rect.top, "Button5" ) ;
	m_BtnHelp.Init ( rect.left, rect.top, "Button444" ) ;
	m_EnemyBtnResume.Init ( rect.left, rect.top, "Button460" ) ;

	if ( ! m_EnemyLstData.Init ( rect.left, rect.top, NULL, 2 ) )
		return false ;
	m_EnemyLstData.SetFontColor ( 0xffff00 ) ;
	m_EnemyLstData.SetParentCob ( this ) ;

	m_DlgEnmState.Create ( IDD_DIALOG_ENMSTATE, CDlgEnemy::GetParent () ) ;
	CRect Temprect ;
	m_DlgEnmState.GetWindowRect ( Temprect ) ;
	m_DlgEnmState.MoveWindow ( rect.Width () - 2,
							   65,
							   Temprect.Width (),
							   Temprect.Height () ) ;
	return TRUE;
}

void CDlgEnemy::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_nPntX = x ;
	m_Pnt.y = y ;
}

void CDlgEnemy::Show()
{
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
					CDlgEnemy::WindowProc ( WM_SHOWWINDOW, true, NULL ) ;
				}
			}
		}
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog46",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			ShowAni->Show ( 0,
							m_Pnt.x,
							m_Pnt.y ) ;
		}
		else
			return ;

		m_EnemyLstData.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_EnemyBtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_EnemyBtnResume.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_DlgEnmState.m_bShow )
			m_DlgEnmState.Show() ;
	}
}

void CDlgEnemy::OnEnemyBtnClose()
{
	DXPlaySound("Sound/BtnClick.wav");
	CDlgEnemy::ShowWindow ( SW_HIDE ) ;
}

void CDlgEnemy::OnBtnHelp()
{
	CDlgEnemy::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_FRIEND ) ;
}

void CDlgEnemy::OnEnemyBtnResume()
{
	CDlgEnemy::ShowWindow ( SW_HIDE ) ;
	CDlgEnemy::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FRIEND_RESUME, 1 ) ;
}

void CDlgEnemy::SetEnemyList()
{
	m_EnemyLstData.ResetContent ( ) ;
	int nAmount = g_objHero.GetEnemyAmount () ;

	int nOnline = 0 ;
	int i ;
	for ( i = 0; i < nAmount; i++)
	{
		CEnemy* pEnemy = g_objHero.GetEnemyByIndex ( i ) ;
		if ( pEnemy && pEnemy->IsOnline () )
			nOnline++ ;
	}
	int nOffLine = nOnline ;

	for ( i = 0; i < nAmount; i++)
	{
		CEnemy* pEnemy	= g_objHero.GetEnemyByIndex(i);
		if (pEnemy)
		{
			char strListData[64] ;
			strcpy ( strListData, pEnemy->GetName () ) ;
			if ( pEnemy->IsOnline () )
			{
				m_EnemyLstData.InsertString ( 0, strListData ) ;
				m_EnemyLstData.SetListRealIndex ( nOnline - 1, 1 ) ;
				nOnline-- ;
			}
			else
			{
				m_EnemyLstData.AddString(strListData);
				m_EnemyLstData.SetListRealIndex ( nOffLine, -1 ) ;
				nOffLine++ ;
			}
		}
	}
}


void CDlgEnemy::SetEnemyState()
{
	char strEnemyName[16] ;
	m_EnemyLstData.GetText ( m_nCurrentQueryIndex, strEnemyName ) ;
	if ( strEnemyName != NULL )
	{
		m_DlgEnmState.SetEnemyState ( strEnemyName ) ;
		if ( !m_DlgEnmState.m_bShow )
			m_DlgEnmState.ShowWindow ( SW_SHOW ) ;
	}
}
