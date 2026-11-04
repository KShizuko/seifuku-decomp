#include "stdafx.h"
#include "MyShell.h"
#include "MyShellDlg.h"

#include "BaseFunc.h"
#include "3DGameMap.h"
#include "Role.h"
#include "GamePlayerSet.h"
#include "Hero.h"
#include "GameMsg.h"
#include "GameDataSet.h"
#include "GameDataSetProvisional.h"
#include "NetMsg.h"
#include "Entrance.h"
#include "Register.h"
#include "Shop.h"
#include "3DRoleData.h"
#include "ND_Bitmap.h"
#include "../NDSound/NDsound.h"
#include <string>
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	REPLAY_OK_STR		"ANSWER_OK"
#define NEW_ROLE_STR		"NEW_ROLE"

extern HWND	g_hGameWnd;
extern UINT	g_uStatus;

void	GameScreenMsg	(const char* pszMsg);
BOOL	GameInit		(HWND hWnd);
BOOL	m_bAlt = false ;
BOOL	m_bShiftDown = false ;
DWORD	g_dwLastTime = 0 ;

static DWORD HashName(const char* pszName)
{
	return Str2ID(pszName);
}

bool TestDebuger(const char* pszFile)
{
	if (!pszFile)
		return false;

	HFILE hFile = _lopen(pszFile, OF_READ);
	if (HFILE_ERROR == hFile)
		return false;


	_lclose(hFile);
	return true;
}

void GameShow ( bool bDeviceLost )
{
	static DWORD	s_dwFrameRate = 0 ;
	static DWORD s_dwLastFrame = 0 ;

	DWORD dwElapse = 1 ;
	if ( TimeGet () - s_dwLastFrame >= 1 )
		dwElapse = TimeGet () - s_dwLastFrame ;
	s_dwFrameRate = ( s_dwFrameRate + 1000 / dwElapse ) / 2 ;
	s_dwLastFrame = TimeGet () ;

	CMyPos posAlign = g_objHero.GetAlignPos () ;
	g_objPlayerSet.Process ( posAlign.x, posAlign.y ) ;
	TimeGet () ;
	g_objGameMap.Process ( NULL ) ;
	TimeGet () ;

	CMyPos posView ;
	g_objHero.GetWorldPos ( posView ) ;
	posView.x -= CMyBitmap::GetScreenWidth () / 2 ;
	posView.y -= CMyBitmap::GetScreenHeight () / 2 ;
	BYTE btState[256] ;
	GetKeyboardState ( btState ) ;
	BOOL bAlt = ( btState[VK_MENU] & 0x80 ) == 0x80 ;
	g_objGameMap.SetViewPos ( posView, bAlt && m_bAlt, true ) ;

	MouseProcess () ;
	if ( !bDeviceLost )
	{
		TimeGet () ;
		try
		{
			g_objGameMap.Show ( m_bShiftDown || ( btState[VK_SHIFT] & 0x80 ) == 0x80 ) ;
		}
		catch ( ... )
		{
			LogMsg ( "game map show error." ) ;
		}
		g_objHero.ShowXp () ;
		g_objGameMsg.Show ( bAlt ) ;
		TimeGet () ;
	}

	CMyPos posHero ;
	g_objHero.GetPos ( posHero ) ;
	char szMsg[256] = "" ;
	if ( _stricmp ( GameDataSetQuery()->GetVersionInfo (), "english" ) == 0 )
	{
		char szWeek[7][16] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" } ;
		time_t tNow ;
		time ( &tNow ) ;
		tNow -= 28800 ;
		tm* pTime = gmtime ( &tNow ) ;
		sprintf ( szMsg, "[%s] (%03d,%03d) Ping: %04d; Fps: %02d; Time: %02d:%02d, %s", g_szServerName, posHero.x, posHero.y, g_objHero.GetPing (), s_dwFrameRate, pTime->tm_hour, pTime->tm_min, szWeek[pTime->tm_wday] ) ;
	}
	else
		sprintf ( szMsg, "[%s] (%03d,%03d) ping=%d, Fps:%d", g_szServerName, posHero.x, posHero.y, g_objHero.GetPing (), s_dwFrameRate ) ;
	if ( !bDeviceLost )
	{
		CMyBitmap::ShowString ( 6, 3, 0x7f7f7f, szMsg ) ;
		CMyBitmap::ShowString ( 5, 2, 0xffff00, szMsg ) ;
	}

	GameDataSetQuery()->Process () ;
	int nHeroX, nHeroY ;
	g_objHero.GetWorldPos ( nHeroX, nHeroY ) ;
	::DXProcessSound ( nHeroX, nHeroY ) ;
	g_objNetwork.ProcessNetMsg () ;
}

BOOL BeginShow ()
{
	int nLost = CMyBitmap::IfDeviceLost () ;
	if ( nLost == 1 )
	{
		g_objGameMap.ProcessPuzzle () ;
		return false ;
	}
	if ( nLost == 2 )
		CMyBitmap::ResetDevice () ;
	CMyBitmap::Begin3D () ;
	CMyBitmap::ClearBuffer ( true, false, 0xff000000 ) ;
	return true ;
}

void EndShow ()
{
	CMyBitmap::End3D ();
	CMyBitmap::Flip ( NULL, NULL, NULL );
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

class CMutexGuard
{
public:
	CMutexGuard( HANDLE hMutex, const char* pszName, int nAmount ) ;
	virtual ~CMutexGuard() ;

	HANDLE		m_hMutex ;
	int			m_nAmount ;
	std::string	m_strName ;
};

CMutexGuard::CMutexGuard( HANDLE hMutex, const char* pszName, int nAmount )
	: m_strName ( pszName )
{
	m_hMutex = hMutex ;
	m_nAmount = nAmount ;
}

CMutexGuard::~CMutexGuard()
{
	if ( m_hMutex != NULL )
		CloseHandle ( m_hMutex ) ;
	m_hMutex = NULL ;
}

static CMutexGuard* CreateMutexGuard( const char* pszName, int nAmount )
{
	if ( !pszName || nAmount <= 0 )
		return NULL ;

	HANDLE hMutex = NULL ;
	for ( int i = 0; i < nAmount; i++ )
	{
		char strMutex[256] = "" ;
		sprintf ( strMutex, "%s%02d", pszName, i ) ;
		hMutex = ::OpenMutex ( MUTEX_ALL_ACCESS, false, strMutex ) ;
		if ( hMutex != NULL )
		{
			CloseHandle ( hMutex ) ;
			hMutex = NULL ;
			continue ;
		}
		hMutex = ::CreateMutex ( NULL, false, strMutex ) ;
		if ( hMutex != NULL )
			break ;
	}
	if ( hMutex == NULL )
		return NULL ;

	CMutexGuard* pGuard = new CMutexGuard ( hMutex, pszName, nAmount ) ;
	if ( pGuard == NULL )
	{
		CloseHandle ( hMutex ) ;
		return NULL ;
	}
	return pGuard ;
}

CMyShellDlg::CMyShellDlg(CWnd* pParent )
	: CDialog(CMyShellDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_dwGameLoop	=0;
	m_uCmd			=CMD_NORMAL;

	m_hCursorH	= ::LoadCursor( NULL, IDC_HAND ) ;
	m_Hbr = ::CreateSolidBrush ( 0 ) ;

	m_bLogin = false ;
	m_bMsgDlg = false ;
	m_uMsgDlgType = 0 ;
	m_bPickUp = false ;
	m_bIconFix = false ;
	m_nScreenMode = 0 ;
	m_MagicID = 0 ;
	m_DisMagicID = 0 ;
	m_XpID = 0 ;
	m_uCurSkill = 0 ;
	m_bDlgMove = false ;
	m_bWait = false ;
	m_dwScale = 256 ;
	m_bExpShow = false ;
	m_bExit = false ;
	m_strOwnerIni[0] = 0 ;
	m_bInitSuc = false ;
	m_nMsgDlgWidth = 0 ;
	m_pMutexGuard = NULL ;
	m_idActiveNpc = 0 ;
	m_bCursorNpc = false ;
	m_bCursorPk = false ;
	m_bCursorPick = false ;
	m_bGameStarted = false ;
}

void CMyShellDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_SYSHIDE, m_BtnSysHide);
	DDX_Control(pDX, IDC_SHELL_BTN_MSGHIDE, m_BtnMsgHide);
	DDX_Control(pDX, IDC_SLI_LINE, m_SliLine);
}

BEGIN_MESSAGE_MAP(CMyShellDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_MY_MESSAGE, OnMyMessage)
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOVE()
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_BTN_SYSHIDE, OnBtnSyshide)
	ON_BN_CLICKED(IDC_SHELL_BTN_MSGHIDE, OnBtnMsghide)
END_MESSAGE_MAP()

BOOL CMyShellDlg::OnInitDialog()
{
	if ( strcmp ( GameDataSetQuery()->GetVersionInfo (), "English" ) == 0 )
	{
		m_pMutexGuard = CreateMutexGuard ( "TQ_CONQUER", 2 ) ;
		if ( m_pMutexGuard == NULL )
			CMyShellDlg::OnExit ( 1 ) ;
	}

	CDlgLogo dlglogo ;
	dlglogo.Create ( IDD_DIALOG_LOGO, this ) ;
	dlglogo.ShowWindow ( SW_SHOW ) ;

	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_nScreenMode = GetPrivateProfileInt ( "ScreenMode", "ScreenModeRecord", -1, g_strSetupIni ) ;

	if ( m_nScreenMode == 1 )
	{
		CMyShellDlg::SetScreenMode ( 1 ) ;
	}
	else if ( m_nScreenMode == 2 )
	{
		_SCR_WIDTH = 1024 ;
		_SCR_HEIGHT = 768 ;
		m_nScreenMode = 2 ;
		EnumDisplaySettings( NULL,
							 ENUM_CURRENT_SETTINGS,
							 &m_OldDeviceMode ) ;
		CMyShellDlg::SetScreenMode ( 2 ) ;
	}
	else if ( m_nScreenMode == 3 )
	{
		_SCR_WIDTH = 1024 ;
		_SCR_HEIGHT = 768 ;
		m_nScreenMode = 3 ;
		CMyShellDlg::SetScreenMode ( 3 ) ;
	}
	else
	{
		EnumDisplaySettings( NULL,
							 ENUM_CURRENT_SETTINGS,
							 &m_OldDeviceMode ) ;
		m_nScreenMode = 0 ;
		CMyShellDlg::SetScreenMode ( 0 ) ;
	}

	g_objFontScale.LoadIni () ;
	GameDataSetQuery()->Init ( FALSE ) ;
	RoleDataQuery()->Init();

	this->MoveWindow(0, 0, _SCR_WIDTH, _SCR_HEIGHT);
	CMyShellDlg::GetParent()->SetWindowText ( GameDataSetQuery()->GetStr ( 10375 ) ) ;

	if ( ! GameInit ( m_hWnd ) )
	{
		EndDialog(0) ;
		return false;
	}

	g_hGameWnd = this->GetSafeHwnd () ;

#ifndef _SOUND_DISABLE
	if ( 0 != DXSoundinit ( this->GetSafeHwnd ( ) ) )
	{
		this->MessageBox ( GameDataSetQuery()->GetStr ( 10048 ), "Error", MB_OK|MB_ICONERROR ) ;
	}
#endif

	m_DlgHelp.Create ( IDD_DIALOG_HELP, this ) ;

	m_DlgLog.Create ( IDD_DIALOG_LOG, this ) ;

	m_DlgTitleBar.Create ( IDD_DIALOG_TITLEBAR, this ) ;
	m_DlgTitleBar.m_pParentWnd = this ;
	int nMode = GetPrivateProfileInt ( "Interface", "InterfaceRecord", -1, g_strSetupIni ) ;
	if ( nMode != -1 )
	{
		if ( nMode != 0 )
			sprintf ( g_strControlAni, "ani/Control%d.Ani", nMode + 1 ) ;
		else
			strcpy ( g_strControlAni, "ani/Control.Ani" ) ;
		m_DlgTitleBar.ResetBk ( nMode ) ;
		m_DlgHelp.ResetBk ( nMode ) ;
		m_DlgLog.ResetBk ( nMode ) ;
	}
	else
	{
		m_DlgTitleBar.ResetBk ( 0 ) ;
		m_DlgHelp.ResetBk ( 0 ) ;
		m_DlgLog.ResetBk ( 0 ) ;
	}

	m_DlgRoleChoose.Create ( IDD_DIALOG_ROLECHOOSE, this ) ;
	m_DlgRoleCreate.m_DlgProIntro.Create ( IDD_DIALOG_PROINTRO, this ) ;
	m_DlgRoleCreate.Create ( IDD_DIALOG_ROLECREATE, this ) ;

	m_DlgLogin.Create ( IDD_DIALOG_LOGIN, this ) ;
	m_DlgLogin.ShowWindow ( SW_SHOW ) ;

	if ( m_nScreenMode == 2 || m_nScreenMode == 3 )
		CMyShellDlg::MoveControl () ;

	int nFontSize = atoi ( GameDataSetQuery()->GetStr ( 10461 ) ) ;
	if ( nFontSize < 12 || nFontSize > 18 )
		nFontSize = 14 ;
	HFONT font;
	font = ::CreateFont ( nFontSize,
						  0,
						  0,
						  0,
						  0,
						  0,
						  0,
						  0,
						  DEFAULT_CHARSET,
						  OUT_DEFAULT_PRECIS,
						  CLIP_DEFAULT_PRECIS,
						  ANTIALIASED_QUALITY,
						  VARIABLE_PITCH,
						  GameDataSetQuery()->GetStr ( 10460 ) ) ;

	m_pGameFont = CFont::FromHandle ( font ) ;

	m_DlgLogin.m_LoginAccountEdt.SetFont ( m_pGameFont ) ;
	m_DlgLogin.m_LoginPasswordEdt.SetFont ( m_pGameFont ) ;
	m_DlgRoleCreate.m_RoleCreateNameEdt.SetFont ( m_pGameFont ) ;
	m_DlgLog.GetDlgItem ( IDC_LOG_DATA )->SetFont ( m_pGameFont ) ;

	if ( strcmp ( GameDataSetQuery()->GetVersionInfo (), "English" ) == 0 )
	{
		m_DlgLogin.m_LoginAccountEdt.EnableChinese ( false ) ;
		m_DlgLogin.m_LoginPasswordEdt.EnableChinese ( false ) ;
		m_DlgRoleCreate.m_RoleCreateNameEdt.EnableChinese ( false ) ;
	}

	if ( strcmp ( GameDataSetQuery()->GetVersionInfo (), "English" ) != 0 )
	{
		TEXTMETRIC tm ;
		CDC* pDC = CDC::FromHandle ( ::GetDC ( m_hWnd ) ) ;
		::GetTextMetrics ( pDC->m_hDC, &tm ) ;
		if ( tm.tmDigitizedAspectX > 96 )
		{
			AfxMessageBox ( CMyShellApp::StringChange ( GameDataSetQuery()->GetStr ( 10407 ) ) ) ;
			CMyShellDlg::OnExit ( 1 ) ;
		}
	}

	if (TestDebuger("\\\\.\\SICE")
			|| TestDebuger("\\\\.\\SIWDEBUG")
			|| TestDebuger("\\\\.\\NTICE")
			|| TestDebuger("\\\\.\\SIWVID"))
	{
		EndDialog(0);
		return false;
	}

	if (IsDebuggerPresent())
	{
		EndDialog(0) ;
		return false;
	}

	dlglogo.ShowWindow ( SW_HIDE ) ;
	CMyShellDlg::GetParent()->CenterWindow() ;

	CRect rect ;
	CMyShellDlg::GetParent()->GetWindowRect ( rect ) ;
	m_DlgTitleBar.MoveWindow ( rect.left,
							   rect.top - 20,
							   CMyBitmap::GetScreenWidth (),
							   20 ) ;
	m_DlgTitleBar.ShowWindow (SW_SHOW ) ;

	m_bInitSuc = true ;

	return TRUE;
}

void CMyShellDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CMyShellDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CMyShellDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMyShellDlg::OnTimer(UINT_PTR nIDEvent)
{
	static DWORD dwTicks = 0;
	dwTicks ++;
	if ( g_uStatus == _STATUS_NORMAL )
	{
		if (GetParent()->IsIconic())
		{
			if (dwTicks%5 != 0)
				return;
		}
		else
		{
			HWND hWnd = ::GetForegroundWindow () ;
			if (hWnd != GetParent()->m_hWnd
					&& dwTicks%3 != 0)
				return;
		}
	}
	( ( CMyShellApp* ) AfxGetApp () )->SetOnButton ( 0 ) ;
	CMyShellDlg::CheckCursor () ;

	static volatile bool bInLoop = false;
	if (!bInLoop)
	{
		bInLoop = true;

		if (nIDEvent == 1)
		{
			if ( m_bDlgMove )
				CMyShellDlg::MoveDlg( ) ;

			switch ( g_uStatus )
			{
			case ( _STATUS_NONE ):
				{
					BeginShow () ;
					m_bLogin = false ;

					if ( m_DlgLogin.m_bShow )
						m_DlgLogin.Show () ;

					if ( m_DlgRoleChoose.m_bShow )
						m_DlgRoleChoose.Show () ;

					if ( m_DlgRoleCreate.m_bShow )
						m_DlgRoleCreate.Show() ;

					EndShow ( ) ;
				}
				break ;

			case ( _STATUS_EXIT ):
				{
					m_DlgMain.WriteRecord () ;
					const char* pszMsg	=g_objEntrance.GetLastMsg();
					if (pszMsg)
					{
						const char* pszText = pszMsg ;
						if ( strstr ( pszMsg, "服务器未启动" ) )
							pszText = GameDataSetQuery()->GetStr ( 10417 ) ;
						else if ( strstr ( pszMsg, "帐号名或口令错" ) )
							pszText = GameDataSetQuery()->GetStr ( 10418 ) ;
						else if ( strstr ( pszMsg, "请稍后重新登录" ) )
							pszText = GameDataSetQuery()->GetStr ( 10419 ) ;
						else if ( strstr ( pszMsg, "服务器人数已满" ) )
							pszText = GameDataSetQuery()->GetStr ( 10438 ) ;
						::MessageBox(NULL, pszText, GameDataSetQuery()->GetStr ( 10050 ), MB_OK|MB_ICONERROR);
						g_uStatus	=_STATUS_NONE;
						m_DlgLogin.ShowWindow ( SW_SHOW ) ;
					}
				}
				break ;
			case ( _STATUS_ACCOUNT ):
				{
					GameScreenMsg(GameDataSetQuery()->GetStr ( 10051 ) ) ;
					g_objNetwork.ProcessNetMsg();
				}
				break;
			case ( _STATUS_LOGIN ):
				{
					char szMsg[128];
					sprintf(szMsg, GameDataSetQuery()->GetStr ( 10052 ), g_szServerName);
 					GameScreenMsg(szMsg);

					g_objNetwork.ProcessNetMsg();

					const char* pszMsg	=g_objEntrance.GetLastMsg();
					if (pszMsg)
					{
						if (0 == stricmp(REPLAY_OK_STR, pszMsg ) )
						{
							CMyShellDlg::StartGame () ;
						}
						else if (0 == stricmp(NEW_ROLE_STR, pszMsg ) )
						{
							g_uStatus = _STATUS_NONE ;
							WPARAM wParam = ON_LOGIN_NEW ;
							this->PostMessage ( WM_MY_MESSAGE, wParam ) ;
						}
						else
						{
							::MessageBox(NULL, pszMsg, "Error", MB_OK|MB_ICONERROR);

							g_uStatus = _STATUS_NONE ;
							m_DlgLogin.ShowWindow ( SW_SHOW ) ;
						}
					}
				}
				break ;

			case ( _STATUS_REGISTER ):
				{
					GameScreenMsg(GameDataSetQuery()->GetStr ( 10054 ) ) ;
					g_objNetwork.ProcessNetMsg () ;

					const char* pszMsg	=g_objRegister.GetLastMsg();
					if ( pszMsg )
					{
						if ( 0 != stricmp(REPLAY_OK_STR, pszMsg ) )
						{
							g_uStatus = _STATUS_NONE ;
							::MessageBox(NULL, pszMsg, "Error", MB_OK|MB_ICONERROR);
							m_DlgRoleCreate.ShowWindow ( SW_SHOW ) ;
							m_DlgRoleCreate.SetFocus () ;
						}
						else
						{
							m_bWait = true ;
							g_dwLastTime = TimeGet() ;
						}
					}
					if ( !m_bWait )
						break ;
					if ( TimeGet () - g_dwLastTime < 10000 )
						break ;
					g_uStatus = _STATUS_NONE ;
					::MessageBox ( NULL, CMyShellApp::StringChange ( GameDataSetQuery()->GetStr ( 10055 ) ), GameDataSetQuery()->GetStr ( 10056 ), MB_OK ) ;
					m_DlgLogin.ShowWindow ( SW_SHOW ) ;
					m_bWait = false ;
				}
				break ;
			case ( _STATUS_WAITING ):
				{
					GameScreenMsg(GameDataSetQuery()->GetStr ( 10057 ) ) ;
					g_objNetwork.ProcessNetMsg();
				}
				break;

			case ( _STATUS_NORMAL ):
				{
					g_objGameMap.ProcessTerrain () ;
					bool bDeviceLost = !BeginShow () ;
					GameShow ( bDeviceLost ) ;
					if ( bDeviceLost )
					{
						EndShow ( ) ;
						break ;
					}

					m_SliLine.Show ( 0, 0 ) ;
					if ( !g_objGameMap.m_bShowMiniMap )
					{
						if ( m_BtnSysHide.IsWindowEnabled () )
							m_BtnSysHide.EnableWindow ( false ) ;
						if ( m_BtnMsgHide.IsWindowEnabled () )
							m_BtnMsgHide.EnableWindow ( false ) ;
					}
					else
					{
						if ( !m_BtnSysHide.IsWindowEnabled () )
							m_BtnSysHide.EnableWindow ( true ) ;
						if ( !m_BtnMsgHide.IsWindowEnabled () )
							m_BtnMsgHide.EnableWindow ( true ) ;
						m_BtnSysHide.Show ( 0, 0 ) ;
						m_BtnMsgHide.Show ( 0, 0 ) ;
					}

					m_DlgMain.Show();

					if ( m_bPickUp )
						CMyShellDlg::ShowPickUpIcon () ;
					else if ( !m_bPickUp && m_bMsgDlg )
						CMyShellDlg::ShowMsgDlg () ;

					if ( m_DlgConfirm.m_bShow )
						m_DlgConfirm.Show () ;

					if ( m_DlgTask.m_bShow )
						m_DlgTask.Show () ;

					EndShow ( ) ;
				}
				break ;

			default:
				 break ;
			}
		}

		bInLoop = false;
	}
}

void CMyShellDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	static int s_nCornerClick = 0 ;
	static int s_nNoActiveClick = 0 ;
	POINT ptCursor ;
	if ( GetCursorPos ( &ptCursor ) )
	{
		CMyShellDlg::ScreenToClient ( &ptCursor ) ;
		if ( point.x == ptCursor.x && point.y == ptCursor.y )
			s_nCornerClick = 0 ;
		else
		{
			if ( s_nCornerClick == 128 )
				g_objHero.RequestAction ( 10 ) ;
			s_nCornerClick++ ;
		}
	}
	if ( CWnd::GetActiveWindow () == NULL )
	{
		if ( s_nNoActiveClick == 128 )
			g_objHero.RequestAction ( 10 ) ;
		s_nNoActiveClick++ ;
	}
	else
		s_nNoActiveClick = 0 ;

	if ( strlen( g_objGameMsg.GetCaptureName () ) > 0 )
	{
		m_DlgMain.m_DlgTalk.m_TalkNameEdt.SetWindowText ( g_objGameMsg.GetCaptureName () ) ;
		return ;
	}

	if (g_objHero.IsDead() && !g_objHero.TestStatus(USERSTATUS_GHOST))
		return;

	if (_STATUS_WAITING == g_uStatus)
		return;

	CMyShellDlg::SetFocus () ;
	MouseSet(point.x, point.y, 1);

	if(g_objHero.TestStatus(USERSTATUS_GHOST))
	{
		int nPosX, nPosY;
		g_objGameMap.Mouse2Cell(point.x, point.y, nPosX, nPosY);
		g_objHero.Walk(nPosX, nPosY);
		return;
	}

	CPlayer* pTarget = g_objPlayerSet.GetFocusPlayer(PLAYER_MONSTERPRI);
	if (pTarget && (pTarget->GetRoleType() == _ROLE_MONSTER || pTarget->GetRoleType() == _ROLE_PLAYER || pTarget->GetRoleType() == _ROLE_HERO))
	{
		int nTargetX, nTargetY;
		pTarget->GetPos(nTargetX, nTargetY);

		switch(m_uCmd)
		{
		case CMD_NORMAL:
		case CMD_ATK:
			if (nFlags&MK_CONTROL)
			{
				g_objHero.LockAttack (pTarget, true);
			}
			else if ( nFlags&MK_SHIFT && !pTarget->IsNpc () )
			{
				m_DlgMain.m_DlgTalk.SetTalkTarget ( pTarget ) ;
				g_objHero.Direction ( nTargetX, nTargetY ) ;
			}
			else
			{
				OBJID idItem = ID_NONE;
				CMyPos ItemPos;
				if ( g_objHero.GetPkMode () != PKMODE_MONSTERONLY || pTarget->IsNpc () )
					g_objHero.LockAttack (pTarget) ;
				else if ( !m_bPickUp && (idItem = g_objGameMap.GetFocusMapItem (ItemPos)) != ID_NONE )
				{
					CMyShellDlg::PickMapItem ( idItem, ItemPos.x, ItemPos.y ) ;
					return ;
				}
			}
			break;
		case CMD_TRADE:
			if ( !pTarget->IsNpc () )
				g_objHero.ApplyTrade ( pTarget ) ;
			break ;
		case CMD_TEMJOIN:
			if ( !pTarget->IsNpc () )
				g_objHero.TeamApplyJion ( pTarget->GetID () ) ;
			break ;
		case CMD_TEMINVITE:
			if ( !pTarget->IsNpc () )
				g_objHero.TeamInvite ( pTarget->GetID () ) ;
			break ;
		case CMD_FACJOIN:
			if ( CMyShellDlg::MessageBox ( GameDataSetQuery()->GetStr ( 10342 ), GameDataSetQuery()->GetStr ( 10343 ), MB_YESNO ) == IDYES )
				g_objHero.SyndicateApplyJoin ( pTarget->GetID () ) ;
			break ;
		case CMD_FACINVITE:
			g_objHero.SyndicateApplyInvite ( pTarget->GetID () ) ;
			break ;
		case CMD_MPCCOURT:
			g_objHero.Court(pTarget);
			break;
		case CMD_DIVORCE:
			g_objHero.Divorce(pTarget);
			break;
		case CMD_FRDMAKE:
			g_objHero.MakeFriend(pTarget->GetID());
			break;
		case CMD_FRDBREAK:
			g_objHero.BreakOffFriend(pTarget->GetID());
			break;
		case CMD_NPCEQUIP:
			m_DlgMain.m_StandChk.SetCheckValue ( 0 ) ;
			g_objHero.m_objPlayerDummy.LockDummy ( pTarget->GetID () ) ;
			break ;
		}

		m_uCmd	=CMD_NORMAL;
	}
	else
	{
		if ( m_uCmd == CMD_LAY )
		{
			g_objHero.FixMousePlayer () ;
			m_uCmd = CMD_NORMAL ;
			return ;
		}

		int nPosX, nPosY;
		g_objGameMap.Mouse2Cell(point.x, point.y, nPosX, nPosY);

		if ( nFlags&MK_SHIFT )
		{
			g_objHero.Direction ( nPosX, nPosY ) ;
			return ;
		}
		else
		{
			OBJID idItem = ID_NONE;
			CMyPos posItem;

			CPlayer* pPlayer = g_objPlayerSet.GetFocusPlayer();
			if ( m_bPickUp )
			{
				if ( m_PickUpDlgID == DLG_MPCGOODS )
				{
					CMyShellDlg::DropMapItem ( point.x, point.y );
					return ;
				}
			}
			else if ( (idItem = g_objGameMap.GetFocusMapItem (posItem)) != ID_NONE )
			{
				CMyPos posHero ;
				g_objHero.GetPos ( posHero.x, posHero.y ) ;
				if ( nFlags&MK_CONTROL && !( posHero.x == posItem.x && posHero.y == posItem.y ) )
					g_objHero.Jump(nPosX, nPosY);
				else
					CMyShellDlg::PickMapItem ( idItem, posItem.x, posItem.y ) ;
				return ;
			}
			if ( !m_bPickUp && pPlayer)
			{
				if ( CMD_DELNPC == m_uCmd )
				{
					m_uCmd = CMD_NORMAL ;
					g_objHero.DelNpc ( pPlayer->GetID () ) ;
					return ;
				}

				switch(pPlayer->GetRoleType())
				{
				case _ROLE_SHOPKEEPER_NPC:
					if ( !m_DlgMain.m_DlgNpcBuy.m_bShow && !m_DlgMain.m_DlgDepot.m_bShow )
						m_DlgMain.OpenNpcShop ( 1, pPlayer->GetID () ) ;
					break;
				case _ROLE_TASK_NPC:
				case _ROLE_STORAGE_NPC:
				case _ROLE_TRUNCK_NPC:
					if (!m_DlgTask.m_bShow)
					{
						g_objHero.ActiveNpc(pPlayer->GetID());
						m_idActiveNpc = pPlayer->GetID () ;
					}
					break ;
				case _ROLE_FACE_NPC:
					if ( !m_DlgMain.m_DlgNpcFace.m_bShow )
						m_DlgMain.OpenNpcFace () ;
					break ;
				case _ROLE_FORGE_NPC:
					if ( !m_DlgMain.m_DlgImprove.m_bShow )
						m_DlgMain.OpenImprove () ;
					break  ;
				case _ROLE_EMBED_NPC:
					if ( !m_DlgMain.m_DlgEmbed.m_bShow )
						m_DlgMain.OpenEmbed () ;
					break ;
				case _ROLE_STATUARY_NPC:
					if ( nFlags&MK_CONTROL && g_objHero.GetPkMode() == PKMODE_FREE )
						g_objHero.LockAttack (pPlayer, true);
					else
						g_objGameMsg.AddMsg ( ( char* ) GameDataSetQuery()->GetStr ( 10352) ) ;
					break ;
				case _ROLE_SYNFLAG_NPC:
					if ( nFlags&MK_CONTROL )
						g_objHero.LockAttack (pPlayer, true);
					else
					{
						g_objHero.ActiveNpc(pPlayer->GetID());
						g_objGameMsg.AddMsg ( ( char* ) GameDataSetQuery()->GetStr ( 10352) ) ;
					}
					break ;
				case _ROLE_BOOTH_NPC:
					{
						if ( pPlayer->GetID() == g_objHero.m_objBoothManager.GetID () )
							m_DlgMain.OpenBooth ( true ) ;
						else
						{
							if ( m_DlgMain.m_DlgTrade.m_bShow || ( m_DlgMain.m_DlgNpcBooth.m_bShow && pPlayer->GetID() == g_objHero.m_objBooth.GetID () ) )
							{
								g_objGameMsg.AddMsg ( ( char* ) GameDataSetQuery()->GetStr ( 10398) ) ;
							}
							else
							{
								g_objHero.m_objBooth.Open ( pPlayer->GetID() ) ;
								m_DlgMain.OpenBooth ( false ) ;
							}
						}
					}
					break ;
				case _SYNTRANS_NPC:
					if (!m_DlgTask.m_bShow)
						g_objHero.ActiveNpc(pPlayer->GetID());
					break;
				case _ROLE_BOOTH_FLAG_NPC:
					if ( g_objHero.CanBooth ( pPlayer ) )
						m_DlgConfirm.SetConfirmDlg ( 13, NULL, pPlayer->GetID () ) ;
					break;
				case _ROLE_DICE_NPC:
					if ( !m_DlgMain.m_DlgDice.m_bShow)
					{
						if (pTarget)
							m_DlgConfirm.SetConfirmDlg ( 18, NULL, pTarget->GetID () ) ;
					}
					else
						m_DlgConfirm.SetConfirmDlg ( 19 ) ;
					break ;
				case _ROLE_WEAPONGOAL_NPC:
				case _ROLE_TARGET_NPC:
					g_objHero.LockAttack ( pPlayer, true ) ;
					break ;
				case _ROLE_MAGICGOAL_NPC:
					g_objGameMsg.AddMsg ( ( char* ) GameDataSetQuery()->GetStr ( 10354 ) ) ;
					break ;
				case 26:
					if ( nFlags&MK_CONTROL )
						g_objHero.LockAttack ( pPlayer, true ) ;
					else
						g_objHero.ActiveNpc ( pPlayer->GetID () ) ;
					break ;
				case 27:
					if (!m_DlgTask.m_bShow)
						g_objHero.ActiveNpc(pPlayer->GetID());
					break ;
				}
				return ;
			}

			if ( !pPlayer && CMD_DELNPC == m_uCmd )
			{
				OBJID tempID = g_objGameMap.GetFocusTerrainNpc () ;
				if ( tempID != ID_NONE )
				{
					m_uCmd = CMD_NORMAL ;
					g_objHero.DelNpc ( tempID ) ;
					return ;
				}

			}

			if ( nFlags&MK_CONTROL )
				g_objHero.Jump ( nPosX, nPosY ) ;
			else if ( nFlags && m_DlgMain.m_RunChk.GetCheckValue () )
				g_objHero.Run ( nPosX, nPosY ) ;
			else
				g_objHero.Walk(nPosX, nPosY);
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}

HBRUSH CMyShellDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CMyShellDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if ( pWnd == this )
		m_bMouseInShell = true ;
	else
		m_bMouseInShell = false ;
	CDialog::OnSetCursor ( pWnd, nHitTest, message ) ;
	CMyShellApp* pApp = ( CMyShellApp* ) AfxGetApp () ;
	switch ( m_uCmd )
	{
	case CMD_MPCCOURT:
		::SetCursor ( pApp->m_hCursor[4] ) ;
		return true ;
	case CMD_TEMJOIN:
		::SetCursor ( pApp->m_hCursor[9] ) ;
		return true ;
	case CMD_TEMINVITE:
		::SetCursor ( pApp->m_hCursor[8] ) ;
		return true ;
	case CMD_FRDMAKE:
		::SetCursor ( pApp->m_hCursor[2] ) ;
		return true ;
	case CMD_FACJOIN:
		::SetCursor ( pApp->m_hCursor[0] ) ;
		return true ;
	case CMD_TRADE:
		::SetCursor ( pApp->m_hCursor[10] ) ;
		return true ;
	case CMD_DELNPC:
		::SetCursor ( pApp->m_hCursor[13] ) ;
		return true ;
	case CMD_NPCEQUIP:
		::SetCursor ( pApp->m_hCursor[5] ) ;
		return true ;
	default:
		break ;
	}

	if ( m_bPickUp )
		::SetCursor ( NULL ) ;
	else if ( m_bIconFix )
		::SetCursor ( pApp->m_hCursor[1] ) ;
	else if ( pApp->GetOnButton () )
		::SetCursor ( pApp->m_hCursor[11] ) ;
	else if ( m_bCursorNpc )
		::SetCursor ( pApp->m_hCursor[12] ) ;
	else if ( m_bCursorPk )
		::SetCursor ( pApp->m_hCursor[6] ) ;
	else if ( m_bCursorPick )
		::SetCursor ( pApp->m_hCursor[3] ) ;
	else
		::SetCursor ( pApp->m_hCursor[7] ) ;
	return true ;
}

void CMyShellDlg::CheckCursor ()
{
	if ( !m_bMouseInShell )
	{
		m_bCursorNpc = false ;
		m_bCursorPk = false ;
		m_bCursorPick = false ;
		return ;
	}
	CMyShellApp* pApp = ( CMyShellApp* ) AfxGetApp () ;
	CPlayer* pPlayer = g_objPlayerSet.GetFocusPlayer ( PLAYER_NORMAL ) ;
	if ( pPlayer )
	{
		int nType = pPlayer->GetRoleType () ;
		if ( pPlayer->IsNpc ()
				&& !( pPlayer->GetRoleType () == _ROLE_STATUARY_NPC && m_uCmd == CMD_DELNPC )
				&& pPlayer->GetRoleType () != _ROLE_PLAYER )
			m_bCursorNpc = true ;
		else
		{
			m_bCursorNpc = false ;
			if ( m_uCmd == CMD_NORMAL )
			{
				switch ( g_objHero.GetPkMode () )
				{
				case PKMODE_FREE:
					m_bCursorPk = true ;
					break ;
				case PKMODE_TMEXCLUDE:
					if ( !g_objHero.IsTeamMember ( pPlayer->GetID () ) )
						m_bCursorPk = true ;
					break ;
				default:
					m_bCursorPk = false ;
					break ;
				}
			}
		}
	}
	else
	{
		CMyPos posItem = { 0, 0 } ;
		if ( !m_bPickUp && g_objGameMap.GetFocusMapItem ( posItem ) )
			m_bCursorPick = true ;
		else
			m_bCursorPick = false ;
		m_bCursorNpc = false ;
		m_bCursorPk = false ;
	}
}

LRESULT CMyShellDlg::OnMyMessage ( WPARAM wParam, LPARAM lParam )
{
	switch ( wParam )
	{
	case ON_EXIT:
		if ( !m_bExit )
		{
			m_DlgMain.WriteRecord () ;
			CMyShellDlg::OnExit ( )	;
		}
		break ;
	case ON_EXIT_LOGIN:
		if ( m_DlgLogin.m_bShow && !m_bExit )
			CMyShellDlg::OnExit () ;
		else
		{
			if ( CMyShellDlg::MessageBox ( GameDataSetQuery()->GetStr ( 10408 ), GameDataSetQuery()->GetStr ( 10041 ), MB_YESNO ) == IDYES )
			{
				m_DlgMain.WriteRecord();
				m_DlgMain.m_GoodBox.RemoveIcon ( 0, 0, true ) ;
				m_DlgMain.m_DlgQuery.m_DlgWeapon.Reset () ;
				m_DlgMain.m_DlgQuery.m_DlgSkill.Reset () ;
				g_objNetwork.Destroy() ;
				g_objGameMsg.ClearMsg() ;
				g_uStatus	=_STATUS_NONE ;
				m_DlgLogin.ShowWindow ( SW_SHOW ) ;
			}
		}
		break ;
	case ON_MIN:
		if ( !m_bExit )
		{
			CMyShellDlg::GetParent()->ShowWindow ( SW_SHOWMINIMIZED ) ;
			m_DlgTitleBar.ShowWindow ( SW_HIDE ) ;
		}
		break ;
	case ON_HELPDLG_SHOW:
		CMyShellDlg::ShowHelpDlg ( lParam ) ;
		break ;
	case ON_LOGIN_NEW:
		{
			m_DlgRoleChoose.ShowWindow(SW_SHOW);
			char Acn[16];
			char Pws[16];
			m_DlgLogin.m_LoginAccountEdt.GetWindowText ( Acn, 16) ;
			m_DlgLogin.m_LoginPasswordEdt.GetWindowText ( Pws, 16) ;
			m_DlgRoleCreate.SetAcnAndPsw ( Acn, Pws ) ;
			m_DlgRoleChoose.SetFocus () ;
		}
		break ;
	case ON_ROLECHOOSE_RETURN:
		m_DlgLogin.ShowWindow ( SW_SHOW ) ;
		m_DlgLogin.SetFocus() ;
		break ;
	case ON_ROLECHOOSE_CHOOSE:
		CMyShellDlg::CreateRole ( lParam ) ;
		break ;
	case ON_ROLECREATE_RETURN:
		m_DlgRoleChoose.ShowWindow ( SW_SHOW ) ;
		m_DlgRoleChoose.SetFocus() ;
		break ;
	case ON_TALK_CLOSE:
		m_DlgMain.OnShellEnter () ;
		break ;
	case ON_MSGDLG_POINT:
		m_MsgDlgPnt.x = LOWORD ( lParam ) ;
		m_MsgDlgPnt.y = HIWORD ( lParam ) + 42 ;
		break ;
	case ON_MSGDLG_GOODS:
		m_bMsgDlg = true ;
		m_uMsgDlgType = ON_MSGDLG_GOODS ;
		m_dwMsgDlgOwner = lParam ;
		break ;
	case ON_MSGDLG_IMAGE:
		m_bMsgDlg = true ;
		m_uMsgDlgType = ON_MSGDLG_IMAGE;
		m_dwMsgDlgOwner = lParam ;
		break ;
	case ON_MSGDLG_FACE:
		if ( m_bMsgDlg )
			break ;
		m_bMsgDlg = true ;
		m_uMsgDlgType = ON_MSGDLG_IMAGE;
		m_dwMsgDlgOwner = lParam ;
		break ;
	case ON_MSGDLG_HIDE:
		m_bMsgDlg = false ;
		m_nMsgDlgWidth = 0 ;
		if ( !m_bExpShow )
			g_objGameMap.SetShowExpFlag ( false ) ;
		break ;
	case ON_MSGDLG_STATIC:
		m_bMsgDlg = true ;
		m_uMsgDlgType = ON_MSGDLG_STATIC ;
		m_btStaticID = ( BYTE )lParam ;
		break ;
	case ON_ICON_PICKUP:
		CMyShellDlg::OnIconPickUp ( lParam ) ;
		break ;
	case ON_ICON_DROP:
		CMyShellDlg::OnIconDrop ( lParam ) ;
		break ;
	case ON_ICON_ID:
		m_IconTypeID = lParam ;
		break ;
	case ON_ICON_PNT:
		m_IconMousePnt.x = HIWORD ( lParam ) ;
		m_IconMousePnt.y = LOWORD ( lParam ) ;
		break ;
	case ON_ICON_TRADE:
		CMyShellDlg::OnIconTrade ( lParam ) ;
		break ;
	case ON_ICON_USE:
		CMyShellDlg::OnIconUse ( lParam ) ;
		break ;
	case ON_ICON_QUSE:
		CMyShellDlg::OnIconQUse ( lParam ) ;
		break ;
	case ON_ICON_BUY:
		CMyShellDlg::OnIconBuy ( lParam ) ;
		break ;
	case ON_MPCGOODS_CLOSE:
		CMyShellDlg::OnMpcGoodsClose ( ) ;
		break ;
	case ON_MPCGOODS_MONEYDROP:
		m_DlgConfirm.SetConfirmDlg( 5, NULL ) ;
		break ;
	case ON_EMONEY_NAVIGATION:
		{
			char szUrl[256] = "" ;
			if ( IniStrGet ( "ini/oem.ini", "Emoney", "URL", szUrl ) )
				::ShellExecute ( m_hWnd, NULL, szUrl, NULL, NULL, SW_SHOW ) ;
			else if ( IniStrGet ( "ini/Common.ini", "Emoney", "URL", szUrl ) )
				::ShellExecute ( m_hWnd, NULL, szUrl, NULL, NULL, SW_SHOW ) ;
		}
		break ;
	case ON_NPCBUY_CLOSE:
		m_DlgMain.m_uCurDlgID = -1 ;
		m_bIconFix = false ;
		if ( m_DlgMain.m_DlgMpcGoods.m_bShow )
			m_DlgMain.m_DlgMpcGoods.ShowWindow ( SW_HIDE ) ;
		if ( m_bPickUp && m_PickUpDlgID == DLG_NPCBUY )
			CMyShellDlg::OnIconDrop ( DLG_ALL ) ;
		m_DlgMain.m_DlgMpcGoods.SetGridContain ();
		break ;
	case ON_NPCBUY_FIX:
		if ( !m_bPickUp )
			m_bIconFix = true ;
		break ;
	case ON_MAIN_TRADE:
		g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10065 ) ) ;
		m_uCmd = CMD_TRADE ;
		break ;
	case ON_TRADE_ACCEPT:
		if ( !m_DlgMain.m_DlgTrade.m_bShow )
			g_objHero.AcceptTrade ( ) ;
		break ;
	case ON_TRADE_CLOSE:
	case ON_IMPROVE_CLOSE:
	case ON_EMBED_CLOSE:
	case ON_FUSE_CLOSE:
	case ON_ENCHANT_CLOSE:
		m_DlgMain.m_uCurDlgID = -1 ;
		if ( m_DlgMain.m_DlgMpcGoods.m_bShow )
			m_DlgMain.m_DlgMpcGoods.ShowWindow ( SW_HIDE ) ;
		break ;
	case ON_MAIN_SCREENMOVE:
		m_bAlt = lParam ;
		if ( m_bAlt == 0 )
			g_objGameMap.ResetViewPos () ;
		break ;
	case ON_TALK_DEL:
		m_DlgConfirm.SetConfirmDlg ( 7 ) ;
		break ;
	case ON_TALK_LOG:
		{
			if ( !m_DlgLog.IsWindowVisible () )
			{
				CRect rect, Hrect ;
				CMyShellDlg::GetWindowRect ( rect ) ;
				m_DlgLog.GetWindowRect ( Hrect ) ;
				m_DlgLog.MoveWindow ( rect.left + ( rect.Width () - Hrect.Width () ) / 2,
									   rect.top + ( rect.Height () - Hrect.Height () ) / 2,
									   Hrect.Width (),
									   Hrect.Height () ) ;
				m_DlgLog.ShowWindow ( SW_SHOW ) ;
			}
			else
				m_DlgLog.ShowWindow ( SW_HIDE ) ;
		}
		break ;
	case ON_TALK_CHAT:
		if ( m_DlgMain.m_DlgBlack.m_bShow )
			m_DlgMain.m_DlgBlack.ShowWindow ( SW_HIDE ) ;
		if ( !m_DlgMain.m_DlgChat.m_bShow )
		{
			m_DlgMain.CloseOtherDlg ( true ) ;
			m_DlgMain.m_DlgChat.ShowWindow ( SW_SHOW ) ;
		}
		else
			m_DlgMain.m_DlgChat.ShowWindow ( SW_HIDE ) ;
		break ;
	case ON_NPCFACE_CLOSE:
	case ON_EFFIGY_CLOSE:
	case ON_LAY_CLOSE:
		m_DlgMain.m_uCurDlgID = -1 ;
		break ;
	case ON_GROUP_JOIN:
		m_uCmd = CMD_TEMJOIN ;
		break ;
	case ON_GROUP_INVITE:
		m_uCmd = CMD_TEMINVITE ;
		break ;
	case ON_TEAMJOIN_ACCEPT:
		g_objHero.TeamAgreeApply () ;
		break ;
	case ON_TEAMINVITE_ACCEPT:
		g_objHero.TeamAcceptInvite () ;
		break ;
	case ON_FRIENDDEL_ACCEPT:
		m_DlgMain.m_DlgFriend.m_DlgFrdState.DeleteFriend () ;
		break ;
	case ON_TALKDEL_ACCEPT:
		m_DlgMain.m_DlgTalk.DeleteTalkTarget () ;
		break ;
	case ON_FACTIONSUB_ACCEPT:
		if ( lParam )
			g_objHero.SynContribute ( lParam ) ;
		break ;
	case ON_FACTION_KICK:
		m_DlgConfirm.SetConfirmDlg ( 12, m_DlgMain.m_DlgQuery_Faction.m_strCurName ) ;
		break ;
	case ON_FACTION_SUB:
		m_DlgConfirm.SetConfirmDlg ( 9 ) ;
		break ;
	case ON_FACTION_INVITE:
		m_uCmd = CMD_FACINVITE ;
		break ;
	case ON_FACTION_JOIN:
		g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10066 ) ) ;
		m_uCmd = CMD_FACJOIN ;
		break ;
	case ON_FACTION_OUT:
		m_DlgConfirm.SetConfirmDlg ( 8 ) ;
		break ;
	case ON_FACTIONINVITE_ACCEPT:
		g_objHero.SyndicateAcceptInvite () ;
		break ;
	case ON_FACTIONJOIN_ACCEPT:
		g_objHero.SynDicateAcceptJoin () ;
		break ;
	case ON_MONEYDROP_ACCEPT:
		if ( lParam )
		{
			int xPos, yPos ;
			g_objHero.GetScrPos ( xPos, yPos ) ;
			g_objHero.DropMoney ( lParam, xPos, yPos ) ;
		}
		break ;
	case ON_FACTIONOUT_ACCEPT:
		g_objHero.LeaveSyndicate () ;
		break ;
	case ON_ACTION_INI:
		{
			int nAction = GetPrivateProfileInt ( "Button810", "ActionID", -1, "ini/ControlAction.Ini" ) ;
			if ( nAction != -1 )
				g_objHero.ActiveAction ( nAction, m_DlgMain.m_DlgQuery_Faction.m_strCurName ) ;
		}
		break ;
	case ON_FRIEND_ADD:
		g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10345 ) ) ;
		m_uCmd = CMD_FRDMAKE ;
		break ;
	case ON_FRIEND_CHAT:
		{
			char strName[_MAX_NAMESIZE]="";
			m_DlgMain.m_DlgFriend.m_FriendLstData.GetText(lParam,strName);
			m_DlgMain.m_DlgTalk.m_TalkNameEdt.SetWindowText ( strName ) ;
			m_DlgMain.m_DlgTalk.SetTalkMode ( 5005 ) ;
			m_DlgMain.m_DlgTalk.m_TalkDataCob.SetFocus () ;
		}
		break ;
	case ON_FRIEND_LEAVE:
		if ( !m_DlgMain.m_DlgMsgSend.m_bShow )
		{
			m_DlgMain.CloseOtherDlg ( true ) ;
			m_DlgMain.m_DlgMsgSend.ShowWindow ( SW_SHOW ) ;
			char strFriendName[16] ;
			m_DlgMain.m_DlgFriend.m_FriendLstData.GetText ( lParam, strFriendName ) ;
			m_DlgMain.m_DlgMsgSend.SetTargetFriend ( strFriendName ) ;
		}
		break ;
	case ON_FRDSTATE_DEL:
		m_DlgConfirm.SetConfirmDlg( 6, NULL ) ;
		break ;
	case ON_FRIEND_RESUME:
		if ( lParam )
			m_DlgMain.m_DlgFriend.ShowWindow ( SW_SHOW ) ;
		else
			m_DlgMain.m_DlgEnemy.ShowWindow ( SW_SHOW ) ;
		break ;
	case ON_SETUP_MODECHANGE:
		if ( m_nScreenMode != lParam )
		{
			CMyShellDlg::SetScreenMode ( lParam, false ) ;
		}
		break ;
	case ON_SETUP_HOTKEY:
		m_DlgMain.m_DlgSetup.ShowWindow ( SW_HIDE ) ;
		m_DlgMain.m_DlgHotkey.ShowWindow ( SW_SHOW ) ;
		break ;
	case ON_SETUP_OTHER:
		m_DlgMain.m_DlgSetup.ShowWindow ( SW_HIDE ) ;
		m_DlgMain.m_DlgOther.ShowWindow ( SW_SHOW ) ;
		break ;
	case ON_MAGIC_CHOOSE:
		{
			if ( m_DlgMain.m_DlgMagic.m_bShow || m_DlgMain.m_DlgKongfu.m_bShow )
			{
				m_DlgMain.m_DlgMagic.ShowWindow ( SW_HIDE ) ;
				m_DlgMain.m_DlgKongfu.ShowWindow ( SW_HIDE ) ;
			}
			m_uCurSkill = 1 ;
			m_MagicID = lParam ;
			char SkillType[32] ;
			sprintf ( SkillType, "MagicSkillType%d", m_MagicID ) ;
			m_DlgMain.m_MagicImg.InsertImage ( SkillType, m_MagicID, false ) ;
			m_bMsgDlg = false ;
			m_nMsgDlgWidth = 0 ;
		}
		break ;
	case ON_XP_CHOOSE:
		{
			m_XpID = lParam ;
			for ( int i = 0; i < g_objHero.GetXpSkillAmount (); i++ )
			{
				CMagic* pXp = g_objHero.GetXpSkillByIndex ( i ) ;
				if ( pXp && pXp->GetIDType() == m_XpID )
				{
					if ( pXp->TestTarget ( 2 ) )
					{
						g_objHero.MagicAttack( m_XpID, g_objHero.GetID() ) ;
					}
					else
					{
						m_uCurSkill = 2 ;
						char SkillType[32] ;
						sprintf ( SkillType, "XpSkillType%d", m_XpID ) ;
						m_DlgMain.m_MagicImg.InsertImage ( SkillType, m_XpID, false ) ;
						break ;
					}
				}
			}
		}
		break ;
	case ON_OWNER_MOVE:
		m_bDlgMove = true ;
		CMyShellDlg::GetParent()->GetWindowRect ( m_InitDlgRect ) ;
		break ;
	case ON_OWNER_STOP:
		m_bDlgMove = false ;
		break ;
	case ON_MSGSEND_VIEW:
		m_DlgMain.CloseOtherDlg ( true ) ;
		m_DlgMain.m_DlgMsgReceive.ShowWindow ( SW_SHOW ) ;
		break ;
	case ON_MSGREC_REVERT:
		m_DlgMain.CloseOtherDlg ( true ) ;
		if ( lParam )
		{
			m_DlgMain.m_DlgMsgSend.SetTargetFriend ( ( char* )lParam ) ;
			delete ( char* )lParam ;
		}
		m_DlgMain.m_DlgMsgSend.ShowWindow ( SW_SHOW ) ;
		break ;
	case ON_MPCBOOTH_CLOSE:
		if ( m_DlgMain.m_DlgHawk.m_bShow )
			m_DlgMain.m_DlgHawk.ShowWindow ( SW_HIDE ) ;
		if ( lParam == 0 )
			m_DlgMain.m_DlgMpcBooth.ShowWindow ( SW_HIDE ) ;
		m_DlgMain.m_uCurDlgID = -1 ;
		if ( m_DlgMain.m_DlgMpcGoods.m_bShow )
			m_DlgMain.m_DlgMpcGoods.ShowWindow ( SW_HIDE ) ;
		if ( m_DlgMain.m_DlgNpcBooth.m_bShow )
		{
			g_objHero.m_objBooth.Close () ;
			m_DlgMain.m_DlgNpcBooth.ShowWindow ( SW_HIDE ) ;
		}
		break ;
	case ON_MPCBOOTH_ADDITEM:
		m_DlgConfirm.SetConfirmDlg ( 10 ) ;
		break ;
	case ON_BOOTHOPEN_ACCEPT:
		{
			CPlayer* pPlayer = g_objPlayerSet.GetPlayer ( lParam ) ;
			if ( pPlayer != NULL )
			{
				if(!g_objHero.m_objBoothManager.IsActive())
					g_objHero.LockBooth(pPlayer);
			}
		}
		break ;
	case ON_MPCBOOTHADD_ACCEPT:
		{
			CItem* pItem = g_objHero.GetItemByIndex ( m_DlgMain.m_DlgMpcBooth.m_nCurRealID ) ;
			if ( pItem )
				g_objHero.m_objBoothManager.AddItem ( pItem->GetID (),
													  lParam ) ;
		}
		break ;
	case ON_MPCBOOTHADD_EMONEY_ACCEPT:
		{
			CItem* pItem = g_objHero.GetItemByIndex ( m_DlgMain.m_DlgMpcBooth.m_nCurRealID ) ;
			if ( pItem )
				g_objHero.m_objBoothManager.AddItem ( pItem->GetID (), lParam, true, true ) ;
		}
		break ;
	case ON_MPCBOOTH_HAWK:
		if ( !m_DlgMain.m_DlgHawk.m_bShow )
		{
			m_DlgMain.m_DlgHawk.ShowWindow ( SW_SHOW ) ;
		}
		break ;
	case ON_NPCBOOTH_CLOSE:
		g_objHero.m_objBooth.Close () ;
		m_DlgMain.m_uCurDlgID = -1 ;
		if ( m_DlgMain.m_DlgMpcGoods.m_bShow )
			m_DlgMain.m_DlgMpcGoods.ShowWindow ( SW_HIDE ) ;
		break ;
	case ON_CHAT_SIFT:
		if ( !m_DlgMain.m_DlgBlack.m_bShow )
			m_DlgMain.m_DlgBlack.ShowWindow ( SW_SHOW ) ;
		break ;
	case ON_MPCCOURT_ACCEPT:
		{
			CPlayer* pPlayer = g_objPlayerSet.GetPlayer ( lParam ) ;
			if ( pPlayer != NULL )
				g_objHero.Marriage ( pPlayer ) ;
		}
		break ;
	case ON_OTHER_DELNPC:
		m_DlgMain.m_DlgOther.ShowWindow ( SW_HIDE ) ;
		m_uCmd = CMD_DELNPC ;
		break ;
	case ON_MAIN_NPCEQUIP:
		if ( lParam )
			m_uCmd = CMD_NPCEQUIP ;
		else if ( CMD_NPCEQUIP == m_uCmd )
			m_uCmd = CMD_NORMAL ;
		break ;
	case ON_IMPTITEM_ACCEPT:
		{
			CItem* pItem = g_objHero.GetItemByIndex ( m_nImptIndex ) ;
			if ( pItem && pItem->GetID () == m_IconRealID )
			{
				g_objHero.SellItem ( pItem->GetID () ) ;
			}
		}
		break ;
	case ON_DROPITEM_ACCEPT:
		{
			CItem* pItem = g_objHero.GetItemByIndex ( m_nDropIndex ) ;
			if ( pItem )
			{
				if ( !g_objHero.DropItem ( pItem->GetID (), m_DropPnt.x, m_DropPnt.y ) )
					CMyShellDlg::WindowProc ( WM_MY_MESSAGE, CMD_FLASHITEM, NULL ) ;
			}
		}
		break ;
	case ON_TACKOFF_ACCEPT:
		m_DlgMain.m_DlgEmbed.TackOffGem () ;
		break ;
	case ON_EMBED_TACKOFF:
		m_DlgConfirm.SetConfirmDlg ( 17 ) ;
		break ;
	case ON_SETUP_SHIFT:
		m_bShiftDown = lParam ;
		break ;
	case ON_SETUP_KOSHOW:
		g_bKoShowRecord = lParam ;
		break ;
	case ON_FRIENDADD_ACCEPT:
		g_objHero.MakeFriend ( g_objHero.GetLastFriendApply () ) ;
		break ;
	case ON_EMONEYBUY_ACCEPT:
		g_objHero.BuyItem ( lParam ) ;
		break ;
	case ON_EXPBALL_CONFIRM:
		m_DlgConfirm.SetConfirmDlg ( CONFIRM_TYPE_26, NULL, lParam ) ;
		break ;
	case ON_EXPBALL_ACCEPT:
		g_objHero.UseItem ( lParam ) ;
		if ( g_objHero.m_objEffect )
			g_objHero.m_objEffect->Add ( "ExpBall" ) ;
		break ;
	case ON_EXERCISE_EXIT:
		switch ( lParam )
		{
		case 0 :
		case 1 :
		case 2 :
			CMyShellDlg::ConfirmedExit () ;
			break ;
		case 3 :
			if ( m_bLogin )
			{
				m_bExit = false ;
				m_bIconFix = false ;
				m_PickUpDlgID = 0 ;
				m_bPickUp = false ;
				m_bLogin = false ;
				m_uCmd = 0 ;
				m_DlgMain.CloseCurDlg () ;
				m_DlgMain.m_DlgMpcGoods.ShowWindow ( SW_HIDE ) ;
				m_DlgMain.m_DlgTalk.m_TalkDataCob.SetWindowText ( "" ) ;
				m_DlgMain.WriteRecord () ;
				m_DlgMain.m_GoodBox.RemoveIcon ( 0, 0, true ) ;
				if ( g_objHero.GetTeamMemberAmount () )
					g_objHero.ClearTeamMember () ;
				g_objNetwork.Destroy () ;
				g_objGameMsg.ClearMsg () ;
				g_uStatus = _STATUS_NONE ;
				m_MagicID = 0 ;
				m_DlgRoleCreate.ShowWindow ( SW_HIDE ) ;
				m_DlgRoleCreate.EnableWindow ( FALSE ) ;
				m_DlgLogin.ShowWindow ( SW_SHOW ) ;
				m_DlgLogin.SetFocus () ;
			}
			else
			{
				m_bExit = false ;
				m_bIconFix = false ;
				m_PickUpDlgID = 0 ;
				m_bPickUp = false ;
				m_uCmd = 0 ;
				m_DlgRoleCreate.EnableWindow ( FALSE ) ;
				m_DlgRoleCreate.ShowWindow ( SW_HIDE ) ;
				m_DlgLogin.ShowWindow ( SW_SHOW ) ;
				m_DlgLogin.EnableWindow ( TRUE ) ;
				m_DlgLogin.SetFocus () ;
			}
			g_objGameMap.Reset () ;
			g_objHero.Reset () ;
			DXCloseAllMedia () ;
			DXCloseAllSound () ;
			break ;
		}
		break ;
	case ON_EXERCISE_CANCEL:
		m_bExit = false ;
		if ( !m_bLogin )
		{
			if ( m_DlgLogin.m_bShow && !m_DlgLogin.IsWindowVisible () )
			{
				m_DlgLogin.ShowWindow ( SW_SHOW ) ;
				m_DlgLogin.EnableWindow ( TRUE ) ;
				m_DlgLogin.SetFocus () ;
				DXSetSoundVolume ( 75 ) ;
			}
		}
		else if ( m_DlgRoleCreate.m_bShow )
		{
			m_DlgRoleCreate.ShowWindow ( SW_SHOW ) ;
			m_DlgRoleCreate.EnableWindow ( TRUE ) ;
			m_DlgRoleCreate.SetFocus () ;
		}
		break ;
	case ON_EXERCISE_TIME:
		{
			int nTotalTime = m_DlgMain.m_DlgExercise.GetTotalTime () ;
			char szMsg[256] ;
			sprintf ( szMsg, GameDataSetQuery()->GetStr ( 11015 ), nTotalTime / 60, nTotalTime % 60 ) ;
			m_DlgConfirm.SetConfirmDlg ( CONFIRM_TYPE_28, szMsg, 0 ) ;
		}
		break ;
	case ON_MAIN_EXPDOWN:
		m_bExpShow = !m_bExpShow ;
		if ( m_bExpShow )
			g_objGameMap.SetShowExpFlag ( true ) ;
		else
			g_objGameMap.SetShowExpFlag ( false ) ;
		break ;
	case ON_JOINDICE_ACCEPT:
		g_objHero.m_objDiceManager.Open ( lParam ) ;
		break ;
	case ON_OUTDICE_ACCEPT:
		g_objHero.m_objDiceManager.Close () ;
		break ;
	case ON_DICE_LIST:
		m_DlgMain.m_DlgDiceList.ShowWindow ( !m_DlgMain.m_DlgDiceList.m_bShow ) ;
		break ;
	case ON_ENMSTATE_DEL:
		m_DlgConfirm.SetConfirmDlg( 20, NULL ) ;
		break ;
	case ON_ENEMYDEL_ACCEPT:
		m_DlgMain.m_DlgEnemy.m_DlgEnmState.DelEnemy () ;
		break ;
	case ON_FRDSTATE_SHOW:
		if ( m_DlgMain.m_DlgEnemy.m_bShow )
			m_DlgMain.m_DlgEnemy.ShowWindow ( SW_HIDE ) ;
		break ;
	case ON_DICE_HIDE:
		if ( m_DlgMain.m_DlgDiceList.m_bShow )
			m_DlgMain.m_DlgDiceList.ShowWindow ( SW_HIDE ) ;
		break ;
	case ON_BLACK_CLOSE:
		m_DlgMain.CloseOtherDlg ( true ) ;
		m_DlgMain.m_DlgChat.ShowWindow ( SW_SHOW ) ;
		break ;
	case ON_INTERFACE_CHANGE:
		m_DlgTitleBar.ResetBk ( lParam ) ;
		m_DlgHelp.ResetBk ( lParam ) ;
		m_DlgLog.ResetBk ( lParam ) ;
		break ;
	case ON_INSHELP_SHOW:
		m_DlgMain.CloseOtherDlg ( false ) ;
		break ;
	case ON_RESOURCE_ACCEPT:
		::ShellExecute(m_hWnd, NULL, GameDataSetQuery()->GetStr ( 10367 ), NULL, NULL, SW_SHOW );
		break ;
	case ON_TALKACT_SHOW:
		if ( m_DlgMain.m_DlgPk.m_bShow )
			m_DlgMain.m_DlgPk.ShowWindow ( SW_HIDE ) ;
		break ;
	case ON_LEVWORD_CHAT:
		{
			BulletinTitleInfo* pBulletInfo = g_objHero.m_objBulletin.GetListLine ( lParam ) ;
			if ( pBulletInfo )
			{
				m_DlgMain.m_DlgTalk.m_TalkNameEdt.SetWindowText ( pBulletInfo->szName ) ;
				m_DlgMain.m_DlgTalk.SetTalkMode ( 5002 ) ;
				m_DlgMain.m_DlgTalk.m_TalkDataCob.SetFocus () ;
			}
		}
		break ;
	default:
		break ;
	}
	return 0 ;
}

