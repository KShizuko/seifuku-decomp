#include "stdafx.h"
#include "MyShell.h"
#include "DlgQuery_Skill.h"

#include "Hero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgQuery_Skill::CDlgQuery_Skill(CWnd* pParent )
	: CDialog(CDlgQuery_Skill::IDD, pParent)
{
	m_Hbr = CreateSolidBrush ( 0x2132B ) ;
	m_DlgPnt.x = 0 ;
	m_DlgPnt.y = 0 ;
	m_nSkillClass = 0 ;
	m_bShowInfo = false ;
	m_nTopIndex = 0 ;
	m_pCurSkill = NULL ;
}


void CDlgQuery_Skill::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SKILL_BTN_Z, m_BtnZ);
	DDX_Control(pDX, IDC_SKILL_BTN_M, m_BtnM);
	DDX_Control(pDX, IDC_SKILL_BTN_J, m_BtnJ);
	DDX_Control(pDX, IDC_SKILL_BTN_UP, m_BtnUp);
	DDX_Control(pDX, IDC_SKILL_BTN_DOWN, m_BtnDown);
	DDX_Control(pDX, IDC_SKILL_IMA1, m_Ima1);
	DDX_Control(pDX, IDC_SKILL_IMA2, m_Ima2);
	DDX_Control(pDX, IDC_SKILL_IMA3, m_Ima3);
	DDX_Control(pDX, IDC_SKILL_IMA4, m_Ima4);
	DDX_Control(pDX, IDC_SKILL_IMA5, m_Ima5);
	DDX_Control(pDX, IDC_SKILL_IMA6, m_Ima6);
	DDX_Control(pDX, IDC_SKILL_IMA11, m_Ima11);
	DDX_Control(pDX, IDC_SKILL_IMA22, m_Ima22);
	DDX_Control(pDX, IDC_SKILL_IMA33, m_Ima33);
	DDX_Control(pDX, IDC_SKILL_IMA44, m_Ima44);
	DDX_Control(pDX, IDC_SKILL_IMA55, m_Ima55);
	DDX_Control(pDX, IDC_SKILL_IMA66, m_Ima66);
}


BEGIN_MESSAGE_MAP(CDlgQuery_Skill, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_SKILL_BTN_Z, OnSkillBtnZ)
	ON_BN_CLICKED(IDC_SKILL_BTN_M, OnSkillBtnM)
	ON_BN_CLICKED(IDC_SKILL_BTN_J, OnSkillBtnJ)
	ON_BN_CLICKED(IDC_SKILL_IMA1, OnSkillIma1)
	ON_BN_CLICKED(IDC_SKILL_IMA11, OnSkillIma11)
	ON_BN_CLICKED(IDC_SKILL_IMA2, OnSkillIma2)
	ON_BN_CLICKED(IDC_SKILL_IMA22, OnSkillIma22)
	ON_BN_CLICKED(IDC_SKILL_IMA3, OnSkillIma3)
	ON_BN_CLICKED(IDC_SKILL_IMA33, OnSkillIma33)
	ON_BN_CLICKED(IDC_SKILL_IMA4, OnSkillIma4)
	ON_BN_CLICKED(IDC_SKILL_IMA44, OnSkillIma44)
	ON_BN_CLICKED(IDC_SKILL_IMA5, OnSkillIma5)
	ON_BN_CLICKED(IDC_SKILL_IMA55, OnSkillIma55)
	ON_BN_CLICKED(IDC_SKILL_IMA6, OnSkillIma6)
	ON_BN_CLICKED(IDC_SKILL_IMA66, OnSkillIma66)
	ON_BN_CLICKED(IDC_SKILL_BTN_UP, OnSkillBtnUp)
	ON_BN_CLICKED(IDC_SKILL_BTN_DOWN, OnSkillBtnDown)
END_MESSAGE_MAP()

