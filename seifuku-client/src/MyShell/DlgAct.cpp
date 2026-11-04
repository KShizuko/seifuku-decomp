#include "stdafx.h"
#include "MyShell.h"
#include "DlgAct.h"

#include "Hero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgAct::CDlgAct(CWnd* pParent )
	: CDialog(CDlgAct::IDD, pParent)
{
	m_bLoad = false ;
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_Pnt = CPoint ( 0, 0 ) ;
}


void CDlgAct::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ACT_BTN1, m_ActBtn1);
	DDX_Control(pDX, IDC_ACT_BTN2, m_ActBtn2);
	DDX_Control(pDX, IDC_ACT_BTN3, m_ActBtn3);
	DDX_Control(pDX, IDC_ACT_BTN4, m_ActBtn4);
	DDX_Control(pDX, IDC_ACT_BTN5, m_ActBtn5);
	DDX_Control(pDX, IDC_ACT_BTN6, m_ActBtn6);
	DDX_Control(pDX, IDC_ACT_BTN7, m_ActBtn7);
	DDX_Control(pDX, IDC_ACT_BTN8, m_ActBtn8);
	DDX_Control(pDX, IDC_ACT_BTN9, m_ActBtn9);
	DDX_Control(pDX, IDC_ACT_BTN10, m_ActBtn10);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_CloseBtn);
}


BEGIN_MESSAGE_MAP(CDlgAct, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_ACT_BTN1, OnActBtn1)
	ON_BN_CLICKED(IDC_ACT_BTN2, OnActBtn2)
	ON_BN_CLICKED(IDC_ACT_BTN3, OnActBtn3)
	ON_BN_CLICKED(IDC_ACT_BTN4, OnActBtn4)
	ON_BN_CLICKED(IDC_ACT_BTN5, OnActBtn5)
	ON_BN_CLICKED(IDC_ACT_BTN6, OnActBtn6)
	ON_BN_CLICKED(IDC_ACT_BTN7, OnActBtn7)
	ON_BN_CLICKED(IDC_ACT_BTN8, OnActBtn8)
	ON_BN_CLICKED(IDC_ACT_BTN9, OnActBtn9)
	ON_BN_CLICKED(IDC_ACT_BTN10, OnActBtn10)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBtnClose)
END_MESSAGE_MAP()

void CDlgAct::Show()
{
	if ( m_bLoad && m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog22",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0,
							m_Pnt.x,
							m_Pnt.y ) ;
		else
			return ;

		m_ActBtn1.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ActBtn2.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ActBtn3.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ActBtn4.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ActBtn5.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ActBtn6.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ActBtn7.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ActBtn8.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ActBtn9.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ActBtn10.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_CloseBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

LRESULT CDlgAct::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		if ( wParam == false )
		{
			m_bShow = false ;
		}
		else
		{
			m_bShow = true ;
		}
	}
	else if ( message == WM_COMMAND )
	{
		if ( wParam == 2 )
		{
			CDlgAct::OnBtnClose () ;
			return true ;
		}
		else if ( wParam == 1 )
			return true ;
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

HBRUSH CDlgAct::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgAct::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgAct::GetWindowRect ( rect ) ;

	char strName[32] ;
	sprintf ( strName, "ButtonA%d", 230 ) ;
	if ( ! ( m_ActBtn1.Init ( rect.left, rect.top, strName, DLG_ACT, 230 ) ) )
		return false ;
	sprintf ( strName, "ButtonA%d", 210 ) ;
	if ( ! ( m_ActBtn2.Init ( rect.left, rect.top, strName, DLG_ACT, 210 ) ) )
		return false ;
	sprintf ( strName, "ButtonA%d", 170 ) ;
	if ( ! ( m_ActBtn3.Init ( rect.left, rect.top, strName, DLG_ACT, 170 ) ) )
		return false ;
	sprintf ( strName, "ButtonA%d", 150 ) ;
	if ( ! ( m_ActBtn4.Init ( rect.left, rect.top, strName, DLG_ACT, 150 ) ) )
		return false ;
	sprintf ( strName, "ButtonA%d", 160 ) ;
	if ( ! ( m_ActBtn5.Init ( rect.left, rect.top, strName, DLG_ACT, 160 ) ) )
		return false ;
	sprintf ( strName, "ButtonA%d", 270 ) ;
	if ( ! ( m_ActBtn6.Init ( rect.left, rect.top, strName, DLG_ACT, 270 ) ) )
		return false ;
	sprintf ( strName, "ButtonA%d", 1 ) ;
	if ( ! ( m_ActBtn7.Init ( rect.left, rect.top, strName, DLG_ACT, 1 ) ) )
		return false ;
	sprintf ( strName, "ButtonA%d", 190 ) ;
	if ( ! ( m_ActBtn8.Init ( rect.left, rect.top, strName, DLG_ACT, 190 ) ) )
		return false ;
	sprintf ( strName, "ButtonA%d", 200 ) ;
	if ( ! ( m_ActBtn9.Init ( rect.left, rect.top, strName, DLG_ACT, 200 ) ) )
		return false ;
	sprintf ( strName, "ButtonA%d", 250 ) ;
	if ( ! ( m_ActBtn10.Init ( rect.left, rect.top, strName, DLG_ACT, 250 ) ) )
		return false ;
	if ( ! ( m_CloseBtn.Init ( rect.left, rect.top, "Button5" ) ) )
		return false ;

	m_bLoad = true ;
	return TRUE;
}

void CDlgAct::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgAct::OnActBtn1()
{
	g_objHero.Emotion(230);
}

void CDlgAct::OnActBtn2()
{
	g_objHero.Emotion(210);
}

void CDlgAct::OnActBtn3()
{
	g_objHero.Emotion(170);
}

void CDlgAct::OnActBtn4()
{
	g_objHero.Emotion(150);
}

void CDlgAct::OnActBtn5()
{
	g_objHero.Emotion(160);
}

void CDlgAct::OnActBtn6()
{
	g_objHero.Emotion(270);
}

void CDlgAct::OnActBtn7()
{
	g_objHero.Emotion(1);
}

void CDlgAct::OnActBtn8()
{
	g_objHero.Emotion(190);
}

void CDlgAct::OnActBtn9()
{
	g_objHero.Emotion(200);
}

void CDlgAct::OnActBtn10()
{
	g_objHero.Emotion(250);
}

void CDlgAct::OnBtnClose()
{
	CDlgAct::ShowWindow ( SW_HIDE ) ;
}

void CDlgAct::SetCurAct(int nIndex)
{
	switch ( nIndex )
	{
	case 210:
		CDlgAct::OnActBtn2() ;
		break ;
	case 230:
		CDlgAct::OnActBtn1() ;
		break ;
	case 170:
		CDlgAct::OnActBtn3() ;
		break ;
	case 150:
		CDlgAct::OnActBtn4() ;
		break ;
	case 160:
		CDlgAct::OnActBtn5() ;
		break ;
	case 250:
		CDlgAct::OnActBtn10() ;
		break ;
	case 1:
		CDlgAct::OnActBtn7() ;
		break ;
	case 190:
		CDlgAct::OnActBtn8() ;
		break ;
	case 200:
		CDlgAct::OnActBtn9() ;
		break ;
	case 270:
		CDlgAct::OnActBtn6() ;
		break ;
	}
}
