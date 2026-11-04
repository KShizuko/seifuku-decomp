#pragma	warning(disable:4786)

#ifndef	ALL_GLOBAL_DEFINE_H
#define	ALL_GLOBAL_DEFINE_H

#include "common.h"
#include "mycom.h"

#ifdef ENG_RC
#include "eng_rc.h"
#else
#include "chn_rc.h"
#endif

const int	GAME_VERSION	= 100;
const int	SERVER_VERSION	= 100;
const int	NPCSERVER_VERSION	= 987654321;
const int	NPCSERVER_VERSION_RELOGIN	= 987654322;

#define		NPCSERVER_ACCOUNT 		"NpcServer Tqdig"
#define		NPCSERVER_PASSWORD		"skl8934##VJICW "

const int	IPSTR_SIZE			= 16;
const int	_MAX_NAMESIZE		= 16;
const int	MAX_FIELDSIZE		= 1024;

#define		SYSTEM_NAME 		"SYSTEM"
#define		ALLUSERS_NAME		"ALLUSERS"
#define		REPLAY_OK_STR		"ANSWER_OK"
#define		REPLAY_AGAIN_STR	"ANSWER_AGAIN"

#define	CONFIG_FILENAME		"shell.ini"
const int	DBSTR_SIZE				= 16;
const int	_MAX_MSGSIZE			= 1024;
const int	MAX_PACKETSIZE			= 1024;
const int	MAX_MESSAGESIZE			= MAX_PACKETSIZE + 24;
const int	SERVER_LISTEN_PORT		= 5816;

const int	RESEND_MSGLOGIN_SECS		= 5;

typedef	unsigned long	OBJID;
typedef	unsigned long	PROCESS_ID;
typedef	long			SOCKET_ID;
typedef unsigned __int64	I64;

typedef	char			IPSTR[IPSTR_SIZE];
typedef	char			NAMESTR[_MAX_NAMESIZE];
typedef	char			SQLBUF[1024];
typedef	char			MSGBUF[1024];
typedef	char			NETMSGBUF[1024];
typedef	char			MESSAGESTR[MAX_MESSAGESIZE];
typedef	char			FIELDBUF[MAX_FIELDSIZE];

const int	GAMESERVER_INTERPORT	= 0;

enum{
		MSGPORT_SHELL			= 0,
		MSGPORT_SOCKET			= 1,
		MSGPORT_WORLD			= 2,
		MSGPORT_MAPGROUP_FIRST	= 3,
};

const int	ID_NONE			= 0;
const int	INVALID_ID		= -1;
const int	INVALID_ROOMID	= -1;
const int	BCAST_NPCID		= 0;

const bool	UPDATE_TRUE				= true;
const bool	UPDATE_FALSE			= false;
const bool	DELRECORD_TRUE			= true;
const bool	DELRECORD_FALSE			= false;
const bool	SYNCHRO_TRUE			= true;
const bool	SYNCHRO_FALSE			= false;

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
const OBJID PLAYER_ID_LAST		= 3999999999;

const OBJID CLIENT_ID_FIRST		= 4000000000;

#define	MSGHEAD_DEFINE

const int	DEFAULT_LOGIN_MAPGROUP		= 0;
const int	DEFAULT_LOGIN_MAPID			= 1000;
const int	DEFAULT_LOGIN_POSX			= 44;
const int	DEFAULT_LOGIN_POSY			= 44;
const int	DEFAULT_LOGIN_DIR			= 0;
const int	DEFAULT_LOGIN_STATUS		= 0;
const int	DEFAULT_LOGIN_POSE			= 0;
const int	DEFAULT_LOGIN_EMOTION		= 0;

const int	CELLS_PER_BLOCK				= 18;
const int	CELLS_PER_VIEW				= 18;
const int	BLOCKWIDTH_PER_MAP			= 100;

const int	DEFAULT_MAP_WIDTH			= 50;
const int	DEFAULT_MAP_HEIGHT			= 50;

const int	MAPS_PER_MAPGROUP			= 1;

enum {
	OBJ_NONE	= 1234,
	OBJ_USER,
	OBJ_ITEM,
	OBJ_MAP,
	OBJ_FRIEND,
	OBJ_NPC,
	OBJ_NPCGEN,
	OBJ_NPCTYPE,
	OBJ_AGENT,
};

class CUser;
class CNpc;
class CAgent;
GUID_DECLARE(CUser,OBJ_USER)
GUID_DECLARE(CNpc,OBJ_NPC)
GUID_DECLARE(CAgent,OBJ_AGENT)

#define NOMATE_NAME		"无"

#define	SYNNAME_NONE	"无"
#define	SYNLEADER_TITLE	"帮主"
#define	SYNMEMBER_TITLE	"普通帮众"
#define	NOSYN_TITLE		"无"

const int	STATESTR_SIZE		= 2048;

class CUser;
typedef	CUser*	CUserPtr;
class CItem;
typedef	CItem*	CItemPtr;
class CGameMap;
typedef	CGameMap*	CMapPtr;
class CFriend;
typedef	CFriend*	CFriendPtr;
#define	CNpcPtr			INpc*
class CNpcGenerator;
typedef	CNpcGenerator*	CGenPtr;
class CNpcType;
typedef	CNpcType*	CNpcTypePtr;

typedef	void			CGameSocket;

const int _DELTA_X[9]={ 0,-1,-1,-1, 0, 1, 1, 1, 0 };
const int _DELTA_Y[9]={ 1, 1, 0,-1,-1,-1, 0, 1, 0 };
const int MAX_DIRSIZE = 8;
inline int GetDirByPos(int nFromX, int nFromY, int nToX, int nToY)
{
	if(nFromX < nToX)
	{
		if(nFromY < nToY)
			return 7;
		else if(nFromY > nToY)
			return 5;
		else
			return 6;
	}
	else if(nFromX > nToX)
	{
		if(nFromY < nToY)
			return 1;
		else if(nFromY > nToY)
			return 3;
		else
			return 2;
	}
	else
	{
		if(nFromY < nToY)
			return 0;
		else if(nFromY > nToY)
			return 4;
	}
	return MAX_DIRSIZE;
}

#endif