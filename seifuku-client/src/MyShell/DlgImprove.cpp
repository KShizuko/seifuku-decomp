#include "stdafx.h"
#include "MyShell.h"
#include "DlgImprove.h"

#include "Hero.h"
#include "Item.h"
#include "GameMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgImprove::CDlgImprove(CWnd* pParent )
	: CDialog(CDlgImprove::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_fFrameStep = 0.0f ;
	m_nFrameMode = 0 ;
	m_nPntX = 0 ;
	m_bPickUp = false ;
	m_nImproveType = 0;
	m_nIconRealID = 0 ;
}


void CDlgImprove::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMPROVE_BTN_CLOSEB, m_ImproveBtnCloseB);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
	DDX_Control(pDX, IDC_IMPROVE_BTN_CLOSE, m_ImproveBtnClose);
	DDX_Control(pDX, IDC_IMPROVE_IMA_GOOD, m_ImproveImaGood);
	DDX_Control(pDX, IDC_IMPROVE_IMA_GEM, m_ImproveImaGem);
	DDX_Control(pDX, IDC_IMPROVE_BTN_IMP, m_ImproveBtnImp);
}


BEGIN_MESSAGE_MAP(CDlgImprove, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_IMPROVE_IMA_GEM, OnImproveImaGem)
	ON_BN_CLICKED(IDC_IMPROVE_IMA_GOOD, OnImproveImaGood)
	ON_BN_CLICKED(IDC_IMPROVE_BTN_IMP, OnImproveBtnImp)
	ON_BN_CLICKED(IDC_IMPROVE_BTN_CLOSE, OnImproveBtnClose)
	ON_BN_DOUBLECLICKED(IDC_IMPROVE_IMA_GEM, OnDoubleclickedImproveImaGem)
	ON_BN_DOUBLECLICKED(IDC_IMPROVE_IMA_GOOD, OnDoubleclickedImproveImaGood)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_IMPROVE_BTN_CLOSEB, OnImproveBtnCloseb)
END_MESSAGE_MAP()

LRESULT CDlgImprove::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_SHOWWINDOW:
		m_bShow = wParam ;
		if ( wParam )
		{
			CRect rect ;
			CDlgImprove::GetWindowRect ( rect ) ;
			if ( m_nFrameMode == 0 )
			{
				DXPlaySound ( "Sound/DlgOpen.wav" ) ;
				m_dwLastShowTime = 0 ;
				m_nFrameMode = 1 ;
				CDlgImprove::MoveWindow ( -rect.Width(), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
			}
			else if ( m_nFrameMode == 2 )
			{
				CDlgImprove::MoveWindow ( 0, m_Pnt.y, rect.Width(), rect.Height(), true ) ;
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

HBRUSH CDlgImprove::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgImprove::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgImprove::GetWindowRect ( rect ) ;
	m_fFrameStep = ( float ) rect.Width () / 20 ;

	if ( ! m_ImproveImaGood.Init ( rect.left, rect.top, NULL, ITEM5_IMAGE ) )
		return false ;
	if ( ! m_ImproveImaGem.Init ( rect.left, rect.top, NULL, ITEM5_IMAGE ) )
		return false ;

	if ( ! m_ImproveBtnImp.Init ( rect.left, rect.top, "Button260" ) )
		return false ;
	if ( ! m_ImproveBtnClose.Init ( rect.left, rect.top, "Button5" ) )
		return false ;
	m_ImproveBtnCloseB.Init ( rect.left, rect.top, "Button333" ) ;
	m_BtnHelp.Init ( rect.left, rect.top, "Button444" ) ;

	return TRUE;
}

void CDlgImprove::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_nPntX = x ;
	m_Pnt.y = y ;
}

void CDlgImprove::Show()
{
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
					CDlgImprove::WindowProc ( WM_SHOWWINDOW, true, NULL ) ;
				}
			}
		}

		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														  "Dialog26",
														  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0, m_Pnt.x,m_Pnt.y ) ;
		else
			return ;

		m_ImproveImaGem.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ImproveImaGood.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_ImproveBtnImp.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ImproveBtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ImproveBtnCloseB.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgImprove::OnImproveImaGem()
{
	if ( m_bPickUp && !m_ImproveImaGem.GetImageLoadState () )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem != NULL )
		{
			OBJID ItemType = pItem->GetTypeID() ;
			if ( ItemType == 1088000 )
				m_nImproveType = 1 ;
			else if ( ItemType == 1088001 || ItemType == 1088002 )
				m_nImproveType = 2 ;
			else
			{
				m_nImproveType = 0 ;
				return ;
			}
			char strImage[32] ;
			sprintf ( strImage, "Item%u", ItemType ) ;
			m_ImproveImaGem.InsertImage ( strImage ) ;
			m_GemID = pItem->GetID () ;
			CDlgImprove::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
			m_nGemIndex = m_nIconRealID ;
			CDlgImprove::GetParent ()->PostMessage ( WM_MY_MESSAGE, CMD_FLASHITEM ) ;
		}
	}
}

