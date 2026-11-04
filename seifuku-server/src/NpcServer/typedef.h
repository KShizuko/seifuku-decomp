#ifndef	TYPEDEF_HEADFILE_H
#define	TYPEDEF_HEADFILE_H

#include <limits.h>

#define	NPCSERVER_TITLE				"Conquer Online NPC Server"

#define	LOGFILE_DIRECTION			"syslog"
#define	LOGFILE_FILENAME			"syslog\\NpcServer"

const int	MAX_TEXTLINESIZE		= 500;
const DWORD	END_OF_TEXT				= INT_MAX;
const int	SAVESTATEWIN_SECS		= 5*60;
const int	UPDATESTATEWIN_SECS		= 10;

const int	CONNECT_DELAY_MS		= 900;
const int	SYSTEM_ONTIMER_MS		= 500;
const int	TIMER_OFFSET_LIMIT		= 10;

typedef		unsigned long			OBJID;
const OBJID	ID_NONE					= 0;

const int	IPSTR_SIZE				= 16;
typedef		char					IPSTR[IPSTR_SIZE];

const int	MAX_PACKETSIZE			= 1024;

#endif