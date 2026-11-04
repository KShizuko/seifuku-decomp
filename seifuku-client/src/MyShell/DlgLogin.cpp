#include "stdafx.h"
#include "MyShell.h"
#include "DlgLogin.h"

#include "NetMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _SOUND_DISABLE
	#define DXPlaySound
#endif

extern int GameLoginServer(const char* pszAccount, const char* pszPsw, const char* pszServerIP, int nPort, const char* pszServerName);

static BOOL s_bKeyReady = false ;

CDlgLogin::CDlgLogin(CWnd* pParent )
	: CDialog(CDlgLogin::IDD, pParent)
{
	m_bLoad = false ;
	m_bShow = false ;
	m_Hbr = CreateSolidBrush ( 0x2132B ) ;
	m_HbrEdit = CreateSolidBrush ( 0 ) ;
	m_b1024 = false ;
	m_nCurrentFocus = 1 ;
	m_nCurGroup = 1 ;
	m_nGroupBase = 1 ;
}


void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGIN_STA_DESC, m_StaDesc);
	DDX_Control(pDX, IDC_LOGIN_ROLEIMG, m_LoginRoleImg);
	DDX_Control(pDX, IDC_LOGIN_EDTIMG, m_LoginEdtImg);
	DDX_Control(pDX, IDC_LOGIN_BTN_ADD, m_LoginAddBtn);
	DDX_Control(pDX, IDC_LOGIN_EDT_ACCOUNT, m_LoginAccountEdt);
	DDX_Control(pDX, IDC_LOGIN_EDT_PASSWORD, m_LoginPasswordEdt);
	DDX_Control(pDX, IDC_LOGIN_BTN_OK, m_LoginOkBtn);
	DDX_Control(pDX, IDC_LOGIN_BTN_EXIT, m_LoginExitBtn);
	DDX_Control(pDX, IDC_LOGIN_BTN_NEW, m_LoginNewBtn);
	DDX_Control(pDX, IDC_LOGIN_BTN_NET, m_LoginNetBtn);
	DDX_Control(pDX, IDC_LOGIN_BTN_RECORD, m_LoginRecordBtn ) ;
	DDX_Control(pDX, IDC_BTN_HELP, m_LoginHelpBtn);
	DDX_Control(pDX, IDC_LOGIN_GROUP1, m_ImgGroup1);
	DDX_Control(pDX, IDC_LOGIN_GROUP2, m_ImgGroup2);
	DDX_Control(pDX, IDC_LOGIN_GROUP3, m_ImgGroup3);
	DDX_Control(pDX, IDC_LOGIN_GROUP4, m_ImgGroup4);
	DDX_Control(pDX, IDC_LOGIN_SERVER1, m_ImgServer1);
	DDX_Control(pDX, IDC_LOGIN_SERVER2, m_ImgServer2);
	DDX_Control(pDX, IDC_LOGIN_SERVER3, m_ImgServer3);
	DDX_Control(pDX, IDC_LOGIN_SERVER4, m_ImgServer4);
	DDX_Control(pDX, IDC_LOGIN_SERVER5, m_ImgServer5);
	DDX_Control(pDX, IDC_LOGIN_SERVER6, m_ImgServer6);
	DDX_Control(pDX, IDC_LOGIN_SERVER7, m_ImgServer7);
	DDX_Control(pDX, IDC_LOGIN_SERVER8, m_ImgServer8);
	DDX_Control(pDX, IDC_LOGIN_SERVER9, m_ImgServer9);
	DDX_Control(pDX, IDC_LOGIN_SERVER10, m_ImgServer10);
	DDX_Control(pDX, IDC_LOGIN_SERVER11, m_ImgServer11);
	DDX_Control(pDX, IDC_LOGIN_SERVER12, m_ImgServer12);
	DDX_Control(pDX, IDC_STCSVR1, m_StaServer1);
	DDX_Control(pDX, IDC_STCSVR2, m_StaServer2);
	DDX_Control(pDX, IDC_STCSVR3, m_StaServer3);
	DDX_Control(pDX, IDC_STCSVR4, m_StaServer4);
	DDX_Control(pDX, IDC_STCSVR5, m_StaServer5);
	DDX_Control(pDX, IDC_STCSVR6, m_StaServer6);
	DDX_Control(pDX, IDC_STCSVR7, m_StaServer7);
	DDX_Control(pDX, IDC_STCSVR8, m_StaServer8);
	DDX_Control(pDX, IDC_STCSVR9, m_StaServer9);
	DDX_Control(pDX, IDC_STCSVR10, m_StaServer10);
	DDX_Control(pDX, IDC_STCSVR11, m_StaServer11);
	DDX_Control(pDX, IDC_STCSVR12, m_StaServer12);
	DDX_Control(pDX, IDC_LOGIN_BTN_LEFTPAGE, m_BtnLeftPage);
	DDX_Control(pDX, IDC_LOGIN_BTN_RIGHTPAGE, m_BtnRightPage);
}

BEGIN_MESSAGE_MAP(CDlgLogin, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_LOGIN_BTN_OK, OnLoginBtnOk)
	ON_BN_CLICKED(IDC_LOGIN_BTN_EXIT, OnLoginBtnExit)
	ON_BN_CLICKED(IDC_LOGIN_BTN_NEW, OnLoginBtnNew)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_LOGIN_BTN_RECORD, OnLoginBtnRecord)
	ON_BN_CLICKED(IDC_LOGIN_BTN_NET, OnLoginBtnNet)
	ON_EN_SETFOCUS(IDC_LOGIN_EDT_ACCOUNT, OnSetfocusLoginEdtAccount)
	ON_EN_SETFOCUS(IDC_LOGIN_EDT_PASSWORD, OnSetfocusLoginEdtPassword)
	ON_BN_CLICKED(IDC_LOGIN_GROUP1, OnLoginGroup1)
	ON_BN_CLICKED(IDC_LOGIN_SERVER1, OnLoginServer1)
	ON_BN_CLICKED(IDC_LOGIN_SERVER2, OnLoginServer2)
	ON_BN_CLICKED(IDC_LOGIN_SERVER3, OnLoginServer3)
	ON_BN_CLICKED(IDC_LOGIN_BTN_ADD, OnLoginBtnAdd)
	ON_BN_CLICKED(IDC_LOGIN_GROUP2, OnLoginGroup2)
	ON_BN_CLICKED(IDC_LOGIN_GROUP3, OnLoginGroup3)
	ON_BN_CLICKED(IDC_LOGIN_GROUP4, OnLoginGroup4)
	ON_BN_CLICKED(IDC_LOGIN_SERVER4, OnLoginServer4)
	ON_BN_CLICKED(IDC_LOGIN_SERVER5, OnLoginServer5)
	ON_BN_CLICKED(IDC_LOGIN_SERVER6, OnLoginServer6)
	ON_BN_CLICKED(IDC_LOGIN_SERVER7, OnLoginServer7)
	ON_BN_CLICKED(IDC_LOGIN_SERVER8, OnLoginServer8)
	ON_BN_CLICKED(IDC_LOGIN_SERVER9, OnLoginServer9)
	ON_BN_CLICKED(IDC_LOGIN_SERVER10, OnLoginServer10)
	ON_BN_CLICKED(IDC_LOGIN_SERVER11, OnLoginServer11)
	ON_BN_CLICKED(IDC_LOGIN_SERVER12, OnLoginServer12)
	ON_BN_CLICKED(IDC_LOGIN_BTN_LEFTPAGE, OnBtnLeftpage)
	ON_BN_CLICKED(IDC_LOGIN_BTN_RIGHTPAGE, OnBtnRightpage)
