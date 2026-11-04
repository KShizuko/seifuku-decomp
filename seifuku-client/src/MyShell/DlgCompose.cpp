#include "stdafx.h"
#include "MyShell.h"
#include "DlgCompose.h"

#include "Hero.h"
#include "Item.h"
#include "GameMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgCompose::CDlgCompose(CWnd* pParent )
	: CDialog(CDlgCompose::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = CreateSolidBrush ( 0x2132B ) ;
	m_fFrameStep = 0.0f ;
	m_nFrameMode = 0 ;
	m_nPntX = 0 ;
	m_bPickUp = false ;
	m_nIconRealID = 0 ;
	m_idItem = 0 ;
	m_idItem1 = 0 ;
	m_idItem2 = 0 ;
	m_idGem1 = 0 ;
	m_idGem2 = 0 ;
	m_idItemType = 0 ;
	m_btAddition = 0 ;
	m_btLevel = 0 ;
}


void CDlgCompose::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMPOSE_IMG_ITEM2, m_ImgItem2);
	DDX_Control(pDX, IDC_COMPOSE_IMG_ITEM1, m_ImgItem1);
	DDX_Control(pDX, IDC_COMPOSE_IMG_ITEM, m_ImgItem);
	DDX_Control(pDX, IDC_COMPOSE_IMG_GEM2, m_ImgGem2);
	DDX_Control(pDX, IDC_COMPOSE_IMG_GEM1, m_ImgGem1);
	DDX_Control(pDX, IDC_COMPOSE_BTN_OK, m_BtnOk);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
	DDX_Control(pDX, IDC_COMPOSE_BTN_CLOSE, m_BtnClose);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_BtnCloseB);
}


BEGIN_MESSAGE_MAP(CDlgCompose, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBtnClose)
	ON_BN_CLICKED(IDC_COMPOSE_BTN_CLOSE, OnComposeBtnClose)
	ON_BN_CLICKED(IDC_COMPOSE_IMG_ITEM, OnComposeImgItem)
	ON_BN_CLICKED(IDC_COMPOSE_IMG_ITEM1, OnComposeImgItem1)
	ON_BN_CLICKED(IDC_COMPOSE_IMG_ITEM2, OnComposeImgItem2)
	ON_BN_CLICKED(IDC_COMPOSE_IMG_GEM1, OnComposeImgGem1)
	ON_BN_CLICKED(IDC_COMPOSE_IMG_GEM2, OnComposeImgGem2)
	ON_BN_CLICKED(IDC_COMPOSE_BTN_OK, OnComposeBtnOk)
END_MESSAGE_MAP()

LRESULT CDlgCompose::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_SHOWWINDOW:
		m_bShow = wParam ;
		if ( wParam )
		{
			CRect rect ;
			CDlgCompose::GetWindowRect ( rect ) ;
			if ( m_nFrameMode == 0 )
			{
				DXPlaySound ( "Sound/DlgOpen.wav" ) ;
				m_dwLastShowTime = 0 ;
				m_nFrameMode = 1 ;
				CDlgCompose::MoveWindow ( -rect.Width(), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
			}
			else if ( m_nFrameMode == 2 )
			{
				CDlgCompose::MoveWindow ( 0, m_Pnt.y, rect.Width(), rect.Height(), true ) ;
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
			return false ;
		}
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

HBRUSH CDlgCompose::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgCompose::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgCompose::GetWindowRect ( rect ) ;
	m_fFrameStep = ( float ) rect.Width () / DLG_FRAME_AMOUNT ;

	m_BtnHelp.Init ( rect.left, rect.top, "Button444" ) ;
	m_BtnClose.Init ( rect.left, rect.top, "Button5" ) ;
	m_BtnCloseB.Init ( rect.left, rect.top, "Button333" ) ;
	m_BtnOk.Init ( rect.left, rect.top, "Button555" ) ;

	m_ImgItem2.Init ( rect.left, rect.top, NULL, ITEM6_IMAGE ) ;
	m_ImgItem1.Init ( rect.left, rect.top, NULL, ITEM6_IMAGE ) ;
	m_ImgItem.Init ( rect.left, rect.top, NULL, ITEM6_IMAGE ) ;
	m_ImgGem2.Init ( rect.left, rect.top, NULL, ITEM6_IMAGE ) ;
	m_ImgGem1.Init ( rect.left, rect.top, NULL, ITEM6_IMAGE ) ;
	m_ImgItem2.EnableWindow ( false ) ;
	m_ImgItem1.EnableWindow ( false ) ;
	m_ImgGem2.EnableWindow ( false ) ;
	m_ImgGem1.EnableWindow ( false ) ;

	return TRUE;
}

void CDlgCompose::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_nPntX = x ;
	m_Pnt.y = y ;
}

