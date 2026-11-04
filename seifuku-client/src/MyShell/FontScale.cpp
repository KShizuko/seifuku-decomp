#include "stdafx.h"
#include "MyShell.h"
#include "FontScale.h"
#include "MyIni.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFontScale g_objFontScale ;

CFontScale::CFontScale()
{
	m_FontCell[0].nWidth = 6 ;		m_FontCell[0].nHeight = 11 ;
	m_FontCell[1].nWidth = 6 ;		m_FontCell[1].nHeight = 12 ;
	m_FontCell[2].nWidth = 7 ;		m_FontCell[2].nHeight = 13 ;
	m_FontCell[3].nWidth = 8 ;		m_FontCell[3].nHeight = 15 ;
	m_FontCell[4].nWidth = 8 ;		m_FontCell[4].nHeight = 16 ;
	m_FontCell[5].nWidth = 10 ;		m_FontCell[5].nHeight = 19 ;
	m_FontCell[6].nWidth = 11 ;		m_FontCell[6].nHeight = 21 ;
	m_fScaleX = 0 ;
	m_fScaleY = 0 ;
	m_bScale = false ;
	m_nUnused = 0 ;
	m_pIni = NULL ;
}

CFontScale::~CFontScale()
{
	SAFE_DELETE ( m_pIni ) ;
}

void CFontScale::Init(CWnd* pDlg, int nFontSize)
{
	if ( pDlg == NULL )
		return ;
	if ( nFontSize < 8 || nFontSize > 16 )
		return ;
	int nWidth = m_FontCell[nFontSize - 8].nWidth ;
	int nHeight = m_FontCell[nFontSize - 8].nHeight ;

	CRect rect ( 0, 0, 4, 8 ) ;
	::MapDialogRect ( pDlg->m_hWnd, &rect ) ;
	if ( nWidth == rect.right && nHeight == rect.bottom )
		return ;

	m_bScale = true ;
	m_fScaleX = ( float ) nWidth / rect.right ;
	m_fScaleY = ( float ) nHeight / rect.bottom ;

	CRect rcWnd ;
	pDlg->GetWindowRect ( rcWnd ) ;
	pDlg->MoveWindow ( rcWnd.left, rcWnd.top,
					   ( int ) ( rcWnd.Width() * m_fScaleX + 0.5f ),
					   ( int ) ( rcWnd.Height() * m_fScaleY + 0.5f ), FALSE ) ;
}

void CFontScale::Adjust(CWnd* pWnd, int nID)
{
	if ( pWnd == NULL )
		return ;
	CRect rect ;
	pWnd->GetWindowRect ( rect ) ;
	if ( m_bScale )
	{
		pWnd->MoveWindow ( rect.left, rect.top,
						   ( int ) ( rect.Width() * m_fScaleX + 0.5f ),
						   ( int ) ( rect.Height() * m_fScaleY + 0.5f ), FALSE ) ;
	}
	if ( nID <= 0 || m_pIni == NULL )
		return ;
	m_mapWndID[pWnd->GetSafeHwnd()] = nID ;
	int nParentID = 0 ;
	CWnd* pParent = pWnd->GetParent() ;
	if ( pParent )
	{
		std::map<HWND, int>::iterator it = m_mapWndID.find ( pParent->GetSafeHwnd() ) ;
		if ( it != m_mapWndID.end() )
			nParentID = it->second ;
	}
	char szSection[256] = "" ;
	sprintf ( szSection, "%d-%d", nParentID, nID ) ;
	if ( !m_pIni->HasSection ( szSection ) )
		return ;
	int nX = m_pIni->GetInt ( szSection, "x" ) ;
	int nY = m_pIni->GetInt ( szSection, "y" ) ;
	int nWidth = m_pIni->GetInt ( szSection, "w" ) ;
	int nHeight = m_pIni->GetInt ( szSection, "h" ) ;
	if ( nX == -1 )	nX = rect.left ;
	if ( nY == -1 )	nY = rect.top ;
	if ( nWidth == -1 )	nWidth = rect.Width() ;
	if ( nHeight == -1 )	nHeight = rect.Height() ;
	pWnd->MoveWindow ( nX, nY, nWidth, nHeight, FALSE ) ;
}

void CFontScale::LoadIni()
{
	m_pIni = CMyIni::CreateNew ( "ini/GUI.ini", false ) ;
}
