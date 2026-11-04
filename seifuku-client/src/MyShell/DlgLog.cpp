#include "stdafx.h"
#include "MyShell.h"
#include "DlgLog.h"

#include "GameMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgLog::CDlgLog(CWnd* pParent )
	: CDialog(CDlgLog::IDD, pParent)
{
	m_EdtHbr = CreateSolidBrush ( RGB ( 43, 75, 90 ) ) ;
	m_bFirst = true ;
	m_nLastLine = 0 ;
	m_nLogChannel = 0 ;
}


void CDlgLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG_LOGTEAM, m_LogTeam);
	DDX_Control(pDX, IDC_LOG_LOGS, m_LogS);
	DDX_Control(pDX, IDC_LOG_LOGG, m_LogG);
	DDX_Control(pDX, IDC_LOG_LOGFRD, m_LogFrd);
	DDX_Control(pDX, IDC_LOG_LOGFAC, m_LogFac);
	DDX_Control(pDX, IDC_LOG_LOGALL, m_LogAll);
	DDX_Control(pDX, IDC_LOG_CLOSE, m_LogClose);
	DDX_Control(pDX, IDC_LOG_DELALL, m_LogDelAll);
	DDX_Control(pDX, IDC_LOG_DEL, m_LogDel);
	DDX_Control(pDX, IDC_LOG_RECORD, m_LogRecord);
	DDX_Control(pDX, IDC_LOG_DATA, m_LogData);
	DDX_Control(pDX, IDC_LOG_FILE, m_LogFile);
}


BEGIN_MESSAGE_MAP(CDlgLog, CDialog)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_LOG_FILE, OnSelchangeLogFile)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_LOG_DEL, OnLogDel)
	ON_BN_CLICKED(IDC_LOG_DELALL, OnLogDelall)
	ON_BN_CLICKED(IDC_LOG_RECORD, OnLogRecord)
	ON_BN_CLICKED(IDC_LOG_CLOSE, OnLogClose)
	ON_BN_CLICKED(IDC_LOG_LOGG, OnLogLogg)
	ON_BN_CLICKED(IDC_LOG_LOGS, OnLogLogs)
	ON_BN_CLICKED(IDC_LOG_LOGFAC, OnLogLogfac)
	ON_BN_CLICKED(IDC_LOG_LOGFRD, OnLogLogfrd)
	ON_BN_CLICKED(IDC_LOG_LOGTEAM, OnLogLogteam)
	ON_BN_CLICKED(IDC_LOG_LOGALL, OnLogLogall)
END_MESSAGE_MAP()

BOOL CDlgLog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CDlgLog::MoveWindow ( 0, 0, 585, 321, true ) ;

	CRect rect ;
	CDlgLog::GetWindowRect ( rect ) ;

	int nWidth = 78 ;
	int nHeight = 24 ;
	m_LogTeam.MoveWindow ( 392, 58, nWidth, nHeight, true ) ;
	m_LogS.MoveWindow ( 122, 58, nWidth, nHeight, true ) ;
	m_LogG.MoveWindow ( 32, 58, nWidth, nHeight, true ) ;
	m_LogFrd.MoveWindow ( 302, 58, nWidth, nHeight, true ) ;
	m_LogFac.MoveWindow ( 212, 58, nWidth, nHeight, true ) ;
	m_LogAll.MoveWindow ( 482, 58, nWidth, nHeight, true ) ;
	m_LogClose.MoveWindow ( 558, 10, 16, 16, true ) ;
	m_LogRecord.MoveWindow ( 337, 32, 16, 16, true ) ;
	m_LogDelAll.MoveWindow ( 495, 30, 73, 28, true ) ;
	m_LogDel.MoveWindow ( 417, 30, 73, 28, true ) ;
	m_LogData.MoveWindow ( 20, 88, 542, 210, true ) ;
	m_LogFile.MoveWindow ( 30, 32, 114, 22, true ) ;

	return TRUE;
}

void CDlgLog::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_Hbr)
		DeleteObject( m_Hbr ) ;
	if ( m_EdtHbr )
		DeleteObject ( m_EdtHbr ) ;
}