HHOOK glhHook ;
CDlgMain* gpDlgMain = NULL ;
CMyShellDlg* gpDlgShell = NULL ;
DWORD	gdwLastTime = 0 ;
int		gnLastCode = 0 ;
LRESULT CALLBACK KeyBoardProc(int nCode,WPARAM wparam,LPARAM lparam)
{

	if ( wparam == 13 )
	{
		CWnd* pFocus = CWnd::GetFocus () ;
		if ( pFocus == NULL )
			return true ;
		if ( !gpDlgShell->m_bLogin)
			return true ;
		HWND hFocus = pFocus->m_hWnd ;
		if ( ( gpDlgMain->m_DlgMsgSend.m_bShow && hFocus == gpDlgMain->m_DlgMsgSend.m_MsgSendData.m_hWnd )
			|| ( gpDlgMain->m_DlgHawk.m_bShow && hFocus == gpDlgMain->m_DlgHawk.m_EdtData.m_hWnd )
			|| ( gpDlgMain->m_DlgLevWord.m_bShow && gpDlgMain->m_DlgLevWord.m_LevWordEdtData.IsWindowEnabled () && hFocus == gpDlgMain->m_DlgLevWord.m_LevWordEdtData.m_hWnd )
			|| ( gpDlgShell->m_DlgTask.m_bShow && hFocus == gpDlgShell->m_DlgTask.m_TaskEdtAnswer.m_hWnd )
			|| ( gpDlgMain->m_DlgTalk.m_bShow && hFocus == gpDlgMain->m_DlgTalk.m_TalkNameEdt.m_hWnd )
			|| ( gpDlgMain->m_DlgBlack.m_bShow && hFocus == gpDlgMain->m_DlgBlack.m_BlackEdtName.m_hWnd )
			|| hFocus == gpDlgMain->m_DlgTalk.m_TalkDataCob.GetSafeHwnd () )
		{
			return CallNextHookEx(glhHook,nCode,wparam,lparam);
		}
		gpDlgMain->OnShellEnter () ;
		return true ;
	}

	if ( wparam < 16 ||
			( wparam > 18 && wparam < 37 ) ||
				( wparam > 40 && wparam < 48 ) ||
					( wparam > 90 && wparam < 112) ||
						wparam > 123 || gpDlgMain->m_DlgHotkey.m_bShow )
		return CallNextHookEx(glhHook,nCode,wparam,lparam);

	int nValue = 0 ;
	BYTE btState[256] ;
	GetKeyboardState ( btState ) ;
	nValue += btState[VK_SHIFT] >> 7 ;
	if ( nValue && wparam == 53 )
		return true;

	nValue += btState[VK_MENU] >> 7 ;
	nValue += btState[VK_CONTROL] >> 7 ;
	if ( wparam >= 48 && wparam <= 90 && !nValue )
		return CallNextHookEx(glhHook,nCode,wparam,lparam);

	if ( gnLastCode == wparam )
	{
		if ( ( TimeGet() - gdwLastTime ) < 200 )
				return CallNextHookEx(glhHook,nCode,wparam,lparam);
		else
			gdwLastTime = TimeGet() ;
	}
	else
	{
		gnLastCode = wparam ;
		gdwLastTime = TimeGet() ;
	}

	if ( wparam >= 48 && wparam <= 90 )
		if ( gpDlgShell->ProcessHotkey ( wparam ) )
			return true ;

	int nIndex = -1;
	switch ( wparam )
	{
	case 37:
		if ( btState[VK_CONTROL] >> 7 )
		{
			gpDlgMain->m_DlgTalk.CycleTalkTarget ( SB_LEFT ) ;
			return true ;
		}
		break ;
	case 38:
		if ( btState[VK_CONTROL] >> 7 )
		{
			gpDlgMain->m_DlgTalk.CycleTalkChannel ( SB_TOP ) ;
			return true ;
		}
		break ;
	case 39:
		if ( btState[VK_CONTROL] >> 7 )
		{
			gpDlgMain->m_DlgTalk.CycleTalkTarget ( SB_RIGHT ) ;
			return true ;
		}
		break ;
	case 40:
		if ( btState[VK_CONTROL] >> 7 )
		{
			gpDlgMain->m_DlgTalk.CycleTalkChannel ( SB_BOTTOM ) ;
			return true ;
		}
		break ;
	case 112:
		nIndex = 0 ;
		break ;
	case 113:
		nIndex = 1 ;
		break ;
	case 114:
		nIndex = 2 ;
		break ;
	case 115:
		nIndex = 3 ;
		break ;
	case 116:
		nIndex = 4 ;
		break ;
	case 117:
		nIndex = 5 ;
		break ;
	case 118:
		nIndex = 6 ;
		break ;
	case 119:
		nIndex = 7 ;
		break ;
	case 120:
		nIndex = 8 ;
		break ;
	case 121:
		nIndex = 9 ;
		break ;
	case 123:
		g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10067 ) ) ;
		CMyBitmap::ScreenShot ( NULL ) ;
		gdwLastTime = TimeGet() ;
		return true ;
	}
	if ( nIndex != -1 )
	{
		if ( gpDlgMain->m_GoodBox.GetGridAble ( CPoint ( nIndex + 1, 1 ) ) )
		{
			LPARAM lParam = ( nIndex + 1 ) | gpDlgMain->m_GoodBox.GetIconTypeID ( nIndex ) << 8 ;
			gpDlgShell->OnIconQUse ( lParam ) ;
		}
		return true;
	}
	return CallNextHookEx(glhHook,nCode,wparam,lparam);
}

void CMyShellDlg::StartGame()
{
	if ( !m_bLogin )
		m_bLogin = true ;
	if ( !m_bGameStarted )
	{
		m_bGameStarted = true ;

		m_DlgLogin.EndDialog ( 0 ) ;
		m_DlgRoleChoose.EndDialog ( 0 ) ;
		m_DlgRoleCreate.EndDialog ( 0 ) ;
		CRect rect ;

		if ( m_nScreenMode == 2 || m_nScreenMode == 3 )
		{
			CRect DlgRect ;
			CMyShellDlg::GetWindowRect ( DlgRect ) ;
			m_SliLine.GetWindowRect ( rect ) ;
			m_SliLine.MoveWindow ( 0, rect.top - DlgRect.top + 168, rect.Width (), rect.Height () ) ;
		}
		CMyShellDlg::GetWindowRect ( rect ) ;
		m_SliLine.Init ( rect.left, rect.top, "Slider0", true ) ;
		m_SliLine.SetSliderWidth ( 300 ) ;
		m_SliLine.SetMaxValue ( 300 ) ;
		m_BtnSysHide.MoveWindow ( CMyBitmap::GetScreenWidth () - 40, 4, 16, 16 ) ;
		m_BtnMsgHide.MoveWindow ( CMyBitmap::GetScreenWidth () - 20, 4, 16, 16 ) ;
		m_BtnSysHide.Init ( rect.left, rect.top, "Button01" ) ;
		m_BtnMsgHide.Init ( rect.left, rect.top, "Button02" ) ;

		m_DlgTask.Create ( IDD_DIALOG_TASK, this ) ;

		m_DlgConfirm.Create ( IDD_DIALOG_CONFIRM, this );
		m_DlgConfirm.GetWindowRect ( rect ) ;
		m_DlgConfirm.MoveWindow ( ( CMyBitmap::GetScreenWidth () - rect.Width () ) / 2,
								  ( CMyBitmap::GetScreenHeight () - rect.Height () ) / 2,
								  rect.Width (),
								  rect.Height () ) ;
		m_DlgConfirm.m_ConfirmEdtData.SetFont( m_pGameFont );

		m_DlgMain.Create ( IDD_MAIN_DIALOG, this ) ;
		m_DlgMain.GetWindowRect ( rect ) ;
		m_DlgMain.MoveWindow ( 0,
							   CMyBitmap::GetScreenHeight () - DLG_MAIN_HEIGHT,
							   CMyBitmap::GetScreenWidth (),
							   DLG_MAIN_HEIGHT ) ;
		if ( m_nScreenMode == 2 || m_nScreenMode == 3 )
			m_DlgMain.m_DlgSetup.SetScreenMode ( 1 ) ;

		m_DlgTask.m_TaskEdtAnswer.SetFont ( m_pGameFont ) ;
		m_DlgMain.m_DlgDepot.m_DepotEdtMoney.SetFont ( m_pGameFont ) ;
		m_DlgMain.m_DlgTalk.m_TalkDataCob.SetFont ( m_pGameFont ) ;
		m_DlgMain.m_DlgTalk.m_TalkNameEdt.SetFont ( m_pGameFont ) ;
		m_DlgMain.m_DlgTrade.m_TradeEdtMpcMoney.SetFont ( m_pGameFont ) ;
		m_DlgMain.m_DlgMsgSend.m_MsgSendData.SetFont ( m_pGameFont ) ;
		m_DlgMain.m_DlgHawk.m_EdtData.SetFont ( m_pGameFont ) ;
		m_DlgMain.m_DlgQuery_Faction.m_EdtWord.SetFont ( m_pGameFont ) ;
		m_DlgMain.m_DlgBlack.m_BlackEdtName.SetFont ( m_pGameFont ) ;
		m_DlgMain.m_DlgLevWord.m_LevWordEdtData.SetFont ( m_pGameFont ) ;
		m_DlgMain.m_DlgHotkey.m_LstParent.SetFont ( m_pGameFont ) ;
		m_DlgMain.m_DlgHotkey.m_LstChild.SetFont ( m_pGameFont ) ;

		if ( strcmp ( GameDataSetQuery()->GetVersionInfo (), "English" ) == 0 )
		{
			m_DlgTask.m_TaskEdtAnswer.EnableChinese ( false ) ;
			m_DlgMain.m_DlgDepot.m_DepotEdtMoney.EnableChinese ( false ) ;
			m_DlgMain.m_DlgTalk.m_TalkDataCob.EnableChinese ( false ) ;
			m_DlgMain.m_DlgTalk.m_TalkNameEdt.EnableChinese ( false ) ;
			m_DlgMain.m_DlgTrade.m_TradeEdtMpcMoney.EnableChinese ( false ) ;
			m_DlgMain.m_DlgMsgSend.m_MsgSendData.EnableChinese ( false ) ;
			m_DlgMain.m_DlgHawk.m_EdtData.EnableChinese ( false ) ;
			m_DlgMain.m_DlgQuery_Faction.m_EdtWord.EnableChinese ( false ) ;
			m_DlgMain.m_DlgBlack.m_BlackEdtName.EnableChinese ( false ) ;
			m_DlgMain.m_DlgLevWord.m_LevWordEdtData.EnableChinese ( false ) ;
		}

		POINT ptRgn[6] ;
		ptRgn[0].x = 0 ;			ptRgn[0].y = 0 ;
		ptRgn[1].x = 100 ;			ptRgn[1].y = 0 ;
		ptRgn[2].x = 100 ;			ptRgn[2].y = 90 ;
		ptRgn[3].x = CMyBitmap::GetScreenWidth () ;	ptRgn[3].y = 90 ;
		ptRgn[4].x = CMyBitmap::GetScreenWidth () ;	ptRgn[4].y = 141 ;
		ptRgn[5].x = 0 ;			ptRgn[5].y = 141 ;
		CRgn rgn ;
		rgn.CreatePolygonRgn ( ptRgn, 6, ALTERNATE ) ;
		m_DlgMain.SetWindowRgn ( rgn, true ) ;
		m_DlgMain.ShowWindow ( SW_SHOW ) ;

		CMyShellDlg::GetWindowRect ( rect ) ;
		CRect PrgRect ;

		m_DlgMain.m_ExpPrg.GetWindowRect ( PrgRect ) ;
		m_StaticPoint[0] = CPoint ( PrgRect.left - rect.left, PrgRect.top - rect.top - 20 ) ;
		m_DlgMain.m_RunChk.GetWindowRect ( PrgRect ) ;
		m_StaticPoint[1] = CPoint ( PrgRect.left - rect.left, PrgRect.top - rect.top - 20 ) ;
		m_DlgMain.m_TalkChk.GetWindowRect ( PrgRect ) ;
		m_StaticPoint[2] = CPoint ( PrgRect.left - rect.left, PrgRect.top - rect.top - 20 ) ;
		m_DlgMain.m_MapChk.GetWindowRect ( PrgRect ) ;
		m_StaticPoint[3] = CPoint ( PrgRect.left - rect.left, PrgRect.top - rect.top - 20 ) ;
		m_DlgMain.m_ScreenMoveChk.GetWindowRect ( PrgRect ) ;
		m_StaticPoint[4] = CPoint ( PrgRect.left - rect.left, PrgRect.top - rect.top - 20 ) ;
		m_DlgMain.m_StandChk.GetWindowRect ( PrgRect ) ;
		m_StaticPoint[5] = CPoint ( PrgRect.left - rect.left, PrgRect.top - rect.top - 20 ) ;
		m_DlgMain.m_XpPrg.GetWindowRect ( PrgRect ) ;
		m_StaticPoint[6] = CPoint ( PrgRect.left - rect.left, PrgRect.top - rect.top - 20 ) ;
		m_DlgMain.m_MpPrg.GetWindowRect ( PrgRect ) ;
		m_StaticPoint[7] = CPoint ( PrgRect.left - rect.left, PrgRect.top - rect.top - 20 ) ;
		m_DlgMain.m_HpPrg.GetWindowRect ( PrgRect ) ;
		m_StaticPoint[8] = CPoint ( PrgRect.left - rect.left, PrgRect.top - rect.top - 20 ) ;

		glhHook = SetWindowsHookEx ( WH_KEYBOARD,
									 KeyBoardProc,
									 NULL,
									 GetCurrentThreadId() );
		if ( glhHook != NULL )
		{
			gpDlgMain = &m_DlgMain ;
			gpDlgShell = this ;
		}
	}
	else
	{
		m_DlgMain.m_DlgGroup.SetGroupContain () ;
	}
	m_PickUpDlgID = 0 ;
	m_bPickUp = false ;
	CRect MapRect ;
	m_DlgMain.m_MapChk.GetWindowRect ( MapRect ) ;
	CMyPos posHide = { MapRect.left, MapRect.top } ;
	g_objGameMap.m_objRadar.SetHidePos ( posHide ) ;
	CMyShellDlg::SetFocus () ;
}

