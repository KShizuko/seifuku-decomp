#include "stdafx.h"
#include "MyShell.h"
#include "DlgQuery_Weapon.h"

#include "Hero.h"
#include "ND_Bitmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgQuery_Weapon::CDlgQuery_Weapon(CWnd* pParent )
	: CDialog(CDlgQuery_Weapon::IDD, pParent)
{
	m_Hbr = CreateSolidBrush ( 0x2132B ) ;
	m_DlgPnt.x = 0 ;
	m_DlgPnt.y = 0 ;
	m_nWeaponClass = 0 ;
	m_bShowInfo = false ;
	m_nTopIndex = 0 ;
	m_pCurSkill = NULL ;
}


void CDlgQuery_Weapon::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WEAPON_BTN_UP, m_BtnUp);
	DDX_Control(pDX, IDC_WEAPON_BTN_DOWN, m_BtnDown);
	DDX_Control(pDX, IDC_WEAPON_BTN_S, m_BtnS);
	DDX_Control(pDX, IDC_WEAPON_BTN_D, m_BtnD);
	DDX_Control(pDX, IDC_WEAPON_BTN_O, m_BtnO);
	DDX_Control(pDX, IDC_WEAPON_IMA1, m_Ima1);
	DDX_Control(pDX, IDC_WEAPON_IMA2, m_Ima2);
	DDX_Control(pDX, IDC_WEAPON_IMA3, m_Ima3);
	DDX_Control(pDX, IDC_WEAPON_IMA4, m_Ima4);
	DDX_Control(pDX, IDC_WEAPON_IMA5, m_Ima5);
	DDX_Control(pDX, IDC_WEAPON_IMA6, m_Ima6);
	DDX_Control(pDX, IDC_WEAPON_IMA11, m_Ima11);
	DDX_Control(pDX, IDC_WEAPON_IMA22, m_Ima22);
	DDX_Control(pDX, IDC_WEAPON_IMA33, m_Ima33);
	DDX_Control(pDX, IDC_WEAPON_IMA44, m_Ima44);
	DDX_Control(pDX, IDC_WEAPON_IMA55, m_Ima55);
	DDX_Control(pDX, IDC_WEAPON_IMA66, m_Ima66);
}


BEGIN_MESSAGE_MAP(CDlgQuery_Weapon, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_WEAPON_BTN_S, OnWeaponBtnS)
	ON_BN_CLICKED(IDC_WEAPON_BTN_D, OnWeaponBtnD)
	ON_BN_CLICKED(IDC_WEAPON_BTN_O, OnWeaponBtnO)
	ON_BN_CLICKED(IDC_WEAPON_IMA1, OnWeaponIma1)
	ON_BN_CLICKED(IDC_WEAPON_IMA11, OnWeaponIma11)
	ON_BN_CLICKED(IDC_WEAPON_IMA2, OnWeaponIma2)
	ON_BN_CLICKED(IDC_WEAPON_IMA22, OnWeaponIma22)
	ON_BN_CLICKED(IDC_WEAPON_IMA3, OnWeaponIma3)
	ON_BN_CLICKED(IDC_WEAPON_IMA33, OnWeaponIma33)
	ON_BN_CLICKED(IDC_WEAPON_IMA4, OnWeaponIma4)
	ON_BN_CLICKED(IDC_WEAPON_IMA44, OnWeaponIma44)
	ON_BN_CLICKED(IDC_WEAPON_IMA5, OnWeaponIma5)
	ON_BN_CLICKED(IDC_WEAPON_IMA55, OnWeaponIma55)
	ON_BN_CLICKED(IDC_WEAPON_IMA6, OnWeaponIma6)
	ON_BN_CLICKED(IDC_WEAPON_IMA66, OnWeaponIma66)
	ON_BN_CLICKED(IDC_WEAPON_BTN_UP, OnWeaponBtnUp)
	ON_BN_CLICKED(IDC_WEAPON_BTN_DOWN, OnWeaponBtnDown)
END_MESSAGE_MAP()

