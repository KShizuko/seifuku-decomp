#if !defined(AFX_NPCSERVER_H__8E808F2F_DE9F_4DAA_B75A_F9ACC61FDE4A__INCLUDED_)
#define AFX_NPCSERVER_H__8E808F2F_DE9F_4DAA_B75A_F9ACC61FDE4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CNpcServerApp : public CWinApp
{
public:
	CNpcServerApp();

	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	HANDLE	m_hMutex;

	DECLARE_MESSAGE_MAP()
};

#endif
