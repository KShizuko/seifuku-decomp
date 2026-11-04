#include "stdafx.h"
#include "MyShell.h"
#include "DlgConfirm.h"
#include "ND_Bitmap.h"

#include "Hero.h"
#include "GameMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const float _DLG_WIDTH = 110.0;

static int s_nEdtChanged = 0 ;

CDlgConfirm::CDlgConfirm(CWnd* pParent )
	: CDialog(CDlgConfirm::IDD, pParent)
{
	m_bShow = false ;
	m_bInitSuc = false ;
	m_Hbr = CreateSolidBrush ( 0 ) ;
	m_nEdtNum = 0 ;
	m_nEdtNum2 = 0 ;
	m_dwShowTime = 0 ;
}


void CDlgConfirm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONFIRM_BTN_EXERCISE, m_ConfirmBtnExercise ) ;
	DDX_Control(pDX, IDC_CONFIRM_STA_TEXT, m_ConfirmStaText ) ;
	DDX_Control(pDX, IDC_CONFIRM_EDT_DATA2, m_ConfirmEdtData2 ) ;
	DDX_Control(pDX, IDC_CONFIRM_STA_DATA2, m_ConfirmStaData2 ) ;
	DDX_Control(pDX, IDC_CONFIRM_STA_DATA, m_ConfirmStaData ) ;
	DDX_Control(pDX, IDC_CONFIRM_EDT_DATA, m_ConfirmEdtData);
	DDX_Control(pDX, IDC_CONFIRM_BTN_OK, m_ConfirmBtnOk ) ;
	DDX_Control(pDX, IDC_CONFIRM_BTN_CANCEL, m_ConfirmBtnCancel ) ;
}


BEGIN_MESSAGE_MAP(CDlgConfirm, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_CONFIRM_BTN_OK, OnConfirmBtnOk)
	ON_BN_CLICKED(IDC_CONFIRM_BTN_CANCEL, OnConfirmBtnCancel)
	ON_EN_CHANGE(IDC_CONFIRM_EDT_DATA, OnChangeConfirmEdtData)
	ON_EN_CHANGE(IDC_CONFIRM_EDT_DATA2, OnChangeConfirmEdtData2)
	ON_BN_CLICKED(IDC_CONFIRM_BTN_EXERCISE, OnConfirmBtnExercise)
END_MESSAGE_MAP()

HBRUSH CDlgConfirm::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgConfirm::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgConfirm::GetWindowRect ( rect ) ;

	m_nWidth = rect.Width () ;
	m_nHeight = rect.Height () ;
	m_nLine = 0 ;
	m_nLineNum = 0 ;
	IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
												  "Dialog15",
												  EXIGENCE_IMMEDIATE ) ;
	CMySize sizeAni = ShowAni->GetSize ( 0 ) ;
	m_nAniWidth = sizeAni.iWidth ;
	m_nAniHeight = sizeAni.iHeight ;

	m_ConfirmEdtData.Init ( rect.left, rect.top ) ;
	m_ConfirmEdtData.SetLimitText ( 11 ) ;
	m_ConfirmEdtData2.Init ( rect.left, rect.top ) ;
	m_ConfirmEdtData2.SetLimitText ( 11 ) ;

	if ( ! ( m_ConfirmBtnOk.Init ( rect.left, rect.top, "Button150" ) ) )
		return false ;
	if ( ! ( m_ConfirmBtnCancel.Init ( rect.left, rect.top, "Button151" ) ) )
		return false ;
	if ( ! ( m_ConfirmBtnExercise.Init ( rect.left, rect.top, "Exercise" ) ) )
		return false ;
	m_ConfirmBtnExercise.EnableWindow ( false ) ;
	m_ConfirmStaData.Init ( rect.left, rect.top, 1, "Dialog56", "NULL" ) ;
	m_ConfirmStaData2.Init ( rect.left, rect.top, 1, "Dialog57", "NULL" ) ;
	m_ConfirmStaText.Init ( rect.left, rect.top, 0, NULL, "NULL" ) ;
	m_ConfirmStaText.SetStaticPosType ( 1 ) ;
	m_ConfirmStaText.SetFontColor ( 0xffff00 ) ;

	m_bInitSuc = true ;
	return TRUE;
}

