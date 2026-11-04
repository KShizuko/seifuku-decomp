#include "stdafx.h"
#include "MyShell.h"
#include "DlgEnchant.h"

#include "Hero.h"
#include "Item.h"
#include "GameMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef _SOUND_DISABLE
	#define DXPlaySound
#endif

CDlgEnchant::CDlgEnchant(CWnd* pParent )
	: CDialog(CDlgEnchant::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_fFrameStep = 0.0f ;
	m_nFrameMode = 0 ;
	m_nPntX = 0 ;
	m_bPickUp = false ;
	m_nIconRealID = 0 ;
	m_GoodID = 0 ;
	m_GemID = 0 ;
	m_nGemIndex = 0 ;
	m_nGoodIndex = 0 ;
}


void CDlgEnchant::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
	DDX_Control(pDX, IDC_ENCHANT_BTN_CLOSES, m_EnchantBtnCloses);
	DDX_Control(pDX, IDC_ENCHANT_BTN_CLOSE, m_EnchantBtnClose);
	DDX_Control(pDX, IDC_ENCHANT_BTN_OK, m_EnchantBtnOk);
	DDX_Control(pDX, IDC_ENCHANT_IMA_GOOD, m_EnchantImaGood);
	DDX_Control(pDX, IDC_ENCHANT_IMA_GEM, m_EnchantImaGem);
}


BEGIN_MESSAGE_MAP(CDlgEnchant, CDialog)
	ON_BN_CLICKED(IDC_ENCHANT_BTN_CLOSE, OnEnchantBtnClose)
	ON_BN_CLICKED(IDC_ENCHANT_BTN_OK, OnEnchantBtnOk)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_ENCHANT_BTN_CLOSES, OnEnchantBtnCloses)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_ENCHANT_IMA_GOOD, OnEnchantImaGood)
	ON_BN_CLICKED(IDC_ENCHANT_IMA_GEM, OnEnchantImaGem)
	ON_WM_MOVE()
	ON_BN_DOUBLECLICKED(IDC_ENCHANT_IMA_GEM, OnDoubleclickedEnchantImaGem)
	ON_BN_DOUBLECLICKED(IDC_ENCHANT_IMA_GOOD, OnDoubleclickedEnchantImaGood)
END_MESSAGE_MAP()

void CDlgEnchant::OnEnchantBtnClose()
{
	CDlgEnchant::OnEnchantBtnCloses () ;
}

void CDlgEnchant::OnEnchantBtnOk()
{
	if ( !m_EnchantImaGem.GetImageLoadState () )
	{
		g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10472 ) ) ;
	}
	else if ( !m_EnchantImaGood.GetImageLoadState () )
	{
		g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10471 ) ) ;
	}
	else if ( m_EnchantImaGem.GetImageLoadState () && m_EnchantImaGood.GetImageLoadState () )
	{
		g_objHero.EnchantItem ( m_GoodID, m_GemID ) ;
		CDlgEnchant::ShowWindow ( SW_HIDE ) ;
		CDlgEnchant::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ENCHANT_CLOSE ) ;
		m_EnchantImaGem.RemoveImage () ;
		m_EnchantImaGood.RemoveImage () ;
		m_GoodID = 0 ;
		m_GemID = 0 ;
		m_nGemIndex = 0 ;
		m_nGoodIndex = 0 ;
	}
}

void CDlgEnchant::OnBtnHelp()
{
	CDlgEnchant::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_ENCHANT ) ;
}

void CDlgEnchant::OnEnchantBtnCloses()
{
	m_EnchantImaGem.RemoveImage () ;
	m_EnchantImaGood.RemoveImage () ;
	m_GoodID = 0 ;
	m_GemID = 0 ;
	m_nGemIndex = 0 ;
	m_nGoodIndex = 0 ;
	CDlgEnchant::ShowWindow ( SW_HIDE ) ;
	CDlgEnchant::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ENCHANT_CLOSE ) ;
}

void CDlgEnchant::SetPickUpState( BOOL bPickUp, int nIconRealID )
{
	m_bPickUp = bPickUp ;
	m_nIconRealID = nIconRealID ;
}

OBJID CDlgEnchant::GetMouseMoveId()
{
	if ( m_EnchantImaGem.m_bMouseMove )
		return m_GemID ;
	else if ( m_EnchantImaGood.m_bMouseMove )
		return m_GoodID ;
	return 0 ;
}

int CDlgEnchant::GetMouseMoveIndex()
{
	if ( m_EnchantImaGem.m_bMouseMove )
		return m_nGemIndex ;
	return m_nGoodIndex ;
}

HBRUSH CDlgEnchant::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr;
}

