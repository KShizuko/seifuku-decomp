#include "stdafx.h"
#include "MyShell.h"
#include "DlgMpcBooth.h"

#include "Item.h"
#include "Hero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgMpcBooth::CDlgMpcBooth(CWnd* pParent )
	: CDialog(CDlgMpcBooth::IDD, pParent)
{
	m_bShow = false ;
	m_fFrameStep = 0.0f ;
	m_nFrameMode = 0 ;
	m_nPntX = 0 ;
	m_bPickUp = false ;
	m_nIconRealID = 0 ;
	m_nCurBase = 0 ;
	m_nCurRealID = 0 ;
	m_nCurChangeIndex = 0 ;
	m_Hbr = CreateSolidBrush ( 0x2132B ) ;
}


void CDlgMpcBooth::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MPCBOOTH_STA_MONEYTYPE6, m_StaMoneyType6);
	DDX_Control(pDX, IDC_MPCBOOTH_STA_MONEYTYPE5, m_StaMoneyType5);
	DDX_Control(pDX, IDC_MPCBOOTH_STA_MONEYTYPE4, m_StaMoneyType4);
	DDX_Control(pDX, IDC_MPCBOOTH_STA_MONEYTYPE3, m_StaMoneyType3);
	DDX_Control(pDX, IDC_MPCBOOTH_STA_MONEYTYPE2, m_StaMoneyType2);
	DDX_Control(pDX, IDC_MPCBOOTH_STA_MONEYTYPE1, m_StaMoneyType1);
	DDX_Control(pDX, IDC_STA_PRICE1, m_StaPrice1);
	DDX_Control(pDX, IDC_STA_PRICE2, m_StaPrice2);
	DDX_Control(pDX, IDC_STA_PRICE3, m_StaPrice3);
	DDX_Control(pDX, IDC_STA_PRICE4, m_StaPrice4);
	DDX_Control(pDX, IDC_STA_PRICE5, m_StaPrice5);
	DDX_Control(pDX, IDC_STA_PRICE6, m_StaPrice6);
	DDX_Control(pDX, IDC_MPCBOOTH_IMA_GOOD1, m_MpcBoothImaGood1);
	DDX_Control(pDX, IDC_MPCBOOTH_IMA_GOOD2, m_MpcBoothImaGood2);
	DDX_Control(pDX, IDC_MPCBOOTH_IMA_GOOD3, m_MpcBoothImaGood3);
	DDX_Control(pDX, IDC_MPCBOOTH_IMA_GOOD4, m_MpcBoothImaGood4);
	DDX_Control(pDX, IDC_MPCBOOTH_IMA_GOOD5, m_MpcBoothImaGood5);
	DDX_Control(pDX, IDC_MPCBOOTH_IMA_GOOD6, m_MpcBoothImaGood6);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_CHANGE1, m_BtnChange1);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_CHANGE2, m_BtnChange2);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_CHANGE3, m_BtnChange3);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_CHANGE4, m_BtnChange4);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_CHANGE5, m_BtnChange5);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_CHANGE6, m_BtnChange6);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_GETBACK1, m_BtnGetBack1);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_GETBACK2, m_BtnGetBack2);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_GETBACK3, m_BtnGetBack3);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_GETBACK4, m_BtnGetBack4);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_GETBACK5, m_BtnGetBack5);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_GETBACK6, m_BtnGetBack6);
	DDX_Control(pDX, IDC_MPCBOOTH_CLOSEB, m_MpcBoothBtnCloseB);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_UP, m_MpcBoothBtnUp);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_HAWK, m_MpcBoothBtnHawk);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_DOWN, m_MpcBoothBtnDown);
	DDX_Control(pDX, IDC_MPCBOOTH_BTN_CLOSES, m_MpcBoothBtnCloseS);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
}

