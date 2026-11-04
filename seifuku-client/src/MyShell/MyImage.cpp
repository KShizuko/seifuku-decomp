#include "stdafx.h"
#include "MyShell.h"
#include "MyImage.h"
#include "GameDataSetProvisional.h"
#include "3DRoleData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMyImage::CMyImage()
{
	m_bImageLoad = false ;
	m_bMouseMove = false ;
	m_uFrame = 0 ;
	m_ShowRect.left = 0 ;
	m_ShowRect.top = 0  ;
	m_nImageTypeID = 0 ;
	m_btActType = 0 ;
	m_dwActTime = 0 ;
	m_bImgZoom = false ;
	m_nCurMoney = 0 ;
	m_btMilNum = 0 ;
	m_btThoNum = 0 ;
	m_btHunNum = 0 ;
	m_nWeaponLevel = 0 ;
	m_btGoldStar = 0 ;
	m_btSillerStar = 0 ;
	m_btCopperStar = 0 ;
	m_bSetCursor = false ;
}

CMyImage::~CMyImage()
{
}


BEGIN_MESSAGE_MAP(CMyImage, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL CMyImage::Init(int xPos, int yPos, char *ImageID, BYTE btItemType, int nItemPos )
{
	g_objFontScale.Adjust ( this, GetDlgCtrlID () ) ;
	CRect rect ;
	CMyImage::GetWindowRect ( rect ) ;
	m_IntPoint.x = rect.left - xPos ;
	m_IntPoint.y = rect.top - yPos ;
	if ( g_objFontScale.IsScale () )
	{
		m_IntPoint.x = (int) ( m_IntPoint.x * g_objFontScale.GetScaleX () + 0.5f ) ;
		m_IntPoint.y = (int) ( m_IntPoint.y * g_objFontScale.GetScaleY () + 0.5f ) ;
		int nW = (int) ( rect.Width () * g_objFontScale.GetScaleX () + 0.5f ) ;
		int nH = (int) ( rect.Height () * g_objFontScale.GetScaleY () + 0.5f ) ;
		CMyImage::MoveWindow ( m_IntPoint.x, m_IntPoint.y, nW, nH, FALSE ) ;
		CMyImage::GetWindowRect ( rect ) ;
	}
	m_nWidth = rect.Width() ;
	m_nHeight = rect.Height() ;
	m_btItemType = btItemType ;

	switch ( btItemType )
	{
	case NORMAL_IMAGE:
		strcpy ( m_strAni, g_strControlAni ) ;
		m_nItemPos = nItemPos ;
		break ;
	case ITEM_IMAGE:
	case ITEM12_IMAGE:
		strcpy ( m_strAni, "ani/ItemMinIcon.Ani" ) ;
		m_nItemPos = nItemPos ;
		break ;
	case FACE_IMAGE:
		strcpy ( m_strAni, "ani/PlayerFace.ani" ) ;
		m_nItemPos = nItemPos ;
		break ;
	case SKILL_IMAGE:
	case XP_IMAGE:
		strcpy( m_strAni, g_strMagicAni ) ;
		break ;
	case ITEM5_IMAGE:
	case ITEM6_IMAGE:
	case ITEM7_IMAGE:
	case BOOTHITEM_IMAGE:
		strcpy(m_strAni, "ani/ItemMinIcon.Ani");
		break;
	case CONTROL_IMAGE:
		strcpy ( m_strAni, g_strControlAni ) ;
		break ;
	case SERVER_IMAGE:
		strcpy ( m_strAni, "ani/Server.ani" ) ;
		break ;
	default:
		strcpy ( m_strAni, "ani/ItemMinIcon.Ani" ) ;
		break ;
	}

	if ( ImageID != NULL )
	{
		m_bImageLoad = true ;
		CMySize Size = { 0, 0 } ;
		if ( m_btItemType == FACE_IMAGE )
		{
			m_dwImageID = atoi ( ImageID ) ;
			IAni* pSizeAni = RoleDataQuery()->GetPlayerFaceIcon ( m_dwImageID, EXIGENCE_IMMEDIATE ) ;
			if ( pSizeAni != NULL )
				Size = pSizeAni->GetSize ( m_uFrame ) ;
			else
				return false;
			m_ShowRect.right = Size.iWidth ;
			m_ShowRect.bottom = Size.iHeight ;
		}
		else
		{
			strcpy ( m_strImageID, ImageID ) ;
			IAni* pSizeAni = GameDataSetQuery()->GetDataAni ( ( char * )m_strAni, ImageID, EXIGENCE_IMMEDIATE ) ;
			if ( pSizeAni != NULL )
				Size = pSizeAni->GetSize ( m_uFrame ) ;
			else
				return false;
			m_ShowRect.right = Size.iWidth ;
			m_ShowRect.bottom = Size.iHeight ;
		}
	}
	return true ;

}

BOOL CMyImage::InsertImage(char *ImageID, int nImageTypeID, BOOL bImgZoom )
{
	m_bImgZoom = bImgZoom ;
	m_bImageLoad = true ;
	CMySize Size = { 0, 0 } ;

	if ( m_btItemType == FACE_IMAGE )
	{
		m_dwImageID = atoi ( ImageID ) ;
		IAni* pSizeAni = RoleDataQuery()->GetPlayerFaceIcon ( m_dwImageID, EXIGENCE_IMMEDIATE ) ;
		if ( pSizeAni != NULL )
			Size = pSizeAni->GetSize ( m_uFrame ) ;
		else
			return false ;
		m_ShowRect.right = Size.iWidth ;
		m_ShowRect.bottom = Size.iHeight ;
	}
	else
	{
		strcpy ( m_strImageID, ImageID ) ;
		IAni* pSizeAni = GameDataSetQuery()->GetDataAni ( ( char * )m_strAni, ImageID, EXIGENCE_IMMEDIATE ) ;
		if ( pSizeAni != NULL )
			Size = pSizeAni->GetSize ( m_uFrame ) ;
		else
			return false ;
		if ( m_btItemType == SKILL_IMAGE || m_btItemType == XP_IMAGE )
			m_nImageTypeID = nImageTypeID ;
		m_ShowRect.right = Size.iWidth ;
		m_ShowRect.bottom = Size.iHeight ;
	}
	return true ;
}

void CMyImage::RemoveImage()
{
	m_bImageLoad = false ;
}

void CMyImage::Show(int x, int y)
{
	m_Point = CPoint ( x + m_IntPoint.x, y + m_IntPoint.y ) ;

	if ( m_bImageLoad )
	{
		if ( m_btActType != 0 )
		{
			char* strEffect ;
			switch ( m_btActType )
			{
			case 1: strEffect = "FireLight" ; break ;
			case 2: strEffect = "RedLight" ; break ;
			case 3: strEffect = "BlueLight" ; break ;
			case 4: strEffect = "RoyalBlueLight" ; break ;
			case 5: strEffect = "GoldenLight" ; break ;
			case 6: strEffect = "YellowLight" ; break ;
			default:
				return ;
			}
			IAni* pEffect = GameDataSetQuery()->GetDataAni ( "ani/effect.ani", strEffect, EXIGENCE_IMMEDIATE ) ;
			if ( pEffect == NULL )
				return ;
			UINT uFrame = 0 ;
			if ( m_dwActTime == 0 )
				m_dwActTime = ::TimeGet () ;
			else
				uFrame = ( ( ::TimeGet () - m_dwActTime ) / 100 ) % pEffect->GetFrameAmount () ;
			RECT rect = { 0, 0, 64, 64 } ;
			pEffect->ShowEx ( uFrame, m_Point.x, m_Point.y, &rect, m_nWidth, m_nHeight ) ;
		}

		if ( m_btItemType == FACE_IMAGE )
		{
			IAni *pAni = RoleDataQuery()->GetPlayerFaceIcon ( m_dwImageID, EXIGENCE_IMMEDIATE ) ;
			if ( pAni != NULL )
			{
				pAni->ShowEx ( m_uFrame,
							   m_Point.x,
							   m_Point.y,
							   m_ShowRect,
							   m_nWidth,
							   m_nHeight ) ;
			}
		}
		else
		{
			IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) m_strAni,
														  m_strImageID,
														  EXIGENCE_IMMEDIATE ) ;
			if ( m_btItemType == CONTROL_IMAGE )
				ShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) g_strControlAni, m_strImageID, EXIGENCE_IMMEDIATE ) ;
			else if ( m_btItemType == SKILL_IMAGE || m_btItemType == XP_IMAGE )
				ShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) g_strMagicAni, m_strImageID, EXIGENCE_IMMEDIATE ) ;

			if ( ShowAni != NULL )
			{
				if ( m_btItemType == CONTROL_IMAGE )
				{
					ShowAni->Show ( m_uFrame,
									m_Point.x,
									m_Point.y ) ;
				}
				else if ( m_btItemType == SKILL_IMAGE || m_btItemType == XP_IMAGE )
				{
					RECT rect = { 0, 0, 50, 50 } ;
					ShowAni->ShowEx( m_uFrame,
								     m_Point.x,
									 m_Point.y,
									 &rect,
									 m_nWidth,
									 m_nHeight ) ;
				}
				else
				{
					ShowAni->ShowEx ( m_uFrame,
									  m_Point.x,
									  m_Point.y,
									  m_ShowRect,
									  m_nWidth,
									  m_nHeight ) ;
				}
			}
		}
	}

	if ( m_bImgZoom )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni, "Image0", EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			RECT rect = { 0, 0, 64, 64 } ;
			ShowAni->ShowEx( 0,
							m_Point.x,
							m_Point.y,
							&rect,
							m_nWidth,
							m_nHeight ) ;
		}
	}

	if ( m_bMouseMove )
		this->MouseMoveText () ;
}


