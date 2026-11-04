#include "stdafx.h"
#include "MyShell.h"
#include "DlgRoleCreate.h"

#include "AllMsg.h"
#include "Hero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef _SOUND_DISABLE
	#define DXPlaySound
#endif

CDlgRoleCreate::CDlgRoleCreate(CWnd* pParent )
	: CDialog(CDlgRoleCreate::IDD, pParent)
{
	m_bShow = false ;
	m_bInitSuc = false ;
	m_Hbr = CreateSolidBrush ( 0x2132B ) ;
	m_EdtHbr = CreateSolidBrush ( 0x000000 ) ;
	m_b1024 = false ;
	m_nRoleSelIndex = 1 ;
}


void CDlgRoleCreate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ROLECHOOSE_MIMG2, m_Img2);
	DDX_Control(pDX, IDC_ROLECHOOSE_MIMG1, m_Img1);
	DDX_Control(pDX, IDC_ROLECREATE_BTN_EXIT, m_RoleCreateExitBtn);
	DDX_Control(pDX, IDC_ROLECREATE_STA_DATA, m_RoleCreateDataSta ) ;
	DDX_Control(pDX, IDC_ROLECREATE_EDT_NAME, m_RoleCreateNameEdt ) ;
	DDX_Control(pDX, IDC_ROLECREATE_BTN_OK,m_RoleCreateOkBtn );
	DDX_Control(pDX, IDC_ROLECREATE_BTN_RETURN, m_RoleCreateReturnBtn );
	DDX_Control(pDX, IDC_BTN_HELP1, m_HelpBtn );
}


BEGIN_MESSAGE_MAP(CDlgRoleCreate, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_ROLECREATE_BTN_RETURN, OnRolecreateBtnReturn)
	ON_BN_CLICKED(IDC_ROLECREATE_BTN_OK, OnRolecreateBtnOk)
	ON_BN_CLICKED(IDC_BTN_HELP1, OnBtnHelp)
	ON_BN_CLICKED(IDC_ROLECREATE_BTN_EXIT, OnRolecreateBtnExit)
	ON_BN_CLICKED(IDC_ROLECHOOSE_MIMG1, OnRolechooseMimg1)
	ON_BN_CLICKED(IDC_ROLECHOOSE_MIMG2, OnRolechooseMimg2)
END_MESSAGE_MAP()

LRESULT CDlgRoleCreate::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
		if ( m_bShow )
		{
			CRect rectIntro, rectParent ;
			m_DlgProIntro.GetWindowRect ( rectIntro ) ;
			CDlgRoleCreate::GetParent ()->GetWindowRect ( rectParent ) ;
			m_DlgProIntro.MoveWindow ( ( rectParent.Width () - rectIntro.Width () ) / 2,
									   ( rectParent.Height () - rectIntro.Height () ) / 2,
									   rectIntro.Width (),
									   rectIntro.Height (),
									   true ) ;
			m_DlgProIntro.ShowWindow ( SW_SHOW ) ;

			int nPro = m_btRoleType ;
			if ( m_btRoleType > 3 )
				nPro = m_btRoleType - 4 ;
			switch ( nPro )
			{
			case 0:
				m_DlgProIntro.m_Sta1.SetWindowText ( GameDataSetQuery()->GetStr ( 10385 ) ) ;
				m_DlgProIntro.m_Sta2.SetWindowText ( GameDataSetQuery()->GetStr ( 10386 ) ) ;
				m_DlgProIntro.m_Sta3.SetWindowText ( GameDataSetQuery()->GetStr ( 10401 ) ) ;
				break ;
			case 1:
				m_DlgProIntro.m_Sta1.SetWindowText ( GameDataSetQuery()->GetStr ( 10383 ) ) ;
				m_DlgProIntro.m_Sta2.SetWindowText ( GameDataSetQuery()->GetStr ( 10384 ) ) ;
				break ;
			case 2:
				m_DlgProIntro.m_Sta1.SetWindowText ( GameDataSetQuery()->GetStr ( 10392 ) ) ;
				m_DlgProIntro.m_Sta2.SetWindowText ( GameDataSetQuery()->GetStr ( 10393 ) ) ;
				break ;
			case 3:
				m_DlgProIntro.m_Sta1.SetWindowText ( GameDataSetQuery()->GetStr ( 10387 ) ) ;
				m_DlgProIntro.m_Sta2.SetWindowText ( GameDataSetQuery()->GetStr ( 10388 ) ) ;
				m_DlgProIntro.m_Sta3.SetWindowText ( GameDataSetQuery()->GetStr ( 10389 ) ) ;
				break ;
			}
			m_RoleCreateNameEdt.SetFocus () ;
			m_ShowRoleA.Reset () ;
			m_ShowRoleB.Reset () ;
		}
		else
		{
			if ( m_DlgProIntro.IsWindowVisible () )
				m_DlgProIntro.ShowWindow ( SW_HIDE ) ;
			m_ShowRoleA.Reset () ;
			m_ShowRoleB.Reset () ;
		}
	}
	else if ( message == WM_COMMAND )
	{
		if ( wParam == 1 )
		{
			CDlgRoleCreate::OnRolecreateBtnOk () ;
			return true ;
		}
		else if ( wParam == 2 )
		{
			CDlgRoleCreate::OnRolecreateBtnReturn () ;
			return true ;
		}
	}
	else if ( message == WM_DESTROY )
	{
		::DeleteObject ( m_Hbr ) ;
		::DeleteObject ( m_EdtHbr ) ;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

HBRUSH CDlgRoleCreate::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	if ( nCtlColor == CTLCOLOR_EDIT )
		return m_EdtHbr ;
	return m_Hbr ;
}

