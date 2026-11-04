#if !defined(AFX_STDAFX_H__2DEF01BC_6018_41FB_9A48_4D2196F1C7C1__INCLUDED_)
#define AFX_STDAFX_H__2DEF01BC_6018_41FB_9A48_4D2196F1C7C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#define _WINSOCKAPI_
#define	FD_SETSIZE			600
#include <winsock2.h>

#define VC_EXTRALEAN

#include <afxwin.h>
#include <afxext.h>
#include <afxdtctl.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif

#define MFC_VER_4
#define INDEX           register int
#define WINDEX          register WORD
#define LINDEX          register long
#define DWINDEX         register DWORD
typedef signed  char    INT8  ;
typedef unsigned char   UINT8 ;
typedef signed  short   INT16 ;
typedef unsigned short  UINT16;

#endif
