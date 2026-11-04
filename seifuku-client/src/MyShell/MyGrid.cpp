#include "stdafx.h"
#include "MyShell.h"
#include "MyGrid.h"

#include "Hero.h"
#include "Shop.h"
#include "3DGameMap.h"
#include "GameDataSetProvisional.h"
#include "3DRoleData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
const int CHAR_WIDTH = 6 ;

CMyGrid::CMyGrid()
{
	m_GriPar.m_bPickUp = false ;
	m_GriPar.m_bMouseMove = false ;
	m_GriPar.m_nGridAbleNum = 0 ;
	for ( int i = 0; i < 256; i++ )
		m_GriPar.m_bGridAble[i] = false ;
	m_GriPar.m_bEnable = true ;
	m_GriPar.m_bPickEnable = true ;
	m_GriPar.m_ClientRect = CRect ( 0, 0, 0, 0 ) ;
}

CMyGrid::~CMyGrid()
{
}


BEGIN_MESSAGE_MAP(CMyGrid, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CMyGrid::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}

BOOL CMyGrid::Init(int xPos, int yPos, BYTE btLine, BYTE btRow, BYTE btOwnerID)
{
	g_objFontScale.Adjust ( this, GetDlgCtrlID () ) ;
	memset(&m_GriPar, 0L, sizeof(m_GriPar));

	CRect rect ;
	CMyGrid::GetWindowRect ( rect ) ;
	m_GriPar.m_IntPoint.x = rect.left - xPos ;
	m_GriPar.m_IntPoint.y = rect.top - yPos ;
	m_GriPar.m_ClientRect.right = ( rect.right - rect.left ) ;
	m_GriPar.m_ClientRect.bottom = ( rect.bottom - rect.top ) ;

	m_GriPar.m_btGridLine = btLine ;
	m_GriPar.m_btGridRow = btRow ;
	m_GriPar.m_btOwnerID = btOwnerID ;
	m_GriPar.m_btGridWidth = ( rect.Width () - (btRow - 1) * 1) / btRow ;
	m_GriPar.m_btGridHeight = ( rect.Height () - (btLine - 1) * 1 ) / btLine ;


	for ( int i = 0; i < 256; i++ )
	{
		m_GriPar.m_bGridAble[i] = false ;
		m_GriPar.m_IconIndex[i] = 0;
		m_GriPar.m_IconTypeID[i]= 0;
	}

	m_GriPar.m_bEnable = true;
	m_GriPar.m_bPickEnable = true;
	return true ;
}

