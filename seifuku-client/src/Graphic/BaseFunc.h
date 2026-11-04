#ifndef _BASEFUNC_H
#define _BASEFUNC_H
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include <windows.H>

#pragma warning(disable: 4786)

typedef DWORD OBJID;

typedef struct
{
	int	x, y;
}CMyPos;

typedef struct {
	int	iWidth, iHeight;
}CMySize;

DWORD	Str2ID		(const char* str);
DWORD	TimeGet		(void);
BOOL	ReadLine	(const char* pBuf, DWORD dwSize, DWORD* pdwPos, char* szLine, DWORD dwMax);
BOOL	IniDataGet	(const char *pszFileName, const char *pszTitle, const char *pszSubTitle, int &iData, BOOL bCritical=false);
BOOL	IniStrGet	(const char *pszFileName, const char *pszTitle, const char *pszSubTitle, char *pszStr, BOOL bCritical=false);
int		RandGet		(int nMax, BOOL bRealRand=false);
void	ErrorMsg	(const char* fmt, ...);
void	LogMsg		(const char* fmt, ...);
void	DebugMsg	(const char* fmt, ...);
void	RepairString	(char* pszString);
void	ReplaceString	(char* pszString, char cFind, char cReplace);

#define SAFE_DELETE(p) { if(p){ try{ delete p; } catch(...){} p=NULL; }}

#define	MYASSERT(x)	{if(!(x)) LogMsg("★ASSERT(" #x ") in %d line of %s★", __LINE__, __FILE__); }
#define	IF_SUC(x)  if( ((x)) ? true : ( LogMsg("★IF_SUC(%s)★ failed in %s, %d", #x, __FILE__, __LINE__), false ) )
#define	IF_NOT(x)  if( (!(x)) ? ( LogMsg("★IF_NOT(%s)★ in %s, %d", #x, __FILE__, __LINE__),1 ) : 0 )

#endif
