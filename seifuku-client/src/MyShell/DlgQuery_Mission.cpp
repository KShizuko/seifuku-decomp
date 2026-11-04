#include "stdafx.h"
#include "MyShell.h"
#include "DlgQuery_Mission.h"

#include "Hero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgQuery_Mission::CDlgQuery_Mission(CWnd* pParent )
	: CDialog(CDlgQuery_Mission::IDD, pParent)
{
	m_bInitSuc = false ;
	m_Hbr = CreateSolidBrush ( 0x2132B ) ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_bPickUp = false ;
	m_bShow = false ;
	m_bLHandEnable = false ;
	m_nIconRealID = -1 ;
}


void CDlgQuery_Mission::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MISSION_IMA_GARMENT, m_ImgGarment);
	DDX_Control(pDX, IDC_MISSION_IMA_TREASURE, m_ImgTreasure);
	DDX_Control(pDX, IDC_MISSION_IMA_HEAD, m_ImgHead);
	DDX_Control(pDX, IDC_MISSION_IMA_NECK, m_ImgNeck);
	DDX_Control(pDX, IDC_MISSION_IMA_GEAR, m_ImgGear);
	DDX_Control(pDX, IDC_MISSION_IMA_BODY, m_ImgBody);
	DDX_Control(pDX, IDC_MISSION_IMA_LHAND, m_ImgLHand);
	DDX_Control(pDX, IDC_MISSION_IMA_RHAND, m_ImgRHand);
	DDX_Control(pDX, IDC_MISSION_IMA_FOOT, m_ImgShoe);
}


BEGIN_MESSAGE_MAP(CDlgQuery_Mission, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_MISSION_IMA_HEAD, OnMissionImaHead)
	ON_BN_CLICKED(IDC_MISSION_IMA_NECK, OnMissionImaNeck)
	ON_BN_CLICKED(IDC_MISSION_IMA_GEAR, OnMissionImaGear)
	ON_BN_CLICKED(IDC_MISSION_IMA_BODY, OnMissionImaBody)
	ON_BN_CLICKED(IDC_MISSION_IMA_LHAND, OnMissionImaLhand)
	ON_BN_CLICKED(IDC_MISSION_IMA_RHAND, OnMissionImaRhand)
	ON_BN_CLICKED(IDC_MISSION_IMA_FOOT, OnMissionImaFoot)
	ON_WM_SHOWWINDOW()
	ON_BN_DOUBLECLICKED(IDC_MISSION_IMA_HEAD, OnDoubleclickedMissionImaHead)
	ON_BN_DOUBLECLICKED(IDC_MISSION_IMA_LHAND, OnDoubleclickedMissionImaLhand)
	ON_BN_DOUBLECLICKED(IDC_MISSION_IMA_NECK, OnDoubleclickedMissionImaNeck)
	ON_BN_DOUBLECLICKED(IDC_MISSION_IMA_BODY, OnDoubleclickedMissionImaBody)
	ON_BN_DOUBLECLICKED(IDC_MISSION_IMA_FOOT, OnDoubleclickedMissionImaFoot)
	ON_BN_DOUBLECLICKED(IDC_MISSION_IMA_GEAR, OnDoubleclickedMissionImaGear)
	ON_BN_DOUBLECLICKED(IDC_MISSION_IMA_RHAND, OnDoubleclickedMissionImaRhand)
	ON_BN_CLICKED(IDC_MISSION_IMA_TREASURE, OnMissionImaTreasure)
	ON_BN_DOUBLECLICKED(IDC_MISSION_IMA_TREASURE, OnDoubleclickedMissionImaTreasure)
	ON_BN_CLICKED(IDC_MISSION_IMA_GARMENT, OnMissionImaGarment)
	ON_BN_DOUBLECLICKED(IDC_MISSION_IMA_GARMENT, OnDoubleclickedMissionImaGarment)
END_MESSAGE_MAP()

