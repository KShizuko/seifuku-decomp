#include "stdafx.h"
#include "MyShell.h"
#include "DlgNpcEquip.h"

#include "Hero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgNpcEquip::CDlgNpcEquip(CWnd* pParent )
	: CDialog(CDlgNpcEquip::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = CreateSolidBrush ( 0x2132B ) ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_fFrameStep = 0.0f ;
	m_nFrameMode = 0 ;
	m_nPntX = 0 ;
}


void CDlgNpcEquip::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NPCEQUIP_IMA_GARMENT, m_ImgGarment);
	DDX_Control(pDX, IDC_NPCEQUIP_IMA_TREASURE, m_ImgTreasure);
	DDX_Control(pDX, IDC_STA_NAME, m_StaName);
	DDX_Control(pDX, IDC_STA_MATE, m_StaFaction);
	DDX_Control(pDX, IDC_NPCEQUIP_IMA_BODY, m_ImgBody);
	DDX_Control(pDX, IDC_NPCEQUIP_IMA_HEAD, m_ImgHead);
	DDX_Control(pDX, IDC_NPCEQUIP_IMA_NECK, m_ImgNeck);
	DDX_Control(pDX, IDC_NPCEQUIP_IMA_GEAR, m_ImgGear);
	DDX_Control(pDX, IDC_NPCEQUIP_IMA_LHAND, m_ImgLHand);
	DDX_Control(pDX, IDC_NPCEQUIP_IMA_RHAND, m_ImgRHand);
	DDX_Control(pDX, IDC_NPCEQUIP_IMA_FOOT, m_ImgShoe);
	DDX_Control(pDX, IDC_NPCEQUIP_CLOSEB, m_BtnCloseB);
	DDX_Control(pDX, IDC_NPCEQUIP_CLOSE, m_BtnClose);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
}


BEGIN_MESSAGE_MAP(CDlgNpcEquip, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_NPCEQUIP_CLOSE, OnNpcequipClose)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_NPCEQUIP_CLOSEB, OnNpcequipCloseb)
END_MESSAGE_MAP()

LRESULT CDlgNpcEquip::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
		if ( wParam == false )
		{
			DXPlaySound ( "Sound/DlgClose.wav" ) ;
		}
		else
		{
			CRect rect ;
			CDlgNpcEquip::GetWindowRect( rect ) ;
			if ( m_nFrameMode == 0 )
			{
				DXPlaySound ( "Sound/DlgOpen.wav" ) ;
				m_dwLastShowTime = 0 ;
				m_nFrameMode = 1 ;
				CDlgNpcEquip::MoveWindow ( CMyBitmap::GetScreenWidth (), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
			}
			else if ( m_nFrameMode == 2 )
			{
				CDlgNpcEquip::MoveWindow ( CMyBitmap::GetScreenWidth () - rect.Width(), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
				m_nFrameMode = 0 ;
			}
		}
	}
	else if ( message == WM_COMMAND )
	{
		if ( wParam == 1 )
			return true ;
		else if (wParam == 2)
		{
			return true;
		}
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

HBRUSH CDlgNpcEquip::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgNpcEquip::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgNpcEquip::GetWindowRect ( rect ) ;
	m_fFrameStep = ( float ) rect.Width () / 20 ;

	m_ImgHead.Init ( rect.left, rect.top, NULL, ITEM12_IMAGE, ITEMPOSITION_HELMET ) ;
	m_ImgNeck.Init ( rect.left, rect.top, NULL, ITEM12_IMAGE, ITEMPOSITION_NECKLACE ) ;
	m_ImgGear.Init ( rect.left, rect.top, NULL, ITEM12_IMAGE, ITEMPOSITION_RINGR ) ;
	m_ImgBody.Init ( rect.left, rect.top, NULL, ITEM12_IMAGE, ITEMPOSITION_ARMOR ) ;
	m_ImgLHand.Init ( rect.left, rect.top, NULL, ITEM12_IMAGE, ITEMPOSITION_WEAPONL ) ;
	m_ImgRHand.Init ( rect.left, rect.top, NULL, ITEM12_IMAGE, ITEMPOSITION_WEAPONR ) ;
	m_ImgShoe.Init ( rect.left, rect.top, NULL, ITEM12_IMAGE, ITEMPOSITION_SHOES ) ;
	m_ImgTreasure.Init ( rect.left, rect.top, NULL, ITEM12_IMAGE, ITEMPOSITION_TREASURE ) ;
	m_ImgGarment.Init ( rect.left, rect.top, NULL, ITEM12_IMAGE, ITEMPOSITION_TREASURE ) ;

	m_BtnClose.Init ( rect.left, rect.top, "Button5" ) ;
	m_BtnCloseB.Init ( rect.left, rect.top, "Button3" ) ;
	m_BtnHelp.Init ( rect.left, rect.top, "Button4" ) ;

	m_StaName.Init(rect.left, rect.top, 0, NULL, "NULL");
	m_StaFaction.Init(rect.left,rect.top, 0, NULL, "NULL");

	return TRUE;
}

void CDlgNpcEquip::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_nPntX = x ;
	m_Pnt.y = y ;
}

