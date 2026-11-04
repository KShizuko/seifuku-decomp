#include "stdafx.h"
#include "MyShell.h"
#include "DlgFuse.h"

#include "Hero.h"
#include "GameMsg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef _SOUND_DISABLE
	#define DXPlaySound
#endif

CDlgFuse::CDlgFuse(CWnd* pParent )
	: CDialog(CDlgFuse::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_fFrameStep = 0.0f ;
	m_nFrameMode = 0 ;
	m_nPntX = 0 ;
	m_FuseInfo.btType = ( BYTE ) ( DWORD ) pParent ;
	m_FuseInfo.setItem.clear () ;
	m_btImage = 0 ;
	m_idItem = 0 ;
}


void CDlgFuse::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FUSE_STA_ODDS, m_StaOdds);
	DDX_Control(pDX, IDC_FUSE_STA_LEVEL, m_StaLevel);
	DDX_Control(pDX, IDC_FUSE_GRI_LAY, m_GriLay);
	DDX_Control(pDX, IDC_FUSE_BTN_FUSE, m_BtnFuse);
	DDX_Control(pDX, IDC_FUSE_CHK_OK, m_ChkOk);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
	DDX_Control(pDX, IDC_FUSE_BTN_CLOSEC, m_BtnCloseC);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_BtnClose);
}


BEGIN_MESSAGE_MAP(CDlgFuse, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_FUSE_BTN_CLOSEC, OnBtnClose)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBtnCloseB)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_FUSE_CHK_OK, OnChkOk)
	ON_BN_CLICKED(IDC_FUSE_BTN_FUSE, OnBtnFuse)
END_MESSAGE_MAP()

LRESULT CDlgFuse::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_DESTROY )
	{
		::DeleteObject ( m_Hbr ) ;
	}
	else if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
		if ( wParam )
		{
			CRect rect ;
			CDlgFuse::GetWindowRect ( rect ) ;
			if ( m_nFrameMode == 0 )
			{
				DXPlaySound ( "Sound/DlgOpen.wav" ) ;
				m_dwLastShowTime = 0 ;
				m_nFrameMode = 1 ;
				CDlgFuse::MoveWindow ( -rect.Width(), m_Pnt.y, rect.Width(), rect.Height(), true ) ;
			}
			else if ( m_nFrameMode == 2 )
			{
				CDlgFuse::MoveWindow ( 0, m_Pnt.y, rect.Width(), rect.Height(), true ) ;
				m_nFrameMode = 0 ;
			}
		}
		else
		{
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
			return true ;
		}
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

HBRUSH CDlgFuse::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgFuse::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgFuse::GetWindowRect ( rect ) ;
	m_fFrameStep = ( float ) rect.Width () / 20 ;

	m_StaOdds.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_StaOdds.SetFontColor ( 0xFFFFFF00 ) ;
	m_StaLevel.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_StaLevel.SetFontColor ( 0xFFFFFF00 ) ;

	m_BtnFuse.Init ( rect.left, rect.top, "Button540" ) ;
	m_BtnHelp.Init ( rect.left, rect.top, "Button4" ) ;
	m_BtnCloseC.Init ( rect.left, rect.top, "Button5" ) ;
	m_BtnClose.Init ( rect.left, rect.top, "Button3" ) ;
	m_ChkOk.Init ( rect.left, rect.top, "Button541", 2 ) ;

	m_GriLay.MoveWindow ( 22, 88, 214, 172, true ) ;
	m_GriLay.Init ( rect.left, rect.top, 4, 5, DLG_FUSE ) ;

	return TRUE;
}

void CDlgFuse::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_nPntX = x ;
	m_Pnt.y = y ;
}

void CDlgFuse::Show()
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
					CDlgFuse::WindowProc ( WM_SHOWWINDOW, true, NULL ) ;
				}
			}
		}

		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														  "Dialog54",
														  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			ShowAni->Show( 0,
						   m_Pnt.x ,
						   m_Pnt.y  );
		}
		else
			return ;

		m_StaOdds.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_StaLevel.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_BtnFuse.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ChkOk.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnCloseC.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_BtnClose.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_GriLay.Show ( m_Pnt.x, m_Pnt.y ) ;
	}
}

void CDlgFuse::OnBtnClose()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;
	CDlgFuse::ShowWindow ( SW_HIDE ) ;
	m_FuseInfo.setItem.clear () ;
	m_GriLay.RemoveIcon ( 0, 0, true ) ;
	m_StaLevel.SetWindowText ( NULL ) ;
	m_StaOdds.SetWindowText ( NULL ) ;
	m_ChkOk.SetCheckValue ( 0 ) ;
	CDlgFuse::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FUSE_CLOSE ) ;
}

void CDlgFuse::OnBtnHelp()
{
	CDlgFuse::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_FUSE ) ;
}

