#include "stdafx.h"
#include "MyShell.h"
#include "DlgXp.h"

#include "Hero.h"
#include "Magic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgXp::CDlgXp(CWnd* pParent )
	: CDialog(CDlgXp::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = CreateSolidBrush ( 0x2132B ) ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_bDie = false ;
	m_bTransform = false ;
	m_bFly = false ;
	m_btBoothType = 0 ;
}


void CDlgXp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_XP_BTN_UP, m_XpBtnUp);
	DDX_Control(pDX, IDC_XP_BTN_DOWN, m_XpBtnDown);
	DDX_Control(pDX, IDC_XP_IMG10, m_XpImg10);
	DDX_Control(pDX, IDC_XP_IMG9, m_XpImg9);
	DDX_Control(pDX, IDC_XP_IMG8, m_XpImg8);
	DDX_Control(pDX, IDC_XP_IMG7, m_XpImg7);
	DDX_Control(pDX, IDC_XP_IMG6, m_XpImg6);
	DDX_Control(pDX, IDC_XP_IMG5, m_XpImg5);
	DDX_Control(pDX, IDC_XP_IMG4, m_XpImg4);
	DDX_Control(pDX, IDC_XP_IMG3, m_XpImg3);
	DDX_Control(pDX, IDC_XP_IMG2, m_XpImg2);
	DDX_Control(pDX, IDC_XP_IMG1, m_XpImg1);
}


BEGIN_MESSAGE_MAP(CDlgXp, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_XP_IMG1, OnXpImg1)
	ON_BN_CLICKED(IDC_XP_IMG2, OnXpImg2)
	ON_BN_CLICKED(IDC_XP_IMG3, OnXpImg3)
	ON_BN_CLICKED(IDC_XP_IMG4, OnXpImg4)
	ON_BN_CLICKED(IDC_XP_IMG5, OnXpImg5)
	ON_BN_CLICKED(IDC_XP_IMG6, OnXpImg6)
	ON_BN_CLICKED(IDC_XP_IMG7, OnXpImg7)
	ON_BN_CLICKED(IDC_XP_IMG8, OnXpImg8)
	ON_BN_CLICKED(IDC_XP_IMG9, OnXpImg9)
	ON_BN_CLICKED(IDC_XP_IMG10, OnXpImg10)
	ON_BN_CLICKED(IDC_XP_BTN_UP, OnXpBtnUp)
	ON_BN_CLICKED(IDC_XP_BTN_DOWN, OnXpBtnDown)
END_MESSAGE_MAP()

LRESULT CDlgXp::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
		if ( !m_bShow )
		{
			CDlgXp::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_HIDE ) ;
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

HBRUSH CDlgXp::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgXp::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgXp::GetWindowRect ( rect ) ;

	m_XpImg1.Init ( rect.left, rect.top, NULL, XP_IMAGE ) ;
	m_XpImg2.Init ( rect.left, rect.top, NULL, XP_IMAGE ) ;
	m_XpImg3.Init ( rect.left, rect.top, NULL, XP_IMAGE ) ;
	m_XpImg4.Init ( rect.left, rect.top, NULL, XP_IMAGE ) ;
	m_XpImg5.Init ( rect.left, rect.top, NULL, XP_IMAGE ) ;
	m_XpImg6.Init ( rect.left, rect.top, NULL, XP_IMAGE ) ;
	m_XpImg7.Init ( rect.left, rect.top, NULL, XP_IMAGE ) ;
	m_XpImg8.Init ( rect.left, rect.top, NULL, XP_IMAGE ) ;
	m_XpImg9.Init ( rect.left, rect.top, NULL, XP_IMAGE ) ;
	m_XpImg10.Init ( rect.left, rect.top, NULL, XP_IMAGE ) ;

	m_XpBtnUp.Init ( rect.left, rect.top, "Button173" ) ;
	m_XpBtnUp.EnableWindow ( false ) ;
	m_XpBtnDown.Init ( rect.left, rect.top, "Button174" ) ;
	m_XpBtnDown.EnableWindow ( false ) ;

	return TRUE;
}

