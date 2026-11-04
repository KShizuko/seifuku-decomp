#include "stdafx.h"
#include "MyShell.h"
#include "MyEdit.h"

#include "ND_Bitmap.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMyEdit::CMyEdit()
{
	m_EdtPar.m_nSelNum = 0 ;
	m_EdtPar.m_btRightLeft = 5 ;
	m_EdtPar.m_btEndCharPos = 7 ;
	m_EdtPar.m_uFrame = 0 ;
	m_EdtFnt.m_OffsetPoint = CPoint ( 1, 3 ) ;
	m_EdtFnt.m_bPassword = false ;
	m_EdtPar.m_strEditID[0] = 0 ;
	m_EdtPar.m_bMultiLine = false ;
	m_EdtPar.m_bChinese = true;
	m_EdtPar.m_nLastSelEnd = 0;
}

CMyEdit::~CMyEdit()
{
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_ENABLE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

void CMyEdit::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CEdit::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CMyEdit::Show(int x, int y)
{
	m_EdtPar.m_Pnt.x = m_EdtPar.m_IntPnt.x + x + 3 ;
	m_EdtPar.m_Pnt.y = m_EdtPar.m_IntPnt.y + y + 3 ;

	if ( m_EdtPar.m_strEditID[0] != 0 )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  m_EdtPar.m_strEditID,
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( m_EdtPar.m_uFrame,
							m_EdtPar.m_Pnt.x -2,
							m_EdtPar.m_Pnt.y - 4 );
	}
	if ( m_EdtPar.m_bMultiLine )
		CMyEdit::ShowMultiLine();
	else
		CMyEdit::ShowChar() ;
}



void CMyEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMyEdit::SetFocus () ;
	CEdit::OnLButtonDown(nFlags, point);
}

