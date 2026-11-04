#include "stdafx.h"
#include "MyShell.h"
#include "DlgHotkey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int	g_nHotkeyValue[HOTKEY_PARENT_AMOUNT * HOTKEY_CHILD_AMOUNT] ;

CDlgHotkey::CDlgHotkey(CWnd* pParent )
	: CDialog(CDlgHotkey::IDD, pParent)
{
	m_Pnt = CPoint ( 0, 0 ) ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_HbrBlack = ::CreateSolidBrush ( 0 ) ;
	m_bShow = false ;
	m_wVirtualKey = 0 ;
	m_wModifiers = 0 ;
	m_szHotKey[0] = 0 ;
}


void CDlgHotkey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOTKEY_BTN_DEL, m_BtnDel);
	DDX_Control(pDX, IDC_HOTKEY, m_HotKey);
	DDX_Control(pDX, IDC_HOTKEY_STA_OLD, m_StaOld);
	DDX_Control(pDX, IDC_HOTKEY_LST_PARENT, m_LstParent);
	DDX_Control(pDX, IDC_HOTKEY_LST_CHILD, m_LstChild);
	DDX_Control(pDX, IDC_HOTKEY_CLOSEB, m_BtnCloseB);
	DDX_Control(pDX, IDC_HOTKEY_CLOSE, m_BtnClose);
	DDX_Control(pDX, IDC_HOTKEY_BTN_REVERT, m_BtnRevert);
	DDX_Control(pDX, IDC_HOTKEY_BTN_CHANGE, m_BtnChange);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
}


BEGIN_MESSAGE_MAP(CDlgHotkey, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_HOTKEY_CLOSE, OnHotkeyClose)
	ON_BN_CLICKED(IDC_HOTKEY_CLOSEB, OnHotkeyCloseb)
	ON_BN_CLICKED(IDC_HOTKEY_BTN_CHANGE, OnHotkeyBtnChange)
	ON_BN_CLICKED(IDC_HOTKEY_BTN_SET, OnHotkeyBtnSet)
	ON_BN_CLICKED(IDC_HOTKEY_BTN_REVERT, OnHotkeyBtnRevert)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_HOTKEY_BTN_DEL, OnHotkeyBtnDel)
END_MESSAGE_MAP()

