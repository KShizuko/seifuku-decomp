#include "stdafx.h"
#include "NpcServer.h"
#include "NpcServerDlg.h"
#include "typedef.h"
#include "LogFile.h"
#include "inifile.h"
#include "P_ServerManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long				s_nDatabaseTimeSum;
long	g_nRestart = false;
struct STAT_ST g_stat;
long	g_bExitEvent = false;

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
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
END_MESSAGE_MAP()

CNpcServerDlg::CNpcServerDlg(CWnd* pParent )
	: CDialog(CNpcServerDlg::IDD, pParent)
{
	m_sKernel = _T("");
	m_sShell = _T("");
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nState			= STATE_NONE;
	m_pInterPort		= NULL;
	m_szServerName[0]	= 0;
	m_nTextLines		= 0;
}

void CNpcServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEXT, m_ctlText);
	DDX_Text(pDX, IDC_STATIC_KERNEL, m_sKernel);
	DDX_Text(pDX, IDC_STATIC_SHELL, m_sShell);
}

BEGIN_MESSAGE_MAP(CNpcServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CNpcServerDlg::OnInitDialog()
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
	
	char	szExitEvent[] = "TQ ConqerOnline Exit Event";
	m_hExitEvent = ::CreateEvent(NULL, TRUE, FALSE, szExitEvent);
	if(!m_hExitEvent)
	{
		MessageBox("Create exit event failed!");
		this->EndDialog(-1);
		return false;
	}

	m_hMutexServer = ::CreateMutex(NULL, false, "ConquerNpcServer");
	if (m_hMutexServer)
	{
		if (ERROR_ALREADY_EXISTS == ::GetLastError())
		{
		}
	}
	else
	{
		MessageBox("Create mutext failed!");
		this->EndDialog(-1);
		return false;
	}

	CIniFile	iniShell("shell.ini", "AccountServer");
	iniShell.GetString(m_szShellServerName, "SERVERNAME", 16);

	char	szTitle[1024];
	_snprintf(szTitle, 1024, "%s - %s - %s %s", NPCSERVER_TITLE, m_szShellServerName, __DATE__, __TIME__);
	szTitle[1023] = 0;
	SetWindowText(szTitle);
	::CreateDirectory(LOGFILE_DIRECTION, NULL);
	::InitLog(szTitle, LOGFILE_FILENAME, time(NULL));
	LOGMSG("\n\n\n=================================================================");
	LOGMSG(szTitle);
	LOGMSG("=================================================================");

	m_nState	= STATE_STARTING;
	m_tUpdateStateWin.Startup(UPDATESTATEWIN_SECS);
	m_tSaveStateWin.Startup(SAVESTATEWIN_SECS);

	CIniFile	ini(CONFIG_INI_FILENAME, "InternetPort");
	int		nPortSize	= ini.GetInt("PORT_SIZE");
	char	szMasterIP[16];
	ini.GetString(szMasterIP, "MASTER_IP", 16);
	int		nMasterPort	= ini.GetInt("MASTER_PORT");
	int		nCurrPort	= ini.GetInt("CURRENT_PORTID");
	char	szKey[256];
	ini.GetString(szKey, "LOGIN_KEY", 256);

	ini.SetSection("System");
	ini.GetString(m_szServerName, "SERVERNAME", 16);

	if(nCurrPort && nPortSize)
	{
		m_pInterPort = CInternetPort::CreateNew(nCurrPort, nPortSize, szMasterIP, nMasterPort, szKey);
		if(m_pInterPort)
			m_pInterPort->Open();
	}

	SetTimer(1, 500, NULL);

	_snprintf(szTitle, 1024, "%s - %s", NPCSERVER_TITLE, m_szShellServerName);
	::SetProp(GetSafeHwnd(), szTitle, (HANDLE)1);

	return TRUE;
}

void CNpcServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ((nID & 0xFFF0) == SC_CLOSE)
	{
		PostMessage(WM_COMMAND, IDCANCEL);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CNpcServerDlg::OnPaint() 
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

HCURSOR CNpcServerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CNpcServerDlg::OnCancel() 
{
	PrintText("CLOSE button pressed.");
	ShowText();
	if(AfxMessageBox("Are you CLOSE server new?", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK)
	{
		m_nState	= STATE_CLOSING;
	}

}

void CNpcServerDlg::PrintText(LPCTSTR szMsg)
{
	LOCKTHREAD;

	m_sUpdate	+= "【";
	m_sUpdate	+= szMsg;
	m_sUpdate	+= "】\r\n";

	const int TEXTWINDOW_SIZE = 8;
	if(m_nTextLines >= TEXTWINDOW_SIZE)
	{
		int nPos = m_sText.Find("\n", 0);
		if(nPos != -1)
			m_sText = m_sText.Mid(nPos + 1);
	}
	char	buf[20];
	DateTime(buf);
	m_sText += buf+11;
	m_sText += "【";
	m_sText += szMsg;
	m_sText += "】";
	m_sText += "\r\n";
	m_nTextLines++;

	LOGMSG("SHELL: %s", szMsg);
}

void CNpcServerDlg::CloseAll()
{

	m_nState	= STATE_CLOSING;
}

void CNpcServerDlg::SetState(LPCTSTR szMsg)
{
	LOCKTHREAD;

	m_sKernel = szMsg;
}

void CNpcServerDlg::ChangeEncrypt(DWORD nKey)
{
	LOCKTHREAD;

}

void CNpcServerDlg::OnTimer(UINT nIDEvent) 
{
	if(!g_bExitEvent && ::WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
	{
		g_bExitEvent	= true;
		m_tExitDelay.Startup(50);
	}
	else if(m_tExitDelay.TimeOver())
		m_nState	= STATE_CLOSING;

	if(nIDEvent == 1)
	{
		switch(m_nState)
		{
		case	STATE_STARTING:
			{
				m_nState	= STATE_NORMAL;

				if(!m_thdKernel.CreateThread(this))
				{
					PrintText("Can't create kernel thread!");
					ShowText();
					m_nState	= STATE_CLOSING;
					break;
				}

				PrintText("Create kernel thread ok.");
				ShowText();
			}
			break;
		case	STATE_NORMAL:
			{
				if(!m_sUpdate.IsEmpty())
				{
					ShowText();
				}
				if(m_tUpdateStateWin.ToNextTime())
				{
					SetShellState();

					if(m_tSaveStateWin.ToNextTime())
					{
						LOGMSG(m_sShell);
						LOGMSG(m_sKernel);
					}

					LOCKTHREAD;
					UpdateData(false);
				}

				ProcessInterMsg();

				extern long g_nRestart;
				if(g_nRestart)
					m_nState = STATE_CLOSING;
			}
			break;
		case	STATE_CLOSING:
			{
				if(!m_thdKernel.CloseThread(3000))
				{
					PrintText("Closing kernel thread, waiting . . .");
					ShowText();
					break;
				}

				PrintText("Close kernel thread ok.");
				ShowText();

				if(m_hMutexServer)
				{
					::CloseHandle(m_hMutexServer);
					::ReleaseMutex(m_hMutexServer);
					m_hMutexServer = NULL;
				}

				if(m_pInterPort)
				{
					SAFE_DELETE(m_pInterPort);
				}

				m_nState	= STATE_CLOSED;
			}
			break;
		case	STATE_CLOSED:
			{
				KillTimer(1);
				m_nState	= STATE_NONE;
				Sleep(1000);

				extern long g_nRestart;
				if(g_nRestart)
					::ShellExecute(m_hWnd, NULL, AfxGetAppName(), NULL, NULL, SW_SHOWNORMAL);

				CDialog::OnCancel();

			}
			break;
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CNpcServerDlg::ShowText()
{
	if(!m_sUpdate.IsEmpty())
	{
		LOCKTHREAD;

		if(m_ctlText.GetLineCount() > MAX_TEXTLINESIZE)
		{
			m_ctlText.SetSel(0, m_ctlText.LineIndex(MAX_TEXTLINESIZE/2), true);
			m_ctlText.ReplaceSel("");
		}

		m_ctlText.SetSel(END_OF_TEXT, END_OF_TEXT, true);

		m_ctlText.ReplaceSel(m_sUpdate);

		UpdateData(false);

		m_sUpdate.Empty();
	}
}

void CNpcServerDlg::SetShellState()
{
	extern struct STAT_ST	g_stat;
	STAT_ST	stat = g_stat;
	memset(&g_stat, 0, sizeof(STAT_ST));
	memset(&g_stat, 0, sizeof(STAT_ST));
	m_sShell.Format("kernel(ms): %d/%d", stat.nKernelCount ? stat.nKernelSum/stat.nKernelCount : 0, stat.nKernelMax);
}

bool CNpcServerDlg::ProcessInterMsg()
{
	if(!m_pInterPort)
		return false;

	char			buf[1024];
	CMessageStatus	cStatus;

	DEBUG_TRY
	if(!m_pInterPort->IsOpen())
	{
		if(!m_pInterPort->Open())
			return false;
	}

	while(m_pInterPort->Recv(PORT_ANY, PACKET_ANY, STRUCT_TYPE(buf), buf, &cStatus))
	{
		switch(cStatus.m_nPacket)
		{
		case	QUERY_STATUS:
			{
				m_pInterPort->Send(cStatus.m_nPortFrom, ACK_TITLE, STRING_TYPE(m_szServerName), m_szServerName);
				m_pInterPort->Send(cStatus.m_nPortFrom, ACK_SHELLMSG, STRING_TYPE(m_sShell), (LPCTSTR)m_sShell);
				m_pInterPort->Send(cStatus.m_nPortFrom, ACK_KERNELMSG, STRING_TYPE(m_sKernel), (LPCTSTR)m_sKernel);

				LOCKTHREAD;
				m_pInterPort->Send(cStatus.m_nPortFrom, ACK_TEXT, STRING_TYPE(m_sText), m_sText);
			}
			break;
		case	QUERY_COMMOND:
			{
				LPCTSTR pStr = (LPCTSTR)buf;
				CString str;
				str.Format("【remote】%s", pStr);
				PrintText(str);
				
				if(strcmp(pStr, "restartserver") == 0)
				{
					PrintText(pStr);
					g_nRestart = true;
				}
			}
			break;
		default:
			PrintText("CMsgServerDlg::ProcessInterMsg()");
			return false;
		}
	}
	DEBUG_CATCH("CNpcServerDlg::Process()")

	return true;
}

void CNpcServerDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	::CloseHandle(m_hExitEvent);
	CString	str;
	str.Format("%s - %s", NPCSERVER_TITLE, m_szShellServerName);
	::RemoveProp(GetSafeHwnd(), str);
}