BEGIN_MESSAGE_MAP(CDlgMpcBooth, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_CLOSES, OnMpcboothBtnCloses)
	ON_BN_CLICKED(IDC_MPCBOOTH_CLOSEB, OnMpcboothBtnCloseb)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_UP, OnMpcboothBtnUp)
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_DOWN, OnMpcboothBtnDown)
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_HAWK, OnMpcboothBtnHawk)
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_CHANGE1, OnMpcboothBtnChange1)
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_GETBACK1, OnMpcboothBtnGetback1)
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_CHANGE2, OnMpcboothBtnChange2)
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_GETBACK2, OnMpcboothBtnGetback2)
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_CHANGE3, OnMpcboothBtnChange3)
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_GETBACK3, OnMpcboothBtnGetback3)
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_CHANGE4, OnMpcboothBtnChange4)
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_GETBACK4, OnMpcboothBtnGetback4)
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_CHANGE5, OnMpcboothBtnChange5)
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_GETBACK5, OnMpcboothBtnGetback5)
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_CHANGE6, OnMpcboothBtnChange6)
	ON_BN_CLICKED(IDC_MPCBOOTH_BTN_GETBACK6, OnMpcboothBtnGetback6)
	ON_BN_CLICKED(IDC_MPCBOOTH_IMA_GOOD1, OnMpcboothImaGood1)
	ON_BN_CLICKED(IDC_MPCBOOTH_IMA_GOOD2, OnMpcboothImaGood2)
	ON_BN_CLICKED(IDC_MPCBOOTH_IMA_GOOD3, OnMpcboothImaGood3)
	ON_BN_CLICKED(IDC_MPCBOOTH_IMA_GOOD4, OnMpcboothImaGood4)
	ON_BN_CLICKED(IDC_MPCBOOTH_IMA_GOOD5, OnMpcboothImaGood5)
	ON_BN_CLICKED(IDC_MPCBOOTH_IMA_GOOD6, OnMpcboothImaGood6)
END_MESSAGE_MAP()

LRESULT CDlgMpcBooth::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_SHOWWINDOW:
		m_bShow = wParam ;
		if ( wParam )
		{
			CRect rect ;
			CDlgMpcBooth::GetWindowRect ( rect ) ;
			if ( m_nFrameMode == 0 )
			{
				DXPlaySound ( "Sound/DlgOpen.wav" ) ;
				m_dwLastShowTime = 0 ;
				m_nFrameMode = 1 ;
				CDlgMpcBooth::MoveWindow ( -rect.Width(), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
			}
			else if ( m_nFrameMode == 2 )
			{
				CDlgMpcBooth::MoveWindow ( 0, m_Pnt.y, rect.Width(), rect.Height(), true ) ;
				m_nFrameMode = 0 ;
			}
		}
		else
		{
			DXPlaySound ( "Sound/DlgClose.wav" ) ;
		}
		break ;
	case WM_COMMAND:
		if ( wParam == 1 )
		{
			return true ;
		}
		else if ( wParam == 2 )
		{
			return true ;
		}
		break ;
	case WM_DESTROY:
		DeleteObject ( m_Hbr ) ;
		break ;
	case WM_ERASEBKGND:
		return true ;
	case WM_NCPAINT:
		return false ;
	case WM_PAINT:
		{
			PAINTSTRUCT ps ;
			::BeginPaint ( m_hWnd, &ps ) ;
			::EndPaint ( m_hWnd, &ps ) ;
		}
		return false ;

	}

	return CDialog::WindowProc(message, wParam, lParam);
}

