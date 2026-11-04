#include "stdafx.h"
#include "MyShell.h"
#include "MyScrollButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMyScrollButton::CMyScrollButton()
{
	m_bDrpValue = false ;
	m_uFrame = 0 ;
}

CMyScrollButton::~CMyScrollButton()
{
}


BEGIN_MESSAGE_MAP(CMyScrollButton, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CMyScrollButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_uFrame = 1 ;
	SetTimer( 1, 100, NULL ) ;
	DWORD wparam ;
	switch ( m_btScrBtnType )
	{
	case MYSCROLL_UP:
		wparam = MAKELONG ( SB_LINEUP, 0 ) ;
		m_ParentWnd->PostMessage ( WM_VSCROLL, wparam, NULL ) ;
		break ;
	case MYSCROLL_DOWN:
		wparam = MAKELONG ( SB_LINEDOWN, 0 ) ;
		m_ParentWnd->PostMessage ( WM_VSCROLL, wparam, NULL ) ;
		break ;
	case MYSCROLL_ROLL:
		break ;
	case MYSCROLL_DROP:
		m_bDrpValue = !m_bDrpValue ;
		wparam = m_bDrpValue ;
		m_ParentWnd->PostMessage ( CB_SHOWDROPDOWN, wparam, 1 ) ;
		break ;
	}
}

void CMyScrollButton::Init(CWnd *ListWnd, BYTE btScrBtnType)
{
	m_ParentWnd = ListWnd ;
	m_btScrBtnType = btScrBtnType ;
}

void CMyScrollButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	DWORD wparam ;
	switch ( m_btScrBtnType )
	{
	case MYSCROLL_UP:
		wparam = MAKELONG ( SB_LINEUP, 0 ) ;
		m_ParentWnd->PostMessage ( WM_VSCROLL, wparam, NULL ) ;
		break ;
	case MYSCROLL_DOWN:
		wparam = MAKELONG ( SB_LINEDOWN, 0 ) ;
		m_ParentWnd->PostMessage ( WM_VSCROLL, wparam, NULL ) ;
		break ;
	case MYSCROLL_ROLL:
		break ;
	case MYSCROLL_DROP:
		break ;
	}
}

void CMyScrollButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	KillTimer ( 1 ) ;
	m_uFrame = 0 ;
	DWORD wparam ;
	if ( m_btScrBtnType != MYSCROLL_DROP )
	{
		wparam = MAKELONG ( SB_ENDSCROLL, 0 ) ;
		m_ParentWnd->PostMessage ( WM_VSCROLL, wparam, NULL ) ;
	}
}

void CMyScrollButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}

void CMyScrollButton::OnTimer(UINT nIDEvent)
{
	DWORD wparam ;
	switch ( m_btScrBtnType )
	{
	case MYSCROLL_UP:
		wparam = MAKELONG ( SB_LINEUP, 0 ) ;
		m_ParentWnd->PostMessage ( WM_VSCROLL, wparam, NULL ) ;
		break ;
	case MYSCROLL_DOWN:
		wparam = MAKELONG ( SB_LINEDOWN, 0 ) ;
		m_ParentWnd->PostMessage ( WM_VSCROLL, wparam, NULL ) ;
		break ;
	}
	CButton::OnTimer(nIDEvent);
}

BOOL CMyScrollButton::Load(char *ScrButtonID)
{
	strcpy ( m_strScrButtonID, ScrButtonID ) ;
		return true ;
}

void CMyScrollButton::Show(int xPos, int yPos)
{
	IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) g_strControlAni,
													  m_strScrButtonID,
													  EXIGENCE_IMMEDIATE ) ;
	if ( ShowAni != NULL )
	{
		CMySize Size = ShowAni->GetSize ( m_uFrame ) ;
		RECT rect = { 0, 0, Size.iWidth, Size.iHeight } ;
		ShowAni->ShowEx ( m_uFrame, xPos, yPos, &rect, 16, 16, 0, _SHOWWAY_NORMAL ) ;
	}
	m_ScrBtnPnt.x = xPos ;
	m_ScrBtnPnt.y = yPos ;
}

LRESULT CMyScrollButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