HBRUSH CDlgQuery_Mission::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgQuery_Mission::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgQuery_Mission::GetWindowRect ( rect ) ;

	if ( ! ( m_ImgHead.Init ( rect.left, rect.top, NULL, ITEM_IMAGE, ITEMPOSITION_HELMET ) ) )
		return false ;
	if ( ! ( m_ImgNeck.Init ( rect.left, rect.top, NULL, ITEM_IMAGE, ITEMPOSITION_NECKLACE ) ) )
		return false ;
	if ( ! ( m_ImgGear.Init ( rect.left, rect.top, NULL, ITEM_IMAGE, ITEMPOSITION_RINGR ) ) )
		return false ;
	if ( ! ( m_ImgBody.Init ( rect.left, rect.top, NULL, ITEM_IMAGE, ITEMPOSITION_ARMOR ) ) )
		return false ;
	if ( ! ( m_ImgLHand.Init ( rect.left, rect.top, NULL, ITEM_IMAGE, ITEMPOSITION_WEAPONL ) ) )
		return false ;
	if ( ! ( m_ImgRHand.Init ( rect.left, rect.top, NULL, ITEM_IMAGE, ITEMPOSITION_WEAPONR ) ) )
		return false ;
	if ( ! ( m_ImgShoe.Init ( rect.left, rect.top, NULL, ITEM_IMAGE, ITEMPOSITION_SHOES ) ) )
		return false ;
	if ( ! ( m_ImgTreasure.Init ( rect.left, rect.top, NULL, ITEM_IMAGE, ITEMPOSITION_TREASURE ) ) )
		return false ;
	if ( ! ( m_ImgGarment.Init ( rect.left, rect.top, NULL, ITEM_IMAGE, ITEMPOSITION_MOUNT ) ) )
		return false ;

	m_bInitSuc = true ;
	return TRUE;
}

void CDlgQuery_Mission::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgQuery_Mission::Show(int x,int y)
{
	if ( m_bInitSuc )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog16",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni == NULL )
			return ;
		ShowAni->Show ( 0, m_Pnt.x + x, m_Pnt.y + y ) ;

		CMyPos RolePos;
		RolePos.x = m_Pnt.x + x + 80;
		RolePos.y = m_Pnt.y + y + 225;

		m_ImgHead.Show ( m_Pnt.x+x, m_Pnt.y +y) ;
		m_ImgNeck.Show ( m_Pnt.x+x, m_Pnt.y +y) ;
		m_ImgGear.Show ( m_Pnt.x+x, m_Pnt.y +y) ;
		m_ImgBody.Show ( m_Pnt.x+x, m_Pnt.y +y) ;
		if ( m_bLHandEnable )
		{
			IAni* HandAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														  "Image160",
														  EXIGENCE_IMMEDIATE ) ;
			if ( HandAni != NULL )
				HandAni->Show ( 0, m_Pnt.x + x + 56, m_Pnt.y + y + 248 ) ;
			m_ImgLHand.Show ( m_Pnt.x+x, m_Pnt.y +y) ;
		}
		m_ImgRHand.Show ( m_Pnt.x+x, m_Pnt.y +y) ;
		m_ImgShoe.Show ( m_Pnt.x+x, m_Pnt.y +y) ;
		m_ImgTreasure.Show ( m_Pnt.x+x, m_Pnt.y +y) ;
		m_ImgGarment.Show ( m_Pnt.x+x, m_Pnt.y +y) ;

		g_objHero.m_objDummy.ShowDummy(RolePos,150,-15);
	}
}

LRESULT CDlgQuery_Mission::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_COMMAND )
	{
		if ( wParam == 1 || wParam == 2 )
			return true ;
	}
	else if ( message == WM_DESTROY )
	{
		::DeleteObject ( m_Hbr ) ;
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

void CDlgQuery_Mission::OnMissionImaHead()
{
	if ( m_bPickUp )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem != NULL && pItem->GetSort () == ITEMSORT_HELMET )
		{
			if ( IsEquipEnable () )
			{
				if ( m_ImgHead.GetImageLoadState () )
					g_objHero.UnequipItem ( ITEMPOSITION_HELMET ) ;
				HeroEquipItem ( ITEMPOSITION_HELMET ) ;
			}
		}
	}
}