HBRUSH CDlgMpcBooth::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgMpcBooth::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgMpcBooth::GetWindowRect ( rect ) ;
	m_fFrameStep = ( float ) rect.Width () / 20 ;

	m_MpcBoothBtnCloseB.Init ( rect.left, rect.top, "Button333" ) ;
	m_MpcBoothBtnUp.Init ( rect.left, rect.top, "Button203" ) ;
	m_MpcBoothBtnHawk.Init ( rect.left, rect.top, "Button343" ) ;
	m_MpcBoothBtnDown.Init ( rect.left, rect.top, "Button204" ) ;
	m_MpcBoothBtnCloseS.Init ( rect.left, rect.top, "Button5" ) ;
	m_BtnHelp.Init ( rect.left, rect.top, "Button444" ) ;

	m_BtnChange1.Init ( rect.left, rect.top, "Button340" ) ;
	m_BtnChange2.Init ( rect.left, rect.top, "Button340" ) ;
	m_BtnChange3.Init ( rect.left, rect.top, "Button340" ) ;
	m_BtnChange4.Init ( rect.left, rect.top, "Button340" ) ;
	m_BtnChange5.Init ( rect.left, rect.top, "Button340" ) ;
	m_BtnChange6.Init ( rect.left, rect.top, "Button340" ) ;

	m_BtnGetBack1.Init ( rect.left, rect.top, "Button341" ) ;
	m_BtnGetBack2.Init ( rect.left, rect.top, "Button341" ) ;
	m_BtnGetBack3.Init ( rect.left, rect.top, "Button341" ) ;
	m_BtnGetBack4.Init ( rect.left, rect.top, "Button341" ) ;
	m_BtnGetBack5.Init ( rect.left, rect.top, "Button341" ) ;
	m_BtnGetBack6.Init ( rect.left, rect.top, "Button341" ) ;

	m_MpcBoothImaGood1.Init ( rect.left, rect.top,  NULL, ITEM7_IMAGE ) ;
	m_MpcBoothImaGood2.Init ( rect.left, rect.top,  NULL, ITEM7_IMAGE ) ;
	m_MpcBoothImaGood3.Init ( rect.left, rect.top,  NULL, ITEM7_IMAGE ) ;
	m_MpcBoothImaGood4.Init ( rect.left, rect.top,  NULL, ITEM7_IMAGE ) ;
	m_MpcBoothImaGood5.Init ( rect.left, rect.top,  NULL, ITEM7_IMAGE ) ;
	m_MpcBoothImaGood6.Init ( rect.left, rect.top,  NULL, ITEM7_IMAGE ) ;

	m_StaPrice1.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_StaPrice2.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_StaPrice3.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_StaPrice4.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_StaPrice5.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_StaPrice6.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;

	m_StaMoneyType1.Init ( rect.left, rect.top, 1, "Dialog56", "NULL" ) ;
	m_StaMoneyType2.Init ( rect.left, rect.top, 1, "Dialog56", "NULL" ) ;
	m_StaMoneyType3.Init ( rect.left, rect.top, 1, "Dialog56", "NULL" ) ;
	m_StaMoneyType4.Init ( rect.left, rect.top, 1, "Dialog56", "NULL" ) ;
	m_StaMoneyType5.Init ( rect.left, rect.top, 1, "Dialog56", "NULL" ) ;
	m_StaMoneyType6.Init ( rect.left, rect.top, 1, "Dialog56", "NULL" ) ;
	return TRUE;
}

void CDlgMpcBooth::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_nPntX = x ;
	m_Pnt.y = y ;
}

