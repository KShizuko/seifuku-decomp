#include "stdafx.h"
#include "MyShell.h"
#include "DlgFacState.h"

#include "Hero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgFacState::CDlgFacState(CWnd* pParent )
	: CDialog(CDlgFacState::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
}


void CDlgFacState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STA4, m_StaProffer);
	DDX_Control(pDX, IDC_STA3, m_StaSynJob);
	DDX_Control(pDX, IDC_STA2, m_StaLevel);
	DDX_Control(pDX, IDC_STA1, m_StaName);
	DDX_Control(pDX, IDC_FACSTATE_BTN_CLOSE, m_BtnClose);
}


BEGIN_MESSAGE_MAP(CDlgFacState, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_FACSTATE_BTN_CLOSE, OnFacstateBtnClose)
END_MESSAGE_MAP()

LRESULT CDlgFacState::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
		if ( wParam )
			DXPlaySound ( "Sound/BtnClick.wav" ) ;
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

HBRUSH CDlgFacState::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgFacState::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgFacState::GetWindowRect ( rect ) ;

	m_StaName.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_StaLevel.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_StaSynJob.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_StaProffer.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;

	m_BtnClose.Init ( rect.left, rect.top, "Button5" ) ;

	return TRUE;
}

void CDlgFacState::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgFacState::Show()
{
	if ( m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog41",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0,
							m_Pnt.x,
							m_Pnt.y ) ;
		else
			return ;

		m_StaName.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaLevel.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaSynJob.Show(m_Pnt.x, m_Pnt.y ) ;
		m_StaProffer.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgFacState::SetFacState( const char* strMemName )
{
	char szName[16] ;
	char szLevel[16] ;
	int nLevel ;
	sscanf ( strMemName, "%s %d", szName, &nLevel ) ;
	itoa ( nLevel, szLevel, 10 ) ;
	m_StaName.SetWindowText ( szName ) ;
	m_StaLevel.SetWindowText ( szLevel ) ;

	SynMemberInfo* pMemberInfo = g_objHero.GetReturnSynMemberInfo () ;
	if ( pMemberInfo )
	{
		m_StaSynJob.SetWindowText ( pMemberInfo->szRank ) ;
		m_StaProffer.SetWindowText ( pMemberInfo->szProffer ) ;
	}
	CDlgFacState::ShowWindow ( SW_SHOW ) ;
}

void CDlgFacState::OnFacstateBtnClose()
{
	CDlgFacState::ShowWindow ( SW_HIDE ) ;
}