void CDlgQuery_Mission::OnMissionImaNeck()
{
	if ( m_bPickUp )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem != NULL && pItem->GetSort () == ITEMSORT_NECKLACE )
		{
			if ( IsEquipEnable () )
			{
				if ( m_ImgNeck.GetImageLoadState () )
					g_objHero.UnequipItem ( ITEMPOSITION_NECKLACE ) ;
				HeroEquipItem ( ITEMPOSITION_NECKLACE ) ;
			}
		}
	}
}

void CDlgQuery_Mission::OnMissionImaGear()
{
	if ( m_bPickUp )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem != NULL && pItem->GetSort () == ITEMSORT_RING )
		{
			if ( IsEquipEnable () )
			{
				if ( m_ImgGear.GetImageLoadState () )
					g_objHero.UnequipItem ( ITEMPOSITION_RINGR ) ;
				HeroEquipItem ( ITEMPOSITION_RINGR ) ;
			}
		}
	}
}

void CDlgQuery_Mission::OnMissionImaBody()
{
	if ( m_bPickUp )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem != NULL && pItem->GetSort () == ITEMSORT_ARMOR )
		{
			if ( IsEquipEnable () )
			{
				if ( m_ImgBody.GetImageLoadState () )
					g_objHero.UnequipItem ( ITEMPOSITION_ARMOR ) ;
				HeroEquipItem ( ITEMPOSITION_ARMOR ) ;
			}
		}
	}
}

void CDlgQuery_Mission::OnMissionImaLhand()
{
	if ( m_bPickUp )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem != NULL )
		{
			BOOL bEquip = false ;
			if ( IsEquipEnable ()
				&& g_objHero.GetProfession () % 10 >= 2
				&& g_objHero.GetProfession () % 1000 / 10 == 1
				&& pItem->GetSort () == ITEMSORT_WEAPON_SINGLE_HAND )
				bEquip = true ;
			else if ( pItem->GetTypeID () / 10000 == 105
				&& g_objHero.GetProfession () >= 40 && g_objHero.GetProfession () <= 45
				&& g_objHero.HaveBow () )
				bEquip = true ;
			else if ( pItem->GetSort () == ITEMSORT_SHIELD && IsEquipEnable () )
				bEquip = true ;
			if ( bEquip )
			{
				if ( m_ImgLHand.GetImageLoadState () )
					g_objHero.UnequipItem ( ITEMPOSITION_WEAPONL ) ;
				HeroEquipItem ( ITEMPOSITION_WEAPONL ) ;
			}
		}
	}
}

void CDlgQuery_Mission::OnMissionImaRhand()
{
	if ( m_bPickUp )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem != NULL
			&& ( pItem->GetSort () == ITEMSORT_WEAPON_SINGLE_HAND || pItem->GetSort () == ITEMSORT_WEAPON_DOUBLE_HAND )
			&& pItem->GetSort () != ITEMSORT_SHIELD )
		{
			if ( IsEquipEnable () )
			{
				if ( m_ImgRHand.GetImageLoadState () )
					g_objHero.UnequipItem ( ITEMPOSITION_WEAPONR ) ;
				HeroEquipItem ( ITEMPOSITION_WEAPONR ) ;
			}
		}
	}
}

void CDlgQuery_Mission::OnMissionImaFoot()
{
	if ( m_bPickUp )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem != NULL && pItem->GetSort () == ITEMSORT_SHOES )
		{
			if ( IsEquipEnable () )
			{
				if ( m_ImgShoe.GetImageLoadState () )
					g_objHero.UnequipItem ( ITEMPOSITION_SHOES ) ;
				HeroEquipItem ( ITEMPOSITION_SHOES ) ;
			}
		}
	}
}

