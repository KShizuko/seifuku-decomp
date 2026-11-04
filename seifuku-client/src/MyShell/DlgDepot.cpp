#include "stdafx.h"
#include "MyShell.h"
#include "DlgDepot.h"

#include "Hero.h"
#include "GameMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef _SOUND_DISABLE
	#define DXPlaySound
#endif

CDlgDepot::CDlgDepot(CWnd* pParent )
	: CDialog(CDlgDepot::IDD, pParent)
{
	m_bShow = false ;
	m_nCurPage = 0 ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_HbrBlack = ::CreateSolidBrush ( 0 ) ;
	m_bInitSuc = false ;
	m_fFrameStep = 0.0f ;
	m_nFrameMode = 0 ;
	m_nPntX = 0 ;
	m_nMoney = 0 ;
}


void CDlgDepot::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEPOT_STA_PAGE, m_DepotStaPage);
	DDX_Control(pDX, IDC_DEPOT_STA_ALLPAGE, m_DepotStaAllPage);
	DDX_Control(pDX, IDC_DEPOT_BTN_U, m_DepotBtnU);
	DDX_Control(pDX, IDC_DEPOT_BTN_D, m_DepotBtnD);
	DDX_Control(pDX, IDC_DEPOT_CLOSEB, m_DepotCloseB);
	DDX_Control(pDX, IDC_DEPOT_BTN_ADD, m_DepotBtnAdd ) ;
	DDX_Control(pDX, IDC_DEPOT_BTN_GET, m_DepotBtnGet ) ;
	DDX_Control(pDX, IDC_DEPOT_CLOSE, m_DepotClose ) ;
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp ) ;
	DDX_Control(pDX, IDC_DEPOT_EDT_MONEY, m_DepotEdtMoney ) ;
	DDX_Control(pDX, IDC_DEPOT_STA_LAY, m_DepotStaLay ) ;
	DDX_Control(pDX, IDC_DEPOT_GRI_LAY, m_DepotGriLay ) ;
}


BEGIN_MESSAGE_MAP(CDlgDepot, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_DEPOT_CLOSE, OnDepotClose)
	ON_BN_CLICKED(IDC_DEPOT_BTN_GET, OnDepotBtnGet)
	ON_BN_CLICKED(IDC_DEPOT_BTN_ADD, OnDepotBtnAdd)
	ON_BN_CLICKED(IDC_DEPOT_CLOSEB, OnDepotCloseb)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_DEPOT_BTN_U, OnDepotBtnU)
	ON_BN_CLICKED(IDC_DEPOT_BTN_D, OnDepotBtnD)
	ON_EN_CHANGE(IDC_DEPOT_EDT_MONEY, OnChangeDepotEdtMoney)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

LRESULT CDlgDepot::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
		if ( wParam )
		{
			CRect rect ;
			m_nChange = 0 ;
			CDlgDepot::GetWindowRect ( rect ) ;
			if ( m_nFrameMode == 0 )
			{
				DXPlaySound ( "Sound/DlgOpen.wav" ) ;
				m_dwLastShowTime = 0 ;
				m_nFrameMode = 1 ;
				CDlgDepot::MoveWindow ( -rect.Width(), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
			}
			else if ( m_nFrameMode == 2 )
			{
				CDlgDepot::MoveWindow ( 0, m_Pnt.y, rect.Width(), rect.Height(), true ) ;
				m_nFrameMode = 0 ;
				m_DepotEdtMoney.SetFocus () ;
			}
		}
		else
		{
			m_nCurPage = 0 ;
			m_DepotGriLay.RemoveIcon ( 0, 0, true ) ;
			DXPlaySound ( "Sound/DlgClose.wav" ) ;
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
			CDlgDepot::OnDepotClose () ;
			return true ;
		}
	}
	else if ( message == WM_DESTROY )
	{
		::DeleteObject ( m_Hbr ) ;
		::DeleteObject ( m_HbrBlack ) ;
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

HBRUSH CDlgDepot::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	if ( nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_STATIC )
		return m_HbrBlack ;
	return m_Hbr ;
}