END_MESSAGE_MAP()

BOOL CDlgLogin::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;
	m_pGameInputStr = ::GameInputCreate () ;

	switch ( TimeGet () % 9 )
	{
	case 0:
		strcpy ( m_strBkName, "Dialog104" ) ;
		break ;
	case 1:
		strcpy ( m_strBkName, "Dialog101" ) ;
		break ;
	case 2:
		strcpy ( m_strBkName, "Dialog102" ) ;
		break ;
	case 3:
		strcpy ( m_strBkName, "Dialog103" ) ;
		break ;
	case 4:
		strcpy ( m_strBkName, "Dialog105" ) ;
		break ;
	case 5:
		strcpy ( m_strBkName, "Dialog106" ) ;
		break ;
	case 6:
		strcpy ( m_strBkName, "Dialog107" ) ;
		break ;
	case 7:
		strcpy ( m_strBkName, "Dialog108" ) ;
		break ;
	case 8:
		strcpy ( m_strBkName, "Dialog109" ) ;
		break ;
	}

	if ( ! ( m_LoginPasswordEdt.Init ( 0, 0, NULL, 0x000000, 0xffffff ) ) )
		return false ;

	m_LoginPasswordEdt.SetLimitText ( 64 ) ;
	m_LoginPasswordEdt.SetPasswordType ( true ) ;
	m_LoginPasswordEdt.SetPasswordChar ( '*' ) ;
	m_LoginPasswordEdt.SetOffset ( CPoint ( 4, 6 ) ) ;
	memset ( m_szPassword, 0, 32 ) ;

	if ( ! ( m_LoginAccountEdt.Init ( 0, 0, NULL, 0x000000, 0xffffff ) ) )
		return false ;
	m_LoginAccountEdt.SetLimitText ( 64 ) ;
	m_LoginAccountEdt.SetOffset ( CPoint ( 4, 6 ) ) ;

	char strAccount[64] ;
	if ( GetPrivateProfileString ( "Account", "AccountRecord", "", strAccount, 64, g_strSetupIni ) != 0 )
		m_LoginAccountEdt.SetWindowText ( strAccount ) ;

	m_LoginEdtImg.Init ( 0, 0, "Image120", CONTROL_IMAGE ) ;
	m_ImgGroup1.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	m_ImgGroup2.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	m_ImgGroup3.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	m_ImgGroup4.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	m_ImgServer1.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	m_ImgServer2.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	m_ImgServer3.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	m_ImgServer4.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	m_ImgServer5.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	m_ImgServer6.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	m_ImgServer7.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	m_ImgServer8.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	m_ImgServer9.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	m_ImgServer10.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	m_ImgServer11.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	m_ImgServer12.Init ( 0, 0, NULL, SERVER_IMAGE ) ;
	switch ( ( TimeGet () % 10 ) % 5 )
	{
	case 0:
		m_LoginRoleImg.Init ( 0, 0, "Image134", CONTROL_IMAGE ) ;
		break ;
	case 1:
		m_LoginRoleImg.Init ( 0, 0, "Image130", CONTROL_IMAGE ) ;
		break ;
	case 2:
		m_LoginRoleImg.Init ( 0, 0, "Image131", CONTROL_IMAGE ) ;
		break ;
	case 3:
		m_LoginRoleImg.Init ( 0, 0, "Image132", CONTROL_IMAGE ) ;
		break ;
	case 4:
		m_LoginRoleImg.Init ( 0, 0, "Image133", CONTROL_IMAGE ) ;
		break ;
	}

	if ( !m_LoginOkBtn.Init ( 0, 0, "Button13" ) )
		return false ;
	if ( !m_LoginExitBtn.Init ( 0, 0, "Button14" ) )
		return false ;
	if ( !m_LoginNewBtn.Init ( 0, 0, "Button10" ) )
		return false ;
	if ( !m_LoginRecordBtn.Init ( 0, 0, "Button12" ) )
		return false ;
	if ( !m_LoginAddBtn.Init ( 0, 0, "Button16" ) )
		return false ;
	if ( !m_LoginHelpBtn.Init ( 0, 0, "Button15" ) )
		return false ;
	if ( !m_BtnLeftPage.Init ( 0, 0, "Button543" ) )
		return false ;
	if ( !m_BtnRightPage.Init ( 0, 0, "Button544" ) )
		return false ;
	m_StaDesc.Init ( 0, 0, 0, NULL, "NULL" ) ;
	m_StaDesc.SetFontColor ( 0xffffff00 ) ;
	m_StaServer1.Init ( 0, 0, 0, NULL, "NULL" ) ;
	m_StaServer2.Init ( 0, 0, 0, NULL, "NULL" ) ;
	m_StaServer3.Init ( 0, 0, 0, NULL, "NULL" ) ;
	m_StaServer4.Init ( 0, 0, 0, NULL, "NULL" ) ;
	m_StaServer5.Init ( 0, 0, 0, NULL, "NULL" ) ;
	m_StaServer6.Init ( 0, 0, 0, NULL, "NULL" ) ;
	m_StaServer7.Init ( 0, 0, 0, NULL, "NULL" ) ;
	m_StaServer8.Init ( 0, 0, 0, NULL, "NULL" ) ;
	m_StaServer9.Init ( 0, 0, 0, NULL, "NULL" ) ;
	m_StaServer10.Init ( 0, 0, 0, NULL, "NULL" ) ;
	m_StaServer11.Init ( 0, 0, 0, NULL, "NULL" ) ;
	m_StaServer12.Init ( 0, 0, 0, NULL, "NULL" ) ;

	int nOemID = GetPrivateProfileInt ( "Oem", "id", 0, ".\\ini\\oem.ini" ) ;
	if ( nOemID == 0 )
		m_strServerDat = ".\\Server.dat" ;
	else
		m_strServerDat.Format ( ".\\Server%d.dat", nOemID ) ;
	int nGroupAmount = GetPrivateProfileInt ( "Header", "GroupAmount", 0, m_strServerDat ) ;
	if ( nGroupAmount == 0 )
	{
		this->MessageBox( GameDataSetQuery()->GetStr ( 10012 ) ) ;
		return false;
	}

	m_nGroupAmount = nGroupAmount ;
	int nGroup = GetPrivateProfileInt ( "Group", "GroupRecord", -1, g_strSetupIni ) ;
	if ( nGroup != -1 )
	{
		m_nCurGroup = nGroup ;
		if ( nGroup <= 4 )
			m_nGroupBase = 1 ;
		else if ( nGroup >= m_nGroupAmount - 3 )
			m_nGroupBase = m_nGroupAmount - 3 ;
		else
			m_nGroupBase = nGroup ;
		CDlgLogin::SetGroupPage () ;
		switch ( nGroup - m_nGroupBase )
		{
		case 0:
			CDlgLogin::OnLoginGroup1 () ;
			break ;
		case 1:
			CDlgLogin::OnLoginGroup2 () ;
			break ;
		case 2:
			CDlgLogin::OnLoginGroup3 () ;
			break ;
		case 3:
			CDlgLogin::OnLoginGroup4 () ;
			break ;
		}
	}
	else
	{
		m_nGroupBase = 1 ;
		m_nCurGroup = 1 ;
		CDlgLogin::SetGroupPage () ;
		CDlgLogin::OnLoginGroup1 () ;
	}

	int nServer = GetPrivateProfileInt ( "Server", "ServerRecord", -1, g_strSetupIni ) ;
	if ( nServer != -1 )
	{
		sprintf ( m_strServer, "Ip%d", nServer ) ;
		m_nCurrentFocus = nServer ;
		CDlgLogin::SetServerDesc () ;
	}

	m_ServerAdv.Init ( _SCR_WIDTH > 800 ) ;
	CDlgLogin::SetServerAdv () ;

	m_bLoad = true ;

	return TRUE;
}