void CMyShellDlg::CreateRole ( BYTE btRoleType )
{
	m_DlgRoleCreate.SetRoleType ( btRoleType ) ;
	m_DlgRoleCreate.ShowWindow(SW_SHOW);
	m_DlgRoleCreate.SetFocus () ;
}

void CMyShellDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	MouseSet(point.x, point.y, 0);

	if ( m_bDlgMove )
	{
		if ( nFlags == 0 )
			m_bDlgMove = false ;
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CMyShellDlg::SetMsgDlgPnt(LPARAM lParam, int nWidth)
{
	int nMsgDlgHeight = lParam * 18 ;
	if ( m_MsgDlgPnt.x + nWidth > CMyBitmap::GetScreenWidth () )
		m_MsgDlgPnt.x -= (m_MsgDlgPnt.x + nWidth) - CMyBitmap::GetScreenWidth () ;
	if ( m_MsgDlgPnt.y + nMsgDlgHeight > CMyBitmap::GetScreenHeight () )
	{
		if ( m_MsgDlgPnt.y - 42 - nMsgDlgHeight < 0 )
			m_MsgDlgPnt.y = 0 ;
		else
			m_MsgDlgPnt.y -= 42 + nMsgDlgHeight ;
	}
}

void CMyShellDlg::ShowMsgDlg()
{
	switch ( m_uMsgDlgType )
	{
	case ON_MSGDLG_GOODS:
		{
			switch ( m_dwMsgDlgOwner )
			{
			case DLG_MAIN:
				{
					DWORD dwTypeID = m_DlgMain.m_GoodBox.GetMouseMoveTypeID () ;
					int nItemNum = 0 ;
					int nItemIndex = 0 ;
					for (int i = 0; i < g_objHero.GetItemAmount (); i++ )
					{
						if ( g_objHero.GetItemByIndex ( i )->GetTypeID () == dwTypeID )
						{
							if ( nItemNum  == 0 )
							{
								nItemNum++ ;
								nItemIndex = i ;
							}
							else
								nItemNum++ ;
						}
					}
					CItem* pItem = g_objHero.GetItemByIndex ( nItemIndex ) ;
					IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
					if ( pAni != NULL && pItem != NULL )
						ShowMessage ( pItem, pAni, nItemNum ) ;
				}
				break ;
			case DLG_QUERY_MISSION:
				{
					int nIndex = m_DlgMain.m_DlgQuery.m_DlgMission.GetMouseMovePosition() ;
					if ( nIndex < 1 )
						return ;
					CItem *pItem = g_objHero.GetEquipment( nIndex ) ;
					IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
					if ( pAni != NULL )
						ShowMessage ( pItem, pAni ) ;
				}
				break ;
			case DLG_NPCBUY:
				{
					DWORD dwIndex = m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.GetMouseMoveIndex () ;
					CItem* pItem = g_objShop.GetItem ( dwIndex ) ;
					IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
					if ( pAni != NULL && pItem != NULL )
						ShowMessage ( pItem, pAni ) ;
				}
				break ;
			case DLG_MPCGOODS:
				{
					DWORD dwIndex = m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.GetMouseMoveIndex () ;
					CItem* pItem = g_objHero.GetItemByIndex ( dwIndex ) ;
					IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
					if ( pAni != NULL && pItem != NULL )
						ShowMessage ( pItem, pAni, -1 ) ;
				}
				break ;
			case DLG_TRADENPC:
				{
					DWORD dwIndex = m_DlgMain.m_DlgTrade.m_TradeGriNpc.GetMouseMoveIndex () ;
					CItem* pItem = g_objHero.GetPlayerTradeItemByIndex ( m_DlgMain.m_DlgTrade.m_nNpcPage + dwIndex ) ;
					IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
					if ( pAni != NULL && pItem != NULL )
						ShowMessage ( pItem, pAni ) ;
				}
				break ;
			case DLG_IMPROVE:
				{
					CItem* pItem = g_objHero.GetItemByIndex ( m_DlgMain.m_DlgImprove.GetMouseMoveId () ) ;
					IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
					if ( pAni != NULL && pItem != NULL )
						ShowMessage ( pItem, pAni ) ;
				}
				break ;
			case DLG_ENCHANT:
				{
					OBJID idItem = m_DlgMain.m_DlgEnchant.GetMouseMoveId () ;
					CItem* pItem = idItem ? g_objHero.GetItem ( idItem ) : NULL ;
					IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
					if ( pAni != NULL && pItem != NULL )
						ShowMessage ( pItem, pAni ) ;
				}
				break ;
			case DLG_TRADEMPC:
				{
					DWORD dwIndex = m_DlgMain.m_DlgTrade.m_TradeGriMpc.GetMouseMoveIndex () ;
					CItem* pItem = g_objHero.GetHeroTradeItemByIndex ( m_DlgMain.m_DlgTrade.m_nMpcPage + dwIndex ) ;
					IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
					if ( pAni != NULL && pItem != NULL )
						ShowMessage ( pItem, pAni ) ;
				}
				break ;
			case DLG_DEPOT:
				{
					DWORD dwIndex = m_DlgMain.m_DlgDepot.m_DepotGriLay.GetMouseMoveIndex () ;
					dwIndex = m_DlgMain.m_DlgDepot.m_nCurPage * 20 + dwIndex ;
					CItem* pItem = g_objHero.GetPackageItemByIndex ( dwIndex ) ;
					IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
					if ( pAni != NULL && pItem != NULL )
						ShowMessage ( pItem, pAni, 0, false ) ;
				}
				break ;
			case DLG_NPCFACE:
				{
					CMyBitmap::ShowString ( m_MsgDlgPnt.x,
											m_MsgDlgPnt.y + 20,
											0xffff00,
											GameDataSetQuery()->GetStr ( 10068 ) ) ;
					CMyBitmap::ShowString ( m_MsgDlgPnt.x,
											m_MsgDlgPnt.y + 38,
											0xffff00,
											GameDataSetQuery()->GetStr ( 10069 ) ) ;
				}
				break ;
			case DLG_EMBED:
				{
					OBJID idItem = 0 ;
					int nType = 0 ;
					CItem* pItem = NULL ;
					if ( m_DlgMain.m_DlgEmbed.m_bShow )
					{
						nType = m_DlgMain.m_DlgEmbed.GetMouseMoveId ( &idItem ) ;
						pItem = g_objHero.GetItemByIndex ( idItem ) ;
					}
					else if ( m_DlgMain.m_DlgCompose.m_bShow )
					{
						nType = m_DlgMain.m_DlgCompose.GetMouseMoveId ( idItem ) ;
						pItem = g_objHero.GetItem ( idItem ) ;
					}
					if ( nType == 1 )
					{
						IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
						if ( pAni != NULL && pItem != NULL )
							ShowMessage ( pItem, pAni ) ;
						return ;
					}
					ItemTypeInfo info ;
					if ( !CItem::s_ItemData.GetItemTypeInfo ( idItem, info ) )
						break ;
					int nLine = 1 ;
					int nDescLine = 0 ;
					int nLineFull[10] ;
					nLineFull[0] = 0 ;
					if ( strlen ( info.szDesc ) != 0 )
					{
						int nChar = 0 ;
						for ( UINT i = 0; i < strlen ( info.szDesc ); i++ )
						{
							nChar++ ;
							if ( nChar > 13 )
							{
								nChar = 0 ;
								nDescLine++ ;
								nLine++ ;
								nLineFull[nDescLine] = 1 ;
							}
						}
						if ( nChar < 14 && nChar != 0 )
						{
							nDescLine++ ;
							nLine++ ;
						}
					}
					IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
					if ( pAni != NULL )
					{
						RECT rect = { 0, 0, 100, 200 } ;
						pAni->ShowEx ( 0, m_MsgDlgPnt.x, m_MsgDlgPnt.y, &rect, 100, nLine * 18 ) ;
					}
					CMyBitmap::ShowString ( m_MsgDlgPnt.x, m_MsgDlgPnt.y, 0xffffff, info.szName ) ;
					const char* pszDesc = info.szDesc ;
					int nOffset = 18 ;
					for ( int k = 0; k < nDescLine; k++ )
					{
						char strLine[16] ;
						if ( nLineFull )
						{
							strncpy ( strLine, pszDesc, 14 ) ;
							strLine[14] = 0 ;
							pszDesc += 14 ;
						}
						else
						{
							strncpy ( strLine, pszDesc, 13 ) ;
							strLine[13] = 0 ;
							pszDesc += 13 ;
						}
						CMyBitmap::ShowString ( m_MsgDlgPnt.x, m_MsgDlgPnt.y + nOffset, 0xffffff, strLine ) ;
						nOffset += 18 ;
					}
				}
				break ;
			case DLG_MPCBOOTH:
				{
					int nIndex = m_DlgMain.m_DlgMpcBooth.GetMouseMoveIndex () ;
					if ( nIndex == -1 )
						return ;
					CItem* pItem = g_objHero.m_objBoothManager.GetItemByIndex ( nIndex ) ;
					if ( pItem )
					{
						IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
						if ( pAni != NULL )
							ShowMessage ( pItem, pAni, -1 ) ;
					}
				}
				break ;
			case DLG_NPCBOOTH:
				{
					int nIndex = m_DlgMain.m_DlgNpcBooth.GetMouseMoveIndex () ;
					if ( nIndex == -1 )
						return ;
					CItem* pItem = g_objHero.m_objBooth.GetItemByIndex ( nIndex ) ;
					if ( pItem )
					{
						IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
						if ( pAni != NULL )
							ShowMessage ( pItem, pAni, -1 ) ;
					}
				}
				break ;
			case DLG_NPCEQUIP:
				{
					int nIndex = m_DlgMain.m_DlgNpcEquip.GetMouseMovePosition() ;
					if ( nIndex < 1 )
						return ;
					CItem* pItem = g_objHero.m_objPlayerDummy.GetEquipment(nIndex);
					IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
					if ( pAni != NULL )
						ShowMessage ( pItem, pAni ) ;
				}
				break;
			case DLG_BAG:
				{
					DWORD dwIndex = m_DlgMain.m_DlgBag.m_Grid.GetMouseMoveIndex () ;
					CItem* pItem = g_objHero.GetPackageItemByIndex ( dwIndex ) ;
					IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
					if ( pItem != NULL )
						ShowMessage ( pItem, pAni, 0, false ) ;
				}
				break ;
			case DLG_FUSE:
				{
					int nIndex = m_DlgMain.m_DlgFuse.m_GriLay.GetMouseMoveIndex () ;
					CItem* pItem = g_objHero.GetItem ( m_DlgMain.m_DlgFuse.m_GriLay.GetIconRealID ( nIndex ) ) ;
					IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
					if ( pAni != NULL && pItem != NULL )
						ShowMessage ( pItem, pAni, -1 ) ;
				}
				break ;
			default:
				break ;
			}

		}
		break ;
	case ON_MSGDLG_STATIC:
		{
			char message[256] = "";
			switch ( m_btStaticID )
			{
			case 0:
				g_objGameMap.SetShowExpFlag(true);
				return;
			case 1:
				strcpy ( message, GameDataSetQuery()->GetStr ( 10070 ) ) ;
				break ;
			case 2:
				strcpy ( message, GameDataSetQuery()->GetStr ( 10071 ) ) ;
				break ;
			case 3:
				strcpy ( message, GameDataSetQuery()->GetStr ( 10072 ) ) ;
				break ;
			case 4:
				if ( m_DlgMain.m_ScreenMoveChk.GetCheckValue () )
					strcpy ( message, GameDataSetQuery()->GetStr ( 10074 ) ) ;
				else
					strcpy ( message, GameDataSetQuery()->GetStr ( 10073 ) ) ;
				break ;
			case 5:
				strcpy ( message, GameDataSetQuery()->GetStr ( 10370 ) ) ;
				break ;
			case 6:
				{
					int nUpper, nLower ;
					m_DlgMain.m_XpPrg.GetRange ( nUpper, nLower ) ;
					sprintf ( message, "%d/%d", g_objHero.GetXP (), nLower ) ;
				}
				break ;
			case 7:
				{
					if ( m_DlgMain.m_bNoMana )
						return ;
					int nUpper, nLower ;
					m_DlgMain.m_MpPrg.GetRange ( nUpper, nLower ) ;
					sprintf ( message, "%d/%d", g_objHero.GetMana (), nLower ) ;
				}
				break ;
			case 8:
				{
					int nUpper, nLower ;
					m_DlgMain.m_HpPrg.GetRange ( nUpper, nLower ) ;
					sprintf ( message, "%d/%d", g_objHero.GetData ( CPlayer::_PLAYER_LIFE ), nLower ) ;
				}
				break ;
			default:
				break ;
			}
			IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
			if ( pAni != NULL )
			{
				RECT rect = { 0, 0, 100, 200 } ;
				pAni->ShowEx ( 0, m_StaticPoint[m_btStaticID].x, m_StaticPoint[m_btStaticID].y, &rect, strlen ( message ) * 7, 14 ) ;
			}
			CMyBitmap::ShowString ( m_StaticPoint[m_btStaticID].x,
									m_StaticPoint[m_btStaticID].y,
									0xffffff,
									message) ;
		}
		break ;
	case ON_MSGDLG_IMAGE:
		{
			if ( m_dwMsgDlgOwner == 0 )
			{
				const char* pszText = NULL ;
				int nWidth = 28 ;
				int nPosX = m_MsgDlgPnt.x ;
				if ( m_DlgMain.m_DlgXp.m_bDie )
				{
					if ( m_MsgDlgPnt.x >= CMyBitmap::GetScreenWidth () - 50 )
						pszText = GameDataSetQuery()->GetStr ( 10486 ) ;
					else
						pszText = GameDataSetQuery()->GetStr ( 10355 ) ;
					if ( pszText )
					{
						int nLen = strlen ( pszText ) ;
						IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
						if ( pAni != NULL )
						{
							RECT rect = { 0, 0, 100, 200 } ;
							pAni->ShowEx ( 0, m_MsgDlgPnt.x, m_MsgDlgPnt.y - 60, &rect, nLen * 7, 14 ) ;
						}
						CMyBitmap::ShowString ( m_MsgDlgPnt.x, m_MsgDlgPnt.y - 60, 0xffffff, pszText ) ;
					}
					return ;
				}
				else if ( m_DlgMain.m_DlgXp.m_btBoothType == 2 )
				{
					IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
					if ( pAni != NULL )
					{
						RECT rect = { 0, 0, 100, 200 } ;
						pAni->ShowEx ( 0, m_MsgDlgPnt.x, m_MsgDlgPnt.y - 60, &rect, 28, 14 ) ;
					}
					if ( !m_DlgMain.m_DlgXp.m_bTransform || m_MsgDlgPnt.x != 750 )
					{
						CMyBitmap::ShowString ( m_MsgDlgPnt.x, m_MsgDlgPnt.y - 60, 0xffffff, GameDataSetQuery()->GetStr ( 10357 ) ) ;
						return ;
					}
					CMyBitmap::ShowString ( m_MsgDlgPnt.x, m_MsgDlgPnt.y - 60, 0xffffff, GameDataSetQuery()->GetStr ( 10379 ) ) ;
					return ;
				}
				else if ( !m_DlgMain.m_DlgXp.m_bTransform )
				{
					if ( !m_DlgMain.m_DlgXp.m_bFly )
						return ;
					pszText = GameDataSetQuery()->GetStr ( 10396 ) ;
				}
				else
				{
					pszText = GameDataSetQuery()->GetStr ( 10379 ) ;
					nWidth = 56 ;
					nPosX = m_MsgDlgPnt.x - 4 ;
				}
				IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
				if ( pAni != NULL )
				{
					RECT rect = { 0, 0, 100, 200 } ;
					pAni->ShowEx ( 0, nPosX, m_MsgDlgPnt.y - 60, &rect, nWidth, 14 ) ;
				}
				CMyBitmap::ShowString ( m_MsgDlgPnt.x, m_MsgDlgPnt.y - 60, 0xffffff, pszText ) ;
				break ;
			}
			CMagic* pMagic = g_objHero.GetMagic ( m_dwMsgDlgOwner ) ;
			if ( pMagic )
			{
				IAni* pAni = GameDataSetQuery()->GetDataAni ( g_strControlAni, "Dialog21", EXIGENCE_IMMEDIATE ) ;
				if ( pAni != NULL )
				{
					RECT rect = { 0, 0, 100, 200 } ;
					pAni->ShowEx ( 0, m_MsgDlgPnt.x, m_MsgDlgPnt.y - 60, &rect, strlen ( pMagic->GetName () ) * 7, 14 ) ;
				}
				CMyBitmap::ShowString ( m_MsgDlgPnt.x,
										m_MsgDlgPnt.y - 60,
										0xffffff,
										pMagic->GetName () ) ;
			}
			break;
		}
	default:
		break;
	}
}

#define SFK_SHOW_TIP_LINE( color )	\
	{	\
		CMyBitmap::ShowString ( m_MsgDlgPnt.x, m_MsgDlgPnt.y + nOffset, color, strMsg ) ;	\
		nOffset += 18 ;	\
		if ( m_nMsgDlgWidth <= ( int )strlen ( strMsg ) )	\
			m_nMsgDlgWidth = strlen ( strMsg ) ;	\
	}

void CMyShellDlg::ShowMessage(CItem *pItem, IAni* pAni, int nItemNum, BOOL bShowDur)
{
	if (pItem == NULL)
		return;

	int nMsgLine = 0 ;
	if ( pItem->GetName () )
		nMsgLine++ ;
	if ( pItem->GetProfessionRequired () )
		nMsgLine++ ;
	if ( pItem->GetWeaponSkillRequired () )
		nMsgLine++ ;
	if ( pItem->GetLevelRequired () )
		nMsgLine++ ;
	if ( pItem->GetSexRequried () )
		nMsgLine++ ;
	if ( pItem->GetForceRequired () )
		nMsgLine++ ;
	if ( pItem->GetSpeedRequired () )
		nMsgLine++ ;
	if ( pItem->GetHealthRequired () )
		nMsgLine++ ;
	if ( pItem->GetSoulRequired () )
		nMsgLine++ ;
	if ( pItem->GetMonopoly () )
		nMsgLine++ ;
	if ( pItem->GetWeight () )
		nMsgLine++ ;
	if ( pItem->GetMaxAttack () || pItem->GetMinAttack () )
		nMsgLine++ ;
	if ( pItem->GetDefense () )
		nMsgLine++ ;
	if ( pItem->GetMaxMagicAttack () )
		nMsgLine++ ;
	if ( pItem->GetMagicDefence () )
		nMsgLine++ ;
	if ( pItem->GetDexterity () )
		nMsgLine++ ;
	if ( pItem->GetDodge () )
		nMsgLine++ ;
	if ( pItem->GetLife () )
		nMsgLine++ ;
	if ( pItem->GetMana () )
		nMsgLine++ ;
	if ( pItem->GetGem1 () || pItem->GetGem2 () )
		nMsgLine++ ;
	if ( pItem->GetGem1 () != 0 && pItem->GetGem1 () != 0xFF )
		nMsgLine += 2 ;
	if ( pItem->GetGem2 () != 0 && pItem->GetGem2 () != 0xFF )
		nMsgLine += 2 ;
	if ( pItem->GetStatus () )
		nMsgLine++ ;
	if ( pItem->GetAttackSpeed () && pItem->GetTypeID () > 400000 && pItem->GetTypeID () < 600000 )
		nMsgLine++ ;
	if ( nItemNum > 0 )
		nMsgLine++ ;
	if ( pItem->GetAmountLimit () && bShowDur )
		nMsgLine++ ;
	if ( pItem->GetAdditionDefense () )
		nMsgLine++ ;
	if ( pItem->GetAdditionDexterity () )
		nMsgLine++ ;
	if ( pItem->GetAdditionDodge () )
		nMsgLine++ ;
	if ( pItem->GetAdditionLife () )
		nMsgLine++ ;
	if ( pItem->GetAdditionMAttack () )
		nMsgLine++ ;
	if ( pItem->GetAdditionMDef () )
		nMsgLine++ ;
	if ( pItem->GetAdditionAttackMax () )
		nMsgLine++ ;
	if ( pItem->GetBless () )
		nMsgLine++ ;
	if ( pItem->GetEnchant () )
		nMsgLine++ ;
	int nRestrainWidth = 18 ;
	DWORD dwRestrain = pItem->GetRestrain () ;
	if ( dwRestrain )
	{
		CONST RestrainInfo* pInfo = RoleDataQuery()->GetRestrainInfo ( dwRestrain ) ;
		if ( pInfo )
		{
			int nLen = pInfo->strName.size () ;
			nMsgLine += nLen / 18 ;
			if ( nLen % 18 != 0 )
				nMsgLine++ ;
		}
	}

	int nDescLine = 0 ;
	int nCharIndex[10] ;
	nCharIndex[0] = 0 ;
	if ( strcmp ( GameDataSetQuery()->GetStr ( 10075 ), pItem->GetDesc () ) != 0 )
	{
		int nSingle = 0 ;
		int nLineNum = 0 ;
		UINT i ;
		for ( i = 0; i < strlen ( pItem->GetDesc () ); i++ )
		{
			if ( CMyBitmap::IsDBCSLeadByte( pItem->GetDesc(), i ) )
			{
				nLineNum += 2;
				i++;
			}
			else
			{
				nSingle++;
				nLineNum++;
			}
			if ( nLineNum > 13 )
			{
				if ( strcmp( GameDataSetQuery()->GetVersionInfo (), "English" ) == 0 )
				{
					int j ;
					for ( j = i; j >= 0; j-- )
					{
						if ( pItem->GetDesc()[j] == '~' )
							break;
					}
					if ( j >= 0 )
						i = j ;
				}
				if ( nSingle % 2 == 0 )
				{
					nCharIndex[nDescLine + 1] = i + 1;
				}
				else
				{
					nLineNum = i - nLineNum ;
					i = nLineNum + 13 ;
					nCharIndex[nDescLine + 1] = nLineNum + 14 ;
				}
				nDescLine++ ;
				nMsgLine++ ;
				nSingle = 0;
				nLineNum = 0;
			}
		}
		if ( nLineNum < 14 && nLineNum != 0 )
		{
			nDescLine++ ;
			nMsgLine++ ;
			nCharIndex[nDescLine] = i ;
		}
	}
	if ( m_DlgMain.m_DlgNpcBuy.m_bShow || m_DlgMain.m_DlgTrade.m_bShow )
	{
		if ( pItem->GetPrice () >= 0 )
			nMsgLine++ ;
	}
	if ( nItemNum == -1 && m_bIconFix && pItem->GetSort () != ITEMSORT_EXPEND )
		nMsgLine++ ;

	if ( m_nMsgDlgWidth == 0 )
		m_nMsgDlgWidth = strlen ( pItem->GetName () ) ;
	CMyShellDlg::SetMsgDlgPnt ( nMsgLine, m_nMsgDlgWidth * CMyBitmap::GetFontSize () / 2 ) ;
	IAni* pIcon = RoleDataQuery()->GetItemMinIcon ( pItem->GetTypeID (), EXIGENCE_NORMAL ) ;
	int nHeight = 0 ;
	CMySize sizeIcon = { 0, 0 } ;
	if ( pIcon )
	{
		sizeIcon = pIcon->GetSize ( 0 ) ;
		nHeight = sizeIcon.iHeight ;
	}
	if ( nHeight < nMsgLine * 18 )
		nHeight = nMsgLine * 18 ;
	int nPosX = m_MsgDlgPnt.x - sizeIcon.iWidth - 5 ;
	if ( nPosX < 0 )
		m_MsgDlgPnt.x = sizeIcon.iWidth + 5 ;
	if ( nHeight + m_MsgDlgPnt.y > CMyBitmap::GetScreenHeight () )
		m_MsgDlgPnt.y = CMyBitmap::GetScreenHeight () - nHeight ;
	if ( pAni == NULL )
		return ;
	pAni->ShowEx ( 0, m_MsgDlgPnt.x - sizeIcon.iWidth - 5, m_MsgDlgPnt.y, &CRect ( 0, 0, 100, 200 ),
		m_nMsgDlgWidth * CMyBitmap::GetFontSize () / 2 + sizeIcon.iWidth + 5, nHeight ) ;
	CMyBitmap::ShowRect ( m_MsgDlgPnt.x - sizeIcon.iWidth - 6, m_MsgDlgPnt.y - 1,
		m_nMsgDlgWidth * CMyBitmap::GetFontSize () / 2 + sizeIcon.iWidth + ( m_MsgDlgPnt.x - sizeIcon.iWidth ) + 1, m_MsgDlgPnt.y + 1 + nHeight, 0xffcccccc ) ;
	if ( pIcon )
		pIcon->Show ( 0, m_MsgDlgPnt.x - sizeIcon.iWidth - 5, m_MsgDlgPnt.y ) ;

	char strMsg[128] = "" ;
	DWORD dwColor = 0xffffff ;
	OBJID idType = pItem->GetTypeID () ;
	if ( idType / 100000 < 10 && ( idType < 700000 || idType > 800000 ) )
	{
		switch ( idType % 10 )
		{
		case 6:
			strcpy ( strMsg, GameDataSetQuery()->GetStr ( 10077 ) ) ;
			dwColor = 0x5e87ca ;
			break ;
		case 7:
			strcpy ( strMsg, GameDataSetQuery()->GetStr ( 10078 ) ) ;
			dwColor = 0x00ff00 ;
			break ;
		case 8:
			strcpy ( strMsg, GameDataSetQuery()->GetStr ( 10079 ) ) ;
			dwColor = 0xe804fd ;
			break ;
		case 9:
			strcpy ( strMsg, GameDataSetQuery()->GetStr ( 10080 ) ) ;
			dwColor = 0xebe57f ;
			break ;
		}
		strcat ( strMsg, pItem->GetName () ) ;
	}
	else
		strcpy ( strMsg, pItem->GetName () ) ;
	if ( pItem->GetAddition () )
		sprintf ( strMsg, "%s(+%d)", strMsg, pItem->GetAddition () ) ;
	sprintf ( strMsg, "%s (%s)", strMsg, pItem->GetShowName () ) ;
	int nOffset = 0 ;
	SFK_SHOW_TIP_LINE ( dwColor ) ;

	if ( pItem->GetProfessionRequired () )
	{
		strcpy ( strMsg, GameDataSetQuery()->GetStr ( 10081 ) ) ;
		strcat ( strMsg, g_objHero.GetProfessionalName ( pItem->GetProfessionRequired () ) ) ;
		BOOL bMatch = true ;
		DWORD dwRequired = ( pItem->GetProfessionRequired () % 1000 ) / 10 ;
		if ( dwRequired == 19 )
		{
			if ( ( g_objHero.GetProfession () % 1000 ) / 10 < 10 )
				bMatch = false ;
		}
		else if ( dwRequired != ( g_objHero.GetProfession () % 1000 ) / 10 )
			bMatch = false ;
		if ( ( int )( g_objHero.GetProfession () % 10 ) < ( int )( pItem->GetProfessionRequired () % 10 ) || !bMatch )
			dwColor = 0xff0000 ;
		else
			dwColor = 0xffffff ;
		SFK_SHOW_TIP_LINE ( dwColor ) ;
	}
	if ( pItem->GetWeaponSkillRequired () )
	{
		strcpy ( strMsg, GameDataSetQuery()->GetStr ( 10082 ) ) ;
		strcat ( strMsg, CWeaponSkill::s_Data.GetWeaponSkillName ( pItem->GetWeaponSkillType () ) ) ;
		char strLevel[16] = "" ;
		sprintf ( strLevel, GameDataSetQuery()->GetStr ( 10083 ), pItem->GetWeaponSkillRequired () ) ;
		strcat ( strMsg, strLevel ) ;
		CWeaponSkill* pSkill = g_objHero.GetWeaponSkill ( pItem->GetWeaponSkillType () ) ;
		if ( pSkill == NULL || pSkill->GetLevel () < pItem->GetWeaponSkillRequired () )
			dwColor = 0xff0000 ;
		else
			dwColor = 0xffffff ;
		SFK_SHOW_TIP_LINE ( dwColor ) ;
	}
	if ( pItem->GetLevelRequired () )
	{
		sprintf( strMsg, GameDataSetQuery()->GetStr( 10084 ), pItem->GetLevelRequired () ) ;
		if ( g_objHero.GetLev () < pItem->GetLevelRequired () )
			dwColor = 0xff0000 ;
		else
			dwColor = 0xffffff ;
		SFK_SHOW_TIP_LINE ( dwColor ) ;
	}
	if ( pItem->GetSexRequried () )
	{
		const char* pszSex ;
		if ( pItem->GetSexRequried () == 1 )
			pszSex = GameDataSetQuery()->GetStr ( 10487 ) ;
		else
			pszSex = GameDataSetQuery()->GetStr ( 10488 ) ;
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10085 ), pszSex ) ;
		dwColor = 0xffffffff ;
		if ( pItem->GetSexRequried () != g_objHero.GetSex () )
			dwColor = 0x0ff00000 ;
		SFK_SHOW_TIP_LINE ( dwColor ) ;
	}
	if ( pItem->GetForceRequired () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10086 ), pItem->GetForceRequired () ) ;
		if ( g_objHero.GetForce () < pItem->GetForceRequired () )
			dwColor = 0xff0000 ;
		else
			dwColor = 0xffffff ;
		SFK_SHOW_TIP_LINE ( dwColor ) ;
	}
	if ( pItem->GetSpeedRequired () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10087 ), pItem->GetSpeedRequired () ) ;
		if ( g_objHero.GetSpeed () < pItem->GetSpeedRequired () )
			dwColor = 0xff0000 ;
		else
			dwColor = 0xffffff ;
		SFK_SHOW_TIP_LINE ( dwColor ) ;
	}
	if ( pItem->GetHealthRequired () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10088 ), pItem->GetHealthRequired () ) ;
		if ( g_objHero.GetHealth () < pItem->GetHealthRequired () )
			dwColor = 0xff0000 ;
		else
			dwColor = 0xffffff ;
		SFK_SHOW_TIP_LINE ( dwColor ) ;
	}
	if ( pItem->GetSoulRequired () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10089 ), pItem->GetSoulRequired () ) ;
		if ( g_objHero.GetSoul () < pItem->GetSoulRequired () )
			dwColor = 0xff0000 ;
		else
			dwColor = 0xffffff ;
		SFK_SHOW_TIP_LINE ( dwColor ) ;
	}
	if ( pItem->GetMaxAttack () || pItem->GetMinAttack () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10090 ), pItem->GetMinAttack (), pItem->GetMaxAttack () ) ;
		SFK_SHOW_TIP_LINE ( 0xffffff ) ;
	}
	if ( pItem->GetDefense () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10091 ), pItem->GetDefense () ) ;
		SFK_SHOW_TIP_LINE ( 0xffffff ) ;
	}
	if ( pItem->GetMaxMagicAttack () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10092 ), pItem->GetMaxMagicAttack () ) ;
		SFK_SHOW_TIP_LINE ( 0x8ccefd ) ;
	}
	if ( pItem->GetMagicDefence () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10093 ), pItem->GetMagicDefence () ) ;
		SFK_SHOW_TIP_LINE ( 0x8ccefd ) ;
	}
	if ( pItem->GetDexterity () )
	{
		if ( pItem->GetDexterity () > 0 )
			sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10094 ), pItem->GetDexterity () ) ;
		else
			sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10095 ), -pItem->GetDexterity () ) ;
		SFK_SHOW_TIP_LINE ( 0xffffff ) ;
	}
	if ( pItem->GetDodge () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10096 ), pItem->GetDodge () ) ;
		SFK_SHOW_TIP_LINE ( 0xffffff ) ;
	}
	if ( pItem->GetLife () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10097 ), pItem->GetLife () ) ;
		SFK_SHOW_TIP_LINE ( 0xffffff ) ;
	}
	if ( pItem->GetMana () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10098 ), pItem->GetMana () ) ;
		SFK_SHOW_TIP_LINE ( 0xffffff ) ;
	}
	if ( pItem->GetGem1 () || pItem->GetGem2 () )
	{
		int nGemNum = 0 ;
		if ( pItem->GetGem1 () )
			nGemNum++ ;
		if ( pItem->GetGem2 () )
			nGemNum++ ;
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10099 ), nGemNum ) ;
		SFK_SHOW_TIP_LINE ( 0xffffff ) ;
		ItemTypeInfo info ;
		if ( pItem->GetGem1 () != 0 && pItem->GetGem1 () != 0xFF
			&& CItem::s_ItemData.GetItemTypeInfo ( pItem->GetGem1 () + 700000, info ) )
		{
			sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10118 ), 1, info.szName ) ;
			SFK_SHOW_TIP_LINE ( 0x7df983 ) ;
			const GemInfo& gem = CMyShellApp::m_GemInfo[ ( pItem->GetGem1 () / 10 ) * 3 + pItem->GetGem1 () % 10 - 1 ] ;
			CMyBitmap::ShowString ( m_MsgDlgPnt.x, m_MsgDlgPnt.y + nOffset, 0x7df983, GameDataSetQuery()->GetStr ( gem.strID ) ) ;
			nOffset += 18 ;
			if ( m_nMsgDlgWidth <= ( int )strlen ( strMsg ) )
				m_nMsgDlgWidth = strlen ( strMsg ) ;
		}
		if ( pItem->GetGem2 () != 0 && pItem->GetGem2 () != 0xFF
			&& CItem::s_ItemData.GetItemTypeInfo ( pItem->GetGem2 () + 700000, info ) )
		{
			sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10118 ), 2, info.szName ) ;
			SFK_SHOW_TIP_LINE ( 0x7df983 ) ;
			const GemInfo& gem = CMyShellApp::m_GemInfo[ ( pItem->GetGem2 () / 10 ) * 3 + pItem->GetGem2 () % 10 - 1 ] ;
			CMyBitmap::ShowString ( m_MsgDlgPnt.x, m_MsgDlgPnt.y + nOffset, 0x7df983, GameDataSetQuery()->GetStr ( gem.strID ) ) ;
			nOffset += 18 ;
			if ( m_nMsgDlgWidth <= ( int )strlen ( strMsg ) )
				m_nMsgDlgWidth = strlen ( strMsg ) ;
		}
	}
	if ( pItem->GetAttackSpeed () && pItem->GetTypeID () > 400000 && pItem->GetTypeID () < 600000 )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10402 ), 100000 / pItem->GetAttackSpeed () ) ;
		SFK_SHOW_TIP_LINE ( 0xffffff ) ;
	}
	if ( pItem->GetWeight () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10100 ), pItem->GetWeight () ) ;
		SFK_SHOW_TIP_LINE ( 0xffffff ) ;
	}
	if ( pItem->GetAmountLimit () && bShowDur )
	{
		if ( pItem->GetSort () == ITEMSORT_EXPEND )
			sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10391 ), pItem->GetAmount () ) ;
		else
			sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10101 ), pItem->GetAmount () / 100, pItem->GetAmountLimit () / 100 ) ;
		SFK_SHOW_TIP_LINE ( 0xffffff ) ;
	}
	if ( pItem->GetAdditionDefense () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10421 ), pItem->GetAdditionDefense () ) ;
		SFK_SHOW_TIP_LINE ( 0xc501e7 ) ;
	}
	if ( pItem->GetAdditionDexterity () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10424 ), pItem->GetAdditionDexterity () ) ;
		SFK_SHOW_TIP_LINE ( 0xc501e7 ) ;
	}
	if ( pItem->GetAdditionDodge () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10425 ), pItem->GetAdditionDodge () ) ;
		SFK_SHOW_TIP_LINE ( 0xc501e7 ) ;
	}
	if ( pItem->GetAdditionMAttack () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10422 ), pItem->GetAdditionMAttack () ) ;
		SFK_SHOW_TIP_LINE ( 0xc501e7 ) ;
	}
	if ( pItem->GetAdditionMDef () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10423 ), pItem->GetAdditionMDef () ) ;
		SFK_SHOW_TIP_LINE ( 0xc501e7 ) ;
	}
	if ( pItem->GetAdditionAttackMax () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10420 ), pItem->GetAdditionAttackMax () ) ;
		SFK_SHOW_TIP_LINE ( 0xc501e7 ) ;
	}
	if ( pItem->GetAdditionLife () )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10430 ), pItem->GetAdditionLife () ) ;
		SFK_SHOW_TIP_LINE ( 0xc501e7 ) ;
	}
	if ( nItemNum > 0 )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10105 ), nItemNum ) ;
		if ( nItemNum > 3 )
			dwColor = 0xffffff ;
		else
			dwColor = 0xff0000 ;
		SFK_SHOW_TIP_LINE ( dwColor ) ;
	}
	if ( pItem->GetBless () > 0 )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10465 ), pItem->GetBless () ) ;
		SFK_SHOW_TIP_LINE ( 0xf87409 ) ;
	}
	if ( pItem->GetEnchant () > 0 )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10466 ), pItem->GetEnchant () ) ;
		SFK_SHOW_TIP_LINE ( 0xf3e250 ) ;
	}
	if ( dwRestrain )
	{
		CONST RestrainInfo* pInfo = RoleDataQuery()->GetRestrainInfo ( dwRestrain ) ;
		if ( pInfo )
		{
			int nLen = pInfo->strName.size () ;
			int nLines = nLen / 18 ;
			if ( nLen % 18 != 0 )
				nLines++ ;
			for ( int i = 0; i < nLines; i++ )
			{
				memset ( strMsg, 0, 128 ) ;
				strncpy ( strMsg, pInfo->strName.c_str () + i * 18, 18 ) ;
				SFK_SHOW_TIP_LINE ( 0x0bea5f ) ;
			}
		}
	}
	if ( m_DlgMain.m_DlgNpcBuy.m_bShow || m_DlgMain.m_DlgTrade.m_bShow )
	{
		if ( nItemNum == 0 )
		{
			if ( !g_objShop.GetMoneyType () )
				sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10102 ), pItem->GetPrice () );
			else
				sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10481 ), pItem->GetEmoneyPrice () );
		}
		else if (nItemNum == -1 || nItemNum > 0)
		{
			sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10103 ), pItem->GetSellPrice () );
		}
		SFK_SHOW_TIP_LINE ( 0xffff00 ) ;
	}
	if ( nItemNum == -1 && m_bIconFix && pItem->GetSort() != ITEMSORT_EXPEND )
	{
		sprintf ( strMsg, GameDataSetQuery()->GetStr ( 10104 ), pItem->GetRepairCost () );
		SFK_SHOW_TIP_LINE ( 0xe8a900 ) ;
	}
	if ( strcmp ( GameDataSetQuery()->GetStr ( 10075 ), pItem->GetDesc () ) != 0 )
	{
		for ( int i = 0; i < nDescLine; i++ )
		{
			char strTemp[16] ;
			strncpy ( strTemp, pItem->GetDesc () + nCharIndex[i], nCharIndex[i + 1] - nCharIndex[i] );
			strTemp[nCharIndex[i + 1] - nCharIndex[i]] = '\0';
			CMyBitmap::ShowString ( m_MsgDlgPnt.x, m_MsgDlgPnt.y + nOffset, 0xffffff, CMyShellApp::StringChange ( strTemp, 1 ) ) ;
			nOffset += 18 ;
			if ( m_nMsgDlgWidth <= ( int )strlen ( strTemp ) )
				m_nMsgDlgWidth = strlen ( strTemp ) ;
		}
	}
}

