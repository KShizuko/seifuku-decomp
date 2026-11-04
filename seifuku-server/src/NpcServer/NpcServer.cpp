#include "stdafx.h"
#include "NpcServer.h"
#include "NpcServerDlg.h"
#include "IniFile.h"

BEGIN_MESSAGE_MAP(CNpcServerApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CNpcServerApp::CNpcServerApp()
{
	m_hMutex	= NULL;
}

CNpcServerApp theApp;

BOOL CNpcServerApp::InitInstance()
{
	CIniFile	ini("shell.ini", "AccountServer");
	char	szServerName[16];
	ini.GetString(szServerName, "SERVERNAME", 16);
	CString	strTitle;
	strTitle.Format("%s - %s", NPCSERVER_TITLE, szServerName);

	m_hMutex = ::CreateMutex(NULL, TRUE, strTitle);
	if(!m_hMutex)
	{
		::MessageBox(NULL, "Create mutext failed.", strTitle, MB_ICONEXCLAMATION);
	}
	else if(::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		::CloseHandle(m_hMutex);
		m_hMutex = NULL;

		HWND hWnd = ::GetWindow(::GetDesktopWindow(), GW_CHILD);
		while(::IsWindow(hWnd))
		{
			if(::GetProp(hWnd, strTitle))
			{
				if(::IsIconic(hWnd))
					::ShowWindow(hWnd, SW_RESTORE);
				::SetForegroundWindow(hWnd);
				return FALSE;
			}
			hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
		}
		::MessageBox(NULL, "NpcServer is running, but I can't find its window.", strTitle, MB_ICONEXCLAMATION);
	}
	else
	{

#ifdef _AFXDLL
		Enable3dControls();
#else
#endif

		CNpcServerDlg dlg;
		m_pMainWnd = &dlg;
		int nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
		}
		else if (nResponse == IDCANCEL)
		{
		}
	}

	return FALSE;
}

int CNpcServerApp::ExitInstance()
{
	if(m_hMutex)
	{
		::CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
	return CWinApp::ExitInstance();
}