HBRUSH CDlgLogin::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	if ( nCtlColor == CTLCOLOR_EDIT )
		return m_HbrEdit ;
	return m_Hbr ;
}

void CDlgLogin::Show()
{
	int nSelectBegin, nSelectEnd ;
	if ( CWnd::FromHandle ( ::GetFocus () ) == NULL )
	{
		m_pGameInputStr->Process ( nSelectBegin, nSelectEnd ) ;
		s_bKeyReady = false ;
	}
	if ( CWnd::FromHandle ( ::GetFocus () ) != NULL )
	{
		if ( CWnd::FromHandle ( ::GetFocus () )->m_hWnd == m_LoginAccountEdt.m_hWnd )
		{
			m_LoginAccountEdt.GetSel ( nSelectBegin, nSelectEnd ) ;
			if ( m_pGameInputStr->Process ( nSelectBegin, nSelectEnd ) )
			{
				char strText[256] = "" ;
				strcpy ( strText, m_pGameInputStr->GetStr () ) ;
				m_LoginAccountEdt.SetWindowText ( strText ) ;
				m_LoginAccountEdt.SetSel ( nSelectBegin, nSelectEnd ) ;
			}
			if ( m_pGameInputStr->TestKey ( GI_ENTER ) && s_bKeyReady )
			{
				CDlgLogin::OnLoginBtnOk () ;
				return ;
			}
			if ( m_pGameInputStr->TestKey ( GI_TABLE ) && s_bKeyReady )
				m_LoginPasswordEdt.SetFocus () ;
			else if ( m_pGameInputStr->TestKey ( GI_ESCAPE ) && s_bKeyReady )
				CDlgLogin::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EXIT ) ;
		}
		else if ( CWnd::FromHandle ( ::GetFocus () )->m_hWnd == m_LoginPasswordEdt.m_hWnd )
		{
			m_LoginPasswordEdt.GetSel ( nSelectBegin, nSelectEnd ) ;
			if ( m_pGameInputStr->Process ( nSelectBegin, nSelectEnd ) )
			{
				char strText[256] = "" ;
				strcpy ( strText, m_pGameInputStr->GetStr () ) ;
				CDlgLogin::SetPassword ( strText ) ;
				m_LoginPasswordEdt.SetSel ( nSelectBegin, nSelectEnd ) ;
			}
			m_pGameInputStr->GetStr () ;
			if ( m_pGameInputStr->TestKey ( GI_ENTER ) && s_bKeyReady )
			{
				CDlgLogin::OnLoginBtnOk () ;
				return ;
			}
			if ( m_pGameInputStr->TestKey ( GI_TABLE ) && s_bKeyReady )
				m_LoginAccountEdt.SetFocus () ;
			else if ( m_pGameInputStr->TestKey ( GI_ESCAPE ) && s_bKeyReady )
				CDlgLogin::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EXIT ) ;
		}
		else
			m_pGameInputStr->Process ( nSelectBegin, nSelectEnd ) ;
		s_bKeyReady = true ;
	}

	if ( m_bLoad && m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
												      m_strBkName,
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0, 0, 0 );
		else
			return ;

		m_LoginRoleImg.Show ( 0, 0 ) ;
		m_LoginEdtImg.Show ( 0, 0 ) ;

		m_ImgGroup1.Show ( 0, 0 ) ;
		if ( m_nCurGroup - m_nGroupBase + 1 == 1 )
			m_ImgGroup1.ShowFocus () ;
		m_ImgGroup2.Show ( 0, 0 ) ;
		if ( m_nCurGroup - m_nGroupBase + 1 == 2 )
			m_ImgGroup2.ShowFocus () ;
		m_ImgGroup3.Show ( 0, 0 ) ;
		if ( m_nCurGroup - m_nGroupBase + 1 == 3 )
			m_ImgGroup3.ShowFocus () ;
		m_ImgGroup4.Show ( 0, 0 ) ;
		if ( m_nCurGroup - m_nGroupBase + 1 == 4 )
			m_ImgGroup4.ShowFocus () ;

		m_ImgServer1.Show ( 0, 0 ) ;
		m_ImgServer2.Show ( 0, 0 ) ;
		m_ImgServer3.Show ( 0, 0 ) ;
		m_ImgServer4.Show ( 0, 0 ) ;
		m_ImgServer5.Show ( 0, 0 ) ;
		m_ImgServer6.Show ( 0, 0 ) ;
		m_ImgServer7.Show ( 0, 0 ) ;
		m_ImgServer8.Show ( 0, 0 ) ;
		m_ImgServer9.Show ( 0, 0 ) ;
		m_ImgServer10.Show ( 0, 0 ) ;
		m_ImgServer11.Show ( 0, 0 ) ;
		m_ImgServer12.Show ( 0, 0 ) ;
		switch ( m_nCurrentFocus )
		{
		case 1:
			m_ImgServer1.ShowFocus () ;
			break ;
		case 2:
			m_ImgServer2.ShowFocus () ;
			break ;
		case 3:
			m_ImgServer3.ShowFocus () ;
			break ;
		case 4:
			m_ImgServer4.ShowFocus () ;
			break ;
		case 5:
			m_ImgServer5.ShowFocus () ;
			break ;
		case 6:
			m_ImgServer6.ShowFocus () ;
			break ;
		case 7:
			m_ImgServer7.ShowFocus () ;
			break ;
		case 8:
			m_ImgServer8.ShowFocus () ;
			break ;
		case 9:
			m_ImgServer9.ShowFocus () ;
			break ;
		case 10:
			m_ImgServer10.ShowFocus () ;
			break ;
		case 11:
			m_ImgServer11.ShowFocus () ;
			break ;
		case 12:
			m_ImgServer12.ShowFocus () ;
			break ;
		}

		m_LoginAccountEdt.Show ( -1,-3  ) ;
		m_LoginPasswordEdt.Show (-1, -3) ;

		m_LoginOkBtn.Show ( 0, 0 ) ;
		m_LoginExitBtn.Show ( 0, 0 ) ;
		m_LoginNewBtn.Show ( 0, 0 ) ;
		m_LoginRecordBtn.Show ( 0, 0 ) ;
		m_LoginAddBtn.Show ( 0, 0 ) ;
		m_LoginHelpBtn.Show ( 0, 0 ) ;
		m_BtnLeftPage.Show ( 0, 0 ) ;
		m_BtnRightPage.Show ( 0, 0 ) ;

		m_StaDesc.Show ( 0, 0 ) ;
		static DWORD dwFlashTime = TimeGet () ;
		UINT uFrame = ( ( TimeGet () - dwFlashTime ) / 250 ) & 1 ;
		m_StaServer1.SetCurFrame ( uFrame ) ;
		m_StaServer2.SetCurFrame ( uFrame ) ;
		m_StaServer3.SetCurFrame ( uFrame ) ;
		m_StaServer4.SetCurFrame ( uFrame ) ;
		m_StaServer5.SetCurFrame ( uFrame ) ;
		m_StaServer6.SetCurFrame ( uFrame ) ;
		m_StaServer7.SetCurFrame ( uFrame ) ;
		m_StaServer8.SetCurFrame ( uFrame ) ;
		m_StaServer9.SetCurFrame ( uFrame ) ;
		m_StaServer10.SetCurFrame ( uFrame ) ;
		m_StaServer11.SetCurFrame ( uFrame ) ;
		m_StaServer12.SetCurFrame ( uFrame ) ;
		m_StaServer1.Show ( 0, 0 ) ;
		m_StaServer2.Show ( 0, 0 ) ;
		m_StaServer3.Show ( 0, 0 ) ;
		m_StaServer4.Show ( 0, 0 ) ;
		m_StaServer5.Show ( 0, 0 ) ;
		m_StaServer6.Show ( 0, 0 ) ;
		m_StaServer7.Show ( 0, 0 ) ;
		m_StaServer8.Show ( 0, 0 ) ;
		m_StaServer9.Show ( 0, 0 ) ;
		m_StaServer10.Show ( 0, 0 ) ;
		m_StaServer11.Show ( 0, 0 ) ;
		m_StaServer12.Show ( 0, 0 ) ;

		static int nTime = TimeGet() ;
		if ( TimeGet() - nTime >= 1000 && nTime != 0 && ::GetForegroundWindow () == CDlgLogin::GetParent ()->m_hWnd )
		{
			if ( m_LoginAccountEdt.GetWindowTextLength () > 0 )
				m_LoginPasswordEdt.SetFocus () ;
			else
				m_LoginAccountEdt.SetFocus () ;
			nTime = 0 ;
		}
	}
	CDlgLogin::ShowAdv () ;
}