void CMyShellDlg::OnIconPickUp(LPARAM lParam)
{
	m_PickUpDlgID = lParam ;
	switch ( lParam )
	{
	case DLG_MAIN:
		m_IconMousePnt = m_DlgMain.m_GoodBox.GetPickUpMousePnt ( ) ;
		m_IconTypeID = m_DlgMain.m_GoodBox.GetPickUpTypeID () ;
		m_IconIndex = m_DlgMain.m_GoodBox.GetPickUpIndex () ;
		m_btIconType = m_DlgMain.m_GoodBox.GetPickUpType () ;
		m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( true ) ;
		m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( true ) ;
		m_DlgMain.m_DlgTrade.m_TradeGriMpc.SetPickUp ( true ) ;
		m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( true ) ;
		m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( true ) ;
		m_bPickUp = true ;
		break ;
	case DLG_QUERY_MISSION:
		m_IconTypeID = m_DlgMain.m_DlgQuery.m_DlgMission.m_bShow ;
		m_btIconType = ICON_TYPE_ITEM ;
		m_DlgMain.m_GoodBox.SetPickUp ( true ) ;
		m_bPickUp = true ;
		break ;
	case DLG_NPCBUY:
		m_IconMousePnt = m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.GetPickUpMousePnt () ;
		m_IconTypeID = m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.GetPickUpTypeID () ;
		m_IconIndex =  m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.GetPickUpIndex () ;
		m_btIconType = m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.GetPickUpType () ;
		m_DlgMain.m_GoodBox.SetPickUp ( true ) ;
		m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( true ) ;
		m_DlgMain.m_DlgTrade.m_TradeGriMpc.SetPickUp ( true ) ;
		m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( true ) ;
		m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( true ) ;
		m_bPickUp = true ;
		break ;
	case DLG_MPCGOODS:
		if ( m_bIconFix )
		{
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.ReSumePickUp();
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
			CItem* RepairItem = g_objHero.GetItemByIndex ( m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.GetMouseMoveIndex () ) ;
			if ( RepairItem != NULL )
			{
				DXPlaySound ( "Sound/Fix.wav" ) ;
				g_objHero.RepairItem ( RepairItem->GetID () ) ;
			}
			m_bIconFix = false ;
			break ;
		}

		m_IconMousePnt = m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.GetPickUpMousePnt () ;
		m_IconTypeID = m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.GetPickUpTypeID () ;
		m_IconIndex = m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.GetPickUpIndex () ;
		m_IconRealID = m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.GetIconRealID ( m_IconIndex ) ;
		m_btIconType = m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.GetPickUpType () ;
		m_btIconActType = m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.GetPickUpActType () ;
		m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( true ) ;
		m_DlgMain.m_GoodBox.SetPickUp ( true ) ;
		m_DlgMain.m_DlgTrade.m_TradeGriMpc.SetPickUp ( true ) ;
		m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( true ) ;
		m_DlgMain.m_DlgFuse.m_GriLay.SetPickUp ( true ) ;
		m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( true ) ;
		m_DlgMain.m_DlgQuery.m_DlgMission.SetPickUpState ( true, m_IconIndex ) ;
		m_DlgMain.m_DlgImprove.SetPickUpState ( true, m_IconIndex ) ;
		m_DlgMain.m_DlgEnchant.SetPickUpState ( true, m_IconIndex ) ;
		m_DlgMain.m_DlgEmbed.SetPickUpState ( true, m_IconIndex ) ;
		m_DlgMain.m_DlgCompose.SetPickUpState ( true, m_IconIndex ) ;
		m_DlgMain.m_DlgMpcBooth.SetPickUpState ( true, m_IconIndex ) ;
		m_bPickUp = true ;
		break ;
	case DLG_DEPOT:
		{
			if ( g_objHero.GetItemAmount () > 39 )
			{
				g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10106 ) ) ;
				m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( false ) ;
				m_DlgMain.m_DlgDepot.m_DepotGriLay.ReSumePickUp () ;
				return ;
			}
			m_IconIndex = m_DlgMain.m_DlgDepot.m_DepotGriLay.GetPickUpIndex () ;
			m_IconIndex = m_DlgMain.m_DlgDepot.m_nCurPage * 20 + m_IconIndex ;
			CItem* pOutItem = g_objHero.GetPackageItemByIndex ( m_IconIndex ) ;
			if ( pOutItem != NULL )
				g_objHero.CheckOutItem ( pOutItem->GetID () ) ;
			m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( false ) ;
		}
		break ;
	case DLG_ACT:
		m_btIconType = ICON_TYPE_ACTION ;
		m_bPickUp = true ;
		break ;
	case DLG_MAGIC:
		m_btIconType = ICON_TYPE_SKILL ;
		m_bPickUp = true ;
		break ;
	case DLG_XP:
		m_btIconType = ICON_TYPE_XP ;
		m_bPickUp = true ;
		break ;
	case DLG_BAG:
		{
			if ( g_objHero.GetItemAmount () > 39 )
			{
				g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10106 ) ) ;
				m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( false ) ;
				m_DlgMain.m_DlgBag.m_Grid.ReSumePickUp () ;
				return ;
			}
			m_IconIndex = m_DlgMain.m_DlgBag.m_Grid.GetPickUpIndex () ;
			CItem* pOutItem = g_objHero.GetPackageItemByIndex ( m_IconIndex ) ;
			if ( pOutItem != NULL )
				g_objHero.CheckOutItem ( pOutItem->GetID () ) ;
			m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( false ) ;
		}
		break ;
	case DLG_FUSE:
		if ( !m_DlgMain.m_DlgFuse.m_ChkOk.GetCheckValue () )
		{
			OBJID idItem = m_DlgMain.m_DlgFuse.m_GriLay.GetIconRealID ( m_DlgMain.m_DlgFuse.m_GriLay.GetPickUpIndex () ) ;
			m_DlgMain.m_DlgFuse.DelItem ( idItem ) ;
			m_DlgMain.m_DlgFuse.m_GriLay.SetPickUp ( false ) ;
		}
		else
		{
			m_DlgMain.m_DlgFuse.m_GriLay.ReSumePickUp () ;
			g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10451 ) ) ;
			m_DlgMain.m_DlgFuse.m_GriLay.SetPickUp ( false ) ;
		}
		break ;
	}
	CRect rect ;
	CMyShellDlg::GetWindowRect ( rect ) ;
	switch ( lParam )
	{
	case DLG_ACT:
		m_IconShowPnt.x = m_IconMousePnt.x - rect.left - 16;
		m_IconShowPnt.y = m_IconMousePnt.y - rect.top - 16;
		break ;
	case DLG_MAGIC:
		m_IconShowPnt.x = m_IconMousePnt.x - rect.left - 25;
		m_IconShowPnt.y = m_IconMousePnt.y - rect.top - 25;
		break ;
	default:
		m_IconShowPnt.x = m_IconMousePnt.x - rect.left - 32;
		m_IconShowPnt.y = m_IconMousePnt.y - rect.top - 32;
		break ;
	}
}

void CMyShellDlg::ShowPickUpIcon()
{
	if (!m_bPickUp)
		return;

	POINT mousePnt ;
	GetCursorPos ( &mousePnt ) ;
	IAni* ShowAni = NULL ;
	char strName[32] ;
	switch ( m_btIconType )
	{
	case ICON_TYPE_ITEM:
		ShowAni = RoleDataQuery()->GetItemMinIcon ( m_IconTypeID, EXIGENCE_NORMAL ) ;
		break ;
	case ICON_TYPE_ACTION:
		sprintf ( strName, "ButtonA%d", m_IconTypeID ) ;
		ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
												strName,
												EXIGENCE_IMMEDIATE ) ;
		break ;
	case ICON_TYPE_SKILL:
		sprintf ( strName, "MagicSkillType%d", m_IconTypeID ) ;
		ShowAni = GameDataSetQuery()->GetDataAni ( g_strMagicAni,
												strName,
												EXIGENCE_IMMEDIATE ) ;
		break ;
	case ICON_TYPE_XP:
		sprintf ( strName, "XpSkillType%d", m_IconTypeID ) ;
		ShowAni = GameDataSetQuery()->GetDataAni ( g_strMagicAni,
												strName,
												EXIGENCE_IMMEDIATE ) ;
		break;
	default:
		return ;
	}
	if ( ShowAni != NULL )
		ShowAni->Show ( 0,
						m_IconShowPnt.x + ( mousePnt.x - m_IconMousePnt.x ),
						m_IconShowPnt.y + ( mousePnt.y - m_IconMousePnt.y ) ) ;
}

static BOOL IsImportantItem ( CItem* pItem )
{
	OBJID idType = pItem->GetTypeID () ;
	if ( idType >= 700000 && idType <= 700100 && idType % 10 >= 2 )
		return true ;
	if ( idType / 100000 < 10 && idType % 10 >= 7 )
		return true ;
	if ( pItem->GetSort () == 9 && pItem->GetSubType () == 10000 )
		return true ;
	if ( idType == 1088002 || idType == 1088001 || idType == 1088000
		|| idType == 721509 || idType == 721510 || idType == 721511
		|| idType == 1100003 || idType == 1100006 || idType == 1100009 )
		return true ;
	if ( pItem->GetAddition () != 0 )
		return true ;
	if ( pItem->GetBless () > 0 || pItem->GetEnchant () > 0 || pItem->GetRestrain () > 0 )
		return true ;
	return false ;
}

#define SFK_SET_PICKUP_STATE( bPickUp, nIndex )	\
	{	\
		m_DlgMain.m_DlgQuery.m_DlgMission.SetPickUpState ( bPickUp, nIndex ) ;	\
		m_DlgMain.m_DlgImprove.SetPickUpState ( bPickUp, nIndex ) ;	\
		m_DlgMain.m_DlgEnchant.SetPickUpState ( bPickUp, nIndex ) ;	\
		m_DlgMain.m_DlgEmbed.SetPickUpState ( bPickUp, nIndex ) ;	\
		m_DlgMain.m_DlgCompose.SetPickUpState ( bPickUp, nIndex ) ;	\
		m_DlgMain.m_DlgMpcBooth.SetPickUpState ( bPickUp, nIndex ) ;	\
	}

