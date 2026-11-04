#include "stdafx.h"
#include "MyShell.h"
#include "MyStatic.h"
#include <vector>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MY_STATIC		0
#define MY_PICTURE		1
#define MY_MULTILINE	2
#define MY_TITLE		3

const int POS_LEFT	 = 0;
const int POS_MIDDLE = 1;
const int POS_RIGHT  = 2;

static int WrapLine(const char* pszText, int* pnPos, char* pszLine, int nMax)
{
	if (!pszText || !pszLine || nMax < 1)
		return 0;
	int nLen = strlen(pszText);
	if (*pnPos >= nLen || nLen <= 0)
		return 0;
	memset(pszLine, 0, nMax);
	strncpy(pszLine, pszText + *pnPos, nMax - 1);
	int n = strlen(pszLine);
	if (n < nMax - 1)
	{
		*pnPos += n;
		return n;
	}
	if (*pnPos + n >= nLen)
	{
		*pnPos += n;
		return n;
	}
	switch (pszLine[n-1])
	{
	case ' ': case '!': case '\"': case '$': case '&': case '\'': case '(': case ')': case '*': case '+': case ',': case '-': case '.':
	case '/': case ';': case '<': case '>': case '?': case '@': case '[': case '\\': case ']': case '^': case '_': case '{': case '|':
	case '}': case '~':
		*pnPos += n;
		return n;
	}
	if (IsDBCSLeadByte(pszLine, n - 1))
	{
		pszLine[n-1] = 0;
		n--;
		*pnPos += n;
		return n;
	}
	if (IsDBCSLeadByte(pszLine, n - 2))
	{
		*pnPos += n;
		return n;
	}
	if (pszLine[n-1] == '#')
	{
		pszLine[n-1] = 0;
		n--;
		*pnPos += n;
		return n;
	}
	if (pszLine[n-1] >= '0' && pszLine[n-1] <= '9')
	{
		if (n > 1 && pszLine[n-2] == '#')
		{
			pszLine[n-2] = 0;
			*pnPos += n - 2;
			return n - 2;
		}
		if (n > 2 && pszLine[n-2] >= '0' && pszLine[n-2] <= '9' && pszLine[n-3] == '#')
		{
			*pnPos += n;
			return n;
		}
	}
	char cNext = pszText[*pnPos + n];
	if (cNext == ' ' || cNext == ',' || cNext == ';' || cNext == '.' || cNext == '!' || cNext == '?' || cNext == ':')
	{
		*pnPos += n;
		return n;
	}
	char* pszSpace = strrchr(pszLine, ' ');
	if (!pszSpace)
	{
		*pnPos += n;
		return n;
	}
	pszSpace[1] = 0;
	n = strlen(pszLine);
	*pnPos += n;
	return n;
}

CMyStatic::CMyStatic()
{
	m_dwFontColor = 0xffffff ;
	m_bImageLoad = false ;
	m_nLightType = -1 ;
	m_uFrame = 0 ;
	m_nFontType = 0;
	m_nStaticPosType = POS_LEFT;
	m_ptBkOffset.x = 2;
	m_ptBkOffset.y = 2;
	m_szFontName[0] = 0;
}

CMyStatic::~CMyStatic()
{
}


BEGIN_MESSAGE_MAP(CMyStatic, CStatic)
END_MESSAGE_MAP()

BOOL CMyStatic::Init(int xPos, int yPos, BYTE nType, char *PictureID, char *pszBkAni)
{
	g_objFontScale.Adjust ( this, GetDlgCtrlID () ) ;
	CRect rect ;
	CMyStatic::GetWindowRect ( rect ) ;
	m_IntPoint.x = rect.left - xPos ;
	m_IntPoint.y = rect.top - yPos ;
	m_nStaticWidth = rect.Width();
	if ( pszBkAni )
		strcpy ( m_szBkAni, pszBkAni ) ;
	else
		m_szBkAni[0] = 0 ;
	m_ptBkSrc.x = 0 ;
	m_ptBkSrc.y = 0 ;
	m_nBkWidth = rect.Width () ;
	m_nBkHeight = rect.Height () ;
	if ( nType == MY_TITLE )
	{
		m_nFontType = 1 ;
		m_nStaticPosType = POS_MIDDLE ;
		m_dwFontColor = 0xC69608 ;
		strcpy ( m_szFontName, GameDataSetQuery()->GetStr ( 50000 ) ) ;
	}
	else
		m_szFontName[0] = 0 ;
	if ( g_objFontScale.IsScale () )
	{
		m_IntPoint.x = (int) ( m_IntPoint.x * g_objFontScale.GetScaleX () + 0.5f ) ;
		m_IntPoint.y = (int) ( m_IntPoint.y * g_objFontScale.GetScaleY () + 0.5f ) ;
		int nW = (int) ( rect.Width () * g_objFontScale.GetScaleX () + 0.5f ) ;
		int nH = (int) ( rect.Height () * g_objFontScale.GetScaleY () + 0.5f ) ;
		CMyStatic::MoveWindow ( m_IntPoint.x, m_IntPoint.y, nW, nH, FALSE ) ;
		CMyStatic::GetWindowRect ( rect ) ;
	}

	m_nNum = ( float ) ( rect.right - rect.left ) / 7.5f;

	m_btType = nType ;
	if ( nType == MY_PICTURE && PictureID != NULL )
	{
		strcpy ( m_strPictureID, PictureID ) ;
	}
	CMyStatic::ShowWindow ( SW_HIDE ) ;
	return true ;
}