LRESULT CDlgHotkey::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
		if ( m_bShow )
		{
			m_StaOld.SetWindowText ( NULL ) ;
			CDlgHotkey::OnHotkeyBtnSet () ;
		}
	}
	else if ( message == WM_DESTROY )
	{
		::DeleteObject ( m_Hbr ) ;
		::DeleteObject ( m_HbrBlack ) ;
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
	else if ( message == CB_GETITEMDATA )
	{
		if ( GetFocus ()->m_hWnd == m_LstParent.m_hWnd )
		{
			CDlgHotkey::SetChildList ( lParam ) ;
		}
		else
		{
			int nParent = m_LstParent.GetCurSel () ;
			if ( nParent == -1 )
				nParent = 0 ;
			int nValue = g_nHotkeyValue[nParent * HOTKEY_CHILD_AMOUNT + lParam] ;
			strcpy ( m_szHotKey, GameDataSetQuery()->GetStr ( 10434 ) ) ;
			if ( nValue != -1 )
			{
				char strHotkey[32] ;
				switch ( nValue / 100 )
				{
				case HOTKEYF_ALT:
					strcpy ( strHotkey, "ALT + " ) ;
					break ;
				case HOTKEYF_CONTROL:
					strcpy ( strHotkey, "CTRL + " ) ;
					break ;
				case HOTKEYF_SHIFT:
					strcpy ( strHotkey, "SHIFT + " ) ;
					break ;
				case HOTKEYF_CONTROL + HOTKEYF_ALT:
					strcpy ( strHotkey, "CTRL + ALT + " ) ;
					break ;
				case HOTKEYF_CONTROL + HOTKEYF_SHIFT:
					strcpy ( strHotkey, "CTRL + SHIFT + " ) ;
					break ;
				case HOTKEYF_SHIFT + HOTKEYF_ALT:
					strcpy ( strHotkey, "SHIFT + ALT + " ) ;
					break ;
				}
				int nLen = strlen ( strHotkey ) ;
				strHotkey[ nLen ] = nValue % 100 ;
				strHotkey[ nLen + 1 ] = '\0' ;
				m_StaOld.SetWindowText ( strHotkey ) ;
			}
			else
				m_StaOld.SetWindowText ( GameDataSetQuery()->GetStr ( 10075 ) ) ;
		}
		CDlgHotkey::OnHotkeyBtnSet () ;
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

HBRUSH CDlgHotkey::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_HbrBlack ;
}

BOOL CDlgHotkey::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgHotkey::GetWindowRect ( rect ) ;

	m_LstParent.Init ( rect.left, rect.top, NULL, 1 ) ;
	m_LstParent.SetFontColor ( 0xffffff ) ;
	m_LstParent.SetParentCob ( this ) ;
	m_LstChild.Init ( rect.left, rect.top, NULL, 1 ) ;
	m_LstChild.SetFontColor ( 0xffffff ) ;
	m_LstChild.SetParentCob ( this ) ;
	m_LstParent.AddString ( GameDataSetQuery()->GetStr ( 10300 ) ) ;
	m_LstParent.AddString ( GameDataSetQuery()->GetStr ( 10301 ) ) ;
	m_LstParent.AddString ( GameDataSetQuery()->GetStr ( 10302 ) ) ;
	m_LstParent.AddString ( GameDataSetQuery()->GetStr ( 10303 ) ) ;
	m_LstParent.AddString ( GameDataSetQuery()->GetStr ( 10304 ) ) ;
	m_LstParent.AddString ( GameDataSetQuery()->GetStr ( 10305 ) ) ;
	CDlgHotkey::SetChildList ( 0 ) ;

	m_BtnRevert.Init ( rect.left, rect.top, "Button321" ) ;
	m_BtnChange.Init ( rect.left, rect.top, "Button322" ) ;
	m_BtnDel.Init ( rect.left, rect.top, "Button323" ) ;
	m_BtnHelp.Init ( rect.left, rect.top, "Button4" ) ;
	m_BtnCloseB.Init ( rect.left, rect.top, "Button3" ) ;
	m_BtnClose.Init ( rect.left, rect.top, "Button5" ) ;

	m_StaOld.Init ( rect.left, rect.top ) ;

	m_HotKey.MoveWindow ( 0, 0, 0, 0, true ) ;

	return TRUE;
}

void CDlgHotkey::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgHotkey::Show()
{
	if ( m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog32",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0,
							m_Pnt.x,
							m_Pnt.y ) ;
		else
			return ;

		m_LstParent.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_LstChild.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_BtnRevert.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnChange.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnDel.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnCloseB.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_StaOld.Show ( m_Pnt.x, m_Pnt.y ) ;

		WORD wVirtualKey, wModifiers ;
		m_HotKey.GetHotKey ( wVirtualKey, wModifiers ) ;
		if ( wVirtualKey != m_wVirtualKey || wModifiers != m_wModifiers )
		{
			if ( wModifiers == 0 )
				wModifiers = HOTKEYF_CONTROL ;
			m_wModifiers = wModifiers ;
			m_wVirtualKey = wVirtualKey ;
			switch ( wModifiers )
			{
			case HOTKEYF_ALT:
				strcpy ( m_szHotKey, "ALT + " ) ;
				break ;
			case HOTKEYF_CONTROL:
				strcpy ( m_szHotKey, "CTRL + " ) ;
				break ;
			case HOTKEYF_SHIFT:
				strcpy ( m_szHotKey, "SHIFT + " ) ;
				break ;
			case HOTKEYF_CONTROL + HOTKEYF_ALT:
				strcpy ( m_szHotKey, "CTRL + ALT + " ) ;
				break ;
			case HOTKEYF_CONTROL + HOTKEYF_SHIFT:
				strcpy ( m_szHotKey, "CTRL + SHIFT + " ) ;
				break ;
			case HOTKEYF_SHIFT + HOTKEYF_ALT:
				strcpy ( m_szHotKey, "SHIFT + ALT + " ) ;
				break ;
			}
			if ( wVirtualKey >= '0' && wVirtualKey <= 'Z' )
			{
				int nLen = strlen ( m_szHotKey ) ;
				m_szHotKey[ nLen ] = ( char ) wVirtualKey ;
				m_szHotKey[ nLen + 1 ] = '\0' ;
			}
		}
		CMyBitmap::ShowString ( m_Pnt.x + 246,
								m_Pnt.y + 126,
								0xffffff,
								m_szHotKey ) ;
	}
}