void CDlgCompose::Show()
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
					CDlgCompose::WindowProc(WM_SHOWWINDOW,true,NULL);
				}
			}
			break ;
		case 2:
			break ;
		}

		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														  "Dialog51",
														  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni == NULL )
			return ;
		ShowAni->Show ( 0, m_Pnt.x, m_Pnt.y ) ;

		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnCloseB.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnOk.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_ImgItem.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_ImgItem1.IsWindowEnabled () )
		{
			ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													"Image510",
													EXIGENCE_IMMEDIATE ) ;
			if ( ShowAni != NULL )
			{
				ShowAni->Show ( 0, m_Pnt.x + 136, m_Pnt.y + 50 ) ;
				ShowAni->Show ( 0, m_Pnt.x + 136, m_Pnt.y + 96 ) ;
				m_ImgItem2.Show ( m_Pnt.x, m_Pnt.y ) ;
				m_ImgItem1.Show ( m_Pnt.x, m_Pnt.y ) ;
			}
		}
		if ( m_ImgGem1.IsWindowEnabled () )
		{
			ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													"Image511",
													EXIGENCE_IMMEDIATE ) ;
			if ( ShowAni != NULL )
			{
				ShowAni->Show ( 0, m_Pnt.x + 44, m_Pnt.y + 134 ) ;
				ShowAni->Show ( 0, m_Pnt.x + 92, m_Pnt.y + 134 ) ;
				m_ImgGem2.Show ( m_Pnt.x, m_Pnt.y ) ;
				m_ImgGem1.Show ( m_Pnt.x, m_Pnt.y ) ;
			}
		}
	}
}

void CDlgCompose::OnBtnHelp()
{
	CDlgCompose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_EXP ) ;
}

void CDlgCompose::OnBtnClose()
{
	CDlgCompose::ShowWindow ( SW_HIDE ) ;
	CDlgCompose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EMBED_CLOSE ) ;
	if ( m_bPickUp )
	{
		m_bPickUp = false ;
		CDlgCompose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSG_1003 ) ;
	}
	m_bPickUp = false ;
	m_nIconRealID = 0 ;
	m_idItem = 0 ;
	m_idItem1 = 0 ;
	m_idItem2 = 0 ;
	m_idGem1 = 0 ;
	m_idGem2 = 0 ;
	m_ImgItem2.RemoveImage () ;
	m_ImgItem1.RemoveImage () ;
	m_ImgItem.RemoveImage () ;
	m_ImgGem2.RemoveImage () ;
	m_ImgGem1.RemoveImage () ;
	m_ImgItem1.EnableWindow ( false ) ;
	m_ImgItem2.EnableWindow ( false ) ;
	m_ImgGem1.EnableWindow ( false ) ;
	m_ImgGem2.EnableWindow ( false ) ;
}

void CDlgCompose::OnComposeBtnClose()
{
	CDlgCompose::OnBtnClose () ;
}

void CDlgCompose::OnComposeImgItem()
{
	if ( m_bPickUp && !m_ImgItem.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem->GetSort() == ITEMSORT_HELMET ||
			pItem->GetSort() == ITEMSORT_NECKLACE ||
			pItem->GetSort() == ITEMSORT_ARMOR ||
			pItem->GetSort() == ITEMSORT_WEAPON_SINGLE_HAND ||
			pItem->GetSort() == ITEMSORT_WEAPON_DOUBLE_HAND ||
			pItem->GetSort() == ITEMSORT_SHIELD ||
			pItem->GetSort() == ITEMSORT_RING ||
			pItem->GetSort() == ITEMSORT_SHOES ||
			pItem->GetTypeID () / 1000 == 730 )
		{
			if ( pItem->GetTypeID () / 1000 == 730 )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10455 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
				return ;
			}
			if ( pItem->GetTypeID () / 1000000 != 0 )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10412 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
				return ;
			}
			if ( pItem->GetAddition () == 9 )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10429 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
				return ;
			}

			char strName[32] ;
			sprintf ( strName, "Item%u", pItem->GetTypeID () ) ;
			m_btAddition = pItem->GetAddition () ;
			m_ImgItem1.EnableWindow ( true ) ;
			m_ImgItem2.EnableWindow ( true ) ;
			if ( m_btAddition >= 5 )
			{
				m_ImgGem1.EnableWindow ( true ) ;
				m_ImgGem2.EnableWindow ( true ) ;
			}
			m_ImgItem.InsertImage ( strName ) ;
			m_idItem = pItem->GetID () ;
			m_idItemType = pItem->GetTypeID () ;
			m_btLevel = pItem->GetLevelRequired () ;
			CDlgCompose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
			CDlgCompose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSG_1003 ) ;
		}
	}
}

