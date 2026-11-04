#include "stdafx.h"
#include "MyShell.h"
#include "MyList.h"

#define		MY_LISTBOX	0
#define		MY_COMBOBOX 1

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMyList::CMyList()
{
	m_LstFnt.m_dwFontColor = 0x000000 ;
	m_LstPar.m_OffsetPoint= CPoint ( 0, 0 ) ;
	m_LstPar.m_bVShow = false ;
	m_LstPar.m_btListCount = 0 ;
	m_LstPar.m_nLastClkIndex = -1 ;
	m_LstPar.m_ParentCob = NULL ;
	m_LstPar.m_strListID[0] = 0 ;
	for ( int i = 0; i < 64; i++ )
	{
		m_LstPar.m_nListRealIndex[i] = -1 ;
	}
}

CMyList::~CMyList()
{
}


BEGIN_MESSAGE_MAP(CMyList, CListBox)
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CMyList::Show(int x, int y)
{
	m_LstPar.m_ListPoint.x = m_LstPar.m_IntPoint.x + x ;
	m_LstPar.m_ListPoint.y = m_LstPar.m_IntPoint.y + y ;

	if ( m_LstPar.m_strListID [0] != 0 )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * ) g_strControlAni,
													  m_LstPar.m_strListID,
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( 0,
							  m_LstPar.m_ListPoint.x + m_LstPar.m_OffsetPoint.x,
							  m_LstPar.m_ListPoint.y + m_LstPar.m_OffsetPoint.y) ;
	}
	if ( m_LstPar.m_btListCount  > 0 )
	{
		ShowSel() ;
		ShowChar () ;
	}
	if ( m_LstPar.m_bVShow )
	{
		m_LstScr.m_ScrUpBtn.Show ( x + m_LstScr.m_ScrUpRect.left,
								   y + m_LstScr.m_ScrUpRect.top ) ;
		m_LstScr.m_ScrDoBtn.Show ( x + m_LstScr.m_ScrUpRect.left,
								   y + m_LstScr.m_ScrUpRect.top + m_LstPar.m_nHeight - 19 ) ;
	}
}

void CMyList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}

void CMyList::ShowSel()
{
	m_LstPar.m_nTopIndex = CMyList::GetTopIndex () ;
	m_LstPar.m_nCurIndex = CMyList::GetCurSel () ;
	if ( m_LstPar.m_nCurIndex >= m_LstPar.m_nTopIndex
		 && m_LstPar.m_nCurIndex < m_LstPar.m_nTopIndex + m_LstPar.m_btListLineNum )
	{
		CMyList::GetItemRect ( m_LstPar.m_nCurIndex, m_LstPar.m_SelRect ) ;
		CMyBitmap::ShowBlock ( m_LstPar.m_ListPoint.x,
							   m_LstPar.m_ListPoint.y + m_LstPar.m_SelRect.top,
							   m_LstPar.m_ListPoint.x + m_LstPar.m_SelRect.right,
							   m_LstPar.m_ListPoint.y + m_LstPar.m_SelRect.bottom,
							   0xff000080 ) ;
	}
}

void CMyList::ShowChar()
{
	char strChar [1024] ;
	int i ;
	for ( i = m_LstPar.m_nTopIndex;
		  i< m_LstPar.m_nTopIndex + m_LstPar.m_btShowLineNum;
		  i++ )
	{
		CMyList::GetText ( i, strChar ) ;
		CMyList::GetItemRect ( i, m_LstPar.m_ItemRect ) ;
		strChar [m_LstFnt.m_btCharNum] = 0 ;
		if ( m_LstPar.m_btType == COMPLEX_LIST )
		{
			if ( m_LstPar.m_nListRealIndex [i] != -1 )
				CMyBitmap::ShowString ( m_LstPar.m_ListPoint.x,
										m_LstPar.m_ListPoint.y + m_LstPar.m_ItemRect.top,
										0xffffff,
										strChar ) ;
			else
				CMyBitmap::ShowString ( m_LstPar.m_ListPoint.x,
										m_LstPar.m_ListPoint.y + m_LstPar.m_ItemRect.top,
										0xff0000,
										strChar ) ;
		}
		else
			CMyBitmap::ShowString ( m_LstPar.m_ListPoint.x,
									m_LstPar.m_ListPoint.y + m_LstPar.m_ItemRect.top,
									m_LstFnt.m_dwFontColor,
									strChar ) ;
	}
	if ( m_LstPar.m_nCurIndex >= m_LstPar.m_nTopIndex && m_LstPar.m_nCurIndex < i )
	{
		CMyList::GetText ( m_LstPar.m_nCurIndex, strChar ) ;
		strChar [m_LstFnt.m_btCharNum] = 0 ;
		CMyBitmap::ShowString ( m_LstPar.m_ListPoint.x,
			                    m_LstPar.m_ListPoint.y + m_LstPar.m_SelRect.top,
								0xffffff,
								strChar ) ;
	}
}

