#include "stdafx.h"
#include "MyShell.h"
#include "DlgQuery_Role.h"

#include "Hero.h"
#include "Item.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgQuery_Role::CDlgQuery_Role(CWnd* pParent )
	: CDialog(CDlgQuery_Role::IDD, pParent)
{
	m_bInitSuc = false ;
	m_Hbr = CreateSolidBrush ( 0x2132B ) ;
	m_DlgPnt = CPoint ( 0, 0 ) ;
	m_nShowType = 0 ;
}


void CDlgQuery_Role::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ROLE_BTN_MORE, m_BtnMore);
	DDX_Control(pDX, IDC_ROLE_BTN_BASE, m_BtnBase);
	DDX_Control(pDX, IDC_ROLE_STA_1, m_StaName);
	DDX_Control(pDX, IDC_ROLE_STA_2, m_StaLev);
	DDX_Control(pDX, IDC_ROLE_STA_3, m_StaFamor);
	DDX_Control(pDX, IDC_ROLE_STA_4, m_StaPk);
	DDX_Control(pDX, IDC_ROLE_STA_5, m_StaFere);
	DDX_Control(pDX, IDC_ROLE_STA_6, m_StaSyn);
	DDX_Control(pDX, IDC_ROLE_STA_7, m_StaTitle);
	DDX_Control(pDX, IDC_ROLE_STA_8, m_StaStrong);
	DDX_Control(pDX, IDC_ROLE_STA_9, m_StaSmart);
	DDX_Control(pDX, IDC_ROLE_STA_10, m_StaHa);
	DDX_Control(pDX, IDC_ROLE_STA_11, m_StaEnergy);
	DDX_Control(pDX, IDC_ROLE_STA_12, m_StaPotential);
	DDX_Control(pDX, IDC_ROLE_STA_14, m_StaAtk);
	DDX_Control(pDX, IDC_ROLE_STA_15, m_StaDef);
	DDX_Control(pDX, IDC_ROLE_STA_16, m_StaMagicAtk);
	DDX_Control(pDX, IDC_ROLE_STA_17, m_StaMagicDef);
	DDX_Control(pDX, IDC_ROLE_STA_18, m_Sta18);
	DDX_Control(pDX, IDC_ROLE_STA_19, m_Sta19);
	DDX_Control(pDX, IDC_ROLE_BTN8, m_BtnAddStrong);
	DDX_Control(pDX, IDC_ROLE_BTN9, m_BtnAddSmart);
	DDX_Control(pDX, IDC_ROLE_BTN10, m_BtnAddHa);
	DDX_Control(pDX, IDC_ROLE_BTN11, m_BtnAddEnergy);
}


BEGIN_MESSAGE_MAP(CDlgQuery_Role, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_ROLE_BTN8, OnRoleBtn8)
	ON_BN_CLICKED(IDC_ROLE_BTN9, OnRoleBtn9)
	ON_BN_CLICKED(IDC_ROLE_BTN10, OnRoleBtn10)
	ON_BN_CLICKED(IDC_ROLE_BTN11, OnRoleBtn11)
	ON_BN_CLICKED(IDC_ROLE_BTN_BASE, OnRoleBtnBase)
	ON_BN_CLICKED(IDC_ROLE_BTN_MORE, OnRoleBtnMore)
END_MESSAGE_MAP()