void CDlgMpcBooth::Show()
{
	if ( m_bShow )
	{
		switch ( m_nFrameMode )
		{
		case 0:
			break ;
		case 1:
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
					CDlgMpcBooth::WindowProc ( WM_SHOWWINDOW, true, NULL ) ;
				}
			}
			break ;
		case 2:
			break ;
		}

		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														  "Dialog34",
														  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			ShowAni->Show( 0,
				m_Pnt.x ,
				m_Pnt.y  );
		}
		else
			return ;

		m_MpcBoothBtnCloseB.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MpcBoothBtnUp.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MpcBoothBtnHawk.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MpcBoothBtnDown.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MpcBoothBtnCloseS.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;

		if ( m_MpcBoothImaGood1.GetImageLoadState () )
			m_BtnGetBack1.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_MpcBoothImaGood2.GetImageLoadState () )
			m_BtnGetBack2.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_MpcBoothImaGood3.GetImageLoadState () )
			m_BtnGetBack3.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_MpcBoothImaGood4.GetImageLoadState () )
			m_BtnGetBack4.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_MpcBoothImaGood5.GetImageLoadState () )
			m_BtnGetBack5.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_MpcBoothImaGood6.GetImageLoadState () )
			m_BtnGetBack6.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_MpcBoothImaGood1.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MpcBoothImaGood2.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MpcBoothImaGood3.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MpcBoothImaGood4.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MpcBoothImaGood5.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MpcBoothImaGood6.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_StaPrice1.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaPrice2.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaPrice3.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaPrice4.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaPrice5.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaPrice6.Show ( m_Pnt.x, m_Pnt.y ) ;

		if ( g_objHero.m_objBoothManager.GetItemByIndex ( m_nCurBase ) )
			m_StaMoneyType1.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( g_objHero.m_objBoothManager.GetItemByIndex ( m_nCurBase + 1 ) )
			m_StaMoneyType2.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( g_objHero.m_objBoothManager.GetItemByIndex ( m_nCurBase + 2 ) )
			m_StaMoneyType3.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( g_objHero.m_objBoothManager.GetItemByIndex ( m_nCurBase + 3 ) )
			m_StaMoneyType4.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( g_objHero.m_objBoothManager.GetItemByIndex ( m_nCurBase + 4 ) )
			m_StaMoneyType5.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( g_objHero.m_objBoothManager.GetItemByIndex ( m_nCurBase + 5 ) )
			m_StaMoneyType6.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgMpcBooth::OnMpcboothBtnCloses()
{
	CDlgMpcBooth::ShowWindow ( SW_HIDE ) ;
	CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MPCBOOTH_CLOSE, 1 ) ;
}

void CDlgMpcBooth::OnMpcboothBtnCloseb()
{
	CDlgMpcBooth::OnMpcboothBtnCloses () ;
}

void CDlgMpcBooth::OnBtnHelp()
{
	CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_MPCBOOTH ) ;
}


void CDlgMpcBooth::OnMpcboothBtnUp()
{
	if ( m_nCurBase > 0 )
	{
		CDlgMpcBooth::ClearMpcBooteh () ;
		m_nCurBase -= 6 ;
		for ( int i = m_nCurBase; i < m_nCurBase + 6; i++ )
		{
			CItem* pItem = g_objHero.m_objBoothManager.GetItemByIndex ( i ) ;
			if ( pItem )
			{
				char strName[32] ;
				char strPrice[32] ;
				BYTE btActType ;
				bool bEmoney ;
				sprintf ( strName, "Item%u", pItem->GetTypeID () ) ;
				bEmoney = pItem->GetBoothSellEmoneyPrice () != 0 ;
				if ( bEmoney )
					itoa ( pItem->GetBoothSellEmoneyPrice (), strPrice, 10 ) ;
				else
					itoa ( pItem->GetBoothSellPrice (), strPrice, 10 ) ;
				strcpy ( strPrice, CMyShellApp::StringChange ( strPrice, 2 ) ) ;

				btActType = 0 ;
				if ( pItem->GetTypeID () / 100000 < 10 && ( pItem->GetTypeID () < 700000 || pItem->GetTypeID () > 800000 ) )
				{
					switch ( pItem->GetTypeID () % 10 )
					{
					case 9: btActType = 1 ; break ;
					case 8: btActType = 2 ; break ;
					case 7: btActType = 3 ; break ;
					case 6: btActType = 4 ; break ;
					}
				}

				switch ( i - m_nCurBase )
				{
				case 0:
					m_MpcBoothImaGood1.InsertImage ( strName ) ;
					m_MpcBoothImaGood1.SetActType ( btActType ) ;
					m_StaPrice1.SetWindowText ( strPrice ) ;
					m_StaMoneyType1.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 1:
					m_MpcBoothImaGood2.InsertImage ( strName ) ;
					m_MpcBoothImaGood2.SetActType ( btActType ) ;
					m_StaPrice2.SetWindowText ( strPrice ) ;
					m_StaMoneyType2.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 2:
					m_MpcBoothImaGood3.InsertImage ( strName ) ;
					m_MpcBoothImaGood3.SetActType ( btActType ) ;
					m_StaPrice3.SetWindowText ( strPrice ) ;
					m_StaMoneyType3.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 3:
					m_MpcBoothImaGood4.InsertImage ( strName ) ;
					m_MpcBoothImaGood4.SetActType ( btActType ) ;
					m_StaPrice4.SetWindowText ( strPrice ) ;
					m_StaMoneyType4.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 4:
					m_MpcBoothImaGood5.InsertImage ( strName ) ;
					m_MpcBoothImaGood5.SetActType ( btActType ) ;
					m_StaPrice5.SetWindowText ( strPrice ) ;
					m_StaMoneyType5.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 5:
					m_MpcBoothImaGood6.InsertImage ( strName ) ;
					m_MpcBoothImaGood6.SetActType ( btActType ) ;
					m_StaPrice6.SetWindowText ( strPrice ) ;
					m_StaMoneyType6.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				}
			}
		}
	}
}

