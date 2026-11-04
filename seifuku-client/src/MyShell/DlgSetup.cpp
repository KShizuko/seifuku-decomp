#include "stdafx.h"
#include "MyShell.h"
#include "DlgSetup.h"

#include "Hero.h"
#include "NetMsg.h"
#include "Role.h"
#include "DlgDelRole.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef _SOUND_DISABLE
	#define DXPlaySound
	#define DXSetSoundVolume
	#define DXSetMusicVolume
#endif

CDlgSetup::CDlgSetup(CWnd* pParent )
	: CDialog(CDlgSetup::IDD, pParent)
{
	m_bShow = false ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_nScreenMode = 0 ;
	m_nInterface = 0 ;
	m_bInitSuc = false ;
	m_CtlHbr = CreateSolidBrush ( 0x2132B ) ;
}


void CDlgSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SETUP_BTN_RIGHT, m_SetupBtnRight);
	DDX_Control(pDX, IDC_SETUP_BTN_MODE, m_SetupBtnMode);
	DDX_Control(pDX, IDC_SETUP_BTN_LEFT, m_SetupBtnLeft);
	DDX_Control(pDX, IDC_SETUP_CHK_NAMEOPEN, m_SetupChkNameopen);
	DDX_Control(pDX, IDC_SETUP_CHK_KOSHOW, m_SetupChkKoShow);
	DDX_Control(pDX, IDC_SETUP_BTN_OTHER, m_SetupBtnOther);
	DDX_Control(pDX, IDC_SETUP_BTN_CLOSES, m_SetupBtnCloses);
	DDX_Control(pDX, IDC_SETUP_BTN_HOTKEY, m_SetupBtnHotkey);
	DDX_Control(pDX, IDC_SETUP_BTN_WINB, m_SetupBtnWinb);
	DDX_Control(pDX, IDC_SETUP_BTN_FULLB, m_SetupBtnFullb);
	DDX_Control(pDX, IDC_SETUP_SLI_MUSIC, m_SetupSliMusic);
	DDX_Control(pDX, IDC_SETUP_SLI_EFFECT, m_SetupSliEffect);
	DDX_Control(pDX, IDC_SETUP_BTN_WIN, m_SetupBtnWin);
	DDX_Control(pDX, IDC_SETUP_BTN_FULL, m_SetupBtnFull);
	DDX_Control(pDX, IDC_SETUP_BTN_EXIT, m_SetupBtnExit);
	DDX_Control(pDX, IDC_SETUP_BTN_DELETE, m_SetupBtnDelete);
	DDX_Control(pDX, IDC_SETUP_BTN_RETURN, m_SetupBtnReturn);
	DDX_Control(pDX, IDC_SETUP_BTN_REAL, m_SetupBtnReal);
	DDX_Control(pDX, IDC_SETUP_BTN_SIMPLE, m_SetupBtnSimple);
	DDX_Control(pDX, IDC_SETUP_BTN_SHADOWCLOSE, m_SetupBtnShadowclose);
	DDX_Control(pDX, IDC_BTN_HELP, m_SetupBtnHelp);
	DDX_Control(pDX, IDC_SETUP_PIC1, m_SetupPic1);
	DDX_Control(pDX, IDC_SETUP_PIC2, m_SetupPic2);
}