void CMyShellDlg::OnIconDrop( LPARAM lParam )
{
	CPoint InsertLandR ;
	switch ( lParam )
	{
	case DLG_MAIN:
		if ( m_PickUpDlgID == DLG_MPCGOODS )
		{
			m_bPickUp = false ;
			InsertLandR = m_DlgMain.m_GoodBox.GetInsertLandR () ;
			m_DlgMain.m_GoodBox.InsertIcon ( InsertLandR.y,
											 InsertLandR.x,
											 m_IconTypeID,
											 m_IconIndex,
											 ICON_TYPE_ITEM ) ;
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.ReSumePickUp () ;
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
			m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( false ) ;
			m_DlgMain.m_DlgTrade.m_TradeGriMpc.SetPickUp ( false ) ;
			m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( false ) ;
			m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( false ) ;
			SFK_SET_PICKUP_STATE ( false, 0 ) ;
			CMyShellDlg::SetQUseItemNum ( ) ;
		}
		else if ( m_PickUpDlgID == DLG_MAIN )
		{
			m_bPickUp = false ;
			InsertLandR = m_DlgMain.m_GoodBox.GetInsertLandR () ;
			m_DlgMain.m_GoodBox.InsertIcon ( InsertLandR.y,
											 InsertLandR.x,
											 m_IconTypeID,
											 m_IconIndex,
											 ICON_TYPE_ITEM ) ;
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
			m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( false ) ;
			m_DlgMain.m_DlgTrade.m_TradeGriMpc.SetPickUp ( false ) ;
			m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( false ) ;
			m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( false ) ;
			CMyShellDlg::SetQUseItemNum ( ) ;
		}
		break ;
	case DLG_NPCBUY:
		m_bPickUp = false ;
		if ( m_PickUpDlgID == DLG_MPCGOODS )
		{
			DXPlaySound ( "Sound/Sell.wav" ) ;
			CItem* pItem = g_objHero.GetItemByIndex ( m_IconIndex ) ;
			if ( pItem )
			{
				if ( !IsImportantItem ( pItem ) && !pItem->IsNonsuchType () )
				{
					pItem = g_objHero.GetItemByIndex ( m_IconIndex ) ;
					if ( pItem && pItem->GetID () == m_IconRealID )
						g_objHero.SellItem ( pItem->GetID () ) ;
				}
				else
				{
					m_nImptIndex = m_IconIndex ;
					m_DlgConfirm.SetConfirmDlg ( 15, NULL, 0 ) ;
				}
			}
			SFK_SET_PICKUP_STATE ( false, 0 ) ;
		}
		else if ( m_PickUpDlgID == DLG_NPCBUY )
		{
			InsertLandR = m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.GetInsertLandR () ;
			m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.InsertIcon ( InsertLandR.y,
															   InsertLandR.x,
															   m_IconTypeID,
															   m_IconIndex,
															   ICON_TYPE_ITEM ) ;
		}
		m_DlgMain.m_GoodBox.SetPickUp ( false ) ;
		m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
		m_DlgMain.m_DlgTrade.m_TradeGriMpc.SetPickUp ( false ) ;
		break ;
	case DLG_MPCGOODS:
		m_bPickUp = false ;
		if ( m_PickUpDlgID == DLG_NPCBUY )
		{
			CMyShellDlg::OnIconBuy ( m_IconTypeID ) ;
			m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.ReSumePickUp ( ) ;
		}
		else if ( m_PickUpDlgID == DLG_MPCGOODS )
		{
			InsertLandR = m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.GetInsertLandR () ;
			int nIndex = m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.InsertIcon( InsertLandR.y,
																		   InsertLandR.x,
																		   m_IconTypeID,
																		   m_IconIndex,
																		   ICON_TYPE_ITEM,
																		   m_btIconActType ) ;
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetIconRealID( nIndex, m_IconRealID ) ;
		}
		m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( false ) ;
		m_DlgMain.m_GoodBox.SetPickUp ( false ) ;
		m_DlgMain.m_DlgTrade.m_TradeGriMpc.SetPickUp ( false ) ;
		m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( false ) ;
		m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( false ) ;
		SFK_SET_PICKUP_STATE ( false, 0 ) ;
		break ;
	case DLG_TRADEMPC:
		m_bPickUp = false ;
		if ( m_PickUpDlgID == DLG_MPCGOODS )
		{
			g_objHero.AddHeroTradeItem ( g_objHero.GetItemByIndex ( m_IconIndex ) ) ;
			SFK_SET_PICKUP_STATE ( false, 0 ) ;
		}
		m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
		m_DlgMain.m_GoodBox.SetPickUp ( false ) ;
		m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( false ) ;
		m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( false ) ;
		m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( false ) ;
		break ;
	case DLG_DEPOT:
		if ( m_PickUpDlgID != DLG_MPCGOODS )
		{
			m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( true ) ;
			break ;
		}
		if ( g_objHero.GetPackageItemAmount () >= g_objHero.GetPackageMaxItemAmount () )
		{
			g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10107 ) ) ;
			return ;
		}
		else
		{
			CItem* pItem = g_objHero.GetItemByIndex ( m_IconIndex ) ;
			if ( pItem && pItem->GetID () == m_IconRealID )
				g_objHero.CheckInItem ( pItem->GetID () ) ;
			m_bPickUp = false ;
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
			m_DlgMain.m_GoodBox.SetPickUp ( false ) ;
			m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( false ) ;
			m_DlgMain.m_DlgTrade.m_TradeGriMpc.SetPickUp ( false ) ;
			SFK_SET_PICKUP_STATE ( false, 0 ) ;
			m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( false ) ;
		}
		break ;
	case DLG_ACT:
		{
			if ( m_bPickUp )
			{
				CPoint MousePnt ;
				CRect rect ;
				GetCursorPos ( &MousePnt ) ;
				m_DlgMain.m_GoodBox.GetWindowRect ( rect ) ;
				if ( rect.PtInRect ( MousePnt ) )
				{
					MousePnt.x = MousePnt.x - rect.left ;
					MousePnt.y = MousePnt.y - rect.top ;
					MousePnt = m_DlgMain.m_GoodBox.GetLineAndRow ( MousePnt ) ;
					m_DlgMain.m_GoodBox.InsertIcon ( MousePnt.y,
													 MousePnt.x,
													 m_IconTypeID,
													 0,
													 ICON_TYPE_ACTION,
													 0,
													 false ) ;
				}
				m_bPickUp = false ;
			}
		}
		break ;
	case DLG_MAGIC:
		{
			if ( m_bPickUp )
			{
				CPoint MousePnt ;
				CRect rect ;
				GetCursorPos ( &MousePnt ) ;
				m_DlgMain.m_GoodBox.GetWindowRect ( rect ) ;
				if ( rect.PtInRect ( MousePnt ) )
				{
					MousePnt.x = MousePnt.x - rect.left ;
					MousePnt.y = MousePnt.y - rect.top ;
					MousePnt = m_DlgMain.m_GoodBox.GetLineAndRow ( MousePnt ) ;
					m_DlgMain.m_GoodBox.InsertIcon ( MousePnt.y,
													 MousePnt.x,
													 m_IconTypeID,
													 0,
													 ICON_TYPE_SKILL,
													 0,
													 false ) ;
					m_bPickUp = false ;
					break ;
				}

				m_DlgMain.m_MagicImg.GetWindowRect ( rect ) ;
				if ( rect.PtInRect ( CPoint ( m_IconMousePnt.y, m_IconMousePnt.x ) ) )
				{
					if ( !rect.PtInRect ( MousePnt ) )
					{
						m_DlgMain.m_MagicImg.RemoveImage () ;
						m_uCurSkill = 0 ;
						m_MagicID = 0 ;
						m_XpID = 0 ;
					}
				}
				m_bPickUp = false ;
			}
		}
		break ;
	case DLG_XP:
		{
			if ( m_bPickUp )
			{
				CPoint MousePnt ;
				CRect rect ;
				GetCursorPos ( &MousePnt ) ;
				m_DlgMain.m_GoodBox.GetWindowRect ( rect ) ;
				if ( rect.PtInRect ( MousePnt ) )
				{
					MousePnt.x = MousePnt.x - rect.left ;
					MousePnt.y = MousePnt.y - rect.top ;
					MousePnt = m_DlgMain.m_GoodBox.GetLineAndRow ( MousePnt ) ;
					m_DlgMain.m_GoodBox.InsertIcon ( MousePnt.y,
													 MousePnt.x,
													 m_IconTypeID,
													 0,
													 ICON_TYPE_XP,
													 0,
													 false ) ;
				}
				m_bPickUp = false ;
			}
		}
		break ;
	case DLG_BAG:
		{
			if ( m_PickUpDlgID != DLG_MPCGOODS )
			{
				m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( true ) ;
				break ;
			}
			if ( g_objHero.GetPackageItemAmount () >= g_objHero.GetPackageMaxItemAmount () )
			{
				g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10107 ) ) ;
				return ;
			}
			CItem* pItem = g_objHero.GetItemByIndex ( m_IconIndex ) ;
			if ( pItem && pItem->GetID () == m_IconRealID )
			{
				OBJID idType = pItem->GetTypeID () ;
				if ( idType == 1100003 || idType == 1100006 || idType == 1100009 )
				{
					g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10441 ) ) ;
					return ;
				}
				g_objHero.CheckInItem ( pItem->GetID () ) ;
			}
			m_bPickUp = false ;
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
			m_DlgMain.m_GoodBox.SetPickUp ( false ) ;
			m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( false ) ;
			m_DlgMain.m_DlgTrade.m_TradeGriMpc.SetPickUp ( false ) ;
			SFK_SET_PICKUP_STATE ( false, 0 ) ;
			m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( false ) ;
		}
		break ;
	case DLG_FUSE:
		{
			if ( m_PickUpDlgID != DLG_MPCGOODS )
			{
				m_DlgMain.m_DlgFuse.m_GriLay.SetPickUp ( true ) ;
				break ;
			}
			CItem* pItem = g_objHero.GetItemByIndex ( m_IconIndex ) ;
			if ( pItem && pItem->GetID () == m_IconRealID )
				m_DlgMain.m_DlgFuse.AddItem ( pItem->GetID () ) ;
			m_bPickUp = false ;
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
			m_DlgMain.m_GoodBox.SetPickUp ( false ) ;
			m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( false ) ;
			m_DlgMain.m_DlgTrade.m_TradeGriMpc.SetPickUp ( false ) ;
			m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( false ) ;
			SFK_SET_PICKUP_STATE ( false, 0 ) ;
			m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( false ) ;
			m_DlgMain.m_DlgFuse.m_GriLay.SetPickUp ( false ) ;
			CMyShellDlg::WindowProc ( WM_MY_MESSAGE, CMD_FLASHITEM, NULL ) ;
		}
		break ;
	case DLG_ALL:
		m_bPickUp = false ;
		m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
		m_DlgMain.m_GoodBox.SetPickUp ( false ) ;
		m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( false ) ;
		m_DlgMain.m_DlgTrade.m_TradeGriMpc.SetPickUp ( false ) ;
		m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( false ) ;
		m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( false ) ;
		SFK_SET_PICKUP_STATE ( false, 0 ) ;
		m_DlgMain.m_DlgFuse.m_GriLay.SetPickUp ( false ) ;
		break ;
	default:
		{
			int nMouseX, nMouseY;
			::MouseCheck(nMouseX, nMouseY);

			OBJID idItem = ID_NONE;
			if ( m_PickUpDlgID == DLG_MPCGOODS )
			{
				CItem* pItem = g_objHero.GetItemByIndex(m_IconIndex);
				if (pItem)
					idItem = pItem->GetID();
				m_bPickUp = false ;
				m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
			}
			g_objHero.DropItem(idItem, nMouseX, nMouseY);
		}
		break;
	}
}

void CMyShellDlg::OnIconTrade( LPARAM lParam)
{
	CPoint TradeLandR ;
	switch ( lParam )
	{
	case DLG_MAIN:
		if ( m_PickUpDlgID == DLG_MAIN )
		{
			TradeLandR = m_DlgMain.m_GoodBox.GetInsertLandR () ;
			m_DlgMain.m_GoodBox.ChangeIcon( TradeLandR,
											m_IconTypeID,
											m_IconIndex,
											ICON_TYPE_ITEM ) ;
			m_IconTypeID = m_DlgMain.m_GoodBox.GetPickUpTypeID () ;
			m_IconIndex = m_DlgMain.m_GoodBox.GetPickUpIndex () ;
			CMyShellDlg::SetQUseItemNum ( ) ;
		}
		else if ( m_PickUpDlgID == DLG_MPCGOODS )
		{
			m_bPickUp = false ;
			TradeLandR = m_DlgMain.m_GoodBox.GetInsertLandR () ;
			m_DlgMain.m_GoodBox.ChangeIcon( TradeLandR,
											m_IconTypeID,
											m_IconIndex,
											ICON_TYPE_ITEM ) ;
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.ReSumePickUp () ;
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
			m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( false ) ;
			m_DlgMain.m_GoodBox.SetPickUp ( false ) ;
			SFK_SET_PICKUP_STATE ( false, 0 ) ;
			CMyShellDlg::SetQUseItemNum ( ) ;
		}
		break ;
	case DLG_NPCBUY:
		if ( m_PickUpDlgID == DLG_MPCGOODS )
		{
			m_bPickUp = false ;
			DXPlaySound ( "Sound/Sell.wav" ) ;
			CItem* pItem = g_objHero.GetItemByIndex ( m_IconIndex ) ;
			if ( pItem )
			{
				if ( !IsImportantItem ( pItem ) )
				{
					pItem = g_objHero.GetItemByIndex ( m_IconIndex ) ;
					if ( pItem && pItem->GetID () == m_IconRealID )
						g_objHero.SellItem ( pItem->GetID () ) ;
				}
				else
				{
					m_nImptIndex = m_IconIndex ;
					m_DlgConfirm.SetConfirmDlg ( 15, NULL, 0 ) ;
				}
			}
			m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( false ) ;
			m_DlgMain.m_GoodBox.SetPickUp ( false ) ;
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
			SFK_SET_PICKUP_STATE ( false, 0 ) ;
			m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( false ) ;
			m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( false ) ;
		}
		else if ( m_PickUpDlgID == DLG_NPCBUY )
		{
			TradeLandR = m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.GetInsertLandR () ;
			m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.ChangeIcon( TradeLandR,
															  m_IconTypeID,
															  m_IconIndex,
															  ICON_TYPE_ITEM ) ;
			m_IconTypeID = m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.GetPickUpTypeID () ;
			m_IconIndex = m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.GetPickUpIndex () ;
		}
		break ;
	case DLG_MPCGOODS:
		if ( m_PickUpDlgID == DLG_NPCBUY )
		{
			m_bPickUp = false ;
			CMyShellDlg::OnIconBuy ( m_IconTypeID ) ;
			m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.ReSumePickUp () ;
			m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( false ) ;
			m_DlgMain.m_GoodBox.SetPickUp ( false ) ;
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
		}
		else if ( m_PickUpDlgID == DLG_MPCGOODS )
		{
			TradeLandR = m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.GetInsertLandR () ;
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.ChangeIcon( TradeLandR,
															  m_IconTypeID,
															  m_IconIndex,
															  ICON_TYPE_ITEM,
															  m_btIconActType ) ;
			m_IconTypeID = m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.GetPickUpTypeID () ;
			m_IconIndex = m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.GetPickUpIndex () ;
			m_btIconActType = m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.GetPickUpActType () ;
			SFK_SET_PICKUP_STATE ( true, m_IconIndex ) ;
		}
		break ;
	case DLG_DEPOT:
		if ( m_PickUpDlgID != DLG_MPCGOODS )
			break ;
		if ( g_objHero.GetPackageItemAmount () >= g_objHero.GetPackageMaxItemAmount () )
		{
			g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10107 ) ) ;
			return ;
		}
		else
		{
			CItem* pItem = g_objHero.GetItemByIndex ( m_IconIndex ) ;
			if ( pItem && pItem->GetID () == m_IconRealID )
				g_objHero.CheckInItem ( pItem->GetID () ) ;
			m_bPickUp = false ;
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
			m_DlgMain.m_GoodBox.SetPickUp ( false ) ;
			m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( false ) ;
			m_DlgMain.m_DlgTrade.m_TradeGriMpc.SetPickUp ( false ) ;
			m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( false ) ;
			m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( false ) ;
			SFK_SET_PICKUP_STATE ( false, 0 ) ;
		}
		break;
	case DLG_BAG:
		{
			if ( m_PickUpDlgID != DLG_MPCGOODS )
				break ;
			if ( g_objHero.GetPackageItemAmount () >= g_objHero.GetPackageMaxItemAmount () )
			{
				g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10107 ) ) ;
				return ;
			}
			CItem* pItem = g_objHero.GetItemByIndex ( m_IconIndex ) ;
			if ( pItem && pItem->GetID () == m_IconRealID )
			{
				OBJID idType = pItem->GetTypeID () ;
				if ( idType == 1100003 || idType == 1100006 || idType == 1100009 )
				{
					g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10441 ) ) ;
					return ;
				}
				g_objHero.CheckInItem ( pItem->GetID () ) ;
			}
			m_bPickUp = false ;
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
			m_DlgMain.m_GoodBox.SetPickUp ( false ) ;
			m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( false ) ;
			m_DlgMain.m_DlgTrade.m_TradeGriMpc.SetPickUp ( false ) ;
			m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( false ) ;
			m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( false ) ;
			SFK_SET_PICKUP_STATE ( false, 0 ) ;
		}
		break ;
	case DLG_FUSE:
		if ( m_PickUpDlgID == DLG_MPCGOODS )
		{
			CItem* pItem = g_objHero.GetItemByIndex ( m_IconIndex ) ;
			if ( pItem && pItem->GetID () == m_IconRealID )
				m_DlgMain.m_DlgFuse.AddItem ( pItem->GetID () ) ;
			m_bPickUp = false ;
			m_DlgMain.m_DlgMpcGoods.m_MpcGoodsGri.SetPickUp ( false ) ;
			m_DlgMain.m_GoodBox.SetPickUp ( false ) ;
			m_DlgMain.m_DlgNpcBuy.m_NpcBuySaleGri.SetPickUp ( false ) ;
			m_DlgMain.m_DlgTrade.m_TradeGriMpc.SetPickUp ( false ) ;
			m_DlgMain.m_DlgDepot.m_DepotGriLay.SetPickUp ( false ) ;
			SFK_SET_PICKUP_STATE ( false, 0 ) ;
			m_DlgMain.m_DlgBag.m_Grid.SetPickUp ( false ) ;
			m_DlgMain.m_DlgFuse.m_GriLay.SetPickUp ( false ) ;
			CMyShellDlg::WindowProc ( WM_MY_MESSAGE, CMD_FLASHITEM, NULL ) ;
		}
		else
			m_DlgMain.m_DlgFuse.m_GriLay.SetPickUp ( true ) ;
		break ;
	}
}

void CMyShellDlg::OnIconUse(LPARAM lParam)
{
	CItem* pItem = g_objHero.GetItemByIndex ( lParam ) ;
	if ( pItem == NULL )
		return ;
	OBJID idType = pItem->GetTypeID () ;
	if ( idType == 1100003 || idType == 1100006 || idType == 1100009 )
	{
		if ( g_objHero.m_objBoothManager.BeShowCloseDialog () )
		{
			g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10445 ) ) ;
			return ;
		}
		if ( m_DlgMain.m_DlgTrade.m_bShow )
		{
			g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10446 ) ) ;
			return ;
		}
		if ( m_DlgMain.m_DlgGroup.m_bShow )
			m_DlgMain.m_DlgGroup.ShowWindow ( SW_HIDE ) ;
		if ( m_DlgMain.m_DlgDepot.m_bShow )
			m_DlgMain.m_DlgDepot.ShowWindow ( SW_HIDE ) ;
		m_DlgMain.m_DlgBag.SetPage ( ( idType % 10 ) / 3 ) ;
		m_DlgMain.m_DlgBag.ShowWindow ( SW_SHOW ) ;
		g_objHero.OpenPackage ( pItem->GetID (), 30 ) ;
		return ;
	}
	if ( pItem->GetSort () >= 1 && pItem->GetSort () <= 8 )
		DXPlaySound ( "Sound/Equip.wav" ) ;
	if ( pItem->GetTypeID () == 723700 )
	{
		if ( g_objHero.TestStatus ( USERSTATUS_DIE ) )
			g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10492 ), _TXTATR_SYSTEM, 0xff0000 ) ;
		else
		{
			int nBonus = g_objHero.GetDayBonus () ;
			if ( nBonus >= 9 )
				g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10491 ), _TXTATR_SYSTEM, 0xff0000 ) ;
			else if ( nBonus >= 4 )
				m_DlgConfirm.SetConfirmDlg ( CONFIRM_TYPE_25, NULL, pItem->GetID () ) ;
			else
				m_DlgConfirm.SetConfirmDlg ( CONFIRM_TYPE_26, NULL, pItem->GetID () ) ;
		}
		return ;
	}
	g_objHero.UseItem ( pItem->GetID () ) ;
}

void CMyShellDlg::OnIconBuy(LPARAM lParam)
{
	if ( g_objShop.GetMoneyType () != 0 )
	{
		CItem* pItem = g_objShop.GetItemByTypeID ( lParam ) ;
		if ( pItem )
		{
			char strText[256] = "" ;
			sprintf ( strText, GameDataSetQuery()->GetStr ( 10482 ), pItem->GetEmoneyPrice (), pItem->GetName () ) ;
			m_DlgConfirm.SetConfirmDlg ( CONFIRM_TYPE_24, strText, lParam ) ;
		}
	}
	else
		g_objHero.BuyItem( lParam ) ;
}

void CMyShellDlg::OnIconQUse (LPARAM lParam)
{
	int nGridIndex = (BYTE)lParam ;
	int nIconTypeID = lParam >> 8 & 0xffffff ;
	switch ( m_DlgMain.m_GoodBox.GetIconType ( nGridIndex - 1 ) )
	{
	case ICON_TYPE_ITEM:
		{
			CItem *pItem = NULL ;
			bool bLoad = false;
			int i ;
			int nAmount = g_objHero.GetItemAmount() ;
			for ( i = 0; i < nAmount; i++ )
			{
				pItem = g_objHero.GetItemByIndex( i ) ;
				if ( pItem->GetTypeID() == nIconTypeID )
				{
					if ( bLoad )
						break ;
					g_objHero.UseItem( pItem->GetID() ) ;
					m_DlgMain.PlayItemQUse ( nGridIndex ) ;
					bLoad = true;
				}
			}
			if ( i == nAmount )
			{
				m_DlgMain.m_GoodBox.RemoveIcon ( 1, nGridIndex ) ;
				for ( int j = 1; j < 11; j++ )
				{
					if ( m_DlgMain.m_GoodBox.GetGridAble ( CPoint ( j, 1 ) ) && j != nGridIndex
						&& m_DlgMain.m_GoodBox.GetIconTypeID ( j - 1 ) == nIconTypeID )
						m_DlgMain.m_GoodBox.RemoveIcon ( 1, j ) ;
				}
			}
			m_bMsgDlg = false ;
			m_nMsgDlgWidth = 0 ;
		}
		break ;
	case ICON_TYPE_ACTION:
		m_DlgMain.m_DlgAct.SetCurAct ( nIconTypeID ) ;
		break ;
	case ICON_TYPE_SKILL:
		{
			CMagic* pMagic = g_objHero.GetMagic ( nIconTypeID ) ;
			if ( pMagic == NULL || !pMagic->IsEnable () )
			{
				g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10382 ) ) ;
				break ;
			}
			if ( nIconTypeID == 1190 || nIconTypeID == 1195 )
			{
				g_objHero.MagicAttack ( nIconTypeID, g_objHero.GetID () ) ;
				break ;
			}
			m_MagicID = nIconTypeID ;
			m_uCurSkill = 1 ;
			char SkillType[32] ;
			sprintf ( SkillType, "MagicSkillType%d", m_MagicID ) ;
			m_DlgMain.m_MagicImg.InsertImage ( SkillType, m_MagicID, false ) ;
		}
		break ;
	case ICON_TYPE_XP:
		{
			m_XpID = nIconTypeID ;
			CMagic* pXp = g_objHero.GetMagic ( nIconTypeID ) ;
			if ( pXp == NULL || !pXp->IsEnable () )
			{
				g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10382 ) ) ;
				break ;
			}
			if ( pXp->TestTarget ( 2 ) )
			{
				g_objHero.MagicAttack ( nIconTypeID, g_objHero.GetID () ) ;
				break ;
			}
			m_uCurSkill = 2 ;
			char SkillType[32] ;
			sprintf ( SkillType, "XpSkillType%d", nIconTypeID ) ;
			m_DlgMain.m_MagicImg.InsertImage ( SkillType, nIconTypeID, false ) ;
		}
		break ;
	default:
		break ;
	}
}