void CDlgMpcBooth::OnMpcboothBtnDown()
{
	int nAmount = g_objHero.m_objBoothManager.GetItemAmount () ;
	if ( nAmount >= m_nCurBase + 6 )
	{
		CDlgMpcBooth::ClearMpcBooteh () ;
		m_nCurBase += 6 ;
		nAmount = ( nAmount > m_nCurBase + 6 ) ? m_nCurBase + 6 : nAmount ;
		for ( int i = m_nCurBase; i < m_nCurBase + 6; i++ )
		{
			if ( i >= nAmount )
			{
				CDlgMpcBooth::SetMoneyTypeID ( i - m_nCurBase, "Dialog56" ) ;
				continue ;
			}
			CItem* pItem = g_objHero.m_objBoothManager.GetItemByIndex ( i ) ;
			if ( pItem )
			{
				char strName[32] ;
				char strPrice[32] ;
				BYTE btActType ;
				bool bEmoney ;
				sprintf ( strName, "Item%u", pItem->GetTypeID () ) ;
				bEmoney = pItem->GetBoothSellEmoneyPrice () != 0 ;
				if ( bEmoney )
					itoa ( pItem->GetBoothSellEmoneyPrice (), strPrice, 10 ) ;
				else
					itoa ( pItem->GetBoothSellPrice (), strPrice, 10 ) ;
				strcpy ( strPrice, CMyShellApp::StringChange ( strPrice, 2 ) ) ;

				btActType = 0 ;
				if ( pItem->GetTypeID () / 100000 < 10 && ( pItem->GetTypeID () < 700000 || pItem->GetTypeID () > 800000 ) )
				{
					switch ( pItem->GetTypeID () % 10 )
					{
					case 9: btActType = 1 ; break ;
					case 8: btActType = 2 ; break ;
					case 7: btActType = 3 ; break ;
					case 6: btActType = 4 ; break ;
					}
				}

				switch ( i - m_nCurBase )
				{
				case 0:
					m_MpcBoothImaGood1.InsertImage ( strName ) ;
					m_MpcBoothImaGood1.SetActType ( btActType ) ;
					m_StaPrice1.SetWindowText ( strPrice ) ;
					m_StaMoneyType1.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 1:
					m_MpcBoothImaGood2.InsertImage ( strName ) ;
					m_MpcBoothImaGood2.SetActType ( btActType ) ;
					m_StaPrice2.SetWindowText ( strPrice ) ;
					m_StaMoneyType2.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 2:
					m_MpcBoothImaGood3.InsertImage ( strName ) ;
					m_MpcBoothImaGood3.SetActType ( btActType ) ;
					m_StaPrice3.SetWindowText ( strPrice ) ;
					m_StaMoneyType3.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 3:
					m_MpcBoothImaGood4.InsertImage ( strName ) ;
					m_MpcBoothImaGood4.SetActType ( btActType ) ;
					m_StaPrice4.SetWindowText ( strPrice ) ;
					m_StaMoneyType4.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 4:
					m_MpcBoothImaGood5.InsertImage ( strName ) ;
					m_MpcBoothImaGood5.SetActType ( btActType ) ;
					m_StaPrice5.SetWindowText ( strPrice ) ;
					m_StaMoneyType5.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 5:
					m_MpcBoothImaGood6.InsertImage ( strName ) ;
					m_MpcBoothImaGood6.SetActType ( btActType ) ;
					m_StaPrice6.SetWindowText ( strPrice ) ;
					m_StaMoneyType6.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				}
			}
		}
	}
}


