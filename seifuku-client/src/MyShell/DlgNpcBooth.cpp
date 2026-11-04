#include "stdafx.h"
#include "MyShell.h"
#include "DlgNpcBooth.h"

#include "Hero.h"
#include "Item.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgNpcBooth::CDlgNpcBooth(CWnd* pParent )
	: CDialog(CDlgNpcBooth::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = CreateSolidBrush ( 0x2132B ) ;
	m_fFrameStep = 0.0f ;
	m_nFrameMode = 0 ;
	m_nPntX = 0 ;
	m_nCurBase = 0 ;
}


void CDlgNpcBooth::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NPCBOOTH_STA_MONEYTYPE6, m_StaMoneyType6);
	DDX_Control(pDX, IDC_NPCBOOTH_STA_MONEYTYPE5, m_StaMoneyType5);
	DDX_Control(pDX, IDC_NPCBOOTH_STA_MONEYTYPE4, m_StaMoneyType4);
	DDX_Control(pDX, IDC_NPCBOOTH_STA_MONEYTYPE3, m_StaMoneyType3);
	DDX_Control(pDX, IDC_NPCBOOTH_STA_MONEYTYPE2, m_StaMoneyType2);
	DDX_Control(pDX, IDC_NPCBOOTH_STA_MONEYTYPE1, m_StaMoneyType1);
	DDX_Control(pDX, IDC_STA_PRICE11, m_StaPrice1);
	DDX_Control(pDX, IDC_STA_PRICE22, m_StaPrice2);
	DDX_Control(pDX, IDC_STA_PRICE33, m_StaPrice3);
	DDX_Control(pDX, IDC_STA_PRICE44, m_StaPrice4);
	DDX_Control(pDX, IDC_STA_PRICE55, m_StaPrice5);
	DDX_Control(pDX, IDC_STA_PRICE66, m_StaPrice6);
	DDX_Control(pDX, IDC_NPCBOOTH_IMA_GOOD1, m_NpcBoothImaGood1);
	DDX_Control(pDX, IDC_NPCBOOTH_IMA_GOOD2, m_NpcBoothImaGood2);
	DDX_Control(pDX, IDC_NPCBOOTH_IMA_GOOD3, m_NpcBoothImaGood3);
	DDX_Control(pDX, IDC_NPCBOOTH_IMA_GOOD4, m_NpcBoothImaGood4);
	DDX_Control(pDX, IDC_NPCBOOTH_IMA_GOOD5, m_NpcBoothImaGood5);
	DDX_Control(pDX, IDC_NPCBOOTH_IMA_GOOD6, m_NpcBoothImaGood6);
	DDX_Control(pDX, IDC_NPCBOOTH_BTN_CLOSEB, m_NpcBoothBtnCloseB);
	DDX_Control(pDX, IDC_NPCBOOTH_BTN_CLOSE, m_NpcBoothBtnClose);
	DDX_Control(pDX, IDC_NPCBOOTH_BTN_UP, m_NpcBoothBtnUp);
	DDX_Control(pDX, IDC_NPCBOOTH_BTN_DOWN, m_NpcBoothBtnDown);
	DDX_Control(pDX, IDC_NPCBOOTH_BTN_BUY1, m_BtnBuy1);
	DDX_Control(pDX, IDC_NPCBOOTH_BTN_BUY2, m_BtnBuy2);
	DDX_Control(pDX, IDC_NPCBOOTH_BTN_BUY3, m_BtnBuy3);
	DDX_Control(pDX, IDC_NPCBOOTH_BTN_BUY4, m_BtnBuy4);
	DDX_Control(pDX, IDC_NPCBOOTH_BTN_BUY5, m_BtnBuy5);
	DDX_Control(pDX, IDC_NPCBOOTH_BTN_BUY6, m_BtnBuy6);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
}


