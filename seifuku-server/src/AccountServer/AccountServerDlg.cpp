#include "stdafx.h"
#include "AccountServer.h"
#include "AccountServerDlg.h"
#include "allheads.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	afx_msg void OnEnableLogin();
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDC_ENABLE_LOGIN, OnEnableLogin)
END_MESSAGE_MAP()

CAccountServerDlg::CAccountServerDlg(CWnd* pParent )
	: CDialog(CAccountServerDlg::IDD, pParent)
{
	m_sText = _T("");
	m_sState2 = _T("");
	m_sState = _T("");
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nLoginAcceptP5	= 0;
	m_nPointFeeP5		= 0;
}

void CAccountServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEXT, m_sText);
	DDX_Text(pDX, IDC_STATE2, m_sState2);
	DDX_Text(pDX, IDC_STATE, m_sState);
}

BEGIN_MESSAGE_MAP(CAccountServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDCLOSE, OnCloseButtom)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CAccountServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	
	SetWindowText(SERVERTITLE);
	time( &g_tStartServerTime );
	char	bufStart[20];
	DateTime(bufStart, g_tStartServerTime);

	CreateDirectory("!log!", NULL);
	CreateDirectory("!txt!", NULL);
	CreateDirectory("syslog", NULL);

	InitLog(SERVERTITLE, LOGFILE, g_tStartServerTime);

	LOGMSG(		"\n\n\n================================================================================\n"
				"=== %s\n"
				"=== Start server time is %s\n"
				"================================================================================\n\n"
				, SERVERTITLE
				, bufStart );
	LOGSERVER(	"\n\n\n================================================================================\n"
				"=== %s\n"
				"=== Start server time is %s\n"
				"================================================================================\n\n"
				, SERVERTITLE
				, bufStart );
	LOGACCOUNT(	"\n\n\n================================================================================\n"
				"=== %s\n"
				"=== Start server time is %s\n"
				"================================================================================\n\n"
				, SERVERTITLE
				, bufStart );
	LOGPOINT(	"\n\n\n================================================================================\n"
				"=== %s\n"
				"=== Start server time is %s\n"
				"================================================================================\n\n"
				, SERVERTITLE
				, bufStart );

	LOGMSG("***************************************************************");
	LOGMSG("** Account server starting");
	LOGMSG("***************************************************************");

	if(!GetConfig())
	{
		LOGERROR("Cannot read INI file, exiting");
		AfxMessageBox("Cannot read the INI file.");
		return true;
	}
	LOGMSG("INI read OK!");

	if(strlen(SERVER_TITLE))
		SetWindowText(SERVER_TITLE);
	if(g_db.Init(DBHOSTNAME, NULL, NULL, DATABASENAME))
	{
		g_xDatabase	=::CreateMutex(NULL, false, "Database");
		if (!g_xDatabase)
		{
			LOGERROR("Cannot create database mutex, exiting");
			::MessageBox(NULL, "Cannot create g_xDatabase.", "Error",MB_OK|MB_ICONERROR);
			return true;
		}
	}
	else
	{
		LOGERROR("Cannot open the database.");
		::MessageBox(NULL, "Cannot open the database.", "Error",MB_OK|MB_ICONERROR);
		return true;
	}
	LOGMSG("Kernel init OK!");

	char szSQL[256];
	sprintf(szSQL, "select * from %s where account_id = 0", POINTTABLE);
	IRecordset* pRes = g_db.GetInterface()->CreateNewRecordset(szSQL);	
	if(!pRes)
	{
		LOGERROR("Player point table missing");
		::MessageBox(NULL, "Cannot open the players' point table.", "Error",MB_OK|MB_ICONERROR);
		return true;
	}
	pRes->Release();
	
	g_pOnlineTable = new COnlineTable(ONLINETABLESIZE);
	LOGMSG("OnlineTable object created!");
	g_pPointThread = new CPointThread(POINTLISTENPORT, POINTSOCKETSNDBUF);
	LOGMSG("PointThread object created!");
	g_pLoginThread = new CLoginThread(LOGINLISTENPORT);
	LOGMSG("LoginThread object created!");

	int err2 = 0;
	err2 += !g_pPointThread->CreateThread(false);
	LOGMSG("PointThread thread created!");
	err2 += !g_pLoginThread->CreateThread(false);
	LOGMSG("LoginThread thread created!");

	if(!err2)
	{
		err2 += !g_pPointThread->ResumeThread();
		err2 += !g_pLoginThread->ResumeThread();
		if(err2)
		{
			LOGERROR("ResumeThread() failed. Cannot start");
			AfxMessageBox("Error starting the worker threads.");
			return true;
		}
	}
	else
	{
		LOGERROR("Cannot start worker threads, exiting");
		AfxMessageBox("Cannot start the worker threads.");
		return true;
	}

	SetTimer(1, 1000, NULL);
	LOGMSG("SetTimer started!");

	LOGMSG("Startup complete, running normally-------------------------------------");
	if(g_bEnableLogin)
		PrintText("Server running...");
	else
		PrintText("Login paused for %d seconds, waiting for the game servers to synchronise...", ENABLELOGINDELAY);

	return TRUE;
}

void CAccountServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if(nID == SC_CLOSE)
	{
		LOGMSG("'X' key pressed");
		if(AfxMessageBox("Close the server?", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK)
		{
			PostMessage(WM_CLOSE);
			LOGMSG("Close message sent");
		}
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CAccountServerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CAccountServerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAccountServerDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 1)
	{
		g_pOnlineTable->RemoveOvertime();

		int		nMovieCount = 6;
		char	bufPointChar[]	= "-<(|)>-<(|)>-<(|)>-<(|)>-<(|)>";
		char	bufLoginChar[]	= "-<(|)>-<(|)>-<(|)>-<(|)>-<(|)>";

		char	bufScale[21]	= "--------------------";
		char	bufScaleArrow[21]	= "===================>";
		int		nLoginCount	= InterlockedExchange(&s_nLoginCount, 0);
		int		nCurrPlayers	= g_pOnlineTable->GetPlayerCount();

		static int	st_nMaxPlayers = 0;
		static char	st_szMaxPlayersTime[20] = "0000-00-00 00:00:00";
		if(st_nMaxPlayers < nCurrPlayers)
		{
			st_nMaxPlayers	= nCurrPlayers;
			DateTime(st_szMaxPlayersTime, time(NULL));
		}
		int		nMaxServerTime	= InterlockedExchange(&s_nMaxServerTime, 0);
		int		nAvgServerTime	= -1;
		if(nLoginCount)
			nAvgServerTime = InterlockedExchange(&s_nAvgServerTime, 0) / nLoginCount;
		int		nPointCount = InterlockedExchange(&s_nPointCount, 0);
		int		nLoginScale = 20 - (20*nLoginCount*LOGINLOOPDELAY/1000) % 21;
		int		nPointScale = 20 - (20*nPointCount*POINTLOOPDELAY/1000) % 21;
		int		nSocketCount	= InterlockedExchangeAdd(&s_nSocketCount, 0);
		int		nDatabaseTime	= InterlockedExchange(&s_nDatabaseTimeSum, 0);
		int		nAllTime	= InterlockedExchange(&s_nAllTimeSum, 0);
		if(!nAllTime)
			nAllTime = 20;
		int		nDatabaseScale = 20 - ((nDatabaseTime*20 / nAllTime) % 21);
		if(nSocketCount < 0)
			nSocketCount = 0;
		static int	st_nLoginPos = 0;
		static int	st_nPointPos = 0;
		if(nLoginCount)
		{
			st_nLoginPos = (st_nLoginPos + 1) % nMovieCount;
		}
		if(nPointCount)
		{
			st_nPointPos = (st_nPointPos + 1) % nMovieCount;
		}

		ASSERT(MAXGAMESERVERS <= 100);
		char	bufServerState[100];
		memset(bufServerState, c_flagNone, 100);
#define	BSS bufServerState
		::GetServerState(bufServerState);

		const int	nMinutes_per_times	= 300;
		const int	nFirstLog			= 0;
		static int	nSaveLog = 0;
		nSaveLog	= (++nSaveLog) % nMinutes_per_times;
		bool	bSaveLog = (nSaveLog == nFirstLog);
		if(bSaveLog)
		{
			m_nLoginAcceptP5 = InterlockedExchange(&s_nLoginAccept, 0);
			m_nPointFeeP5  = InterlockedExchange(&s_nPointFee, 0);
		}
		int	nMinute5Scale = 20 - (((nSaveLog+nMinutes_per_times-nFirstLog)%nMinutes_per_times+1)*20/nMinutes_per_times) % 21;

		time_t	tCurrTime;
		time( &tCurrTime );
		tm *	pTmFile = localtime(& g_tStartServerTime);
		int	nSec = (int)difftime(tCurrTime, g_tStartServerTime);
		m_sState.Format(
						"start server: %04d-%02d-%02d  %02d:%02d\n"
						"running: %d day(s)  %02d:%02d:%02d\n"
						"\n"
							"[%c]point thread: [%-20s]\n"
							"[%c]login thread: [%-20s]\n"
						"\n"
						"server state: [%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c]\n"
						"server state: [%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c]\n"
						"server state: [%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c]\n"
						"server state: [%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c]\n"
						"server state: [%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c]\n"
						"\n"
							"connections:   [%-20s]\n"
							"database time: [%-20s]\n"
						, pTmFile->tm_year+1900, pTmFile->tm_mon+1, pTmFile->tm_mday
														, pTmFile->tm_hour, pTmFile->tm_min
						, nSec / 86400, (nSec/3600)%24, (nSec/60)%60, nSec%60
							, bufPointChar[st_nPointPos], bufScale + nPointScale
							, bufLoginChar[st_nLoginPos], bufScale + nLoginScale
						, ' ',BSS[1],BSS[2],BSS[3],BSS[4],BSS[5],BSS[6],BSS[7],BSS[8],BSS[9]
								, BSS[10],BSS[11],BSS[12],BSS[13],BSS[14],BSS[15],BSS[16],BSS[17],BSS[18],BSS[19]
						, BSS[20],BSS[21],BSS[22],BSS[23],BSS[24],BSS[25],BSS[26],BSS[27],BSS[28],BSS[29]
								, BSS[30],BSS[31],BSS[32],BSS[33],BSS[34],BSS[35],BSS[36],BSS[37],BSS[38],BSS[39]
						, BSS[40],BSS[41],BSS[42],BSS[43],BSS[44],BSS[45],BSS[46],BSS[47],BSS[48],BSS[49]
								, BSS[50],BSS[51],BSS[52],BSS[53],BSS[54],BSS[55],BSS[56],BSS[57],BSS[58],BSS[59]
						, BSS[60],BSS[61],BSS[62],BSS[63],BSS[64],BSS[65],BSS[66],BSS[67],BSS[68],BSS[69]
								, BSS[70],BSS[71],BSS[72],BSS[73],BSS[74],BSS[75],BSS[76],BSS[77],BSS[78],BSS[79]
						, BSS[80],BSS[81],BSS[82],BSS[83],BSS[84],BSS[85],BSS[86],BSS[87],BSS[88],BSS[89]
								, BSS[90],BSS[91],BSS[92],BSS[93],BSS[94],BSS[95],BSS[96],BSS[97],BSS[98],BSS[99]
							, bufScaleArrow + 20 - (nSocketCount*20/MAXCONNECTS)%21
							, bufScaleArrow + nDatabaseScale
						);
		m_sState2.Format(
						"[%-20s]\n"
						"login (5 min): %3d\n"
						"point (5 min): %3d\n"
						"point total: %4d\n"
						"\n"
							"online table: %d\n"
							"max online: %d\t%s\n"
							"service: max %dms, avg %dms\n"
						, bufScale + nMinute5Scale
						, m_nLoginAcceptP5
						, m_nPointFeeP5
						, InterlockedExchangeAdd(&s_nPointSum, 0)
							, nCurrPlayers
							, st_nMaxPlayers, st_szMaxPlayersTime
							, nMaxServerTime, nAvgServerTime
						);
		char bufText[4096];
		if(LockedGetText(bufText))
		{
			m_sText = bufText;
		}
		UpdateData(false);

		if(bSaveLog)
		{
			LOGMSG(CString("Server state:\n") + m_sState + "\n" + m_sState2 + "\n" + bufText);
		}

		static int	nOnlineCount = 0;
		if(++nOnlineCount >= OUTPUTONLINESECS)
		{
			nOnlineCount = 0;

			FILE *	fOnline = fopen(ONLINEFILE, "w");
			if(fOnline)
			{
				char	bufTime[20];
				DateTime(bufTime, time(NULL));
				fprintf(fOnline, SERVERTITLE "\n");
				fprintf(fOnline, "%s\n", bufTime);
				int nAmount = g_pOnlineTable->GetPlayerCount();
				for(int i = 1; i < g_nServerAccount; i++)
				{
					LPCTSTR		szServerName = g_aServerAccount[i].m_szServerName;
					int			nCount = g_pOnlineTable->GetPlayerCount(szServerName);
					int			nState = g_pPointThread->GetServerState(szServerName);
					const char*		pState = "";
					if(nState == CPointThread::STATE_OFFLINE)
					{
						nAmount -= nCount;
						nCount = 0;
						pState = "(已断开)";
					}
					else if(nState == CPointThread::STATE_BUSY)
						pState = "(忙)";
					else if(nState == CPointThread::STATE_FULL)
						pState = "(已满)";
					fprintf(fOnline, "%2d. %-4s\t：%4d %s\n", 
								i, 
								szServerName, 
								nCount, 
								pState
							);
				}
				fprintf(fOnline, "总在线人数：%6d\n", nAmount);
				fprintf(fOnline, ".\n");
				fprintf(fOnline, "服务器状态：\n%s\n分时统计：\n%s\n面板消息：\n%s\n", m_sState, m_sState2, bufText);
				fclose(fOnline);
			}
		}

		static int	nHeartbeatCount = 0;
		if(++nHeartbeatCount >= HEARTBEATINTERVALSECS / 5)
		{
			g_pPointThread->CheckHeartbeatAll();
			nHeartbeatCount = 0;
		}

		static bool	bLogOK = false;
		if(!g_bEnableLogin && (clock()/CLOCKS_PER_SEC) >= ENABLELOGINDELAY)
		{
			g_bEnableLogin = true;
		}
		if(!bLogOK && g_bEnableLogin)
		{
			bLogOK = true;
			LOGMSG("Account server now allows player logins");
			PrintText("Login allowed");
		}

		static int	nCountCount = 0;
		if(++nCountCount >= COUNTFILESECS)
		{
			try{
				nCountCount = 0;
				g_pPointThread->LogCount();

				if(STAT_SERIAL)
					g_pPointThread->LogSerialCount();
			}catch(...){ LOGCATCH("write COUNT.TXT"); }
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CAboutDlg::OnEnableLogin() 
{
	g_bEnableLogin = true;

	CDialog::OnOK();
}

void CAccountServerDlg::OnCancel() 
{
	LOGMSG("'ESC' key pressed");
	if(AfxMessageBox("Close the server?", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK)
	{
		PostMessage(WM_CLOSE);
		LOGMSG("Close message sent");
	}
	return;
	
}

void CAccountServerDlg::OnClose() 
{
	static bool	bFlag = false;
	if(!bFlag)
	{
		g_bEnableLogin = false;
		bFlag = true;
		LOGMSG("Server closing------------------------------------------------");
		PrintText("Server closing...");

		KillTimer(1);
	}

	if(g_pLoginThread->CloseThread(200))
	{
		if(g_pPointThread->CloseThread(200))
		{
			delete	g_pLoginThread;
			delete	g_pPointThread;
			delete	g_pOnlineTable;

			g_db.Destroy();
			if (g_xDatabase)
			{
				::CloseHandle(g_xDatabase);
				g_xDatabase	=NULL;
			}
			LOGMSG("***************************************************************");
			LOGMSG("**  Server closed normally");
			LOGMSG("***************************************************************\n\n\n");

			CDialog::OnClose();
			CDialog::OnOK();
			return;
		}
	}

	Sleep(500);
	PostMessage(WM_CLOSE);
}

void CAccountServerDlg::OnCloseButtom() 
{
	LOGMSG("'Close' button pressed");
	if(AfxMessageBox("Close the server?", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK)
	{
		PostMessage(WM_CLOSE);
		LOGMSG("Close message sent");
	}
}