void CDlgXp::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgXp::Show()
{
	if ( m_bShow )
	{
		m_XpImg1.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_XpImg2.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_XpImg3.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_XpImg4.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_XpImg5.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_XpImg6.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_XpImg7.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_XpImg8.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_XpImg9.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_XpImg10.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_XpBtnUp.IsWindowEnabled () )
			m_XpBtnUp.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_XpBtnDown.IsWindowEnabled () )
			m_XpBtnDown.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgXp::InitXpDialog()
{
	m_btBoothType = 0 ;
	m_nCurBase = 0 ;
	m_bFly = false ;
	if ( m_bDie )
		return ;
	int nAmount = g_objHero.GetXpSkillAmount() ;
	if ( nAmount > 10 )
	{
		m_XpBtnUp.EnableWindow ( true ) ;
		m_XpBtnDown.EnableWindow ( true ) ;
	}
	else
	{
		m_XpBtnUp.EnableWindow ( false ) ;
		m_XpBtnDown.EnableWindow ( false ) ;
	}
	for ( int i = 0; i < nAmount; i++ )
	{
		CMagic* pXp = g_objHero.GetXpSkillByIndex(i) ;
		if ( pXp )
		{
			if ( g_objHero.IsTransform () && pXp->IsTransformMagic () )
				continue ;
			char SkillType[32] ;
			BOOL bDisable = !pXp->IsEnable () ;
			m_XpIdType[i] = pXp->GetIDType () ;
			sprintf ( SkillType, "XpSkillType%d", m_XpIdType[i] ) ;
			switch ( i )
			{
			case 0:
				m_XpImg1.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
				break ;
			case 1:
				m_XpImg2.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
				break ;
			case 2:
				m_XpImg3.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
				break ;
			case 3:
				m_XpImg4.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
				break ;
			case 4:
				m_XpImg5.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
				break ;
			case 5:
				m_XpImg6.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
				break ;
			case 6:
				m_XpImg7.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
				break ;
			case 7:
				m_XpImg8.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
				break ;
			case 8:
				m_XpImg9.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
				break ;
			case 9:
				m_XpImg10.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
				break ;
			default:
				break;
			}
		}
	}
	CDlgXp::ShowWindow ( SW_SHOW ) ;
}

void CDlgXp::OnXpImg1()
{
	if ( m_bDie )
	{
		if ( g_objHero.ReBorn ( 0 ) )
		{
			CDlgXp::ShowWindow ( SW_HIDE ) ;
			m_bDie = false ;
		}
		return ;
	}
	else if (m_btBoothType)
	{
		CDlgXp::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MPCBOOTH_CLOSE ) ;
		g_objHero.m_objBoothManager.Close();
		CDlgXp::ShowWindow(SW_HIDE);
		m_btBoothType = false;
		if ( m_bTransform )
		{
			CDlgXp::MoveWindow ( CMyBitmap::GetScreenWidth () - 68, CMyBitmap::GetScreenHeight () - 173, 68, 50 ) ;
			m_XpImg1.InsertImage ( "XpSkillType3" ) ;
			CDlgXp::ShowWindow ( SW_SHOW ) ;
		}
		return;
	}
	else if ( m_bTransform )
	{
		g_objHero.AbortTransform () ;
		m_bTransform = false ;
		CDlgXp::ShowWindow ( SW_HIDE ) ;
		return ;
	}
	else if ( m_bFly )
	{
		g_objHero.ApplyLanding () ;
		m_bFly = false ;
		CDlgXp::ShowWindow ( SW_HIDE ) ;
		return ;
	}
	else
		CDlgXp::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_XP_CHOOSE, m_XpIdType[m_nCurBase] ) ;
}

void CDlgXp::OnXpImg2()
{
	if ( m_bDie )
	{
		if ( g_objHero.ReBorn ( 1 ) )
		{
			CDlgXp::ShowWindow ( SW_HIDE ) ;
			m_bDie = false ;
		}
		return ;
	}
	if ( m_bTransform )
	{
		g_objHero.AbortTransform () ;
		m_bTransform = false ;
		if ( m_btBoothType == 2 )
		{
			CDlgXp::MoveWindow ( CMyBitmap::GetScreenWidth () - 68, CMyBitmap::GetScreenHeight () - 173, 68, 50 ) ;
			m_XpImg1.InsertImage ( "XpSkillType2" ) ;
		}
		else
			CDlgXp::ShowWindow ( SW_HIDE ) ;
		return ;
	}
	CDlgXp::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_XP_CHOOSE, m_XpIdType[m_nCurBase + 1] ) ;
}