void CDlgConfirm::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;

}

LRESULT CDlgConfirm::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		if ( wParam == 0 )
		{
			m_bShow = false ;
			m_dwShowTime = 0 ;
		}
		else
		{
			m_bShow = true ;
			m_dwShowTime = TimeGet () ;
		}
	}
	else if ( message == WM_COMMAND )
	{
		if ( wParam == 1 )
		{
			CDlgConfirm::OnConfirmBtnOk () ;
			return true ;
		}
		else if ( wParam == 2 )
		{
			CDlgConfirm::OnConfirmBtnCancel () ;
			return true ;
		}

	}
	else if ( message == WM_DESTROY )
	{
		DeleteObject ( m_Hbr ) ;
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

	return CDialog::WindowProc(message, wParam, lParam);
}

void CDlgConfirm::OnConfirmBtnOk()
{
	switch ( m_uType )
	{
	case CONFIRM_TYPE_25:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EXPBALL_CONFIRM, m_TargetID ) ;
		break ;
	case CONFIRM_TYPE_26:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EXPBALL_ACCEPT, m_TargetID ) ;
		break ;
	case CONFIRM_TYPE_24:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EMONEYBUY_ACCEPT, m_TargetID ) ;
		break ;
	case TRADE_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_TRADE_ACCEPT ) ;
		break ;
	case CONFIRM_TYPE_23:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FRIENDADD_ACCEPT ) ;
		break ;
	case TEAMJOIN_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_TEAMJOIN_ACCEPT ) ;
		break ;
	case TEAMINVITE_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_TEAMINVITE_ACCEPT ) ;
		break ;
	case FACTIONJOIN_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FACTIONJOIN_ACCEPT ) ;
		break ;
	case FACTIONINVITE_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FACTIONINVITE_ACCEPT ) ;
		break ;
	case MONEYDROP_CONFIRM:
		if ( m_nEdtNum > 0 && m_nEdtNum <= g_objHero.GetMoney () )
		{
			CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MONEYDROP_ACCEPT, m_nEdtNum ) ;
		}
		m_ConfirmEdtData.SetWindowText ( NULL ) ;
		break ;
	case FRIENDDEL_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FRIENDDEL_ACCEPT ) ;
		break ;
	case TALKDEL_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_TALKDEL_ACCEPT ) ;
		break ;
	case FACTIONOUT_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FACTIONOUT_ACCEPT ) ;
		break ;
	case FACTIONSUB_CONFIRM:
		if ( m_nEdtNum > 0 && m_nEdtNum <= g_objHero.GetMoney () )
		{
			CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_FACTIONSUB_ACCEPT, m_nEdtNum ) ;
		}
		m_ConfirmEdtData.SetWindowText ( NULL ) ;
		break ;
	case MPCBOOTHADD_CONFIRM:
		if ( m_nEdtNum2 <= 0 )
		{
			if ( m_nEdtNum > 0 )
			{
				CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MPCBOOTHADD_ACCEPT, m_nEdtNum ) ;
			}
		}
		else
		{
			CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MPCBOOTHADD_EMONEY_ACCEPT, m_nEdtNum2 ) ;
		}
		m_ConfirmEdtData.SetWindowText ( NULL ) ;
		m_ConfirmEdtData2.SetWindowText ( NULL ) ;
		break ;
	case MPCBOOTHCHG_CONFIRM:
		if ( m_nEdtNum2 <= 0 )
		{
			if ( m_nEdtNum > 0 )
			{
				CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSG_76, m_nEdtNum ) ;
			}
		}
		else
		{
			CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSG_77, m_nEdtNum2 ) ;
		}
		m_ConfirmEdtData.SetWindowText ( NULL ) ;
		break ;
	case FACTIONDEL_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ACTION_INI ) ;
		break ;
	case BOOTHOPEN_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_BOOTHOPEN_ACCEPT, m_TargetID ) ;
		break ;
	case MPCCOURT_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MPCCOURT_ACCEPT, m_TargetID ) ;
		break ;
	case IMPTITEM_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_IMPTITEM_ACCEPT ) ;
		break ;
	case DROPITEM_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_DROPITEM_ACCEPT ) ;
		break ;
	case TACKOFF_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_TACKOFF_ACCEPT ) ;
		break ;
	case JOINDICE_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_JOINDICE_ACCEPT, m_TargetID ) ;
		break ;
	case OUTDICE_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_OUTDICE_ACCEPT ) ;
		break ;
	case ENEMYDEL_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ENEMYDEL_ACCEPT ) ;
		break ;
	case RESOURCE_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_RESOURCE_ACCEPT ) ;
		break ;
	case CONFIRM_TYPE_27:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EXERCISE_EXIT, m_TargetID ) ;
		break ;
	default:
		break ;
	}
	CDlgConfirm::ShowWindow ( SW_HIDE ) ;
}

