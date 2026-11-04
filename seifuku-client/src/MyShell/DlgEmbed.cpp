#include "stdafx.h"
#include "MyShell.h"
#include "DlgEmbed.h"

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

CDlgEmbed::CDlgEmbed(CWnd* pParent )
	: CDialog(CDlgEmbed::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_fFrameStep = 0.0f ;
	m_nFrameMode = 0 ;
	m_nPntX = 0 ;
	m_bPickUp = false ;
	m_nIconRealID = 0 ;
	m_bGem1Old = false ;
	m_bGem2Old = false ;
	m_nCurGem = 0 ;
	m_bTackOffWait = false ;
	m_nTackOffGem = 0 ;
}


void CDlgEmbed::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EMBED_CLOSES, m_EmbedBtnCloses);
	DDX_Control(pDX, IDC_EMBED_IMA_GOOD, m_EmbedImaGood);
	DDX_Control(pDX, IDC_EMBED_IMA_GEM2, m_EmbedImaGem2);
	DDX_Control(pDX, IDC_EMBED_IMA_GEM1, m_EmbedImaGem1);
	DDX_Control(pDX, IDC_EMBED_CLOSE, m_EmbedBtnClose);
	DDX_Control(pDX, IDC_EMBED_BTN_TACKOFF, m_EmbedBtnTackOff);
	DDX_Control(pDX, IDC_EMBED_BTN_EMBED, m_EmbedBtnEmbed);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
}


BEGIN_MESSAGE_MAP(CDlgEmbed, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_EMBED_IMA_GOOD, OnEmbedImaGood)
	ON_BN_CLICKED(IDC_EMBED_CLOSE, OnEmbedClose)
	ON_BN_CLICKED(IDC_EMBED_IMA_GEM1, OnEmbedImaGem1)
	ON_BN_CLICKED(IDC_EMBED_IMA_GEM2, OnEmbedImaGem2)
	ON_BN_DOUBLECLICKED(IDC_EMBED_IMA_GOOD, OnDoubleclickedEmbedImaGood)
	ON_BN_DOUBLECLICKED(IDC_EMBED_IMA_GEM2, OnDoubleclickedEmbedImaGem2)
	ON_BN_DOUBLECLICKED(IDC_EMBED_IMA_GEM1, OnDoubleclickedEmbedImaGem1)
	ON_BN_CLICKED(IDC_EMBED_BTN_EMBED, OnEmbedBtnEmbed)
	ON_BN_CLICKED(IDC_EMBED_BTN_TACKOFF, OnEmbedBtnTackOff)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_EMBED_CLOSES, OnCloseB)
END_MESSAGE_MAP()

LRESULT CDlgEmbed::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_DESTROY:
		::DeleteObject ( m_Hbr ) ;
		break ;
	case WM_SHOWWINDOW:
		m_bShow = wParam ;
		if ( wParam )
		{
			CRect rect ;
			CDlgEmbed::GetWindowRect ( rect ) ;
			if ( m_nFrameMode == 0 )
			{
				DXPlaySound ( "Sound/DlgOpen.wav" ) ;
				m_dwLastShowTime = 0 ;
				m_nFrameMode = 1 ;
				CDlgEmbed::MoveWindow ( -rect.Width(), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
			}
			else if ( m_nFrameMode == 2 )
			{
				CDlgEmbed::MoveWindow ( 0, m_Pnt.y, rect.Width(), rect.Height(), true ) ;
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

HBRUSH CDlgEmbed::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr;
}

BOOL CDlgEmbed::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgEmbed::GetWindowRect ( rect ) ;
	m_fFrameStep = ( float ) rect.Width () / 20 ;

	if ( ! m_EmbedImaGood.Init ( rect.left, rect.top, NULL, ITEM6_IMAGE ) )
		return false ;
	if ( ! m_EmbedImaGem1.Init ( rect.left, rect.top, NULL, ITEM6_IMAGE ) )
		return false ;
	m_EmbedImaGem1.EnableWindow ( false ) ;
	if ( ! m_EmbedImaGem2.Init ( rect.left, rect.top, NULL, ITEM6_IMAGE ) )
		return false ;
	m_EmbedImaGem2.EnableWindow ( false ) ;

	if ( ! m_EmbedBtnEmbed.Init ( rect.left, rect.top, "Button290" ) )
		return false ;
	if ( ! m_EmbedBtnTackOff.Init ( rect.left, rect.top, "Button291" ) )
		return false ;
	if ( ! m_EmbedBtnClose.Init ( rect.left, rect.top, "Button333" ) )
		return false ;
	if ( ! m_EmbedBtnCloses.Init ( rect.left, rect.top, "Button5" ) )
		return false ;
	if ( ! m_BtnHelp.Init ( rect.left, rect.top, "Button444" ) )
		return false ;
	return true ;
}

void CDlgEmbed::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_nPntX = x ;
	m_Pnt.y = y ;
}

