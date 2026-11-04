#include "stdafx.h"
#include "MyShell.h"
#include "DlgLogo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgLogo::CDlgLogo(CWnd* pParent )
	: CDialog(CDlgLogo::IDD, pParent)
{
	m_hBmp = NULL ;
	m_Hbr = NULL ;
}


void CDlgLogo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgLogo, CDialog)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CDlgLogo::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Init(this, 12) ;

	char strName[32] ;
	sprintf ( strName, "Data/Main/Logo%d.bmp", TimeGet() % 2 + 1) ;
	HBITMAP& hBmp = m_hBmp ;
	hBmp =  (HBITMAP )::LoadImage ( AfxGetInstanceHandle(),
									strName,
									IMAGE_BITMAP,
									0,
									0,
									LR_LOADFROMFILE ) ;
	m_Hbr = CreatePatternBrush ( hBmp ) ;

	return TRUE;
}

void CDlgLogo::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_Hbr)
		DeleteObject( m_Hbr ) ;
	if (m_hBmp)
		DeleteObject( m_hBmp ) ;
}

HBRUSH CDlgLogo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;

	if ( nCtlColor == CTLCOLOR_DLG )
		return m_Hbr ;
	return NULL ;
}