void CMyGrid::Show(int x, int y)
{
	m_GriPar.m_Point = CPoint ( m_GriPar.m_IntPoint.x + x, m_GriPar.m_IntPoint.y + y ) ;
	int index ;
	int nRowLeft ;
	int nLineLeft ;

	for ( int i = 1; i <= m_GriPar.m_btGridLine; i++ )
		for ( int j = 1; j <= m_GriPar.m_btGridRow; j++ )
		{
			if ( !GetGridAble ( CPoint ( j, i ) ) )
				continue ;

			index = ( i - 1 ) * m_GriPar.m_btGridRow + j ;
			nRowLeft = ( j - 1 ) * ( m_GriPar.m_btGridWidth + 1 ) ;
			nLineLeft = ( i - 1 ) * ( m_GriPar.m_btGridHeight + 1 ) ;

			switch ( m_GriPar.m_btIconType[index - 1] )
			{
			case ICON_TYPE_ITEM:
				{
					if ( m_GriPar.m_btIconActType[index - 1] != 0 )
					{
						char* strEffect ;
						switch ( m_GriPar.m_btIconActType[index - 1] )
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
						if ( m_GriPar.m_dwIconActTime[index - 1] == 0 )
							m_GriPar.m_dwIconActTime[index - 1] = ::TimeGet () ;
						else
							uFrame = ( ( ::TimeGet () - m_GriPar.m_dwIconActTime[index - 1] ) / 100 ) % pEffect->GetFrameAmount () ;
						RECT rect = { 0, 0, 64, 64 } ;
						pEffect->ShowEx ( uFrame,
										  m_GriPar.m_Point.x + nRowLeft,
										  m_GriPar.m_Point.y + nLineLeft,
										  &rect,
										  m_GriPar.m_btGridWidth,
										  m_GriPar.m_btGridHeight ) ;
					}
					IAni* ShowAni = RoleDataQuery()->GetItemMinIcon ( m_GriPar.m_IconTypeID[index - 1], EXIGENCE_IMMEDIATE ) ;
					if ( ShowAni != NULL )
					{
						RECT rect = { 0, 0, 64, 64 } ;
						ShowAni->ShowEx ( 0,
										  m_GriPar.m_Point.x + nRowLeft,
										  m_GriPar.m_Point.y + nLineLeft,
										  &rect,
										  m_GriPar.m_btGridWidth,
										  m_GriPar.m_btGridHeight ) ;
						if ( m_GriPar.m_btOwnerID == DLG_MAIN )
						{
							char strNum[32] ;
							sprintf ( strNum, "%d", m_GriPar.m_IconNum[j - 1] ) ;
							CMyBitmap::ShowString ( m_GriPar.m_Point.x + nRowLeft,
													m_GriPar.m_Point.y + nLineLeft,
													0xffffff,
													strNum ) ;
						}
					}
				}
				break ;
			case ICON_TYPE_ACTION:
				{
					char strName[32] ;
					sprintf( strName, "ButtonA%d", m_GriPar.m_IconTypeID[index - 1] ) ;
					IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
																  strName,
																  EXIGENCE_IMMEDIATE ) ;
					if ( ShowAni != NULL )
					{
						ShowAni->Show ( 0,
										m_GriPar.m_Point.x + nRowLeft + 3,
										m_GriPar.m_Point.y + nLineLeft + 5 ) ;
					}
				}
				break ;
			case ICON_TYPE_SKILL:
				{
					char strName[32] ;
					sprintf( strName, "MagicSkillType%d", m_GriPar.m_IconTypeID[index - 1] ) ;
					IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strMagicAni,
																  strName,
																  EXIGENCE_IMMEDIATE ) ;
					RECT rect = { 0, 0, GRID_ICONMOVE_WIDTH, GRID_ICONMOVE_HEIGHT } ;
					if ( ShowAni != NULL )
					{
						ShowAni->ShowEx( 0,
										 m_GriPar.m_Point.x + nRowLeft + 2,
									     m_GriPar.m_Point.y + nLineLeft + 4,
										 &rect,
										 m_GriPar.m_btGridWidth - 3,
										 m_GriPar.m_btGridHeight - 3 ) ;
					}
				}
				break ;
			case ICON_TYPE_XP:
				{
					char strName[32] ;
					sprintf( strName, "XpSkillType%d", m_GriPar.m_IconTypeID[index - 1] ) ;
					IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strMagicAni,
																  strName,
																  EXIGENCE_IMMEDIATE ) ;
					RECT rect = { 0, 0, GRID_ICONMOVE_WIDTH, GRID_ICONMOVE_HEIGHT } ;
					if ( ShowAni != NULL )
					{
						ShowAni->ShowEx( 0,
										 m_GriPar.m_Point.x + nRowLeft + 2,
									     m_GriPar.m_Point.y + nLineLeft + 4,
										 &rect,
										 m_GriPar.m_btGridWidth - 3,
										 m_GriPar.m_btGridHeight - 3 ) ;
					}
				}
				break ;
			default:
				break ;
			}
		}

	if ( m_GriPar.m_bMouseMove )
		MouseMoveTest ( ) ;
}

CPoint CMyGrid::GetLineAndRow(CPoint MousePoint)
{
	CPoint LandR ;
	for ( int i = 1; i <= m_GriPar.m_btGridLine; i++ )
	{
		if ( MousePoint.y > i * m_GriPar.m_btGridHeight + (i - 1) * 1 )
			continue ;
		else
		{
			LandR.y = i ;
			for ( int j = 1; j <= m_GriPar.m_btGridRow; j++ )
			{
				if ( MousePoint.x > j * m_GriPar.m_btGridWidth +( j -1) * 1)
					continue ;
				else
					LandR.x = j ;
				return LandR ;
			}
		}
	}
	return LandR ;
}

void CMyGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ( !m_GriPar.m_bEnable )
		return ;
	CPoint LandR ;
	LandR = GetLineAndRow ( point ) ;
	BOOL IsAvailab = GetGridAble ( LandR ) ;
	if ( IsAvailab && m_GriPar.m_bPickEnable )
	{
		if ( m_GriPar.m_bPickUp )
		{
			if ( m_GriPar.m_btIconType [GetGridIndex ( LandR ) - 1] == ICON_TYPE_ITEM )
			{
				m_GriPar.m_PickUpIconTypeID = m_GriPar.m_IconTypeID[GetGridIndex ( LandR ) - 1] ;
				m_GriPar.m_dwPickUpIconIndex = m_GriPar.m_IconIndex[GetGridIndex ( LandR ) - 1] ;
				m_GriPar.m_btPickUpIconType = m_GriPar.m_btIconType[GetGridIndex ( LandR )- 1] ;
				m_GriPar.m_btPickUpIconActType = m_GriPar.m_btIconActType[GetGridIndex ( LandR )- 1] ;
				m_GriPar.m_InsetLandR = LandR ;
				GetCursorPos( &m_GriPar.m_PickUpMousePoint ) ;
				CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_TRADE,  m_GriPar.m_btOwnerID) ;
			}
			else
			{
				m_GriPar.m_InsetLandR = LandR ;
				m_GriPar.m_bPickUp = false ;
				RemoveIcon ( LandR.y, LandR.x ) ;
				CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  m_GriPar.m_btOwnerID) ;
			}

		}
		else
		{
			if ( m_GriPar.m_btIconType [GetGridIndex ( LandR ) - 1] == ICON_TYPE_ITEM )
			{
				m_GriPar.m_bPickUp = true ;
				RemoveIcon ( LandR.y, LandR.x ) ;
				m_GriPar.m_PickUpIconLandR = LandR ;
				m_GriPar.m_PickUpIconTypeID = m_GriPar.m_IconTypeID[GetGridIndex ( LandR ) - 1] ;
				m_GriPar.m_dwPickUpIconIndex = m_GriPar.m_IconIndex[GetGridIndex ( LandR ) - 1] ;
				m_GriPar.m_btPickUpIconType = m_GriPar.m_btIconType[GetGridIndex ( LandR ) - 1] ;
				m_GriPar.m_btPickUpIconActType = m_GriPar.m_btIconActType[GetGridIndex ( LandR ) - 1] ;
				GetCursorPos( &m_GriPar.m_PickUpMousePoint ) ;
				CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_PICKUP,  m_GriPar.m_btOwnerID) ;
			}
			else
			{
				LPARAM lParam ;
				CPoint MousePnt ;
				m_GriPar.m_btPickUpIconType = m_GriPar.m_btIconType [GetGridIndex ( LandR ) - 1] ;
				CMyGrid::SetGridAble ( LandR, false ) ;
				CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_ID, m_GriPar.m_IconTypeID [GetGridIndex ( LandR ) - 1] ) ;
				GetCursorPos ( &MousePnt ) ;
				lParam = MAKELONG ( MousePnt.x, MousePnt.y ) ;
				CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_PNT, lParam ) ;
				if ( m_GriPar.m_btPickUpIconType == ICON_TYPE_ACTION )
					CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_PICKUP, DLG_ACT ) ;
				else if ( m_GriPar.m_btPickUpIconType == ICON_TYPE_SKILL )
					CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_PICKUP, DLG_MAGIC ) ;
				else if ( m_GriPar.m_btPickUpIconType == ICON_TYPE_XP )
					CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_PICKUP, DLG_XP ) ;
			}
		}
	}
	else if ( !IsAvailab )
	{
		if ( m_GriPar.m_bPickUp )
		{
			m_GriPar.m_InsetLandR = LandR ;
			m_GriPar.m_bPickUp = false ;
			CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP,  m_GriPar.m_btOwnerID) ;
		}
		else
		{

		}
	}

	CButton::OnLButtonDown(nFlags, point);
}