void CDlgConfirm::OnConfirmBtnCancel()
{
	switch ( m_uType )
	{
	case IMPTITEM_CONFIRM:
	case DROPITEM_CONFIRM:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_MSG_1003 ) ;
		break ;
	case CONFIRM_TYPE_27:
		CDlgConfirm::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EXERCISE_CANCEL ) ;
		break ;
	default:
		break ;
	}
	CDlgConfirm::ShowWindow ( SW_HIDE ) ;
}

void CDlgConfirm::Show()
{
	if ( m_bInitSuc && m_bShow )
	{
		IAni* ShowAni ;
		if ( m_uType != MPCCOURT_CONFIRM )
		{
			ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														  "Dialog15",
														  EXIGENCE_IMMEDIATE ) ;
			if ( ShowAni != NULL )
				ShowAni->Show ( 0,
								m_Pnt.x - 5,
								m_Pnt.y - 5 ) ;
			else
				return ;
		}
		else
		{
			ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
														  "Dialog45",
														  EXIGENCE_IMMEDIATE ) ;
			if ( ShowAni != NULL )
				ShowAni->Show ( 0,
								m_Pnt.x - 25,
								m_Pnt.y - 25 ) ;
			else
				return ;
		}

		if ( s_nEdtChanged == 1 )
		{
			if ( m_ConfirmEdtData2.GetWindowTextLength () > 0 )
			{
				m_ConfirmEdtData2.SetWindowText ( NULL ) ;
				m_nEdtNum2 = 0 ;
			}
		}
		else if ( s_nEdtChanged == 2 )
		{
			if ( m_ConfirmEdtData.GetWindowTextLength () > 0 )
			{
				m_ConfirmEdtData.SetWindowText ( NULL ) ;
				m_nEdtNum = 0 ;
			}
		}

		m_ConfirmBtnOk.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_ConfirmBtnCancel.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_ConfirmBtnExercise.IsWindowEnabled () )
			m_ConfirmBtnExercise.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_ConfirmStaText.Show ( m_Pnt.x, m_Pnt.y ) ;
		switch ( m_uType )
		{
		case TRADE_CONFIRM:
		case TEAMJOIN_CONFIRM:
		case TEAMINVITE_CONFIRM:
			if ( TimeGet () - m_dwShowTime > 10000 )
				CDlgConfirm::ShowWindow ( SW_HIDE ) ;
			break ;
		case MONEYDROP_CONFIRM:
		case FACTIONSUB_CONFIRM:
			m_ConfirmStaData.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_ConfirmEdtData.Show ( m_Pnt.x, m_Pnt.y ) ;
			break ;
		case MPCBOOTHADD_CONFIRM:
			m_ConfirmStaData.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_ConfirmStaData2.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_ConfirmEdtData.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_ConfirmEdtData2.Show ( m_Pnt.x, m_Pnt.y ) ;
			break ;
		case MPCBOOTHCHG_CONFIRM:
			m_ConfirmStaData.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_ConfirmStaData2.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_ConfirmEdtData.Show ( m_Pnt.x, m_Pnt.y ) ;
			m_ConfirmEdtData2.Show ( m_Pnt.x, m_Pnt.y ) ;
			break ;
		default:
			break ;
		}
	}
}