void CMyStatic::Show(int x, int y)
{
	if ( m_szBkAni[0] != 0 && strcmp ( m_szBkAni, "NULL" ) != 0 )
	{
		IAni* pBkAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														m_szBkAni,
														EXIGENCE_IMMEDIATE ) ;
		if ( pBkAni != NULL )
		{
			CMyBitmap* pFrame = pBkAni->GetFrame ( 0 ) ;
			if ( pFrame != NULL )
			{
				CRect rect ( 0, 0, pFrame->GetWidth (), pFrame->GetHeight () ) ;
				pBkAni->ShowEx ( 0,
								 m_IntPoint.x + x - m_ptBkOffset.x,
								 m_IntPoint.y + y - m_ptBkOffset.y,
								 rect, m_nBkWidth, m_nBkHeight, 0, 0 ) ;
			}
		}
	}

	switch ( m_btType )
	{
	case MY_PICTURE:
		{
			IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														m_strPictureID,
														EXIGENCE_IMMEDIATE ) ;
			if ( ShowAni != NULL )
			{
				if (m_bImageLoad )
					ShowAni->Show ( 1,
									m_IntPoint.x + x,
									m_IntPoint.y + y);
				else
					ShowAni->Show ( 0,
									m_IntPoint.x + x,
									m_IntPoint.y + y);
			}
		}
		return ;
	case MY_STATIC:
	case MY_TITLE:
		break ;
	default:
		return ;
	}

	char Static[1024] = "" ;
	CMyStatic::GetWindowText ( Static, 1024 ) ;

	CMySize psizeFont;
	CMyBitmap::GetFontSize(psizeFont);

	std::vector<std::string> setLine ;
	int nPos = 0 ;
	int nMaxChar = ( m_nStaticWidth - psizeFont.iHeight ) / ( psizeFont.iWidth < 1 ? 1 : psizeFont.iWidth ) ;
	while ( true )
	{
		char szLine[256] = "" ;
		if ( WrapLine ( Static, &nPos, szLine, nMaxChar ) == 0 )
			break ;
		setLine.push_back ( std::string ( szLine ) ) ;
	}

	int nFontSize = 14 ;
	switch ( m_nFontType )
	{
	case 0:	nFontSize = 12 ; break ;
	case 1:	nFontSize = 14 ; break ;
	case 2:	nFontSize = 18 ; break ;
	case 3:	nFontSize = 12 ; break ;
	}
	const char* pszFontName = m_szFontName[0] != 0 ? m_szFontName : NULL ;

	int posBX = 0;
	switch(m_nStaticPosType)
	{
	case POS_LEFT:
		posBX = 0;
		CMyBitmap::ShowEmotionString ( m_IntPoint.x + x + posBX,
									   m_IntPoint.y + y,
									   m_dwFontColor,
									   Static, (IDataIcon*) GameDataSetQuery()->QueryInterface(0), pszFontName, nFontSize, 0, 0, 0 ) ;
		break;
	case POS_MIDDLE:
		{
			int nLine = 0 ;
			for ( std::vector<std::string>::iterator it = setLine.begin(); it != setLine.end(); it++, nLine++ )
			{
				int nLength = it->size()*(psizeFont.iWidth+m_nFontType);
				posBX = (m_nStaticWidth - nLength)/2;
				CMyBitmap::ShowEmotionString ( m_IntPoint.x + x + posBX,
											   m_IntPoint.y + y + nLine * (int) CMyBitmap::GetFontSize (),
											   m_dwFontColor,
											   it->c_str(), (IDataIcon*) GameDataSetQuery()->QueryInterface(0), pszFontName, nFontSize, 0, 0, 0 ) ;
			}
		}
		break;
	case POS_RIGHT:
		{
			int nLength = strlen(Static)*(psizeFont.iWidth+m_nFontType);
			posBX = m_nStaticWidth - nLength;
			CMyBitmap::ShowEmotionString ( m_IntPoint.x + x + posBX,
										   m_IntPoint.y + y,
										   m_dwFontColor,
										   Static, (IDataIcon*) GameDataSetQuery()->QueryInterface(0), pszFontName, nFontSize, 0, 0, 0 ) ;
		}
		break;
	default:
		break;
	}
	if ( m_nLightType != -1 )
	{
		char strLight[32] ;
		sprintf ( strLight, "Light%d", m_nLightType ) ;
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  strLight,
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
			ShowAni->Show ( m_uFrame,
							m_IntPoint.x + x - 30,
							m_IntPoint.y + y - 6) ;
	}
}

void CMyStatic::SetPictureID(const char* pszID)
{
	if ( m_btType == MY_PICTURE )
		strcpy ( m_strPictureID, pszID ) ;
}

void CMyStatic::SetFontName(int nType)
{
	if ( nType == 0 )
		strcpy ( m_szFontName, GameDataSetQuery()->GetStr ( 50000 ) ) ;
	else if ( nType == 1 )
		strcpy ( m_szFontName, GameDataSetQuery()->GetStr ( 10049 ) ) ;
	else if ( nType == 2 )
		strcpy ( m_szFontName, GameDataSetQuery()->GetStr ( 160023 ) ) ;
}

void CMyStatic::SetFontColor(DWORD dwColor)
{
	m_dwFontColor = dwColor ;
}

void CMyStatic::InsertImage()
{
	m_bImageLoad = true ;
}

void CMyStatic::RemoveImage()
{
	m_bImageLoad = false ;
}

void CMyStatic::SetLightType( int nType )
{
	m_nLightType = nType ;
}

void CMyStatic::SetCurFrame(UINT uFrame)
{
	m_uFrame = uFrame ;
}

LRESULT CMyStatic::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

	return CStatic::WindowProc(message, wParam, lParam);
}
