#include "stdafx.h"
#include "MyShell.h"
#include "MyProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMyProgress::CMyProgress()
{
	m_PrgPar.m_bBorder = false ;
	m_PrgPar.m_btShowMode = 0 ;
}

CMyProgress::~CMyProgress()
{
}


BEGIN_MESSAGE_MAP(CMyProgress, CProgressCtrl)
END_MESSAGE_MAP()

void CMyProgress::Show(int x, int y, int nPos, int nCurpos)
{
	if ( nCurpos > m_PrgPar.m_nLower )
		nCurpos = m_PrgPar.m_nLower ;

	if ( nPos > m_PrgPar.m_nLower )
		nPos = m_PrgPar.m_nLower ;
	IAni *ShowAni ;
	switch  ( m_PrgPar.m_btType )
	{
	case MY_PROGRESS_V:
		{
			switch ( m_PrgPar.m_btShowMode )
			{
			case 0:
				{
					if ( nPos <= nCurpos )
					{
						nCurpos = nPos ;
						if ( nCurpos == 0 )
							break ;

						int upleft = m_PrgPar.m_nHeight - ( nPos * m_PrgPar.m_nStep );
						RECT rect = { 0,
									  upleft ,
									  m_PrgPar.m_nWidth,
									  m_PrgPar.m_nHeight } ;

						ShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) g_strControlAni,
																m_PrgPar.m_strProgressID,
																EXIGENCE_IMMEDIATE ) ;
						if ( ShowAni != NULL )
							ShowAni->ShowEx ( 0,
											  m_PrgPar.m_IntPoint.x + x,
											  m_PrgPar.m_IntPoint.y + y + upleft,
											  &rect,
											  m_PrgPar.m_nWidth,
											  m_PrgPar.m_nHeight - upleft ) ;
					}
					else
					{
						int upleft = m_PrgPar.m_nHeight - ( nPos * m_PrgPar.m_nStep );
						RECT rect = { 0,
									  upleft ,
									  m_PrgPar.m_nWidth,
									  m_PrgPar.m_nHeight } ;

						ShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) g_strControlAni,
																m_PrgPar.m_strProgressID,
																EXIGENCE_IMMEDIATE ) ;
						if ( ShowAni != NULL )
						{
							ShowAni->ShowEx ( 1,
											  m_PrgPar.m_IntPoint.x + x,
											  m_PrgPar.m_IntPoint.y + y + upleft,
											  &rect,
											  m_PrgPar.m_nWidth,
											  nPos * m_PrgPar.m_nStep ) ;
							nCurpos += m_PrgPar.m_fAddStep / m_PrgPar.m_nStep + 0.5f;
							upleft = m_PrgPar.m_nHeight - ( nCurpos * m_PrgPar.m_nStep );
							rect.top = upleft ;
							ShowAni->ShowEx ( 0,
											  m_PrgPar.m_IntPoint.x + x,
											  m_PrgPar.m_IntPoint.y + y + upleft,
											  &rect,
											  m_PrgPar.m_nWidth,
											  ( int ) ( nCurpos * m_PrgPar.m_nStep + 0.5f ) ) ;
						}
					}
				}
				break ;
			case 1:
				{
					if ( nPos <= nCurpos )
					{
						if ( nPos == 0 )
							break ;

						int upleft = m_PrgPar.m_nHeight - ( nPos * m_PrgPar.m_nStep );
						RECT rect = { 0,
									  upleft ,
									  m_PrgPar.m_nHWidth,
									  m_PrgPar.m_nHeight } ;

						ShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) g_strControlAni,
																m_PrgPar.m_strProgressID,
																EXIGENCE_IMMEDIATE ) ;
						if ( ShowAni != NULL )
							ShowAni->ShowEx ( 2,
											  m_PrgPar.m_IntPoint.x + x,
											  m_PrgPar.m_IntPoint.y + y + upleft,
											  &rect,
											  m_PrgPar.m_nHWidth,
											  m_PrgPar.m_nHeight - upleft ) ;
					}
					else
					{
						int upleft = m_PrgPar.m_nHeight - ( nPos * m_PrgPar.m_nStep );
						RECT rect = { 0,
									  upleft ,
									  m_PrgPar.m_nHWidth,
									  m_PrgPar.m_nHeight } ;

						ShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) g_strControlAni,
																m_PrgPar.m_strProgressID,
																EXIGENCE_IMMEDIATE ) ;
						if ( ShowAni != NULL )
						{
							upleft = m_PrgPar.m_nHeight - ( nCurpos * m_PrgPar.m_nStep );
							rect.top = upleft ;
							ShowAni->ShowEx ( 2,
											  m_PrgPar.m_IntPoint.x + x,
											  m_PrgPar.m_IntPoint.y + y + upleft,
											  &rect,
											  m_PrgPar.m_nHWidth,
											  ( int ) ( nCurpos * m_PrgPar.m_nStep + 0.5f ) ) ;
						}
					}
				}
				break ;
			case 2:
				{
					ShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) g_strControlAni,
															m_PrgPar.m_strProgressID,
															EXIGENCE_IMMEDIATE ) ;

					if ( ShowAni != NULL )
						ShowAni->Show ( 2,
										m_PrgPar.m_IntPoint.x + x,
										m_PrgPar.m_IntPoint.y + y ) ;
				}
			}
		}
		break ;
	case MY_PROGRESS_HLEFT:
		{
			if ( nPos > 0 )
			{
				int left = ( float )nPos * m_PrgPar.m_nStep ;
				CMyBitmap::ShowBlock ( m_PrgPar.m_IntPoint.x + x,
									   m_PrgPar.m_IntPoint.y + y,
									   m_PrgPar.m_IntPoint.x + x + left,
									   m_PrgPar.m_IntPoint.y + y + m_PrgPar.m_nHeight / 4,
									   0xfff1d06e) ;
				CMyBitmap::ShowBlock ( m_PrgPar.m_IntPoint.x + x,
									   m_PrgPar.m_IntPoint.y + y + m_PrgPar.m_nHeight / 4,
									   m_PrgPar.m_IntPoint.x + x + left,
									   m_PrgPar.m_IntPoint.y + y + m_PrgPar.m_nHeight * 3 / 4,
									   0xffe8a326) ;
				CMyBitmap::ShowBlock ( m_PrgPar.m_IntPoint.x + x,
									   m_PrgPar.m_IntPoint.y + y + m_PrgPar.m_nHeight * 3 / 4,
									   m_PrgPar.m_IntPoint.x + x + left,
									   m_PrgPar.m_IntPoint.y + y + m_PrgPar.m_nHeight,
									   0xffab916c) ;
			}
			else if ( nPos < 0 )
			{
				int left = - ( float )nPos * m_PrgPar.m_nStep ;
				CMyBitmap::ShowBlock ( m_PrgPar.m_IntPoint.x + x,
									   m_PrgPar.m_IntPoint.y + y,
									   m_PrgPar.m_IntPoint.x + x + left,
									   m_PrgPar.m_IntPoint.y + y + m_PrgPar.m_nHeight,
									   0xffff0000) ;
			}

		}
		break ;
	case MY_PROGRESS_HRIGHT:
		{
			if ( nPos != m_PrgPar.m_nMax )
			{
				int left = ( float )nPos * m_PrgPar.m_nStep ;
				RECT rect = { left,
							  0,
							  m_PrgPar.m_nWidth,
							  m_PrgPar.m_nHeight } ;
				IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) g_strControlAni,
															  m_PrgPar.m_strProgressID,
															  EXIGENCE_IMMEDIATE ) ;
				if ( ShowAni )
				{
					ShowAni->ShowEx ( 0,
					                  m_PrgPar.m_IntPoint.x + x + left,
									  m_PrgPar.m_IntPoint.y + y,
									  &rect,
									  m_PrgPar.m_nWidth - left,
									  m_PrgPar.m_nHeight ) ;
				}
			}
		}
		break ;
	}
	if ( m_PrgPar.m_bBorder )
		GameDataSetQuery()->GetDataAni ( ( char * ) g_strControlAni,
									  m_PrgPar.m_strProgressID,
									  EXIGENCE_NORMAL )->Show ( 1,
																m_PrgPar.m_IntPoint.x + x - m_PrgPar.m_BorderOffset.x,
																m_PrgPar.m_IntPoint.y + y - m_PrgPar.m_BorderOffset.y ) ;
}