void CDlgEmbed::Show()
{
	if ( m_bTackOffWait )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nGoodIndex ) ;
		if ( pItem != NULL )
		{
			switch ( m_nTackOffGem )
			{
			case 1:
				if ( pItem->GetGem1 () == 0xFF )
				{
					m_bTackOffWait = false ;
					m_nTackOffGem = 0 ;
					m_nCurGem = 0 ;
					m_EmbedImaGem1.RemoveImage () ;
				}
				break ;
			case 2:
				if ( pItem->GetGem2 () == 0xFF )
				{
					m_bTackOffWait = false ;
					m_nTackOffGem = 0 ;
					m_nCurGem = 0 ;
					m_EmbedImaGem2.RemoveImage () ;
				}
				break ;
			case 3:
				if ( pItem->GetGem1 () == 0xFF && pItem->GetGem2 () == 0xFF )
				{
					m_bTackOffWait = false ;
					m_nTackOffGem = 0 ;
					m_nCurGem = 0 ;
					m_EmbedImaGem1.RemoveImage () ;
					m_EmbedImaGem2.RemoveImage () ;
				}
				break ;
			}
		}
	}

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
					CDlgEmbed::WindowProc ( WM_SHOWWINDOW, true, NULL ) ;
				}
			}
		}

		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														  "Dialog29",
														  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0,
							m_Pnt.x,
							m_Pnt.y ) ;
		else
			return ;

		m_EmbedImaGood.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_EmbedImaGem1.IsWindowEnabled () )
		{
			m_EmbedImaGem1.ShowBk ( "Dialog30" ) ;
			m_EmbedImaGem1.Show ( m_Pnt.x, m_Pnt.y ) ;
		}
		if ( m_EmbedImaGem2.IsWindowEnabled () )
		{
			m_EmbedImaGem2.ShowBk ( "Dialog30" ) ;
			m_EmbedImaGem2.Show ( m_Pnt.x, m_Pnt.y ) ;
		}
		switch ( m_nCurGem )
		{
		case 1:
			m_EmbedImaGem1.ShowFocus () ;
			break ;
		case 2:
			m_EmbedImaGem2.ShowFocus () ;
			break ;
		}

		m_EmbedBtnEmbed.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_EmbedBtnTackOff.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_EmbedBtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_EmbedBtnCloses.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgEmbed::SetPickUpState( BOOL bPickUp, int nIconRealID )
{
	m_bPickUp = bPickUp ;
	m_nIconRealID = nIconRealID ;
}