void CMyImage::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}

BOOL CMyImage::GetImageLoadState()
{
	return m_bImageLoad ;
}

char* CMyImage::GetImageID()
{
	return m_strImageID ;
}

void CMyImage::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( m_btItemType == NORMAL_IMAGE && m_nItemPos != -1 && !m_bMouseMove )
	{
		m_bMouseMove = true ;
		CMyImage::GetParent()->PostMessage( WM_MY_MESSAGE, m_nItemPos, 0 ) ;
	}
	switch ( m_btItemType )
	{
	case ITEM_IMAGE:
		if ( m_bImageLoad && !m_bMouseMove )
		{
			m_bMouseMove = true ;
			DWORD lParam = MAKELONG( m_Point.x, m_Point.y ) ;
			CMyImage::GetParent()->GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_POINT, lParam ) ;
			CMyImage::GetParent()->GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_GOODS, 9 ) ;
		}
		break ;
	case FACE_IMAGE:
		if ( m_nItemPos != -1 && !m_bMouseMove )
		{
			m_bMouseMove = true ;
			DWORD lParam = MAKELONG( m_Point.x, m_Point.y ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_POINT, lParam ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_GOODS, 23 ) ;
		}
		break ;
	case SKILL_IMAGE:
	case XP_IMAGE:
		if ( m_bImageLoad && !m_bMouseMove )
		{
			m_bMouseMove = true ;
			DWORD lParam = MAKELONG( m_Point.x, m_Point.y ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_POINT, lParam ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_IMAGE, m_nImageTypeID ) ;
		}
		break ;
	case ITEM5_IMAGE:
		if ( m_bImageLoad && !m_bMouseMove )
		{
			m_bMouseMove = true ;
			DWORD lParam = MAKELONG( m_Point.x, m_Point.y ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_POINT, lParam ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_GOODS, 22 ) ;
		}
		break ;
	case ITEM13_IMAGE:
		if ( m_bImageLoad && !m_bMouseMove )
		{
			m_bMouseMove = true ;
			DWORD lParam = MAKELONG( m_Point.x, m_Point.y ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_POINT, lParam ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_GOODS, DLG_ENCHANT ) ;
		}
		break ;
	case ITEM6_IMAGE:
		if ( m_bImageLoad && !m_bMouseMove )
		{
			m_bMouseMove = true ;
			DWORD lParam = MAKELONG( m_Point.x, m_Point.y ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_POINT, lParam ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_GOODS, 24 ) ;
		}
		break ;
	case ITEM7_IMAGE:
		if ( m_bImageLoad && !m_bMouseMove )
		{
			m_bMouseMove = true ;
			DWORD lParam = MAKELONG( m_Point.x, m_Point.y ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_POINT, lParam ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_GOODS, 28 ) ;
		}
		break ;
	case BOOTHITEM_IMAGE:
		if ( m_bImageLoad && !m_bMouseMove )
		{
			m_bMouseMove = true ;
			DWORD lParam = MAKELONG( m_Point.x, m_Point.y ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_POINT, lParam ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_GOODS, 29 ) ;
		}
		break;
	case ITEM12_IMAGE:
		if ( m_bImageLoad && !m_bMouseMove )
		{
			m_bMouseMove = true ;
			DWORD lParam = MAKELONG( m_Point.x, m_Point.y ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_POINT, lParam ) ;
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_GOODS, 36 ) ;
		}
		break;
	default:
		break ;
	}
	CButton::OnMouseMove( nFlags, point ) ;
}