void CDlgLog::SetLog(BYTE btType)
{
	switch ( btType )
	{
	case 0:
		{
			m_LogFile.ResetContent () ;
			int nAmount = g_objGameMsg.m_objChatLog.GetLogFileAmount () ;
			for ( int i = 0; i < nAmount; i++ )
			{
				ChatLogFile* pLogFile ;
				pLogFile = g_objGameMsg.m_objChatLog.GetLogFileByIndex ( i ) ;
				if ( pLogFile != NULL )
				{
					char strTemp[64] ;
					sprintf ( strTemp, "%d-%d-%d", pLogFile->nYear, pLogFile->nMonth, pLogFile->nDay ) ;
					m_LogFile.InsertString ( 0, strTemp ) ;
				}
			}
			m_LogFile.SetCurSel ( 0 ) ;
			CDlgLog::OnSelchangeLogFile () ;
		}
		break ;
	case 1:
		{
			int nCurIndex = m_LogData.LineIndex ( m_LogData.GetFirstVisibleLine () ) ;
			int nLineAmount = g_objGameMsg.m_objChatLog.GetLineAmount () ;
			CString strText ;
			for ( int i = m_nLastLine; i < nLineAmount; i++ )
			{
				ChatLogLine* pLogLine ;
				pLogLine = g_objGameMsg.m_objChatLog.GetLineByIndex ( i ) ;
				if ( m_nLogChannel != 0 )
				{
					if ( pLogLine == NULL || !g_objGameMsg.m_objChatLog.TestLine ( m_nLogChannel, pLogLine ) )
						continue ;
				}
				if ( pLogLine != NULL )
				{
					int nLastSel = m_LogData.GetWindowTextLength () ;
					m_LogData.SetSel ( nLastSel, nLastSel, true ) ;
					strText = pLogLine->szTxt ;
					strText.Insert ( strText.GetLength (), "\r\n\r\n" ) ;
					m_LogData.ReplaceSel ( strText ) ;
				}
			}
			m_LogData.SetSel ( nCurIndex, nCurIndex ) ;
			m_nLastLine = nLineAmount ;
		}
		break ;
	}
}

void CDlgLog::OnSelchangeLogFile()
{
	ChatLogFile* pLogFile = g_objGameMsg.m_objChatLog.GetLogFileByIndex ( m_LogFile.GetCount () - m_LogFile.GetCurSel () - 1 ) ;
	if ( pLogFile != NULL )
	{
		m_nLastLine = 0 ;
		m_LogData.SetWindowText ( NULL ) ;
		g_objGameMsg.m_objChatLog.LoadLog ( pLogFile->szFileName ) ;
	}
}

HBRUSH CDlgLog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if ( nCtlColor == CTLCOLOR_DLG )
		return m_Hbr ;
	else if ( nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_MSGBOX )
	{
		pDC->SetBkColor ( RGB ( 43, 75, 90 ) ) ;
		pDC->SetTextColor ( RGB ( 214, 213, 211 ) ) ;
		return m_EdtHbr ;
	}

	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CDlgLog::OnLogDel()
{
	ChatLogFile* pLogFile = g_objGameMsg.m_objChatLog.GetLogFileByIndex ( m_LogFile.GetCount () - m_LogFile.GetCurSel () - 1 ) ;
	if ( pLogFile != NULL )
		g_objGameMsg.m_objChatLog.DelLogFile ( pLogFile->szFileName ) ;
}

void CDlgLog::OnLogDelall()
{
	g_objGameMsg.m_objChatLog.DelAllLogFile () ;
}

void CDlgLog::OnLogRecord()
{
	if ( m_LogRecord.GetCheck () )
		g_objGameMsg.m_objChatLog.LogOn () ;
	else
		g_objGameMsg.m_objChatLog.LogOff () ;
}

void CDlgLog::OnLogClose()
{
	CDlgLog::ShowWindow ( SW_HIDE ) ;
}

void CDlgLog::OnLogLogg()
{
	if ( _TXTATR_TALK != m_nLogChannel )
	{
		m_LogData.SetWindowText ( NULL ) ;
		m_nLastLine = 0 ;
	}
	m_nLogChannel = _TXTATR_TALK ;
	CDlgLog::SetLog ( 1 ) ;
}

void CDlgLog::OnLogLogs()
{
	if ( _TXTATR_PRIVATE != m_nLogChannel )
	{
		m_LogData.SetWindowText ( NULL ) ;
		m_nLastLine = 0 ;
	}
	m_nLogChannel = _TXTATR_PRIVATE ;
	CDlgLog::SetLog ( 1 ) ;
}

void CDlgLog::OnLogLogfac()
{
	if ( _TXTATR_SYNDICATE != m_nLogChannel )
	{
		m_LogData.SetWindowText ( NULL ) ;
		m_nLastLine = 0 ;
	}
	m_nLogChannel = _TXTATR_SYNDICATE ;
	CDlgLog::SetLog ( 1 ) ;
}

void CDlgLog::OnLogLogfrd()
{
	if ( _TXTATR_FRIEND != m_nLogChannel )
	{
		m_LogData.SetWindowText ( NULL ) ;
		m_nLastLine = 0 ;
	}
	m_nLogChannel = _TXTATR_FRIEND ;
	CDlgLog::SetLog ( 1 ) ;
}