BEGIN_MESSAGE_MAP(CDlgNpcBooth, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_NPCBOOTH_BTN_CLOSE, OnNpcboothBtnClose)
	ON_BN_CLICKED(IDC_NPCBOOTH_BTN_CLOSEB, OnNpcboothBtnCloseb)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_NPCBOOTH_BTN_UP, OnNpcboothBtnUp)
	ON_BN_CLICKED(IDC_NPCBOOTH_BTN_DOWN, OnNpcboothBtnDown)
	ON_BN_CLICKED(IDC_NPCBOOTH_BTN_BUY1, OnNpcboothBtnBuy1)
	ON_BN_CLICKED(IDC_NPCBOOTH_BTN_BUY2, OnNpcboothBtnBuy2)
	ON_BN_CLICKED(IDC_NPCBOOTH_BTN_BUY3, OnNpcboothBtnBuy3)
	ON_BN_CLICKED(IDC_NPCBOOTH_BTN_BUY4, OnNpcboothBtnBuy4)
	ON_BN_CLICKED(IDC_NPCBOOTH_BTN_BUY5, OnNpcboothBtnBuy5)
	ON_BN_CLICKED(IDC_NPCBOOTH_BTN_BUY6, OnNpcboothBtnBuy6)
END_MESSAGE_MAP()