void CMyImage::MouseMoveText()
{
	POINT MousePoint ;
	GetCursorPos ( &MousePoint ) ;
	CRect rect ;
	CMyImage::GetWindowRect ( rect ) ;
	if ( !rect.PtInRect ( MousePoint ) )
	{
		m_bMouseMove = false ;
		if ( m_btItemType == ITEM_IMAGE )
		{
			CMyImage::GetParent()->GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_HIDE ) ;
		}
		else if ( m_btItemType == SKILL_IMAGE || m_btItemType == XP_IMAGE || m_btItemType == ITEM5_IMAGE ||
				  m_btItemType == ITEM6_IMAGE || m_btItemType == ITEM7_IMAGE || m_btItemType == BOOTHITEM_IMAGE ||
				  m_btItemType == FACE_IMAGE || m_btItemType == ITEM12_IMAGE )
		{
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_HIDE ) ;
		}
	}
	else
	{
		if ( m_btItemType == XP_IMAGE || m_btItemType == SKILL_IMAGE )
			CMyImage::GetParent()->GetParent()->PostMessage( WM_MY_MESSAGE, ON_MSGDLG_FACE, m_nImageTypeID ) ;
	}
}

void CMyImage::ShowFocus()
{
	IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char* )g_strControlAni,
												  "ImageFocus",
												  EXIGENCE_IMMEDIATE ) ;
	if ( ShowAni != NULL )
		ShowAni->ShowEx ( 0,
						  m_Point.x,
						  m_Point.y,
						  CRect ( 0, 0, 100, 100 ),
						  m_nWidth,
						  m_nHeight ) ;
}