BOOL CDlgDepot::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgDepot::GetWindowRect ( rect ) ;
	m_fFrameStep = ( float ) rect.Width () / 20 ;

	if ( ! m_DepotBtnAdd.Init( rect.left, rect.top, "Button180" ) )
		return false ;
	if ( ! m_DepotBtnGet.Init( rect.left, rect.top, "Button181" ) )
		return false ;
	if ( ! m_DepotClose.Init( rect.left, rect.top, "Button5" ) )
		return false ;
	if ( ! m_DepotCloseB.Init( rect.left, rect.top, "Button3" ) )
		return false ;
	if ( ! m_BtnHelp.Init( rect.left, rect.top, "Button4" ) )
		return false ;
	m_DepotBtnU.Init( rect.left, rect.top, "Button203" ) ;
	m_DepotBtnD.Init( rect.left, rect.top, "Button204" ) ;

	if ( ! m_DepotEdtMoney.Init ( rect.left, rect.top, NULL, 0x000000, 0xffffff ) )
		return false ;
	m_DepotEdtMoney.SetOffset ( CPoint ( 4, 3 ) ) ;
	m_DepotEdtMoney.SetLimitText ( 12 ) ;

	if ( ! m_DepotStaLay.Init ( rect.left, rect.top, 0, NULL, "NULL" ) )
		return false ;
	m_DepotStaLay.SetStaticPosType ( 2 ) ;
	m_DepotStaPage.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_DepotStaAllPage.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;

	m_DepotGriLay.MoveWindow ( 16, 168, 214, 172, true ) ;
	if ( ! m_DepotGriLay.Init( rect.left, rect.top, 4, 5, DLG_DEPOT ) )
		return false ;

	m_bInitSuc = true ;
	return TRUE;
}

void CDlgDepot::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_nPntX = x ;
	m_Pnt.y = y ;
}

void CDlgDepot::Show()
{
	if ( !m_bInitSuc )
		return ;
	if ( m_bShow )
	{
		if ( m_nFrameMode == 1 )
		{
			if ( m_dwLastShowTime == 0 )
			{
				m_dwLastShowTime = TimeGet() ;
				m_Pnt.x = m_nPntX ;
			}
			else
			{
				if ( TimeGet() - m_dwLastShowTime <= 300 )
				{
					m_Pnt.x = m_nPntX ;
					m_Pnt.x += ( TimeGet() - m_dwLastShowTime ) / 15 * m_fFrameStep ;
				}
				else
				{
					m_Pnt.x = 0;
					m_nFrameMode = 2 ;
					CDlgDepot::WindowProc ( WM_SHOWWINDOW, true, NULL ) ;
				}
			}
		}

		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														  "Dialog18",
														  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			ShowAni->Show( 0,
						   m_Pnt.x ,
						   m_Pnt.y  );
		}
		else
			return ;

		m_DepotBtnAdd.Show( m_Pnt.x, m_Pnt.y ) ;
		m_DepotBtnGet.Show( m_Pnt.x, m_Pnt.y ) ;
		m_DepotClose.Show( m_Pnt.x, m_Pnt.y ) ;
		m_DepotCloseB.Show( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show( m_Pnt.x, m_Pnt.y ) ;
		if ( m_DepotBtnU.IsWindowEnabled () )
			m_DepotBtnU.Show( m_Pnt.x, m_Pnt.y ) ;
		if ( m_DepotBtnD.IsWindowEnabled () )
			m_DepotBtnD.Show( m_Pnt.x, m_Pnt.y ) ;

		m_DepotEdtMoney.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_DepotStaLay.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_DepotStaPage.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_DepotStaAllPage.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_DepotGriLay.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgDepot::OnDepotClose()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	CloseDepot() ;
	CDlgDepot::ShowWindow ( SW_HIDE ) ;
	CDlgDepot::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_NPCBUY_CLOSE ) ;
}

BOOL CDlgDepot::OpenDepot(OBJID DepotID)
{
	g_objHero.OpenBank ( DepotID ) ;
	g_objHero.OpenPackage ( DepotID, PACKAGE_STORAGE ) ;
	m_DepotEdtMoney.SetWindowText ( NULL ) ;
	m_nCurPage = 0 ;
	return true ;
}

