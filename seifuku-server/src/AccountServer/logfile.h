#ifndef	LOGFILE_H
#define LOGFILE_H

#include <afxmt.h>
#pragma warning(disable:4786)

#ifdef	_DEBUG
	#define PROFILE
#endif

void InitLog(const char * pTitle, const char * pFilename, time_t tStart);
void log_SaveFile(const char * pFilename, const char * pBuf, char* szCountHeadLine = NULL);

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
	if(!(ret < 4096-2)) log_SaveFile(x, "ASSERT(ret < 4096-2)");	\
\
	va_end( argptr );              \
\
	log_SaveFile(x,buf);\
}

inline void	LOGISPPOINT(const char * x, const char * sFormat, ...)
{
	va_list argptr;
	va_start( argptr, sFormat);

	time_t ltime;
	time( &ltime );
	tm *	pTm = localtime(&ltime);
	char	buf[4096] = "";
	sprintf(buf, "# %02d:%02d:%02d "  ": ", pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
	int		ret = vsprintf(buf + strlen(buf), sFormat, argptr);
	strcpy(buf + strlen(buf), "\n");
	if(!(ret < 4096-2)) log_SaveFile(x, "ASSERT(ret < 4096-2)");

	va_end( argptr );

	log_SaveFile(x,buf);
}

extern char		log_szFilename[];

log_InlineLog(log_szFilename, WARNING)
log_InlineLog(log_szFilename, Message)
log_InlineLog(log_szFilename, ERROR)
log_InlineLog("server", server)
log_InlineLog("account", account)
log_InlineLog(log_szFilename, Other)
log_InlineLog(log_szFilename, CATCH_ERROR)
log_InlineLog("point", Point)

#define LOGWARNING		log_InlineLogWARNING
#define LOGMSG			log_InlineLogMessage
#define LOGERROR		log_InlineLogERROR
#define LOGSERVER		log_InlineLogserver
#define LOGACCOUNT		log_InlineLogaccount
#define LogSave			log_InlineLogOther
#define LOGCATCH		log_InlineLogCATCH_ERROR
#define LOGPOINT		log_InlineLogPoint

void	DateTime(char * buf20, time_t tInput);

#endif
