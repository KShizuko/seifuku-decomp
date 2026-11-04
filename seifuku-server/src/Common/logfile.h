#ifndef	LOGFILE_H
#define LOGFILE_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "time.h"

#ifdef	_DEBUG
	#define PROFILE
#endif

void InitLog(const char * pTitle, const char * pFilename, time_t tStart);
void log_Save(const char * pFilename, const char * pBuf);

#define log_InlineLog(x,y)\
inline void	log_InlineLog##y(const char * sFormat, ...)\
{\
	va_list argptr;\
	va_start( argptr, sFormat);     \
\
	time_t ltime;\
	time( &ltime );\
	tm *	pTm = localtime(&ltime);\
	char	buf[4096] = "";					\
	sprintf(buf, "%02d:%02d:%02d " #y ": ", pTm->tm_hour, pTm->tm_min, pTm->tm_sec);\
	int		ret = vsprintf(buf + strlen(buf), sFormat, argptr);\
	strcpy(buf + strlen(buf), "\n");\
	if(!(ret < 4096-2)) log_Save(x, "ASSERT(ret < 4096-2)");	\
\
	va_end( argptr );              \
\
	log_Save(x,buf);\
}

extern char		log_szFilename[];

log_InlineLog(log_szFilename, WARNING)
log_InlineLog(log_szFilename, Message)
log_InlineLog(log_szFilename, ERROR)
log_InlineLog(log_szFilename, CATCH_ERROR)
log_InlineLog("gmlog\\event", event)
log_InlineLog("gmlog\\card", card)
log_InlineLog("syslog\\dump", DUMP)
log_InlineLog("syslog\\cheat", CHEAT)
inline void log_InlineLogNULL(const char * sFormat, ...){}

#define LOGWARNING		log_InlineLogWARNING
#define LOGMSG			log_InlineLogMessage
#define LOGERROR		log_InlineLogERROR
#define LOGCATCH		log_InlineLogCATCH_ERROR
#define LOGEVENT		log_InlineLogevent
#define LOGCARD			log_InlineLogcard
#define LOGCHEAT		log_InlineLogCHEAT
#ifdef	DUMP_DEBUG_MSG
#define LOGDUMP			log_InlineLogDUMP
#else
#define LOGDUMP			log_InlineLogNULL
#endif

#ifndef	PROFILE
	#undef	log_InlineLog
	#define log_InlineLog(x,y) inline void	log_InlineLog##y(const char * sFormat, ...) { }
#endif
log_InlineLog(log_szFilename, Profile)
#define LOGPROFILE			log_InlineLogProfile

#include <windows.h>

#endif
