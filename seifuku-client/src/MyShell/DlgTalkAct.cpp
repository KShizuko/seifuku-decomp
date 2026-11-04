#include "stdafx.h"
#include "MyShell.h"
#include "DlgTalkAct.h"

#include "3DRoleData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgTalkAct::CDlgTalkAct(CWnd* pParent )
	: CDialog(CDlgTalkAct::IDD, pParent)
{
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0 ) ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_nSpace = 0 ;
	m_nCurIndex = 0;
	m_nCurAmount = 0;
	m_pParentCWnd = NULL;
}


void CDlgTalkAct::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TALKACT_BTN_PAGEUP, m_TalkActBtnPageUp);
	DDX_Control(pDX, IDC_TALKACT_BTN_PAGEDOWN, m_TalkActBtnPageDown);
	DDX_Control(pDX, IDC_TALKACT_BTN_HELP, m_TalkActBtnHelp);
	DDX_Control(pDX, IDC_TALKACT_BTN_CLOSEB, m_TalkActBtnCloseB);
}


BEGIN_MESSAGE_MAP(CDlgTalkAct, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_TALKACT_BTN_HELP, OnTalkActBtnHelp)
	ON_BN_CLICKED(IDC_TALKACT_BTN_CLOSEB, OnTalkActBtnCloseB)
	ON_BN_CLICKED(IDC_TALKACT_BTN_PAGEUP, OnTalkActBtnPageUp)
	ON_BN_CLICKED(IDC_TALKACT_BTN_PAGEDOWN, OnTalkActBtnPageDown)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

LRESULT CDlgTalkAct::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
	}
	else if ( message == WM_COMMAND )
	{
		if ( wParam == 1 || wParam == 2 )
			return true ;
	}
	else if ( message == WM_DESTROY )
	{
		::DeleteObject ( m_Hbr ) ;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

HBRUSH CDlgTalkAct::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgTalkAct::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgTalkAct::GetWindowRect ( rect ) ;
	m_nSpace = ( int ) ( ( rect.Width () / 8.0f - 20.0f ) / 2.0f ) ;
	m_nCurAmount = ( GameDataSetQuery()->GetEmotionAmount () > 32 ) ? 32 : GameDataSetQuery()->GetEmotionAmount () ;

	m_TalkActBtnHelp.Init ( rect.left, rect.top, "Button444" ) ;
	m_TalkActBtnCloseB.Init ( rect.left, rect.top, "Button333" ) ;
	m_TalkActBtnPageUp.Init ( rect.left, rect.top, "Button500" ) ;
	m_TalkActBtnPageDown.Init ( rect.left, rect.top, "Button501" ) ;

	return TRUE;
}

void CDlgTalkAct::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgTalkAct::Show()
{
	if ( m_bShow )
	{
		IAni* pShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													   "Dialog50",
													   EXIGENCE_IMMEDIATE ) ;
		if ( pShowAni != NULL )
			pShowAni->Show ( 0, m_Pnt.x + 3, m_Pnt.y - 5 );
		else
			return ;

		m_TalkActBtnHelp.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TalkActBtnCloseB.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TalkActBtnPageUp.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TalkActBtnPageDown.Show ( m_Pnt.x, m_Pnt.y ) ;

		for ( int i = 0; i < m_nCurAmount; i++ )
		{
			char strIcon[32] ;
			sprintf ( strIcon, "ICON%d", m_nCurIndex + i ) ;
			IAni* pIcon = GameDataSetQuery()->GetDataAni ( "ani/EmotionIco.ani", strIcon ) ;
			if ( pIcon != NULL )
			{
				pIcon->Show ( ( TimeGet () / 300 ) % pIcon->GetFrameAmount (),
							  m_Pnt.x + m_nSpace + ( i % 8 ) * ( 2 * m_nSpace + 20 ),
							  m_Pnt.y + ( i / 8 ) * 20 ) ;
			}
		}
	}
}

void CDlgTalkAct::OnTalkActBtnHelp()
{
	CDlgTalkAct::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_TALKACT ) ;
}

void CDlgTalkAct::OnTalkActBtnCloseB()
{
	CDlgTalkAct::ShowWindow ( SW_HIDE ) ;
}

void CDlgTalkAct::OnTalkActBtnPageUp()
{
	if ( m_nCurIndex > 0 )
	{
		m_nCurIndex -= 32 ;
		m_nCurAmount = ( GameDataSetQuery()->GetEmotionAmount () > m_nCurIndex + 32 ) ? 32 : GameDataSetQuery()->GetEmotionAmount () - m_nCurIndex;
	}
}

void CDlgTalkAct::OnTalkActBtnPageDown()
{
	if ( m_nCurIndex + 32 < GameDataSetQuery()->GetEmotionAmount () )
	{
		m_nCurIndex += 32 ;
		m_nCurAmount = ( GameDataSetQuery()->GetEmotionAmount () > m_nCurIndex + 32 ) ? 32 : GameDataSetQuery()->GetEmotionAmount () - m_nCurIndex;
	}
}

void CDlgTalkAct::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nRow = point.y / 20 ;
	int nLine = point.x / ( 2 * m_nSpace + 20 ) ;
	int nIndex = nRow * 8 + nLine ;
	if ( nIndex >= m_nCurAmount )
		return ;

	if ( m_pParentCWnd )
		m_pParentCWnd->PostMessage ( WM_MY_MESSAGE, ACTION_CHOOSE, nIndex + m_nCurIndex ) ;
}

void CDlgTalkAct::SetParentCWnd(CWnd *ParentCWnd)
{
	m_pParentCWnd = ParentCWnd ;
}