void CDlgQuery_Mission::OnMissionImaTreasure()
{
	if ( m_bPickUp )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem != NULL && pItem->GetSort () == ITEMSORT_MOUNT )
		{
			if ( IsEquipEnable () )
			{
				if ( m_ImgTreasure.GetImageLoadState () )
					g_objHero.UnequipItem ( ITEMPOSITION_TREASURE ) ;
				HeroEquipItem ( ITEMPOSITION_TREASURE ) ;
			}
		}
	}
}

void CDlgQuery_Mission::OnMissionImaGarment()
{
	if ( m_bPickUp )
	{
		CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
		if ( pItem != NULL && pItem->GetSort () == ITEMSORT_SPRITE )
		{
			if ( IsEquipEnable () )
			{
				if ( m_ImgGarment.GetImageLoadState () )
					g_objHero.UnequipItem ( ITEMPOSITION_MOUNT ) ;
				HeroEquipItem ( ITEMPOSITION_MOUNT ) ;
			}
		}
	}
}

BOOL CDlgQuery_Mission::HeroEquipItem(int nPos)
{
	CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
	DXPlaySound ( "Sound/Equip.wav" ) ;
	g_objHero.EquipItem( nPos, pItem->GetID () ) ;
	m_bPickUp = false ;
	CDlgQuery_Mission::GetParent ()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_ICON_DROP,  DLG_ALL) ;
	return true ;
}

void CDlgQuery_Mission::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	FlashEquip () ;
}

