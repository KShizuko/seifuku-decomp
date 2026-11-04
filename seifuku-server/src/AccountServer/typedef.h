#ifndef _TYPEDEF_H
#define _TYPEDEF_H

#include "Define.h"

#define	ENCRYPT
#define	SERVERNAME			"Conquer Online Account Server beta V4.02"
#define	INDEXOPTIMIZE
#define	RC5ENCRYPT

#define	FLAG_ISP_TO_NORMAL	"!ISP计费失败!"
#define	FLAG_NO_POINT		"!NO_POINT!"

#define	ACCOUNTVERSION		(0xC200+19)

const unsigned char RC5PASSWORD_KEY[16] = {	0x3C, 0xDC, 0xFE, 0xE8, 0xC4, 0x54, 0xD6, 0x7E, 
											0x16, 0xA6, 0xF8, 0x1A, 0xE8, 0xD0, 0x38, 0xBE };
const unsigned char RC5BARPASSWORD_KEY[16] = {	0x44, 0xD0, 0xE2, 0xBA, 0x4A, 0x38, 0x14, 0x44, 
												0x64, 0xE0, 0x12, 0xAE, 0xDA, 0x56, 0x1C, 0xF8 };

#define POINT_KEY1				0xb87ea46c
#define POINT_KEY2				0xbad36236
#define ACCOUNT_KEY1			0xb29d3c84
#define ACCOUNT_KEY2			0xcc624ada

#define	ONLINEFILE			"Online.txt"
#define LOGFILE				"AccountServer"
#ifdef	ENCRYPT
	#define	SERVERTITLE			SERVERNAME "(x)"
#else
	#define	SERVERTITLE			SERVERNAME
#endif
#define IPSTRSIZE		16
#define RECVREPEATTIMES		3
#define MAXGAMESERVERS		100
#define	MAXCONNECTS			(FD_SETSIZE - MAXGAMESERVERS - 10)
#define SERVERNAMESIZE		1024
extern	char	SERVERKEY[];
extern	int		LOGINLOOPDELAY;
extern	int		POINTLOOPDELAY;
extern	int		REBUILDLISTENDELAYSEC;
extern	int		LOGINLISTENPORT;
extern	int		POINTLISTENPORT;
extern	int		POINTSOCKETSNDBUF;
extern	int		MAXBANIPS;
extern	int		BANERRORS;
extern	int		BANERRORSECS;
extern	int		BANSECS;
extern	int		ONLINETABLESIZE;
extern	int		HEARTBEATINTERVALSECS;
extern	int		HEARTBEATKICKSECS;
extern	int		ONLINEHEARTBEATSECS;
extern	int		POINTFEEINTERVALSECS;
extern	int		ENABLELOGINDELAY;
extern	int		OUTPUTONLINESECS;
extern	int		REJOINTIMEWAITSECS;
extern	int		COUNTFILESECS;
extern	int		ACCOUNT_AHEAD_DAYS;
extern	int		SERVER_BUSY_DELAY_SECS;
extern	int		SERVER_FULL_DELAY_SECS;
extern	int		KICKOUT_USER_WHEN_NO_POINT;
extern	int		ENABLE_LOGIN_NO_POINT;
extern	int		MONTH_CARD_TO_POINT_CARD;
extern	int		MAX_GM_ID;
extern	int		STAT_SERIAL;
extern	char	SERVER_TITLE[];
extern	int		ENABLE_91U_CREATE_ACCOUNT;

extern	char	DBHOSTNAME[];
extern	char	DATABASENAME[];
extern  char    POINTTABLE[]; 

bool	GetConfig();
bool	GetServerAccount();

#undef	ASSERT
#define	ASSERT(x)	{if(!(x)) LOGERROR("ASSERT(" #x ") in %d line of %s", __LINE__, __FILE__); }
#define LOCKTHREAD		CSingleLock xLock(&m_xCtrl, true); ASSERT(xLock.IsLocked());

#endif