void CMyGrid::OnRButtonDown(UINT nFlags, CPoint point)
{
	if ( !m_GriPar.m_bEnable )
		return ;
	CPoint LandR ;
	LandR = GetLineAndRow ( point ) ;
	BOOL IsAvailab = GetGridAble ( LandR ) ;

	if ( IsAvailab )
	{
		if ( m_GriPar.m_btOwnerID == DLG_MAIN )
		{
			m_GriPar.m_dwPickUpIconIndex = m_GriPar.m_IconIndex [GetGridIndex ( LandR ) - 1] ;
			LPARAM lParam = GetGridIndex( LandR ) | m_GriPar.m_IconTypeID [GetGridIndex (LandR) - 1] <<8 ;
			CMyGrid::GetParent()->GetParent()->PostMessage ( WM_MY_MESSAGE, ON_ICON_QUSE, lParam ) ;
		}
		else if ( m_GriPar.m_btOwnerID == DLG_MPCGOODS )
		{
			m_GriPar.m_dwPickUpIconIndex = m_GriPar.m_IconIndex [GetGridIndex ( LandR ) - 1] ;
			CMyGrid::GetParent()->GetParent()->PostMessage ( WM_MY_MESSAGE, ON_ICON_USE, m_GriPar.m_dwPickUpIconIndex ) ;
		}
		else if ( m_GriPar.m_btOwnerID == DLG_NPCBUY )
		{
			m_GriPar.m_PickUpIconTypeID = m_GriPar.m_IconTypeID [GetGridIndex ( LandR ) - 1] ;
			CMyGrid::GetParent()->GetParent()->PostMessage ( WM_MY_MESSAGE, ON_ICON_BUY, m_GriPar.m_PickUpIconTypeID ) ;
		}
	}

	CButton::OnRButtonDown(nFlags, point);
}

BOOL CMyGrid::GetGridAble(CPoint lrPoint)
{
	int Index = ( lrPoint.y - 1 ) * m_GriPar.m_btGridRow + lrPoint.x;
	if ( Index > 256 || Index <= 0 )
		return false ;
	return m_GriPar.m_bGridAble[Index - 1];
}

int CMyGrid::InsertIcon(int nLine, int nRow, OBJID ItemTypeID, DWORD dwItemIndex, BYTE btItemType, BYTE btActType, BOOL bAuto)
{
	CPoint lrPoint ;
	lrPoint.x = nRow ;
	lrPoint.y = nLine ;
	if ( ! (nLine <= m_GriPar.m_btGridLine && nRow <= m_GriPar.m_btGridRow ) )
		return -1 ;
	if ( bAuto )
	{
		if ( !GetGridAble ( lrPoint ) )
		{
			if ( LoadIcon( lrPoint, ItemTypeID, dwItemIndex, btItemType, btActType) )
			{
				m_GriPar.m_nGridAbleNum++ ;
				return ( lrPoint.y - 1 ) * m_GriPar.m_btGridRow + lrPoint.x - 1;
			}
		}
		else
		{
			for ( int i = 1; i <= m_GriPar.m_btGridLine; i++ )
				for ( int j = 1; j <= m_GriPar.m_btGridRow; j++ )
				{
					if ( !GetGridAble ( CPoint ( j, i ) ) )
					{
						if ( LoadIcon ( CPoint ( j, i ), ItemTypeID, dwItemIndex, btItemType, btActType ) )
						{
							m_GriPar.m_nGridAbleNum++ ;
							return ( i - 1 ) * m_GriPar.m_btGridRow + j - 1; ;
						}
					}
				}
		}
	}
	else
	{
		if ( LoadIcon( lrPoint, ItemTypeID, dwItemIndex, btItemType, btActType ) )
		{
			m_GriPar.m_nGridAbleNum++ ;
			return ( lrPoint.y - 1 ) * m_GriPar.m_btGridRow + lrPoint.x - 1;
		}
	}
	return -1 ;
}

void CMyGrid::SetGridAble(CPoint lrPoint, BOOL bAble)
{
	int Index = ( lrPoint.y - 1 ) * m_GriPar.m_btGridRow + lrPoint.x ;
	m_GriPar.m_bGridAble[Index - 1] = bAble ;
}

BOOL CMyGrid::LoadIcon(CPoint lrPoint, OBJID ItemTypeID, DWORD dwItemIndex, BYTE btItemType, BYTE btActType)
{
	SetGridAble ( lrPoint, true ) ;
	int Index = ( lrPoint.y - 1 ) * m_GriPar.m_btGridRow + lrPoint.x ;
	m_GriPar.m_IconTypeID [Index-1] = ItemTypeID ;
	m_GriPar.m_IconIndex[Index-1] = dwItemIndex ;
	m_GriPar.m_btIconType[Index-1] = btItemType ;
	m_GriPar.m_btIconActType[Index-1] = btActType ;
	if ( btActType != 0 )
		m_GriPar.m_dwIconActTime[Index-1] = 0 ;
	return true ;
}