LRESULT CMyShellDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_NCPAINT )
		return 0 ;
	if ( message == WM_PAINT )
	{
		PAINTSTRUCT ps ;
		HDC hDC = ::BeginPaint ( m_hWnd, &ps ) ;
		::EndPaint ( m_hWnd, &ps ) ;
		return 0 ;
	}
	if ( message == WM_ERASEBKGND )
		return 1 ;
	if ( message == WM_COMMAND )
	{
		if ( wParam == IDOK )
		{
			if ( m_bLogin )
				m_DlgMain.OnShellEnter () ;
			return true ;
		}
		if ( wParam == IDCANCEL )
		{
			if ( m_DlgMain.m_DlgMpcGoods.m_bShow )
			{
				m_DlgMain.m_DlgMpcGoods.ShowWindow ( SW_HIDE ) ;
				m_DlgMain.CloseCurDlg () ;
				return true ;
			}
			if ( m_DlgMain.m_DlgNpcEquip.m_bShow )
			{
				m_DlgMain.m_DlgNpcEquip.ShowWindow ( SW_HIDE ) ;
				return true ;
			}
			if ( m_DlgMain.m_DlgDice.m_bShow )
			{
				g_objHero.m_objDiceManager.Close () ;
				return true ;
			}
			if ( m_DlgMain.IsCurDlgShow () )
			{
				m_DlgMain.CloseCurDlg () ;
				return true ;
			}
			CMyShellDlg::OnExit ( 2 ) ;
			return true ;
		}
		if ( wParam == ID_HELP )
			return true ;
	}
	else if ( message == WM_HSCROLL )
	{
		g_objGameMsg.SetShowLines ( m_SliLine.GetSliderValue () / 16 + 1 ) ;
	}
	else if ( message == WM_ACTIVATEAPP && wParam == TRUE )
	{
		int nActive = 0 ;
	}
	else if ( message == WM_MY_MESSAGE )
	{
		switch ( wParam )
		{
		case CMD_FLASHITEM:
			m_DlgMain.m_DlgMpcGoods.SetGridContain () ;
			CMyShellDlg::SetQUseItemNum () ;
			break ;
		case CMD_FLASHMONEY:
		case CMD_TRADPLAYERMONEY:
		case CMD_TRADHEROEMONEY:
			m_DlgMain.m_DlgTrade.SetNpcMoney () ;
			m_DlgMain.m_DlgTrade.SetNpcEmoney () ;
			break ;
		case CMD_OPENTRADE:
			m_DlgMain.CloseCurDlg () ;
			m_DlgMain.m_uCurDlgID = DLG_TRADE ;
			if ( m_DlgMain.m_DlgBag.m_bShow )
				m_DlgMain.m_DlgBag.ShowWindow ( SW_HIDE ) ;
			if ( m_DlgMain.m_DlgExercise.m_bShow )
				m_DlgMain.m_DlgExercise.ShowWindow ( SW_HIDE ) ;
			m_DlgMain.m_DlgTrade.ShowWindow ( SW_SHOW ) ;
			if ( !m_DlgMain.m_DlgMpcGoods.m_bShow )
				m_DlgMain.m_DlgMpcGoods.ShowWindow ( SW_SHOW ) ;
			break ;
		case CMD_TRADEOK:
			m_DlgMain.m_DlgTrade.ShowWindow ( SW_HIDE ) ;
			break ;
		case CMD_TRADEFALSE:
			if ( g_objHero.IsTradeActive () )
				g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10109 ) ) ;
			CMyShellDlg::OnIconDrop ( DLG_ALL ) ;
			m_DlgMain.m_DlgTrade.ShowWindow ( SW_HIDE ) ;
			m_DlgMain.m_DlgMpcGoods.SetGridContain () ;
			break ;
		case CMD_TRADEPLAYERITEM:
			m_DlgMain.m_DlgTrade.SetNpcGridContain () ;
			break ;
		case CMD_TRADEHEROITEM:
			m_DlgMain.m_DlgTrade.SetMpcGridContain () ;
			break ;
		case CMD_TRADEPLAYERAGREE:
			g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10108 ) ) ;
			m_DlgMain.m_DlgTrade.m_TradeStaNpcAgree.InsertImage () ;
			break ;
		case CMD_FRIEND_APPLY:
			m_DlgConfirm.SetConfirmDlg ( CONFIRM_TYPE_23, NULL, 0 ) ;
			break ;
		case CMD_TRADEPLAYERAPPLY:
			if ( !m_DlgMain.m_DlgTrade.m_bShow )
			{
				char strName[32] ;
				strcpy ( strName, g_objHero.GetApplyTradeName () ) ;
				m_DlgConfirm.SetConfirmDlg ( 0, strName, 0 ) ;
			}
			break ;
		case CMD_FLASHPACKAGEITEM:
			if ( m_DlgMain.m_DlgBag.m_bShow )
				m_DlgMain.m_DlgBag.SetPackage () ;
			m_DlgMain.m_DlgDepot.SetItem () ;
			break ;
		case CMD_FLASHBANKMONEY:
			m_DlgMain.m_DlgDepot.SetMoney () ;
			break ;
		case CMD_TASKDIALOGOPEN:
			if ( !m_DlgTask.m_bShow )
				m_DlgTask.ShowWindow ( SW_SHOW ) ;
			break ;
		case CMD_FLASHFRIENDLIST:
			m_DlgMain.m_DlgFriend.SetFriendList () ;
			break ;
		case CMD_FLASHEQUIPMENT:
			m_DlgMain.m_DlgQuery.m_DlgMission.FlashEquip () ;
		case CMD_FLASHPK:
			m_DlgMain.m_DlgQuery.m_DlgRole.SetRoleInfo () ;
			break ;
		case CMD_FLASHTEAM:
		case CMD_FLASH_TEAM_FACE:
			m_DlgMain.m_DlgGroup.SetGroupContain () ;
			break ;
		case CMD_TEAMINVITE:
			{
				char strName[32] ;
				strcpy ( strName, g_objHero.GetTeamApplyerName () ) ;
				m_DlgConfirm.SetConfirmDlg ( 2, strName, 0 ) ;
			}
			break ;
		case CMD_TEAMAPPLY:
			{
				char strName[32] ;
				strcpy ( strName, g_objHero.GetTeamApplyerName () ) ;
				m_DlgConfirm.SetConfirmDlg ( 1, strName, 0 ) ;
			}
			break ;
		case CMD_FLASHWEAPONSKILL:
			m_DlgMain.m_DlgQuery.m_DlgWeapon.ShowSkill () ;
			break ;
		case CMD_CHANGEPKMODE:
			{
				switch ( g_objHero.GetPkMode () )
				{
				case 0:
					m_DlgMain.m_bPkFlash = true ;
					m_DlgMain.m_PkBtn.ChangeImage ( "Button47" ) ;
					break ;
				case 1:
					m_DlgMain.m_PkBtn.ChangeImage ( "Button49" ) ;
					break ;
				case 2:
					m_DlgMain.m_PkBtn.ChangeImage ( "Button48" ) ;
					break ;
				case 3:
					m_DlgMain.m_PkBtn.ChangeImage ( "Button412" ) ;
					break ;
				}
				char strAppName[32] ;
				sprintf ( strAppName, "%sRecord", g_objHero.GetName () ) ;
				char* pstrTemp = strchr ( strAppName, '[' ) ;
				if ( pstrTemp != NULL )
					strncpy ( pstrTemp, "(", 1 ) ;
				pstrTemp = strchr ( strAppName, ']' ) ;
				if ( pstrTemp != NULL )
					strncpy ( pstrTemp, ")", 1 ) ;
				char strRecord[8] ;
				itoa ( g_objHero.GetPkMode (), strRecord, 10 ) ;
				WritePrivateProfileString ( strAppName, "PkModeRecord", strRecord, m_strOwnerIni ) ;
			}
			break ;
		case CMD_SETPKMODE:
			{
				char strHash[64] ;
				_i64toa ( HashName ( g_objHero.GetName () ), strHash, 10 ) ;
				sprintf ( m_strOwnerIni, "Log/%s/%s/Setup.ini", g_szServerName, strHash ) ;
				char strAppName[32] ;
				sprintf ( strAppName, "%sRecord", g_objHero.GetName () ) ;
				char* pstrTemp = strchr ( strAppName, '[' ) ;
				if ( pstrTemp != NULL )
					strncpy ( pstrTemp, "(", 1 ) ;
				pstrTemp = strchr ( strAppName, ']' ) ;
				if ( pstrTemp != NULL )
					strncpy ( pstrTemp, ")", 1 ) ;
				if ( GetPrivateProfileInt ( strAppName, "RecordAmount", -1, m_strOwnerIni ) == -1 )
				{
					sprintf ( m_strOwnerIni, "Log/%s/%s/Setup.ini", g_szServerName, g_objHero.GetName () ) ;
					if ( GetPrivateProfileInt ( strAppName, "RecordAmount", -1, m_strOwnerIni ) == -1 )
					{
						_i64toa ( HashName ( g_objHero.GetName () ), strHash, 10 ) ;
						sprintf ( m_strOwnerIni, "Log/%s/%s/Setup.ini", g_szServerName, strHash ) ;
					}
				}

				int nRecord = GetPrivateProfileInt ( strAppName, "LineRecord", -1, m_strOwnerIni ) ;
				if ( nRecord == -1 )
				{
					m_SliLine.SetSliderValue ( 80 ) ;
					g_objGameMsg.SetShowLines ( 6 ) ;
				}
				else
				{
					m_SliLine.SetSliderValue ( nRecord * 16 ) ;
					g_objGameMsg.SetShowLines ( nRecord + 1 ) ;
				}
				nRecord = GetPrivateProfileInt ( strAppName, "LogRecord", -1, m_strOwnerIni ) ;
				if ( nRecord == 0 )
					g_objGameMsg.m_objChatLog.LogOff () ;
				else if ( nRecord == 1 )
				{
					g_objGameMsg.m_objChatLog.LogOn () ;
					m_DlgLog.m_LogRecord.SetCheck ( 1 ) ;
				}
				for ( int i = 0; i < HOTKEY_PARENT_AMOUNT; i++ )
				{
					for ( int j = 0; j < HOTKEY_CHILD_AMOUNT; j++ )
					{
						char strKeyName[32] ;
						sprintf ( strKeyName, "HotKey%d%d", i, j ) ;
						g_nHotkeyValue[i * HOTKEY_CHILD_AMOUNT + j] = GetPrivateProfileInt ( strAppName, strKeyName, -1, m_strOwnerIni ) ;
					}
				}
				m_nFacWordValue = GetPrivateProfileInt ( strAppName, "FacWordRecord", -1, m_strOwnerIni ) ;
				m_DlgMain.SetRecord ( m_strOwnerIni ) ;
				CMyShellDlg::SetQUseItemNum () ;
			}
		case CMD_LEVELUP:
			m_DlgMain.m_DlgQuery.m_DlgRole.SetRoleInfo () ;
			CMyShellDlg::FlashInsHelp () ;
			break ;
		case CMD_FLASHCHATLOGFILELIST:
			m_DlgLog.SetLog ( 0 ) ;
			break ;
		case CMD_FLASHCURRENTCHATLOGLINELIST:
			if ( !m_DlgLog.IsWindowVisible () )
			{
				if ( m_DlgLog.m_bFirst )
				{
					m_DlgLog.SetLog ( 1 ) ;
					m_DlgLog.m_bFirst = false ;
				}
			}
			else
				m_DlgLog.SetLog ( 1 ) ;
			break ;
		case CMD_FLASH_MAGIC:
			m_DlgMain.m_DlgQuery.m_DlgSkill.ShowSkill () ;
			break ;
		case CMD_FLASH_MAGIC_EXP:
			m_DlgMain.m_DlgQuery.m_DlgSkill.FlashSkillExp () ;
			break ;
		case CMD_SYNDICATE_APPLY:
			{
				CPlayer* pPlayer = g_objPlayerSet.GetPlayer ( g_objHero.GetLastSyndicateApplyID () ) ;
				if ( pPlayer != NULL )
				{
					char strName[32] ;
					strcpy ( strName, pPlayer->GetName () ) ;
					m_DlgConfirm.SetConfirmDlg ( 3, strName, 0 ) ;
				}
			}
			break ;
		case CMD_SYNDICATE_INVITE:
			{
				char strName[32] ;
				strcpy ( strName, g_objPlayerSet.GetSyndicateName ( g_objHero.GetLastSyndicateID () ) ) ;
				m_DlgConfirm.SetConfirmDlg ( 4, strName, 0 ) ;
			}
			break ;
		case CMD_FLASH_SYNDICATE:
			m_DlgMain.m_DlgQuery_Faction.SetFaction () ;
			break ;
		case CMD_START_MAGIC_BUSY:
			m_DlgMain.m_MpPrg.SetShowMode ( 1 ) ;
			break ;
		case CMD_END_MAGIC_BUSY:
			m_DlgMain.m_MpPrg.SetShowMode ( 0 ) ;
			break ;
		case CMD_XPFULL:
			if ( !g_objHero.IsDead () )
				m_DlgMain.OnXpFull () ;
			break ;
		case CMD_XPEMPTY:
			{
				m_DlgMain.m_XpPrg.SetShowMode ( 0 ) ;
				m_DlgMain.m_DlgXp.ShowWindow ( SW_HIDE ) ;
				m_uCurSkill = 1 ;
				char SkillType[32] ;
				if ( m_MagicID == 0 )
				{
					sprintf ( SkillType, "Magic%d", 0 ) ;
					m_DlgMain.m_MagicImg.InsertImage ( SkillType, 0, false ) ;
				}
				else
				{
					sprintf ( SkillType, "MagicSkillType%d", m_MagicID ) ;
					m_DlgMain.m_MagicImg.InsertImage ( SkillType, m_MagicID, false ) ;
				}
			}
			break ;
		case CMD_XPCHANGE:
			if ( g_objHero.GetXP () < 100 )
				m_DlgMain.PlayXp () ;
			break ;
		case CMD_QUERYFRIEND_OK:
			m_DlgMain.m_DlgFriend.SetFriendState () ;
			break ;
		case CMD_QUERYFRIEND_TIMEOUT:
			CMyShellDlg::MessageBox ( CMyShellApp::StringChange ( GameDataSetQuery()->GetStr ( 10110 ) ), GameDataSetQuery()->GetStr ( 10117 ), MB_ICONERROR ) ;
			break ;
		case CMD_FLASH_SYNDICATE_WORD:
			{
				static BOOL s_bNewWord = true ;
				m_DlgMain.m_DlgQuery_Faction.SetWord () ;
				if ( !m_DlgMain.m_bFactionRead && !m_DlgMain.m_DlgQuery_Faction.m_bShow )
				{
					if ( s_bNewWord )
					{
						s_bNewWord = false ;
						char strWord[256] ;
						strcpy ( strWord, g_objHero.GetSyndicateAnnounce () ) ;
						if ( strcmp ( strWord, "NULL" ) == 0 || strWord[0] == 0 )
							break ;
						int nValue = 0 ;
						for ( UINT i = 0; i < strlen ( strWord ); i++ )
							nValue += abs ( ( int )( char )strWord[i] ) ;
						if ( nValue == m_nFacWordValue )
							break ;
					}
					m_DlgMain.m_bFactionRead = true ;
				}
			}
			break ;
		case CMD_REBORN:
		case CMD_GHOST_END:
			m_DlgMain.HeroReborn () ;
			break ;
		case CMD_GHOST_BEGIN:
			m_DlgMain.HeroDie () ;
			break ;
		case CMD_FLASH_LEAVE_WORD:
			m_DlgMain.m_DlgMsgReceive.SetReceiveMsg () ;
			break ;
		case CMD_NEW_LEAVE_WORD:
			if ( !m_DlgMain.m_bLevRead )
				m_DlgMain.m_bLevRead = true ;
			break ;
		case CMD_FLASH_BOOTH_ITEM:
			m_DlgMain.m_DlgNpcBooth.SetNpcBooth () ;
			break ;
		case CMD_FLASH_BOOTHMANAGER_ITEM:
			m_DlgMain.m_DlgMpcBooth.SetMpcBooth () ;
			break ;
		case CMD_FLASH_BULLETIN_LIST:
			if ( m_DlgMain.m_DlgLevWord.m_bShow )
				m_DlgMain.m_DlgLevWord.SetLevWordList () ;
			break ;
		case CMD_FLASH_BULLETIN_NOTE:
			if ( m_DlgMain.m_DlgLevWord.m_bShow )
				m_DlgMain.m_DlgLevWord.SetLevWordNote () ;
			break ;
		case CMD_OPEN_PLACE_STATUARY_DIALOG:
			if ( !m_DlgMain.m_DlgEffigy.m_bShow )
				m_DlgMain.OpenEffigy () ;
			break ;
		case CMD_COURT:
			m_uCmd = CMD_MPCCOURT ;
			break ;
		case CMD_BECOURT:
			{
				CPlayer* pPlayer = g_objPlayerSet.GetPlayer ( ( OBJID )lParam ) ;
				if ( pPlayer != NULL )
					m_DlgConfirm.SetConfirmDlg ( 14, pPlayer->GetName (), lParam ) ;
			}
			break ;
		case CMD_OPEN_PLACE_NPC_INFO_DIALOG:
			m_uCmd = CMD_LAY ;
			break ;
		case CMD_OPEN_PLACE_NPC_MODIFY_DIALOG:
			if ( !m_DlgMain.m_DlgLay.m_bShow )
				m_DlgMain.OpenLay () ;
			break ;
		case CMD_OPEN_FLASH_SYN_MEMBER:
			m_DlgMain.m_DlgQuery_Faction.SetMemberList () ;
			break ;
		case CMD_OPEN_FLASH_SYN_MEMBER_INFO:
			m_DlgMain.m_DlgQuery_Faction.SetFacState () ;
			break ;
		case CMD_FLASH_MAGIC_STATUS:
			{
				CMagic* pMagic = g_objHero.GetMagic ( m_MagicID ) ;
				if ( pMagic != NULL )
				{
					if ( !pMagic->IsEnable () )
					{
						m_DlgMain.m_MagicImg.RemoveImage () ;
						m_DisMagicID = m_MagicID ;
						m_MagicID = 0 ;
					}
					else
					{
						char SkillType[32] ;
						sprintf ( SkillType, "MagicSkillType%d", m_MagicID ) ;
						m_DlgMain.m_MagicImg.InsertImage ( SkillType, m_MagicID, false ) ;
					}
				}
				if ( m_DisMagicID != 0 )
				{
					pMagic = g_objHero.GetMagic ( m_DisMagicID ) ;
					if ( pMagic != NULL && pMagic->IsEnable () )
					{
						char SkillType[32] ;
						sprintf ( SkillType, "MagicSkillType%d", m_DisMagicID ) ;
						m_DlgMain.m_MagicImg.InsertImage ( SkillType, m_DisMagicID, false ) ;
						m_MagicID = m_DisMagicID ;
						m_DisMagicID = 0 ;
					}
				}
				if ( m_DlgMain.m_DlgMagic.m_bShow )
					m_DlgMain.m_DlgMagic.InitMagicDialog () ;
				if ( m_DlgMain.m_DlgKongfu.m_bShow )
					m_DlgMain.m_DlgKongfu.InitKongfuDialog () ;
				if ( m_DlgMain.m_DlgXp.m_bShow && m_DlgMain.m_DlgXp.m_btBoothType != 2
					&& !m_DlgMain.m_DlgXp.m_bTransform && !m_DlgMain.m_DlgXp.m_bFly )
					m_DlgMain.m_DlgXp.InitXpDialog () ;
			}
			break ;
		case CMD_FLASH_OTHERPLAYER_EQUIPMENT:
			m_DlgMain.m_DlgNpcEquip.SetNpcEquip () ;
			break ;
		case CMD_OPEN_ABORT_TRANSFORM_DLG:
			if ( g_objHero.IsMagicLock () )
				m_DlgMain.m_DlgXp.SetTransform ( true ) ;
			break ;
		case CMD_CLOSE_ABORT_TRANSFORM_DLG:
			if ( g_objHero.IsMagicLock () )
				m_DlgMain.m_DlgXp.SetTransform ( false ) ;
			break ;
		case CMD_DICE_JOIN:
			m_DlgMain.OpenDice ( true ) ;
			break ;
		case CMD_DICE_QUIT:
			m_DlgMain.OpenDice ( false ) ;
			break ;
		case CMD_DICE_FLASH_PLAYERLIST:
			m_DlgMain.m_DlgDiceList.SetDiceList () ;
			break ;
		case CMD_DICE_FLASH_ANTES:
			m_DlgMain.m_DlgDice.SetAntes () ;
			break ;
		case CMD_FLASHENEMYLIST:
			m_DlgMain.m_DlgEnemy.SetEnemyList () ;
			break ;
		case CMD_QUERYENEMY_OK:
			m_DlgMain.m_DlgEnemy.SetEnemyState () ;
			break ;
		case CMD_QUERYENEMY_TIMEOUT:
			CMyShellDlg::MessageBox ( CMyShellApp::StringChange ( GameDataSetQuery()->GetStr ( 10399 ) ), GameDataSetQuery()->GetStr ( 10117 ), MB_ICONERROR ) ;
			break ;
		case CMD_RESET:
			m_DlgMain.CloseCurDlg () ;
			if ( m_DlgMain.m_DlgMpcGoods.m_bShow )
				m_DlgMain.m_DlgMpcGoods.ShowWindow ( SW_HIDE ) ;
			m_DlgMain.m_DlgQuery.m_DlgSkill.ClearSkill () ;
			break ;
		case CMD_RESOURCE_LOSE:
			m_DlgMain.m_DlgSetup.ResetInterface () ;
			m_DlgConfirm.SetConfirmDlg ( 21, NULL, 0 ) ;
			break ;
		case CMD_ACTION_MESSAGEBOX:
			m_DlgMain.OpenCompose () ;
			break ;
		case CMD_ACTION_IDENTBOX:
			if ( lParam != NULL )
			{
				::ShellExecute ( m_hWnd, NULL, ( const char* )lParam, NULL, NULL, SW_SHOWNORMAL ) ;
				delete ( char* )lParam ;
			}
			break ;
		case CMD_FRIEND_ACCEPT:
			m_DlgMain.CloseCurDlg () ;
			if ( !m_DlgMain.m_DlgMpcGoods.m_bShow )
				m_DlgMain.m_DlgMpcGoods.ShowWindow ( SW_SHOW ) ;
			m_DlgMain.m_uCurDlgID = DLG_FUSE ;
			m_DlgMain.m_DlgFuse.Open ( 1, m_idActiveNpc ) ;
			break ;
		case CMD_ACTION_SELECTEMBLEM:
			m_DlgMain.CloseCurDlg () ;
			if ( !m_DlgMain.m_DlgMpcGoods.m_bShow )
				m_DlgMain.m_DlgMpcGoods.ShowWindow ( SW_SHOW ) ;
			m_DlgMain.m_uCurDlgID = DLG_FUSE ;
			m_DlgMain.m_DlgFuse.Open ( 2, m_idActiveNpc ) ;
			break ;
		case CMD_ACTION_OPENDIALOG:
			if ( !m_DlgMain.m_DlgNpcBuy.m_bShow && !m_DlgMain.m_DlgDepot.m_bShow )
				m_DlgMain.OpenDepot ( 3, m_idActiveNpc ) ;
			break ;
		case CMD_ACTION_TEACHAPPLY:
			if ( !m_DlgMain.m_DlgEnchant.m_bShow )
				m_DlgMain.OpenEnchant () ;
			break ;
		case CMD_EXERCISE_CONFIRM:
			switch ( lParam )
			{
			case 0:
				m_DlgMain.m_DlgExeConfirm.SetTime ( g_objHero.GetTableParam ( 0 ) ) ;
				break ;
			case 1:
				m_DlgMain.m_DlgExercise.FlashExp ( m_DlgMain.m_DlgExercise.GetTotalTime () ) ;
				break ;
			case 3:
				m_DlgMain.m_DlgExercise.SetTime ( g_objHero.GetTableParam ( 0 ), g_objHero.GetTableParam ( 1 ), g_objHero.IsBlessed () ) ;
				break ;
			case 4:
				m_DlgConfirm.SetConfirmDlg ( CONFIRM_TYPE_28, GameDataSetQuery()->GetStr ( 11001 ), 0 ) ;
				m_bExit = false ;
				break ;
			case 5:
				this->PostMessage ( WM_MY_MESSAGE, ON_EXERCISE_EXIT, g_objHero.GetExerciseID () ) ;
				break ;
			case 6:
				m_DlgConfirm.SetConfirmDlg ( CONFIRM_TYPE_27, NULL, g_objHero.GetExerciseID () ) ;
				m_bExit = false ;
				break ;
			default:
				break ;
			}
			break ;
		default:
			break ;
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CMyShellDlg::OnMpcGoodsClose()
{
	m_DlgMain.m_DlgMpcGoods.ShowWindow ( SW_HIDE ) ;
}

void CMyShellDlg::OnExit( UINT uType )
{
	if ( uType == 1 )
	{
		try
		{
			if ( m_pMutexGuard != NULL )
				delete m_pMutexGuard ;
			m_pMutexGuard = NULL ;
		}
		catch ( ... )
		{
			m_pMutexGuard = NULL ;
		}
		CMyShellDlg::RealExit ( 1 ) ;
		return ;
	}
	m_bExit = true ;
	if ( m_bLogin && ::IsWindow ( m_DlgConfirm.GetSafeHwnd () ) )
	{
		m_DlgConfirm.SetConfirmDlg ( CONFIRM_TYPE_27, NULL, 0 ) ;
		return ;
	}
	if ( CMyShellDlg::MessageBox ( GameDataSetQuery()->GetStr ( 10111 ), GameDataSetQuery()->GetStr ( 10041 ), MB_YESNO ) == IDYES )
		CMyShellDlg::ConfirmedExit () ;
	m_bExit = false ;
}

void CMyShellDlg::ConfirmedExit()
{
	try
	{
		if ( m_pMutexGuard != NULL )
			delete m_pMutexGuard ;
		m_pMutexGuard = NULL ;
	}
	catch ( ... )
	{
		m_pMutexGuard = NULL ;
	}
	CMyShellDlg::RealExit ( 0 ) ;
}

void CMyShellDlg::RealExit( UINT uType )
{
	if ( uType != 1 )
		g_objNetwork.Destroy () ;
	if ( m_nScreenMode == 1 )
		ChangeDisplaySettings( &m_OldDeviceMode,
							   0 ) ;
	if ( m_pGameFont )
		m_pGameFont->DeleteObject () ;
	::DeleteObject ( m_Hbr ) ;

#ifndef _SOUND_DISABLE
	DXDestroy() ;
#endif
	gpDlgMain = NULL ;
	gpDlgShell = NULL ;

	if ( m_bLogin && m_strOwnerIni[0] != 0 && strlen ( m_strOwnerIni ) != 0 )
	{
		char strAppName[32] ;
		sprintf ( strAppName, "%sRecord", g_objHero.GetName () ) ;

		char* pstrTemp = strchr ( strAppName, '[' ) ;
		if ( pstrTemp != NULL )
		{
			strncpy ( pstrTemp, "(", 1 ) ;
			pstrTemp = NULL ;
		}
		pstrTemp = strchr ( strAppName, ']' ) ;
		if ( pstrTemp != NULL )
		{
			strncpy ( pstrTemp, ")", 1 ) ;
			pstrTemp = NULL ;
		}

		if ( uType != 1 )
		{
			char strHash[64] ;
			_i64toa ( HashName ( g_objHero.GetName () ), strHash, 10 ) ;
			sprintf ( m_strOwnerIni, "Log/%s/%s/Setup.ini", g_szServerName, strHash ) ;
		}

		char strRecord[16] ;

		itoa ( m_SliLine.GetSliderValue () / 14, strRecord, 10 ) ;
		WritePrivateProfileString ( strAppName, "LineRecord", strRecord, m_strOwnerIni ) ;
		itoa ( m_DlgLog.m_LogRecord.GetCheck (), strRecord, 10 ) ;
		WritePrivateProfileString ( strAppName, "LogRecord", strRecord, m_strOwnerIni ) ;

		for ( int i = 0; i < HOTKEY_PARENT_AMOUNT; i++ )
		{
			for ( int j = 0; j < HOTKEY_CHILD_AMOUNT; j++ )
			{
				int nValue = g_nHotkeyValue[i * HOTKEY_CHILD_AMOUNT + j] ;
				if ( nValue == -1 )
					continue ;
				char strKeyName[32], strValue[16]  ;
				sprintf ( strKeyName, "HotKey%d%d", i, j ) ;
				itoa ( nValue, strValue, 10 ) ;
				if ( uType != 1 && nValue == 0 )
					WritePrivateProfileString ( strAppName, strKeyName, "-1", m_strOwnerIni ) ;
				else
					WritePrivateProfileString ( strAppName, strKeyName, strValue, m_strOwnerIni ) ;
			}
		}
		if ( uType != 1 )
			m_DlgMain.WriteRecord () ;
	}
	if ( strcmp ( "中文", GameDataSetQuery()->GetVersionInfo () ) == 0 )
	{
		char szUrl[256] = "" ;
		if ( IniStrGet ( "ini/oem.ini", "ExitPopUp", "url", szUrl ) )
			::ShellExecute ( m_hWnd, NULL, szUrl, NULL, NULL, SW_SHOW ) ;
		else
			::ShellExecute ( m_hWnd, NULL, "http://localhost/yxtc", NULL, NULL, SW_SHOW ) ;
	}
	GameDataSetDestroy () ;
	RoleDataDestroy () ;
	CMyShellDlg::EndDialog ( 0 ) ;
	::PostQuitMessage( WM_QUIT ) ;
	return ;
}

