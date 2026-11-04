#ifndef	TYPEDEF_H
#define	TYPEDEF_H

#include "define.h"

#define	BETA

#define	GAME_TITLE			"Conquer Online Server"

const int	THREADDELAY_MS			= 400;
const int	MAPGROUP_ONTIMER_MS		= 500;
const int	SOCKETKERNEL_ONTIMER_MS	= 1000;
const int	WORLDKERNEL_ONTIMER_MS	= 1000;
const int	TIMER_OFFSET_LIMIT		= 10;
const int	SOCKET_PROCESS_INTERVAL	= 100;
const int	CLOSETHREAD_MILLISECS	= 30000;

const int	TEXTWINDOW_SIZE		= 8;

#define		LOGFILE_DIR				"syslog"
#define		LOGFILE_FILENAME		"syslog\\CQ_SERVER"
#define		ONLINE_FILENAME			"Game server online.txt"

struct ST_CONFIG{
	int		MAPGROUP_SIZE;
	int		CURRENT_PORTID;
	int		PORT_SIZE;
	char	MASTER_IP[IPSTR_SIZE];
	int		MASTER_PORT;
	char	LOGIN_KEY[256];
};

extern ST_CONFIG	CONFIG;

#endif