#include "stdafx.h"
#include "MyShell.h"
#include "MyCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#undef new
#endif

CMyCombo::CMyCombo()
{
	m_nStartChar = 0 ;
	m_bChinese = true;
	m_pBmp = NULL ;
	m_OffsetPoint.x = 3 ;
	m_OffsetPoint.y = 3 ;
}

CMyCombo::~CMyCombo()
{
	if ( m_pBmp != NULL )
	{
		delete m_pBmp ;
		m_pBmp = NULL ;
	}
}


BEGIN_MESSAGE_MAP(CMyCombo, CEdit)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
END_MESSAGE_MAP()

void CMyCombo::Show(int x, int y)
{
	int nX = m_IntPnt.x + x ;
	int nY = m_IntPnt.y + y ;

	if ( m_pBmp != NULL )
		m_pBmp->Show ( nX, nY, 0, 0 ) ;

	CMyCombo::ShowChar( nX, nY );
}

void CMyCombo::ShowChar( int x, int y )
{
	char strChar[1024] = "";
	if ( 0 >= CMyCombo::GetLine ( 0, strChar, sizeof( strChar ) ) )
	{
		if ( ::GetFocus () == m_hWnd && TimeGet () / 300 % 4 < 3 )
		{
			CMyBitmap::ShowBlock ( x + m_OffsetPoint.x + 1,
								   y + m_OffsetPoint.y,
								   x + m_OffsetPoint.x + 3,
								   y + m_OffsetPoint.y + m_nFontWidth * 2,
								   0xffffffff );
		}
		return;
	}

	int nSelStart = 0, nSelEnd = 0;
	CMyCombo::GetSel( nSelStart, nSelEnd );
	if ( nSelStart < 0 )
		nSelStart = 0 ;
	if ( nSelEnd < 0 )
		nSelEnd = 0 ;

	if ( ( DWORD ) nSelStart < ( DWORD ) m_nStartChar )
		m_nStartChar = nSelStart ;
	if ( ( DWORD ) nSelEnd > ( DWORD ) ( m_nStartChar + m_nCharNum ) )
		m_nStartChar += nSelEnd - ( m_nStartChar + m_nCharNum ) ;
	if ( nSelEnd - nSelStart > m_nCharNum )
		m_nStartChar = nSelStart ;

	if ( nSelStart != nSelEnd )
	{
		int nSelNum = nSelEnd - nSelStart ;
		if ( nSelNum > m_nCharNum )
			nSelNum = m_nCharNum ;
		int nSelBegin = CMyBitmap::GetStrShowLen ( strChar, m_nStartChar, nSelStart ) ;
		int nSelStop = CMyBitmap::GetStrShowLen ( strChar, m_nStartChar, nSelStart + nSelNum ) ;
		CMyBitmap::ShowBlock ( x + m_OffsetPoint.x + nSelBegin * m_nFontWidth,
							   y + m_OffsetPoint.y,
							   x + m_OffsetPoint.x + nSelStop * m_nFontWidth,
							   y + m_OffsetPoint.y + m_nFontWidth * 2,
							   0xff2222ff ) ;
	}

	int nShowNum = strlen ( strChar + m_nStartChar ) ;
	if ( nShowNum > m_nCharNum )
		nShowNum = m_nCharNum ;
	if ( nShowNum > 0 )
	{
		char strShow[1024] = "";
		strncpy ( strShow, strChar + m_nStartChar, nShowNum ) ;
		if ( ( DWORD ) m_nStartChar > 0 && IsDBCSLeadByte ( strChar, m_nStartChar - 1 ) )
			strShow[0] = ' ' ;
		if ( IsDBCSLeadByte ( strShow, nShowNum - 1 ) )
			strShow[nShowNum - 1] = ' ' ;
		CMyBitmap::ShowEmotionString ( x + m_OffsetPoint.x,
									   y + m_OffsetPoint.y,
									   m_dwFontColor,
									   strShow, (IDataIcon*) GameDataSetQuery()->QueryInterface(0), NULL, 14, 0, 0, 0 ) ;
	}

	int nCursor = -1 ;
	if ( nSelStart == nSelEnd )
		nCursor = nSelStart ;
	if ( nCursor != -1 && ::GetFocus () == m_hWnd && TimeGet () / 300 % 4 < 3 )
	{
		int nCursorPos = CMyBitmap::GetStrShowLen ( strChar, m_nStartChar, nCursor ) ;
		CMyBitmap::ShowBlock ( x + m_OffsetPoint.x + nCursorPos * m_nFontWidth + 1,
							   y + m_OffsetPoint.y,
							   x + m_OffsetPoint.x + nCursorPos * m_nFontWidth + 3,
							   y + m_OffsetPoint.y + m_nFontWidth * 2,
							   0xffffffff ) ;
	}
}