void CMyImage::ShowBk(const char *strBkID)
{
	if ( strBkID != NULL )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char* )g_strControlAni,
													  strBkID,
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0,
							m_Point.x,
							m_Point.y ) ;
	}
}

void CMyImage::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ( m_bImgZoom )
		return ;
	if ( m_btItemType == BOOTHITEM_IMAGE )
		return ;
	if ( ( m_btItemType == SKILL_IMAGE || m_btItemType == XP_IMAGE ) && m_nImageTypeID != 0 )
	{
		LPARAM lParam ;
		CPoint MousePnt ;
		CMyImage::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_ID, m_nImageTypeID ) ;
		GetCursorPos ( &MousePnt ) ;
		lParam = MAKELONG ( MousePnt.x, MousePnt.y ) ;
		CMyImage::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_PNT, lParam ) ;
		if ( m_btItemType == SKILL_IMAGE && m_bImageLoad )
			CMyImage::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_PICKUP, DLG_MAGIC ) ;
		else if ( m_btItemType == XP_IMAGE && m_bImageLoad )
			CMyImage::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_PICKUP, DLG_XP ) ;
	}
	if ( m_btItemType == SKILL_IMAGE || m_btItemType == XP_IMAGE )
		CMyImage::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_HIDE ) ;
	CButton::OnLButtonDown(nFlags, point);
}

void CMyImage::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMyImage::GetParent()->PostMessage( WM_RBUTTONDOWN, 0, 0 ) ;
	CButton::OnRButtonDown(nFlags, point);
}

void CMyImage::OnLButtonUp(UINT nFlags, CPoint point)
{
	switch ( m_btItemType )
	{
	case SKILL_IMAGE:
		if ( m_nImageTypeID != 0 )
			CMyImage::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP, DLG_MAGIC ) ;
		break ;
	case XP_IMAGE:
		if ( m_nImageTypeID != 0 )
			CMyImage::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP, DLG_XP ) ;
		break ;
	default:
		break ;
	}
	CButton::OnLButtonUp(nFlags, point);
}

void CMyImage::SetActType(BYTE btActType)
{
	m_dwActTime = 0 ;
	m_btActType = btActType ;
}

void CMyImage::SetCurFrame(UINT uFrame)
{
	m_uFrame = uFrame ;
}