void CDlgQuery_Mission::FlashEquip()
{
	CItem *pEquip = NULL ;
	CString strImage ;
	DWORD dwTypeID ;
	BYTE btActType ;

	m_ImgBody.RemoveImage() ;
	pEquip = g_objHero.GetEquipment( ITEMPOSITION_ARMOR ) ;
	if ( pEquip != NULL )
	{
		dwTypeID = pEquip->GetTypeID () ;
		btActType = 0 ;
		if ( dwTypeID / 100000 < 10 && ( dwTypeID < 700000 || dwTypeID > 800000 ) )
		{
			switch ( dwTypeID % 10 )
			{
			case 9: btActType = 1 ; break ;
			case 8: btActType = 2 ; break ;
			case 7: btActType = 3 ; break ;
			case 6: btActType = 4 ; break ;
			}
			m_ImgBody.SetActType ( btActType ) ;
		}
		strImage.Format ( "Item%u", dwTypeID ) ;
		m_ImgBody.InsertImage( ( char* ) ( LPCTSTR ) strImage );
	}

	m_ImgTreasure.RemoveImage() ;
	pEquip = g_objHero.GetEquipment( ITEMPOSITION_TREASURE ) ;
	if ( pEquip != NULL )
	{
		dwTypeID = pEquip->GetTypeID () ;
		btActType = 0 ;
		switch ( dwTypeID % 10 )
		{
		case 9: btActType = 1 ; break ;
		case 8: btActType = 2 ; break ;
		case 7: btActType = 3 ; break ;
		case 6: btActType = 4 ; break ;
		}
		m_ImgTreasure.SetActType ( btActType ) ;
		strImage.Format ( "Item%u", dwTypeID ) ;
		m_ImgTreasure.InsertImage( ( char* ) ( LPCTSTR ) strImage );
	}

	m_ImgGarment.RemoveImage() ;
	pEquip = g_objHero.GetEquipment( ITEMPOSITION_MOUNT ) ;
	if ( pEquip != NULL )
	{
		dwTypeID = pEquip->GetTypeID () ;
		btActType = 0 ;
		switch ( dwTypeID % 10 )
		{
		case 9: btActType = 1 ; break ;
		case 8: btActType = 2 ; break ;
		case 7: btActType = 3 ; break ;
		case 6: btActType = 4 ; break ;
		}
		m_ImgGarment.SetActType ( btActType ) ;
		strImage.Format ( "Item%u", dwTypeID ) ;
		m_ImgGarment.InsertImage( ( char* ) ( LPCTSTR ) strImage );
	}

	m_ImgShoe.RemoveImage() ;
	pEquip = g_objHero.GetEquipment( ITEMPOSITION_SHOES ) ;
	if ( pEquip != NULL )
	{
		dwTypeID = pEquip->GetTypeID () ;
		btActType = 0 ;
		if ( dwTypeID / 100000 < 10 && ( dwTypeID < 700000 || dwTypeID > 800000 ) )
		{
			switch ( dwTypeID % 10 )
			{
			case 9: btActType = 1 ; break ;
			case 8: btActType = 2 ; break ;
			case 7: btActType = 3 ; break ;
			case 6: btActType = 4 ; break ;
			}
			m_ImgShoe.SetActType ( btActType ) ;
		}
		strImage.Format ( "Item%u", dwTypeID ) ;
		m_ImgShoe.InsertImage( ( char* ) ( LPCTSTR ) strImage );
	}

	m_ImgGear.RemoveImage() ;
	pEquip = g_objHero.GetEquipment( ITEMPOSITION_RINGR ) ;
	if ( pEquip != NULL )
	{
		dwTypeID = pEquip->GetTypeID () ;
		btActType = 0 ;
		if ( dwTypeID / 100000 < 10 && ( dwTypeID < 700000 || dwTypeID > 800000 ) )
		{
			switch ( dwTypeID % 10 )
			{
			case 9: btActType = 1 ; break ;
			case 8: btActType = 2 ; break ;
			case 7: btActType = 3 ; break ;
			case 6: btActType = 4 ; break ;
			}
			m_ImgGear.SetActType ( btActType ) ;
		}
		strImage.Format ( "Item%u", dwTypeID ) ;
		m_ImgGear.InsertImage( ( char* ) ( LPCTSTR ) strImage );
	}

	m_ImgHead.RemoveImage() ;
	pEquip = g_objHero.GetEquipment( ITEMPOSITION_HELMET ) ;
	if ( pEquip != NULL )
	{
		dwTypeID = pEquip->GetTypeID () ;
		btActType = 0 ;
		if ( dwTypeID / 100000 < 10 && ( dwTypeID < 700000 || dwTypeID > 800000 ) )
		{
			switch ( dwTypeID % 10 )
			{
			case 9: btActType = 1 ; break ;
			case 8: btActType = 2 ; break ;
			case 7: btActType = 3 ; break ;
			case 6: btActType = 4 ; break ;
			}
			m_ImgHead.SetActType ( btActType ) ;
		}
		strImage.Format ( "Item%u", dwTypeID ) ;
		m_ImgHead.InsertImage( ( char* ) ( LPCTSTR ) strImage );
	}

	if ( ( g_objHero.GetProfession () >= 11 && g_objHero.GetProfession () <= 15 )
		|| ( g_objHero.GetProfession () >= 21 && g_objHero.GetProfession () <= 25 )
		|| ( g_objHero.GetProfession () >= 40 && g_objHero.GetProfession () <= 45 ) )
	{
		m_bLHandEnable = true ;
		m_ImgLHand.EnableWindow ( true ) ;
		m_ImgLHand.RemoveImage() ;
		pEquip = g_objHero.GetEquipment( ITEMPOSITION_WEAPONL ) ;
		if ( pEquip != NULL )
		{
			dwTypeID = pEquip->GetTypeID () ;
			btActType = 0 ;
			if ( dwTypeID / 100000 < 10 && ( dwTypeID < 700000 || dwTypeID > 800000 ) )
			{
				switch ( dwTypeID % 10 )
				{
				case 9: btActType = 1 ; break ;
				case 8: btActType = 2 ; break ;
				case 7: btActType = 3 ; break ;
				case 6: btActType = 4 ; break ;
				}
				m_ImgLHand.SetActType ( btActType ) ;
			}
			strImage.Format ( "Item%u", dwTypeID ) ;
			m_ImgLHand.InsertImage( ( char* ) ( LPCTSTR ) strImage );
		}
	}
	else
	{
		m_bLHandEnable = false ;
		m_ImgLHand.EnableWindow ( false ) ;
	}

	m_ImgNeck.RemoveImage() ;
	pEquip = g_objHero.GetEquipment( ITEMPOSITION_NECKLACE ) ;
	if ( pEquip != NULL )
	{
		dwTypeID = pEquip->GetTypeID () ;
		btActType = 0 ;
		if ( dwTypeID / 100000 < 10 && ( dwTypeID < 700000 || dwTypeID > 800000 ) )
		{
			switch ( dwTypeID % 10 )
			{
			case 9: btActType = 1 ; break ;
			case 8: btActType = 2 ; break ;
			case 7: btActType = 3 ; break ;
			case 6: btActType = 4 ; break ;
			}
			m_ImgNeck.SetActType ( btActType ) ;
		}
		strImage.Format ( "Item%u", dwTypeID ) ;
		m_ImgNeck.InsertImage( ( char* ) ( LPCTSTR ) strImage );
	}

	m_ImgRHand.RemoveImage() ;
	pEquip = g_objHero.GetEquipment( ITEMPOSITION_WEAPONR ) ;
	if ( pEquip != NULL )
	{
		dwTypeID = pEquip->GetTypeID () ;
		btActType = 0 ;
		if ( dwTypeID / 100000 < 10 && ( dwTypeID < 700000 || dwTypeID > 800000 ) )
		{
			switch ( dwTypeID % 10 )
			{
			case 9: btActType = 1 ; break ;
			case 8: btActType = 2 ; break ;
			case 7: btActType = 3 ; break ;
			case 6: btActType = 4 ; break ;
			}
			m_ImgRHand.SetActType ( btActType ) ;
		}
		strImage.Format ( "Item%u", dwTypeID ) ;
		m_ImgRHand.InsertImage( ( char* ) ( LPCTSTR ) strImage );
	}
}