int CMyGrid::GetGridIndex(CPoint lrPoint)
{
	return ( lrPoint.y - 1 ) * m_GriPar.m_btGridRow + lrPoint.x ;
}

OBJID CMyGrid::GetPickUpTypeID ( )
{
	return m_GriPar.m_PickUpIconTypeID ;
}

int CMyGrid::GetPickUpIndex()
{
	return m_GriPar.m_dwPickUpIconIndex ;
}

DWORD CMyGrid::GetMouseMoveIndex()
{
	return m_GriPar.m_dwMouseMoveIconIndex ;
}

DWORD CMyGrid::GetMouseMoveTypeID()
{
	return m_GriPar.m_dwMouseMoveIconTypeID ;
}

CPoint CMyGrid::GetPickUpMousePnt()
{
	return m_GriPar.m_PickUpMousePoint ;
}

BOOL CMyGrid::GetPickUp()
{
	return m_GriPar.m_bPickUp ;
}

void CMyGrid::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( !m_GriPar.m_ClientRect.PtInRect ( point ) )
		return  ;
	CPoint LandR ;
	LandR = GetLineAndRow ( point ) ;
	BOOL IsAvailab = GetGridAble ( LandR ) ;
	if ( IsAvailab )
	{
		if ( m_GriPar.m_btIconType [ GetGridIndex ( LandR ) - 1 ] == ICON_TYPE_ACTION )
			return ;
		if ( !m_GriPar.m_bMouseMove )
		{
			m_GriPar.m_bMouseMove = true ;
			m_GriPar.m_MouseMoveIconLandR = LandR ;
			int nPosX = m_GriPar.m_Point.x + m_GriPar.m_btGridWidth * ( LandR.x - 1 ) ;
			int nPosY = m_GriPar.m_Point.y + m_GriPar.m_btGridHeight * ( LandR.y - 1 ) ;
			m_GriPar.m_dwMouseMoveIconIndex = m_GriPar.m_IconIndex [GetGridIndex ( LandR ) - 1 ];
			m_GriPar.m_dwMouseMoveIconTypeID = m_GriPar.m_IconTypeID [GetGridIndex ( LandR ) - 1 ];
			LPARAM lParam = MAKELONG( nPosX, nPosY ) ;
			if ( m_GriPar.m_btIconType [ GetGridIndex ( LandR ) - 1 ] == ICON_TYPE_SKILL ||
					m_GriPar.m_btIconType [ GetGridIndex ( LandR ) - 1 ] == ICON_TYPE_XP )
			{
				CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_POINT, lParam ) ;
				CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_IMAGE, m_GriPar.m_dwMouseMoveIconTypeID ) ;
				return ;
			}
			CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_POINT, lParam ) ;
			CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_GOODS, m_GriPar.m_btOwnerID ) ;
		}
		else if ( m_GriPar.m_bMouseMove && m_GriPar.m_MouseMoveIconLandR != LandR )
		{
			m_GriPar.m_bMouseMove = true ;
			m_GriPar.m_MouseMoveIconLandR = LandR ;
			int nPosX = m_GriPar.m_Point.x + m_GriPar.m_btGridWidth * ( LandR.x - 1 ) ;
			int nPosY = m_GriPar.m_Point.y + m_GriPar.m_btGridHeight * ( LandR.y - 1 ) ;
			m_GriPar.m_dwMouseMoveIconIndex = m_GriPar.m_IconIndex [GetGridIndex ( LandR ) - 1 ];
			m_GriPar.m_dwMouseMoveIconTypeID = m_GriPar.m_IconTypeID [GetGridIndex ( LandR ) - 1 ];
			LPARAM lParam = MAKELONG( nPosX, nPosY ) ;
			if ( m_GriPar.m_btIconType [ GetGridIndex ( LandR ) - 1 ] == ICON_TYPE_SKILL ||
					m_GriPar.m_btIconType [ GetGridIndex ( LandR ) - 1 ] == ICON_TYPE_XP )
			{
				CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_POINT, lParam ) ;
				CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_IMAGE, m_GriPar.m_dwMouseMoveIconTypeID ) ;
				return ;
			}
			CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_POINT, lParam ) ;
			CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_GOODS, m_GriPar.m_btOwnerID ) ;
		}
	}
	else
	{
		if ( m_GriPar.m_bMouseMove )
		{
			m_GriPar.m_bMouseMove = false ;
			CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_HIDE ) ;
		}
	}
	CButton::OnMouseMove(nFlags, point);
}