void CDlgLog::OnLogLogteam()
{
	if ( _TXTATR_TEAM != m_nLogChannel )
	{
		m_LogData.SetWindowText ( NULL ) ;
		m_nLastLine = 0 ;
	}
	m_nLogChannel = _TXTATR_TEAM ;
	CDlgLog::SetLog ( 1 ) ;
}

void CDlgLog::OnLogLogall()
{
	if ( 0 != m_nLogChannel )
	{
		m_LogData.SetWindowText ( NULL ) ;
		m_nLastLine = 0 ;
	}
	m_nLogChannel = 0 ;
	CDlgLog::SetLog ( 1 ) ;
}

void CDlgLog::ResetBk(int nMode)
{
	char strName[32] ;
	char strMain[8] ;
	if ( nMode == 0 )
		strcpy ( strMain, "Main" ) ;
	else
		sprintf ( strMain, "Main%d", nMode ) ;
	sprintf ( strName, "Data/%s/LogBk.bmp", strMain ) ;
	HBITMAP hBmp ;
	hBmp =  (HBITMAP )::LoadImage ( AfxGetInstanceHandle(),
									strName,
									IMAGE_BITMAP,
									0,
									0,
									LR_LOADFROMFILE ) ;
	m_Hbr = CreatePatternBrush ( hBmp ) ;

	sprintf ( strName, "Data/%s/LogDel.bmp", strMain ) ;
	hBmp = (HBITMAP)::LoadImage ( AfxGetInstanceHandle(),
								  strName,
								  IMAGE_BITMAP,
								  0,
							      0,
								  LR_LOADFROMFILE ) ;
	m_LogDel.SetBitmap ( hBmp ) ;

	sprintf ( strName, "Data/%s/LogDelAll.bmp", strMain ) ;
	hBmp = (HBITMAP)::LoadImage ( AfxGetInstanceHandle(),
								  strName,
								  IMAGE_BITMAP,
								  0,
							      0,
								  LR_LOADFROMFILE ) ;
	m_LogDelAll.SetBitmap ( hBmp ) ;

	sprintf ( strName, "Data/%s/LogAll.bmp", strMain ) ;
	hBmp = (HBITMAP)::LoadImage ( AfxGetInstanceHandle(),
								  strName,
								  IMAGE_BITMAP,
								  0,
							      0,
								  LR_LOADFROMFILE ) ;
	m_LogAll.SetBitmap ( hBmp ) ;

	sprintf ( strName, "Data/%s/LogFac.bmp", strMain ) ;
	hBmp = (HBITMAP)::LoadImage ( AfxGetInstanceHandle(),
								  strName,
								  IMAGE_BITMAP,
								  0,
							      0,
								  LR_LOADFROMFILE ) ;
	m_LogFac.SetBitmap ( hBmp ) ;

	sprintf ( strName, "Data/%s/LogFrd.bmp", strMain ) ;
	hBmp = (HBITMAP)::LoadImage ( AfxGetInstanceHandle(),
								  strName,
								  IMAGE_BITMAP,
								  0,
							      0,
								  LR_LOADFROMFILE ) ;
	m_LogFrd.SetBitmap ( hBmp ) ;

	sprintf ( strName, "Data/%s/LogG.bmp", strMain ) ;
	hBmp = (HBITMAP)::LoadImage ( AfxGetInstanceHandle(),
								  strName,
								  IMAGE_BITMAP,
								  0,
							      0,
								  LR_LOADFROMFILE ) ;
	m_LogG.SetBitmap ( hBmp ) ;

	sprintf ( strName, "Data/%s/LogS.bmp", strMain ) ;
	hBmp = (HBITMAP)::LoadImage ( AfxGetInstanceHandle(),
								  strName,
								  IMAGE_BITMAP,
								  0,
							      0,
								  LR_LOADFROMFILE ) ;
	m_LogS.SetBitmap ( hBmp ) ;

	sprintf ( strName, "Data/%s/LogTeam.bmp", strMain ) ;
	hBmp = (HBITMAP)::LoadImage ( AfxGetInstanceHandle(),
								  strName,
								  IMAGE_BITMAP,
								  0,
							      0,
								  LR_LOADFROMFILE ) ;
	m_LogTeam.SetBitmap ( hBmp ) ;

	sprintf ( strName, "Data/%s/Close.bmp", strMain ) ;
	hBmp = (HBITMAP)::LoadImage ( AfxGetInstanceHandle(),
								  strName,
								  IMAGE_BITMAP,
								  0,
							      0,
								  LR_LOADFROMFILE ) ;
	m_LogClose.SetBitmap ( hBmp ) ;

	CDlgLog::RedrawWindow () ;
}