void CDlgMpcBooth::OnMpcboothBtnHawk()
{
	CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MPCBOOTH_HAWK ) ;
}

void CDlgMpcBooth::OnMpcboothBtnChange1()
{
}

void CDlgMpcBooth::OnMpcboothBtnChange2()
{
}

void CDlgMpcBooth::OnMpcboothBtnChange3()
{
}

void CDlgMpcBooth::OnMpcboothBtnChange4()
{
}

void CDlgMpcBooth::OnMpcboothBtnChange5()
{
}

void CDlgMpcBooth::OnMpcboothBtnChange6()
{
}

void CDlgMpcBooth::OnMpcboothBtnGetback1()
{
	if ( m_MpcBoothImaGood1.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.m_objBoothManager.GetItemByIndex ( m_nCurBase ) ;
		if ( pItem )
			g_objHero.m_objBoothManager.DelItem ( pItem->GetID () ) ;
	}
}


void CDlgMpcBooth::OnMpcboothBtnGetback2()
{
	if ( m_MpcBoothImaGood1.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.m_objBoothManager.GetItemByIndex ( m_nCurBase + 1 ) ;
		if ( pItem )
			g_objHero.m_objBoothManager.DelItem ( pItem->GetID () ) ;
	}
}


void CDlgMpcBooth::OnMpcboothBtnGetback3()
{
	if ( m_MpcBoothImaGood1.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.m_objBoothManager.GetItemByIndex ( m_nCurBase + 2 ) ;
		if ( pItem )
			g_objHero.m_objBoothManager.DelItem ( pItem->GetID () ) ;
	}
}


void CDlgMpcBooth::OnMpcboothBtnGetback4()
{
	if ( m_MpcBoothImaGood1.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.m_objBoothManager.GetItemByIndex ( m_nCurBase + 3 ) ;
		if ( pItem )
			g_objHero.m_objBoothManager.DelItem ( pItem->GetID () ) ;
	}
}


void CDlgMpcBooth::OnMpcboothBtnGetback5()
{
	if ( m_MpcBoothImaGood1.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.m_objBoothManager.GetItemByIndex ( m_nCurBase + 4 ) ;
		if ( pItem )
			g_objHero.m_objBoothManager.DelItem ( pItem->GetID () ) ;
	}
}



void CDlgMpcBooth::OnMpcboothBtnGetback6()
{
	if ( m_MpcBoothImaGood1.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.m_objBoothManager.GetItemByIndex ( m_nCurBase + 5 ) ;
		if ( pItem )
			g_objHero.m_objBoothManager.DelItem ( pItem->GetID () ) ;
	}
}

void CDlgMpcBooth::OnMpcboothImaGood1()
{
	if ( m_bPickUp && !m_MpcBoothImaGood1.GetImageLoadState () )
	{
		m_nCurRealID = m_nIconRealID ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MPCBOOTH_ADDITEM ) ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSG_1003 ) ;
	}
}