HBRUSH CDlgQuery_Weapon::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgQuery_Weapon::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgQuery_Weapon::GetWindowRect ( rect ) ;

	if ( ! ( m_BtnS.Init ( rect.left, rect.top, "Button90" ) ) )
		return false ;
	if ( ! ( m_BtnD.Init ( rect.left, rect.top, "Button91" ) ) )
		return false ;
	if ( ! ( m_BtnO.Init ( rect.left, rect.top, "Button92" ) ) )
		return false ;
	if ( ! ( m_BtnDown.Init ( rect.left, rect.top, "Button174" ) ) )
		return false ;
	if ( ! ( m_BtnUp.Init ( rect.left, rect.top, "Button173" ) ) )
		return false ;

	if ( ! ( m_Ima1.Init ( rect.left, rect.top ) ) )
		return false ;
	if ( ! ( m_Ima2.Init ( rect.left, rect.top ) ) )
		return false ;
	if ( ! ( m_Ima3.Init ( rect.left, rect.top ) ) )
		return false ;
	if ( ! ( m_Ima4.Init ( rect.left, rect.top ) ) )
		return false ;
	if ( ! ( m_Ima5.Init ( rect.left, rect.top ) ) )
		return false ;
	if ( ! ( m_Ima6.Init ( rect.left, rect.top ) ) )
		return false ;
	if ( ! ( m_Ima11.Init ( rect.left, rect.top, NULL, CONTROL_IMAGE ) ) )
		return false ;
	if ( ! ( m_Ima22.Init ( rect.left, rect.top, NULL, CONTROL_IMAGE ) ) )
		return false ;
	if ( ! ( m_Ima33.Init ( rect.left, rect.top, NULL, CONTROL_IMAGE ) ) )
		return false ;
	if ( ! ( m_Ima44.Init ( rect.left, rect.top, NULL, CONTROL_IMAGE ) ) )
		return false ;
	if ( ! ( m_Ima55.Init ( rect.left, rect.top, NULL, CONTROL_IMAGE ) ) )
		return false ;
	if ( ! ( m_Ima66.Init ( rect.left, rect.top, NULL, CONTROL_IMAGE ) ) )
		return false ;

	return TRUE;
}

void CDlgQuery_Weapon::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_DlgPnt.x = x ;
	m_DlgPnt.y = y ;
}

void CDlgQuery_Weapon::Show(int xPos, int yPos)
{
	IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
												  "Dialog9",
												  EXIGENCE_IMMEDIATE ) ;
	if ( ShowAni == NULL )
		return ;
	ShowAni->Show ( 0, m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;

	if ( m_bShowInfo && m_pCurSkill != NULL )
	{
		char strText[64] ;
		char strTemp[16] ;
		strcpy ( strText, m_pCurSkill->GetName () ) ;
		if ( m_pCurSkill->GetLevel () != 0 )
			sprintf ( strTemp, GameDataSetQuery()->GetStr ( 10018 ), m_pCurSkill->GetLevel () ) ;
		else
			strcpy ( strTemp, GameDataSetQuery()->GetStr ( 10115 ) ) ;
		strcat ( strText, strTemp ) ;
		int nPosY = 5 ;
		CMyBitmap::ShowString ( m_DlgPnt.x + xPos + 30, m_DlgPnt.y + yPos + nPosY, 0xFFFFFF, strText ) ;
		nPosY += 18 ;

		if ( m_pCurSkill->GetLevel () == 20 )
			CMyBitmap::ShowString ( m_DlgPnt.x + xPos + 30, m_DlgPnt.y + yPos + nPosY, 0xFFFFFF, GameDataSetQuery()->GetStr ( 10020 ) ) ;
		else
		{
			float fExp = m_pCurSkill->GetExp () * 100.0f ;
			sprintf ( strText, GameDataSetQuery()->GetStr ( 10019 ), fExp / __max ( 1, m_pCurSkill->GetNextLevelExpRequired () ) ) ;
			CMyBitmap::ShowString ( m_DlgPnt.x + xPos + 30, m_DlgPnt.y + yPos + nPosY, 0xFFFFFF, strText ) ;
		}
		nPosY += 18 ;

		CMyBitmap::ShowString ( m_DlgPnt.x + xPos + 30,
								m_DlgPnt.y + yPos + nPosY,
								0xFFFFFF,
								CMyShellApp::StringChange ( m_pCurSkill->GetDescribe (), 1 ) ) ;
	}

	m_BtnS.Show ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_BtnD.Show ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_BtnO.Show ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_BtnUp.Show ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_BtnDown.Show ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;

	m_Ima1.Show ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_Ima2.Show ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_Ima3.Show ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_Ima4.Show ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_Ima5.Show ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_Ima6.Show ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_Ima11.ShowWeaponLevel ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_Ima22.ShowWeaponLevel ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_Ima33.ShowWeaponLevel ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_Ima44.ShowWeaponLevel ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_Ima55.ShowWeaponLevel ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_Ima66.ShowWeaponLevel ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
}