void CMyList::ShowVScroll()
{

}


void CMyList::SetFontColor(DWORD dwColor)
{
	m_LstFnt.m_dwFontColor = dwColor ;
}



void CMyList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
}




BOOL CMyList::Init(int xPos, int yPos, char *ListID, BYTE btType)
{
	g_objFontScale.Adjust ( this, GetDlgCtrlID () ) ;
	CRect rect;
	CRgn rgn ;
	CMyList::GetWindowRect ( rect ) ;
	m_LstPar.m_IntPoint.x = rect.left - xPos ;
	m_LstPar.m_IntPoint.y = rect.top - yPos ;
	if ( g_objFontScale.IsScale () )
	{
		m_LstPar.m_IntPoint.x = (int) ( m_LstPar.m_IntPoint.x * g_objFontScale.GetScaleX () + 0.5f ) ;
		m_LstPar.m_IntPoint.y = (int) ( m_LstPar.m_IntPoint.y * g_objFontScale.GetScaleY () + 0.5f ) ;
		int nW = (int) ( rect.Width () * g_objFontScale.GetScaleX () + 0.5f ) ;
		int nH = (int) ( rect.Height () * g_objFontScale.GetScaleY () + 0.5f ) ;
		CMyList::MoveWindow ( m_LstPar.m_IntPoint.x, m_LstPar.m_IntPoint.y, nW, nH, FALSE ) ;
		CMyList::GetWindowRect ( rect ) ;
	}
	m_LstPar.m_nHeight = rect.bottom - rect.top ;
	m_LstPar.m_nWidth = rect.right - rect.left ;
	m_LstFnt.m_btFontHeight = CMyBitmap::GetFontSize () + 2 ;
	m_LstFnt.m_btCharNum = m_LstPar.m_nWidth / ( CMyBitmap::GetFontSize () / 2 ) ;
	m_LstPar.m_btListLineNum = ( m_LstPar.m_nHeight ) / m_LstFnt.m_btFontHeight ;
	m_LstPar.m_btType = btType ;

	if ( ListID )
	{
		strcpy ( m_LstPar.m_strListID, ListID ) ;
	}

	rgn.CreateRectRgn ( 0, 0, m_LstPar.m_nWidth - 16, m_LstPar.m_nHeight  ) ;
	CMyList::SetWindowRgn ( rgn, true ) ;

	m_LstScr.m_ScrUpRect = CRect ( m_LstPar.m_IntPoint.x + m_LstPar.m_nWidth - 16,
								   m_LstPar.m_IntPoint.y,
								   m_LstPar.m_IntPoint.x + m_LstPar.m_nWidth,
								   m_LstPar.m_IntPoint.y + 16 ) ;
	m_LstScr.m_ScrDoRect = CRect ( m_LstPar.m_IntPoint.x + m_LstPar.m_nWidth - 16,
								   m_LstPar.m_IntPoint.y + m_LstPar.m_nHeight - 19,
								   m_LstPar.m_IntPoint.x + m_LstPar.m_nWidth,
								   m_LstPar.m_IntPoint.y + m_LstPar.m_nHeight - 3) ;

	m_LstScr.m_ScrUpBtn.Create ( "",
								 WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW,
								 m_LstScr.m_ScrUpRect,
								 CMyList::GetParent (),
								 1 );
	m_LstScr.m_ScrUpBtn.Init( this, MYSCROLL_UP ) ;
	m_LstScr.m_ScrDoBtn.Create ( "",
								 WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW,
								 m_LstScr.m_ScrDoRect,
								 CMyList::GetParent (),
								 2 );
	m_LstScr.m_ScrDoBtn.Init( this, MYSCROLL_DOWN ) ;
	m_LstScr.m_ScrUpBtn.Load ( "Button173" ) ;
	m_LstScr.m_ScrDoBtn.Load ( "Button174" ) ;

	return true ;
}

void CMyList::SetOffset(CPoint offsetPnt)
{
	m_LstPar.m_OffsetPoint = offsetPnt ;
}