LRESULT CDlgNpcBooth::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_SHOWWINDOW:
		m_bShow = wParam ;
		if ( wParam )
		{
			CRect rect ;
			CDlgNpcBooth::GetWindowRect ( rect ) ;
			if ( m_nFrameMode == 0 )
			{
				DXPlaySound ( "Sound/DlgOpen.wav" ) ;
				m_dwLastShowTime = 0 ;
				m_nFrameMode = 1 ;
				CDlgNpcBooth::MoveWindow ( -rect.Width(), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
			}
			else if ( m_nFrameMode == 2 )
			{
				CDlgNpcBooth::MoveWindow ( 0, m_Pnt.y, rect.Width(), rect.Height(), true ) ;
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

HBRUSH CDlgNpcBooth::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgNpcBooth::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgNpcBooth::GetWindowRect ( rect ) ;
	m_fFrameStep = ( float ) rect.Width () / 20 ;

	m_BtnBuy1.Init ( rect.left, rect.top, "Button350" ) ;
	m_BtnBuy2.Init ( rect.left, rect.top, "Button350" ) ;
	m_BtnBuy3.Init ( rect.left, rect.top, "Button350" ) ;
	m_BtnBuy4.Init ( rect.left, rect.top, "Button350" ) ;
	m_BtnBuy5.Init ( rect.left, rect.top, "Button350" ) ;
	m_BtnBuy6.Init ( rect.left, rect.top, "Button350" ) ;
	m_NpcBoothBtnCloseB.Init ( rect.left, rect.top, "Button333" ) ;
	m_NpcBoothBtnUp.Init ( rect.left, rect.top, "Button203" ) ;
	m_NpcBoothBtnDown.Init ( rect.left, rect.top, "Button204" ) ;
	m_NpcBoothBtnClose.Init ( rect.left, rect.top, "Button5" ) ;
	m_BtnHelp.Init ( rect.left, rect.top, "Button444" ) ;

	m_NpcBoothImaGood1.Init ( rect.left, rect.top , NULL, 8 ) ;
	m_NpcBoothImaGood2.Init ( rect.left, rect.top , NULL, 8 ) ;
	m_NpcBoothImaGood3.Init ( rect.left, rect.top , NULL, 8 ) ;
	m_NpcBoothImaGood4.Init ( rect.left, rect.top , NULL, 8 ) ;
	m_NpcBoothImaGood5.Init ( rect.left, rect.top , NULL, 8 ) ;
	m_NpcBoothImaGood6.Init ( rect.left, rect.top , NULL, 8 ) ;

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

void CDlgNpcBooth::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_nPntX = x ;
	m_Pnt.y = y ;

}

void CDlgNpcBooth::Show()
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
					CDlgNpcBooth::WindowProc(WM_SHOWWINDOW,true,NULL);
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

		m_BtnBuy1.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnBuy2.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnBuy3.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnBuy4.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnBuy5.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnBuy6.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_NpcBoothBtnCloseB.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_NpcBoothBtnUp.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_NpcBoothBtnDown.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_NpcBoothBtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_NpcBoothImaGood1.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_NpcBoothImaGood2.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_NpcBoothImaGood3.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_NpcBoothImaGood4.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_NpcBoothImaGood5.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_NpcBoothImaGood6.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_StaPrice1.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaPrice2.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaPrice3.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaPrice4.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaPrice5.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaPrice6.Show ( m_Pnt.x, m_Pnt.y ) ;

		if ( g_objHero.m_objBooth.GetItemByIndex ( m_nCurBase ) )
			m_StaMoneyType1.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( g_objHero.m_objBooth.GetItemByIndex ( m_nCurBase + 1 ) )
			m_StaMoneyType2.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( g_objHero.m_objBooth.GetItemByIndex ( m_nCurBase + 2 ) )
			m_StaMoneyType3.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( g_objHero.m_objBooth.GetItemByIndex ( m_nCurBase + 3 ) )
			m_StaMoneyType4.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( g_objHero.m_objBooth.GetItemByIndex ( m_nCurBase + 4 ) )
			m_StaMoneyType5.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( g_objHero.m_objBooth.GetItemByIndex ( m_nCurBase + 5 ) )
			m_StaMoneyType6.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgNpcBooth::SetNpcBooth()
{
	m_nCurBase = 0 ;
	CDlgNpcBooth::ClearNpcBooth () ;
	int nAmount = g_objHero.m_objBooth.GetItemAmount () ;
	nAmount = ( nAmount > m_nCurBase + 6 ) ? m_nCurBase + 6 : nAmount ;
	for ( int i = m_nCurBase; i < m_nCurBase + 6; i++ )
	{
		if ( i >= nAmount )
		{
			CDlgNpcBooth::SetMoneyTypeID ( i - m_nCurBase, "Dialog56" ) ;
			continue ;
		}
		CItem* pItem = g_objHero.m_objBooth.GetItemByIndex ( i ) ;
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
				m_NpcBoothImaGood1.InsertImage ( strName ) ;
				m_NpcBoothImaGood1.SetActType ( btActType ) ;
				m_StaPrice1.SetWindowText ( strPrice ) ;
				m_StaMoneyType1.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
				break ;
			case 1:
				m_NpcBoothImaGood2.InsertImage ( strName ) ;
				m_NpcBoothImaGood2.SetActType ( btActType ) ;
				m_StaPrice2.SetWindowText ( strPrice ) ;
				m_StaMoneyType2.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
				break ;
			case 2:
				m_NpcBoothImaGood3.InsertImage ( strName ) ;
				m_NpcBoothImaGood3.SetActType ( btActType ) ;
				m_StaPrice3.SetWindowText ( strPrice ) ;
				m_StaMoneyType3.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
				break ;
			case 3:
				m_NpcBoothImaGood4.InsertImage ( strName ) ;
				m_NpcBoothImaGood4.SetActType ( btActType ) ;
				m_StaPrice4.SetWindowText ( strPrice ) ;
				m_StaMoneyType4.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
				break ;
			case 4:
				m_NpcBoothImaGood5.InsertImage ( strName ) ;
				m_NpcBoothImaGood5.SetActType ( btActType ) ;
				m_StaPrice5.SetWindowText ( strPrice ) ;
				m_StaMoneyType5.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
				break ;
			case 5:
				m_NpcBoothImaGood6.InsertImage ( strName ) ;
				m_NpcBoothImaGood6.SetActType ( btActType ) ;
				m_StaPrice6.SetWindowText ( strPrice ) ;
				m_StaMoneyType6.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
				break ;
			}
		}
	}
}

void CDlgNpcBooth::ClearNpcBooth()
{
	m_NpcBoothImaGood1.RemoveImage () ;
	m_NpcBoothImaGood2.RemoveImage () ;
	m_NpcBoothImaGood3.RemoveImage () ;
	m_NpcBoothImaGood4.RemoveImage () ;
	m_NpcBoothImaGood5.RemoveImage () ;
	m_NpcBoothImaGood6.RemoveImage () ;

	m_StaPrice1.SetWindowText ( NULL ) ;
	m_StaPrice2.SetWindowText ( NULL ) ;
	m_StaPrice3.SetWindowText ( NULL ) ;
	m_StaPrice4.SetWindowText ( NULL ) ;
	m_StaPrice5.SetWindowText ( NULL ) ;
	m_StaPrice6.SetWindowText ( NULL ) ;
}