void CDlgMpcBooth::OnMpcboothImaGood2()
{
	if ( m_bPickUp && !m_MpcBoothImaGood2.GetImageLoadState () )
	{
		m_nCurRealID = m_nIconRealID ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MPCBOOTH_ADDITEM ) ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSG_1003 ) ;
	}
}

void CDlgMpcBooth::OnMpcboothImaGood3()
{
	if ( m_bPickUp && !m_MpcBoothImaGood3.GetImageLoadState () )
	{
		m_nCurRealID = m_nIconRealID ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MPCBOOTH_ADDITEM ) ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSG_1003 ) ;
	}
}

void CDlgMpcBooth::OnMpcboothImaGood4()
{
	if ( m_bPickUp && !m_MpcBoothImaGood4.GetImageLoadState () )
	{
		m_nCurRealID = m_nIconRealID ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MPCBOOTH_ADDITEM ) ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSG_1003 ) ;
	}
}

void CDlgMpcBooth::OnMpcboothImaGood5()
{
	if ( m_bPickUp && !m_MpcBoothImaGood5.GetImageLoadState () )
	{
		m_nCurRealID = m_nIconRealID ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MPCBOOTH_ADDITEM ) ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSG_1003 ) ;
	}
}

void CDlgMpcBooth::OnMpcboothImaGood6()
{
	if ( m_bPickUp && !m_MpcBoothImaGood6.GetImageLoadState () )
	{
		m_nCurRealID = m_nIconRealID ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MPCBOOTH_ADDITEM ) ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
		CDlgMpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSG_1003 ) ;
	}
}

void CDlgMpcBooth::SetPickUpState(BOOL bPickUp,OBJID nIconRealID)
{
	m_bPickUp = bPickUp;
	m_nIconRealID = nIconRealID;
}

void CDlgMpcBooth::SetMpcBooth()
{
	CDlgMpcBooth::ClearMpcBooteh () ;
	int nAmount = g_objHero.m_objBoothManager.GetItemAmount () ;
	nAmount = ( nAmount > m_nCurBase + 6 ) ? m_nCurBase + 6 : nAmount ;
	for ( int i = m_nCurBase; i < m_nCurBase + 6; i++ )
	{
		if ( i >= nAmount )
		{
			CDlgMpcBooth::SetMoneyTypeID ( i - m_nCurBase, "Dialog56" ) ;
			continue ;
		}
		CItem* pItem = g_objHero.m_objBoothManager.GetItemByIndex ( i ) ;
		if ( pItem )
		{
			char strName[32] ;
			char strPrice[32] ;
			BYTE btActType ;
			bool bEmoney ;
			sprintf ( strName, "Item%u", pItem->GetTypeID () ) ;
			bEmoney = pItem->GetBoothSellEmoneyPrice () != 0 ;
			if ( bEmoney )
				itoa ( pItem->GetBoothSellEmoneyPrice (), strPrice, 10 ) ;
			else
				itoa ( pItem->GetBoothSellPrice (), strPrice, 10 ) ;
			strcpy ( strPrice, CMyShellApp::StringChange ( strPrice, 2 ) ) ;

			btActType = 0 ;
			if ( pItem->GetTypeID () / 100000 < 10 && ( pItem->GetTypeID () < 700000 || pItem->GetTypeID () > 800000 ) )
			{
				switch ( pItem->GetTypeID () % 10 )
				{
				case 9: btActType = 1 ; break ;
				case 8: btActType = 2 ; break ;
				case 7: btActType = 3 ; break ;
				case 6: btActType = 4 ; break ;
				}
			}

			switch ( i - m_nCurBase )
			{
			case 0:
				m_MpcBoothImaGood1.InsertImage ( strName ) ;
				m_MpcBoothImaGood1.SetActType ( btActType ) ;
				m_StaPrice1.SetWindowText ( strPrice ) ;
				m_StaMoneyType1.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
				break ;
			case 1:
				m_MpcBoothImaGood2.InsertImage ( strName ) ;
				m_MpcBoothImaGood2.SetActType ( btActType ) ;
				m_StaPrice2.SetWindowText ( strPrice ) ;
				m_StaMoneyType2.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
				break ;
			case 2:
				m_MpcBoothImaGood3.InsertImage ( strName ) ;
				m_MpcBoothImaGood3.SetActType ( btActType ) ;
				m_StaPrice3.SetWindowText ( strPrice ) ;
				m_StaMoneyType3.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
				break ;
			case 3:
				m_MpcBoothImaGood4.InsertImage ( strName ) ;
				m_MpcBoothImaGood4.SetActType ( btActType ) ;
				m_StaPrice4.SetWindowText ( strPrice ) ;
				m_StaMoneyType4.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
				break ;
			case 4:
				m_MpcBoothImaGood5.InsertImage ( strName ) ;
				m_MpcBoothImaGood5.SetActType ( btActType ) ;
				m_StaPrice5.SetWindowText ( strPrice ) ;
				m_StaMoneyType5.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
				break ;
			case 5:
				m_MpcBoothImaGood6.InsertImage ( strName ) ;
				m_MpcBoothImaGood6.SetActType ( btActType ) ;
				m_StaPrice6.SetWindowText ( strPrice ) ;
				m_StaMoneyType6.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
				break ;
			}
		}
	}
}

