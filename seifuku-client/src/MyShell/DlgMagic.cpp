#include "stdafx.h"
#include "MyShell.h"
#include "DlgMagic.h"

#include "Hero.h"
#include "Magic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgMagic::CDlgMagic(CWnd* pParent )
	: CDialog(CDlgMagic::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_nCurBase = 0 ;
}


void CDlgMagic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAGIC_BTN_UP, m_MagicBtnU);
	DDX_Control(pDX, IDC_MAGIC_BTN_DOWN, m_MagicBtnD);
	DDX_Control(pDX, IDC_MAGIC_IMG10, m_MagicImg10);
	DDX_Control(pDX, IDC_MAGIC_IMG9, m_MagicImg9);
	DDX_Control(pDX, IDC_MAGIC_IMG8, m_MagicImg8);
	DDX_Control(pDX, IDC_MAGIC_IMG7, m_MagicImg7);
	DDX_Control(pDX, IDC_MAGIC_IMG6, m_MagicImg6);
	DDX_Control(pDX, IDC_MAGIC_IMG5, m_MagicImg5);
	DDX_Control(pDX, IDC_MAGIC_IMG4, m_MagicImg4);
	DDX_Control(pDX, IDC_MAGIC_IMG3, m_MagicImg3);
	DDX_Control(pDX, IDC_MAGIC_IMG2, m_MagicImg2);
	DDX_Control(pDX, IDC_MAGIC_IMG1, m_MagicImg1);
}


BEGIN_MESSAGE_MAP(CDlgMagic, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_MAGIC_IMG1, OnMagicImg1)
	ON_BN_CLICKED(IDC_MAGIC_IMG2, OnMagicImg2)
	ON_BN_CLICKED(IDC_MAGIC_IMG3, OnMagicImg3)
	ON_BN_CLICKED(IDC_MAGIC_IMG4, OnMagicImg4)
	ON_BN_CLICKED(IDC_MAGIC_IMG5, OnMagicImg5)
	ON_BN_CLICKED(IDC_MAGIC_IMG6, OnMagicImg6)
	ON_BN_CLICKED(IDC_MAGIC_IMG7, OnMagicImg7)
	ON_BN_CLICKED(IDC_MAGIC_IMG8, OnMagicImg8)
	ON_BN_CLICKED(IDC_MAGIC_IMG9, OnMagicImg9)
	ON_BN_CLICKED(IDC_MAGIC_IMG10, OnMagicImg10)
	ON_BN_CLICKED(IDC_MAGIC_BTN_DOWN, OnMagicBtnDown)
	ON_BN_CLICKED(IDC_MAGIC_BTN_UP, OnMagicBtnUp)
END_MESSAGE_MAP()

LRESULT CDlgMagic::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
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

HBRUSH CDlgMagic::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgMagic::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgMagic::GetWindowRect ( rect ) ;

	m_MagicImg1.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ;
	m_MagicImg2.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ;
	m_MagicImg3.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ;
	m_MagicImg4.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ;
	m_MagicImg5.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ;
	m_MagicImg6.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ;
	m_MagicImg7.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ;
	m_MagicImg8.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ;
	m_MagicImg9.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ;
	m_MagicImg10.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ;

	m_MagicBtnU.Init ( rect.left, rect.top, "Button173" ) ;
	m_MagicBtnU.EnableWindow ( false ) ;
	m_MagicBtnD.Init ( rect.left, rect.top, "Button174" ) ;
	m_MagicBtnD.EnableWindow ( false ) ;

	return TRUE;
}

