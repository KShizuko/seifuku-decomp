#include "stdafx.h"
#include "MyShell.h"
#include "DlgExercise.h"
#include "ND_Bitmap.h"

#include "Hero.h"
#include "GameMsg.h"
#include "Shop.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgExercise::CDlgExercise(CWnd* pParent )
	: CDialog(CDlgExercise::IDD, pParent)
{
	m_Hbr = CreateSolidBrush ( 0 ) ;
	m_bInitSuc = false ;
	m_bShow = false ;
}


void CDlgExercise::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXERCISE_STA_SETTLE2, m_StaSettle2 ) ;
	DDX_Control(pDX, IDC_EXERCISE_STA_BLESSCOST, m_StaBlessCost ) ;
	DDX_Control(pDX, IDC_EXERCISE_STA_BLESS, m_StaBless ) ;
	DDX_Control(pDX, IDC_EXERCISE_STA_BLESSEMONEY, m_StaBlessEmoney ) ;
	DDX_Control(pDX, IDC_EXERCISE_STA_SETTLE1, m_StaSettle1 ) ;
	DDX_Control(pDX, IDC_EXERCISE_STA_BALANCE, m_StaBalance ) ;
	DDX_Control(pDX, IDC_EXERCISE_STA_COST, m_StaCost ) ;
	DDX_Control(pDX, IDC_EXERCISE_STA_COSTBG, m_StaCostBG ) ;
	DDX_Control(pDX, IDC_EXERCISE_STA_EMONEY, m_StaEmoney ) ;
	DDX_Control(pDX, IDC_EXERCISE_STA_EMONEY2, m_StaEmoney2 ) ;
	DDX_Control(pDX, IDC_EXERCISE_STA_TIME, m_StaTime ) ;
	DDX_Control(pDX, IDC_EXERCISE_STA_EXP, m_StaExp ) ;
	DDX_Control(pDX, IDC_EXERCISE_STA_EXPTITLE, m_StaExpTitle ) ;
	DDX_Control(pDX, IDC_EXERCISE_STA_CLOCK, m_StaClock ) ;
	DDX_Control(pDX, IDC_EXERCISE_BTN_CONTINUE, m_BtnContinue ) ;
	DDX_Control(pDX, IDC_EXERCISE_BTN_HELP, m_BtnHelp ) ;
	DDX_Control(pDX, IDC_EXERCISE_BTN_OK, m_BtnOk ) ;
	DDX_Control(pDX, IDC_EXERCISE_BTN_CLOSE, m_BtnClose ) ;
	DDX_Control(pDX, IDC_EXERCISE_BTN_PAY, m_BtnPay ) ;
}


BEGIN_MESSAGE_MAP(CDlgExercise, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_EXERCISE_BTN_CLOSE, OnBtnClose)
	ON_BN_CLICKED(IDC_EXERCISE_BTN_OK, OnBtnOk)
	ON_BN_CLICKED(IDC_EXERCISE_BTN_PAY, OnBtnPay)
	ON_BN_CLICKED(IDC_EXERCISE_BTN_CONTINUE, OnBtnContinue)
	ON_BN_CLICKED(IDC_EXERCISE_BTN_HELP, OnBtnHelp)
END_MESSAGE_MAP()