BOOL CDlgRoleCreate::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	if ( ! ( m_RoleCreateDataSta.Init ( 0, 0, 0, NULL, "NULL" ) ) )
		return false ;
	m_RoleCreateDataSta.SetFontColor ( 0xffffff ) ;
	m_RoleCreateDataSta.SetWindowText ( GameDataSetQuery()->GetStr ( 10378 ) ) ;
	CRect rect ;
	CDlgRoleCreate::GetWindowRect ( rect ) ;

	if ( ! ( m_RoleCreateNameEdt.Init ( 0, 0, NULL, 0x000000, 0xffffff ) ) )
		return false ;
	m_RoleCreateNameEdt.SetLimitText ( 15 ) ;
	m_RoleCreateNameEdt.SetOffset( CPoint ( 4, 6 ) ) ;

	if ( ! ( m_RoleCreateOkBtn.Init( 0, 0, "Button20" ) ) )
		return false ;
	if ( ! ( m_RoleCreateReturnBtn.Init( 0, 0, "Button30" ) ) )
		return false ;
	if ( ! ( m_RoleCreateExitBtn.Init( 0, 0, "Button14" ) ) )
		return false ;
	if ( ! ( m_HelpBtn.Init( 0, 0, "Button15" ) ) )
		return false ;

	m_bInitSuc = true ;
	return TRUE;
}

void CDlgRoleCreate::Show()
{
	if ( m_bInitSuc && m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog3",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni == NULL )
			return ;

		CMyPos posA, posB ;
		int nSize ;
		if ( !m_b1024 )
		{
			ShowAni->Show ( 0, 0, 0 );
			posA.x = 155 ;
			posA.y = 344 ;
			posB.x = 645 ;
			posB.y = 344 ;
			nSize = 150 ;
		}
		else
		{
			ShowAni->Show ( 1, 0, 0 );
			posA.x = 215 ;
			posA.y = 430 ;
			posB.x = 807 ;
			posB.y = 430 ;
			nSize = 150 ;
		}
		m_ShowRoleA.ShowRole ( 2 * m_btRoleType + 100, posA, nSize, 0, m_nRoleSelIndex ) ;
		m_ShowRoleB.ShowRole ( 2 * m_btRoleType + 101, posB, nSize, 0, m_nRoleSelIndex == 0 ) ;

		m_RoleCreateDataSta.Show ( 0, 0 ) ;

		m_RoleCreateNameEdt.Show ( 0, 0 ) ;

		m_RoleCreateOkBtn.Show ( 0, 0 ) ;
		m_RoleCreateReturnBtn.Show ( 0, 0 ) ;
		m_RoleCreateExitBtn.Show ( 0, 0 ) ;
		m_HelpBtn.Show ( 0, 0 ) ;

		if ( m_DlgProIntro.m_bShow )
			m_DlgProIntro.Show () ;
	}
}

void CDlgRoleCreate::SetRoleType(BYTE btRoleType)
{
	m_btRoleType = btRoleType ;
}

