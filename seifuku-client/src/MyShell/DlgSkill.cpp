#include "stdafx.h"
#include "MyShell.h"
#include "DlgSkill.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgSkill::CDlgSkill(CWnd* pParent )
	: CDialog(CDlgSkill::IDD, pParent)
{
	m_bInitSuc = false ;
	m_bShow = false ;
	m_Hbr = CreateSolidBrush ( 0x2132B ) ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_nFrame = 0 ;
}


void CDlgSkill::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SKILL_BTN_CLOSE, m_BtnClose ) ;
	DDX_Control(pDX, 1667, m_BtnH ) ;
	DDX_Control(pDX, IDC_SKILL_BTN_UP, m_BtnUp ) ;
	DDX_Control(pDX, IDC_SKILL_BTN_DOWN, m_BtnDown ) ;
	DDX_Control(pDX, 1513, m_BtnSkill1 ) ;
	DDX_Control(pDX, 1514, m_BtnSkill2 ) ;
	DDX_Control(pDX, 1515, m_BtnSkill3 ) ;
	DDX_Control(pDX, 1516, m_BtnSkill4 ) ;
	DDX_Control(pDX, 1517, m_BtnSkill5 ) ;
	DDX_Control(pDX, 1518, m_BtnSkill6 ) ;
	DDX_Control(pDX, IDC_SKILL_BTN_7, m_BtnSkill7 ) ;
	DDX_Control(pDX, IDC_SKILL_BTN_8, m_BtnSkill8 ) ;
	DDX_Control(pDX, 1507, m_StaLev1 ) ;
	DDX_Control(pDX, 1508, m_StaLev2 ) ;
	DDX_Control(pDX, 1030, m_StaLev3 ) ;
	DDX_Control(pDX, 1031, m_StaLev4 ) ;
	DDX_Control(pDX, 1032, m_StaLev5 ) ;
	DDX_Control(pDX, 1033, m_StaLev6 ) ;
	DDX_Control(pDX, 1034, m_StaLev7 ) ;
	DDX_Control(pDX, 1509, m_StaLev8 ) ;
}


BEGIN_MESSAGE_MAP(CDlgSkill, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_SKILL_BTN_CLOSE, OnSkillBtnClose)
END_MESSAGE_MAP()

LRESULT CDlgSkill::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		if ( wParam == false )
			m_bShow = false ;
		else
		{
			m_bShow = true ;
			m_fFrameStep = ( float )m_nHeight / m_nFrame ;
		}
	}
	else if ( message == WM_ERASEBKGND )
		return true ;
	else if ( message == WM_NCPAINT )
		return false ;
	else if ( message == WM_PAINT )
	{
		PAINTSTRUCT ps ;
		HDC hdc = ::BeginPaint ( m_hWnd, &ps ) ;
		::EndPaint ( m_hWnd, &ps ) ;
		return false ;
	}
	else if ( message == WM_DESTROY )
		::DeleteObject ( m_Hbr ) ;

	return CDialog::WindowProc(message, wParam, lParam);
}

HBRUSH CDlgSkill::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgSkill::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgSkill::GetWindowRect ( rect ) ;
	m_nWidth = rect.right - rect.left ;
	m_nHeight = rect.bottom - rect.top ;

	if ( !m_BtnClose.Init ( rect.left, rect.top, "Button5" ) )
		return false ;
	if ( !m_BtnH.Init ( rect.left, rect.top, "Button4" ) )
		return false ;
	if ( !m_BtnUp.Init ( rect.left, rect.top, "ScrButton0" ) )
		return false ;
	if ( !m_BtnDown.Init ( rect.left, rect.top, "ScrButton1" ) )
		return false ;
	if ( !m_BtnSkill1.Init ( rect.left, rect.top, "Button130" ) )
		return false ;
	if ( !m_BtnSkill2.Init ( rect.left, rect.top, "Button130" ) )
		return false ;
	if ( !m_BtnSkill3.Init ( rect.left, rect.top, "Button130" ) )
		return false ;
	if ( !m_BtnSkill4.Init ( rect.left, rect.top, "Button130" ) )
		return false ;
	if ( !m_BtnSkill5.Init ( rect.left, rect.top, "Button130" ) )
		return false ;
	if ( !m_BtnSkill6.Init ( rect.left, rect.top, "Button130" ) )
		return false ;
	if ( !m_BtnSkill7.Init ( rect.left, rect.top, "Button130" ) )
		return false ;
	if ( !m_BtnSkill8.Init ( rect.left, rect.top, "Button130" ) )
		return false ;
	if ( !m_StaLev1.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaLev2.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaLev3.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaLev4.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaLev5.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaLev6.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaLev7.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	if ( !m_StaLev8.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;

	m_bInitSuc = true ;
	return TRUE;
}

void CDlgSkill::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgSkill::Show()
{
	if ( m_nFrame != 0 )
		CDlgSkill::Slide () ;

	if ( m_bInitSuc && m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog13",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0, m_Pnt.x, m_Pnt.y - 1 ) ;
		else
			return ;

		m_BtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnH.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnUp.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnDown.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnSkill1.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnSkill2.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnSkill3.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnSkill4.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnSkill5.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnSkill6.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnSkill7.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnSkill8.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaLev1.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaLev2.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaLev3.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaLev4.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaLev5.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaLev6.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaLev7.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaLev8.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgSkill::OnSkillBtnClose()
{
	if ( m_nFrame > 0 )
		m_nFrame = -m_nFrame ;
	else if ( m_nFrame == 0 )
		m_nFrame = -15 ;
}

void CDlgSkill::Slide()
{
	if ( m_nFrame > 0 )
	{
		m_nFrame-- ;
		if ( m_nFrame >= 0 )
			CDlgSkill::MoveWindow ( 0, ( int )( __int64 )( -( m_nFrame * m_fFrameStep ) ), m_nWidth, m_nHeight, true ) ;
	}
	else
	{
		m_nFrame++ ;
		if ( m_nFrame < 0 )
			CDlgSkill::MoveWindow ( 0, ( int )( __int64 )( -( m_nFrame * m_fFrameStep ) - m_nHeight ), m_nWidth, m_nHeight, true ) ;
		else
			CDlgSkill::ShowWindow ( SW_HIDE ) ;
	}
}
