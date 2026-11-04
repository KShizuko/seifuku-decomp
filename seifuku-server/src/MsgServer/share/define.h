#ifndef	ALL_GLOBAL_DEFINE_H
#define	ALL_GLOBAL_DEFINE_H

#pragma warning(disable:4786)

#include "common.h"
#include "myDDstring.h"
#include "Myheap.h"

const int	GAME_VERSION	= 121;
const int	SERVER_VERSION	= 107;
const int	NPCSERVER_VERSION	= 987654321;
const int	NPCSERVER_VERSION_RELOGIN	= 987654322;

#define		NPCSERVER_ACCOUNT 		"NpcServer Tqdig"
#define		NPCSERVER_PASSWORD		"skl8934##VJICW "

extern int	LOGIN_FREE_LEVEL;
#define	FLAG_ISP_TO_NORMAL	"!ISP计费失败!"
#define	FLAG_NO_POINT		"!NO_POINT!"

const int	IPSTR_SIZE			= 16;
const int	_MAX_NAMESIZE		= 16;
const int	MAX_FIELDSIZE		= 256;
const int	MAX_PARAMSIZE		= 128;
const int	MAX_TENETSIZE		= 256;

#define		SYSTEM_NAME 		"SYSTEM"
#define		ALLUSERS_NAME		"ALLUSERS"
#define		REPLAY_OK_STR		"ANSWER_OK"
#define		REPLAY_AGAIN_STR	"ANSWER_AGAIN"

#define	CONFIG_FILENAME		"shell.ini"
const int	DBSTR_SIZE				= 16;
const int	MAX_PACKETSIZE			= 1024;
const int	MAX_MESSAGESIZE			= MAX_PACKETSIZE + 24;
const int	SERVER_LISTEN_PORT		= 5816;
const int	POINTFEE_SECS			= 3*60;
const int	POINTFEE_DELAY_SECS		= 10;
const int	SOCKET_SENDBUFSIZE		= 64*1024;
const int	SOCKET_NPCBUFSIZE		= 128*1024;
const int	SOCKET_ACCOUNTBUFSIZE	= 32*1024;
const int	MAX_MAPGROUPSIZE		= 10;

typedef	unsigned long	OBJID;
typedef	unsigned long	PROCESS_ID;
typedef	long			SOCKET_ID;
#define	IPP_OF(x)		((void**)&(x))
typedef unsigned __int64	I64;

typedef	char			NAMESTR[_MAX_NAMESIZE];
typedef	char			SQLBUF[1024];
typedef	char			MSGBUF[1024];
typedef	char			MESSAGESTR[MAX_MESSAGESIZE];
typedef	char			PARAMBUF[MAX_PARAMSIZE];
typedef	char			TENETSTR[MAX_TENETSIZE];

const int	GAMESERVER_INTERPORT	= 0;

enum{
		MSGPORT_SHELL			= 0,
		MSGPORT_SOCKET			= 1,
		MSGPORT_WORLD			= 2,
		MSGPORT_MAPGROUP_FIRST	= 3,
};

const int	ID_NONE			= 0;
const int	SOCKET_NONE		= -1;
const int	PROCESS_NONE	= -1;
const int	INVALID_ROOMID	= -1;
const int	BCAST_NPCID		= 0;
const int	INDEX_NONE		= -1;

const bool	UPDATE_TRUE				= true;
const bool	UPDATE_FALSE			= false;
const bool	DELRECORD_TRUE			= true;
const bool	DELRECORD_FALSE			= false;
const bool  INCLUDE_SELF			= true;
const bool  EXCLUDE_SELF			= false;
enum {SYNCHRO_FALSE=false, SYNCHRO_TRUE=true, SYNCHRO_BROADCAST};

#define	MSGHEAD_DEFINE

const OBJID	SCENEID_FIRST		= 000001;
const OBJID	SYSNPCID_FIRST		= 00001;
const OBJID	SYSNPCID_LAST		= 99999;
const OBJID	DYNANPCID_FIRST		= 100001;
const OBJID	DYNANPCID_LAST		= 199999;
const OBJID	SCENEID_LAST		= 299999;

const OBJID	NPCSERVERID_FIRST	= 400001;
const OBJID	MONSTERID_FIRST		= 400001;
const OBJID	MONSTERID_LAST		= 499999;
const OBJID	PETID_FIRST			= 500001;
const OBJID	PETID_LAST			= 599999;
const OBJID	NPCSERVERID_LAST	= 699999;

const OBJID	CALLPETID_FIRST		= 700001;
const OBJID	CALLPETID_LAST		= 799999;

const OBJID TRAPID_FIRST		= 900001;
const OBJID MAGICTRAPID_FIRST	= 900001;
const OBJID MAGICTRAPID_LAST	= 989999;
const OBJID SYSTRAPID_FIRST		= 990001;
const OBJID SYSTRAPID_LAST		= 999999;
const OBJID TRAPID_LAST			= 999999;