void CMyEdit::ShowChar()
{
	CRect rect ;
	m_EditRect.GetWindowRect ( rect ) ;
	m_EdtPar.m_nWidth = rect.Width () ;
	m_EdtPar.m_nHeight = rect.Height () ;
	m_EdtFnt.m_nCharNum = m_EdtPar.m_nWidth / m_EdtFnt.m_btFontWidth ;

	char strChar[1024] = "";
	memset(strChar, 0L, sizeof(strChar));

	if ( 0 >= this->GetWindowText( strChar, 1024 ) )
	{
		if ( ::timeGetTime() / 500 % 2 == 0 && this->GetFocus() == this )
		{
			CMyBitmap::ShowBlock ( m_EdtPar.m_Pnt.x,
								   m_EdtPar.m_Pnt.y,
								   m_EdtPar.m_Pnt.x + 2,
								   m_EdtPar.m_Pnt.y + CMyBitmap::GetFontSize() + 2,
							       0xffffffff );
		}
		return;
	}


	int nSelStart = 0, nSelEnd = 0;
	CMyEdit::GetSel( nSelStart, nSelEnd );
	m_EdtPar.m_nSelNum = nSelEnd - nSelStart;
	if ( nSelStart == nSelEnd )
		m_EdtPar.m_nLastSelEnd = nSelEnd;
	int nCursor = nSelStart;
	CPoint pntCursor = this->PosFromChar( nSelStart );
	if ( pntCursor.x < 0 && pntCursor.x != -1 )
	{
		nCursor = nSelEnd;
		pntCursor = this->PosFromChar( nSelEnd );
	}
	else if ( pntCursor.x > 0 && nSelEnd > m_EdtPar.m_nLastSelEnd )
	{
		nCursor = nSelEnd;
		pntCursor = this->PosFromChar( nSelEnd );
	}

	if ( nSelEnd <= m_EdtPar.m_nLastSelEnd )
		m_EdtPar.m_nLastSelEnd = nSelEnd;

	int nTotalNum = strlen( strChar );
	int nCharNum = 0;
	int nStart = 0;
	if ( pntCursor.x == -1 )
	{
		if ( nTotalNum < m_EdtFnt.m_nCharNum )
		{
			nStart = 0;
			nCharNum = nTotalNum;
		}
		else
		{
			nStart = nCursor - m_EdtFnt.m_nCharNum;
			nCharNum = m_EdtFnt.m_nCharNum;
		}
	}
	else
	{
		nStart = nCursor - pntCursor.x / ( CMyBitmap::GetFontSize() / 2 );
		nCharNum = ( ( nTotalNum - nStart ) > m_EdtFnt.m_nCharNum ) ? m_EdtFnt.m_nCharNum : nTotalNum - nStart;
	}

	if ( nCharNum > 0 )
	{
		char strNorChar [128] = "";
		memset(strNorChar, 0L, sizeof(strNorChar));

		if ( m_EdtFnt.m_bPassword )
		{
			for ( int i = 0; i < nCharNum; i++ )
				strNorChar[i] = '*' ;
			strNorChar[nCharNum] = 0 ;
		}
		else
		{
			{
				int nStrIndex= 0;
				for(int i=0;i< strlen(strChar);i++)
				{
					if(nStrIndex == nStart)
						break;
					else if (nStrIndex > nStart)
					{
						nStart = nStrIndex;
						break;
					}
					if (strChar[i] < 0)
					{
						nStrIndex+=2;
					}
					else
						nStrIndex++;
				}
			}

			strcpy( strNorChar, strChar + nStart );
			strNorChar[nCharNum] = '\0';
		}

		CMyBitmap::ShowString ( m_EdtPar.m_Pnt.x,
								m_EdtPar.m_Pnt.y,
								m_EdtFnt.m_dwFontColor,
								strNorChar ) ;

		if ( ::timeGetTime() / 500 % 2 == 0 && this->GetFocus() == this )
		{
			CMyBitmap::ShowBlock ( m_EdtPar.m_Pnt.x + ( nCursor - nStart ) * ( CMyBitmap::GetFontSize() / 2 ),
				m_EdtPar.m_Pnt.y,
				m_EdtPar.m_Pnt.x + ( nCursor - nStart ) * ( CMyBitmap::GetFontSize() / 2 ) + 2,
				m_EdtPar.m_Pnt.y + CMyBitmap::GetFontSize() + 2,
				0xffffffff ) ;
		}


		if ( m_EdtPar.m_nSelNum > 0 )
		{
			char strSelChar [128] = "";
			memset(strSelChar, 0L, sizeof(strSelChar));

			if ( nSelStart < nStart )
			{
				m_EdtPar.m_nSelNum -= nStart - nSelStart;
				nSelStart = nStart ;
			}
			strncpy ( strSelChar,
					  strNorChar + ( nSelStart - nStart ),
					  m_EdtPar.m_nSelNum );
			strSelChar[m_EdtPar.m_nSelNum] = 0;

			if (m_EdtPar.m_nWidth > (( nSelStart - nStart + m_EdtPar.m_nSelNum ) * ( CMyBitmap::GetFontSize() / 2 )))
				CMyBitmap::ShowBlock( m_EdtPar.m_Pnt.x + ( nSelStart - nStart ) * ( CMyBitmap::GetFontSize() / 2 ),
				m_EdtPar.m_Pnt.y,
				m_EdtPar.m_Pnt.x + ( nSelStart - nStart + m_EdtPar.m_nSelNum ) * ( CMyBitmap::GetFontSize() / 2 ),
				m_EdtPar.m_Pnt.y + CMyBitmap::GetFontSize() + 2,
				0xffffffff );
			else
				CMyBitmap::ShowBlock( m_EdtPar.m_Pnt.x + ( nSelStart - nStart ) * ( CMyBitmap::GetFontSize() / 2 ),
				m_EdtPar.m_Pnt.y,
				m_EdtPar.m_Pnt.x + m_EdtPar.m_nWidth,
				m_EdtPar.m_Pnt.y + CMyBitmap::GetFontSize() + 2,
				0xffffffff );

			CMyBitmap::ShowString( m_EdtPar.m_Pnt.x + ( nSelStart - nStart ) * ( CMyBitmap::GetFontSize() / 2 ),
								   m_EdtPar.m_Pnt.y,
								   m_EdtFnt.m_dwBackColor,
								   strSelChar );
		}

	}
}