void CDlgMagic::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgMagic::Show()
{
	if ( m_bShow )
	{
		m_MagicImg1.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MagicImg2.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MagicImg3.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MagicImg4.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MagicImg5.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MagicImg6.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MagicImg7.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MagicImg8.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MagicImg9.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MagicImg10.Show ( m_Pnt.x, m_Pnt.y ) ;

		if ( m_MagicBtnU.IsWindowEnabled () )
			m_MagicBtnU.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_MagicBtnD.IsWindowEnabled () )
			m_MagicBtnD.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgMagic::InitMagicDialog()
{
	m_nCurBase = 0 ;
	int nAmount = g_objHero.GetMagicAmount() ;
	if ( nAmount > 10 )
	{
		m_MagicBtnU.EnableWindow ( true ) ;
		m_MagicBtnD.EnableWindow ( true ) ;
	}
	else
	{
		m_MagicBtnU.EnableWindow ( false ) ;
		m_MagicBtnD.EnableWindow ( false ) ;
	}

	for ( int i = 0; i < 10; i++ )
	{
		CMagic* pMagic = g_objHero.GetMagicByIndex(i) ;
		if ( pMagic )
		{
			if ( ! pMagic->IsInMagicList () )
				continue ;
			if ( g_objHero.IsTransform () && pMagic->IsTransformMagic () )
				continue ;
			char SkillType[32] ;
			BOOL bDisable = ! pMagic->IsEnable () ;
			m_MagicIdType[i] = pMagic->GetIDType () ;
			sprintf ( SkillType, "MagicSkillType%d", m_MagicIdType[i] ) ;
			switch ( i )
			{
			case 0:
				m_MagicImg1.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
				break ;
			case 1:
				m_MagicImg2.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
				break ;
			case 2:
				m_MagicImg3.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
				break ;
			case 3:
				m_MagicImg4.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
				break ;
			case 4:
				m_MagicImg5.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
				break ;
			case 5:
				m_MagicImg6.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
				break ;
			case 6:
				m_MagicImg7.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
				break ;
			case 7:
				m_MagicImg8.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
				break ;
			case 8:
				m_MagicImg9.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
				break ;
			case 9:
				m_MagicImg10.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
				break ;
			}
		}
	}
	CDlgMagic::ShowWindow ( SW_SHOW ) ;
}

void CDlgMagic::OnMagicImg1()
{
	CDlgMagic::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MAGIC_CHOOSE, m_MagicIdType[0 + m_nCurBase] ) ;
	CDlgMagic::ShowWindow ( SW_HIDE ) ;
}

void CDlgMagic::OnMagicImg2()
{
	CDlgMagic::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MAGIC_CHOOSE, m_MagicIdType[1 + m_nCurBase] ) ;
	CDlgMagic::ShowWindow ( SW_HIDE ) ;
}

void CDlgMagic::OnMagicImg3()
{
	CDlgMagic::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MAGIC_CHOOSE, m_MagicIdType[2 + m_nCurBase] ) ;
	CDlgMagic::ShowWindow ( SW_HIDE ) ;
}

void CDlgMagic::OnMagicImg4()
{
	CDlgMagic::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MAGIC_CHOOSE, m_MagicIdType[3 + m_nCurBase] ) ;
	CDlgMagic::ShowWindow ( SW_HIDE ) ;
}

void CDlgMagic::OnMagicImg5()
{
	CDlgMagic::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MAGIC_CHOOSE, m_MagicIdType[4 + m_nCurBase] ) ;
	CDlgMagic::ShowWindow ( SW_HIDE ) ;
}

void CDlgMagic::OnMagicImg6()
{
	CDlgMagic::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MAGIC_CHOOSE, m_MagicIdType[5 + m_nCurBase] ) ;
	CDlgMagic::ShowWindow ( SW_HIDE ) ;
}

void CDlgMagic::OnMagicImg7()
{
	CDlgMagic::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MAGIC_CHOOSE, m_MagicIdType[6 + m_nCurBase] ) ;
	CDlgMagic::ShowWindow ( SW_HIDE ) ;
}

void CDlgMagic::OnMagicImg8()
{
	CDlgMagic::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MAGIC_CHOOSE, m_MagicIdType[7 + m_nCurBase] ) ;
	CDlgMagic::ShowWindow ( SW_HIDE ) ;
}