void CDlgLogin::ShowAdv()
{
	for ( vector<ServerAdvInfo>::iterator it = m_setAdv.begin (); it != m_setAdv.end (); it++ )
	{
		ServerAdvInfo* pInfo = &( *it ) ;
		if ( strlen ( pInfo->szUrl ) > 0 )
		{
			DWORD dwColor = pInfo->dwColor ;
			int nPosX = pInfo->nPosX ;
			int nPosY = pInfo->nPosY ;
			int nWidth = strlen ( pInfo->szText ) * pInfo->nFontSize / 2 ;
			int nHeight = pInfo->nFontSize ;
			int nMouseX = 0 ;
			int nMouseY = 0 ;
			int nEvent = ::MouseCheck ( nMouseX, nMouseY ) ;
			if ( nMouseX >= nPosX && nMouseX < nPosX + nWidth && nMouseY >= nPosY && nMouseY <= nPosY + nHeight )
				dwColor = ( ~pInfo->dwColor ) | 0xff000000 ;
			CMyBitmap::ShowString ( pInfo->nPosX, pInfo->nPosY, dwColor, pInfo->szText, pInfo->szFont, pInfo->nFontSize ) ;
			CMyBitmap::ShowLine ( nPosX, nPosY + nHeight, nPosX + nWidth, nPosY + nHeight, dwColor ) ;
		}
		else
			CMyBitmap::ShowString ( pInfo->nPosX, pInfo->nPosY, pInfo->dwColor, pInfo->szText, pInfo->szFont, pInfo->nFontSize ) ;
	}
}