BOOL CMyEdit::Init(int xPos, int yPos, char *EditID, DWORD dwBkColor, DWORD dwFontColor, BOOL bMultiLine)
{
	g_objFontScale.Adjust ( this, GetDlgCtrlID () ) ;
	CMyEdit::SetLimitText ( 1024 ) ;
	CRect rect;
	CMyEdit::GetWindowRect ( rect ) ;
	m_EdtPar.m_bMultiLine = bMultiLine ;
	m_EdtPar.m_IntPnt.x = rect.left - xPos ;
	m_EdtPar.m_IntPnt.y = rect.top - yPos ;
	m_EdtPar.m_nWidth = rect.right - rect.left;
	m_EdtPar.m_nHeight = rect.bottom - rect.top ;
	m_EdtFnt.m_btFontWidth = CMyBitmap::GetFontSize () / 2 ;
	m_EdtFnt.m_nCharNum = m_EdtPar.m_nWidth / ( CMyBitmap::GetFontSize () / 2 );

	m_EdtFnt.m_dwBackColor = dwBkColor ;
	m_EdtFnt.m_dwFontColor = dwFontColor ;

	if ( EditID )
	{
		strcpy ( m_EdtPar.m_strEditID, EditID ) ;
	}

	if ( m_EditRect.m_hWnd == NULL )
	{
		this->MoveWindow( m_EdtPar.m_IntPnt.x, m_EdtPar.m_IntPnt.y, 0, 0, FALSE );
		CRect EditRect = CRect( m_EdtPar.m_IntPnt.x,
								m_EdtPar.m_IntPnt.y,
								m_EdtPar.m_IntPnt.x + rect.Width(),
								m_EdtPar.m_IntPnt.y + rect.Height() );
		m_EditRect.Create( "",
						   WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW,
						   EditRect,
						   this->GetParent(),
						   0 );
		m_EditRect.Init( this );
	}
	return true ;
}

void CMyEdit::SetFontColor(DWORD dwColor)
{
	m_EdtFnt.m_dwFontColor = dwColor ;
}

void CMyEdit::SetOffset(CPoint offsetPnt)
{
	m_EdtFnt.m_OffsetPoint = offsetPnt ;
}

void CMyEdit::SetPasswordType( BOOL bPassword )
{
	m_EdtFnt.m_bPassword = bPassword ;
}

void CMyEdit::SetMultiLine( BOOL bMultiLine )
{
	m_EdtPar.m_bMultiLine = bMultiLine ;
}

void CMyEdit::ReSize( int xPos, int yPos, int nWidth, int nHeight, BOOL bUnused )
{
	this->MoveWindow ( xPos, yPos, nWidth, nHeight, FALSE ) ;
	CRect rect ;
	m_EditRect.GetWindowRect ( rect ) ;
	m_EditRect.MoveWindow ( xPos, yPos, rect.Width (), rect.Height (), FALSE ) ;
	m_EdtPar.m_IntPnt.x = xPos ;
	m_EdtPar.m_IntPnt.y = yPos ;
	m_EdtPar.m_Pnt = m_EdtPar.m_IntPnt ;
	m_EdtPar.m_nWidth = rect.Width () ;
	m_EdtPar.m_nHeight = rect.Height () ;
	m_EdtFnt.m_nCharNum = m_EdtPar.m_nWidth / m_EdtFnt.m_btFontWidth ;
}

void CMyEdit::GoNextLine()
{
	if ( m_EdtPar.m_nHeight / CMyBitmap::GetFontSize () - 2 >= CMyEdit::GetLineCount () )
	{
		char strChar[256];
		char strTemp[256];
		CMyEdit::GetWindowText ( strChar, 256 ) ;
		int nS, nE ;
		CMyEdit::GetSel ( nS, nE ) ;
		strcpy ( strTemp, strChar + nS ) ;
		strcpy ( strChar + nS, "\r\n" ) ;
		strcpy ( strChar + nS + 2, strTemp ) ;
		CMyEdit::SetWindowText ( strChar ) ;
		CMyEdit::SetSel ( nS + 2, nS + 2 ) ;
	}
}

void CMyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( m_EdtPar.m_bMultiLine && nChar == 13 )
		CMyEdit::GoNextLine () ;

	if ( nFlags == 0 && !m_EdtPar.m_bChinese )
		return;

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CMyEdit::EnableChinese( BOOL bChinese )
{
	m_EdtPar.m_bChinese = bChinese;
}

void CMyEdit::OnEnable(BOOL bEnable)
{
	CEdit::OnEnable(bEnable);

	if ( !m_EdtPar.m_bMultiLine && m_EditRect.m_hWnd != NULL )
	{
		if ( bEnable )
			m_EditRect.EnableWindow();
		else
			m_EditRect.EnableWindow( false );
	}
}