void CDlgQuery_Mission::OnDoubleclickedMissionImaHead()
{
	if ( !m_bPickUp )
	{
		DXPlaySound ( "Sound/Unequip.wav" ) ;
		g_objHero.UnequipItem ( ITEMPOSITION_HELMET ) ;
	}
}

void CDlgQuery_Mission::OnDoubleclickedMissionImaLhand()
{
	if ( !m_bPickUp )
	{
		DXPlaySound ( "Sound/Unequip.wav" ) ;
		g_objHero.UnequipItem ( ITEMPOSITION_WEAPONL ) ;
	}
}

void CDlgQuery_Mission::OnDoubleclickedMissionImaNeck()
{
	if ( !m_bPickUp )
	{
		DXPlaySound ( "Sound/Unequip.wav" ) ;
		g_objHero.UnequipItem ( ITEMPOSITION_NECKLACE ) ;
	}
}

void CDlgQuery_Mission::OnDoubleclickedMissionImaBody()
{
	if ( !m_bPickUp )
	{
		DXPlaySound ( "Sound/Unequip.wav" ) ;
		g_objHero.UnequipItem ( ITEMPOSITION_ARMOR ) ;
	}
}

void CDlgQuery_Mission::OnDoubleclickedMissionImaFoot()
{
	if ( !m_bPickUp )
	{
		DXPlaySound ( "Sound/Unequip.wav" ) ;
		g_objHero.UnequipItem ( ITEMPOSITION_SHOES ) ;
	}
}

void CDlgQuery_Mission::OnDoubleclickedMissionImaGear()
{
	if ( !m_bPickUp )
	{
		DXPlaySound ( "Sound/Unequip.wav" ) ;
		g_objHero.UnequipItem ( ITEMPOSITION_RINGR ) ;
	}
}