void CDlgFuse::OnChkOk()
{
	if ( m_FuseInfo.setItem.size () != 0 )
	{
		if ( m_ChkOk.GetCheckValue () == 1 )
		{
			int nSum = 0 ;
			int nMaxLevel = 0 ;
			for ( list<OBJID>::iterator it = m_FuseInfo.setItem.begin (); it != m_FuseInfo.setItem.end (); ++it )
			{
				CItem* pItem = g_objHero.GetItem ( *it, true ) ;
				if ( pItem )
				{
					nSum += pItem->GetFuseInfo () ;
					if ( ( int ) pItem->GetAddition () > nMaxLevel )
						nMaxLevel = pItem->GetAddition () ;
				}
			}
			nMaxLevel = 1 ;
			float fOdds = ( float ) __min ( 10000.0, ( double ) nSum / pow ( 3.0, ( double ) ( nMaxLevel - 1 ) ) ) ;
			fOdds = fOdds / 100 ;
			char strText[32] ;
			sprintf ( strText, GameDataSetQuery()->GetStr ( 10453 ), fOdds ) ;
			m_StaOdds.SetWindowText ( strText ) ;
			sprintf ( strText, GameDataSetQuery()->GetStr ( 10454 ), nMaxLevel ) ;
			m_StaLevel.SetWindowText ( strText ) ;
		}
		else
		{
			m_StaOdds.SetWindowText ( NULL ) ;
			m_StaLevel.SetWindowText ( NULL ) ;
		}
	}
}

void CDlgFuse::OnBtnFuse()
{
	if ( m_ChkOk.GetCheckValue () != 1 )
	{
		g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10452 ) ) ;
		return ;
	}
	if ( m_FuseInfo.setItem.size () > 0 )
	{
		FUSE_INFO infoFuse ;
		infoFuse.btType = m_FuseInfo.btType ;
		for ( list<OBJID>::iterator it = m_FuseInfo.setItem.begin (); it != m_FuseInfo.setItem.end (); ++it )
			infoFuse.setItem.push_back ( *it ) ;
		g_objHero.FuseItem ( m_idItem, infoFuse ) ;
	}
	CDlgFuse::OnBtnClose () ;
}

void CDlgFuse::Open(BYTE btImage, OBJID idItem)
{
	m_btImage = btImage ;
	m_idItem = idItem ;
	CDlgFuse::ShowWindow ( SW_SHOW ) ;
}

void CDlgFuse::AddItem(OBJID idItem)
{
	if ( m_ChkOk.GetCheckValue () == 1 )
	{
		g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10451 ) ) ;
		return ;
	}
	CItem* pItem = g_objHero.GetItem ( idItem, true ) ;
	if ( pItem == NULL )
		return ;
	if ( m_FuseInfo.setItem.size () >= FUSE_ITEM_AMOUNT )
	{
		g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10447 ) ) ;
		return ;
	}
	if ( pItem->IsAdditionStone () )
	{
		g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10464 ) ) ;
		return ;
	}
	if ( pItem->GetAddition () > 0 )
	{
		g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10456 ) ) ;
		return ;
	}
	for ( list<OBJID>::iterator it = m_FuseInfo.setItem.begin (); it != m_FuseInfo.setItem.end (); ++it )
	{
		if ( *it == idItem )
		{
			g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10448 ) ) ;
			return ;
		}
	}
	if ( pItem->GetSort () >= 1 && pItem->GetSort () <= 8 )
	{
		OBJID ItemTypeID = pItem->GetTypeID () ;
		if ( ItemTypeID % 10 < 5 )
		{
			if ( pItem->GetAddition () == 0 && pItem->GetAmount () >= pItem->GetAmountLimit () / 2 )
			{
				g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10450 ) ) ;
				return ;
			}
		}
	}
	m_FuseInfo.setItem.push_back ( idItem ) ;
	CDlgFuse::SetItem () ;
}

void CDlgFuse::DelItem(OBJID idItem)
{
	for ( list<OBJID>::iterator it = m_FuseInfo.setItem.begin (); it != m_FuseInfo.setItem.end (); ++it )
	{
		if ( *it == idItem )
		{
			m_FuseInfo.setItem.erase ( it ) ;
			CDlgFuse::SetItem () ;
			return ;
		}
	}
}

void CDlgFuse::SetItem()
{
	m_GriLay.RemoveIcon ( 0, 0, true ) ;
	int nIndex = 0 ;
	list<OBJID>::iterator it = m_FuseInfo.setItem.begin () ;
	while ( it != m_FuseInfo.setItem.end () )
	{
		CItem* pItem = g_objHero.GetItem ( *it, true ) ;
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
			if ( ItemTypeID >= 1088000 && ItemTypeID <= 1088002 )
				btActType = 6 ;
			if ( ItemTypeID == 1100003 || ItemTypeID == 1100006 || ItemTypeID == 1100009 )
				btActType = 6 ;
			int nRealIndex = m_GriLay.InsertIcon ( 1, 1, ItemTypeID, nIndex, ICON_TYPE_ITEM, btActType ) ;
			m_GriLay.SetIconRealID ( nRealIndex, pItem->GetID () ) ;
			nIndex++ ;
			++it ;
		}
		else
		{
			m_FuseInfo.setItem.erase ( it ) ;
			m_GriLay.RemoveIcon ( 0, 0, true ) ;
			it = m_FuseInfo.setItem.begin () ;
		}
	}
}

void CDlgFuse::OnBtnCloseB()
{
	CDlgFuse::OnBtnClose () ;
}