HBRUSH CDlgQuery_Role::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgQuery_Role::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgQuery_Role::GetWindowRect ( rect ) ;

	m_StaName.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_StaLev.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_StaFamor.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_StaPk.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_StaFere.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_StaSyn.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_StaTitle.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_StaStrong.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_StaSmart.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_StaHa.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_StaEnergy.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_StaPotential.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_StaAtk.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_StaDef.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_StaMagicAtk.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_StaMagicDef.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_Sta18.Init(rect.left,rect.top, 0, NULL, "NULL");
	m_Sta19.Init(rect.left,rect.top, 0, NULL, "NULL");

	if ( ! ( m_BtnAddStrong.Init ( rect.left, rect.top, "Button70" ) ) )
		return false ;
	if ( ! ( m_BtnAddSmart.Init ( rect.left, rect.top, "Button70" ) ) )
		return false ;
	if ( ! ( m_BtnAddHa.Init ( rect.left, rect.top, "Button70" ) ) )
		return false ;
	if ( ! ( m_BtnAddEnergy.Init ( rect.left, rect.top, "Button70" ) ) )
		return false ;
	if ( ! ( m_BtnBase.Init ( rect.left, rect.top, "Button71" ) ) )
		return false ;
	if ( ! ( m_BtnMore.Init ( rect.left, rect.top, "Button72" ) ) )
		return false ;

	m_bInitSuc = true ;
	return TRUE;
}

void CDlgQuery_Role::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
	m_DlgPnt.x = x ;
	m_DlgPnt.y = y ;
}

void CDlgQuery_Role::Show( int xPos, int yPos )
{
	if ( m_bInitSuc )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog7",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni == NULL )
			return ;
		ShowAni->Show ( 0, m_DlgPnt.x + xPos, m_DlgPnt.y + yPos ) ;

		m_StaName.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
		m_StaLev.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
		m_StaFamor.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
		m_StaPk.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
		m_StaFere.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
		m_StaSyn.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
		m_StaTitle.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);

		if ( m_nShowType == 0 )
		{
			m_StaStrong.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
			m_StaSmart.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
			m_StaHa.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
			m_StaEnergy.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
			m_StaPotential.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);

			m_BtnAddStrong.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
			m_BtnAddSmart.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
			m_BtnAddHa.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
			m_BtnAddEnergy.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
		}
		else
		{
			ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													"Dialog31",
													EXIGENCE_IMMEDIATE ) ;
			if ( ShowAni == NULL )
				return ;
			ShowAni->Show ( 0, m_DlgPnt.x + xPos + 8, m_DlgPnt.y + yPos + 194 ) ;

			m_StaAtk.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
			m_StaDef.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
			m_StaMagicAtk.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
			m_StaMagicDef.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
		}

		m_BtnBase.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
		m_BtnMore.Show(m_DlgPnt.x+xPos,m_DlgPnt.y+yPos);
	}
}