void CDlgRoleCreate::OnRolecreateBtnReturn()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	CDlgRoleCreate::ShowWindow ( SW_HIDE ) ;
	CDlgRoleCreate::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ROLECREATE_RETURN ) ;
}

extern CHAR g_szAccount[256] ;

void CDlgRoleCreate::OnRolecreateBtnOk()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;

	if ( m_strAccount[0] == 0 )
	{
		CDlgRoleCreate::MessageBox ( GameDataSetQuery()->GetStr ( 10035 ), GameDataSetQuery()->GetStr ( 10017 ), MB_ICONHAND ) ;
		return;
	}
	if ( m_strPassword[0] == 0 )
	{
		CDlgRoleCreate::MessageBox ( GameDataSetQuery()->GetStr ( 10036 ), GameDataSetQuery()->GetStr ( 10017 ), MB_ICONHAND ) ;
		return;
	}

	m_RoleCreateNameEdt.GetWindowText ( m_strRoleName, 16 ) ;
	if ( m_strRoleName[0] == 0 || strlen ( m_strRoleName ) < 4 )
	{
		CDlgRoleCreate::MessageBox ( GameDataSetQuery()->GetStr ( 10037 ), GameDataSetQuery()->GetStr ( 10017 ), MB_ICONHAND ) ;
		return;
	}

	if ( strcmp ( g_szAccount, m_strRoleName ) == 0 )
	{
		if ( CDlgRoleCreate::MessageBox ( GameDataSetQuery()->GetStr ( 10475 ), GameDataSetQuery()->GetStr ( 10040 ), MB_YESNO ) == IDNO )
			return ;
	}

	if ( CDlgRoleCreate::MessageBox ( GameDataSetQuery()->GetStr ( 10038 ), GameDataSetQuery()->GetStr ( 10021 ), MB_YESNO ) != IDYES )
		return ;

	extern UINT g_uStatus ;
	g_uStatus	= _STATUS_REGISTER ;

	USHORT unLook ;
	if ( m_btRoleType > 3 )
		unLook = m_nRoleSelIndex ? 2001 : 2002 ;
	else
		unLook = m_nRoleSelIndex ? 1003 : 1004 ;

	USHORT unPro ;
	switch ( m_btRoleType % 4 )
	{
	case 0:
		unPro = 10 ;
		break;
	case 1:
		unPro = 20 ;
		break;
	case 2:
		unPro = 40 ;
		break;
	case 3:
		unPro = 100 ;
		break;
	}

	CMsgRegister msg;
	if ( msg.Create( m_strAccount,
					 m_strRoleName,
					 m_strPassword,
					 unLook,
					 unPro,
					 g_objHero.m_idAccount ) )
		msg.Send();

	CDlgRoleCreate::ShowWindow ( SW_HIDE ) ;
	m_RoleCreateNameEdt.SetWindowText ( NULL ) ;
}

void CDlgRoleCreate::SetAcnAndPsw(const char *strAccount, const char *strPassword)
{
	strcpy ( m_strAccount, strAccount ) ;
	strcpy ( m_strPassword, strPassword ) ;
}

void CDlgRoleCreate::ReInit()
{
	m_RoleCreateDataSta.Init ( 0, 0, 0, NULL, "NULL" ) ;
	m_RoleCreateNameEdt.ReInit ( 0, 0 ) ;
	m_RoleCreateOkBtn.Init( 0, 0, "Button20" ) ;
	m_RoleCreateReturnBtn.Init( 0, 0, "Button30" ) ;
	m_RoleCreateExitBtn.Init( 0, 0, "Button14" ) ;
	m_HelpBtn.Init( 0, 0, "Button15" ) ;
	m_b1024 = true ;
}

void CDlgRoleCreate::OnRolecreateBtnExit()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	CDlgRoleCreate::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EXIT ) ;
}

void CDlgRoleCreate::OnBtnHelp()
{
	CDlgRoleCreate::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_ROLECREATE ) ;
}

void CDlgRoleCreate::OnRolechooseMimg1()
{
	m_nRoleSelIndex = 1 ;
	m_ShowRoleA.Blaze () ;
}

void CDlgRoleCreate::OnRolechooseMimg2()
{
	m_nRoleSelIndex = 0 ;
	m_ShowRoleB.Blaze () ;
}