HBRUSH CDlgExercise::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgExercise::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgExercise::GetWindowRect ( rect ) ;

	m_BtnClose.Init ( rect.left, rect.top, "ExerciseClose" ) ;
	m_BtnHelp.Init ( rect.left, rect.top, "ExerciseHelp" ) ;
	m_BtnOk.Init ( rect.left, rect.top, "ExeConfirm" ) ;
	m_BtnContinue.Init ( rect.left, rect.top, "ExeContinue" ) ;
	m_BtnPay.Init ( rect.left, rect.top, "PayDown" ) ;

	m_StaSettle1.Init ( rect.left, rect.top, 3, NULL, "NULL" ) ;
	m_StaSettle1.SetWindowText ( NULL ) ;
	m_StaSettle1.SetFontName ( 1 ) ;
	m_StaSettle2.Init ( rect.left, rect.top, 3, NULL, "NULL" ) ;
	m_StaSettle2.SetWindowText ( NULL ) ;
	m_StaSettle2.SetFontName ( 1 ) ;
	m_StaTime.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_StaEmoney.Init ( rect.left, rect.top, 0, NULL, "Emoney" ) ;
	m_StaCostBG.Init ( rect.left, rect.top, 0, NULL, "ExeMoneyBG01" ) ;
	m_StaCostBG.SetWindowText ( NULL ) ;
	m_StaEmoney2.Init ( rect.left, rect.top, 0, NULL, "Emoney" ) ;
	m_StaBless.Init ( rect.left, rect.top, 0, NULL, "ExeBlessBG" ) ;
	m_StaBless.SetWindowText ( NULL ) ;
	m_StaBlessCost.SetWindowText ( "0" ) ;
	m_StaBlessEmoney.Init ( rect.left, rect.top, 0, NULL, "Emoney" ) ;
	m_StaCost.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_StaBlessCost.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_StaBalance.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_StaClock.Init ( rect.left, rect.top, 0, NULL, "ExerciseClock" ) ;
	m_StaExpTitle.Init ( rect.left, rect.top, 3, NULL, "NULL" ) ;
	m_StaExpTitle.SetWindowText ( GameDataSetQuery()->GetStr ( 11002 ) ) ;
	m_StaExp.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_StaTime.SetWindowText ( NULL ) ;
	m_StaClock.SetWindowText ( NULL ) ;
	m_StaEmoney.SetWindowText ( NULL ) ;
	m_StaEmoney2.SetWindowText ( NULL ) ;
	m_StaCost.SetWindowText ( NULL ) ;
	m_StaBlessEmoney.SetWindowText ( NULL ) ;
	m_StaBalance.SetWindowText ( NULL ) ;

	m_bInitSuc = true ;
	return TRUE;
}

void CDlgExercise::ClearExp ()
{
	m_StaExp.SetWindowText ( "" ) ;
}

void CDlgExercise::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

LRESULT CDlgExercise::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_SHOWWINDOW:
		m_bShow = wParam ;
		if ( wParam == 0 )
			DXPlaySound ( "Sound/window_close.wav" ) ;
		else
			DXPlaySound ( "Sound/DlgOpen.wav" ) ;
		break ;
	case WM_DESTROY:
		DeleteObject ( m_Hbr ) ;
		break ;
	case WM_PAINT:
		{
			PAINTSTRUCT ps ;
			::BeginPaint ( m_hWnd, &ps ) ;
			::EndPaint ( m_hWnd, &ps ) ;
			return false ;
		}
	case WM_ERASEBKGND:
		return true ;
	case WM_NCPAINT:
		return false ;
	case WM_COMMAND:
		if ( wParam == 1 )
		{
			return true ;
		}
		else if ( wParam == 2 )
		{
			CDlgExercise::OnBtnClose () ;
			return true ;
		}
		break ;
	default:
		break ;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CDlgExercise::OnBtnClose()
{
	DXPlaySound ( "Sound/window_close.wav" ) ;
	CDlgExercise::ShowWindow ( SW_HIDE ) ;
	g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 11003 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
}

int CDlgExercise::GetTotalTime ()
{
	return m_nPayTime + m_nBlessTime ;
}

void CDlgExercise::OnBtnOk()
{
	g_objHero.ActTable ( 2, CDlgExercise::GetTotalTime () ) ;
	CDlgExercise::ShowWindow ( SW_HIDE ) ;
}

void CDlgExercise::OnBtnPay()
{
	ShellExecuteA ( m_hWnd, NULL, GameDataSetQuery()->GetStr ( 11004 ), NULL, NULL, SW_SHOW ) ;
	CDlgExercise::ShowWindow ( SW_HIDE ) ;
	CDlgExercise::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EXERCISE_EXIT, 3 ) ;
}

void CDlgExercise::OnBtnContinue()
{
	CDlgExercise::ShowWindow ( SW_HIDE ) ;
	CDlgExercise::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EXERCISE_EXIT, 3 ) ;
}

