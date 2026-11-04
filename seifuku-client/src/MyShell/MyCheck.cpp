#include "stdafx.h"
#include "MyShell.h"
#include "MyCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MY_CHECK_TWOMODE	2
#define MY_CHECK_THREEMODE	3
#define MY_CHECK_FOURMODE	4

CMyCheck::CMyCheck()
{
	m_ChkPar.m_btCurrent = 0 ;
	m_ChkPar.m_uFrame = 0 ;
}

CMyCheck::~CMyCheck()
{
}


BEGIN_MESSAGE_MAP(CMyCheck, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL CMyCheck::Init(int xPos, int yPos, char *CheckID, BYTE btType, BYTE btCurrent)
{
	g_objFontScale.Adjust ( this, GetDlgCtrlID () ) ;
	CRect rect ;
	CMyCheck::GetWindowRect ( rect ) ;
	m_ChkPar.m_IntPoint.x = rect.left - xPos ;
	m_ChkPar.m_IntPoint.y = rect.top - yPos ;
	if ( g_objFontScale.IsScale () )
	{
		m_ChkPar.m_IntPoint.x = (int) ( m_ChkPar.m_IntPoint.x * g_objFontScale.GetScaleX () + 0.5f ) ;
		m_ChkPar.m_IntPoint.y = (int) ( m_ChkPar.m_IntPoint.y * g_objFontScale.GetScaleY () + 0.5f ) ;
		int nW = (int) ( rect.Width () * g_objFontScale.GetScaleX () + 0.5f ) ;
		int nH = (int) ( rect.Height () * g_objFontScale.GetScaleY () + 0.5f ) ;
		CMyCheck::MoveWindow ( m_ChkPar.m_IntPoint.x, m_ChkPar.m_IntPoint.y, nW, nH, FALSE ) ;
		CMyCheck::GetWindowRect ( rect ) ;
	}
	m_ChkPar.m_btType = btType ;
	m_ChkPar.m_btCurrent = btCurrent ;
	m_ChkPar.m_uFrame = btCurrent ;
	m_ChkPar.m_nWidth = rect.right - rect.left ;
	m_ChkPar.m_nHeight = rect.bottom - rect.top ;

	strcpy ( m_ChkPar.m_strCheckID, CheckID ) ;
	m_ChkPar.m_ShowRect.left = 0 ;
	m_ChkPar.m_ShowRect.top = 0 ;
	CMySize Size = { 0, 0 } ;
	IAni* pSizeAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni, CheckID, EXIGENCE_IMMEDIATE ) ;
	if ( pSizeAni != NULL )
	{
		Size = pSizeAni->GetSize ( m_ChkPar.m_uFrame ) ;
		m_ChkPar.m_ShowRect.right = Size.iWidth ;
		m_ChkPar.m_ShowRect.bottom = Size.iHeight ;
	}
	return true ;
}

void CMyCheck::Show(int x, int y)
{
	IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) g_strControlAni,
												  m_ChkPar.m_strCheckID,
												  EXIGENCE_IMMEDIATE ) ;
	if ( ShowAni != NULL )
		ShowAni->Show ( m_ChkPar.m_uFrame,
						m_ChkPar.m_IntPoint.x + x,
						m_ChkPar.m_IntPoint.y + y ) ;
}

void CMyCheck::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_ChkPar.m_btCurrent = ( m_ChkPar.m_btCurrent >= m_ChkPar.m_btType - 1 ) ? 0 : m_ChkPar.m_btCurrent + 1 ;
	m_ChkPar.m_uFrame = m_ChkPar.m_btCurrent ;

	CButton::OnLButtonDown(nFlags, point);
}

void CMyCheck::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}

BYTE CMyCheck::GetCheckValue()
{
	return m_ChkPar.m_btCurrent ;
}

void CMyCheck::SetCheckValue(BYTE btValue)
{
	if ( btValue < m_ChkPar.m_btType )
	{
		m_ChkPar.m_btCurrent = btValue ;
		m_ChkPar.m_uFrame = btValue ;
	}
}

void CMyCheck::OnMouseMove(UINT nFlags, CPoint point)
{
	LPARAM lParam ;
	lParam = MAKELONG ( point.x + m_ChkPar.m_IntPoint.x, point.y + m_ChkPar.m_IntPoint.y) ;
	CMyCheck::GetParent ()->PostMessage ( WM_MOUSEMOVE, nFlags, lParam ) ;
}

LRESULT CMyCheck::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

BOOL CMyCheck::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	( ( CMyShellApp* ) AfxGetApp () )->SetOnButton ( 1 ) ;
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}