LRESULT CDlgLogin::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_SHOWWINDOW:
		m_bShow = wParam ;
		if ( wParam )
		{
			m_pGameInputStr->Init ( AfxGetInstanceHandle(), this->GetParent()->GetParent()->m_hWnd, 64 ) ;
		}
		else
		{
			m_pGameInputStr->Destroy () ;
		}
		break ;
	case WM_DESTROY:
		DeleteObject ( m_Hbr ) ;
		DeleteObject ( m_HbrEdit ) ;
		break ;
	case WM_MOUSEMOVE:
		::MouseSet ( ( short ) LOWORD ( lParam ), ( short ) HIWORD ( lParam ), 0 ) ;
		break ;
	case WM_LBUTTONDOWN:
		{
			for ( vector<ServerAdvInfo>::iterator it = m_setAdv.begin (); it != m_setAdv.end (); it++ )
			{
				ServerAdvInfo* pInfo = &( *it ) ;
				if ( strlen ( pInfo->szUrl ) > 0 )
				{
					int nPosX = pInfo->nPosX ;
					int nPosY = pInfo->nPosY ;
					int nWidth = strlen ( pInfo->szText ) * pInfo->nFontSize / 2 ;
					int nHeight = pInfo->nFontSize ;
					int nMouseX = ( short ) LOWORD ( lParam ) ;
					int nMouseY = ( short ) HIWORD ( lParam ) ;
					if ( nMouseX >= nPosX && nMouseX < nPosX + nWidth && nMouseY >= nPosY && nMouseY <= nPosY + nHeight )
						::ShellExecute ( m_hWnd, NULL, pInfo->szUrl, NULL, NULL, SW_SHOW ) ;
				}
			}
			::MouseSet ( ( short ) LOWORD ( lParam ), ( short ) HIWORD ( lParam ), 1 ) ;
		}
		break ;
	case WM_COMMAND:
		if ( wParam == 1 )
		{
			CDlgLogin::OnLoginBtnOk () ;
			return true ;
		}
		else if ( wParam == 2 )
		{
			CDlgLogin::OnLoginBtnExit () ;
			return true ;
		}
		break ;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CDlgLogin::OnLoginBtnOk()
{
	char strText[16] ;
	itoa ( m_nCurGroup, strText, 10 ) ;
	WritePrivateProfileString ( "Group", "GroupRecord", strText, g_strSetupIni ) ;
	itoa ( m_nCurrentFocus, strText, 10 ) ;
	WritePrivateProfileString ( "Server", "ServerRecord", strText, g_strSetupIni ) ;

	DXPlaySound ( "Sound/BtnClick.wav" ) ;

	CString strAccount;
	m_LoginAccountEdt.GetWindowText(strAccount);

	if (strAccount.GetLength() <= 0 || strAccount.GetLength() > 63)
	{
		this->MessageBox ( GameDataSetQuery()->GetStr ( 10013 ),	GameDataSetQuery()->GetStr ( 10017 ), MB_OK|MB_ICONERROR ) ;
		return;
	}

	CString strPsw = m_szPassword ;

	if (strPsw.GetLength() <= 0 || strAccount.GetLength() > 63)
	{
		this->MessageBox ( GameDataSetQuery()->GetStr ( 10014 ),	GameDataSetQuery()->GetStr ( 10017 ), MB_OK|MB_ICONERROR ) ;
		return;
	}

	char szServerIP[256] = "" ;
	if ( GetPrivateProfileString ( m_strGroup, m_strServer, "", szServerIP, 256, m_strServerDat ) == 0 )
	{
		this->MessageBox ( GameDataSetQuery()->GetStr ( 10012 ),	GameDataSetQuery()->GetStr ( 10017 ), MB_OK|MB_ICONERROR ) ;
		return;
	}
	CString strPortKey ;
	strPortKey.Format ( "Port%u", m_nCurrentFocus ) ;
	UINT uPort = GetPrivateProfileInt ( m_strGroup, strPortKey, 0, m_strServerDat ) ;
	if ( uPort == 0 )
	{
		this->MessageBox ( GameDataSetQuery()->GetStr ( 10012 ),	GameDataSetQuery()->GetStr ( 10017 ), MB_OK|MB_ICONERROR ) ;
		return;
	}
	char szTemp[256] = "" ;
	strcpy ( szTemp, m_strServer ) ;
	char szKey[256] = "" ;
	sprintf ( szKey, "Server%s", szTemp + 2 ) ;
	char szServerName[256] = "" ;
	if ( GetPrivateProfileString ( m_strGroup, szKey, "", szServerName, 256, m_strServerDat ) == 0 )
	{
		this->MessageBox ( GameDataSetQuery()->GetStr ( 10012 ),	GameDataSetQuery()->GetStr ( 10017 ), MB_OK|MB_ICONERROR ) ;
		return;
	}

	GameLoginServer((LPCTSTR )strAccount, LPCTSTR (strPsw), szServerIP, uPort, szServerName);
	strcpy( g_szServerName, szServerName );
	CDlgLogin::ShowWindow ( SW_HIDE ) ;
}

void CDlgLogin::OnLoginBtnExit()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
		WPARAM wParam = ON_EXIT ;
	CDlgLogin::GetParent ()->PostMessage ( WM_MY_MESSAGE, wParam ) ;
}

void CDlgLogin::OnLoginBtnNew()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	char szUrl[1024] = "" ;
	if ( IniStrGet ( "ini/oem.ini", "Register", "url", szUrl ) )
		::ShellExecute(m_hWnd, NULL, szUrl, NULL, NULL, SW_SHOW );
	else
		::ShellExecute(m_hWnd, NULL, GameDataSetQuery()->GetStr ( 10366 ), NULL, NULL, SW_SHOW );
}

void CDlgLogin::OnBtnHelp()
{
	CDlgLogin::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_LOGIN ) ;
}