BEGIN_MESSAGE_MAP(CDlgSetup, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_SETUP_BTN_RETURN, OnSetupBtnReturn)
	ON_BN_CLICKED(IDC_SETUP_BTN_EXIT, OnSetupBtnExit)
	ON_BN_CLICKED(IDC_SETUP_BTN_WIN, OnSetupBtnWin)
	ON_BN_CLICKED(IDC_SETUP_BTN_FULL, OnSetupBtnFull)
	ON_BN_CLICKED(IDC_SETUP_BTN_DELETE, OnSetupBtnDelete)
	ON_BN_CLICKED(IDC_SETUP_BTN_REAL, OnSetupBtnReal)
	ON_BN_CLICKED(IDC_SETUP_BTN_SIMPLE, OnSetupBtnSimple)
	ON_BN_CLICKED(IDC_SETUP_BTN_SHADOWCLOSE, OnSetupBtnShadowclose)
	ON_BN_CLICKED(IDC_SETUP_BTN_WINB, OnSetupBtnWinb)
	ON_BN_CLICKED(IDC_SETUP_BTN_FULLB, OnSetupBtnFullb)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_SETUP_BTN_HOTKEY, OnSetupBtnHotkey)
	ON_BN_CLICKED(IDC_SETUP_BTN_CLOSES, OnSetupBtnCloses)
	ON_BN_CLICKED(IDC_SETUP_BTN_OTHER, OnSetupBtnOther)
	ON_BN_CLICKED(IDC_SETUP_CHK_NAMEOPEN, OnSetupChkNameopen)
	ON_BN_CLICKED(IDC_SETUP_BTN_MODE, OnSetupBtnMode)
	ON_BN_CLICKED(IDC_SETUP_BTN_LEFT, OnSetupBtnLeft)
	ON_BN_CLICKED(IDC_SETUP_BTN_RIGHT, OnSetupBtnRight)
	ON_BN_CLICKED(IDC_SETUP_CHK_KOSHOW, OnSetupChkKoshow)
END_MESSAGE_MAP()

LRESULT CDlgSetup::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		if ( wParam == 1 )
		{
			CDlgSetup::OnSetupBtnReturn () ;
			return true ;
		}
		else if ( wParam == 2 )
		{
			CDlgSetup::OnSetupBtnExit () ;
			return true ;
		}
	}
	else if ( message == WM_HSCROLL )
	{
		DXSetMusicVolume ( m_SetupSliMusic.GetSliderValue ( ) ) ;
		DXSetSoundVolume ( m_SetupSliEffect.GetSliderValue ( ) ) ;
	}
	else if ( message == WM_DESTROY )
	{
		::DeleteObject ( m_Hbr ) ;
		::DeleteObject ( m_CtlHbr ) ;
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

HBRUSH CDlgSetup::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	if ( nCtlColor == CTLCOLOR_DLG )
		return m_Hbr ;
	return m_CtlHbr ;
}

BOOL CDlgSetup::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	if ( GetPrivateProfileInt ( "Dialog4", "FrameAmount", -1, "ani/Control2.Ani" ) == -1
		|| strcmp ( GameDataSetQuery()->GetVersionInfo (), "English" ) == 0
		|| strcmp ( GameDataSetQuery()->GetVersionInfo (), "Japanese" ) == 0 )
	{
		m_SetupBtnMode.EnableWindow ( false ) ;
		m_SetupBtnLeft.EnableWindow ( false ) ;
		m_SetupBtnRight.EnableWindow ( false ) ;
	}

	CRect rect ;
	CDlgSetup::GetWindowRect ( rect ) ;

	HBITMAP hBmp = ( HBITMAP ) ::LoadImage ( AfxGetInstanceHandle (),
											 "Data/Main/SetupBk.bmp",
											 IMAGE_BITMAP,
											 0,
											 0,
											 LR_LOADFROMFILE ) ;
	m_Hbr = CreatePatternBrush ( hBmp ) ;

	if ( ! ( m_SetupSliMusic.Init ( rect.left, rect.top, "Slider0" ) ) )
		return false ;
	m_SetupSliMusic.SetSliderWidth ( 80 ) ;
	m_SetupSliMusic.SetMaxValue ( 100 ) ;
	if ( ! ( m_SetupSliEffect.Init ( rect.left, rect.top, "Slider0" ) ) )
		return false ;
	m_SetupSliEffect.SetSliderWidth ( 80 ) ;
	m_SetupSliEffect.SetMaxValue ( 100 ) ;

	m_SetupChkNameopen.Init ( rect.left, rect.top, "Check100", 2 ) ;
	m_SetupChkKoShow.Init ( rect.left, rect.top, "Check101", 2 ) ;

	if ( ! ( m_SetupBtnWin.Init ( rect.left, rect.top, "Button100" ) ) )
		return false ;
	if ( ! ( m_SetupBtnFull.Init ( rect.left, rect.top, "Button101" ) ) )
		return false ;
	if ( ! ( m_SetupBtnWinb.Init ( rect.left, rect.top, "Button100" ) ) )
		return false ;
	if ( ! ( m_SetupBtnFullb.Init ( rect.left, rect.top, "Button101" ) ) )
		return false ;
	if ( ! ( m_SetupBtnExit.Init ( rect.left, rect.top, "Button103" ) ) )
		return false ;
	if ( ! ( m_SetupBtnDelete.Init ( rect.left, rect.top, "Button102" ) ) )
		return false ;
	if ( ! ( m_SetupBtnReturn.Init ( rect.left, rect.top, "Button3" ) ) )
		return false ;
	if ( ! ( m_SetupBtnCloses.Init ( rect.left, rect.top, "Button5" ) ) )
		return false ;
	if ( ! ( m_SetupBtnReal.Init ( rect.left, rect.top, "Button104" ) ) )
		return false ;
	if ( ! ( m_SetupBtnSimple.Init ( rect.left, rect.top, "Button105" ) ) )
		return false ;
	if ( ! ( m_SetupBtnHotkey.Init ( rect.left, rect.top, "Button107" ) ) )
		return false ;
	if ( ! ( m_SetupBtnOther.Init ( rect.left, rect.top, "Button108" ) ) )
		return false ;
	if ( ! ( m_SetupBtnHelp.Init ( rect.left, rect.top, "Button4" ) ) )
		return false ;

	if ( ! ( m_SetupPic1.Init ( rect.left, rect.top, 1, "Picture100", "NULL" ) ) )
		return false ;
	m_SetupPic1.InsertImage () ;
	if ( ! ( m_SetupPic2.Init ( rect.left, rect.top, 1, "Picture100", "NULL" ) ) )
		return false ;

	m_bInitSuc = true ;
	return TRUE;
}