void CMyEdit::ReInit(int xPos, int yPos)
{
	CRect rect;
	CMyEdit::GetWindowRect ( rect );
	m_EdtPar.m_IntPnt.x = rect.left - xPos;
	m_EdtPar.m_IntPnt.y = rect.top - yPos;
}

void CMyEdit::SetEditBkID(char *EditID)
{
	if ( EditID )
	{
		strcpy ( m_EdtPar.m_strEditID, EditID ) ;
	}
}

char* CMyEdit::GetEditBkID()
{
	return m_EdtPar.m_strEditID ;
}

void CMyEdit::ShowMultiLine()
{
	if ( m_EdtPar.m_bMultiLine )
	{
		int nSelStart, nSelEnd ;
		CMyEdit::GetSel ( nSelStart, nSelEnd ) ;
		int nLineCount = CMyEdit::GetLineCount () ;
		int nLineOffsetX = 0;
		int nLineOffset = 0 ;
		char strChar[256];
		char strShowChar[256] ;
		CMyEdit::GetWindowText ( strChar, 256 ) ;
		for ( int i = 0; i < nLineCount; i++ )
		{
			char strSelChar[256] ;
			CPoint SelPoint ;
			int nLineIndex = CMyEdit::LineIndex ( i ) ;
			strcpy ( strShowChar, strChar + nLineIndex ) ;
			int nNextIndex = CMyEdit::LineIndex ( i + 1 ) ;
			if ( nNextIndex != -1 )
				strShowChar [ nNextIndex - nLineIndex ] = '\0' ;
			CMyBitmap::ShowString ( m_EdtPar.m_Pnt.x + m_EdtFnt.m_OffsetPoint.x,
				m_EdtPar.m_Pnt.y + m_EdtFnt.m_OffsetPoint.y + nLineOffset,
				m_EdtFnt.m_dwFontColor,
				strShowChar ) ;

			if ( nSelEnd != nSelStart )
			{
				if (  nSelEnd > nLineIndex && ( nSelStart < nNextIndex || nNextIndex == -1 ) )
				{
					int nStart = ( nSelStart > nLineIndex ) ? nSelStart - nLineIndex : 0 ;
					strcpy ( strSelChar, strShowChar + nStart ) ;
					int nEnd = ( nSelEnd > nNextIndex ) ? 0 : nSelEnd - nLineIndex - nStart ;
					if ( nNextIndex == -1 )
						nEnd = nSelEnd - nLineIndex - nStart ;
					if ( 0 != nEnd )
						strSelChar [nEnd] = '\0' ;
					CMyBitmap::ShowString ( m_EdtPar.m_Pnt.x + m_EdtFnt.m_OffsetPoint.x + nStart * CMyBitmap::GetFontSize() / 2,
						m_EdtPar.m_Pnt.y + m_EdtFnt.m_OffsetPoint.y + nLineOffset,
						0xffff00,
						strSelChar ) ;
				}
			}
			nLineOffset += CMyBitmap::GetFontSize() ;
		}
		if ( ::timeGetTime() / 500 % 2 == 0 && this->GetFocus() == this )
		{
			CPoint pntCursor = this->PosFromChar ( nSelStart ) ;
			int nColumn = 0 ;
			if ( pntCursor.x == -1 )
				nColumn = strlen ( strShowChar ) ;
			else
			{
				LOGFONT lf ;
				this->GetFont ()->GetLogFont ( &lf ) ;
				nColumn = pntCursor.x / ( lf.lfHeight / 2 ) ;
			}
			CMyBitmap::ShowBlock ( m_EdtPar.m_Pnt.x + nColumn * CMyBitmap::GetFontSize() / 2 + 2,
				m_EdtPar.m_Pnt.y + nLineOffset - CMyBitmap::GetFontSize() + 4,
				m_EdtPar.m_Pnt.x + nColumn * CMyBitmap::GetFontSize() / 2 + 4,
				m_EdtPar.m_Pnt.y + nLineOffset + 2,
				0xffffffff ) ;
		}
	}
}

void CMyEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
}

void CMyEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);
	char strEdit[1024];
	int nCharNum = this->GetWindowText (strEdit,1024);
	CEdit::SetSel(nCharNum,nCharNum);
}

LRESULT CMyEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

	return CEdit::WindowProc(message, wParam, lParam);
}