BOOL CDlgConfirm::SetConfirmDlg(UINT uType, const char *strName, OBJID TargetID)
{
	if ( m_bShow && m_uType == TRADE_CONFIRM )
	{
		g_objGameMsg.AddMsg ( GameDataSetQuery()->GetStr ( 10433 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
		return false ;
	}
	s_nEdtChanged = 0 ;
	m_ConfirmEdtData2.EnableWindow ( false ) ;
	m_ConfirmEdtData.SetWindowText ( "" ) ;
	m_ConfirmEdtData.EnableWindow ( false ) ;
	m_ConfirmBtnExercise.EnableWindow ( false ) ;
	m_ConfirmBtnOk.ChangeImage ( "Button150" ) ;
	m_ConfirmBtnCancel.ChangeImage ( "Button151" ) ;
	m_uType = uType ;
	switch ( uType )
	{
	case CONFIRM_TYPE_25:
		m_ConfirmStaText.SetWindowText ( GameDataSetQuery()->GetStr ( 10489 ) ) ;
		m_TargetID = TargetID ;
		break ;
	case CONFIRM_TYPE_26:
		{
			int nLevel = 0 ;
			int nPercent = 0 ;
			g_objHero.GetLevelForecast ( nLevel, nPercent ) ;
			char szText[256] = "" ;
			sprintf ( szText, GameDataSetQuery()->GetStr ( 10490 ), nLevel, nPercent ) ;
			m_ConfirmStaText.SetWindowText ( szText ) ;
			m_TargetID = TargetID ;
		}
		break ;
	case CONFIRM_TYPE_24:
		m_ConfirmStaText.SetWindowText ( strName ) ;
		m_TargetID = TargetID ;
		break ;
	case TACKOFF_CONFIRM:
		{
			char szText[256] = "" ;
			sprintf ( szText, "%s %s", GameDataSetQuery()->GetStr ( 10381 ), GameDataSetQuery()->GetStr ( 10380 ) ) ;
			m_ConfirmStaText.SetWindowText ( szText ) ;
		}
		break ;
	case DROPITEM_CONFIRM:
		m_ConfirmStaText.SetWindowText ( GameDataSetQuery()->GetStr ( 10372 ) ) ;
		break ;
	case IMPTITEM_CONFIRM:
		m_ConfirmStaText.SetWindowText ( GameDataSetQuery()->GetStr ( 10371 ) ) ;
		break ;
	case RESOURCE_CONFIRM:
		{
			char szText[256] = "" ;
			sprintf ( szText, "%s %s", GameDataSetQuery()->GetStr ( 10405 ), GameDataSetQuery()->GetStr ( 10406 ) ) ;
			m_ConfirmStaText.SetWindowText ( szText ) ;
		}
		break ;
	case ENEMYDEL_CONFIRM:
		m_ConfirmStaText.SetWindowText ( GameDataSetQuery()->GetStr ( 10400 ) ) ;
		break ;
	case OUTDICE_CONFIRM:
		m_ConfirmStaText.SetWindowText ( GameDataSetQuery()->GetStr ( 10395 ) ) ;
		break ;
	case FACTIONOUT_CONFIRM:
		m_ConfirmStaText.SetWindowText ( GameDataSetQuery()->GetStr ( 10009 ) ) ;
		break ;
	case TALKDEL_CONFIRM:
		m_ConfirmStaText.SetWindowText ( GameDataSetQuery()->GetStr ( 10008 ) ) ;
		break ;
	case MONEYDROP_CONFIRM:
		{
			CRect rect ;
			m_nSkip = 0 ;
			m_ConfirmEdtData.EnableWindow ( true ) ;
			CDlgConfirm::GetWindowRect ( rect ) ;
			m_ConfirmEdtData.SetEditBkID("Edit150");
			m_ConfirmEdtData.SetFocus () ;
			m_ConfirmStaText.SetWindowText ( GameDataSetQuery()->GetStr ( 10006 ) ) ;
		}
		break ;
	case TRADE_CONFIRM:
		{
			if ( strName )
			{
				strcpy ( m_strText, strName ) ;
				int len = strlen ( m_strText ) ;
				m_nLeftLeft = ( _DLG_WIDTH - ( float )len * 7 ) / 2;
				char szText[256] = "" ;
				sprintf ( szText, "%s %s %s", m_strText, GameDataSetQuery()->GetStr ( 10000 ), GameDataSetQuery()->GetStr ( 10001 ) ) ;
				m_ConfirmStaText.SetWindowText ( szText ) ;
			}
		}
		break ;
	case CONFIRM_TYPE_23:
		{
			if ( strName )
			{
				strcpy ( m_strText, strName ) ;
				int len = strlen ( m_strText ) ;
				m_nLeftLeft = ( _DLG_WIDTH - ( float )len * 7 ) / 2;
			}
		}
		break ;
	case TEAMJOIN_CONFIRM:
		{
			if ( strName )
			{
				strcpy ( m_strText, strName ) ;
				int len = strlen ( m_strText ) ;
				m_nLeftLeft = ( _DLG_WIDTH - ( float )len * 7 ) / 2;
				char szText[256] = "" ;
				sprintf ( szText, "%s %s %s", m_strText, GameDataSetQuery()->GetStr ( 10002 ), GameDataSetQuery()->GetStr ( 10001 ) ) ;
				m_ConfirmStaText.SetWindowText ( szText ) ;
			}
		}
		break ;
	case TEAMINVITE_CONFIRM:
		{
			if ( strName )
			{
				strcpy ( m_strText, strName ) ;
				int len = strlen ( m_strText ) ;
				m_nLeftLeft = ( _DLG_WIDTH - ( float )len * 7 ) / 2;
				char szText[256] = "" ;
				sprintf ( szText, "%s %s %s", m_strText, GameDataSetQuery()->GetStr ( 10003 ), GameDataSetQuery()->GetStr ( 10001 ) ) ;
				m_ConfirmStaText.SetWindowText ( szText ) ;
			}
		}
		break ;
	case FACTIONJOIN_CONFIRM:
		{
			if ( strName )
			{
				strcpy ( m_strText, strName ) ;
				int len = strlen ( m_strText ) ;
				m_nLeftLeft = ( _DLG_WIDTH - ( float )len * 7 ) / 2;
				char szText[256] = "" ;
				sprintf ( szText, "%s %s %s", m_strText, GameDataSetQuery()->GetStr ( 10004 ), GameDataSetQuery()->GetStr ( 10001 ) ) ;
				m_ConfirmStaText.SetWindowText ( szText ) ;
			}
		}
		break ;
	case FACTIONINVITE_CONFIRM:
		{
			if ( strName )
			{
				strcpy ( m_strText, GameDataSetQuery()->GetStr ( 10011 ) ) ;
				strcat ( m_strText, strName ) ;
				int len = strlen ( m_strText ) ;
				m_nLeftLeft = ( _DLG_WIDTH - ( float )len * 7 ) / 2;
				char szText[256] = "" ;
				sprintf ( szText, "%s %s %s", m_strText, GameDataSetQuery()->GetStr ( 10005 ), GameDataSetQuery()->GetStr ( 10001 ) ) ;
				m_ConfirmStaText.SetWindowText ( szText ) ;
			}
		}
		break ;
	case FACTIONSUB_CONFIRM:
		{
			CRect rect ;
			m_nSkip = 0 ;
			m_ConfirmEdtData.EnableWindow ( true ) ;
			CDlgConfirm::GetWindowRect ( rect ) ;
			m_ConfirmEdtData.SetEditBkID("Edit150");
			m_ConfirmEdtData.SetFocus () ;
			m_ConfirmStaText.SetWindowText ( GameDataSetQuery()->GetStr ( 10010 ) ) ;
		}
		break ;
	case FRIENDDEL_CONFIRM:
		m_ConfirmStaText.SetWindowText ( GameDataSetQuery()->GetStr ( 10007 ) ) ;
		break ;
	case MPCBOOTHADD_CONFIRM:
		{
			CRect rect ;
			m_nSkip = 0 ;
			m_ConfirmEdtData.EnableWindow ( true ) ;
			m_ConfirmEdtData2.EnableWindow ( true ) ;
			CDlgConfirm::GetWindowRect ( rect ) ;
			m_ConfirmEdtData.SetEditBkID("Edit150");
			m_ConfirmEdtData.SetFocus () ;
			m_ConfirmEdtData2.SetEditBkID("Edit150");
			m_ConfirmStaText.SetWindowText ( GameDataSetQuery()->GetStr ( 10353 ) ) ;
		}
		break ;
	case MPCBOOTHCHG_CONFIRM:
		{
			CRect rect ;
			m_nSkip = 0 ;
			m_ConfirmEdtData.EnableWindow ( true ) ;
			CDlgConfirm::GetWindowRect ( rect ) ;
			m_ConfirmEdtData.SetEditBkID("Edit150");
			m_ConfirmEdtData.SetFocus () ;
			m_ConfirmEdtData2.SetEditBkID("Edit150");
			m_ConfirmStaText.SetWindowText ( GameDataSetQuery()->GetStr ( 10353 ) ) ;
		}
		break ;
	case FACTIONDEL_CONFIRM:
		{
			if ( strName )
			{
				strcpy ( m_strText, strName ) ;
				int len = strlen ( m_strText ) ;
				m_nLeftLeft = ( _DLG_WIDTH - ( float )len * 7 ) / 2;
				char szText[256] = "" ;
				sprintf ( szText, "%s %s", GameDataSetQuery()->GetStr ( 10360 ), m_strText ) ;
				m_ConfirmStaText.SetWindowText ( szText ) ;
			}
		}
		break ;
	case BOOTHOPEN_CONFIRM:
	case CONFIRM_TYPE_28:
		m_ConfirmStaText.SetWindowText ( GameDataSetQuery()->GetStr ( 10364 ) ) ;
		m_TargetID = TargetID ;
		break ;
	case MPCCOURT_CONFIRM:
		if ( strName )
		{
			m_TargetID = TargetID ;
			strcpy ( m_strText, strName ) ;
			int len = strlen ( m_strText ) ;
			m_nLeftLeft = ( _DLG_WIDTH - ( float )len * 7 ) / 2;
			char szText[256] = "" ;
			sprintf ( szText, "%s %s %s", m_strText, GameDataSetQuery()->GetStr ( 10365 ), GameDataSetQuery()->GetStr ( 10001 ) ) ;
			m_ConfirmStaText.SetWindowText ( szText ) ;
		}
		break ;
	case JOINDICE_CONFIRM:
		m_ConfirmStaText.SetWindowText ( GameDataSetQuery()->GetStr ( 10394 ) ) ;
		m_TargetID = TargetID ;
		break ;
	case CONFIRM_TYPE_27:
		m_TargetID = TargetID ;
		m_ConfirmStaText.SetWindowText ( GameDataSetQuery()->GetStr ( 11000 ) ) ;
		m_ConfirmBtnExercise.ChangeImage ( "Exercise" ) ;
		m_ConfirmBtnExercise.EnableWindow ( true ) ;
		break ;
	default:
		break ;
	}
	CDlgConfirm::ShowWindow ( SW_SHOW ) ;
	return true ;
}

void CDlgConfirm::OnChangeConfirmEdtData()
{
	char strTemp[12] ;
	m_ConfirmEdtData.GetWindowText ( strTemp, 12 ) ;
	if ( !strcmp ( strTemp, "0" ) )
	{
		m_ConfirmEdtData.SetWindowText ( NULL ) ;
		return ;
	}
	s_nEdtChanged = 1 ;
	m_nSkip++ ;
	if ( m_nSkip % 2 == 0 )
		return ;
	int nLength = m_ConfirmEdtData.GetWindowTextLength () ;
	if ( nLength >= 3 )
	{
		char* pstrTemp = strchr ( strTemp, ',' ) ;
		if ( pstrTemp != NULL )
		{
			char strText[12] ;
			char strNum[12] ;
			strncpy ( strText, strTemp, pstrTemp - strTemp ) ;
			strText[pstrTemp - strTemp] = 0 ;
			strcat ( strText, pstrTemp + 1 ) ;
			pstrTemp = strchr ( strText, ',' ) ;
			if ( pstrTemp != NULL )
			{
				strncpy ( strTemp, strText, pstrTemp - strText ) ;
				strTemp[pstrTemp - strText] = 0 ;
				strcat ( strTemp, pstrTemp + 1 ) ;
				strcpy ( strNum, strTemp ) ;
			}
			else
				strcpy ( strNum, strText ) ;
			if ( strNum != NULL )
			{
				m_nEdtNum = atoi ( strNum ) ;
				int nlen = strlen ( strNum ) ;
				CString cstrText = strNum;
				if ( nlen > 3 && nlen < 7 )
					cstrText.Insert ( nlen - 3, ',' ) ;
				else if ( nlen >= 7 )
				{
					cstrText.Insert ( nlen - 6, ',' ) ;
					cstrText.Insert ( nlen - 2, ',' ) ;
				}
				m_ConfirmEdtData.SetWindowText ( cstrText ) ;
				m_ConfirmEdtData.SetSel ( 12, 12 ) ;
			}
		}
		else if ( strlen ( strTemp ) == 4 )
		{
			m_nEdtNum = atoi ( strTemp ) ;
			CString cstrText = strTemp;
			cstrText.Insert ( 1, ',' ) ;
			m_ConfirmEdtData.SetWindowText ( cstrText ) ;
			m_ConfirmEdtData.SetSel ( 5, 5 ) ;
		}
		else
		{
			m_nEdtNum = atoi ( strTemp ) ;
			m_nSkip++ ;
		}
	}
	else
	{
		m_nSkip = 0 ;
		m_nEdtNum = atoi ( strTemp ) ;
	}
}

void CDlgConfirm::OnChangeConfirmEdtData2()
{
	char strTemp[12] ;
	m_ConfirmEdtData2.GetWindowText ( strTemp, 12 ) ;
	if ( !strcmp ( strTemp, "0" ) )
	{
		m_ConfirmEdtData2.SetWindowText ( NULL ) ;
		return ;
	}
	s_nEdtChanged = 2 ;
	m_nSkip2++ ;
	if ( m_nSkip2 % 2 == 0 )
		return ;
	int nLength = m_ConfirmEdtData2.GetWindowTextLength () ;
	if ( nLength >= 3 )
	{
		char* pstrTemp = strchr ( strTemp, ',' ) ;
		if ( pstrTemp != NULL )
		{
			char strText[12] ;
			char strNum[12] ;
			strncpy ( strText, strTemp, pstrTemp - strTemp ) ;
			strText[pstrTemp - strTemp] = 0 ;
			strcat ( strText, pstrTemp + 1 ) ;
			pstrTemp = strchr ( strText, ',' ) ;
			if ( pstrTemp != NULL )
			{
				strncpy ( strTemp, strText, pstrTemp - strText ) ;
				strTemp[pstrTemp - strText] = 0 ;
				strcat ( strTemp, pstrTemp + 1 ) ;
				strcpy ( strNum, strTemp ) ;
			}
			else
				strcpy ( strNum, strText ) ;
			if ( strNum != NULL )
			{
				m_nEdtNum2 = atoi ( strNum ) ;
				int nlen = strlen ( strNum ) ;
				CString cstrText = strNum;
				if ( nlen > 3 && nlen < 7 )
					cstrText.Insert ( nlen - 3, ',' ) ;
				else if ( nlen >= 7 )
				{
					cstrText.Insert ( nlen - 6, ',' ) ;
					cstrText.Insert ( nlen - 2, ',' ) ;
				}
				m_ConfirmEdtData2.SetWindowText ( cstrText ) ;
				m_ConfirmEdtData2.SetSel ( 12, 12 ) ;
			}
		}
		else if ( strlen ( strTemp ) == 4 )
		{
			m_nEdtNum2 = atoi ( strTemp ) ;
			CString cstrText = strTemp;
			cstrText.Insert ( 1, ',' ) ;
			m_ConfirmEdtData2.SetWindowText ( cstrText ) ;
			m_ConfirmEdtData2.SetSel ( 5, 5 ) ;
		}
		else
		{
			m_nEdtNum2 = atoi ( strTemp ) ;
			m_nSkip2++ ;
		}
	}
	else
	{
		m_nSkip2 = 0 ;
		m_nEdtNum2 = atoi ( strTemp ) ;
	}
}

void CDlgConfirm::OnConfirmBtnExercise()
{
	if ( m_uType == CONFIRM_TYPE_27 )
	{
		g_objHero.RequestTable ( 0 ) ;
		g_objHero.SetExerciseID ( m_TargetID ) ;
	}
	CDlgConfirm::ShowWindow ( SW_HIDE ) ;
}