void CDlgDepot::SetMoney()
{
	DWORD dwMoney = g_objHero.GetSavedMoney ( ) ;
	char strMoney[16] ;
	itoa ( dwMoney, strMoney, 10 ) ;
	if ( strcmp ( strMoney, "0" ) == 0 )
	{
		m_DepotStaLay.SetWindowText ( NULL ) ;
		return ;
	}
	int nLen = strlen ( strMoney ) ;
	if ( strlen ( strMoney ) > 3 && strlen ( strMoney ) < 7 )
	{
		CString strText = strMoney ;
		strText.Insert ( strlen ( strMoney ) - 3, ',' ) ;
		m_DepotStaLay.SetWindowText ( strText ) ;
	}
	else if ( strlen ( strMoney ) > 6 && strlen ( strMoney ) < 10 )
	{
		CString strText = strMoney ;
		strText.Insert ( strlen ( strMoney ) - 3, ',' ) ;
		strText.Insert ( strlen ( strMoney ) - 6, ',' ) ;
		m_DepotStaLay.SetWindowText ( strText ) ;
	}
	else if ( strlen ( strMoney ) > 9 && strlen ( strMoney ) < 13 )
	{
		CString strText = strMoney ;
		strText.Insert ( strlen ( strMoney ) - 3, ',' ) ;
		strText.Insert ( strlen ( strMoney ) - 7, ',' ) ;
		strText.Insert ( strlen ( strMoney ) - 10, ',' ) ;
		m_DepotStaLay.SetWindowText ( strText ) ;
	}
	else
		m_DepotStaLay.SetWindowText ( strMoney ) ;
}

void CDlgDepot::SetItem()
{
	m_DepotBtnU.EnableWindow ( false ) ;
	m_DepotBtnD.EnableWindow ( false ) ;
	if ( g_objHero.GetPackageMaxItemAmount () / DEPOT_PAGE_AMOUNT > 1 )
	{
		m_DepotBtnU.EnableWindow ( true ) ;
		m_DepotBtnD.EnableWindow ( true ) ;
	}
	m_DepotGriLay.RemoveIcon ( 0, 0, true ) ;
	int dwItemAmount = g_objHero.GetPackageItemAmount() ;
	char strText[16] ;
	itoa ( ( dwItemAmount + DEPOT_PAGE_AMOUNT - 1 ) / DEPOT_PAGE_AMOUNT, strText, 10 ) ;
	m_DepotStaAllPage.SetWindowText ( strText ) ;
	if ( dwItemAmount > ( m_nCurPage + 1 ) * DEPOT_PAGE_AMOUNT )
		m_DepotBtnD.SetCurFrame ( 2 ) ;
	dwItemAmount = ( dwItemAmount > ( m_nCurPage + 1 ) * DEPOT_PAGE_AMOUNT ) ? ( m_nCurPage + 1 ) * DEPOT_PAGE_AMOUNT : dwItemAmount ;
	itoa ( m_nCurPage + 1, strText, 10 ) ;
	m_DepotStaPage.SetWindowText ( strText ) ;
	if ( dwItemAmount > 0 )
	{
		for ( int i = m_nCurPage * DEPOT_PAGE_AMOUNT; i < dwItemAmount; i++ )
		{
			CItem* pItem = g_objHero.GetPackageItemByIndex ( i ) ;
			if ( pItem != NULL )
			{
				OBJID ItemTypeID = pItem->GetTypeID () ;
				BYTE btActType = 0 ;
				if ( ItemTypeID < 1000000 && ( ItemTypeID < 700000 || ItemTypeID > 800000 ) )
				{
					switch ( ItemTypeID % 10 )
					{
					case 9:
						btActType = 1 ;
						break ;
					case 8:
						btActType = 2 ;
						break ;
					case 7:
						btActType = 3 ;
						break ;
					case 6:
						btActType = 4 ;
						break ;
					}
				}
				if ( ItemTypeID >= 700001 && ItemTypeID <= 700063 )
					btActType = 5 ;
				if ( ItemTypeID >= 1088000 && ItemTypeID <= 1088001 )
					btActType = 6 ;
				m_DepotGriLay.InsertIcon( 1, 1, ItemTypeID, i - m_nCurPage * DEPOT_PAGE_AMOUNT, ICON_TYPE_ITEM, btActType ) ;
			}
		}
	}
}