void CDlgXp::OnXpImg3()
{
	CDlgXp::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_XP_CHOOSE, m_XpIdType[m_nCurBase + 2] ) ;
}

void CDlgXp::OnXpImg4()
{
	CDlgXp::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_XP_CHOOSE, m_XpIdType[m_nCurBase + 3] ) ;
}

void CDlgXp::OnXpImg5()
{
	CDlgXp::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_XP_CHOOSE, m_XpIdType[m_nCurBase + 4] ) ;
}

void CDlgXp::OnXpImg6()
{
	CDlgXp::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_XP_CHOOSE, m_XpIdType[m_nCurBase + 5] ) ;
}

void CDlgXp::OnXpImg7()
{
	CDlgXp::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_XP_CHOOSE, m_XpIdType[m_nCurBase + 6] ) ;
}

void CDlgXp::OnXpImg8()
{
	CDlgXp::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_XP_CHOOSE, m_XpIdType[m_nCurBase + 7] ) ;
}

void CDlgXp::OnXpImg9()
{
	CDlgXp::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_XP_CHOOSE, m_XpIdType[m_nCurBase + 8] ) ;
}

void CDlgXp::OnXpImg10()
{
	CDlgXp::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_XP_CHOOSE, m_XpIdType[m_nCurBase + 9] ) ;
}

void CDlgXp::SetReborn(BOOL bDie)
{
	m_XpBtnUp.EnableWindow ( false ) ;
	m_XpBtnDown.EnableWindow ( false ) ;
	if (bDie)
	{
		m_bDie = true ;
		CDlgXp::MoveWindow ( CMyBitmap::GetScreenWidth () - 118, CMyBitmap::GetScreenHeight () - 173, 118, 50 ) ;
		m_XpImg1.InsertImage ( "XpSkillType0" ) ;
		m_XpImg2.InsertImage ( "XpSkillType5" ) ;
		CDlgXp::ShowWindow ( SW_SHOW ) ;
	}
	else
	{
		m_bDie = false;
		CDlgXp::ShowWindow ( SW_HIDE ) ;
	}
}

void CDlgXp::SetBooth(BOOL bOpen)
{
	m_XpBtnUp.EnableWindow ( false ) ;
	m_XpBtnDown.EnableWindow ( false ) ;
	CDlgXp::ShowWindow ( SW_HIDE ) ;
	if ( m_bTransform )
	{
		CDlgXp::MoveWindow ( CMyBitmap::GetScreenWidth () - 118, CMyBitmap::GetScreenHeight () - 173, 118, 50 ) ;
		m_XpImg2.InsertImage ( "XpSkillType3" ) ;
	}
	else
	{
		CDlgXp::MoveWindow ( CMyBitmap::GetScreenWidth () - 68, CMyBitmap::GetScreenHeight () - 173, 68, 50 ) ;
	}
	m_XpImg1.InsertImage ( "XpSkillType2" ) ;
	m_btBoothType = 2 ;
	CDlgXp::ShowWindow( SW_SHOW );
}

void CDlgXp::SetTransform(BOOL bTrans)
{
	m_XpBtnUp.EnableWindow ( false ) ;
	m_XpBtnDown.EnableWindow ( false ) ;
	CDlgXp::ShowWindow ( SW_HIDE ) ;
	if ( bTrans )
	{
		if ( m_btBoothType == 2 )
		{
			CDlgXp::MoveWindow ( CMyBitmap::GetScreenWidth () - 118, CMyBitmap::GetScreenHeight () - 173, 118, 50 ) ;
			m_XpImg1.InsertImage ( "XpSkillType2" ) ;
			m_XpImg2.InsertImage ( "XpSkillType3" ) ;
		}
		else
		{
			CDlgXp::MoveWindow ( CMyBitmap::GetScreenWidth () - 68, CMyBitmap::GetScreenHeight () - 173, 68, 50 ) ;
			m_XpImg1.InsertImage ( "XpSkillType3" ) ;
		}
		m_bTransform = true ;
		CDlgXp::ShowWindow ( SW_SHOW ) ;
	}
	else
	{
		m_bTransform = false ;
		CDlgXp::ShowWindow ( SW_HIDE ) ;
		if ( m_btBoothType == 2 )
		{
			CDlgXp::MoveWindow ( CMyBitmap::GetScreenWidth () - 68, CMyBitmap::GetScreenHeight () - 173, 68, 50 ) ;
			m_XpImg1.InsertImage ( "XpSkillType2" ) ;
			CDlgXp::ShowWindow ( SW_SHOW ) ;
		}
	}
}