void CDlgMagic::OnMagicImg9()
{
	CDlgMagic::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MAGIC_CHOOSE, m_MagicIdType[8 + m_nCurBase] ) ;
	CDlgMagic::ShowWindow ( SW_HIDE ) ;
}

void CDlgMagic::OnMagicImg10()
{
	CDlgMagic::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MAGIC_CHOOSE, m_MagicIdType[9 + m_nCurBase] ) ;
	CDlgMagic::ShowWindow ( SW_HIDE ) ;
}

void CDlgMagic::OnMagicBtnDown()
{
	int nAmount = g_objHero.GetMagicAmount() ;
	if ( nAmount > 10 && m_nCurBase + 10 < nAmount )
	{
		m_nCurBase += 10 ;
		nAmount = ( nAmount > m_nCurBase + 10 ) ? m_nCurBase + 10 : nAmount ;
		CDlgMagic::MoveWindow ( CMyBitmap::GetScreenWidth () - ( nAmount - m_nCurBase ) * MAGIC_ICON_SIZE - 18,
								CMyBitmap::GetScreenHeight () - DLG_MAIN_HEIGHT + 18,
								( nAmount - m_nCurBase ) * MAGIC_ICON_SIZE + 18,
								MAGIC_ICON_SIZE ) ;
		for ( int i = m_nCurBase; i < nAmount; i++ )
		{
			CMagic* pMagic = g_objHero.GetMagicByIndex(i) ;
			if ( pMagic )
			{
				if ( ! pMagic->IsInMagicList () )
					continue ;
				char SkillType[32] ;
				BOOL bDisable = ! pMagic->IsEnable () ;
				m_MagicIdType[i] = pMagic->GetIDType () ;
				sprintf ( SkillType, "MagicSkillType%d", m_MagicIdType[i] ) ;
				switch ( i - m_nCurBase )
				{
				case 0:
					m_MagicImg1.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 1:
					m_MagicImg2.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 2:
					m_MagicImg3.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 3:
					m_MagicImg4.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 4:
					m_MagicImg5.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 5:
					m_MagicImg6.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 6:
					m_MagicImg7.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 7:
					m_MagicImg8.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 8:
					m_MagicImg9.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 9:
					m_MagicImg10.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				}
			}
		}
	}
}

void CDlgMagic::OnMagicBtnUp()
{
	if ( m_nCurBase > 0 )
	{
		CDlgMagic::MoveWindow ( CMyBitmap::GetScreenWidth () - 10 * MAGIC_ICON_SIZE - 18,
								CMyBitmap::GetScreenHeight () - DLG_MAIN_HEIGHT + 18,
								10 * MAGIC_ICON_SIZE + 18,
								MAGIC_ICON_SIZE ) ;
		m_nCurBase -= 10 ;
		for ( int i = m_nCurBase; i < m_nCurBase + 10; i++ )
		{
			CMagic* pMagic = g_objHero.GetMagicByIndex(i) ;
			if ( pMagic )
			{
				if ( ! pMagic->IsInMagicList () )
					continue ;
				char SkillType[32] ;
				BOOL bDisable = ! pMagic->IsEnable () ;
				m_MagicIdType[i] = pMagic->GetIDType () ;
				sprintf ( SkillType, "MagicSkillType%d", m_MagicIdType[i] ) ;
				switch ( i - m_nCurBase )
				{
				case 0:
					m_MagicImg1.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 1:
					m_MagicImg2.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 2:
					m_MagicImg3.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 3:
					m_MagicImg4.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 4:
					m_MagicImg5.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 5:
					m_MagicImg6.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 6:
					m_MagicImg7.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 7:
					m_MagicImg8.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 8:
					m_MagicImg9.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				case 9:
					m_MagicImg10.InsertImage ( SkillType, m_MagicIdType[i], bDisable ) ;
					break ;
				}
			}
		}
	}
}