BOOL CMyProgress::Init(int xPos, int yPos, char *ProgressID, int nLower, int nUpper, BYTE btType, BOOL bBorder, CPoint BorderPnt )
{
	g_objFontScale.Adjust ( this, GetDlgCtrlID () ) ;
	CRect rect ;
	CMyProgress::GetWindowRect ( rect ) ;
	m_PrgPar.m_IntPoint.x = rect.left - xPos ;
	m_PrgPar.m_IntPoint.y = rect.top - yPos ;
	if ( g_objFontScale.IsScale () )
	{
		m_PrgPar.m_IntPoint.x = (int) ( m_PrgPar.m_IntPoint.x * g_objFontScale.GetScaleX () + 0.5f ) ;
		m_PrgPar.m_IntPoint.y = (int) ( m_PrgPar.m_IntPoint.y * g_objFontScale.GetScaleY () + 0.5f ) ;
		int nW = (int) ( rect.Width () * g_objFontScale.GetScaleX () + 0.5f ) ;
		int nH = (int) ( rect.Height () * g_objFontScale.GetScaleY () + 0.5f ) ;
		CMyProgress::MoveWindow ( m_PrgPar.m_IntPoint.x, m_PrgPar.m_IntPoint.y, nW, nH, FALSE ) ;
		CMyProgress::GetWindowRect ( rect ) ;
	}

	CMyProgress::SetRange ( nLower, nUpper ) ;

	if ( bBorder )
	{
		m_PrgPar.m_bBorder = true ;
		m_PrgPar.m_BorderOffset = BorderPnt ;
	}

	m_PrgPar.m_btType = btType ;
	switch ( btType )
	{
	case MY_PROGRESS_V:
		strcpy ( m_PrgPar.m_strProgressID, ProgressID ) ;
		break ;
	case MY_PROGRESS_HLEFT:
		m_PrgPar.m_nWidth = rect.right - rect.left ;
		m_PrgPar.m_nHeight = rect.bottom - rect.top - 4 ;
		m_PrgPar.m_nStep = ( float )m_PrgPar.m_nWidth / ( nUpper - nLower ) ;
		break ;
	case MY_PROGRESS_HRIGHT:
		{
			strcpy ( m_PrgPar.m_strProgressID, ProgressID ) ;
			CMySize Size = { 0, 0 } ;
			IAni* pSizeAni = GameDataSetQuery()->GetDataAni ( ( char * ) g_strControlAni, ProgressID, EXIGENCE_IMMEDIATE ) ;
			if ( pSizeAni != NULL )
				Size = pSizeAni->GetSize ( 0 ) ;
			m_PrgPar.m_nWidth = Size.iWidth ;
			m_PrgPar.m_nHeight = Size.iHeight ;
			m_PrgPar.m_nStep = ( float )m_PrgPar.m_nWidth / ( nUpper - nLower ) ;
		}
		break ;
	}
	return true ;
}