void CDlgLogin::OnLoginBtnRecord()
{
	if ( CDlgLogin::MessageBox ( CMyShellApp::StringChange ( GameDataSetQuery()->GetStr ( 10015 ) ), GameDataSetQuery()->GetStr ( 10016 ), MB_YESNO ) == IDYES )
	{
		char strAccount[16] ;
		m_LoginAccountEdt.GetWindowText ( strAccount, 16 ) ;
		if ( strAccount[0] != 0 )
			WritePrivateProfileString ( "Account", "AccountRecord", strAccount, g_strSetupIni ) ;
	}
}

void CDlgLogin::OnLoginBtnNet()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;

	char szUrl[1024] = "" ;
	if ( IniStrGet ( "ini/oem.ini", "OemHelp", "url", szUrl ) )
		::ShellExecute(m_hWnd, NULL, szUrl, NULL, NULL, SW_SHOW );
	else
		::ShellExecute(m_hWnd, NULL, GameDataSetQuery()->GetStr ( 10367 ), NULL, NULL, SW_SHOW );
}

void CDlgLogin::OnSetfocusLoginEdtAccount()
{
	char strText[16] = "" ;
	m_LoginAccountEdt.GetWindowText ( strText, 16 ) ;
	if ( strText != NULL )
		m_pGameInputStr->SetStr ( strText ) ;
}

void CDlgLogin::OnSetfocusLoginEdtPassword()
{
	char strText[16] = "" ;
	m_LoginPasswordEdt.GetWindowText ( strText, 16 ) ;
	if ( strText != NULL )
		m_pGameInputStr->SetStr ( strText ) ;
}

void CDlgLogin::ReInit()
{
	m_LoginEdtImg.Init ( 0, 0, "Image120", CONTROL_IMAGE ) ;
	switch ( ( TimeGet () % 10 ) % 5 )
	{
	case 0:
		m_LoginRoleImg.Init ( 0, 0, "Image134", CONTROL_IMAGE ) ;
		break ;
	case 1:
		m_LoginRoleImg.Init ( 0, 0, "Image130", CONTROL_IMAGE ) ;
		break ;
	case 2:
		m_LoginRoleImg.Init ( 0, 0, "Image131", CONTROL_IMAGE ) ;
		break ;
	case 3:
		m_LoginRoleImg.Init ( 0, 0, "Image132", CONTROL_IMAGE ) ;
		break ;
	case 4:
		m_LoginRoleImg.Init ( 0, 0, "Image133", CONTROL_IMAGE ) ;
		break ;
	}

	m_LoginPasswordEdt.Init ( 0, 0, NULL, 0x000000, 0xffffff ) ;
	m_LoginAccountEdt.Init ( 0, 0, NULL, 0x000000, 0xffffff  ) ;
	m_LoginOkBtn.Init ( 0, 0, "Button13" ) ;
	m_LoginExitBtn.Init ( 0, 0, "Button14" ) ;
	m_LoginNewBtn.Init ( 0, 0, "Button10" ) ;
	m_LoginNetBtn.Init ( 0, 0, "Button11" ) ;
	m_LoginRecordBtn.Init ( 0, 0, "Button12" ) ;
	m_LoginAddBtn.Init ( 0, 0, "Button16" ) ;
	m_LoginHelpBtn.Init ( 0, 0, "Button15" ) ;
	m_BtnLeftPage.Init ( 0, 0, "Button543" ) ;
	m_BtnRightPage.Init ( 0, 0, "Button544" ) ;
	m_b1024 = true ;
}

void CDlgLogin::OnLoginBtnAdd()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	char szUrl[1024] = "" ;
	if ( IniStrGet ( "ini/oem.ini", "ReCharge", "url", szUrl ) )
		::ShellExecute(m_hWnd, NULL, szUrl, NULL, NULL, SW_SHOW );
	else
		::ShellExecute(m_hWnd, NULL, GameDataSetQuery()->GetStr ( 10377 ), NULL, NULL, SW_SHOW );
}


void CDlgLogin::LoginGroup(int nIndex)
{
	if ( nIndex < 1 || nIndex > m_nGroupAmount || nIndex >= m_nGroupBase + 4 )
		return ;
	CDlgLogin::ResetServerStatus();
	sprintf ( m_strGroup, "Group%d", nIndex ) ;
	int nServerAmount = GetPrivateProfileInt ( m_strGroup, "ServerAmount", 0, m_strServerDat ) ;
	if ( nServerAmount == 0 )
	{
		this->MessageBox ( GameDataSetQuery()->GetStr ( 10012 ),	GameDataSetQuery()->GetStr ( 10017 ), MB_OK|MB_ICONERROR ) ;
		return ;
	}

	m_SerSta.FlashStatus () ;
	char strKey[32] = "" ;
	for ( int i = 1; i <= nServerAmount; i++ )
	{
		sprintf ( strKey, "Pic%d", i ) ;
		char strPicID[32] = "" ;
		GetPrivateProfileString ( m_strGroup, strKey, "", strPicID, 32, m_strServerDat ) ;
		sprintf ( strKey, "ServerName%d", i ) ;
		char strText[32] = "" ;
		GetPrivateProfileString ( m_strGroup, strKey, "", strText, 32, m_strServerDat ) ;
		sprintf ( strKey, "Server%d", i ) ;
		char strServer[256] = "" ;
		GetPrivateProfileString ( m_strGroup, strKey, "", strServer, 32, m_strServerDat ) ;
		int nSerSta = m_SerSta.GetServerStatus ( strServer ) ;
		CDlgLogin::InsertServer ( i, strPicID, strText, nSerSta ) ;
	}
	m_nCurGroup = nIndex ;
	CDlgLogin::OnLoginServer1 () ;
	CDlgLogin::SetServerAdv () ;
}

void CDlgLogin::OnLoginGroup1()
{
	CDlgLogin::LoginGroup ( m_nGroupBase ) ;
}

void CDlgLogin::SetServerAdv()
{
	char szGroup[256] = "" ;
	sprintf ( szGroup, "Group%d", m_nCurGroup ) ;
	char szKey[256] = "" ;
	sprintf ( szKey, "Server%d", m_nCurrentFocus ) ;
	char szServerName[256] = "" ;
	if ( GetPrivateProfileString ( szGroup, szKey, "", szServerName, 256, "./Server.dat" ) != 0 )
		m_ServerAdv.GetAdv ( szServerName, m_setAdv ) ;
}

