//////////////////////////////////////////////////////////////////////
// Purpose:		base function for game
//
// WriteBy:		blacknull	10-16-2002
//
// ModifyBy:	blacknull	10-16-2002
//
// CopyRight:	Acme Entertrainment, copyright 2002
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "BaseFunc.h"
#include <time.h>
#include <mmsystem.h>
#include <direct.h>

HWND g_hGameWnd = NULL;
BOOL g_bLocalVersion = false;

#ifdef _ANALYSIS_ON
	DWORD g_dwIniGetAmount = 0;
	DWORD g_dwIniSearchLineAmount = 0;
	DWORD g_dwIniGetAmountFrame = 0;
	DWORD g_dwIniSearchLineAmountFrame = 0;
#endif
int	Double2Int(double dValue)
{
	if((int)(dValue+0.5) >(int)dValue)
		return int(dValue)+1;
	else
		return int(dValue);
}

BOOL IniStrGet(const char *pszFileName, const char *pszTitle, const char *pszSubTitle, char *pszStr, BOOL bCritical )
{
#ifdef _ANALYSIS_ON
	g_dwIniGetAmount ++;
	g_dwIniGetAmountFrame++;
#endif
	if (!pszFileName || !pszTitle || !pszSubTitle || !pszStr)
		return false;

	FILE* fp = fopen(pszFileName, "r");
	if (!fp)
		return false;

	char szSection[256]	= "";
	sprintf(szSection, "[%s]", pszTitle);

	bool bSucFound	=false;
	while(true)
	{
		char szLine[1024]	="";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;

#ifdef _ANALYSIS_ON
		g_dwIniSearchLineAmount ++;
		g_dwIniSearchLineAmountFrame++;
#endif

		if (0 != stricmp(szSection, szLine))
			continue;

		char szFormat[256] = "";
		sprintf(szFormat, "%s=%s\n", pszSubTitle, "%s");

		while (true)
		{
			char szLine[1024]	="";
#ifdef _ANALYSIS_ON
			g_dwIniSearchLineAmount ++;
			g_dwIniSearchLineAmountFrame++;
#endif
			if (EOF == fscanf(fp, "%s\n", szLine))
				goto out;

			if (strrchr(szLine, '[') && strrchr(szLine, ']'))
				goto out;

			if (sscanf(szLine, szFormat, pszStr))
			{
				bSucFound = true;
				goto out;
			}
		}
	}

out:
	fclose(fp);

	if (!bSucFound)
	{
		if (bCritical)
		{
			char szMsg[1024] = "";
			sprintf(szMsg, "Error: [%s] %s not found in %s.", pszTitle, pszSubTitle, pszFileName);

			::LogMsg(szMsg);
		}
		else
			::DebugMsg("Error: [%s] %s not found in %s.", pszTitle, pszSubTitle, pszFileName);
	}

	return bSucFound;
}

BOOL IniDataGet(const char *pszFileName, const char *pszTitle, const char *pszSubTitle, int &iData, BOOL bCritical )
{
	if (!pszFileName || !pszTitle || !pszSubTitle)
		return false;

#ifdef _ANALYSIS_ON
	g_dwIniGetAmount ++;
	g_dwIniGetAmountFrame++;
#endif
	FILE* fp = fopen(pszFileName, "r");
	if (!fp)
		return false;

	char szSection[256]	= "";
	sprintf(szSection, "[%s]", pszTitle);

	bool bSucFound	=false;
	while(true)
	{
		char szLine[1024]	="";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;


		#ifdef _ANALYSIS_ON
			g_dwIniSearchLineAmount ++;
			g_dwIniSearchLineAmountFrame++;
		#endif
			if (0 != stricmp(szSection, szLine))
			continue;

		char szFormat[256] = "";
		sprintf(szFormat, "%s=%s\n", pszSubTitle, "%d");

		while (true)
		{
			#ifdef _ANALYSIS_ON
				g_dwIniSearchLineAmount ++;
				g_dwIniSearchLineAmountFrame++;
			#endif
			char szLine[1024]	="";
			if (EOF == fscanf(fp, "%s\n", szLine))
				goto out;

			if (strrchr(szLine, '[') && strrchr(szLine, ']'))
				goto out;

			if (sscanf(szLine, szFormat, &iData))
			{
				bSucFound = true;
				goto out;
			}
		}
	}

out:
	fclose(fp);

	if (!bSucFound)
	{
		if (bCritical)
		{
			char szMsg[1024] = "";
			sprintf(szMsg, "Error: [%s] %s not found in %s.", pszTitle, pszSubTitle, pszFileName);

			::LogMsg(szMsg);
		}
		else
			::DebugMsg("Error: [%s] %s not found in %s.", pszTitle, pszSubTitle, pszFileName);
	}

	return bSucFound;
}

DWORD TimeGet(void)
{
	return timeGetTime();
}