void CDlgCompose::OnComposeImgItem1()
{
	if ( m_bPickUp && !m_ImgItem1.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem->GetSort() == ITEMSORT_HELMET ||
			pItem->GetSort() == ITEMSORT_NECKLACE ||
			pItem->GetSort() == ITEMSORT_ARMOR ||
			pItem->GetSort() == ITEMSORT_WEAPON_SINGLE_HAND ||
			pItem->GetSort() == ITEMSORT_WEAPON_DOUBLE_HAND ||
			pItem->GetSort() == ITEMSORT_SHIELD ||
			pItem->GetSort() == ITEMSORT_RING ||
			pItem->GetSort() == ITEMSORT_SHOES ||
			pItem->GetTypeID () / 1000 == 730 )
		{
			if ( pItem->GetID () == m_idItem || pItem->GetID () == m_idItem2 )
				return ;

			DWORD dwMainClass = m_idItemType / 1000 ;
			DWORD dwClass = pItem->GetTypeID () / 1000 ;
			if ( dwMainClass == 421 || dwMainClass == 500 || dwClass == 421 || dwClass == 500 )
			{
				if ( dwMainClass != dwClass && dwClass != 730 )
				{
					g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10413 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
					return ;
				}
			}
			else if ( m_idItemType / 100000 == 4 || m_idItemType / 100000 == 5 )
			{
				if ( pItem->GetTypeID () / 100000 != m_idItemType / 100000 && dwClass != 730 )
				{
					g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10413 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
					return ;
				}
			}
			else if ( CItem::ItemGetSort ( m_idItemType ) == ITEMSORT_SHIELD )
			{
				if ( pItem->GetSort () != ITEMSORT_SHIELD && !pItem->IsAdditionStone () )
				{
					g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10413 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
					return ;
				}
			}
			else if ( CItem::ItemGetBaseClass ( m_idItemType ) != CItem::ItemGetBaseClass ( pItem->GetTypeID () ) && !pItem->IsAdditionStone () )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10413 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
				return ;
			}

			if ( pItem->GetAddition () == 0 )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10431 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
				return ;
			}
			if ( m_btAddition > pItem->GetAddition () )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10427 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
				return ;
			}
			if ( dwClass != 730 )
			{
				DWORD dwMainQuality = m_idItemType % 10 ;
				if ( dwMainQuality < 5 )
					dwMainQuality = 5 ;
				DWORD dwQuality = pItem->GetTypeID () % 10 ;
				if ( dwQuality < 5 )
					dwQuality = 5 ;
				if ( dwMainQuality < dwQuality )
				{
					g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10428 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
					return ;
				}
			}

			char strName[32] ;
			sprintf ( strName, "Item%u", pItem->GetTypeID () ) ;
			m_ImgItem1.InsertImage ( strName ) ;
			m_idItem1 = pItem->GetID () ;
			CDlgCompose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
			CDlgCompose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSG_1003 ) ;
		}
	}
}

void CDlgCompose::OnComposeImgItem2()
{
	if ( m_bPickUp && !m_ImgItem2.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem->GetSort() == ITEMSORT_HELMET ||
			pItem->GetSort() == ITEMSORT_NECKLACE ||
			pItem->GetSort() == ITEMSORT_ARMOR ||
			pItem->GetSort() == ITEMSORT_WEAPON_SINGLE_HAND ||
			pItem->GetSort() == ITEMSORT_WEAPON_DOUBLE_HAND ||
			pItem->GetSort() == ITEMSORT_SHIELD ||
			pItem->GetSort() == ITEMSORT_RING ||
			pItem->GetSort() == ITEMSORT_SHOES ||
			pItem->GetTypeID () / 1000 == 730 )
		{
			if ( pItem->GetID () == m_idItem || pItem->GetID () == m_idItem1 )
				return ;

			DWORD dwMainClass = m_idItemType / 1000 ;
			DWORD dwClass = pItem->GetTypeID () / 1000 ;
			if ( dwMainClass == 421 || dwMainClass == 500 || dwClass == 421 || dwClass == 500 )
			{
				if ( dwMainClass != dwClass && dwClass != 730 )
				{
					g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10413 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
					return ;
				}
			}
			else if ( m_idItemType / 100000 == 4 || m_idItemType / 100000 == 5 )
			{
				if ( pItem->GetTypeID () / 100000 != m_idItemType / 100000 && dwClass != 730 )
				{
					g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10413 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
					return ;
				}
			}
			else if ( CItem::ItemGetSort ( m_idItemType ) == ITEMSORT_SHIELD )
			{
				if ( pItem->GetSort () != ITEMSORT_SHIELD && !pItem->IsAdditionStone () )
				{
					g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10413 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
					return ;
				}
			}
			else if ( CItem::ItemGetBaseClass ( m_idItemType ) != CItem::ItemGetBaseClass ( pItem->GetTypeID () ) && !pItem->IsAdditionStone () )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10413 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
				return ;
			}

			if ( pItem->GetAddition () == 0 )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10431 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
				return ;
			}
			if ( m_btAddition > pItem->GetAddition () )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10427 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
				return ;
			}
			if ( dwClass != 730 )
			{
				DWORD dwMainQuality = m_idItemType % 10 ;
				if ( dwMainQuality < 5 )
					dwMainQuality = 5 ;
				DWORD dwQuality = pItem->GetTypeID () % 10 ;
				if ( dwQuality < 5 )
					dwQuality = 5 ;
				if ( dwMainQuality < dwQuality )
				{
					g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10428 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
					return ;
				}
			}

			char strName[32] ;
			sprintf ( strName, "Item%u", pItem->GetTypeID () ) ;
			m_ImgItem2.InsertImage ( strName ) ;
			m_idItem2 = pItem->GetID () ;
			CDlgCompose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
			CDlgCompose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSG_1003 ) ;
		}
	}
}