void CDlgQuery_Mission::OnDoubleclickedMissionImaRhand()
{
	if ( !m_bPickUp )
	{
		DXPlaySound ( "Sound/Unequip.wav" ) ;
		g_objHero.UnequipItem ( ITEMPOSITION_WEAPONR ) ;
	}
}

void CDlgQuery_Mission::OnDoubleclickedMissionImaTreasure()
{
	if ( !m_bPickUp )
	{
		DXPlaySound ( "Sound/Unequip.wav" ) ;
		g_objHero.UnequipItem ( ITEMPOSITION_TREASURE ) ;
	}
}

void CDlgQuery_Mission::OnDoubleclickedMissionImaGarment()
{
	if ( !m_bPickUp )
	{
		DXPlaySound ( "Sound/Unequip.wav" ) ;
		g_objHero.UnequipItem ( ITEMPOSITION_MOUNT ) ;
	}
}

int CDlgQuery_Mission::GetMouseMovePosition()
{
	if ( m_ImgHead.m_bMouseMove )
		return ITEMPOSITION_HELMET ;
	if ( m_ImgNeck.m_bMouseMove )
		return ITEMPOSITION_NECKLACE ;
	if ( m_ImgGear.m_bMouseMove )
		return ITEMPOSITION_RINGR ;
	if ( m_ImgBody.m_bMouseMove )
		return ITEMPOSITION_ARMOR ;
	if ( m_ImgLHand.m_bMouseMove )
		return ITEMPOSITION_WEAPONL ;
	if ( m_ImgRHand.m_bMouseMove )
		return ITEMPOSITION_WEAPONR ;
	if ( m_ImgShoe.m_bMouseMove )
		return ITEMPOSITION_SHOES ;
	if ( m_ImgTreasure.m_bMouseMove )
		return ITEMPOSITION_TREASURE ;
	if ( m_ImgGarment.m_bMouseMove )
		return ITEMPOSITION_MOUNT ;
	return -1 ;
}

BOOL CDlgQuery_Mission::IsEquipEnable()
{
	CItem* pItem = g_objHero.GetItemByIndex ( m_nIconRealID ) ;
	if ( pItem->GetForceRequired () > g_objHero.GetForce () )
		return false ;
	if ( pItem->GetHealthRequired () > g_objHero.GetHealth () )
		return false ;
	if ( pItem->GetLevelRequired () > g_objHero.GetLev () )
		return false ;
	if ( pItem->GetProfessionRequired () != 0 )
	{
		int nSort = pItem->GetProfessionRequired () % 1000 / 10 ;
		int nLevel = pItem->GetProfessionRequired () % 10 ;
		if ( nSort == 19 )
		{
			if ( g_objHero.GetProfessionSort () < 10 )
				return false ;
		}
		else if ( nSort != ( int ) g_objHero.GetProfessionSort () )
			return false ;
		if ( ( int ) g_objHero.GetProfessionLevel () < nLevel )
			return false ;
	}
	if ( pItem->GetSexRequried () != 0 && pItem->GetSexRequried () != g_objHero.GetSex () )
		return false ;
	if ( pItem->GetSoulRequired () > g_objHero.GetSoul () )
		return false ;
	if ( pItem->GetSpeedRequired () > g_objHero.GetSpeed () )
		return false ;
	if ( pItem->GetWeaponSkillRequired () == 0 )
		return true ;
	CWeaponSkill* pSkill = g_objHero.GetWeaponSkill ( pItem->GetWeaponSkillType () ) ;
	if ( pSkill == NULL )
		return false ;
	return pSkill->GetLevel () >= pItem->GetWeaponSkillRequired () ;
}

void CDlgQuery_Mission::SetPickUpState( BOOL bPickUp, int nIconRealID )
{
	m_bPickUp = bPickUp ;
	m_nIconRealID = nIconRealID ;
}