void CDlgDepot::CloseDepot()
{
	g_objHero.CloseBank () ;
	g_objHero.ClosePackage () ;
}

void CDlgDepot::OnDepotBtnGet()
{
	if ( m_nMoney > 0 )
	{
		g_objHero.DrawMoney( m_nMoney ) ;
	}
	m_DepotEdtMoney.SetFocus () ;
}

void CDlgDepot::OnDepotBtnAdd()
{
	if ( m_nMoney > 0 && ( DWORD ) m_nMoney <= g_objHero.GetMoney () )
	{
		g_objHero.SaveMoney ( m_nMoney ) ;
	}
	m_DepotEdtMoney.SetFocus () ;
}

void CDlgDepot::OnBtnHelp()
{
	CDlgDepot::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_DEPOT ) ;
}

void CDlgDepot::OnDepotBtnU()
{
	if ( m_nCurPage > 0 )
	{
		m_nCurPage-- ;
		CDlgDepot::SetItem () ;
	}
}

void CDlgDepot::OnDepotBtnD()
{
	if ( m_nCurPage + 1 < g_objHero.GetPackageItemAmount () / DEPOT_PAGE_AMOUNT + 1 && m_nCurPage < 1 )
	{
		m_nCurPage++ ;
		CDlgDepot::SetItem () ;
	}
}

void CDlgDepot::OnDepotCloseb()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	CloseDepot() ;
	CDlgDepot::ShowWindow ( SW_HIDE ) ;
	CDlgDepot::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_NPCBUY_CLOSE ) ;
}

void CDlgDepot::OnChangeDepotEdtMoney()
{
	char strMoney[12] ;
	m_DepotEdtMoney.GetWindowText ( strMoney, 12 ) ;
	if ( strcmp ( strMoney, "0" ) == 0 )
	{
		m_DepotEdtMoney.SetWindowText ( NULL ) ;
		return ;
	}
	m_nChange++ ;
	if ( m_nChange % 2 == 0 )
		return ;
	int nLen = m_DepotEdtMoney.GetWindowTextLength () ;
	if ( nLen >= 3 )
	{
		char* pszComma = strchr ( strMoney, ',' ) ;
		if ( pszComma )
		{
			char strTemp[12] ;
			char strNum[12] ;
			strncpy ( strTemp, strMoney, pszComma - strMoney ) ;
			strTemp[pszComma - strMoney] = 0 ;
			strcat ( strTemp, pszComma + 1 ) ;
			pszComma = strchr ( strTemp, ',' ) ;
			if ( pszComma )
			{
				strncpy ( strMoney, strTemp, pszComma - strTemp ) ;
				strMoney[pszComma - strTemp] = 0 ;
				strcat ( strMoney, pszComma + 1 ) ;
				strcpy ( strNum, strMoney ) ;
			}
			else
				strcpy ( strNum, strTemp ) ;
			m_nMoney = atoi ( strNum ) ;
			int nNumLen = strlen ( strNum ) ;
			CString strText = strNum ;
			if ( nNumLen > 3 && nNumLen < 7 )
				strText.Insert ( nNumLen - 3, ',' ) ;
			else if ( nNumLen >= 7 )
			{
				strText.Insert ( nNumLen - 6, ',' ) ;
				strText.Insert ( nNumLen - 2, ',' ) ;
			}
			m_DepotEdtMoney.SetWindowText ( strText ) ;
			m_DepotEdtMoney.SetSel ( 12, 12 ) ;
		}
		else
		{
			if ( strlen ( strMoney ) == 4 )
			{
				m_nMoney = atoi ( strMoney ) ;
				CString strText = strMoney ;
				strText.Insert ( 1, ',' ) ;
				m_DepotEdtMoney.SetWindowText ( strText ) ;
				m_DepotEdtMoney.SetSel ( 5, 5 ) ;
			}
			else
			{
				m_nMoney = atoi ( strMoney ) ;
				m_nChange++ ;
			}
		}
	}
	else
	{
		m_nChange = 0 ;
		m_nMoney = atoi ( strMoney ) ;
	}
}

void CDlgDepot::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
}