void CDlgExercise::SetTime ( int nTime, int nBlessTime, BOOL bBlessed )
{
	m_nPayTime = 0 ;
	m_nBlessTime = nBlessTime ;
	m_nTime = nTime ;
	if ( m_nBlessTime == 0 )
	{
		m_StaBless.EnableWindow ( false ) ;
		m_StaBlessEmoney.EnableWindow ( false ) ;
		m_StaBlessCost.EnableWindow ( false ) ;
	}
	else
	{
		m_StaBless.EnableWindow ( true ) ;
		m_StaBlessEmoney.EnableWindow ( true ) ;
		m_StaBlessCost.EnableWindow ( true ) ;
	}
	if ( m_nBlessTime == 0 )
		m_nPayTime = m_nTime ;
	else
		m_nPayTime = m_nTime - m_nBlessTime ;
	CDlgExercise::RequestExp () ;
	CDlgExercise::ClearExp () ;
	char szText[256] ;
	sprintf ( szText, "%u", g_objHero.GetEmoney () ) ;
	InsertComma ( szText ) ;
	m_StaBalance.SetWindowText ( szText ) ;
	int nEmoney = g_objHero.GetEmoney () ;
	int nCost = m_nPayTime / 15 ;
	if ( nEmoney < nCost )
		strcpy ( szText, GameDataSetQuery()->GetStr ( 11007 ) ) ;
	else
		sprintf ( szText, GameDataSetQuery()->GetStr ( 11005 ), m_nTime / 60, m_nTime % 60 ) ;
	m_StaSettle1.SetWindowText ( szText ) ;
	int nParam = g_objHero.GetTableParam ( 2 ) ;
	if ( nEmoney < nCost )
		strcpy ( szText, GameDataSetQuery()->GetStr ( 11008 ) ) ;
	else
		sprintf ( szText, GameDataSetQuery()->GetStr ( 11006 ), nParam * 10 / 60, nParam * 10 % 60 ) ;
	m_StaSettle2.SetWindowText ( szText ) ;
	CDlgExercise::ShowWindow ( SW_SHOW ) ;
}

void CDlgExercise::Show()
{
	if ( m_bShow && m_bInitSuc )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
															"ExerciseBG",
															EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			ShowAni->Show ( 0, m_Pnt.x, m_Pnt.y ) ;

			if ( m_StaBless.IsWindowEnabled () )
				m_StaBless.Show ( m_Pnt.x, m_Pnt.y ) ;
			if ( m_StaBlessEmoney.IsWindowEnabled () )
				m_StaBlessEmoney.Show ( m_Pnt.x, m_Pnt.y ) ;
			if ( m_StaBlessCost.IsWindowEnabled () )
				m_StaBlessCost.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaTime.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaSettle1.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaBalance.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaCostBG.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaCost.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaExp.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaExpTitle.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaClock.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_BtnContinue.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_BtnOk.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_BtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_BtnPay.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaEmoney.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaEmoney2.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_StaSettle2.Show ( m_Pnt.x, m_Pnt.y ) ;
			if ( m_nBlessTime != 0 )
			{
				CRect rect ;
				m_StaCost.GetWindowRect ( rect ) ;
				CDlgExercise::ScreenToClient ( rect ) ;
				IAni* LineAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
																	"ExeDeleLine",
																	EXIGENCE_IMMEDIATE ) ;
				if ( LineAni != NULL )
					LineAni->Show ( 0, m_Pnt.x + rect.left, m_Pnt.y + rect.top ) ;
			}
		}
	}
}

void CDlgExercise::RequestExp ()
{
	g_objHero.ActTable ( 1, CDlgExercise::GetTotalTime () ) ;
}

void CDlgExercise::FlashExp ( int nTable )
{
	int nTotal = m_nPayTime + m_nBlessTime ;
	char szText[256] ;
	itoa ( nTotal / 15, szText, 10 ) ;
	m_StaCost.SetWindowText ( szText ) ;
	itoa ( m_nPayTime / 15, szText, 10 ) ;
	m_StaBlessCost.SetWindowText ( szText ) ;
	sprintf ( szText, GameDataSetQuery()->GetStr ( 11009 ), nTotal / 60, nTotal % 60 ) ;
	m_StaTime.SetWindowText ( szText ) ;
	int nLevel = 0 ;
	double dPercent = 0.0 ;
	nLevel = g_objHero.GetLev () ;
	DWORD dwNextExp = g_objHero.GetNextLevelExp () ;
	int nExp = g_objHero.GetExp () ;
	if ( dwNextExp != 0 )
		dPercent = ( double )nExp / ( double )dwNextExp ;
	float fLevel = ( float )nLevel + ( float )dPercent ;
	int nLevel10 = ( int )( fLevel * 10 ) ;
	float fNow = ( float )nLevel10 / 10 ;
	fLevel = fNow ;
	if ( nTable > 0 )
	{
		TableRecord* pRecord = g_objHero.FindTableRecord ( nTable, 0, false ) ;
		if ( pRecord == NULL )
		{
			g_objHero.ActTable ( 1, nTable ) ;
			return ;
		}
		fNow = pRecord->fValue / 10 ;
	}
	sprintf ( szText, GameDataSetQuery()->GetStr ( 11010 ), fLevel, fNow ) ;
	m_StaExp.SetWindowText ( szText ) ;
}

BOOL CDlgExercise::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgExercise::OnBtnHelp()
{
	CDlgExercise::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_EXERCISE ) ;
}