void CDlgXp::SetFly(BOOL bFly)
{
	if ( bFly )
	{
		m_bFly = true ;
		CDlgXp::MoveWindow ( CMyBitmap::GetScreenWidth () - 68, CMyBitmap::GetScreenHeight () - 173, 68, 50 ) ;
		m_XpImg1.InsertImage ( "XpSkillType4" ) ;
		CDlgXp::ShowWindow ( SW_SHOW ) ;
	}
	else
	{
		m_bFly = false ;
		CDlgXp::ShowWindow ( SW_HIDE ) ;
	}
}

void CDlgXp::OnXpBtnUp()
{
	if ( m_nCurBase > 0 )
	{
		CDlgXp::MoveWindow ( CMyBitmap::GetScreenWidth () - 518, CMyBitmap::GetScreenHeight () - 173, 518, 50 ) ;
		m_nCurBase -= 10 ;
		for ( int i = m_nCurBase; i < m_nCurBase + 10; i++ )
		{
			CMagic* pXp = g_objHero.GetXpSkillByIndex(i) ;
			if ( pXp )
			{
				char SkillType[32] ;
				BOOL bDisable = !pXp->IsEnable () ;
				m_XpIdType[i] = pXp->GetIDType () ;
				sprintf ( SkillType, "XpSkillType%d", m_XpIdType[i] ) ;
				switch ( i - m_nCurBase )
				{
				case 0:
					m_XpImg1.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 1:
					m_XpImg2.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 2:
					m_XpImg3.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 3:
					m_XpImg4.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 4:
					m_XpImg5.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 5:
					m_XpImg6.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 6:
					m_XpImg7.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 7:
					m_XpImg8.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 8:
					m_XpImg9.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 9:
					m_XpImg10.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				}
			}
		}
	}
}

void CDlgXp::OnXpBtnDown()
{
	int nAmount = g_objHero.GetXpSkillAmount() ;
	if ( nAmount > 10 && m_nCurBase + 10 < nAmount )
	{
		m_nCurBase += 10 ;
		int nEnd = nAmount ;
		if ( nAmount > m_nCurBase + 10 )
			nEnd = m_nCurBase + 10 ;
		CDlgXp::MoveWindow ( CMyBitmap::GetScreenWidth () - ( nEnd - m_nCurBase ) * 50 - 18, CMyBitmap::GetScreenHeight () - 173, ( nEnd - m_nCurBase ) * 50 + 18, 50 ) ;
		for ( int i = m_nCurBase; i < nEnd; i++ )
		{
			CMagic* pXp = g_objHero.GetXpSkillByIndex(i) ;
			if ( pXp )
			{
				char SkillType[32] ;
				BOOL bDisable = !pXp->IsEnable () ;
				m_XpIdType[i] = pXp->GetIDType () ;
				sprintf ( SkillType, "XpSkillType%d", m_XpIdType[i] ) ;
				switch ( i - m_nCurBase )
				{
				case 0:
					m_XpImg1.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 1:
					m_XpImg2.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 2:
					m_XpImg3.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 3:
					m_XpImg4.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 4:
					m_XpImg5.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 5:
					m_XpImg6.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 6:
					m_XpImg7.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 7:
					m_XpImg8.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 8:
					m_XpImg9.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				case 9:
					m_XpImg10.InsertImage ( SkillType, m_XpIdType[i], bDisable ) ;
					break ;
				}
			}
		}
	}
}