void CDlgSetup::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgSetup::Show()
{
	if ( m_bInitSuc && m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog10",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0, m_Pnt.x, m_Pnt.y + 2 ) ;
		else
			return ;

		m_SetupSliMusic.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_SetupSliEffect.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_SetupChkNameopen.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_SetupChkKoShow.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_SetupBtnWin.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_SetupBtnFull.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_SetupBtnWinb.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_SetupBtnFullb.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_SetupBtnExit.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_SetupBtnDelete.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_SetupBtnReturn.Show ( m_Pnt.x, m_Pnt.y );
		m_SetupBtnCloses.Show ( m_Pnt.x, m_Pnt.y );
		m_SetupBtnReal.Show ( m_Pnt.x, m_Pnt.y );
		m_SetupBtnSimple.Show ( m_Pnt.x, m_Pnt.y );
		m_SetupBtnHotkey.Show ( m_Pnt.x , m_Pnt.y ) ;
		m_SetupBtnOther.Show ( m_Pnt.x , m_Pnt.y ) ;
		m_SetupBtnHelp.Show ( m_Pnt.x , m_Pnt.y ) ;

		m_SetupPic1.Show ( m_Pnt.x , m_Pnt.y ) ;
		m_SetupPic2.Show ( m_Pnt.x , m_Pnt.y ) ;
	}
}

void CDlgSetup::OnSetupBtnReturn()
{
	DXSetMusicVolume ( m_SetupSliMusic.GetSliderValue ( ) ) ;
	DXSetSoundVolume ( m_SetupSliEffect.GetSliderValue ( ) ) ;
	CDlgSetup::ShowWindow ( SW_HIDE ) ;
}

void CDlgSetup::OnSetupBtnExit()
{
	CDlgSetup::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EXIT ) ;
}

void CDlgSetup::OnSetupBtnWin()
{
	m_SetupPic1.InsertImage () ;
	m_SetupPic2.RemoveImage () ;
	CDlgSetup::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_SETUP_MODECHANGE, 0 ) ;
}

void CDlgSetup::OnSetupBtnFull()
{
	m_SetupPic1.InsertImage () ;
	m_SetupPic2.RemoveImage () ;
	CDlgSetup::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_SETUP_MODECHANGE, 1 ) ;
}

void CDlgSetup::OnSetupBtnDelete()
{
	if ( IDOK == CDlgSetup::MessageBox ( CMyShellApp::StringChange ( GameDataSetQuery()->GetStr ( 10042 ) ), GameDataSetQuery()->GetStr ( 10040 ) , MB_OKCANCEL|MB_ICONWARNING ) )
	{
		CDlgDelRole dlg ;
		if ( dlg.DoModal () == IDOK )
		{
			char szFullFilePath[128];
			sprintf(szFullFilePath, "Log/%s/%s/Setup.ini", g_szServerName, g_objHero.GetName());
			g_objHero.DeleteRole ( atoi ( dlg.m_strCode ) ) ;
			::DeleteFile(szFullFilePath);
		}
		else
			return ;
	}
	CDlgSetup::OnSetupBtnReturn ( ) ;
}