LRESULT CDlgQuery_Role::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		if ( wParam )
			CDlgQuery_Role::SetRoleInfo () ;
	}
	else if ( message == WM_COMMAND )
	{
		if ( wParam == 1 || wParam == 2)
			return true ;
	}
	else if ( message == WM_DESTROY )
	{
		DeleteObject ( m_Hbr ) ;
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

void CDlgQuery_Role::SetRoleInfo ()
{
	char strText[64] ;

	m_StaName.SetWindowText ( g_objHero.GetName () ) ;

	itoa( g_objHero.GetLev (), strText, 10 ) ;
	m_StaLev.SetWindowText ( strText ) ;

	strcpy ( strText, g_objHero.GetProfessionalName(g_objHero.GetProfession()) ) ;
	m_StaFamor.SetWindowText( strText ) ;

	int nPk = g_objHero.GetPk() ;
	if ( nPk > 19 && nPk < 100 )
		m_StaPk.SetFontColor ( 0xFF0000 ) ;
	else if ( nPk > 99 )
		m_StaPk.SetFontColor ( 0 ) ;
	else if ( nPk < 0 )
		m_StaPk.SetFontColor ( 0xFFFF00 ) ;
	if ( g_objHero.GetPkTitle () != NULL )
		sprintf ( strText, "%s%d", g_objHero.GetPkTitle (), nPk ) ;
	else
		itoa( nPk, strText, 10);
	m_StaPk.SetWindowText ( strText) ;

	strcpy ( strText, g_objHero.GetMate () ) ;
	if ( strText[0] == 0 )
		strcpy ( strText, GameDataSetQuery()->GetStr ( 10075 ) ) ;
	m_StaFere.SetWindowText ( strText ) ;

	strcpy ( strText, g_objHero.GetSyndicate () ) ;
	if ( strText[0] == 0 )
		strcpy ( strText, GameDataSetQuery()->GetStr ( 10075 ) ) ;
	m_StaSyn.SetWindowText ( strText ) ;

	m_StaTitle.SetWindowText ( g_objHero.GetSyndicateTitle () ) ;

	itoa( g_objHero.GetForce(),strText,10);
	m_StaStrong.SetWindowText(strText);

	itoa( g_objHero.GetSpeed(), strText, 10);
	m_StaSmart.SetWindowText(strText);

	itoa( g_objHero.GetHealth(),strText,10);
	m_StaHa.SetWindowText( strText);

	itoa( g_objHero.GetSoul(),strText,10);
	m_StaEnergy.SetWindowText(strText);

	itoa(g_objHero.GetAddPoint(),strText,10);
	m_StaPotential.SetWindowText(strText);

	int nMinAtk = g_objHero.GetMinAtk () ;
	int nMaxAtk = g_objHero.GetMaxAtk () ;
	int nDef = g_objHero.GetDef () ;
	int nMagicAtk = 0 ;
	int nMagicDef = 0 ;
	int nAtkPercent = 0 ;
	int nMagicPercent = 0 ;
	for ( int i = 1 ; i < _MAX_EQUIPMENT ; i++ )
	{
		if ( i == ITEMPOSITION_TREASURE )
			continue ;
		CItem* pItem = g_objHero.GetEquipment ( i ) ;
		if ( pItem )
		{
			nMinAtk += pItem->GetAdditionAttackMax () ;
			nMaxAtk += pItem->GetAdditionAttackMax () ;
			nDef += pItem->GetAdditionDefense () ;
			nMagicAtk += pItem->GetAdditionMAttack () ;
			nMagicDef += pItem->GetAdditionMDef () ;
			nMagicAtk += pItem->GetMaxMagicAttack () ;
			nMagicDef += pItem->GetMagicDefence () ;

			unsigned char ucGem = pItem->GetGem1 () ;
			int nIndex = ( ucGem / 10 ) * 3 + ucGem % 10 - 1 ;
			if ( ucGem != 0 && ucGem != 0xFF )
			{
				nAtkPercent += CMyShellApp::m_GemInfo[nIndex].btAtk ;
				nMagicPercent += CMyShellApp::m_GemInfo[nIndex].btMagicAtk ;
			}
			ucGem = pItem->GetGem2 () ;
			nIndex = ( ucGem / 10 ) * 3 + ucGem % 10 - 1 ;
			if ( ucGem != 0 && ucGem != 0xFF )
			{
				nAtkPercent += CMyShellApp::m_GemInfo[nIndex].btAtk ;
				nMagicPercent += CMyShellApp::m_GemInfo[nIndex].btMagicAtk ;
			}
		}
	}
	sprintf ( strText, "%d-%d", nMinAtk + nMinAtk * nAtkPercent / 100, nMaxAtk + nMaxAtk * nAtkPercent / 100 ) ;
	m_StaAtk.SetWindowText ( strText ) ;
	itoa ( nDef, strText, 10 ) ;
	m_StaDef.SetWindowText ( strText ) ;
	itoa ( nMagicAtk + nMagicAtk * nMagicPercent / 100, strText, 10 ) ;
	m_StaMagicAtk.SetWindowText ( strText ) ;
	itoa ( nMagicDef, strText, 10 ) ;
	m_StaMagicDef.SetWindowText ( strText ) ;
}

void CDlgQuery_Role::OnRoleBtn8()
{
	char strText[16] ;
	DXPlaySound ( "Sound/AddPoint.wav" ) ;
	m_StaPotential.GetWindowText ( strText, 16 ) ;
	int nPotential = atoi ( strText ) ;
	if ( nPotential > 0 )
	{
		itoa ( nPotential - 1, strText, 10 ) ;
		m_StaPotential.SetWindowText ( strText ) ;
		m_StaStrong.GetWindowText ( strText, 16 ) ;
		itoa ( atoi ( strText ) + 1, strText, 10 ) ;
		m_StaStrong.SetWindowText ( strText ) ;
		CDlgQuery_Role::AllotPoint () ;
	}
}

void CDlgQuery_Role::OnRoleBtn9()
{
	char strText[16] ;
	DXPlaySound ( "Sound/AddPoint.wav" ) ;
	m_StaPotential.GetWindowText ( strText, 16 ) ;
	int nPotential = atoi ( strText ) ;
	if ( nPotential > 0 )
	{
		itoa ( nPotential - 1, strText, 10 ) ;
		m_StaPotential.SetWindowText ( strText ) ;
		m_StaSmart.GetWindowText ( strText, 16 ) ;
		itoa ( atoi ( strText ) + 1, strText, 10 ) ;
		m_StaSmart.SetWindowText ( strText ) ;
		CDlgQuery_Role::AllotPoint () ;
	}
}

void CDlgQuery_Role::OnRoleBtn10()
{
	char strText[16] ;
	DXPlaySound ( "Sound/AddPoint.wav" ) ;
	m_StaPotential.GetWindowText ( strText, 16 ) ;
	int nPotential = atoi ( strText ) ;
	if ( nPotential > 0 )
	{
		itoa ( nPotential - 1, strText, 10 ) ;
		m_StaPotential.SetWindowText ( strText ) ;
		m_StaHa.GetWindowText ( strText, 16 ) ;
		itoa ( atoi ( strText ) + 1, strText, 10 ) ;
		m_StaHa.SetWindowText ( strText ) ;
		CDlgQuery_Role::AllotPoint () ;
	}
}

void CDlgQuery_Role::OnRoleBtn11()
{
	char strText[16] ;
	DXPlaySound ( "Sound/AddPoint.wav" ) ;
	m_StaPotential.GetWindowText ( strText, 16 ) ;
	int nPotential = atoi ( strText ) ;
	if ( nPotential > 0 )
	{
		itoa ( nPotential - 1, strText, 10 ) ;
		m_StaPotential.SetWindowText ( strText ) ;
		m_StaEnergy.GetWindowText ( strText, 16 ) ;
		itoa ( atoi ( strText ) + 1, strText, 10 ) ;
		m_StaEnergy.SetWindowText ( strText ) ;
		CDlgQuery_Role::AllotPoint () ;
	}
}

void CDlgQuery_Role::OnRoleBtnBase()
{
	m_nShowType = 0 ;
	m_BtnAddStrong.EnableWindow ( true ) ;
	m_BtnAddSmart.EnableWindow ( true ) ;
	m_BtnAddHa.EnableWindow ( true ) ;
	m_BtnAddEnergy.EnableWindow ( true ) ;
}

void CDlgQuery_Role::OnRoleBtnMore()
{
	m_nShowType = 1 ;
	m_BtnAddStrong.EnableWindow ( false ) ;
	m_BtnAddSmart.EnableWindow ( false ) ;
	m_BtnAddHa.EnableWindow ( false ) ;
	m_BtnAddEnergy.EnableWindow ( false ) ;
}

void CDlgQuery_Role::AllotPoint ()
{
	char strText[16] ;
	m_StaStrong.GetWindowText ( strText, 16 ) ;
	int nForce = atoi ( strText ) ;
	m_StaSmart.GetWindowText ( strText, 16 ) ;
	int nSpeed = atoi ( strText ) ;
	m_StaHa.GetWindowText ( strText, 16 ) ;
	int nHealth = atoi ( strText ) ;
	m_StaEnergy.GetWindowText ( strText, 16 ) ;
	int nSoul = atoi ( strText ) ;
	g_objHero.AllotPoint ( nForce, nSpeed, nHealth, nSoul ) ;
}
