#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "common.h"

#include <stdarg.h>
#include "MyLock.h"
#include "BaseFunc.h"

#define	MUTEX_SYSLOG
#ifdef	MUTEX_SYSLOG
CMyCriticalSection	log_xSysLogCrit;
#endif

static void LogSaveV(const char* fmt, va_list argptr)
{
#ifdef	MUTEX_SYSLOG
	CMySingleLock(&log_xSysLogCrit, true);
#endif

	const DWORD _MAX_LOGLINE	=1000000;
	static DWORD dwLineCount	=0;

    char buffer[4096];
    vsprintf( buffer, fmt, argptr );

	time_t ltime;
	time( &ltime );

	struct tm *pTime;
	pTime = localtime( &ltime );

	char szLogName[256];
	sprintf(szLogName, "syslog/sys %d-%d-%d %u.log", pTime->tm_year+1900, pTime->tm_mon+1, pTime->tm_mday, ++dwLineCount/_MAX_LOGLINE);

	FILE* fp	=fopen(szLogName, "a+");
	if(!fp)
		return;

	fprintf(fp, "%04d%02d%02d:%02d%02d%02d %s\n", pTime->tm_year+1900, pTime->tm_mon+1, pTime->tm_mday,
				pTime->tm_hour, pTime->tm_min, pTime->tm_sec, buffer);
	fclose(fp);	
}

bool	SafeCopy(char * pTarget, const char * pSource, int nBufLen )
{
		if(pTarget)
		{
			pTarget[0] = 0;
			if(pSource)
			{
				if(nBufLen && (int)strlen(pSource) >= nBufLen)
				{
					strncpy(pTarget, pSource, nBufLen-1);
					pTarget[nBufLen-1] = 0;
					return false;
				}
				strcpy(pTarget, pSource);
				return true;
			}
		}

	assert(!"SafeCopy()");
	return false;
}

void LogSave (const char* fmt, ...)
{	
	va_list argptr;
	va_start( argptr, fmt );
	LogSaveV(fmt, argptr);
	va_end( argptr );
}

void MyLogSave (const char* pszName, const char* fmt, ...)
{	
#ifdef	MUTEX_SYSLOG
	CMySingleLock(&log_xSysLogCrit, true);
#endif

	if (!pszName || !fmt)
		return;

    char buffer[1024];
    vsprintf( buffer, fmt, (char*) ((&fmt)+1) );

	time_t ltime;
	time( &ltime );

	struct tm *pTime;
	pTime = localtime( &ltime );

	char szLogName[256] = "";
	sprintf(szLogName, "%s %d-%d-%d.log", pszName, pTime->tm_year+1900, pTime->tm_mon+1, pTime->tm_mday);

	FILE* fp	=fopen(szLogName, "a+");
	if(!fp)
		return;

	fprintf(fp, "%s -- %s", buffer, ctime(&ltime));
	fclose(fp);	
}

void MsgLogSave	(const char* pszFileName, const char* fmt, ...)
{
#ifdef	MUTEX_SYSLOG
	CMySingleLock(&log_xSysLogCrit, true);
#endif

    char buffer[1024];
    vsprintf( buffer, fmt, (char*) ((&fmt)+1) );

	time_t ltime;
	time( &ltime );

	struct tm *pTime;
	pTime = localtime( &ltime );

	char szLogName[256] = "";
	sprintf(szLogName, "%s %d-%d-%d.log", pszFileName, pTime->tm_year+1900, pTime->tm_mon+1, pTime->tm_mday);

	FILE* fp	=fopen(szLogName, "a+");
	if(!fp)
		return;

	fprintf(fp, "%s\n", buffer);
	fclose(fp);	
}

void DebugSave (const char* fmt, ...)
{	
	if(!fmt)
		return;

    char szMsg[1024];
    vsprintf( szMsg, fmt, (char*) ((&fmt)+1) );

	::OutputDebugString(szMsg);
	::OutputDebugString("\n");
}

void GmLogSave	(const char* fmt, ...)
{
#ifdef	MUTEX_SYSLOG
	CMySingleLock(&log_xSysLogCrit, true);
#endif

    char buffer[1024];
    vsprintf( buffer, fmt, (char*) ((&fmt)+1) );

	time_t ltime;
	time( &ltime );

	struct tm *pTime;
	pTime = localtime( &ltime );

	char szLogName[256] = "";
	sprintf(szLogName, "gmlog/gm %d-%d-%d.log", pTime->tm_year+1900, pTime->tm_mon+1, pTime->tm_mday);

	FILE* fp	=fopen(szLogName, "a+");
	if(!fp)
		return;

	fprintf(fp, "%s -- %s", buffer, ctime(&ltime));
	fclose(fp);	
}