void CDlgEnchant::OnEnchantImaGood()
{
	if ( m_bPickUp && !m_EnchantImaGood.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem->GetSort() == ITEMSORT_HELMET ||
			pItem->GetSort() == ITEMSORT_NECKLACE ||
			pItem->GetSort() == ITEMSORT_ARMOR ||
			pItem->GetSort() == ITEMSORT_WEAPON_SINGLE_HAND ||
			pItem->GetSort() == ITEMSORT_WEAPON_DOUBLE_HAND ||
			pItem->GetSort() == ITEMSORT_SHIELD ||
			pItem->GetSort() == ITEMSORT_RING ||
			pItem->GetSort() == ITEMSORT_SHOES )
		{
			if ( pItem->GetAmount () != pItem->GetAmountLimit () )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10467 ) ) ;
				m_GoodID = 0 ;
				return ;
			}
			if ( pItem->GetEnchant () >= 255 )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10469 ) ) ;
				m_GoodID = 0 ;
				return ;
			}
			char strImage[32] ;
			sprintf ( strImage, "Item%u", pItem->GetTypeID () ) ;
			m_EnchantImaGood.InsertImage ( strImage ) ;
			m_GoodID = pItem->GetID () ;
			m_nGoodIndex = m_nIconRealID ;
			CDlgEnchant::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
			CDlgEnchant::GetParent ()->PostMessage ( WM_MY_MESSAGE, CMD_FLASHITEM ) ;
		}
		else
		{
			g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10468 ) ) ;
			m_GoodID = 0 ;
			m_nGoodIndex = 0 ;
		}
	}
}

void CDlgEnchant::OnEnchantImaGem()
{
	if ( m_bPickUp && !m_EnchantImaGem.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem != NULL && pItem->IsGem () )
		{
			char strImage[32] ;
			sprintf ( strImage, "Item%u", pItem->GetTypeID () ) ;
			m_EnchantImaGem.InsertImage ( strImage ) ;
			m_GemID = pItem->GetID () ;
			m_nGemIndex = m_nIconRealID ;
			CDlgEnchant::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
			CDlgEnchant::GetParent ()->PostMessage ( WM_MY_MESSAGE, CMD_FLASHITEM ) ;
		}
		else
		{
			g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10470 ) ) ;
			m_GemID = 0 ;
			m_nGemIndex = 0 ;
		}
	}
}

void CDlgEnchant::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_nPntX = x ;
	m_Pnt.y = y ;
}

LRESULT CDlgEnchant::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
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
	case WM_SHOWWINDOW:
		m_bShow = wParam ;
		if ( wParam )
		{
			CRect rect ;
			CDlgEnchant::GetWindowRect ( rect ) ;
			if ( m_nFrameMode == 0 )
			{
				DXPlaySound ( "Sound/DlgOpen.wav" ) ;
				m_dwLastShowTime = 0 ;
				m_nFrameMode = 1 ;
				CDlgEnchant::MoveWindow ( -rect.Width(), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
			}
			else if ( m_nFrameMode == 2 )
			{
				CDlgEnchant::MoveWindow ( 0, m_Pnt.y, rect.Width(), rect.Height(), true ) ;
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
		::DeleteObject ( m_Hbr ) ;
		break ;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CDlgEnchant::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgEnchant::GetWindowRect ( rect ) ;
	m_fFrameStep = ( float ) rect.Width () / 20 ;

	if ( ! m_EnchantImaGood.Init ( rect.left, rect.top, NULL, ITEM13_IMAGE ) )
		return false ;
	if ( ! m_EnchantImaGem.Init ( rect.left, rect.top, NULL, ITEM13_IMAGE ) )
		return false ;

	if ( ! m_EnchantBtnOk.Init ( rect.left, rect.top, "Button542" ) )
		return false ;
	if ( ! m_EnchantBtnCloses.Init ( rect.left, rect.top, "Button5" ) )
		return false ;
	m_EnchantBtnClose.Init ( rect.left, rect.top, "Button333" ) ;
	m_BtnHelp.Init ( rect.left, rect.top, "Button444" ) ;
	return TRUE;
}

void CDlgEnchant::Show()
{
	if (m_bShow)
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
					CDlgEnchant::WindowProc ( WM_SHOWWINDOW, true, NULL ) ;
				}
			}
		}

		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														  "Dialog55",
														  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0,
							m_Pnt.x,
							m_Pnt.y ) ;
		else
			return ;

		m_EnchantImaGem.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_EnchantImaGood.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_EnchantBtnOk.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_EnchantBtnCloses.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_EnchantBtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgEnchant::OnDoubleclickedEnchantImaGem()
{
	if ( m_EnchantImaGem.GetImageLoadState () )
	{
		m_EnchantImaGem.RemoveImage () ;
		CDlgEnchant::GetParent ()->PostMessage ( WM_MY_MESSAGE, CMD_FLASHITEM ) ;
		m_GemID = 0 ;
		m_nGemIndex = 0 ;
	}
}

void CDlgEnchant::OnDoubleclickedEnchantImaGood()
{
	if ( m_EnchantImaGood.GetImageLoadState () )
	{
		m_EnchantImaGood.RemoveImage () ;
		CDlgEnchant::GetParent ()->PostMessage ( WM_MY_MESSAGE, CMD_FLASHITEM ) ;
		m_GoodID = 0 ;
		m_nGoodIndex = 0 ;
	}
}