void CDlgEmbed::OnEmbedImaGood()
{
	if ( m_bPickUp && !m_EmbedImaGood.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( ( pItem->GetSort() == ITEMSORT_HELMET ||
			pItem->GetSort() == ITEMSORT_NECKLACE ||
			pItem->GetSort() == ITEMSORT_ARMOR ||
			pItem->GetSort() == ITEMSORT_WEAPON_SINGLE_HAND ||
			pItem->GetSort() == ITEMSORT_WEAPON_DOUBLE_HAND ||
			pItem->GetSort() == ITEMSORT_SHIELD ||
			pItem->GetSort() == ITEMSORT_RING ||
			pItem->GetSort() == ITEMSORT_SHOES ) &&
			( pItem->GetGem1 () != 0 || pItem->GetGem2 () != 0 ) )
		{
			char strImage[32] ;
			sprintf ( strImage, "Item%u", pItem->GetTypeID () ) ;
			m_EmbedImaGood.InsertImage ( strImage ) ;
			m_GoodID = pItem->GetID () ;
			CDlgEmbed::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
			m_nGoodIndex = m_nIconRealID ;
			CDlgEmbed::GetParent ()->PostMessage ( WM_MY_MESSAGE, CMD_FLASHITEM ) ;
			if ( pItem->GetGem1 () != 0 )
			{
				m_EmbedImaGem1.EnableWindow ( true ) ;
				if ( pItem->GetGem1 () != 0xFF && pItem->GetGem1 () != 0 )
				{
					sprintf ( strImage, "Item%u", 700000 + pItem->GetGem1 () ) ;
					m_EmbedImaGem1.InsertImage ( strImage ) ;
					m_bGem1Old = true ;
					m_Gem1ID = 700000 + pItem->GetGem1 () ;
				}
			}
			if ( pItem->GetGem2 () != 0 )
			{
				m_EmbedImaGem2.EnableWindow ( true ) ;
				if ( pItem->GetGem2 () != 0xFF && pItem->GetGem2 () != 0 )
				{
					sprintf ( strImage, "Item%u", 700000 + pItem->GetGem2 () ) ;
					m_EmbedImaGem2.InsertImage ( strImage ) ;
					m_bGem2Old = true ;
					m_Gem2ID = 700000 + pItem->GetGem2 () ;
				}
			}
		}
		else
			m_GoodID = 0 ;
	}
}

void CDlgEmbed::OnEmbedImaGem1()
{
	if ( m_bPickUp && !m_EmbedImaGem1.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem != NULL && pItem->IsGem () )
		{
			char strImage[32] ;
			sprintf ( strImage, "Item%u", pItem->GetTypeID () ) ;
			m_EmbedImaGem1.InsertImage ( strImage ) ;
			m_Gem1ID = pItem->GetID () ;
			CDlgEmbed::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
			m_nGem1Index = m_nIconRealID ;
			CDlgEmbed::GetParent ()->PostMessage ( WM_MY_MESSAGE, CMD_FLASHITEM ) ;
		}
		else
			m_Gem1ID = 0;
	}
	else
		m_nCurGem = 1 ;
}

void CDlgEmbed::OnEmbedImaGem2()
{
	if ( m_bPickUp && !m_EmbedImaGem2.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem != NULL && pItem->IsGem () )
		{
			char strImage[32] ;
			sprintf ( strImage, "Item%u", pItem->GetTypeID () ) ;
			m_EmbedImaGem2.InsertImage ( strImage ) ;
			m_Gem2ID = pItem->GetID () ;
			CDlgEmbed::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
			m_nGem2Index = m_nIconRealID ;
			CDlgEmbed::GetParent ()->PostMessage ( WM_MY_MESSAGE, CMD_FLASHITEM ) ;
		}
		else
			m_Gem2ID = 0;
	}
	else
		m_nCurGem = 2 ;
}

void CDlgEmbed::OnEmbedClose()
{
	m_EmbedImaGem1.EnableWindow ( false ) ;
	m_EmbedImaGem2.EnableWindow ( false ) ;
	m_EmbedImaGem1.RemoveImage () ;
	m_EmbedImaGem2.RemoveImage () ;
	m_EmbedImaGood.RemoveImage () ;
	m_bGem1Old = false ;
	m_bGem2Old = false ;
	m_GoodID = 0 ;
	m_Gem1ID = 0 ;
	m_Gem2ID = 0 ;
	m_nGoodIndex = 0 ;
	m_nGem1Index = 0 ;
	m_nGem2Index = 0 ;
	m_nCurGem = 0 ;
	m_bTackOffWait = false ;
	m_nTackOffGem = 0 ;
	CDlgEmbed::ShowWindow ( SW_HIDE ) ;
	CDlgEmbed::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EMBED_CLOSE ) ;
}