void CDlgImprove::OnImproveImaGood()
{
	if ( m_bPickUp && !m_ImproveImaGood.GetImageLoadState ())
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
			if ( pItem->GetAmount() != pItem->GetAmountLimit () )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10373 ), 2005, 0xFF0000, 0, false ) ;
				m_GoodID = 0 ;
				return ;
			}
			char strImage[32] ;
			sprintf ( strImage, "Item%u", pItem->GetTypeID() ) ;
			m_ImproveImaGood.InsertImage ( strImage ) ;
			m_GoodID = pItem->GetID () ;
			CDlgImprove::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
			m_nGoodIndex = m_nIconRealID ;
			CDlgImprove::GetParent ()->PostMessage ( WM_MY_MESSAGE, CMD_FLASHITEM ) ;
		}
		else
			m_GoodID = 0 ;
	}
}

void CDlgImprove::OnImproveBtnImp()
{
	if (m_ImproveImaGood.GetImageLoadState() && m_ImproveImaGem.GetImageLoadState())
	{
		if ( m_nImproveType == 1 )
			g_objHero.ImproveItem ( m_GoodID, m_GemID ) ;
		else if ( m_nImproveType == 2 )
			g_objHero.UpLevelItem ( m_GoodID, m_GemID ) ;
		else
			return ;
		CDlgImprove::ShowWindow ( SW_HIDE ) ;
		CDlgImprove::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_IMPROVE_CLOSE ) ;
		m_ImproveImaGem.RemoveImage () ;
		m_ImproveImaGood.RemoveImage () ;
		m_nImproveType = 0 ;
		m_GoodID = 0 ;
		m_GemID = 0 ;
		m_nGemIndex = 0 ;
		m_nGoodIndex = 0 ;
	}
}

void CDlgImprove::OnImproveBtnClose()
{
	m_ImproveImaGem.RemoveImage () ;
	m_ImproveImaGood.RemoveImage () ;
	CDlgImprove::ShowWindow ( SW_HIDE ) ;
	CDlgImprove::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_IMPROVE_CLOSE ) ;
}

void CDlgImprove::SetPickUpState(BOOL bPickUp, int nIconRealID)
{
	m_bPickUp = bPickUp ;
	m_nIconRealID = nIconRealID ;
}

DWORD CDlgImprove::GetMouseMoveId()
{
	if ( m_ImproveImaGem.m_bMouseMove )
		return m_nGemIndex ;
	else
		return m_nGoodIndex ;
}

void CDlgImprove::OnDoubleclickedImproveImaGem()
{
	if ( m_ImproveImaGem.GetImageLoadState () )
	{
		m_ImproveImaGem.RemoveImage () ;
		CDlgImprove::GetParent ()->PostMessage ( WM_MY_MESSAGE, CMD_FLASHITEM ) ;
	}
}

void CDlgImprove::OnDoubleclickedImproveImaGood()
{
	if ( m_ImproveImaGood.GetImageLoadState () )
	{
		m_ImproveImaGood.RemoveImage () ;
		CDlgImprove::GetParent ()->PostMessage ( WM_MY_MESSAGE, CMD_FLASHITEM ) ;
	}
}

void CDlgImprove::OnBtnHelp()
{
	CDlgImprove::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_IMPROVE ) ;
}

void CDlgImprove::OnImproveBtnCloseb()
{
	CDlgImprove::OnImproveBtnClose () ;
}