LRESULT CDlgQuery_Weapon::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_COMMAND )
	{
		if ( wParam == 1 || wParam == 2 )
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

void CDlgQuery_Weapon::OnWeaponBtnS()
{
	m_nWeaponClass = 0 ;
	m_bShowInfo = false ;
	m_nTopIndex = 0 ;
	ShowSkill () ;
}

void CDlgQuery_Weapon::OnWeaponBtnD()
{
	m_nWeaponClass = 1 ;
	m_bShowInfo = false ;
	m_nTopIndex = 0 ;
	ShowSkill () ;
}

void CDlgQuery_Weapon::OnWeaponBtnO()
{
	m_nWeaponClass = 2 ;
	m_bShowInfo = false ;
	m_nTopIndex = 0 ;
	ShowSkill () ;
}

void CDlgQuery_Weapon::ShowSkill()
{
	int nAmount = g_objHero.GetWeaponSkillAmount ( m_nWeaponClass ) ;
	if ( nAmount > m_nTopIndex + 6 )
		nAmount = m_nTopIndex + 6 ;
	ClearSkill () ;
	for ( int i = m_nTopIndex; i < nAmount; i++ )
	{
		CWeaponSkill* pSkill = g_objHero.GetWeaponSkill ( m_nWeaponClass, i ) ;
		if ( pSkill != NULL )
			SetSkill ( i - m_nTopIndex, pSkill->GetType (), pSkill->GetLevel () ) ;
	}
}

void CDlgQuery_Weapon::SetSkill(int nIndex, int nType, DWORD dwLevel)
{
	char strImage[32] ;
	sprintf ( strImage, "Weapon%d", nType ) ;
	switch ( nIndex )
	{
	case 0:
		m_Ima1.InsertImage ( strImage ) ;
		m_Ima11.SetWeaponLevel ( dwLevel ) ;
		break ;
	case 1:
		m_Ima2.InsertImage ( strImage ) ;
		m_Ima22.SetWeaponLevel ( dwLevel ) ;
		break ;
	case 2:
		m_Ima3.InsertImage ( strImage ) ;
		m_Ima33.SetWeaponLevel ( dwLevel ) ;
		break ;
	case 3:
		m_Ima4.InsertImage ( strImage ) ;
		m_Ima44.SetWeaponLevel ( dwLevel ) ;
		break ;
	case 4:
		m_Ima5.InsertImage ( strImage ) ;
		m_Ima55.SetWeaponLevel ( dwLevel ) ;
		break ;
	case 5:
		m_Ima6.InsertImage ( strImage ) ;
		m_Ima66.SetWeaponLevel ( dwLevel ) ;
		break ;
	}
}

void CDlgQuery_Weapon::ClearSkill()
{
	m_Ima1.RemoveImage () ;
	m_Ima2.RemoveImage () ;
	m_Ima3.RemoveImage () ;
	m_Ima4.RemoveImage () ;
	m_Ima5.RemoveImage () ;
	m_Ima6.RemoveImage () ;
	m_Ima11.SetWeaponLevel ( 0 ) ;
	m_Ima22.SetWeaponLevel ( 0 ) ;
	m_Ima33.SetWeaponLevel ( 0 ) ;
	m_Ima44.SetWeaponLevel ( 0 ) ;
	m_Ima55.SetWeaponLevel ( 0 ) ;
	m_Ima66.SetWeaponLevel ( 0 ) ;
}

void CDlgQuery_Weapon::OnWeaponIma1()
{
	if ( m_Ima1.GetImageLoadState () )
	{
		m_bShowInfo = true ;
		m_pCurSkill = g_objHero.GetWeaponSkill ( m_nWeaponClass, m_nTopIndex ) ;
	}
	else
		m_bShowInfo = false ;
}

void CDlgQuery_Weapon::OnWeaponIma11()
{
	CDlgQuery_Weapon::OnWeaponIma1 () ;
}

void CDlgQuery_Weapon::OnWeaponIma2()
{
	if ( m_Ima2.GetImageLoadState () )
	{
		m_bShowInfo = true ;
		m_pCurSkill = g_objHero.GetWeaponSkill ( m_nWeaponClass, m_nTopIndex + 1 ) ;
	}
	else
		m_bShowInfo = false ;
}

void CDlgQuery_Weapon::OnWeaponIma22()
{
	CDlgQuery_Weapon::OnWeaponIma2 () ;
}

void CDlgQuery_Weapon::OnWeaponIma3()
{
	if ( m_Ima3.GetImageLoadState () )
	{
		m_bShowInfo = true ;
		m_pCurSkill = g_objHero.GetWeaponSkill ( m_nWeaponClass, m_nTopIndex + 2 ) ;
	}
	else
		m_bShowInfo = false ;
}

void CDlgQuery_Weapon::OnWeaponIma33()
{
	CDlgQuery_Weapon::OnWeaponIma3 () ;
}

void CDlgQuery_Weapon::OnWeaponIma4()
{
	if ( m_Ima4.GetImageLoadState () )
	{
		m_bShowInfo = true ;
		m_pCurSkill = g_objHero.GetWeaponSkill ( m_nWeaponClass, m_nTopIndex + 3 ) ;
	}
	else
		m_bShowInfo = false ;
}

void CDlgQuery_Weapon::OnWeaponIma44()
{
	CDlgQuery_Weapon::OnWeaponIma4 () ;
}

void CDlgQuery_Weapon::OnWeaponIma5()
{
	if ( m_Ima5.GetImageLoadState () )
	{
		m_bShowInfo = true ;
		m_pCurSkill = g_objHero.GetWeaponSkill ( m_nWeaponClass, m_nTopIndex + 4 ) ;
	}
	else
		m_bShowInfo = false ;
}

void CDlgQuery_Weapon::OnWeaponIma55()
{
	CDlgQuery_Weapon::OnWeaponIma5 () ;
}

void CDlgQuery_Weapon::OnWeaponIma6()
{
	if ( m_Ima6.GetImageLoadState () )
	{
		m_bShowInfo = true ;
		m_pCurSkill = g_objHero.GetWeaponSkill ( m_nWeaponClass, m_nTopIndex + 5 ) ;
	}
	else
		m_bShowInfo = false ;
}

void CDlgQuery_Weapon::OnWeaponIma66()
{
	CDlgQuery_Weapon::OnWeaponIma6 () ;
}

void CDlgQuery_Weapon::OnWeaponBtnUp()
{
	if ( m_nTopIndex >= 6 )
	{
		m_nTopIndex -= 6 ;
		ShowSkill () ;
	}
}

void CDlgQuery_Weapon::OnWeaponBtnDown()
{
	if ( g_objHero.GetWeaponSkillAmount ( m_nWeaponClass ) > m_nTopIndex + 6 )
	{
		m_nTopIndex += 6 ;
		ShowSkill () ;
	}
}

void CDlgQuery_Weapon::Reset()
{
	m_nWeaponClass = 0 ;
	m_bShowInfo = false ;
	m_nTopIndex = 0 ;
	m_pCurSkill = NULL ;
	ClearSkill () ;
}