HBRUSH CDlgQuery_Skill::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgQuery_Skill::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgQuery_Skill::GetWindowRect ( rect ) ;

	if ( ! ( m_BtnZ.Init ( rect.left, rect.top, "Button200" ) ) )
		return false ;
	if ( ! ( m_BtnM.Init ( rect.left, rect.top, "Button201" ) ) )
		return false ;
	if ( ! ( m_BtnJ.Init ( rect.left, rect.top, "Button202" ) ) )
		return false ;
	if ( ! ( m_BtnUp.Init ( rect.left, rect.top, "Button173" ) ) )
		return false ;
	if ( ! ( m_BtnDown.Init ( rect.left, rect.top, "Button174" ) ) )
		return false ;

	if ( ! ( m_Ima1.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ) )
		return false ;
	if ( ! ( m_Ima2.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ) )
		return false ;
	if ( ! ( m_Ima3.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ) )
		return false ;
	if ( ! ( m_Ima4.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ) )
		return false ;
	if ( ! ( m_Ima5.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ) )
		return false ;
	if ( ! ( m_Ima6.Init ( rect.left, rect.top, NULL, SKILL_IMAGE ) ) )
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

void CDlgQuery_Skill::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_DlgPnt.x = x ;
	m_DlgPnt.y = y ;
}

void CDlgQuery_Skill::Show(int xPos, int yPos)
{
	IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
												  "Dialog9",
												  EXIGENCE_IMMEDIATE ) ;
	if ( ShowAni == NULL )
		return ;
	ShowAni->Show ( 0, m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;

	if ( m_bShowInfo )
	{
		char strText[64] ;
		char strTemp[16] ;
		int nPosY = 5 ;
		strcpy ( strText, m_pCurSkill->GetName () ) ;
		if ( m_pCurSkill->GetNextLevelExp () != 0 )
		{
			if ( m_pCurSkill->GetLevel () != 0 )
				sprintf ( strTemp, GameDataSetQuery()->GetStr ( 10018 ), m_pCurSkill->GetLevel () ) ;
			else
				strcpy ( strTemp, GameDataSetQuery()->GetStr ( 10115 ) ) ;
			strcat ( strText, strTemp ) ;
		}
		CMyBitmap::ShowString ( m_DlgPnt.x + xPos + 30, m_DlgPnt.y + yPos + nPosY, 0xFFFFFF, strText ) ;
		nPosY += 18 ;

		if ( m_pCurSkill->GetNextLevelExp () != 0 )
		{
			if ( m_pCurSkill->GetLevel () == 12 )
				CMyBitmap::ShowString ( m_DlgPnt.x + xPos + 30, m_DlgPnt.y + yPos + nPosY, 0xFFFFFF, GameDataSetQuery()->GetStr ( 10020 ) ) ;
			else
			{
				float fExp = m_pCurSkill->GetExp () * 100.0f ;
				sprintf ( strText, GameDataSetQuery()->GetStr ( 10019 ), fExp / m_pCurSkill->GetNextLevelExp () ) ;
				CMyBitmap::ShowString ( m_DlgPnt.x + xPos + 30, m_DlgPnt.y + yPos + nPosY, 0xFFFFFF, strText ) ;
			}
			nPosY += 18 ;
		}

		CMyBitmap::ShowString ( m_DlgPnt.x + xPos + 30,
								m_DlgPnt.y + yPos + nPosY,
								0xFFFFFF,
								CMyShellApp::StringChange ( m_pCurSkill->GetDescribe (), 1 ) ) ;
	}

	m_BtnZ.Show ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_BtnM.Show ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
	m_BtnJ.Show ( m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;
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

LRESULT CDlgQuery_Skill::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

void CDlgQuery_Skill::OnSkillBtnZ()
{
	m_nSkillClass = 0 ;
	m_nTopIndex = 0 ;
	ShowSkill () ;
}

void CDlgQuery_Skill::OnSkillBtnM()
{
	m_nSkillClass = 1 ;
	m_nTopIndex = 0 ;
	ShowSkill () ;
}

void CDlgQuery_Skill::OnSkillBtnJ()
{
	m_nSkillClass = 2 ;
	m_nTopIndex = 0 ;
	ShowSkill () ;
}

void CDlgQuery_Skill::ShowSkill()
{
	int nAmount ;
	int i ;
	CMagic* pMagic ;
	ClearSkill () ;
	m_bShowInfo = false ;
	switch ( m_nSkillClass )
	{
	case 0:
		nAmount = g_objHero.GetKongfuAmount () ;
		if ( nAmount > m_nTopIndex + 6 )
			nAmount = m_nTopIndex + 6 ;
		for ( i = m_nTopIndex; i < nAmount; i++ )
		{
			pMagic = g_objHero.GetKongfuByIndex ( i ) ;
			if ( pMagic != NULL )
				SetSkill ( i - m_nTopIndex, pMagic->GetIDType (), pMagic->GetLevel () ) ;
		}
		break ;
	case 1:
		nAmount = g_objHero.GetMagicAmount () ;
		if ( nAmount > m_nTopIndex + 6 )
			nAmount = m_nTopIndex + 6 ;
		for ( i = m_nTopIndex; i < nAmount; i++ )
		{
			pMagic = g_objHero.GetMagicByIndex ( i ) ;
			if ( pMagic != NULL )
				SetSkill ( i - m_nTopIndex, pMagic->GetIDType (), pMagic->GetLevel () ) ;
		}
		break ;
	case 2:
		nAmount = g_objHero.GetXpSkillAmount () ;
		if ( nAmount > m_nTopIndex + 6 )
			nAmount = m_nTopIndex + 6 ;
		for ( i = m_nTopIndex; i < nAmount; i++ )
		{
			pMagic = g_objHero.GetXpSkillByIndex ( i ) ;
			if ( pMagic != NULL )
				SetSkill ( i - m_nTopIndex, pMagic->GetIDType (), pMagic->GetLevel () ) ;
		}
		break ;
	}
}

void CDlgQuery_Skill::SetSkill(int nIndex, int nType, DWORD dwLevel)
{
	char strImage[32] ;
	switch ( m_nSkillClass )
	{
	case 0:
	case 1:
		sprintf ( strImage, "MagicSkillType%d", nType ) ;
		break ;
	case 2:
		sprintf ( strImage, "XpSkillType%d", nType ) ;
		break ;
	}
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

void CDlgQuery_Skill::ClearSkill()
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

void CDlgQuery_Skill::OnSkillIma1()
{
	if ( m_Ima1.GetImageLoadState () )
	{
		m_bShowInfo = true ;
		switch ( m_nSkillClass )
		{
		case 0:
			m_pCurSkill = g_objHero.GetKongfuByIndex ( m_nTopIndex ) ;
			break ;
		case 1:
			m_pCurSkill = g_objHero.GetMagicByIndex ( m_nTopIndex ) ;
			break ;
		case 2:
			m_pCurSkill = g_objHero.GetXpSkillByIndex ( m_nTopIndex ) ;
			break ;
		}
	}
	else
		m_bShowInfo = false ;
}

void CDlgQuery_Skill::OnSkillIma11()
{
	CDlgQuery_Skill::OnSkillIma1 () ;
}

void CDlgQuery_Skill::OnSkillIma2()
{
	if ( m_Ima2.GetImageLoadState () )
	{
		m_bShowInfo = true ;
		switch ( m_nSkillClass )
		{
		case 0:
			m_pCurSkill = g_objHero.GetKongfuByIndex ( m_nTopIndex + 1 ) ;
			break ;
		case 1:
			m_pCurSkill = g_objHero.GetMagicByIndex ( m_nTopIndex + 1 ) ;
			break ;
		case 2:
			m_pCurSkill = g_objHero.GetXpSkillByIndex ( m_nTopIndex + 1 ) ;
			break ;
		}
	}
	else
		m_bShowInfo = false ;
}

void CDlgQuery_Skill::OnSkillIma22()
{
	CDlgQuery_Skill::OnSkillIma2 () ;
}

void CDlgQuery_Skill::OnSkillIma3()
{
	if ( m_Ima3.GetImageLoadState () )
	{
		m_bShowInfo = true ;
		switch ( m_nSkillClass )
		{
		case 0:
			m_pCurSkill = g_objHero.GetKongfuByIndex ( m_nTopIndex + 2 ) ;
			break ;
		case 1:
			m_pCurSkill = g_objHero.GetMagicByIndex ( m_nTopIndex + 2 ) ;
			break ;
		case 2:
			m_pCurSkill = g_objHero.GetXpSkillByIndex ( m_nTopIndex + 2 ) ;
			break ;
		}
	}
	else
		m_bShowInfo = false ;
}

void CDlgQuery_Skill::OnSkillIma33()
{
	CDlgQuery_Skill::OnSkillIma3 () ;
}

void CDlgQuery_Skill::OnSkillIma4()
{
	if ( m_Ima4.GetImageLoadState () )
	{
		m_bShowInfo = true ;
		switch ( m_nSkillClass )
		{
		case 0:
			m_pCurSkill = g_objHero.GetKongfuByIndex ( m_nTopIndex + 3 ) ;
			break ;
		case 1:
			m_pCurSkill = g_objHero.GetMagicByIndex ( m_nTopIndex + 3 ) ;
			break ;
		case 2:
			m_pCurSkill = g_objHero.GetXpSkillByIndex ( m_nTopIndex + 3 ) ;
			break ;
		}
	}
	else
		m_bShowInfo = false ;
}

void CDlgQuery_Skill::OnSkillIma44()
{
	CDlgQuery_Skill::OnSkillIma4 () ;
}

void CDlgQuery_Skill::OnSkillIma5()
{
	if ( m_Ima5.GetImageLoadState () )
	{
		m_bShowInfo = true ;
		switch ( m_nSkillClass )
		{
		case 0:
			m_pCurSkill = g_objHero.GetKongfuByIndex ( m_nTopIndex + 4 ) ;
			break ;
		case 1:
			m_pCurSkill = g_objHero.GetMagicByIndex ( m_nTopIndex + 4 ) ;
			break ;
		case 2:
			m_pCurSkill = g_objHero.GetXpSkillByIndex ( m_nTopIndex + 4 ) ;
			break ;
		}
	}
	else
		m_bShowInfo = false ;
}

void CDlgQuery_Skill::OnSkillIma55()
{
	CDlgQuery_Skill::OnSkillIma5 () ;
}

void CDlgQuery_Skill::OnSkillIma6()
{
	if ( m_Ima6.GetImageLoadState () )
	{
		m_bShowInfo = true ;
		switch ( m_nSkillClass )
		{
		case 0:
			m_pCurSkill = g_objHero.GetKongfuByIndex ( m_nTopIndex + 5 ) ;
			break ;
		case 1:
			m_pCurSkill = g_objHero.GetMagicByIndex ( m_nTopIndex + 5 ) ;
			break ;
		case 2:
			m_pCurSkill = g_objHero.GetXpSkillByIndex ( m_nTopIndex + 5 ) ;
			break ;
		}
	}
	else
		m_bShowInfo = false ;
}

void CDlgQuery_Skill::OnSkillIma66()
{
	CDlgQuery_Skill::OnSkillIma6 () ;
}

void CDlgQuery_Skill::OnSkillBtnUp()
{
	if ( m_nTopIndex >= 6 )
	{
		m_nTopIndex -= 6 ;
		ShowSkill () ;
	}
}

void CDlgQuery_Skill::OnSkillBtnDown()
{
	int nAmount = 0 ;
	switch ( m_nSkillClass )
	{
	case 0:
		nAmount = g_objHero.GetKongfuAmount () ;
		break ;
	case 1:
		nAmount = g_objHero.GetMagicAmount () ;
		break ;
	case 2:
		nAmount = g_objHero.GetXpSkillAmount () ;
		break ;
	}
	if ( nAmount > m_nTopIndex + 6 )
	{
		m_nTopIndex += 6 ;
		ShowSkill () ;
	}
}

void CDlgQuery_Skill::Reset()
{
	m_nSkillClass = 0 ;
	m_bShowInfo = false ;
	m_nTopIndex = 0 ;
	m_pCurSkill = NULL ;
	ClearSkill () ;
}

void CDlgQuery_Skill::FlashSkillExp()
{
}