void CDlgEmbed::OnDoubleclickedEmbedImaGood()
{
	if ( m_EmbedImaGood.GetImageLoadState () )
	{
		m_EmbedImaGem1.RemoveImage () ;
		m_EmbedImaGem2.RemoveImage () ;
		m_EmbedImaGood.RemoveImage () ;
		m_nCurGem = 0 ;
		CDlgEmbed::GetParent ()->PostMessage ( WM_MY_MESSAGE, CMD_FLASHITEM ) ;
	}
}

void CDlgEmbed::OnDoubleclickedEmbedImaGem2()
{
	if ( !m_bGem2Old )
	{
		m_nCurGem = 0 ;
		m_EmbedImaGem2.RemoveImage () ;
		CDlgEmbed::GetParent ()->PostMessage ( WM_MY_MESSAGE, CMD_FLASHITEM ) ;
	}
}

void CDlgEmbed::OnDoubleclickedEmbedImaGem1()
{
	if ( !m_bGem1Old )
	{
		m_nCurGem = 0 ;
		m_EmbedImaGem1.RemoveImage () ;
		CDlgEmbed::GetParent ()->PostMessage ( WM_MY_MESSAGE, CMD_FLASHITEM ) ;
	}
}

void CDlgEmbed::OnEmbedBtnEmbed()
{
	if ( m_EmbedImaGood.GetImageLoadState () )
	{
		if ( m_EmbedImaGem1.GetImageLoadState () )
			g_objHero.EmbedGem ( m_GoodID, m_Gem1ID, 1 ) ;
		if ( m_EmbedImaGem2.GetImageLoadState () )
			g_objHero.EmbedGem ( m_GoodID, m_Gem2ID, 2 ) ;
		CDlgEmbed::OnEmbedClose();
	}
}

void CDlgEmbed::OnEmbedBtnTackOff()
{
	if ( ( m_nCurGem == 1 && m_bGem1Old ) || ( m_nCurGem == 2 && m_bGem2Old ) )
	{
		CDlgEmbed::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EMBED_TACKOFF ) ;
	}
}

void CDlgEmbed::TackOffGem()
{
	if ( m_nCurGem == 1 )
	{
		if ( m_bGem1Old )
		{
			g_objHero.TakeOutGem ( m_GoodID, 1 ) ;
			m_bTackOffWait = true ;
			m_nTackOffGem = m_nTackOffGem ? 3 : 1 ;
		}
	}
	else if ( m_nCurGem == 2 )
	{
		if ( m_bGem2Old )
		{
			g_objHero.TakeOutGem ( m_GoodID, 2 ) ;
			m_bTackOffWait = true ;
			m_nTackOffGem = m_nTackOffGem ? 3 : 2 ;
		}
	}
}

void CDlgEmbed::OnBtnHelp()
{
	CDlgEmbed::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_EMBED ) ;
}

int CDlgEmbed::GetMouseMoveId(OBJID* pidItem)
{
	if ( m_EmbedImaGood.m_bMouseMove && m_EmbedImaGood.GetImageLoadState () )
	{
		*pidItem = m_nGoodIndex ;
		return 1 ;
	}
	if ( m_EmbedImaGem1.m_bMouseMove && m_EmbedImaGem1.GetImageLoadState () )
	{
		if ( m_bGem1Old )
		{
			*pidItem = m_Gem1ID ;
			return 2 ;
		}
		*pidItem = m_nGem1Index ;
		return 1 ;
	}
	if ( m_EmbedImaGem2.m_bMouseMove && m_EmbedImaGem2.GetImageLoadState () )
	{
		if ( m_bGem2Old )
		{
			*pidItem = m_Gem2ID ;
			return 2 ;
		}
		*pidItem = m_nGem2Index ;
		return 1 ;
	}
	return 0 ;
}

void CDlgEmbed::OnCloseB()
{
	CDlgEmbed::OnEmbedClose();
}