void CMyProgress::ResetValue(  int nLower, int nUpper )
{
	CRect rect ;
	CMyProgress::GetWindowRect ( rect ) ;
	CMyProgress::SetRange ( nLower, nUpper ) ;

	switch ( m_PrgPar.m_btType )
	{
	case MY_PROGRESS_V:
		m_PrgPar.m_nStep = 0 ;
		if ( ( nUpper - nLower ) != 0 )
		{
			if ( ( nUpper - nLower ) < 100 )
				m_PrgPar.m_fAddStep = ( float ) m_PrgPar.m_nHeight / ( nUpper - nLower ) ;
			else
				m_PrgPar.m_fAddStep = ( float ) m_PrgPar.m_nHeight / 100.0f ;
			m_PrgPar.m_nStep = ( float )m_PrgPar.m_nHeight / ( nUpper - nLower ) ;
		}
		break ;
	case MY_PROGRESS_HLEFT:
		m_PrgPar.m_nStep = 0 ;
		if ( nUpper - nLower != 0 )
			m_PrgPar.m_nStep = ( float )m_PrgPar.m_nWidth / ( nUpper - nLower ) ;
		break ;
	case MY_PROGRESS_HRIGHT:
		m_PrgPar.m_nStep = 0 ;
		m_PrgPar.m_nMax = nUpper - nLower ;
		if ( nUpper - nLower != 0 )
			m_PrgPar.m_nStep = ( float )m_PrgPar.m_nWidth / ( nUpper - nLower ) ;
		break ;
	}
}


void CMyProgress::SetRange(int nUpper, int nLower)
{
	m_PrgPar.m_nUpper = nUpper ;
	m_PrgPar.m_nLower = nLower ;
}

void CMyProgress::GetRange(int &nUpper, int &nLower)
{
	nUpper = m_PrgPar.m_nUpper ;
	nLower = m_PrgPar.m_nLower ;
}

void CMyProgress::SetShowMode(BYTE btShowMode)
{
	m_PrgPar.m_btShowMode = btShowMode ;
}

void CMyProgress::SetWidthAndHeight(int nWidth, int nHeight, int nHWidth)
{
	m_PrgPar.m_nWidth = nWidth ;
	m_PrgPar.m_nHeight = nHeight ;
	m_PrgPar.m_nHWidth = nHWidth ;
}

LRESULT CMyProgress::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

	return CProgressCtrl::WindowProc(message, wParam, lParam);
}