void CDlgNpcBooth::SetMoneyTypeID(int nIndex, const char* pszID)
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

void CDlgNpcBooth::OnNpcboothBtnClose()
{
	CDlgNpcBooth::ShowWindow ( SW_HIDE ) ;
	CDlgNpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_NPCBOOTH_CLOSE ) ;
}

void CDlgNpcBooth::OnNpcboothBtnCloseb()
{
	CDlgNpcBooth::ShowWindow ( SW_HIDE ) ;
	CDlgNpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_NPCBOOTH_CLOSE ) ;
}

void CDlgNpcBooth::OnBtnHelp()
{
	CDlgNpcBooth::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_NPCBOOTH ) ;
}


void CDlgNpcBooth::OnNpcboothBtnUp()
{
	if ( m_nCurBase > 0 )
	{
		CDlgNpcBooth::ClearNpcBooth () ;
		m_nCurBase -= 6 ;
		for ( int i = m_nCurBase; i < m_nCurBase + 6; i++ )
		{
			CItem* pItem = g_objHero.m_objBooth.GetItemByIndex ( i ) ;
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
					m_NpcBoothImaGood1.InsertImage ( strName ) ;
					m_NpcBoothImaGood1.SetActType ( btActType ) ;
					m_StaPrice1.SetWindowText ( strPrice ) ;
					m_StaMoneyType1.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 1:
					m_NpcBoothImaGood2.InsertImage ( strName ) ;
					m_NpcBoothImaGood2.SetActType ( btActType ) ;
					m_StaPrice2.SetWindowText ( strPrice ) ;
					m_StaMoneyType2.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 2:
					m_NpcBoothImaGood3.InsertImage ( strName ) ;
					m_NpcBoothImaGood3.SetActType ( btActType ) ;
					m_StaPrice3.SetWindowText ( strPrice ) ;
					m_StaMoneyType3.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 3:
					m_NpcBoothImaGood4.InsertImage ( strName ) ;
					m_NpcBoothImaGood4.SetActType ( btActType ) ;
					m_StaPrice4.SetWindowText ( strPrice ) ;
					m_StaMoneyType4.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 4:
					m_NpcBoothImaGood5.InsertImage ( strName ) ;
					m_NpcBoothImaGood5.SetActType ( btActType ) ;
					m_StaPrice5.SetWindowText ( strPrice ) ;
					m_StaMoneyType5.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 5:
					m_NpcBoothImaGood6.InsertImage ( strName ) ;
					m_NpcBoothImaGood6.SetActType ( btActType ) ;
					m_StaPrice6.SetWindowText ( strPrice ) ;
					m_StaMoneyType6.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				}
			}
		}
	}
}

