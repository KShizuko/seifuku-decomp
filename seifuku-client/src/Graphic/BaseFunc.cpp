#include "BaseFunc.h"
#include <direct.h>

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

void ErrorMsg(const char* fmt, ...)
{
    char szMsg[1024];
    vsprintf( szMsg, fmt, (char*) ((&fmt)+1) );

	LogMsg(szMsg);
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

DWORD TimeGet(void)
{
	return timeGetTime();
}

int	RandGet(int nMax, BOOL bRealRand)
{
	if(nMax == 0)
		return 0;
	if (bRealRand)
		::srand( (unsigned)TimeGet() );

	return ::rand()%nMax;
}

BOOL IniDataGet(const char *pszFileName, const char *pszTitle, const char *pszSubTitle, int &iData, BOOL bCritical )
{
	if (!pszFileName || !pszTitle || !pszSubTitle)
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

			if (0 != stricmp(szSection, szLine))
			continue;

		char szFormat[256] = "";
		sprintf(szFormat, "%s=%s\n", pszSubTitle, "%d");

		while (true)
		{
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

BOOL	ReadLine(const char* pBuf, DWORD dwSize, DWORD* pdwPos, char* szLine, DWORD dwMax)
{
	if (!pBuf || !szLine)
		return false;
	if (*pdwPos >= dwSize)
		return false;
	DWORD i = 0;
	for (; i < dwMax; i++)
	{
		szLine[i] = pBuf[(*pdwPos)++];
		if (szLine[i] == '\n' || *pdwPos >= dwSize)
		{
			szLine[i] = 0;
			if (i > 0 && szLine[i-1] == '\r')
				szLine[i-1] = 0;
			break;
		}
	}
	return i < dwMax;
}

BOOL IniStrGet(const char *pszFileName, const char *pszTitle, const char *pszSubTitle, char *pszStr, BOOL bCritical )
{
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

		if (0 != stricmp(szSection, szLine))
			continue;

		char szFormat[256] = "";
		sprintf(szFormat, "%s=%s\n", pszSubTitle, "%s");

		while (true)
		{
			char szLine[1024]	="";
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