void CDlgMpcBooth::ClearMpcBooteh()
{
	m_MpcBoothImaGood1.RemoveImage () ;
	m_MpcBoothImaGood2.RemoveImage () ;
	m_MpcBoothImaGood3.RemoveImage () ;
	m_MpcBoothImaGood4.RemoveImage () ;
	m_MpcBoothImaGood5.RemoveImage () ;
	m_MpcBoothImaGood6.RemoveImage () ;
	m_StaPrice1.SetWindowText ( NULL ) ;
	m_StaPrice2.SetWindowText ( NULL ) ;
	m_StaPrice3.SetWindowText ( NULL ) ;
	m_StaPrice4.SetWindowText ( NULL ) ;
	m_StaPrice5.SetWindowText ( NULL ) ;
	m_StaPrice6.SetWindowText ( NULL ) ;
}

void CDlgMpcBooth::SetMoneyTypeID(int nIndex, const char* pszID)
{
	switch ( nIndex )
	{
	case 0:
		m_StaMoneyType1.SetPictureID ( pszID ) ;
		break ;
	case 1:
		m_StaMoneyType2.SetPictureID ( pszID ) ;
		break ;
	case 2:
		m_StaMoneyType3.SetPictureID ( pszID ) ;
		break ;
	case 3:
		m_StaMoneyType4.SetPictureID ( pszID ) ;
		break ;
	case 4:
		m_StaMoneyType5.SetPictureID ( pszID ) ;
		break ;
	case 5:
		m_StaMoneyType6.SetPictureID ( pszID ) ;
		break ;
	}
}

int CDlgMpcBooth::GetMouseMoveIndex()
{
	if ( m_MpcBoothImaGood1.m_bMouseMove )
		return m_nCurBase ;
	if ( m_MpcBoothImaGood2.m_bMouseMove )
		return m_nCurBase + 1;
	if ( m_MpcBoothImaGood3.m_bMouseMove )
		return m_nCurBase + 2;
	if ( m_MpcBoothImaGood4.m_bMouseMove )
		return m_nCurBase + 3;
	if ( m_MpcBoothImaGood5.m_bMouseMove )
		return m_nCurBase + 4;
	if ( m_MpcBoothImaGood6.m_bMouseMove )
		return m_nCurBase + 5;

	return -1 ;
}