void CDlgLogin::SetGroupPage()
{
	int nEnd = ( m_nGroupAmount > m_nGroupBase + 3 ) ? m_nGroupBase + 3 : m_nGroupAmount ;
	for ( int i = m_nGroupBase; i <= nEnd; i++ )
	{
		char strKey[16] ;
		sprintf ( strKey, "Group%d", i ) ;
		char strPicID[32] = "" ;
		if ( GetPrivateProfileString ( "Header", strKey, "", strPicID, 32, m_strServerDat ) != 0 )
		{
			switch ( i - m_nGroupBase )
			{
			case 0:
				m_ImgGroup1.InsertImage ( strPicID ) ;
				break ;
			case 1:
				m_ImgGroup2.InsertImage ( strPicID ) ;
				break ;
			case 2:
				m_ImgGroup3.InsertImage ( strPicID ) ;
				break ;
			case 3:
				m_ImgGroup4.InsertImage ( strPicID ) ;
				break ;
			}
		}
	}
}

void CDlgLogin::OnBtnLeftpage()
{
	if ( m_nGroupBase <= 1 )
		return ;
	m_nGroupBase-- ;
	CDlgLogin::SetGroupPage () ;
	if ( m_nCurGroup > m_nGroupBase + 3 )
	{
		m_nCurGroup-- ;
		CDlgLogin::LoginGroup ( m_nCurGroup ) ;
	}
}

void CDlgLogin::OnBtnRightpage()
{
	if ( m_nGroupBase + 3 >= m_nGroupAmount )
		return ;
	m_nGroupBase++ ;
	CDlgLogin::SetGroupPage () ;
	if ( m_nCurGroup < m_nGroupBase )
	{
		m_nCurGroup++ ;
		CDlgLogin::LoginGroup ( m_nCurGroup ) ;
	}
}

void CDlgLogin::OnLoginServer1()
{
	if ( m_ImgServer1.GetImageLoadState () )
	{
		strcpy ( m_strServer, "Ip1" ) ;
		m_nCurrentFocus = 1 ;
		CDlgLogin::SetServerDesc () ;
		CDlgLogin::SetServerAdv () ;
	}
}

void CDlgLogin::InsertServer(int nIndex, char *strPicID, char *strText, int nSerSta)
{
	switch(nIndex)
	{
	case 1:
		m_ImgServer1.InsertImage ( strPicID ) ;
		m_StaServer1.SetWindowText ( strText ) ;
		m_StaServer1.SetLightType ( nSerSta ) ;
		break;
	case 2:
		m_ImgServer2.InsertImage ( strPicID ) ;
		m_StaServer2.SetWindowText ( strText ) ;
		m_StaServer2.SetLightType ( nSerSta ) ;
		break;
	case 3:
		m_ImgServer3.InsertImage ( strPicID ) ;
		m_StaServer3.SetWindowText ( strText ) ;
		m_StaServer3.SetLightType ( nSerSta ) ;
		break;
	case 4:
		m_ImgServer4.InsertImage ( strPicID ) ;
		m_StaServer4.SetWindowText ( strText ) ;
		m_StaServer4.SetLightType ( nSerSta ) ;
		break;
	case 5:
		m_ImgServer5.InsertImage ( strPicID ) ;
		m_StaServer5.SetWindowText ( strText ) ;
		m_StaServer5.SetLightType ( nSerSta ) ;
		break;
	case 6:
		m_ImgServer6.InsertImage ( strPicID ) ;
		m_StaServer6.SetWindowText ( strText ) ;
		m_StaServer6.SetLightType ( nSerSta ) ;
		break;
	case 7:
		m_ImgServer7.InsertImage ( strPicID ) ;
		m_StaServer7.SetWindowText ( strText ) ;
		m_StaServer7.SetLightType ( nSerSta ) ;
		break;
	case 8:
		m_ImgServer8.InsertImage ( strPicID ) ;
		m_StaServer8.SetWindowText ( strText ) ;
		m_StaServer8.SetLightType ( nSerSta ) ;
		break;
	case 9:
		m_ImgServer9.InsertImage ( strPicID ) ;
		m_StaServer9.SetWindowText ( strText ) ;
		m_StaServer9.SetLightType ( nSerSta ) ;
		break;
	case 10:
		m_ImgServer10.InsertImage ( strPicID ) ;
		m_StaServer10.SetWindowText ( strText ) ;
		m_StaServer10.SetLightType ( nSerSta ) ;
		break;
	case 11:
		m_ImgServer11.InsertImage ( strPicID ) ;
		m_StaServer11.SetWindowText ( strText ) ;
		m_StaServer11.SetLightType ( nSerSta ) ;
		break;
	case 12:
		m_ImgServer12.InsertImage ( strPicID ) ;
		m_StaServer12.SetWindowText ( strText ) ;
		m_StaServer12.SetLightType ( nSerSta ) ;
		break;
	default:
		break;
	}
}

void CDlgLogin::ResetServerStatus()
{
	m_StaDesc.SetWindowText ( NULL ) ;
	m_StaServer1.SetWindowText ( NULL ) ;
	m_StaServer2.SetWindowText ( NULL ) ;
	m_StaServer3.SetWindowText ( NULL ) ;
	m_StaServer4.SetWindowText ( NULL ) ;
	m_StaServer5.SetWindowText ( NULL ) ;
	m_StaServer6.SetWindowText ( NULL ) ;
	m_StaServer7.SetWindowText ( NULL ) ;
	m_StaServer8.SetWindowText ( NULL ) ;
	m_StaServer9.SetWindowText ( NULL ) ;
	m_StaServer10.SetWindowText ( NULL ) ;
	m_StaServer11.SetWindowText ( NULL ) ;
	m_StaServer12.SetWindowText ( NULL ) ;
	m_StaServer1.SetLightType ( -1 ) ;
	m_StaServer2.SetLightType ( -1 ) ;
	m_StaServer3.SetLightType ( -1 ) ;
	m_StaServer4.SetLightType ( -1 ) ;
	m_StaServer5.SetLightType ( -1 ) ;
	m_StaServer6.SetLightType ( -1 ) ;
	m_StaServer7.SetLightType ( -1 ) ;
	m_StaServer8.SetLightType ( -1 ) ;
	m_StaServer9.SetLightType ( -1 ) ;
	m_StaServer10.SetLightType ( -1 ) ;
	m_StaServer11.SetLightType ( -1 ) ;
	m_StaServer12.SetLightType ( -1 ) ;
	m_ImgServer1.RemoveImage () ;
	m_ImgServer2.RemoveImage () ;
	m_ImgServer3.RemoveImage () ;
	m_ImgServer4.RemoveImage () ;
	m_ImgServer5.RemoveImage () ;
	m_ImgServer6.RemoveImage () ;
	m_ImgServer7.RemoveImage () ;
	m_ImgServer8.RemoveImage () ;
	m_ImgServer9.RemoveImage () ;
	m_ImgServer10.RemoveImage () ;
	m_ImgServer11.RemoveImage () ;
	m_ImgServer12.RemoveImage () ;
}