void CDlgCompose::OnComposeImgGem1()
{
	if ( m_bPickUp && !m_ImgGem1.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem != NULL && pItem->IsGem () && pItem->GetID () != m_idGem2 )
		{
			if ( pItem->GetTypeID () / 10000 != 70 )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10416 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
				return ;
			}
			char strName[32] ;
			sprintf ( strName, "Item%u", pItem->GetTypeID () ) ;
			m_ImgGem1.InsertImage ( strName ) ;
			m_idGem1 = pItem->GetID () ;
			CDlgCompose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
			CDlgCompose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSG_1003 ) ;
		}
	}
}

void CDlgCompose::OnComposeImgGem2()
{
	if ( m_bPickUp && !m_ImgGem2.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem != NULL && pItem->IsGem () && pItem->GetID () != m_idGem1 )
		{
			if ( pItem->GetTypeID () / 10000 != 70 )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10416 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
				return ;
			}
			char strName[32] ;
			sprintf ( strName, "Item%u", pItem->GetTypeID () ) ;
			m_ImgGem2.InsertImage ( strName ) ;
			m_idGem2 = pItem->GetID () ;
			CDlgCompose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
			CDlgCompose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSG_1003 ) ;
		}
	}
}

void CDlgCompose::OnComposeBtnOk()
{
	if ( m_idItem == 0 )
	{
		g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10410 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
		return ;
	}
	if ( m_btAddition >= 5 )
	{
		if ( m_idItemType / 100000 == 4 || m_idItemType / 100000 == 5 )
		{
			if ( m_idGem1 == 0 || m_idGem2 == 0 )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10414 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
				return ;
			}
		}
		else if ( m_idGem1 == 0 && m_idGem2 == 0 )
		{
			g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10415 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
			return ;
		}
	}

	int nAmount = 0 ;
	if ( m_idItem != 0 )
		nAmount++ ;
	if ( m_idItem1 != 0 )
		nAmount++ ;
	if ( m_idItem2 != 0 )
		nAmount++ ;
	if ( m_idGem1 != 0 )
		nAmount++ ;
	if ( m_idGem2 != 0 )
		nAmount++ ;
	if ( nAmount < 3 )
	{
		g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10411 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
		return ;
	}

	if ( m_idItem1 != 0 && m_idItem2 != 0 )
	{
		g_objHero.SendDataArray ( m_idItem, m_idItem1, m_idItem2, m_idGem1, m_idGem2 ) ;
		CDlgCompose::OnBtnClose () ;
	}
}

void CDlgCompose::SetPickUpState(BOOL bPickUp, int nIconRealID)
{
	m_bPickUp = bPickUp ;
	m_nIconRealID = nIconRealID ;
}

BOOL CDlgCompose::GetMouseMoveId(OBJID& idItem)
{
	if ( m_ImgItem.m_bMouseMove && m_ImgItem.GetImageLoadState () )
	{
		idItem = m_idItem ;
		return true ;
	}
	if ( m_ImgItem1.m_bMouseMove && m_ImgItem1.GetImageLoadState () )
	{
		idItem = m_idItem1 ;
		return true ;
	}
	if ( m_ImgItem2.m_bMouseMove && m_ImgItem2.GetImageLoadState () )
	{
		idItem = m_idItem2 ;
		return true ;
	}
	if ( m_ImgGem1.m_bMouseMove && m_ImgGem1.GetImageLoadState () )
	{
		idItem = m_idGem1 ;
		return true ;
	}
	if ( m_ImgGem2.m_bMouseMove && m_ImgGem2.GetImageLoadState () )
	{
		idItem = m_idGem2 ;
		return true ;
	}
	return false ;
}