void CMyCombo::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}

BOOL CMyCombo::Init(int xPos, int yPos, char *FileName, DWORD dwFontColor, DWORD dwBackColor )
{
	g_objFontScale.Adjust ( this, GetDlgCtrlID () ) ;
	CRect rect;
	CMyCombo::GetWindowRect ( rect ) ;
	m_IntPnt.x = rect.left - xPos ;
	m_IntPnt.y = rect.top - yPos ;
	m_nWidth = rect.right - rect.left ;
	m_nHeight = rect.bottom - rect.top ;
	m_nFontWidth = CMyBitmap::GetFontSize ( ) / 2 ;
	m_nCharNum = m_nWidth / m_nFontWidth ;

	m_dwFontColor = dwFontColor ;
	m_dwBackColor = dwBackColor ;

	if ( m_EditRect.m_hWnd == NULL )
	{
		this->MoveWindow( m_IntPnt.x, m_IntPnt.y, 0, 0, FALSE );
		CRect EditRect = CRect( m_IntPnt.x,
								m_IntPnt.y,
								m_IntPnt.x + rect.Width(),
								m_IntPnt.y + rect.Height() );
		m_EditRect.Create( "",
						   WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW,
						   EditRect,
						   this->GetParent(),
						   0 );
		m_EditRect.Init( this );
	}

 	CMyCombo::SetLimitText ( 256 ) ;

	return true ;
}

void CMyCombo::ReInit(int xPos, int yPos)
{
	CRect rect;
	m_EditRect.GetWindowRect ( rect ) ;
	m_IntPnt.x = rect.left - xPos ;
	m_IntPnt.y = rect.top - yPos ;
	m_nWidth =	  rect.right - rect.left ;
	m_nHeight =  rect.bottom - rect.top ;
	m_nFontWidth = CMyBitmap::GetFontSize ( ) / 2 ;
	m_nCharNum = m_nWidth / m_nFontWidth ;
}

LRESULT CMyCombo::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_ERASEBKGND:
		return true ;
	case WM_NCPAINT:
		return false ;
	case WM_PAINT:
		{
			PAINTSTRUCT ps ;
			::BeginPaint ( m_hWnd, &ps ) ;
			::EndPaint ( m_hWnd, &ps ) ;
			return false ;
		}
	}
	return CEdit::WindowProc(message, wParam, lParam);
}

void CMyCombo::InitListBox(CDlgListBox *ListBox)
{
	m_CobLstBox = ListBox ;
}

void CMyCombo::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch ( nChar )
	{
	case 38:
		CMyCombo::GetParent()->PostMessage ( WM_VSCROLL, SB_LINEUP ) ;
		return ;
	case 40:
		CMyCombo::GetParent()->PostMessage ( WM_VSCROLL, SB_LINEDOWN ) ;
		return ;
	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMyCombo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( nFlags == 0 && !m_bChinese )
		return;

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CMyCombo::EnableChinese( BOOL bChinese )
{
	m_bChinese = bChinese;
}
