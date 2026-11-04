#include "stdafx.h"
#include "MyShell.h"
#include "MyButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMyButton::CMyButton()
{
	m_uFrame = 0 ;
}

CMyButton::~CMyButton()
{
}


BEGIN_MESSAGE_MAP(CMyButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void CMyButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}


void CMyButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CButton::OnMouseMove(nFlags, point);
}


void CMyButton::Show( int x, int y )
{
	m_BtnPnt.x = m_IntPnt.x  + x ;
	m_BtnPnt.y = m_IntPnt.y  + y ;
	IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) g_strControlAni,
											  m_strButtonID,
											  EXIGENCE_IMMEDIATE ) ;
	if ( ShowAni != NULL )
	{
		ShowAni->Show ( m_uFrame,
					m_BtnPnt.x,
					m_BtnPnt.y ) ;
	}
}

void CMyButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( 1== m_uFrame )
		m_uFrame = 0 ;
	if ( m_btOwnerID == DLG_ACT)
	{
		CMyButton::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_DROP, m_btOwnerID ) ;
	}
	CButton::OnLButtonUp(nFlags, point);
}

void CMyButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_uFrame = 1 ;
	if ( m_btOwnerID == DLG_ACT )
	{
		LPARAM lParam ;
		CPoint MousePnt ;
		CMyButton::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_ID, m_nButtonID ) ;
		GetCursorPos ( &MousePnt ) ;
		lParam = MAKELONG ( MousePnt.x, MousePnt.y ) ;
		CMyButton::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_PNT, lParam ) ;
		CMyButton::GetParent ()->GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ICON_PICKUP, m_btOwnerID ) ;
	}
	else if ( m_btOwnerID == 32 )
	{
		if ( nFlags&MK_CONTROL )
		{
			m_uFrame = 0 ;
			return ;
		}
	}
	CButton::OnLButtonDown(nFlags, point);
}

BOOL CMyButton::Init(int xPos, int yPos, char *ButtonID, BYTE btOwnerID, int nButtonID)
{
	g_objFontScale.Adjust ( this, GetDlgCtrlID () ) ;
	CRect rect;
	CMyButton::GetWindowRect ( rect ) ;
	m_IntPnt.x = rect.left - xPos ;
	m_IntPnt.y = rect.top - yPos ;
	if ( g_objFontScale.IsScale () )
	{
		m_IntPnt.x = (int) ( m_IntPnt.x * g_objFontScale.GetScaleX () + 0.5f ) ;
		m_IntPnt.y = (int) ( m_IntPnt.y * g_objFontScale.GetScaleY () + 0.5f ) ;
		int nW = (int) ( rect.Width () * g_objFontScale.GetScaleX () + 0.5f ) ;
		int nH = (int) ( rect.Height () * g_objFontScale.GetScaleY () + 0.5f ) ;
		CMyButton::MoveWindow ( m_IntPnt.x, m_IntPnt.y, nW, nH, FALSE ) ;
		char szTemp[128] = "" ;
		sprintf ( szTemp, "Rect.Left :%d rect.top:%d nW:%d nH:%d", m_IntPnt.x, m_IntPnt.y, nW, nH ) ;
		CMyButton::GetWindowRect ( rect ) ;
	}
	m_nWidth = rect.right - rect.left ;
	m_nHeight = rect.bottom - rect.top ;
	m_btOwnerID = btOwnerID ;
	m_nButtonID = nButtonID ;

	strcpy ( m_strButtonID, ButtonID ) ;
	return true ;
}

void CMyButton::SetCurFrame(UINT uFrame)
{
	m_uFrame = uFrame ;
}

LRESULT CMyButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_ENABLE )
		if ( !wParam )
			m_uFrame = 2 ;
		else
			m_uFrame = 0 ;
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
	return CButton::WindowProc(message, wParam, lParam);
}

void CMyButton::ChangeImage(char *ButtonID)
{
	strcpy ( m_strButtonID, ButtonID ) ;
}

void CMyButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CButton::OnLButtonDblClk(nFlags, point);
}

BOOL CMyButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	( ( CMyShellApp* ) AfxGetApp () )->SetOnButton ( 1 ) ;
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}
