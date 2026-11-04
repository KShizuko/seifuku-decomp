#include "stdafx.h"
#include "MyShell.h"
#include "MySlider.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMySlider::CMySlider()
{
	m_SliPar.m_bBtnDown = false ;
	m_SliPar.m_nSliderPos = 0 ;
	m_SliPar.m_nSliderCurPos = 0 ;
	m_SliPar.m_wdSliderWidth = 0 ;
	m_SliPar.m_MousePoint = CPoint ( 0, 0 ) ;
	m_SliPar.m_uFrame = 0 ;
}

CMySlider::~CMySlider()
{
}


BEGIN_MESSAGE_MAP(CMySlider, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CMySlider::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_SliPar.m_bBtnDown = true ;
	m_SliPar.m_uFrame = 1 ;
	GetCursorPos ( &m_SliPar.m_MousePoint ) ;
	CButton::OnLButtonDown(nFlags, point);
}

void CMySlider::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_SliPar.m_bBtnDown = false ;
	m_SliPar.m_uFrame = 0 ;

	m_SliPar.m_nSliderCurPos = m_SliPar.m_nSliderPos ;
	CRect rect ;
	CMySlider::GetWindowRect ( rect ) ;
	if ( !m_SliPar.m_bV )
	{
		CMySlider::MoveWindow ( m_SliPar.m_IntPoint.x + m_SliPar.m_nSliderPos,
								m_SliPar.m_IntPoint.y,
								rect.right - rect.left,
								rect.bottom - rect.top,
								true ) ;
		CMySlider::GetParent ()->PostMessage ( WM_HSCROLL ,0) ;
	}
	else
	{
		CMySlider::MoveWindow ( m_SliPar.m_IntPoint.x,
								m_SliPar.m_IntPoint.y - m_SliPar.m_nSliderPos,
								rect.right - rect.left,
								rect.bottom - rect.top,
								true ) ;
		CMySlider::GetParent ()->PostMessage ( WM_VSCROLL , 0) ;
	}



	CButton::OnLButtonUp(nFlags, point);
}



void CMySlider::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( m_SliPar.m_bBtnDown )
	{
		CPoint MousePoint ;
		GetCursorPos( &MousePoint ) ;
		if ( !m_SliPar.m_bV )
		{
			m_SliPar.m_nSliderPos = ( ( MousePoint.x - m_SliPar.m_MousePoint.x + m_SliPar.m_nSliderCurPos  ) < 0 ) ? 0 : ( MousePoint.x - m_SliPar.m_MousePoint.x + m_SliPar.m_nSliderCurPos) ;
			m_SliPar.m_nSliderPos = ( m_SliPar.m_nSliderPos > m_SliPar.m_wdSliderWidth ) ? m_SliPar.m_wdSliderWidth : m_SliPar.m_nSliderPos ;
		}
		else
		{
			m_SliPar.m_nSliderPos = ( ( m_SliPar.m_MousePoint.y - MousePoint.y + m_SliPar.m_nSliderCurPos  ) < 0 ) ? 0 : ( m_SliPar.m_MousePoint.y - MousePoint.y + m_SliPar.m_nSliderCurPos) ;
			m_SliPar.m_nSliderPos = ( m_SliPar.m_nSliderPos > m_SliPar.m_wdSliderWidth ) ? m_SliPar.m_wdSliderWidth : m_SliPar.m_nSliderPos ;
		}

	}
	CButton::OnMouseMove(nFlags, point);
}


void CMySlider::Show(int x, int y)
{
	IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) g_strControlAni,
												  m_SliPar.m_strSliderID,
												  EXIGENCE_IMMEDIATE ) ;
	if ( ShowAni != NULL )
	{
		if ( !m_SliPar.m_bV )
			ShowAni->Show ( m_SliPar.m_uFrame,
							m_SliPar.m_IntPoint.x + x + m_SliPar.m_nSliderPos,
							m_SliPar.m_IntPoint.y + y) ;
		else
			ShowAni->Show ( m_SliPar.m_uFrame,
							m_SliPar.m_IntPoint.x + x,
							m_SliPar.m_IntPoint.y + y  - m_SliPar.m_nSliderPos) ;
	}
}

void CMySlider::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}

void CMySlider::SetSliderWidth(int nWidth)
{
	m_SliPar.m_wdSliderWidth = nWidth ;
}

void CMySlider::SetMaxValue(int nValue)
{
	m_SliPar.m_wdSliderMaxValue = nValue ;
}

int CMySlider::GetSliderValue()
{
	return ( m_SliPar.m_nSliderPos * m_SliPar.m_wdSliderMaxValue ) / m_SliPar.m_wdSliderWidth ;
}

BOOL CMySlider::Init(int xPos, int yPos, char *SliderID, BOOL bType)
{
	g_objFontScale.Adjust ( this, GetDlgCtrlID () ) ;
	CRect rect ;
	CMySlider::GetWindowRect ( rect ) ;
	m_SliPar.m_IntPoint.x = rect.left - xPos ;
	m_SliPar.m_IntPoint.y = rect.top - yPos ;
	if ( g_objFontScale.IsScale () )
	{
		m_SliPar.m_IntPoint.x = (int) ( m_SliPar.m_IntPoint.x * g_objFontScale.GetScaleX () + 0.5f ) ;
		m_SliPar.m_IntPoint.y = (int) ( m_SliPar.m_IntPoint.y * g_objFontScale.GetScaleY () + 0.5f ) ;
		int nW = (int) ( rect.Width () * g_objFontScale.GetScaleX () + 0.5f ) ;
		int nH = (int) ( rect.Height () * g_objFontScale.GetScaleY () + 0.5f ) ;
		CMySlider::MoveWindow ( m_SliPar.m_IntPoint.x, m_SliPar.m_IntPoint.y, nW, nH, FALSE ) ;
	}
	m_SliPar.m_bV = bType ;

	strcpy ( m_SliPar.m_strSliderID, SliderID ) ;

	return true ;
}

void CMySlider::SetSliderValue(int nValue)
{
	if ( nValue > 0 && nValue <= m_SliPar.m_wdSliderMaxValue )
	{
		m_SliPar.m_nSliderPos = ( nValue * m_SliPar.m_wdSliderWidth ) / m_SliPar.m_wdSliderMaxValue ;
		m_SliPar.m_nSliderCurPos = m_SliPar.m_nSliderPos ;
		CRect rect ;
		CMySlider::GetWindowRect ( rect ) ;
		if ( !m_SliPar.m_bV )
			CMySlider::MoveWindow ( m_SliPar.m_IntPoint.x + m_SliPar.m_nSliderPos,
									m_SliPar.m_IntPoint.y,
									rect.right - rect.left,
									rect.bottom - rect.top,
									true ) ;
		else
			CMySlider::MoveWindow ( m_SliPar.m_IntPoint.x,
									m_SliPar.m_IntPoint.y - m_SliPar.m_nSliderPos,
									rect.right - rect.left,
									rect.bottom - rect.top,
									true ) ;
	}
}

LRESULT CMySlider::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