void CDlgLogin::OnLoginGroup2()
{
	CDlgLogin::LoginGroup ( m_nGroupBase + 1 ) ;
}

void CDlgLogin::OnLoginGroup3()
{
	CDlgLogin::LoginGroup ( m_nGroupBase + 2 ) ;
}

void CDlgLogin::OnLoginGroup4()
{
	CDlgLogin::LoginGroup ( m_nGroupBase + 3 ) ;
}

void CDlgLogin::OnLoginServer2()
{
	if ( m_ImgServer2.GetImageLoadState () )
	{
		strcpy ( m_strServer, "Ip2" ) ;
		m_nCurrentFocus = 2 ;
		CDlgLogin::SetServerDesc () ;
		CDlgLogin::SetServerAdv () ;
	}
}

void CDlgLogin::OnLoginServer3()
{
	if ( m_ImgServer3.GetImageLoadState () )
	{
		strcpy ( m_strServer, "Ip3" ) ;
		m_nCurrentFocus = 3 ;
		CDlgLogin::SetServerDesc () ;
		CDlgLogin::SetServerAdv () ;
	}
}

void CDlgLogin::OnLoginServer4()
{
	if ( m_ImgServer4.GetImageLoadState () )
	{
		strcpy ( m_strServer, "Ip4" ) ;
		m_nCurrentFocus = 4 ;
		CDlgLogin::SetServerDesc () ;
		CDlgLogin::SetServerAdv () ;
	}
}

void CDlgLogin::OnLoginServer5()
{
	if ( m_ImgServer5.GetImageLoadState () )
	{
		strcpy ( m_strServer, "Ip5" ) ;
		m_nCurrentFocus = 5 ;
		CDlgLogin::SetServerDesc () ;
		CDlgLogin::SetServerAdv () ;
	}
}

void CDlgLogin::OnLoginServer6()
{
	if ( m_ImgServer6.GetImageLoadState () )
	{
		strcpy ( m_strServer, "Ip6" ) ;
		m_nCurrentFocus = 6 ;
		CDlgLogin::SetServerDesc () ;
		CDlgLogin::SetServerAdv () ;
	}
}

void CDlgLogin::OnLoginServer7()
{
	if ( m_ImgServer7.GetImageLoadState () )
	{
		strcpy ( m_strServer, "Ip7" ) ;
		m_nCurrentFocus = 7 ;
		CDlgLogin::SetServerDesc () ;
		CDlgLogin::SetServerAdv () ;
	}
}

void CDlgLogin::OnLoginServer8()
{
	if ( m_ImgServer8.GetImageLoadState () )
	{
		strcpy ( m_strServer, "Ip8" ) ;
		m_nCurrentFocus = 8 ;
		CDlgLogin::SetServerDesc () ;
		CDlgLogin::SetServerAdv () ;
	}
}

void CDlgLogin::OnLoginServer9()
{
	if ( m_ImgServer9.GetImageLoadState () )
	{
		strcpy ( m_strServer, "Ip9" ) ;
		m_nCurrentFocus = 9 ;
		CDlgLogin::SetServerDesc () ;
		CDlgLogin::SetServerAdv () ;
	}
}

void CDlgLogin::OnLoginServer10()
{
	if ( m_ImgServer10.GetImageLoadState () )
	{
		strcpy ( m_strServer, "Ip10" ) ;
		m_nCurrentFocus = 10 ;
		CDlgLogin::SetServerDesc () ;
		CDlgLogin::SetServerAdv () ;
	}
}

void CDlgLogin::OnLoginServer11()
{
	if ( m_ImgServer11.GetImageLoadState () )
	{
		strcpy ( m_strServer, "Ip11" ) ;
		m_nCurrentFocus = 11 ;
		CDlgLogin::SetServerDesc () ;
		CDlgLogin::SetServerAdv () ;
	}
}

void CDlgLogin::OnLoginServer12()
{
	if ( m_ImgServer12.GetImageLoadState () )
	{
		strcpy ( m_strServer, "Ip12" ) ;
		m_nCurrentFocus = 12 ;
		CDlgLogin::SetServerDesc () ;
		CDlgLogin::SetServerAdv () ;
	}
}

void CDlgLogin::SetServerDesc()
{
	char strSection[16] ;
	char strKey[16] ;
	char strText[256] ;
	sprintf ( strSection, "Group%d", m_nCurGroup ) ;
	sprintf ( strKey, "HintWord%d", m_nCurrentFocus ) ;
	GetPrivateProfileString ( strSection, strKey, "", strText, 256, m_strServerDat ) ;
	m_StaDesc.SetWindowText ( strText ) ;
}

void CDlgLogin::SetAccount ( const char* pszAccount, const char* pszPassword )
{
	if ( pszAccount != NULL && pszPassword != NULL )
	{
		m_LoginAccountEdt.SetWindowText ( pszAccount ) ;
		CDlgLogin::SetPassword ( pszPassword ) ;
		m_pGameInputStr->SetStr ( ( char* )pszPassword ) ;
	}
}

void CDlgLogin::SetPassword ( const char* pszPassword )
{
	if ( pszPassword != NULL )
	{
		strcpy ( m_szPassword, pszPassword ) ;
		char szStars[256] = "" ;
		int nLen = strlen ( pszPassword ) ;
		for ( int i = 0; i < nLen; i++ )
			szStars[i] = '*' ;
		m_LoginPasswordEdt.SetWindowText ( szStars ) ;
	}
}