void CDlgNpcBooth::OnNpcboothBtnDown()
{
	int nAmount = g_objHero.m_objBooth.GetItemAmount () ;
	if ( nAmount > m_nCurBase + 6 )
	{
		CDlgNpcBooth::ClearNpcBooth () ;
		m_nCurBase += 6 ;
		nAmount = ( nAmount > m_nCurBase + 6 ) ? m_nCurBase + 6 : nAmount ;
		for ( int i = m_nCurBase; i < m_nCurBase + 6; i++ )
		{
			if ( i >= nAmount )
			{
				CDlgNpcBooth::SetMoneyTypeID ( i - m_nCurBase, "Dialog56" ) ;
				continue ;
			}
			CItem* pItem = g_objHero.m_objBooth.GetItemByIndex ( i ) ;
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
					m_NpcBoothImaGood1.InsertImage ( strName ) ;
					m_NpcBoothImaGood1.SetActType ( btActType ) ;
					m_StaPrice1.SetWindowText ( strPrice ) ;
					m_StaMoneyType1.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 1:
					m_NpcBoothImaGood2.InsertImage ( strName ) ;
					m_NpcBoothImaGood2.SetActType ( btActType ) ;
					m_StaPrice2.SetWindowText ( strPrice ) ;
					m_StaMoneyType2.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 2:
					m_NpcBoothImaGood3.InsertImage ( strName ) ;
					m_NpcBoothImaGood3.SetActType ( btActType ) ;
					m_StaPrice3.SetWindowText ( strPrice ) ;
					m_StaMoneyType3.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 3:
					m_NpcBoothImaGood4.InsertImage ( strName ) ;
					m_NpcBoothImaGood4.SetActType ( btActType ) ;
					m_StaPrice4.SetWindowText ( strPrice ) ;
					m_StaMoneyType4.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 4:
					m_NpcBoothImaGood5.InsertImage ( strName ) ;
					m_NpcBoothImaGood5.SetActType ( btActType ) ;
					m_StaPrice5.SetWindowText ( strPrice ) ;
					m_StaMoneyType5.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				case 5:
					m_NpcBoothImaGood6.InsertImage ( strName ) ;
					m_NpcBoothImaGood6.SetActType ( btActType ) ;
					m_StaPrice6.SetWindowText ( strPrice ) ;
					m_StaMoneyType6.SetPictureID ( bEmoney ? "Dialog57" : "Dialog56" ) ;
					break ;
				}
			}
		}
	}
}

void CDlgNpcBooth::OnNpcboothBtnBuy1()
{
	CItem* pItem = g_objHero.m_objBooth.GetItemByIndex ( m_nCurBase ) ;
	if ( pItem )
		g_objHero.m_objBooth.BuyItem ( pItem->GetID () ) ;
}

void CDlgNpcBooth::OnNpcboothBtnBuy2()
{
	CItem* pItem = g_objHero.m_objBooth.GetItemByIndex ( m_nCurBase + 1 ) ;
	if ( pItem )
		g_objHero.m_objBooth.BuyItem ( pItem->GetID () ) ;
}

void CDlgNpcBooth::OnNpcboothBtnBuy3()
{
	CItem* pItem = g_objHero.m_objBooth.GetItemByIndex ( m_nCurBase + 2 ) ;
	if ( pItem )
		g_objHero.m_objBooth.BuyItem ( pItem->GetID () ) ;
}

void CDlgNpcBooth::OnNpcboothBtnBuy4()
{
	CItem* pItem = g_objHero.m_objBooth.GetItemByIndex ( m_nCurBase + 3 ) ;
	if ( pItem )
		g_objHero.m_objBooth.BuyItem ( pItem->GetID () ) ;
}

void CDlgNpcBooth::OnNpcboothBtnBuy5()
{
	CItem* pItem = g_objHero.m_objBooth.GetItemByIndex ( m_nCurBase + 4 ) ;
	if ( pItem )
		g_objHero.m_objBooth.BuyItem ( pItem->GetID () ) ;
}

void CDlgNpcBooth::OnNpcboothBtnBuy6()
{
	CItem* pItem = g_objHero.m_objBooth.GetItemByIndex ( m_nCurBase + 5 ) ;
	if ( pItem )
		g_objHero.m_objBooth.BuyItem ( pItem->GetID () ) ;
}

int CDlgNpcBooth::GetMouseMoveIndex()
{
	if ( m_NpcBoothImaGood1.m_bMouseMove )
		return m_nCurBase ;
	if ( m_NpcBoothImaGood2.m_bMouseMove )
		return m_nCurBase + 1;
	if ( m_NpcBoothImaGood3.m_bMouseMove )
		return m_nCurBase + 2;
	if ( m_NpcBoothImaGood4.m_bMouseMove )
		return m_nCurBase + 3;
	if ( m_NpcBoothImaGood5.m_bMouseMove )
		return m_nCurBase + 4;
	if ( m_NpcBoothImaGood6.m_bMouseMove )
		return m_nCurBase + 5;

	return -1 ;
}
