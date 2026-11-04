#include "stdafx.h"
#include "protocol.h"
#include "common.h"
#include "IniFile.h"
#include "LogFile.h"
#include "I_Shell.h"
#include "SocketThread.h"
#include "WorldThread.h"
#include "MapGroupThread.h"
#include "MessagePort.h"
#include "P_ServerManager.h"
#include "basetype.h"
#include "array.h"
#include "mystack.h"
#include "I_MessagePort.h"
#include "MapGroupKernel/DeadLoop.h"
#include "MsgServer.h"
#include "MsgServerDlg.h"
#include "StringManager.h"

ST_CONFIG		CONFIG;
int				LOGIN_FREE_LEVEL	= 16;
struct STAT_STRUCT	g_stat = {0};
long				g_nRestart		= false;
long				g_nServerClosed = false;
long				g_bExitEvent	= false;
long				s_nDatabaseTimeSum;

void LogSystemInfo()
{
	LOGMSG(__FILE__);
	LOGMSG("This server is for language %s", CStringManager::Instance()->GetStr("Language"));

	char	szBuf[4096];
	DWORD	dwSize	= sizeof(szBuf);
	if(::GetComputerName(szBuf, &dwSize))
		LOGMSG("Computer name: %s", szBuf);
	else
		LOGERROR("GetComputerName() failed at %s, %d", __FILE__, __LINE__);

	dwSize	= sizeof(szBuf);
	if(::GetUserName(szBuf, &dwSize))
	{
		LOGMSG("User name: %s", szBuf);
	}
	else
		LOGERROR("GetUserName() failed at %s, %d", __FILE__, __LINE__);

	if(::GetSystemDirectory(szBuf, sizeof(szBuf)))
	{
		LOGMSG("System Directory: %s", szBuf);
	}
	else
		LOGERROR("GetSystemDirectory() failed at %s, %d", __FILE__, __LINE__);

	if(::GetWindowsDirectory(szBuf, sizeof(szBuf)))
	{
		LOGMSG("Windows Directory: %s", szBuf);
	}
	else
		LOGERROR("GetWindowsDirectory() failed at %s, %d", __FILE__, __LINE__);

	LOGMSG("Some environment variables:");
	const char* setEnvVar[] = {
		"OS=%OS%",
		"LANG=%LANG%",
		"NUMBER_OF_PROCESSORS=%NUMBER_OF_PROCESSORS%",
		"PROCESSOR_ARCHITECTURE=%PROCESSOR_ARCHITECTURE%",
		"PROCESSOR_IDENTIFIER=%PROCESSOR_IDENTIFIER%",
		"PROCESSOR_LEVEL=%PROCESSOR_LEVEL%",
		"PROCESSOR_REVISION=%PROCESSOR_REVISION%",
		"PATH=%PATH%",
		"TEMP=%TEMP%",
	};
	for(DWORD i = 0; i < sizeof(setEnvVar)/sizeof(setEnvVar[0]); i++)
	{
		const char* pszVar = setEnvVar[i];
		dwSize	= ::ExpandEnvironmentStrings(pszVar, szBuf, sizeof(szBuf));
		if(dwSize > sizeof(szBuf))
		{
			LOGERROR("Buffer too small to expand: \"%s\" at %s, %d", pszVar, __FILE__, __LINE__);
		}
		else if(dwSize)
			LOGMSG("\t%s", szBuf);
		else
			LOGERROR("ExpandEnvironmentStrings() failed (%u) at %s, %d", i, __FILE__, __LINE__);
	}

	HKEY	hKey;
	if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System", 0, KEY_QUERY_VALUE, &hKey) != ERROR_ACCESS_DENIED)
	{
		LOGMSG("Registry information:");

		BYTE	bufData[4096];
		DWORD	dwType;
		DWORD	dwLen;

		dwLen	= sizeof(bufData);
		dwType	= REG_SZ;
		if(::RegQueryValueEx(hKey, "Identifier", NULL, &dwType, bufData, &dwLen) == ERROR_SUCCESS)
			LOGMSG("\tIdentifier: %s", bufData);

		dwLen	= sizeof(bufData);
		dwType	= REG_FULL_RESOURCE_DESCRIPTOR;
		if(::RegQueryValueEx(hKey, "Configuration Data", NULL, &dwType, bufData, &dwLen) == ERROR_SUCCESS && dwLen)
		{
			_snprintf(szBuf, sizeof(szBuf), "%02x", bufData[0]);
			for(DWORD i = 1; i < dwLen; i++)
			{
				char	szHex[4];
				_snprintf(szHex, sizeof(szHex), " %02x", bufData[i]);
				strncat(szBuf, szHex, sizeof(szBuf));
			}
			LOGMSG("\tConfiguration Data: %s", szBuf);
		}

		dwLen	= sizeof(bufData);
		dwType	= REG_SZ;
		if(::RegQueryValueEx(hKey, "SystemBiosDate", NULL, &dwType, bufData, &dwLen) == ERROR_SUCCESS)
			LOGMSG("\tSystemBiosDate: %s", bufData);

		dwLen	= sizeof(bufData);
		dwType	= REG_MULTI_SZ;
		if(::RegQueryValueEx(hKey, "SystemBiosVersion", NULL, &dwType, bufData, &dwLen) == ERROR_SUCCESS)
		{
			for(DWORD i = 0; i < dwLen - 1; i++)
			{
				if(bufData[i] == 0)
					bufData[i] = ' ';
			}
			LOGMSG("\tSystemBiosVersion: %s", bufData);
		}

		dwLen	= sizeof(bufData);
		dwType	= REG_SZ;
		if(::RegQueryValueEx(hKey, "VideoBiosDate", NULL, &dwType, bufData, &dwLen) == ERROR_SUCCESS)
			LOGMSG("\tVideoBiosDate: %s", bufData);

		dwLen	= sizeof(bufData);
		dwType	= REG_MULTI_SZ;
		if(::RegQueryValueEx(hKey, "VideoBiosVersion", NULL, &dwType, bufData, &dwLen) == ERROR_SUCCESS)
		{
			for(DWORD i = 0; i < dwLen - 1; i++)
			{
				if(bufData[i] == 0)
					bufData[i] = ' ';
			}
			LOGMSG("\tVideoBiosVersion: %s", bufData);
		}

		::RegCloseKey(hKey);
	}
	else
	{
		LOGERROR("Failed to get registry information at %s, %d", __FILE__, __LINE__);
	}

	SYSTEM_INFO	si;
	memset(&si, 0, sizeof(si));

	typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
	PGNSI pGNSI = (PGNSI)::GetProcAddress(::GetModuleHandle("kernel32.dll"), "GetNativeSystemInfo");
	if(pGNSI)
		pGNSI(&si);
	else
		::GetSystemInfo(&si);

	MYASSERT(PROCESSOR_ARCHITECTURE_INTEL == si.wProcessorArchitecture);
	LOGMSG("System information:");
	LOGMSG("\tOEM ID: %u", si.dwOemId);
	LOGMSG("\tNumber of processors: %u", si.dwNumberOfProcessors);
	switch(si.wProcessorLevel)
	{
	case 5:
		LOGMSG("\tProcessor type: Intel Pentium model %u stepping %u", si.wProcessorRevision >> 8, si.wProcessorRevision & 0xFF);
		break;
	case 6:
		LOGMSG("\tProcessor type: Intel Pentium Pro/II/III model %u stepping %u", si.wProcessorRevision >> 8, si.wProcessorRevision & 0xFF);
		break;
	case 15:
		LOGMSG("\tProcessor type: Intel Pentium IV model %u stepping %u", si.wProcessorRevision >> 8, si.wProcessorRevision & 0xFF);
		break;
	default:
		LOGMSG("\tProcessor type: Intel family %u model %u stepping %u", si.wProcessorLevel, si.wProcessorRevision >> 8, si.wProcessorRevision & 0xFF);
		break;
	}
	LOGMSG("\tActive processor mask: 0x%04x", si.dwActiveProcessorMask);
	LOGMSG("\tPage size: %u", si.dwPageSize);
	LOGMSG("\tMinimum application address: 0x%08x", si.lpMinimumApplicationAddress);
	LOGMSG("\tMaximum application address: 0x%08x", si.lpMaximumApplicationAddress);
	LOGMSG("\tAllocation granularity: 0x%08x", si.dwAllocationGranularity);

	MEMORYSTATUS	ms;
	::GlobalMemoryStatus(&ms);
	if(ms.dwLength == sizeof(ms))
	{
		LOGMSG("Memory status:");
		LOGMSG("\tTotal size of physical memory in bytes: 0x%08x", ms.dwTotalPhys);
		LOGMSG("\tSize of physical memory available in bytes: 0x%08x", ms.dwAvailPhys);
		LOGMSG("\tTotal possible size of the paging file in bytes: 0x%08x", ms.dwTotalPageFile);
		LOGMSG("\tSize of space available in the paging file in bytes: 0x%08x", ms.dwAvailPageFile);
		LOGMSG("\tTotal size of the user mode portion of the virtual address space in bytes: 0x%08x", ms.dwTotalVirtual);
		LOGMSG("\tSize of space available in the virtual address space in bytes: 0x%08x", ms.dwAvailVirtual);
	}
	else
	{
		LOGERROR("GlobalMemoryStatus() failed at %s, %d", __FILE__, __LINE__);
	}

	char	szOS[256] = "";
	OSVERSIONINFOEX	osvi;
	memset(&osvi, 0, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!::GetVersionEx((OSVERSIONINFO*)&osvi))
	{
		LOGMSG("GetVersionEx(OSVERSIONINFOEX2) failed with error:%u at %s, %d", ::GetLastError(), __FILE__, __LINE__);

		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if(::GetVersionEx((OSVERSIONINFO*)&osvi))
		{
			LOGMSG("Windows %u.%u %s SP%u.%u build(%u), platform(%u)", osvi.dwMajorVersion, osvi.dwMinorVersion,
					osvi.szCSDVersion, osvi.wServicePackMajor, osvi.wServicePackMinor, osvi.dwBuildNumber, osvi.dwPlatformId);
		}
		else
		{
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			if(::GetVersionEx((OSVERSIONINFO*)&osvi))
			{
				LOGMSG("Windows %u.%u %s build(%u), platform(%u)",
						osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.szCSDVersion, osvi.dwBuildNumber, osvi.dwPlatformId);
			}
			else
				LOGERROR("GetVersionEx(OSVERSIONINFO) failed with error:%u at %s, %d", ::GetLastError(), __FILE__, __LINE__);
		}
		return;
	}

	if(VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4)
	{
		strncat(szOS, "Microsoft ", 256);

		if(osvi.dwMajorVersion == 6)
		{
			if(osvi.dwMinorVersion == 0)
			{
				if(osvi.wProductType == VER_NT_WORKSTATION)
					strncat(szOS, "Windows Vista ", 256);
				else
					strncat(szOS, "Windows Server 2008 ", 256);

				typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
				PGPI pGPI = (PGPI)::GetProcAddress(::GetModuleHandle("kernel32.dll"), "GetProductInfo");
				MYASSERT(pGPI);
				DWORD	dwType;
				pGPI(6, 0, 0, 0, &dwType);

				switch(dwType)
				{
				case PRODUCT_ULTIMATE:
				case PRODUCT_ULTIMATE_N:
					strncat(szOS, "Ultimate Edition", 256);
					break;
				case PRODUCT_HOME_PREMIUM:
				case PRODUCT_HOME_PREMIUM_N:
					strncat(szOS, "Home Premium Edition", 256);
					break;
				case PRODUCT_HOME_BASIC:
				case PRODUCT_HOME_BASIC_N:
					strncat(szOS, "Home Basic Edition", 256);
					break;
				case PRODUCT_HOME_SERVER:
					strncat(szOS, "Home Server Edition", 256);
					break;
				case PRODUCT_BUSINESS:
				case PRODUCT_BUSINESS_N:
					strncat(szOS, "Business Edition", 256);
					break;
				case PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT:
					strncat(szOS, "Windows Essential Business Server Management Server", 256);
					break;
				case PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING:
					strncat(szOS, "Windows Essential Business Server Messaging Server", 256);
					break;
				case PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY:
					strncat(szOS, "Windows Essential Business Server Security Server", 256);
					break;
				case PRODUCT_STARTER:
					strncat(szOS, "Starter Edition", 256);
					break;
				case PRODUCT_CLUSTER_SERVER:
					strncat(szOS, "Cluster Server Edition", 256);
					break;
				case PRODUCT_DATACENTER_SERVER:
					strncat(szOS, "Datacenter Edition", 256);
					break;
				case PRODUCT_DATACENTER_SERVER_CORE:
					strncat(szOS, "Datacenter Edition (core installation)", 256);
					break;
				case PRODUCT_ENTERPRISE:
				case PRODUCT_ENTERPRISE_N:
				case PRODUCT_ENTERPRISE_SERVER:
					strncat(szOS, "Enterprise Edition", 256);
					break;
				case PRODUCT_ENTERPRISE_SERVER_CORE:
					strncat(szOS, "Enterprise Edition (core installation)", 256);
					break;
				case PRODUCT_ENTERPRISE_SERVER_IA64:
					strncat(szOS, "Enterprise Edition for Itanium-based Systems", 256);
					break;
				case PRODUCT_SMALLBUSINESS_SERVER:
					strncat(szOS, "Small Business Server", 256);
					break;
				case PRODUCT_SERVER_FOR_SMALLBUSINESS:
					strncat(szOS, "Server for Small Business Edition", 256);
					break;
				case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
					strncat(szOS, "Small Business Server Premium Edition", 256);
					break;
				case PRODUCT_STANDARD_SERVER:
					strncat(szOS, "Standard Edition", 256);
					break;
				case PRODUCT_STANDARD_SERVER_CORE:
					strncat(szOS, "Standard Edition (core installation)", 256);
					break;
				case PRODUCT_STORAGE_ENTERPRISE_SERVER:
					strncat(szOS, "Storage Server Enterprise Edition", 256);
					break;
				case PRODUCT_STORAGE_EXPRESS_SERVER:
					strncat(szOS, "Storage Server Express Edition", 256);
					break;
				case PRODUCT_STORAGE_STANDARD_SERVER:
					strncat(szOS, "Storage Server Standard Edition", 256);
					break;
				case PRODUCT_STORAGE_WORKGROUP_SERVER:
					strncat(szOS, "Storage Server Workgroup Edition", 256);
					break;
				case PRODUCT_WEB_SERVER:
					strncat(szOS, "Web Server Edition", 256);
					break;
				case PRODUCT_WEB_SERVER_CORE:
					strncat(szOS, "Web Server Edition (core installation)", 256);
					break;
				default:
					strncat(szOS, "Windows 6.0", 256);
					break;
				}

				if(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
					strncat(szOS, ", 32-bit", 256);
				else if(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					strncat(szOS, ", 64-bit", 256);
				else
					strncat(szOS, ", UnkonwnArch", 256);
			}
			else
			{
				strncat(szOS, ", Windows 6.x", 256);
			}
			return;
		}
		else if(osvi.dwMajorVersion == 5)
		{
			switch(osvi.dwMinorVersion)
			{
			case 2:
				{
					if(::GetSystemMetrics(SM_SERVERR2))
						strncat(szOS, "Windows Server 2003 R2, ", 256);
					else if(osvi.wSuiteMask == 0x200)
						strncat(szOS, "Windows Storage Server 2003", 256);
					else if(osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
						strncat(szOS, "Windows XP Professional x64 Edition", 256);
					else
						strncat(szOS, "Windows Server 2003, ", 256);

					if(osvi.wProductType != VER_NT_WORKSTATION)
					{
						if(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
						{
							if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
								strncat(szOS, "Datacenter Edition for Itanium-based Systems", 256);
							else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
								strncat(szOS, "Enterprise Edition for Itanium-based Systems", 256);
						}
						else if(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
						{
							if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
								strncat(szOS, "Datacenter x64 Edition", 256);
							else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
								strncat(szOS, "Enterprise x64 Edition", 256);
							else
								strncat(szOS, "Standard x64 Edition", 256);
						}
						else
						{
							if(osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
								strncat(szOS, "Compute Cluster Edition", 256);
							else if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
								strncat(szOS, "Datacenter Edition", 256);
							else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
								strncat(szOS, "Enterprise Edition", 256);
							else if(osvi.wSuiteMask & VER_SUITE_BLADE)
								strncat(szOS, "Web Edition", 256);
							else
								strncat(szOS, "Standard Edition", 256);
						}
					}
				}
				break;
			case 1:
				{
					strncat(szOS, "Windows XP ", 256);
					if(osvi.wSuiteMask & VER_SUITE_PERSONAL)
						strncat(szOS, "Home Edition", 256);
					else
						strncat(szOS, "Professional", 256);
				}
				break;
			case 0:
				{
					strncat(szOS, "Windows 2000 ", 256);
					if(osvi.wProductType == VER_NT_WORKSTATION)
					{
						strncat(szOS, "Professional", 256);
					}
					else
					{
						if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
							strncat(szOS, "Datacenter Server", 256);
						else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
							strncat(szOS, "Advanced Server", 256);
						else
							strncat(szOS, "Server", 256);
					}
				}
				break;
			}
		}
		else
		{
			LOGERROR("Bad Windows version %u.%u build(%u) platform(%u)",
					osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber, osvi.dwPlatformId);
			return;
		}

		if(strlen(osvi.szCSDVersion) > 0)
		{
			strncat(szOS, " ", 256);
			strncat(szOS, osvi.szCSDVersion, 256);
		}

		char	buf[80];
		_snprintf(buf, 80, " (build %d)", osvi.dwBuildNumber);
		strncat(szOS, buf, 256);

		LOGMSG(szOS);
	}
	else
	{
		LOGERROR("Unsupported Windows version %u.%u build(%u) platform(%u)",
				osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber, osvi.dwPlatformId);
	}
}

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

CMsgServerDlg::CMsgServerDlg(CWnd* pParent )
	: CDialog(CMsgServerDlg::IDD, pParent)
{
	m_sKernelState = _T("");
	m_sShellState = _T("");
	m_sTalk = _T("");
	m_sText = _T("");
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nState		= SHELLSTATE_NONE;
	m_nTextLines	= 0;
	m_pMsgPort		= NULL;
	m_pInterPort		= NULL;
	m_pSocketThread	= NULL;
	m_pWorldThread	= NULL;

	m_hMutexThread	= NULL;

}

void CMsgServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_KERNEL, m_sKernelState);
	DDX_Text(pDX, IDC_SHELL, m_sShellState);
	DDX_Text(pDX, IDC_TALK, m_sTalk);
	DDX_Text(pDX, IDC_TEXT, m_sText);
}

BEGIN_MESSAGE_MAP(CMsgServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CMsgServerDlg::OnInitDialog()
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

#if	!defined(PALED_DEBUG)
#ifdef _DEBUG
	this->MessageBox("This server build is for internal use only; running it outside is strictly forbidden!", "Warning", MB_OK|MB_ICONEXCLAMATION);
#endif
#endif

	char	szExitEvent[] = "TQ ConqerOnline Exit Event";
	m_hExitEvent = ::CreateEvent(NULL, TRUE, FALSE, szExitEvent);
	if(!m_hExitEvent)
	{
		MessageBox("Create exit event failed!");
		this->EndDialog(-1);
		return false;
	}

	memset(&g_stat, 0, sizeof(g_stat));
	DateTime(m_szStartServer, time(NULL));

	m_hMutexServer = ::CreateMutex(NULL, false, "ConquerServer");
	if (m_hMutexServer)
	{
		::GetLastError();
	}
	else
	{
		MessageBox("Create mutex failed!");
		this->EndDialog(-1);
		return false;
	}

#ifdef	DEBUG_MULTITHREAD
	m_hMutexThread    =::CreateMutex(NULL, false, "FW_DEBUG_MULTITHREAD");
	if(!m_hMutexThread)
	{
		PrintText("Create mutex handle failed!");
		return false;
	}
#endif
	if(!LoadConfigIni())
		MessageBox("Load config.ini failed!");
	else if(!CMessagePort::InitPortSet(MSGPORT_MAPGROUP_FIRST + CONFIG.MAPGROUP_SIZE))
		MessageBox("Initial intra message port failed!");
	else
	{
		m_pMsgPort = CMessagePort::GetInterface(MSGPORT_SHELL);
		m_pMsgPort->Open();
		m_nState	= SHELLSTATE_INIT;

		if(CONFIG.CURRENT_PORTID)
		{
			m_pInterPort = CInternetPort::CreateNew(CONFIG.CURRENT_PORTID, CONFIG.PORT_SIZE, CONFIG.MASTER_IP, CONFIG.MASTER_PORT, CONFIG.LOGIN_KEY);
			if(m_pInterPort)
				m_pInterPort->Open();
		}
	}

	for(int i = 0; i < CONFIG.MAPGROUP_SIZE + MSGPORT_MAPGROUP_FIRST; i++)
		DeadLoopSet().Push(DEADLOOP());

	CIniFile	ini("shell.ini", "AccountServer");
	ini.GetString(m_szServer, "SERVERNAME", _MAX_NAMESIZE);

	CString strTitle;
	strTitle.Format("%s - %s - %s %s", GAME_TITLE, m_szServer, __DATE__, __TIME__);
	SetWindowText(strTitle);

	CreateDirectory(LOGFILE_DIR, NULL);
	InitLog(strTitle, LOGFILE_FILENAME, time(NULL));
	LOGMSG("=================================================================");
	LOGMSG(strTitle);
	LOGMSG("=================================================================");
	LogSystemInfo();

	SetTimer(1, 200, NULL);

	strTitle.Format("%s - %s", GAME_TITLE, m_szServer);
	::SetProp(GetSafeHwnd(), strTitle, (HANDLE)1);

	return TRUE;
}

void CMsgServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMsgServerDlg::OnPaint()
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

HCURSOR CMsgServerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMsgServerDlg::OnTimer(UINT nIDEvent)
{
#ifdef	DEBUG_MULTITHREAD
	if(::WaitForSingleObject(m_hMutexThread, INFINITE) == WAIT_ABANDONED)
		return ;
#endif

	if(!g_bExitEvent && ::WaitForSingleObject(m_hExitEvent, 20) == WAIT_OBJECT_0)
	{
		char	szCmd[] = "shutdown";
		g_bExitEvent	= true;
		m_pMsgPort->Send(MSGPORT_WORLD, WORLD_SHELLTALK, STRING_TYPE(szCmd), szCmd);
		m_tShutdown.Startup(50);
	}
	else if(m_tShutdown.TimeOver())
	{
		char	szCmd[] = "kickoutall";
		m_pMsgPort->Send(MSGPORT_WORLD, WORLD_SHELLTALK, STRING_TYPE(szCmd), szCmd);
		m_tKickout.Startup(10);
	}
	else if(m_tKickout.TimeOver())
	{
		m_nState	= SHELLSTATE_CLOSING;
		PrintText("Server closing...");
	}

	static	int nLock = 0;
	if(nIDEvent == 1 && nLock <= 0)
	{
		nLock++;

		if(m_pMsgPort)
			Process();

		DEBUG_TRY
		if(m_pInterPort)
			ProcessInterMsg();
		DEBUG_CATCH("ProcessInterMsg()")

		extern long				g_nRestart;
		switch(m_nState)
		{
		case	SHELLSTATE_NONE:
			{
				m_nState	= SHELLSTATE_END;
			}
			break;
		case	SHELLSTATE_INIT:
			{
				m_nState	= SHELLSTATE_CLOSING;

				PrintText("1. Init world thread...");
				m_pWorldThread = new CWorldThread(CMessagePort::GetInterface(MSGPORT_WORLD));
				if(!m_pWorldThread)
					break;
				if(!m_pWorldThread->CreateThread(false))
					break;

				PrintText("2. Init map group thread...");
				int i; for(i = 0; i < CONFIG.MAPGROUP_SIZE; i++)
				{
					CMapGroupThread* pThread = new CMapGroupThread(CMessagePort::GetInterface(i + MSGPORT_MAPGROUP_FIRST));
					if(!pThread)
						break;
					m_setMapGroupThread.push_back(pThread);
					if(!pThread->CreateThread(false))
						break;
				}
				if(i < CONFIG.MAPGROUP_SIZE)
					break;

				PrintText("3. Init socket thread...");
				m_pSocketThread = new CSocketThread(CMessagePort::GetInterface(MSGPORT_SOCKET));
				if(!m_pSocketThread)
					break;
				if(!m_pSocketThread->CreateThread(false))
					break;
				PrintText("Init OK.");

				m_pWorldThread->ResumeThread();
				for( i = 0; i < CONFIG.MAPGROUP_SIZE; i++)
					m_setMapGroupThread[i]->ResumeThread();
				m_pSocketThread->ResumeThread();
				PrintText("All thread start OK.");

				m_nState	= SHELLSTATE_RUNNING;
			}
			break;
		case	SHELLSTATE_RUNNING:
			{
				const int nSecsPerLogin = 5*60;
				if(m_tStat5Min.ToNextTime(nSecsPerLogin))
				{
					InterlockedExchange(&g_stat.nLoginPlayers, 0);
					InterlockedExchange(&g_stat.nLogoutPlayers, 0);

					LOGMSG("\n-----------------------------------------------\n"
							"%s"
							"-----------------------------\n"
							"%s"
							"-----------------------------------------------\n",
							m_sShellState, m_sKernelState);
				}
				const int nSecsPerUpdate = 5;
				if(m_tStat.ToNextTime(nSecsPerUpdate))
				{
					char	szCurr[20];
					DateTime(szCurr, time(NULL));

					m_sShellState.Format("start server: %s\r\n"
											"current: %s\r\n\r\n"
											"socket thread: %3d(max:%d)\r\n"
											"world thread:  %3d(max:%d)\r\n"
											"map group 0 thread: %3d(max:%d)[%3d]\r\n"
											"map group 1 thread: %3d(max:%d)\r\n"
											"map group 2 thread: %3d(max:%d)\r\n"
											"map group 3 thread: %3d(max:%d)\r\n"
											"map group 4 thread: %3d(max:%d)\r\n"
											"debug [%d][%d][%d][%d][%d]\r\n"
											, m_szStartServer
											, szCurr
											, g_stat.nSocketTimes ? g_stat.nAllSocketMS/g_stat.nSocketTimes : 0
												, g_stat.nMaxSocketMS
											, g_stat.nWorldTimes ? g_stat.nAllWorldMS/g_stat.nWorldTimes : 0
												, g_stat.nMaxWorldMS
											, g_stat.setMapGroupTimes[0] ? g_stat.setAllMapGroupMS[0]/g_stat.setMapGroupTimes[0] : 0
												, g_stat.setMaxMapGroupMS[0]
												, g_stat.nPacketID
											, g_stat.setMapGroupTimes[1] ? g_stat.setAllMapGroupMS[1]/g_stat.setMapGroupTimes[1] : 0
												, g_stat.setMaxMapGroupMS[1]
											, g_stat.setMapGroupTimes[2] ? g_stat.setAllMapGroupMS[2]/g_stat.setMapGroupTimes[2] : 0
												, g_stat.setMaxMapGroupMS[2]
											, g_stat.setMapGroupTimes[3] ? g_stat.setAllMapGroupMS[3]/g_stat.setMapGroupTimes[3] : 0
												, g_stat.setMaxMapGroupMS[3]
											, g_stat.setMapGroupTimes[4] ? g_stat.setAllMapGroupMS[4]/g_stat.setMapGroupTimes[4] : 0
												, g_stat.setMaxMapGroupMS[4]
											, g_stat.setDebug[0], g_stat.setDebug[1], g_stat.setDebug[2], g_stat.setDebug[3], g_stat.setDebug[4]
											);
					m_sKernelState.Format("players: %3d(max:%d)\r\n"
											"login: %d/5min, logout: %d/5min\r\n\r\n"
											"socket bytes: %3d/s (pps: %d)\r\n"
											"npc socket bytes: %3d/s (pps: %d)\r\n"
											, g_stat.nAllPlayers, g_stat.nMaxPlayers
											, g_stat.nLoginPlayers, g_stat.nLogoutPlayers
											, g_stat.nSocketBytes/nSecsPerUpdate, g_stat.nSocketPackets/nSecsPerUpdate
											, g_stat.nNpcSocketBytes/nSecsPerUpdate, g_stat.nNpcSocketPackets/nSecsPerUpdate
											);
					UpdateData(false);
					FILE* pFile = fopen(ONLINE_FILENAME, "wt");
					if(pFile)
					{
						fprintf(pFile, m_sShellState);
						fprintf(pFile, "\n\n");
						fprintf(pFile, m_sKernelState);
						fclose(pFile);
					}
				}

				extern long g_nMessagePortErrorCount;
				if(g_nMessagePortErrorCount > 10000 && g_nMessagePortErrorCount%1000 == 0)
				{
					InterlockedExchange(&g_nMessagePortErrorCount, 0);

					PrintText("Message pipe block, server may be slowly!!!");
					LOGERROR("dump all call stack:");
					CMySingleLock xLock(&CDeadLoop::s_xCtrl, true);
					while(DeadLoopSet()[MSGPORT_MAPGROUP_FIRST].size())
						LOGERROR("* %s", (LPCTSTR)DeadLoopSet()[MSGPORT_MAPGROUP_FIRST].pop());
				}

				extern long g_nRestart;
				if(g_nRestart)
					m_nState = SHELLSTATE_CLOSING;
			}
			break;
		case	SHELLSTATE_CLOSING:
			{
				PrintText("Kernel closing...");

				int i;
				if(m_pSocketThread)
					m_pSocketThread->CloseThread();
				for( i = 0; i < m_setMapGroupThread.size(); i++)
					m_setMapGroupThread[i]->CloseThread();
				if(m_pWorldThread)
					m_pWorldThread->CloseThread();
				if(m_pSocketThread)
				{
					if(m_pSocketThread->CloseThread(CLOSETHREAD_MILLISECS))
						PrintText("Close socket OK.");
					else
						PrintText("Close socket failed!");
					SAFE_DELETE(m_pSocketThread);
					Process();
				}
				for( i = 0; i < m_setMapGroupThread.size(); i++)
				{
					char szText[1024];
					if(m_setMapGroupThread[i]->CloseThread(CLOSETHREAD_MILLISECS))
						sprintf(szText, "Close map group thread[%d] OK.", i);
					else
						sprintf(szText, "Close map group thread[%d] failed!", i);
					PrintText(szText);
					SAFE_DELETE(m_setMapGroupThread[i]);
					Process();
				}
				if(m_pWorldThread)
				{
					if(m_pWorldThread->CloseThread(CLOSETHREAD_MILLISECS))
						PrintText("Close world thread OK.");
					else
						PrintText("Close world thread failed!");
					SAFE_DELETE(m_pWorldThread);
					Process();
				}

				if (m_hMutexServer)
				{
					::WaitForSingleObject(m_hMutexServer, 10*1000);
					::ReleaseMutex(m_hMutexServer);
					::CloseHandle(m_hMutexServer);
					m_hMutexServer = NULL;
				}

				PrintText("Server is over, close all  after 3 second!");
				Process();

				m_pMsgPort->Close();
				CMessagePort::ClearPortSet();
				m_pMsgPort = NULL;

				if(m_pInterPort)
				{
					SAFE_DELETE(m_pInterPort);
				}

				m_nState	= SHELLSTATE_END;
			}
			break;
		case	SHELLSTATE_END:
			{
				KillTimer(1);
#ifdef	DEBUG_MULTITHREAD
	if(m_hMutexThread)
	{
		::CloseHandle(m_hMutexThread);
		m_hMutexThread = NULL;
	}
#endif
				Sleep(3000);

				if(g_nRestart)
					::ShellExecute(m_hWnd, NULL, AfxGetAppName(), NULL, NULL, SW_SHOWNORMAL);

				CDialog::OnOK();
			}
			break;
		default:
			MYASSERT(!"switch(m_nState)");
		}

		nLock--;
	}

#ifdef	DEBUG_MULTITHREAD
	::ReleaseMutex(m_hMutexThread);
#endif

	CDialog::OnTimer(nIDEvent);
}

void CMsgServerDlg::OnCancel()
{
	PrintText("CLOSE key pressed");
	if(AfxMessageBox("Are you sure?", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK)
	{
		m_nState	= SHELLSTATE_CLOSING;
		PrintText("Server closing...");
	}

}

void CMsgServerDlg::OnSend()
{
	this->UpdateData(TRUE);
	if(m_pMsgPort)
	{
		m_pMsgPort->Send(MSGPORT_WORLD, WORLD_SHELLTALK, STRING_TYPE((LPCTSTR)m_sTalk), (LPCTSTR)m_sTalk);
	}
}

bool CMsgServerDlg::Process()
{
	char			buf[MAX_MESSAGESIZE];
	CMessageStatus	cStatus;

	DEBUG_TRY
	while(m_pMsgPort->Recv(PORT_ANY, PACKET_ANY, STRUCT_TYPE(buf), buf, &cStatus))
	{
		if(ProcessMsg(cStatus.m_nPacket, buf, cStatus.m_nVarType, cStatus.m_nPortFrom))
			return true;

		PrintText("ERROR: CMsgServerDlg::Process() ProcessMsg()");
		return false;
	}
	DEBUG_CATCH("CMsgServerDlg::Process()")

	return true;
}

bool CMsgServerDlg::ProcessInterMsg()
{
	if(!m_pInterPort)
		return false;

	char			buf[MAX_MESSAGESIZE];
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
				m_pInterPort->Send(cStatus.m_nPortFrom, ACK_TITLE, STRING_TYPE(m_szServer), m_szServer);
				m_pInterPort->Send(cStatus.m_nPortFrom, ACK_SHELLMSG, STRING_TYPE(m_sShellState), m_sShellState);
				m_pInterPort->Send(cStatus.m_nPortFrom, ACK_KERNELMSG, STRING_TYPE(m_sKernelState), m_sKernelState);
				m_pInterPort->Send(cStatus.m_nPortFrom, ACK_TEXT, STRING_TYPE(m_sText), m_sText);
			}
			break;
		case	QUERY_COMMOND:
			{
				LPCTSTR pStr = (LPCTSTR)buf;
				CString str;
				str.Format("【remote】%s", pStr);
				PrintText(str);
				m_pMsgPort->Send(MSGPORT_WORLD, WORLD_SHELLTALK, STRING_TYPE(pStr), pStr);
			}
			break;
		default:
			PrintText("CMsgServerDlg::ProcessInterMsg()");
			return false;
		}
	}
	DEBUG_CATCH("CMsgServerDlg::Process()")

	return true;
}

void CMsgServerDlg::PrintText(LPCTSTR szMsg)
{
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

	UpdateData(false);

	LOGMSG("SHELL: %s", szMsg);
}

bool CMsgServerDlg::ProcessMsg(OBJID idPacket, void* pMsg, int nType, int nSource)
{
	switch(idPacket)
	{
	case	SHELL_PRINTTEXT:
		{
			PrintText((char*)pMsg);
		}
		break;
	case	SHELL_KERNELSTATE:
		{
			m_sKernelState = (char*)pMsg;
			UpdateData(false);
		}
	default:
		LOGERROR("Shell process invalid msg [%d].", idPacket);
		return false;
	}
	return true;
}

bool CMsgServerDlg::LoadConfigIni()
{
	CIniFile	ini(CONFIG_FILENAME, "System");

	CONFIG.MAPGROUP_SIZE	= 0;
	CONFIG.CURRENT_PORTID	= 0;
	CONFIG.PORT_SIZE		= 0;
	CONFIG.MASTER_IP[0]		= 0;
	CONFIG.MASTER_PORT		= 0;
	CONFIG.LOGIN_KEY[0]		= 0;

	CONFIG.MAPGROUP_SIZE	= ini.GetInt("MAPGROUP_SIZE");
	if(CONFIG.MAPGROUP_SIZE == 0)
		return false;

	int	nFreeLevel	= ini.GetInt("FREE_LEV");
	if(nFreeLevel)
		LOGIN_FREE_LEVEL	= nFreeLevel;

	ini.SetSection("InternetPort");
	CONFIG.CURRENT_PORTID	= ini.GetInt("CURRENT_PORTID");
	CONFIG.PORT_SIZE		= ini.GetInt("PORT_SIZE");
	ini.GetString(CONFIG.MASTER_IP, "MASTER_IP", IPSTR_SIZE);
	CONFIG.MASTER_PORT		= ini.GetInt("MASTER_PORT");
	ini.GetString(CONFIG.LOGIN_KEY, "LOGIN_KEY", 256);
	if(CONFIG.MAPGROUP_SIZE == 0)
		return false;

	return true;
}

void CMsgServerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	::CloseHandle(m_hExitEvent);
	CString	str;
	str.Format("%s - %s", GAME_TITLE, m_szServer);
	::RemoveProp(GetSafeHwnd(), str);
}