void CMyGrid::MouseMoveTest()
{
	POINT MousePoint ;
	GetCursorPos ( &MousePoint ) ;
	CRect rect ;
	CMyGrid::GetWindowRect ( rect ) ;
	if ( !rect.PtInRect ( MousePoint ) )
	{
		m_GriPar.m_bMouseMove = false ;
		CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSGDLG_HIDE ) ;
	}
}

void CMyGrid::SetPickUp(BOOL bPickUp)
{
	m_GriPar.m_bPickUp = bPickUp ;
}

CPoint CMyGrid::GetInsertLandR()
{
	return m_GriPar.m_InsetLandR ;
}

int CMyGrid::ChangeIcon(CPoint IconLandR, OBJID ItemTypeID, DWORD dwItemIndex, BYTE btItemType, BYTE btActType)
{
	int nIndex = ( IconLandR.y - 1 ) * m_GriPar.m_btGridRow + IconLandR.x ;
	m_GriPar.m_PickUpIconTypeID = m_GriPar.m_IconTypeID [nIndex - 1] ;
	m_GriPar.m_dwPickUpIconIndex = m_GriPar.m_IconIndex [nIndex - 1] ;
	m_GriPar.m_IconTypeID [nIndex-1] = ItemTypeID ;
	m_GriPar.m_IconIndex[nIndex-1] = dwItemIndex ;
	m_GriPar.m_btIconType[nIndex-1] = btItemType ;
	m_GriPar.m_btIconActType[nIndex-1] = btActType ;
	if ( btActType != 0 )
		m_GriPar.m_dwIconActTime[nIndex-1] = 0 ;
	return 1 ;
}

void CMyGrid::RemoveIcon(int nLine, int nRow, BOOL bAll)
{
	if ( bAll )
	{
		m_GriPar.m_nGridAbleNum = 0;
		for ( int i = 0; i < 256; i++ )
		{
			m_GriPar.m_bGridAble[i] = false ;
		}
		return ;
	}
	else
	{
		SetGridAble( CPoint ( nRow, nLine ), false ) ;
		m_GriPar.m_nGridAbleNum-- ;
	}
}

void CMyGrid::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CButton::OnLButtonDblClk(nFlags, point);
}



void CMyGrid::ReSumePickUp()
{
	LoadIcon( m_GriPar.m_PickUpIconLandR, m_GriPar.m_PickUpIconTypeID, m_GriPar.m_dwPickUpIconIndex, m_GriPar.m_btPickUpIconType, m_GriPar.m_btPickUpIconActType ) ;
}

void CMyGrid::SetEnable(BOOL bEnable)
{
	m_GriPar.m_bEnable = bEnable ;
}

void CMyGrid::SetPickEnable(BOOL bEnable)
{
	m_GriPar.m_bPickEnable = bEnable ;
}

int CMyGrid::GetIconTypeID(int nIndex)
{
	return m_GriPar.m_IconTypeID [nIndex] ;
}

void CMyGrid::SetIconNum(int nIndex, int nNum)
{
	m_GriPar.m_IconNum[nIndex] = nNum ;
}

void CMyGrid::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( m_GriPar.m_btPickUpIconType == ICON_TYPE_ACTION )
		CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP, DLG_ACT ) ;
	else if ( m_GriPar.m_btPickUpIconType == ICON_TYPE_SKILL )
		CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP, DLG_MAGIC ) ;
	else if ( m_GriPar.m_btPickUpIconType == ICON_TYPE_XP )
		CMyGrid::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP, DLG_XP ) ;
	CButton::OnLButtonUp(nFlags, point);
}

BYTE CMyGrid::GetPickUpType()
{
	return m_GriPar.m_btPickUpIconType ;
}

BYTE CMyGrid::GetIconType(int nIndex)
{
	return m_GriPar.m_btIconType [nIndex] ;
}

BYTE CMyGrid::GetPickUpActType()
{
	return m_GriPar.m_btPickUpIconActType ;
}

void CMyGrid::SetIconRealID(int nIndex, OBJID rID)
{
	m_GriPar.m_IconRealID [nIndex] = rID ;
}

int CMyGrid::GetIconRealID(int nIndex)
{
	return m_GriPar.m_IconRealID[nIndex] ;
}

LRESULT CMyGrid::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