void CDlgSetup::OnSetupBtnReal()
{
	CRole::s_nShadowType = CRole::SHADOW_REAL ;
}

void CDlgSetup::OnSetupBtnSimple()
{
	CRole::s_nShadowType = CRole::SHADOW_SIMPLE ;
}

void CDlgSetup::OnSetupBtnShadowclose()
{
	CRole::s_nShadowType = CRole::SHADOW_NONE ;
}

void CDlgSetup::OnSetupBtnWinb()
{
	m_SetupPic2.InsertImage () ;
	m_SetupPic1.RemoveImage () ;
	CDlgSetup::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_SETUP_MODECHANGE, 2 ) ;
}

void CDlgSetup::OnSetupBtnFullb()
{
	m_SetupPic2.InsertImage () ;
	m_SetupPic1.RemoveImage () ;
	CDlgSetup::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_SETUP_MODECHANGE, 3 ) ;
}

void CDlgSetup::OnBtnHelp()
{
	CDlgSetup::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_SETUP ) ;
}

void CDlgSetup::OnSetupBtnHotkey()
{
	CDlgSetup::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_SETUP_HOTKEY, 0 ) ;
}

void CDlgSetup::OnSetupBtnCloses()
{
	CDlgSetup::OnSetupBtnReturn () ;
}

void CDlgSetup::OnSetupBtnOther()
{
	CDlgSetup::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_SETUP_OTHER, 0 ) ;
}

void CDlgSetup::OnSetupChkNameopen()
{
	CDlgSetup::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_SETUP_SHIFT, m_SetupChkNameopen.GetCheckValue () ) ;
}

void CDlgSetup::OnSetupChkKoshow()
{
	CDlgSetup::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_SETUP_KOSHOW, m_SetupChkKoShow.GetCheckValue () ) ;
}

void CDlgSetup::OnSetupBtnMode()
{
	char strControl[32] ;
	char strMagic[32] ;
	if ( m_nInterface != 0 )
	{
		sprintf ( strControl, "ani/Control%d.Ani", m_nInterface + 1 ) ;
		sprintf ( strMagic, "ani/Magic%d.Ani", m_nInterface ) ;
	}
	else
	{
		strcpy ( strControl, "ani/Control.Ani" ) ;
		strcpy ( strMagic, "ani/Magic.Ani" ) ;
	}
	strcpy ( g_strControlAni, strControl ) ;
	strcpy ( g_strMagicAni, strMagic ) ;
	CDlgSetup::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_INTERFACE_CHANGE, m_nInterface ) ;
}

void CDlgSetup::OnSetupBtnLeft()
{
	char strButton[32] ;
	m_nInterface-- ;
	if ( m_nInterface < 0 )
		m_nInterface = MAX_INTERFACE_NUM - 1 ;
	sprintf ( strButton, "Button101%d", m_nInterface + 1 ) ;
	m_SetupBtnMode.ChangeImage ( strButton ) ;
}

void CDlgSetup::OnSetupBtnRight()
{
	char strButton[32] ;
	m_nInterface++ ;
	if ( m_nInterface > MAX_INTERFACE_NUM - 1 )
		m_nInterface = 0 ;
	sprintf ( strButton, "Button101%d", m_nInterface + 1 ) ;
	m_SetupBtnMode.ChangeImage ( strButton ) ;
}

void CDlgSetup::SetScreenMode(BYTE btMode)
{
	if ( btMode == 1 )
	{
		m_SetupPic2.InsertImage () ;
		m_SetupPic1.RemoveImage () ;
	}
}

void CDlgSetup::ResetInterface()
{
	m_nInterface = 0 ;
	CDlgSetup::OnSetupBtnMode () ;
	m_SetupBtnMode.EnableWindow ( false ) ;
	m_SetupBtnLeft.EnableWindow ( false ) ;
	m_SetupBtnRight.EnableWindow ( false ) ;
}