const OBJID PLAYER_ID_FIRST		= 1000000;
const OBJID PLAYER_ID_LAST		= 1999999999;

const OBJID EUDEMON_ID_FIRST	= 2000000000;
const OBJID EUDEMON_ID_LAST		= 3999999999;

const OBJID CLIENT_ID_FIRST		= 4000000000;

const int	DEFAULT_LOGIN_MAPGROUP		= 0;
const int	DEFAULT_LOGIN_MAPID			= 1010;
const int	DEFAULT_LOGIN_POSX			= 362;
const int	DEFAULT_LOGIN_POSY			= 594;

const int	CELLS_PER_BLOCK				= 18;
const int	CELLS_PER_VIEW				= 18;
const int	ITEMTYPEMONEY_MIN			= 1090000;
const int	ITEMTYPEMONEY_MAX			= 1091020;
const int	SMALLHEAPMONEY_LIMIT		= 10;
const int	SMALLHEAPMONEY_TYPE			= 1090000;
const int	MIDDLEHEAPMONEY_LIMIT		= 100;
const int	MIDDLEHEAPMONEY_TYPE		= 1090010;
const int	BIGHEAPMONEY_LIMIT			= 1000;
const int	BIGHEAPMONEY_TYPE			= 1090020;

const int	SMALLHEAPGOLD_LIMIT			= 2000;
const int	SMALLHEAPGOLD_TYPE			= 1091000;
const int	MIDDLEHEAPGOLD_LIMIT		= 5000;
const int	MIDDLEHEAPGOLD_TYPE			= 1091010;
const int	BIGHEAPGOLD_LIMIT			= 10000;
const int	BIGHEAPGOLD_TYPE			= 1091020;

enum {
	OBJ_NONE	= 0x1000,
	OBJ_USER	= 0x1001,
	OBJ_MONSTER	= 0x1002,
	OBJ_ITEM	= 0x1004,
	OBJ_MAP		= 0x1008,
	OBJ_FRIEND	= 0x1010,
	OBJ_NPC		= 0x1040,
	OBJ_MAPITEM	= 0x1080,
	OBJ_SYN		= 0x1100,
	OBJ_BOOTH	= 0x1200,
	OBJ_TRAP	= 0x1400,
	OBJ_TUTOR	= 0x1800,
};
inline bool IsObjType(OBJID idObjType, OBJID idUnion) { return (idObjType & idUnion & 0x0FFF) != 0; }

class CUser;
typedef	CUser*	CUserPtr;
class CMonster;
typedef	CMonster*	CMonsterPtr;
class CItem;
typedef	CItem*	CItemPtr;
class CGameMap;
typedef	CGameMap*	CMapPtr;
class CFriend;
typedef	CFriend*	CFriendPtr;
class CNpc;
typedef	CNpc*	CNpcPtr;
class CMapItem;
typedef	CMapItem*	CMapItemPtr;
class CSyndicate;
typedef	CSyndicate*	CSynPtr;

#ifdef	WORLD_KERNEL
#else
class CNetMsg;
class ISocket
{
public:
	virtual bool SendMsg			(CNetMsg* pNetMsg)				PURE_VIRTUAL_FUNCTION_0
	virtual bool SendMsg			(SOCKET_ID idSocket, OBJID idMsg, const char* pBuf, int nMsgLen)	PURE_VIRTUAL_FUNCTION_0
	virtual bool SendNpcMsg			(OBJID idNpc, OBJID idMsg, const char* pBuf, int nMsgLen)			PURE_VIRTUAL_FUNCTION_0
	virtual bool CloseSocket		(SOCKET_ID idSocket)			PURE_VIRTUAL_FUNCTION_0
};

typedef	void			CGameSocket;
#endif

#ifdef	PALED_DEBUG_X
#define	DEBUG_CREATEMSG(x,y,a,s,n1,n2)
#define	DEBUG_SENDMSG(x,y)				{MSGBUF D_CM;sprintf(D_CM,"↓ msgtype:%d,socketid:%d",x,y);CMsgTalk msg;if(msg.Create("DEBUG","DEBUG",D_CM))SendMsg(&msg);}
#define	DEBUG_PROCESSMSG(x,y,a,s,n1,n2)	{MSGBUF D_CM;sprintf(D_CM,"↑ msgtype:%d,socketid:%d,msg:%s,id:%d,action:%d,%s,%d,%d",GetType(),GetSocketID(),x,y,a,s,n1,n2);CMsgTalk msg;if(msg.Create("DEBUG","DEBUG",D_CM))SendMsg(&msg);}
#else
#define	DEBUG_CREATEMSG(x,y,a,s,n1,n2)
#define	DEBUG_SENDMSG(x,y)				{}
#define	DEBUG_PROCESSMSG(x,y,a,s,n1,n2)	{}
#endif

inline void EmptyFunction(...) {}
#ifdef MULTI_KERNEL_LOG
	#define LOGDEBUG	LOGMSG
#else
	#define LOGDEBUG	EmptyFunction
#endif

#endif