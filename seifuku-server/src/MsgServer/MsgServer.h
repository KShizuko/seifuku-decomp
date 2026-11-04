#if !defined(AFX_MSGSERVER_H__24319C41_56CE_4280_9FBE_DCFD6CC53BDE__INCLUDED_)
#define AFX_MSGSERVER_H__24319C41_56CE_4280_9FBE_DCFD6CC53BDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CMsgServerApp : public CWinApp
{
public:
	CMsgServerApp();

	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	HANDLE	m_hMutex;

	DECLARE_MESSAGE_MAP()
};

#endif