void CDlgHotkey::OnHotkeyClose()
{
	CDlgHotkey::ShowWindow ( SW_HIDE ) ;
}

void CDlgHotkey::OnHotkeyCloseb()
{
	CDlgHotkey::ShowWindow ( SW_HIDE ) ;
}

void CDlgHotkey::OnHotkeyBtnChange()
{
	if ( m_wVirtualKey != 0 && m_wModifiers != 0 )
	{
		int nParent = m_LstParent.GetCurSel () ;
		int nChild = m_LstChild.GetCurSel () ;
		if ( nParent == -1 )
			nParent = 0 ;
		if ( nChild == -1 )
			nChild = 0 ;
		int nValue = m_wModifiers * 100 + m_wVirtualKey ;
		for ( int i = 0; i < HOTKEY_PARENT_AMOUNT; i++ )
		{
			for ( int j = 0; j < HOTKEY_CHILD_AMOUNT; j++ )
			{
				if ( g_nHotkeyValue[j * HOTKEY_CHILD_AMOUNT + i] == nValue )
				{
					AfxMessageBox ( GameDataSetQuery()->GetStr ( 10340 ) ) ;
					return ;
				}
			}
		}
		g_nHotkeyValue[nParent * HOTKEY_CHILD_AMOUNT + nChild] = nValue ;
		m_StaOld.SetWindowText ( m_szHotKey ) ;
	}
}

void CDlgHotkey::OnHotkeyBtnRevert()
{
	if ( m_StaOld.GetWindowTextLength () )
	{
		char strHotkey[32] ;
		m_StaOld.GetWindowText ( strHotkey, 32 ) ;
		strcpy ( m_szHotKey, strHotkey ) ;
	}
}

void CDlgHotkey::OnHotkeyBtnSet()
{
	m_HotKey.SetFocus () ;
	if ( strlen ( m_szHotKey ) == 0 )
		strcpy ( m_szHotKey, GameDataSetQuery()->GetStr ( 10434 ) ) ;
}

void CDlgHotkey::SetChildList(int nParent)
{
	m_LstChild.ResetContent () ;
	switch ( nParent )
	{
	case 0:
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10306 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10307 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10308 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10309 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10310 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10311 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10312 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10313 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10314 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10315 ) ) ;
		break ;
	case 1:
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10316 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10317 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10318 ) ) ;
		break ;
	case 2:
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10319 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10320 ) ) ;
		break ;
	case 3:
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10321 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10322 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10323 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10324 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10344 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10359 ) ) ;
		break ;
	case 4:
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10325 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10326 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10327 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10328 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10329 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10368 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10369 ) ) ;
		break ;
	case 5:
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10330 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10331 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10332 ) ) ;
		m_LstChild.AddString ( GameDataSetQuery()->GetStr ( 10374 ) ) ;
		break ;
	}
}

void CDlgHotkey::OnBtnHelp()
{
	CDlgHotkey::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_HOTKEY ) ;
}

void CDlgHotkey::OnHotkeyBtnDel()
{
	int nParent = m_LstParent.GetCurSel () ;
	int nChild = m_LstChild.GetCurSel () ;
	if ( nParent == -1 )
		nParent = 0 ;
	if ( nChild == -1 )
		nChild = 0 ;
	g_nHotkeyValue[nParent * HOTKEY_CHILD_AMOUNT + nChild] = -1 ;
	m_StaOld.SetWindowText ( GameDataSetQuery()->GetStr ( 10075 ) ) ;
	strcpy ( m_szHotKey, GameDataSetQuery()->GetStr ( 10434 ) ) ;
}