void CMyImage::ShowMoney(int x, int y)
{
	if ( m_bMouseMove )
		this->MouseMoveText () ;

	m_Point = CPoint ( x + m_IntPoint.x, y + m_IntPoint.y ) ;
	int nLine = 0 ;
	int nRow = 0 ;
	if ( m_btMilNum > 0 )
	{
		IAni* pShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) m_strAni,
													  "Button435",
													  EXIGENCE_IMMEDIATE ) ;
		if ( pShowAni )
		{
			for ( int i = 0; i < m_btMilNum; i++ )
			{
				pShowAni->Show ( 0,
								 m_Point.x + nRow * 16,
								 m_Point.y + nLine * 8) ;
				nLine++ ;
				if ( nLine * 8 + 16 > m_nHeight )
					break ;
			}
			nRow++ ;
		}
	}

	nLine = 0 ;
	if ( m_btThoNum > 0 )
	{
		IAni* pShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) m_strAni,
													  "Button434",
													  EXIGENCE_IMMEDIATE ) ;
		if ( pShowAni )
		{
			for ( int i = 0; i < m_btThoNum; i++ )
			{
				pShowAni->Show ( 0,
								 m_Point.x + nRow * 16,
								 m_Point.y + nLine * 8) ;
				nLine++ ;
				if ( nLine * 8 + 16 > m_nHeight )
					break ;
			}
			nRow++ ;
		}
	}

	nLine = 0 ;
	if ( m_btHunNum > 0 )
	{
		IAni* pShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) m_strAni,
													  "Button433",
													  EXIGENCE_IMMEDIATE ) ;
		if ( pShowAni )
		{
			for ( int i = 0; i < m_btHunNum; i++ )
			{
				pShowAni->Show ( 0,
								 m_Point.x + nRow * 16,
								 m_Point.y + nLine * 8) ;
				nLine++ ;
				if ( nLine * 8 + 16 > m_nHeight )
					break ;
			}
			nRow++ ;
		}
	}
}

void CMyImage::SetMoney ( int nMoney )
{
	if ( m_nCurMoney != nMoney )
	{
		m_nCurMoney = nMoney ;
		m_btMilNum = nMoney / 10000 ;
		nMoney -= 10000 * m_btMilNum ;
		m_btThoNum = nMoney / 1000 ;
		nMoney -= 1000 * m_btThoNum ;
		m_btHunNum = nMoney / 100 ;
	}
}

void CMyImage::ShowWeaponLevel(int x, int y)
{
	m_Point = CPoint ( x + m_IntPoint.x, y + m_IntPoint.y ) ;
	if ( m_nWeaponLevel == 0 )
		return ;

	if ( m_btGoldStar == 1 )
	{
		IAni* pShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Weaponlevel20",
													  EXIGENCE_IMMEDIATE ) ;
		if ( pShowAni != NULL )
		{
			pShowAni->Show ( 0,
							 m_Point.x + 38,
							 m_Point.y + 4 ) ;
			return ;
		}
	}

	if ( m_btSillerStar > 0 )
	{
		IAni* pShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Weaponlevel5",
													  EXIGENCE_IMMEDIATE ) ;
		for ( int i = 0; i < m_btSillerStar; i++ )
		{
			pShowAni->Show ( 0,
							 m_Point.x + i * 26,
							 m_Point.y + 4 ) ;
		}
	}

	if ( m_btCopperStar > 0 )
	{
		IAni* pShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Weaponlevel1",
													  EXIGENCE_IMMEDIATE ) ;
		for ( int i = 0; i < m_btCopperStar; i++ )
		{
			if ( i < 2 )
				pShowAni->Show ( 0,
								 m_Point.x + m_btSillerStar * 24 + i * 14,
								 m_Point.y ) ;
			else
				pShowAni->Show ( 0,
								 m_Point.x + m_btSillerStar * 24 + ( i - 2 ) * 14,
								 m_Point.y + 16 ) ;
		}
	}
}

void CMyImage::SetWeaponLevel(int nLevel)
{
	m_nWeaponLevel = nLevel ;
	if ( nLevel == 20 )
	{
		m_btGoldStar = 1 ;
		m_btSillerStar = 0 ;
		m_btCopperStar = 0 ;
	}
	else
	{
		m_btGoldStar = 0 ;
		m_btSillerStar = nLevel / 5 ;
		m_btCopperStar = nLevel % 5 ;
	}
}

LRESULT CMyImage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_ERASEBKGND )
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

	return CButton::WindowProc(message, wParam, lParam);
}

BOOL CMyImage::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if ( m_bSetCursor )
		( ( CMyShellApp* ) AfxGetApp () )->SetOnButton ( 1 ) ;
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}