DWORD SysTimeGet(void)
{
	time_t long_time;
	time( &long_time );

	struct tm *pTime;
	pTime = localtime( &long_time );

	DWORD dwTime	=pTime->tm_hour*100 + pTime->tm_min;
	return dwTime;
}

void LogMsg (const char* fmt, ...)
{
	if (!fmt)
		return;

    char buffer[1024] = "";
    vsprintf( buffer, fmt, (char*) ((&fmt)+1) );

	static char s_szMsg[8][1024];
	static unsigned int s_nIndex = 0;
	int i;
	for (i = 0; i < 8; i++)
	{
		if (strcmp(s_szMsg[i], buffer) == 0)
			break;
	}
	if (i < 8)
		return;

	strcpy(s_szMsg[s_nIndex++], buffer);
	s_nIndex %= 8;

	time_t ltime;
	time( &ltime );

	struct tm *pTime;
	pTime = localtime( &ltime );

	char szLogName[256] = "debug.log";
	_mkdir("debug");
	sprintf(szLogName, "./debug/%u_%u_%u.log", pTime->tm_year+1900, pTime->tm_mon+1, pTime->tm_mday);

	FILE* fp	=fopen(szLogName, "a+");
	if(!fp)
		return;

	fprintf(fp, "%s -- %s", buffer, ctime(&ltime));
	fclose(fp);
}

void DebugMsg (const char* fmt, ...)
{
    char szMsg[1024];
    vsprintf( szMsg, fmt, (char*) ((&fmt)+1) );
	strcat(szMsg, "\n");

	::OutputDebugString(szMsg);
}

void ErrorOut (const char* fmt, ...)
{
    char szMsg[1024];
    vsprintf( szMsg, fmt, (char*) ((&fmt)+1) );

	LogMsg(szMsg);
}

void ErrorMsg(const char* fmt, ...)
{
    char szMsg[1024];
    vsprintf( szMsg, fmt, (char*) ((&fmt)+1) );

	LogMsg(szMsg);
}

void	SafeCopy(char* pszDest, const char* pszSrc, int nSize)
{
	if (!pszDest || !pszSrc || nSize <= 0)
		return;
	if ((int)strlen(pszSrc) >= nSize)
	{
		strncpy(pszDest, pszSrc, nSize - 1);
		pszDest[nSize - 1] = 0;
	}
	else
		strcpy(pszDest, pszSrc);
}

int	RandGet(int nMax, BOOL bRealRand)
{
	if(nMax == 0)
		return 0;
	if (bRealRand)
		::srand( (unsigned)TimeGet() );

	return ::rand()%nMax;
}

DWORD	Str2ID(const char* str)
{
	DWORD dwId = 0;
	for(const char* p = str; *p; p ++)
		dwId = dwId * 33 + *p;
	return dwId;
}

void	RepairString	(char* pszString)
{
}

BOOL IsDBCSLeadByte	(const char* pszString, int idx)
{
	if (!pszString || (DWORD)idx >= strlen(pszString))
		return FALSE;
	
	BOOL bLeadByte = FALSE;
	for (int i = 0; i < idx; i++)
	{
		if (bLeadByte)
		{
			bLeadByte = FALSE;
			continue;
		}
		
		bLeadByte = ::IsDBCSLeadByte(pszString[i]);
	}
	
	
	return bLeadByte ? FALSE : IsDBCSLeadByte(pszString[idx]);
}

void	ReplaceString(char* pszString, char cFind, char cReplace)
{
	if(!pszString)
		return;
	DWORD dwLength = strlen(pszString);
	if (dwLength > 1024)
		return;
	for(DWORD i = 0; i < dwLength; i ++)
	{
		if(pszString[i] == cFind)
			pszString[i] = cReplace;
	}
}
DWORD GetDate()
{
	time_t ltime = time(NULL);
	struct tm *pTime;
	pTime = localtime( &ltime );
	return pTime->tm_mday;
}

static CMouseInfo infoMouse;

void MouseInit(void)
{
	infoMouse.iEvent	=_MOUSE_NONE;
}

void MouseSet(int x, int y, int event)
{
	infoMouse.iEvent	=event;
	infoMouse.iPosX		=x;
	infoMouse.iPosY		=y;
}

int MouseCheck(int& iMouseX, int& iMouseY)
{
	iMouseX	=infoMouse.iPosX;
	iMouseY	=infoMouse.iPosY;
	return infoMouse.iEvent;
}

void MouseProcess(void)
{
	infoMouse.iEvent	=_MOUSE_NONE;
}

#ifdef WM_MY_MESSAGE
void PostCmd(DWORD dwCommand, int nData)
{
	if(g_hGameWnd)
		::PostMessage(g_hGameWnd, WM_MY_MESSAGE, dwCommand, nData);
}
#endif

int	_SCR_WIDTH	=800;
int	_SCR_HEIGHT	=600;