void CMyShellDlg::DropMapItem(int nPosx, int nPosy)
{
	CItem* pItem = g_objHero.GetItemByIndex ( m_IconIndex ) ;
	if ( pItem )
	{
		if ( IsImportantItem ( pItem ) || pItem->IsNonsuchType () || pItem->GetMonopoly () == 2 || pItem->GetMonopoly () == 3 )
		{
			m_nDropIndex = m_IconIndex ;
			m_DropPnt = CPoint ( nPosx, nPosy ) ;
			m_DlgConfirm.SetConfirmDlg ( 16, NULL, 0 ) ;
		}
		else
		{
			pItem = g_objHero.GetItemByIndex ( m_IconIndex ) ;
			if ( !g_objHero.DropItem ( pItem->GetID (), nPosx, nPosy	) )
			{
				CMyShellDlg::WindowProc ( WM_MY_MESSAGE, CMD_FLASHITEM, NULL ) ;
			}
		}
	}
	CMyShellDlg::OnIconDrop ( DLG_ALL ) ;
}

void CMyShellDlg::PickMapItem(OBJID ItemID, int nPosx, int nPosy)
{
	g_objHero.PickUpItem ( ItemID, nPosx, nPosy ) ;
}

void CMyShellDlg::ShowHelpDlg(LPARAM lParam)
{
	char strFileName[16] ;
	sprintf ( strFileName, "Help%d.htm", lParam ) ;
	CRect rect, Hrect ;
	CMyShellDlg::GetWindowRect ( rect ) ;
	m_DlgHelp.GetWindowRect ( Hrect ) ;
	m_DlgHelp.MoveWindow ( rect.left + ( rect.Width () - Hrect.Width () ) / 2,
						   rect.top + ( rect.Height () - Hrect.Height () ) / 2,
						   Hrect.Width (),
						   Hrect.Height () ) ;
	m_DlgHelp.OpenHelpDlg ( strFileName ) ;
}

void CMyShellDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMyShellDlg::SetQUseItemNum()
{
	for ( int i = 1; i <= GRID_ICONQUICK_NUM; i++ )
	{
		if ( m_DlgMain.m_GoodBox.GetGridAble ( CPoint ( i, 1 ) ) )
		{
			if ( m_DlgMain.m_GoodBox.GetIconType ( i - 1 ) != ICON_TYPE_ITEM )
				continue ;
			DWORD dwTypeID = m_DlgMain.m_GoodBox.GetIconTypeID ( i - 1 ) ;
			int nItemNum = 0 ;
			for ( int j = 0; j < g_objHero.GetItemAmount (); j++ )
			{
				CItem * pItem = g_objHero.GetItemByIndex( j ) ;
				if ( pItem->GetTypeID () == dwTypeID )
					nItemNum++ ;
			}
			if ( nItemNum == 0 )
				m_DlgMain.m_GoodBox.RemoveIcon ( 1, i ) ;
			else
				m_DlgMain.m_GoodBox.SetIconNum ( i - 1, nItemNum ) ;
		}
	}
}

void CMyShellDlg::SetScreenMode(BYTE btType, BOOL bBegin)
{
	switch ( btType )
	{
	case 0:
		if ( m_nScreenMode == 2 || m_nScreenMode == 3 )
		{
			if ( CMyShellDlg::MessageBox ( GameDataSetQuery()->GetStr ( 10112 ), GameDataSetQuery()->GetStr ( 10116 ), MB_YESNO ) == IDYES )
			{
				WritePrivateProfileString ( "ScreenMode", "ScreenModeRecord", "0", g_strSetupIni ) ;
				CMyShellDlg::OnExit( 1 ) ;
				return ;
			}
			else
				return ;
		}
		m_nScreenMode = btType ;
		if ( ( m_OldDeviceMode.dmBitsPerPel != 16 ||
				 m_OldDeviceMode.dmPelsWidth != 800 ||
					m_OldDeviceMode.dmPelsHeight != 600 ) &&
						!bBegin )
			ChangeDisplaySettings( &m_OldDeviceMode,
								   0) ;
		CMyShellDlg::GetParent()->MoveWindow ( 0, 0, CMyBitmap::GetScreenWidth (), CMyBitmap::GetScreenHeight () ) ;

		WritePrivateProfileString ( "ScreenMode", "ScreenModeRecord", "0", g_strSetupIni ) ;
		break ;
	case 1:
		{
		if ( m_nScreenMode == 2 || m_nScreenMode == 3 )
			{
			if ( CMyShellDlg::MessageBox ( GameDataSetQuery()->GetStr ( 10112 ), GameDataSetQuery()->GetStr ( 10116 ), MB_YESNO ) == IDYES )
			{
				WritePrivateProfileString ( "ScreenMode", "ScreenModeRecord", "1", g_strSetupIni ) ;
				CMyShellDlg::OnExit( 1 ) ;
				return ;
			}
			else
				return ;
		}
		m_nScreenMode = btType ;
		EnumDisplaySettings( NULL,
							 ENUM_CURRENT_SETTINGS,
							 &m_OldDeviceMode ) ;
		if ( m_OldDeviceMode.dmBitsPerPel != 16 ||
				m_OldDeviceMode.dmPelsWidth != 800 ||
					m_OldDeviceMode.dmPelsHeight != 600 )
		{
			DEVMODE ChangeMode ;
			ChangeMode = m_OldDeviceMode ;
			ChangeMode.dmBitsPerPel = 16 ;
			ChangeMode.dmPelsWidth  = 800 ;
			ChangeMode.dmPelsHeight = 600 ;
			ChangeDisplaySettings( &ChangeMode,
								   0 ) ;
		}
		WritePrivateProfileString ( "ScreenMode", "ScreenModeRecord", "1", g_strSetupIni ) ;
		if ( m_DlgTitleBar.m_hWnd != NULL )
			m_DlgTitleBar.MoveWindow ( 0, -20, 800, 20 ) ;
		}
		break ;
	case 2:
		if ( m_nScreenMode == 0 || m_nScreenMode == 1 )
		{
			if ( m_nScreenMode == 1 )
			{
				if ( m_OldDeviceMode.dmPelsWidth < 1024 || m_OldDeviceMode.dmPelsHeight < 768 )
				{
					AfxMessageBox ( CMyShellApp::StringChange ( GameDataSetQuery()->GetStr ( 10113 ) ) ) ;
					return ;
				}
			}
			else
			{
				DEVMODE CurrentMode ;
				EnumDisplaySettings( NULL,
									 ENUM_CURRENT_SETTINGS,
									 &CurrentMode ) ;
				if ( CurrentMode.dmPelsWidth < 1024 || CurrentMode.dmPelsHeight < 768 )
				{
					AfxMessageBox ( CMyShellApp::StringChange ( GameDataSetQuery()->GetStr ( 10113 ) ) ) ;
					return ;
				}
			}
			if ( CMyShellDlg::MessageBox ( GameDataSetQuery()->GetStr ( 10112 ), GameDataSetQuery()->GetStr ( 10116 ), MB_YESNO ) == IDYES )
			{
				WritePrivateProfileString ( "ScreenMode", "ScreenModeRecord", "2", g_strSetupIni ) ;
				CMyShellDlg::OnExit( 1 ) ;
				return ;
			}
			else
				return ;
		}
		{
		DEVMODE CurrentMode ;
		EnumDisplaySettings( NULL,
							 ENUM_CURRENT_SETTINGS,
							 &CurrentMode ) ;
		if ( CurrentMode.dmPelsWidth < 1024 || CurrentMode.dmPelsHeight < 768 )
		{
			AfxMessageBox ( CMyShellApp::StringChange ( GameDataSetQuery()->GetStr ( 10113 ) ) ) ;
			_SCR_WIDTH = 800 ;
			_SCR_WIDTH = 600 ;
			return ;
		}
		m_nScreenMode = btType ;

		if ( ( m_OldDeviceMode.dmBitsPerPel != 16 ||
				  m_OldDeviceMode.dmPelsWidth != 1024 ||
					m_OldDeviceMode.dmPelsHeight != 768 ) &&
						!bBegin )
			ChangeDisplaySettings( &m_OldDeviceMode,
								   0) ;
		CMyShellDlg::GetParent()->MoveWindow ( 0, 0, _SCR_WIDTH, _SCR_HEIGHT ) ;
		if ( m_DlgTitleBar.m_hWnd != NULL )
		{
			CRect rect ;
			CMyShellDlg::GetWindowRect ( rect ) ;
			m_DlgTitleBar.MoveWindow ( rect.left,
									   rect.top - 20,
									   _SCR_WIDTH,
									   20 ) ;
		}

		WritePrivateProfileString ( "ScreenMode", "ScreenModeRecord", "2", g_strSetupIni ) ;
		}
		break ;
	case 3:
		{
		if ( m_nScreenMode == 0 || m_nScreenMode == 1 )
		{
			if ( m_nScreenMode == 1 )
			{
				if ( m_OldDeviceMode.dmPelsWidth < 1024 || m_OldDeviceMode.dmPelsHeight < 768 )
				{
					AfxMessageBox ( CMyShellApp::StringChange ( GameDataSetQuery()->GetStr ( 10113 ) ) ) ;
					return ;
				}
			}
			else
			{
				DEVMODE CurrentMode ;
				EnumDisplaySettings( NULL,
									 ENUM_CURRENT_SETTINGS,
									 &CurrentMode ) ;
				if ( CurrentMode.dmPelsWidth < 1024 || CurrentMode.dmPelsHeight < 768 )
				{
					AfxMessageBox ( CMyShellApp::StringChange ( GameDataSetQuery()->GetStr ( 10113 ) ) ) ;
					return ;
				}
			}
			if ( CMyShellDlg::MessageBox ( GameDataSetQuery()->GetStr ( 10112 ), GameDataSetQuery()->GetStr ( 10116 ), MB_YESNO ) == IDYES )
			{
				WritePrivateProfileString ( "ScreenMode", "ScreenModeRecord", "3", g_strSetupIni ) ;
				CMyShellDlg::OnExit( 1 ) ;
				return ;
			}
			else
				return ;
		}
		DEVMODE CurrentMode ;
		EnumDisplaySettings( NULL,
							 ENUM_CURRENT_SETTINGS,
							 &CurrentMode ) ;
		if ( CurrentMode.dmPelsWidth < 1024 || CurrentMode.dmPelsHeight < 768 )
		{
			AfxMessageBox ( CMyShellApp::StringChange ( GameDataSetQuery()->GetStr ( 10113 ) ) ) ;
			_SCR_WIDTH = 800 ;
			_SCR_WIDTH = 600 ;
			return ;
		}
		m_nScreenMode = btType ;
		EnumDisplaySettings( NULL,
							 ENUM_CURRENT_SETTINGS,
							 &m_OldDeviceMode ) ;
		if ( m_OldDeviceMode.dmBitsPerPel != 16 ||
				m_OldDeviceMode.dmPelsWidth != 1024 ||
					m_OldDeviceMode.dmPelsHeight != 768 )
		{
			DEVMODE ChangeMode ;
			ChangeMode = m_OldDeviceMode ;
			ChangeMode.dmBitsPerPel = 16 ;
			ChangeMode.dmPelsWidth  = 1024 ;
			ChangeMode.dmPelsHeight = 768 ;
			ChangeDisplaySettings( &ChangeMode,
								   0 ) ;
		}
		WritePrivateProfileString ( "ScreenMode", "ScreenModeRecord", "3", g_strSetupIni ) ;
		if ( m_DlgTitleBar.m_hWnd != NULL )
		{
			CRect rect ;
			CMyShellDlg::GetWindowRect ( rect ) ;
			m_DlgTitleBar.MoveWindow ( rect.left,
									   rect.top - 20,
									   _SCR_WIDTH,
									   20 ) ;
		}
		}
		break ;
	}
}

void CMyShellDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	MouseSet(point.x, point.y, 1);

	if (_STATUS_WAITING == g_uStatus)
		return;

	if ( m_uCmd == CMD_LAY )
		g_objHero.DelMousePlayer () ;
	else if ( m_uCmd == CMD_NPCEQUIP )
		m_DlgMain.m_StandChk.SetCheckValue ( 0 ) ;

	if ( m_uCmd != CMD_NORMAL )
	{
		m_uCmd = CMD_NORMAL ;
		return ;
	}
	if ( m_bIconFix )
	{
		m_bIconFix = false ;
		return ;
	}

	if ( g_objHero.IsDead () )
		return ;

	if ( m_uCurSkill == 1 )
	{
		CMagic* pMagic = NULL ;
		if ( m_MagicID != 0 )
			pMagic = g_objHero.GetMagic ( m_MagicID ) ;
		if ( pMagic != NULL )
		{
			if ( pMagic->TestTarget ( 4 ) )
			{
				CMyPos posTarget ;
				g_objGameMap.Screen2Cell ( point.x, point.y, posTarget.x, posTarget.y ) ;
				if ( !g_objHero.IsMiner () )
				{
					g_objHero.MagicAttack ( m_MagicID, posTarget ) ;
					return ;
				}
			}
			else
			{
				CPlayer* pPlayer = NULL ;
				if ( pMagic->TestTarget ( 1 ) )
					pPlayer = g_objPlayerSet.GetFocusPlayer ( PLAYER_INCLUDESELF ) ;
				else if ( pMagic->TestTarget ( 2 ) )
					pPlayer = &g_objHero ;
				else
				{
					DWORD dwFlag = PLAYER_NORMAL ;
					if ( pMagic->GetIDType () == 1100 || pMagic->GetIDType () == 1050 )
						dwFlag = PLAYER_INCLUDEDEAD ;
					if ( pMagic->m_infoMagicType.dwCrime == 1 )
						dwFlag |= PLAYER_MONSTERPRI ;
					else
						dwFlag |= PLAYER_PLAYERPRI ;
					pPlayer = g_objPlayerSet.GetFocusPlayer ( dwFlag ) ;
				}
				if ( pPlayer != NULL )
				{
					if ( nFlags & MK_CONTROL )
						g_objHero.MagicAttack ( m_MagicID, pPlayer->GetID (), true ) ;
					else
						g_objHero.MagicAttack ( m_MagicID, pPlayer->GetID (), false ) ;
					CDialog::OnRButtonDown(nFlags, point);
					return ;
				}
			}
		}
	}
	else if ( m_uCurSkill == 2 )
	{
		CPlayer* pPlayer = g_objPlayerSet.GetFocusPlayer ( PLAYER_INCLUDEDEAD ) ;
		if ( pPlayer != NULL )
		{
			g_objHero.MagicAttack ( m_XpID, pPlayer->GetID (), false ) ;
			CDialog::OnRButtonDown(nFlags, point);
			return ;
		}
	}

	if ( g_objHero.IsMiner () )
	{
		g_objHero.Mine () ;
		return ;
	}
	CDialog::OnRButtonDown(nFlags, point);
}

void CMyShellDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
	if ( m_DlgTitleBar.m_hWnd != NULL )
	{
		CRect rect ;
		CMyShellDlg::GetWindowRect ( rect ) ;
		m_DlgTitleBar.MoveWindow ( rect.left,
								   rect.top - 20,
								   CMyBitmap::GetScreenWidth (),
								   20 ) ;
		m_DlgTitleBar.ShowWindow ( SW_SHOW ) ;
	}
}

void CMyShellDlg::MoveDlg()
{
	CPoint MousePnt ;
	MousePnt = m_DlgTitleBar.GetOffset () ;
	CMyShellDlg::GetParent()->MoveWindow ( m_InitDlgRect.left + MousePnt.x,
										   m_InitDlgRect.top + MousePnt.y,
										   m_InitDlgRect.Width (),
										   m_InitDlgRect.Height () ) ;
}

void CMyShellDlg::MoveControl()
{
	CRect rect ;
#define SFK_MOVE_CONTROL( ctrl, dx, dy )	\
	{	\
		ctrl.GetWindowRect ( rect ) ;	\
		ctrl.MoveWindow ( rect.left + dx, rect.top + dy, rect.Width (), rect.Height () ) ;	\
	}
#define SFK_RESIZE_EDIT( ctrl, dx, dy )	\
	{	\
		ctrl.GetWindowRect ( rect ) ;	\
		ctrl.ReSize ( rect.left + dx, rect.top + dy, rect.Width (), rect.Height (), 0 ) ;	\
	}
	m_DlgLogin.MoveWindow ( 0, 0, CMyBitmap::GetScreenWidth (), CMyBitmap::GetScreenHeight () ) ;
	SFK_MOVE_CONTROL ( m_DlgLogin.m_LoginRoleImg, 164, 84 ) ;
	SFK_MOVE_CONTROL ( m_DlgLogin.m_LoginEdtImg, 12, 160 ) ;
	SFK_RESIZE_EDIT ( m_DlgLogin.m_LoginAccountEdt, 12, 160 ) ;
	SFK_RESIZE_EDIT ( m_DlgLogin.m_LoginPasswordEdt, 12, 160 ) ;
	SFK_MOVE_CONTROL ( m_DlgLogin.m_LoginExitBtn, 212, 164 ) ;
	SFK_MOVE_CONTROL ( m_DlgLogin.m_LoginOkBtn, 212, 164 ) ;
	SFK_MOVE_CONTROL ( m_DlgLogin.m_LoginNetBtn, 112, 164 ) ;
	SFK_MOVE_CONTROL ( m_DlgLogin.m_LoginNewBtn, 12, 164 ) ;
	SFK_MOVE_CONTROL ( m_DlgLogin.m_LoginRecordBtn, 12, 160 ) ;
	SFK_MOVE_CONTROL ( m_DlgLogin.m_LoginAddBtn, 12, 164 ) ;
	SFK_MOVE_CONTROL ( m_DlgLogin.m_LoginHelpBtn, 112, 164 ) ;
	m_DlgLogin.ReInit () ;

	m_DlgRoleChoose.MoveWindow ( 0, 0, CMyBitmap::GetScreenWidth (), CMyBitmap::GetScreenHeight () ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleChoose.m_RoleChooseReturnBtn, 12, 164 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleChoose.m_HelpBtn, 112, 164 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleChoose.m_RoleChooseOkBtn, 212, 164 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleChoose.m_RoleChooseDataSta, 84, 90 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleChoose.m_RoleChooseMImg1, 46, 38 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleChoose.m_RoleChooseMImg2, 46, 43 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleChoose.m_RoleChooseMImg3, 46, 48 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleChoose.m_RoleChooseMImg4, 46, 54 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleChoose.m_RoleChooseWImg1, 175, 38 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleChoose.m_RoleChooseWImg2, 175, 43 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleChoose.m_RoleChooseWImg3, 175, 48 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleChoose.m_RoleChooseWImg4, 175, 54 ) ;
	m_DlgRoleChoose.ReInit () ;

	m_DlgRoleCreate.MoveWindow ( 0, 0, CMyBitmap::GetScreenWidth (), CMyBitmap::GetScreenHeight () ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleCreate.m_RoleCreateDataSta, 0, 120 ) ;
	SFK_RESIZE_EDIT ( m_DlgRoleCreate.m_RoleCreateNameEdt, 112, 84 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleCreate.m_RoleCreateExitBtn, 12, 164 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleCreate.m_RoleCreateOkBtn, 212, 164 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleCreate.m_RoleCreateReturnBtn, 212, 164 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleCreate.m_HelpBtn, 112, 164 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleCreate.m_Img1, 48, 40 ) ;
	SFK_MOVE_CONTROL ( m_DlgRoleCreate.m_Img2, 150, 40 ) ;
	m_DlgRoleCreate.ReInit () ;
#undef SFK_MOVE_CONTROL
}

BOOL CMyShellDlg::ProcessHotkey(int nKey)
{
	int nMod = 0 ;
	BYTE btState[256] ;
	GetKeyboardState ( btState ) ;
	if ( btState[VK_MENU] >> 7 )
		nMod = HOTKEYF_ALT ;

	if ( btState[VK_SHIFT] >> 7 )
		nMod += HOTKEYF_SHIFT ;

	if ( btState[VK_CONTROL] >> 7 )
		nMod += HOTKEYF_CONTROL ;

	if ( nMod == 0 )
		return false;

	int nIndex = 0 ;
	for ( int i = 0; i < HOTKEY_PARENT_AMOUNT && nIndex == 0; i++ )
	{
		for ( int j = 0; j < HOTKEY_CHILD_AMOUNT; j++ )
		{
			if ( g_nHotkeyValue[i * HOTKEY_CHILD_AMOUNT + j] == nMod * 100 + nKey )
			{
				nIndex = ( i + 1 ) * 10 + j ;
				break ;
			}
		}
	}

	if ( nIndex != 0 )
	{
		switch ( nIndex )
		{
		case 10:
			g_objHero.Emotion ( 230 ) ;
			break ;
		case 11:
			g_objHero.Emotion ( 210 ) ;
			break ;
		case 12:
			g_objHero.Emotion ( 170 ) ;
			break ;
		case 13:
			g_objHero.Emotion ( 150 ) ;
			break ;
		case 14:
			g_objHero.Emotion ( 160 ) ;
			break ;
		case 15:
			g_objHero.Emotion ( 270 ) ;
			break ;
		case 16:
			g_objHero.Emotion ( 1 ) ;
			break ;
		case 17:
			g_objHero.Emotion ( 190 ) ;
			break ;
		case 18:
			g_objHero.Emotion ( 200 ) ;
			break ;
		case 19:
			g_objHero.Emotion ( 250 ) ;
			break ;
		case 20:
			m_DlgMain.OnMainQuery () ;
			m_DlgMain.m_DlgQuery.OnChkRole () ;
			break ;
		case 21:
			m_DlgMain.OnMainQuery () ;
			m_DlgMain.m_DlgQuery.OnChkMission () ;
			break ;
		case 22:
			m_DlgMain.OnMainQuery () ;
			m_DlgMain.m_DlgQuery.OnChkWeapon () ;
			break ;
		case 30:
			m_DlgMain.OnMainFriend () ;
			break ;
		case 31:
			g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10345 ) ) ;
			m_uCmd = CMD_FRDMAKE ;
			break ;
		case 40:
			switch ( g_objHero.GetPkMode () )
			{
			case 0:
				m_DlgMain.m_DlgPk.OnPkGroup () ;
				break ;
			case 1:
				m_DlgMain.m_DlgPk.OnPkSafe () ;
				break ;
			case 2:
				m_DlgMain.m_DlgPk.OnPkArrestment () ;
				break ;
			case 3:
				m_DlgMain.m_DlgPk.OnPkFree () ;
				break ;
			}
			break ;
		case 41:
			m_DlgMain.OnMainFaction();
			break;
		case 42:
			m_DlgMain.OnMainChat();
			break;
		case 43:
			m_DlgMain.OnMainQuery () ;
			m_DlgMain.m_DlgQuery.OnChkSkill () ;
			break ;
		case 44:
			m_DlgMain.OnMainGood () ;
			break ;
		case 45:
			m_DlgMain.OnMainSetup () ;
		case 50:
			if ( m_DlgMain.m_DlgGroup.m_uTeamMode == 0 )
				m_DlgMain.m_DlgGroup.OnGroupBtn1 () ;
			break ;
		case 51:
			if ( m_DlgMain.m_DlgGroup.m_uTeamMode == 1 )
				m_DlgMain.m_DlgGroup.OnGroupBtn4 () ;
			break ;
		case 52:
			if ( m_DlgMain.m_DlgGroup.m_uTeamMode == 1 )
				m_DlgMain.m_DlgGroup.OnGroupBtn2 () ;
			break ;
		case 53:
			if ( m_DlgMain.m_DlgGroup.m_uTeamMode == 0 )
				m_DlgMain.m_DlgGroup.OnGroupBtn2 () ;
			break ;
		case 54:
			if ( m_DlgMain.m_DlgGroup.m_uTeamMode == 2 )
				m_DlgMain.m_DlgGroup.OnGroupBtn1 () ;
			break ;
		case 55:
			if ( m_DlgMain.m_DlgGroup.m_uTeamMode == 1 )
				m_DlgMain.m_DlgGroup.OnGroupBtn1 () ;
			break ;
		case 56:
			break ;
		case 60:
			m_DlgMain.m_RunChk.SetCheckValue ( !m_DlgMain.m_RunChk.GetCheckValue () ) ;
			break ;
		case 61:
			m_DlgMain.OnMainMap () ;
			break ;
		case 62:
			m_DlgMain.m_ScreenMoveChk.SetCheckValue ( !m_DlgMain.m_ScreenMoveChk.GetCheckValue () ) ;
			m_DlgMain.OnMainScreenmove () ;
			break ;
		case 63:
			m_DlgMain.m_DlgTalk.OnTalkBtnClear () ;
			break ;
		default:
			return false ;
		}
		return true ;
	}
	else
		return false ;
}

BOOL CMyShellDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if ( m_bLogin )
	{
		if ( zDelta > 0 )
		{
			if ( m_dwScale > 256 )
			{
				m_dwScale -= 32 ;
				g_objGameMap.SetScale(m_dwScale);
			}
		}
		else
		{
			if ( m_dwScale < 512 )
			{
				m_dwScale += 32 ;
				g_objGameMap.SetScale(m_dwScale);
			}
		}
	}
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CMyShellDlg::FlashInsHelp()
{
	DWORD dwLev = g_objHero.GetLev () ;
	if ( dwLev == 1 || dwLev == 5 || dwLev == 10 || dwLev == 15 || dwLev == 20 || dwLev == 25
		|| dwLev == 31 || dwLev == 36 || dwLev == 41 || dwLev == 48 || dwLev == 53 || dwLev == 59
		|| dwLev == 64 || dwLev == 69 || dwLev == 76 || dwLev == 81 || dwLev == 88 || dwLev == 93
		|| dwLev == 98 || dwLev == 103 || dwLev == 108 || dwLev == 113 || dwLev == 120 )
		m_DlgMain.m_DlgInsHelp.ShowHelpIcon ( true ) ;
	else
		m_DlgMain.m_DlgInsHelp.ShowHelpIcon ( false ) ;
}

void CMyShellDlg::OnBtnSyshide()
{
	if ( g_objGameMap.m_objRadar.GetMode () == 1 )
	{
		g_objGameMap.m_objRadar.SetMode ( 0 ) ;
		m_BtnSysHide.ChangeImage ( "Button00" ) ;
	}
	else
	{
		g_objGameMap.m_objRadar.SetMode ( 1 ) ;
		m_BtnSysHide.ChangeImage ( "Button01" ) ;
	}
}

void CMyShellDlg::OnBtnMsghide()
{
	if ( g_objGameMap.m_objRadar.GetMode () == 2 )
		g_objGameMap.m_objRadar.SetMode ( 0 ) ;
	else
		g_objGameMap.m_objRadar.SetMode ( 2 ) ;
}