void CDlgNpcEquip::Show()
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
					m_Pnt.x -= ( TimeGet() - m_dwLastShowTime ) / 15 * m_fFrameStep;
				}
				else
				{
					CRect rect ;
					CDlgNpcEquip::GetWindowRect( rect ) ;
					m_Pnt.x = CMyBitmap::GetScreenWidth () -  rect.Width() ;
					m_nFrameMode = 2 ;
					CDlgNpcEquip::WindowProc ( WM_SHOWWINDOW, true, NULL ) ;
				}
			}
		}

		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog42",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0, m_Pnt.x, m_Pnt.y );
		else
			return ;

		CMyPos RolePos;
		RolePos.x = m_Pnt.x + 100;
		RolePos.y = m_Pnt.y + 280;
		g_objHero.m_objPlayerDummy.ShowDummy(RolePos,150,-15);

		m_ImgBody.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ImgHead.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ImgNeck.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ImgTreasure.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ImgGarment.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ImgGear.Show ( m_Pnt.x, m_Pnt.y ) ;

		IAni* HandAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Image160",
													  EXIGENCE_IMMEDIATE ) ;
		if ( HandAni != NULL )
			HandAni->Show ( 0, m_Pnt.x + 72, m_Pnt.y + 285 );

		m_ImgLHand.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ImgRHand.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ImgShoe.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_BtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnCloseB.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_StaFaction.Show(m_Pnt.x, m_Pnt.y );
		m_StaName.Show(m_Pnt.x,m_Pnt.y );
	}
}

void CDlgNpcEquip::SetNpcEquip()
{
	m_StaName.SetWindowText(g_objHero.m_objPlayerDummy.GetName());
	m_StaFaction.SetWindowText(g_objHero.m_objPlayerDummy.GetMate());

	CItem* pItem = NULL;
	CString strImage ;
	DWORD dwTypeID ;
	BYTE btActType ;

	m_ImgBody.RemoveImage() ;
	pItem = g_objHero.m_objPlayerDummy.GetEquipment( ITEMPOSITION_ARMOR ) ;
	if ( pItem != NULL )
	{
		dwTypeID = pItem->GetTypeID () ;
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

	m_ImgShoe.RemoveImage() ;
	pItem = g_objHero.m_objPlayerDummy.GetEquipment( ITEMPOSITION_SHOES ) ;
	if ( pItem != NULL )
	{
		dwTypeID = pItem->GetTypeID () ;
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
	pItem = g_objHero.m_objPlayerDummy.GetEquipment( ITEMPOSITION_RINGR ) ;
	if ( pItem != NULL )
	{
		dwTypeID = pItem->GetTypeID () ;
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
	pItem = g_objHero.m_objPlayerDummy.GetEquipment( ITEMPOSITION_HELMET ) ;
	if ( pItem != NULL )
	{
		dwTypeID = pItem->GetTypeID () ;
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

	m_ImgLHand.RemoveImage() ;
	pItem = g_objHero.m_objPlayerDummy.GetEquipment( ITEMPOSITION_WEAPONL ) ;
	if ( pItem != NULL )
	{
		dwTypeID = pItem->GetTypeID () ;
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

	m_ImgTreasure.RemoveImage() ;
	pItem = g_objHero.m_objPlayerDummy.GetEquipment( ITEMPOSITION_TREASURE ) ;
	if ( pItem != NULL )
	{
		dwTypeID = pItem->GetTypeID () ;
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
	pItem = g_objHero.m_objPlayerDummy.GetEquipment( ITEMPOSITION_MOUNT ) ;
	if ( pItem != NULL )
	{
		dwTypeID = pItem->GetTypeID () ;
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

	m_ImgNeck.RemoveImage() ;
	pItem = g_objHero.m_objPlayerDummy.GetEquipment( ITEMPOSITION_NECKLACE ) ;
	if ( pItem != NULL )
	{
		dwTypeID = pItem->GetTypeID () ;
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
	pItem = g_objHero.m_objPlayerDummy.GetEquipment( ITEMPOSITION_WEAPONR ) ;
	if ( pItem != NULL )
	{
		dwTypeID = pItem->GetTypeID () ;
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

	if ( !m_bShow )
		CDlgNpcEquip::ShowWindow ( SW_SHOW ) ;
}

int CDlgNpcEquip::GetMouseMovePosition()
{
	if ( m_ImgHead.m_bMouseMove )
		return ITEMPOSITION_HELMET ;
	if ( m_ImgNeck.m_bMouseMove )
		return ITEMPOSITION_NECKLACE ;
	if ( m_ImgTreasure.m_bMouseMove )
		return ITEMPOSITION_TREASURE ;
	if ( m_ImgGarment.m_bMouseMove )
		return ITEMPOSITION_MOUNT ;
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
	return -1 ;
}

void CDlgNpcEquip::OnNpcequipClose()
{
	CDlgNpcEquip::ShowWindow ( SW_HIDE ) ;
}

void CDlgNpcEquip::OnNpcequipCloseb()
{
	CDlgNpcEquip::ShowWindow ( SW_HIDE ) ;
}

void CDlgNpcEquip::OnBtnHelp()
{
	CDlgNpcEquip::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_NPCEQUIP ) ;
}