LRESULT CMyList::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == LB_ADDSTRING || message == LB_INSERTSTRING || message == LB_DELETESTRING || message == LB_RESETCONTENT )
	{
		LRESULT result = CListBox::WindowProc ( message, wParam, lParam ) ;
		SetParameter() ;
		return result ;
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
	return CListBox::WindowProc(message, wParam, lParam);
}

void CMyList::SetParameter()
{
	m_LstPar.m_nLastClkIndex = -1 ;
	m_LstPar.m_btListCount = CMyList::GetCount () ;
	if ( m_LstPar.m_btListLineNum >= m_LstPar.m_btListCount )
	{
		m_LstPar.m_btShowLineNum = m_LstPar.m_btListCount ;
	}
	else
	{
		m_LstPar.m_btShowLineNum = m_LstPar.m_btListLineNum ;
		m_LstPar.m_bVShow = true ;
	}
}

void CMyList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if ( m_LstPar.m_btType == SIMPLE_LIST )
	{
		int nCurIndex = CMyList::GetCurSel () ;
		if ( nCurIndex != -1 && m_LstPar.m_ParentCob != NULL )
			m_LstPar.m_ParentCob->PostMessage ( CB_GETITEMDATA,
												true,
												nCurIndex ) ;
	}
	else if ( m_LstPar.m_btType == COMPLEX_LIST )
	{
		int nCurIndex = CMyList::GetCurSel () ;
		if ( nCurIndex != -1 && m_LstPar.m_ParentCob != NULL )
			if ( m_LstPar.m_nListRealIndex[nCurIndex] != -1 )
				m_LstPar.m_ParentCob->PostMessage ( LB_SETSEL,
													1,
													nCurIndex ) ;
			else
				m_LstPar.m_ParentCob->PostMessage ( LB_SETSEL,
													2,
													nCurIndex ) ;

	}
	CListBox::OnLButtonDblClk(nFlags, point);
}

void CMyList::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListBox::OnLButtonDown ( nFlags, point) ;
	if ( m_LstPar.m_btType == SIMPLE_LIST )
	{
		int nCurIndex = CMyList::GetCurSel () ;
		if ( nCurIndex != -1 && m_LstPar.m_ParentCob != NULL )
			m_LstPar.m_ParentCob->PostMessage ( CB_GETITEMDATA,
												NULL,
												nCurIndex ) ;
	}
	else if ( m_LstPar.m_btType == COMPLEX_LIST )
	{
		int nCurIndex = CMyList::GetCurSel () ;
		if ( nCurIndex != -1 && m_LstPar.m_ParentCob != NULL )
		{
			if ( m_LstPar.m_nLastClkIndex != nCurIndex )
			{
				if ( m_LstPar.m_nListRealIndex[nCurIndex] != -1 )
				{
					m_LstPar.m_nLastClkIndex = nCurIndex ;
					m_LstPar.m_ParentCob->PostMessage ( LB_SETSEL,
														0,
														nCurIndex ) ;
				}
				else
				{
					m_LstPar.m_nLastClkIndex = nCurIndex ;
					m_LstPar.m_ParentCob->PostMessage ( LB_SETSEL,
														-1,
														nCurIndex ) ;
				}
			}
			else
			{
				static int bDouble = false ;
				if ( bDouble )
				{
					if ( m_LstPar.m_nListRealIndex[nCurIndex] != -1 )
					{
						m_LstPar.m_nLastClkIndex = nCurIndex ;
						m_LstPar.m_ParentCob->PostMessage ( LB_SETSEL,
															0,
															nCurIndex ) ;
					}
					else
					{
						m_LstPar.m_nLastClkIndex = nCurIndex ;
						m_LstPar.m_ParentCob->PostMessage ( LB_SETSEL,
															-1,
															nCurIndex ) ;
					}
				}
				else
				{
					m_LstPar.m_nLastClkIndex = nCurIndex ;
					m_LstPar.m_ParentCob->PostMessage ( LB_SETSEL,
														-1,
														-1 ) ;
				}
				bDouble = !bDouble ;
			}
		}
	}
}

void CMyList::SetParentCob(CWnd *ParentCob)
{
	m_LstPar.m_ParentCob = ParentCob ;
}

void CMyList::SetListRealIndex(int nIndex, int nRealIndex)
{
	if ( nIndex < 64 )
		m_LstPar.m_nListRealIndex [nIndex] = nRealIndex ;
}

int CMyList::GetListRealIndex(int nIndex)
{
	if ( nIndex < 64 )
		return m_LstPar.m_nListRealIndex [nIndex] ;

	return -1 ;
}
