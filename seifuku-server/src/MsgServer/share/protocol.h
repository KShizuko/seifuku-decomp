#ifndef	PROTOCOL_H
#define	PROTOCOL_H

#include <string.h>
#include "define.h"
#ifdef MAPGROUP_KERNEL
#include "UserData.h"
#endif

enum{
		KERNEL_BASE					= 1000,
		KERNEL_CLIENTMSG			= KERNEL_BASE + 1,
		KERNEL_NPCMSG				= KERNEL_BASE + 2,
		KERNEL_CLOSEKERNEL			= KERNEL_BASE + 3,
		KERNEL_CREATESYNDICATE		= KERNEL_BASE + 10,
		KERNEL_DESTROYSYNDICATE		= KERNEL_BASE + 11,
		KERNEL_CHANGESYNDICATE		= KERNEL_BASE + 12,
		KERNEL_COMBINESYNDICATE		= KERNEL_BASE + 13,
		KERNEL_SUPERMANLIST			= KERNEL_BASE + 14,
		KERNEL_QUERYSUPERMANLIST	= KERNEL_BASE + 15,

		MAPGROUP_BASE				= 10000,
		MAPGROUP_LOGIN				= MAPGROUP_BASE + 1,
		MAPGROUP_BROADCASTMSG		= MAPGROUP_BASE + 2,
		MAPGROUP_SOCKETUSERINFO		= MAPGROUP_BASE + 3,
		MAPGROUP_SENDOBJINFO		= MAPGROUP_BASE + 4,
		MAPGROUP_TRANSMITMSG		= MAPGROUP_BASE + 5,
		MAPGROUP_CHANGEMAPGROUP		= MAPGROUP_BASE + 10,
		MAPGROUP_CREATENEWNPC		= MAPGROUP_BASE + 11,
		MAPGROUP_CHANGETEAM			= MAPGROUP_BASE + 12,
		MAPGROUP_REMOTECALL			= MAPGROUP_BASE + 13,
		MAPGROUP_CHANGENPC			= MAPGROUP_BASE + 14,
		MAPGROUP_DELTRANSNPC		= MAPGROUP_BASE + 15,
		MAPGROUP_DELALLMONSTER		= MAPGROUP_BASE + 16,
		MAPGROUP_SETMAPSYNID		= MAPGROUP_BASE + 17,
		MAPGROUP_LOADALLPET			= MAPGROUP_BASE + 18,
		MAPGROUP_RPC				= MAPGROUP_BASE + 19,
		MAPGROUP_SETTRANSNPC		= MAPGROUP_BASE + 20,
		MAPGROUP_SOCKETIP			= MAPGROUP_BASE + 21,	MAPGROUP_FEEINFO = MAPGROUP_BASE + 30,

		WORLD_BASE					= 20000,
		WORLD_SHELLTALK				= WORLD_BASE + 1,
		WORLD_SETPROCESSID			= WORLD_BASE + 2,
		WORLD_CLOSESOCKET			= WORLD_BASE + 3,
		WORLD_USERNAMEMSG			= WORLD_BASE + 4,
		WORLD_USERIDMSG				= WORLD_BASE + 5,
		WORLD_LEVELUP				= WORLD_BASE + 6,
		WORLD_QUERYFEE				= WORLD_BASE + 7,
		WORLD_SENDTIME				= WORLD_BASE + 8,
		WORLD_TRANSMITMSG			= WORLD_BASE + 9,
		WORLD_NEWSOCKET				= WORLD_BASE + 10,

		SOCKET_BASE					= 30000,
		SOCKET_CLOSESOCKET			= SOCKET_BASE + 1,
		SOCKET_SETPROCESSID			= SOCKET_BASE + 2,
		SOCKET_SENDCLIENTMSG		= SOCKET_BASE + 3,
		SOCKET_SENDNPCMSG			= SOCKET_BASE + 4,
		SOCKET_SETNPCSOCKETID		= SOCKET_BASE + 5,
		SOCKET_SETNPCPROCESSID		= SOCKET_BASE + 6,
		SOCKET_CHANGECODE			= SOCKET_BASE + 7,
		SOCKET_BREAKCONNECT			= SOCKET_BASE + 8,
		SOCKET_BREAKALLCONNECT		= SOCKET_BASE + 11,
		SOCKET_QUERYIP				= SOCKET_BASE + 12,

		SHELL_BASE					= 40000,
		SHELL_PRINTTEXT				= SHELL_BASE + 1,
		SHELL_KERNELSTATE			= SHELL_BASE + 2,
		
		DB_BASE						= 50000,
		DB_QUERY_USERINFO			= DB_BASE + 1,

		DB_MSG_BASEINFO				= DB_BASE + 101,
		DB_MSG_FRIENDS				= DB_BASE + 102,
		DB_MSG_ENEMYS				= DB_BASE + 103,
		DB_MSG_ITEMS				= DB_BASE + 104,
		DB_MSG_WEAPON_SKILL			= DB_BASE + 105,
		DB_MSG_MAGICS				= DB_BASE + 106,
		DB_MSG_SYNATTR				= DB_BASE + 107,
};

struct CLIENTMSG_PACKET0
{
	SOCKET_ID		idSocket;
	OBJID	idPacket;
	int		nSize;
	char	buf[1];
};

struct NPCMSG_PACKET0
{
	OBJID	idNpc;
	OBJID	idPacket;
	int		nSize;
	char	buf[1];
};

struct SENDCLIENTMSG_PACKET0
{
	SOCKET_ID		idSocket;
	OBJID	idPacket;
	int		nSize;
	char	buf[1];
};

struct SENDNPCMSG_PACKET0
{
	OBJID	idNpc;
	OBJID	idPacket;
	int		nSize;
	char	buf[1];
};

struct TRANSMSG_PACKET0
{
	SOCKET_ID		idSocket;
	OBJID			idNpc;
	OBJID	idPacket;
	int		nSize;
	int		nTrans;
	char	buf[1];
};

struct USERNAMEMSG_PACKET0
{
	SOCKET_ID		idSocket;
	OBJID	idPacket;
	int		nSize;
	char	szName[_MAX_NAMESIZE];
	char	buf[1];
};

struct USERIDMSG_PACKET0
{
	SOCKET_ID		idSocket;
	OBJID	idPacket;
	int		nSize;
	OBJID	idUser;
	char	buf[1];
};

struct CHANGE_USERDATA
{
	SOCKET_ID	idSocket;
	int			nData;
};

struct CHANGE_NPCPROCESSID
{
	SOCKET_ID	idNpc;
	PROCESS_ID	idProcess;
};

struct ST_LOGIN
{
	SOCKET_ID	idSocket;
	OBJID		idUser;
	int			nMinorFlag;
	int			nClientVersion;
};

struct ST_QUERYIP
{
	PROCESS_ID	idProcess;
	SOCKET_ID	idSocket;
};

struct ST_SOCKETIP
{
	SOCKET_ID	idSocket;
	DWORD		dwIP;
};	struct ST_FEEINFO { SOCKET_ID idSocket; OBJID idUser; int nType; DWORD dwData; int nTime; };

#ifdef MAPGROUP_KERNEL
struct ST_USERCHANGEMAPGORUP
{
	SOCKET_ID	idSocket;
	UserInfoStruct		info;
};
#endif

struct CHANGEMAPGORUP_INFO0
{
	OBJID		idUser;
	int			nInfoType;
	char		info[1];
};

struct CHANGE_MAPGROUP
{
	OBJID		idUser;
	PROCESS_ID	idProcess;
};

struct ST_CHANGEMAPGROUP
{
	OBJID		idUser;
	OBJID		idMap;
	int			nPosX;
	int			nPosY;
};

struct ST_CREATENEWNPC
{
	OBJID			id;
	OBJID			idMap;
	OBJID			idData;
	OBJID			idOwner;
	int				nData;
	unsigned short	usPosX;
	unsigned short	usPosY;
	unsigned short	usAction;
	unsigned short	usType;
	unsigned char	ucOwnerType;
};

struct CreateSyndicateInfo
{
	OBJID		idSyn;
	NAMESTR		szName;
	TENETSTR	szTenet;
	NAMESTR		szLeaderTitle;
	NAMESTR		szMemberTitle;
	OBJID		idLeader;
	NAMESTR		szLeader;
	int			nMoney;
	OBJID		idFealty;
};

struct	SynFuncInfo0
{
	OBJID			idSyn;
	unsigned short	nSize;
	unsigned char	ucFuncType;
	unsigned char	ucSaveFlag;
	union{
		long		IntParam[1];
		char		StrParam[1];
	};
};

struct ST_CHANGETEAM
{
	int				nAction;
	OBJID			idTeam;
	OBJID			idUser;
	int				nData;
};

struct	REMOTE_CALL0
{
	OBJID			idUser;
	unsigned short	nSize;
	unsigned char	ucFuncType;
	union{
		long		IntParam[1];
		char		StrParam[1];
	};
};

struct	VARIANT_SET0
{
	long		m_setLong[1];
	long&		IntParam(int idx)			{ return m_setLong[idx]; }
	char*		StrParam(int idx)			{ return (char*)&m_setLong[idx]; }
};

struct	ST_QUERY_USERINFO
{
	OBJID	idUser;
};

const int		_MEDAL_BUFSIZE		= 64+1;
const int		_TITLE_BUFSIZE		= 64+1;

struct	ST_DB_USERINFO
{
	OBJID	idUser;

	DWORD	dwCoin_money;
	DWORD	dwLast_login;
	DWORD	dwLookface;
	DWORD	dwHair;
	char	setMedal[_MEDAL_BUFSIZE];
	UCHAR	ucLevel;
	UCHAR	ucMetempsychosis;
	UCHAR	ucNobility;
	DWORD	dwMedal_select;

	char	setTitle[_TITLE_BUFSIZE];
	UCHAR	ucProfession;
	UCHAR	ucAdditional_point;
	UCHAR	ucAuto_allot;
	DWORD	dwTitle_select;

	DWORD	dwMoney;
	DWORD	dwMoney_saved;
	USHORT	usRecordX;
	USHORT	usRecordY;
	OBJID	idAccount;
	OBJID	idRecordmap;
	OBJID	idSyndicate;
	int		nDeed;
	int		nExp;

	DWORD	dwTaskMask;
	DWORD	dwVirtue;
	OBJID	idHome;
	DWORD	dwLockKey;
	char	szMate[_MAX_NAMESIZE];
	char	szName[_MAX_NAMESIZE];
	USHORT	usForce;
	USHORT	usConstitution;
	USHORT	usHealth;
	USHORT	usLife;
	USHORT	usMana;
	USHORT	usSoul;
	USHORT	usSpeed;
	SHORT	sPk;
};

struct	ST_DB_FRIEND_INFO
{
		OBJID	id;
		OBJID	idUser;
		OBJID	idFriend;
		char	szFriendName[_MAX_NAMESIZE];
};

struct	ST_DB_FRIEND
{
	OBJID	idUser;

	int		nAmount;
	ST_DB_FRIEND_INFO	setFriends[1];
};

struct	ST_DB_ENEMY
{
	OBJID	idUser;
};

struct	ST_DB_ITEMINFO
{
	OBJID	id;

};
struct	ST_DB_ITEM
{
	OBJID	idUser;

	int		nAmount;
	ST_DB_ITEMINFO	setItems[1];
};

struct	ST_DB_WEAPON_SKILL
{
	OBJID	idUser;
};

struct	ST_DB_MAGIC
{
	OBJID	idUser;
};

struct	ST_DB_SYNATTR
{
	OBJID	idUser;
};

const int	PROTOCOL_MSG_HEADSIZE		= 4;
struct CProtocolMsgStruct{
	unsigned short	nMsgSize;
	unsigned short	idPacket;
	char			pMsg[1];
};

const int	PROTOCOL_MSG_SEALSIZE		= 8;
inline int	SplitPacket(const char* pBuf, int nBufLen, OBJID* pidPacket, char** ppMsg, int* pMsgSize, bool bSealed)
{
	if(nBufLen > PROTOCOL_MSG_HEADSIZE)
	{
		CProtocolMsgStruct*	pMsgPtr = (CProtocolMsgStruct*)pBuf;
		if(pMsgPtr->nMsgSize < 4 || pMsgPtr->nMsgSize > MAX_PACKETSIZE)
			return -1;
		if(pMsgPtr->nMsgSize + (bSealed ? PROTOCOL_MSG_SEALSIZE : 0) <= nBufLen)
		{
			*pidPacket	= pMsgPtr->idPacket;
			*pMsgSize	= pMsgPtr->nMsgSize - PROTOCOL_MSG_HEADSIZE;
			*ppMsg		= pMsgPtr->pMsg;
			if(bSealed && memcmp(pBuf + pMsgPtr->nMsgSize, "TQClient", PROTOCOL_MSG_SEALSIZE) != 0)
				return -1;
			return pMsgPtr->nMsgSize;
		}
	}
	return 0;
}

inline int	UnitePacket(char* pBuf, int nBufLen, OBJID idPacket, const char* pMsg, int nMsgSize)
{
	if(nBufLen >= nMsgSize + PROTOCOL_MSG_HEADSIZE)
	{
		CProtocolMsgStruct*	pMsgPtr = (CProtocolMsgStruct*)pBuf;
		pMsgPtr->idPacket		= (unsigned short)idPacket;
		pMsgPtr->nMsgSize		= nMsgSize + PROTOCOL_MSG_HEADSIZE;
		memcpy(pMsgPtr->pMsg,	pMsg, (size_t)nMsgSize);
		return pMsgPtr->nMsgSize;
	}
	return 0;
}

const int	PROTOCOL_NPCMSG_HEADSIZE		= 8;
struct CProtocolNpcMsgStruct{
	unsigned short	nMsgSize;
	unsigned short	idPacket;
	unsigned long	idNpc;
	char			pMsg[1];
};

inline int	SplitNpcPacket(const char* pBuf, int nBufLen, OBJID* pidPacket, char** ppMsg, int* pMsgSize, OBJID* pidNpc)
{
	if(nBufLen > PROTOCOL_NPCMSG_HEADSIZE)
	{
		CProtocolNpcMsgStruct*	pMsgPtr = (CProtocolNpcMsgStruct*)pBuf;
		if(pMsgPtr->nMsgSize <= nBufLen)
		{
			if(pMsgPtr->nMsgSize < 4 || pMsgPtr->nMsgSize > MAX_PACKETSIZE)
				return 0;

			*pidPacket	= pMsgPtr->idPacket;
			*pMsgSize	= pMsgPtr->nMsgSize - PROTOCOL_NPCMSG_HEADSIZE;
			*pidNpc		= pMsgPtr->idNpc;
			*ppMsg		= pMsgPtr->pMsg;
			return pMsgPtr->nMsgSize;
		}
	}
	return 0;
}

inline int	UniteNpcPacket(char* pBuf, int nBufLen, OBJID idPacket, const char* pMsg, int nMsgSize, OBJID idNpc)
{
	if(nBufLen >= nMsgSize + PROTOCOL_NPCMSG_HEADSIZE)
	{
		CProtocolNpcMsgStruct*	pMsgPtr = (CProtocolNpcMsgStruct*)pBuf;
		pMsgPtr->idPacket		= (unsigned short)idPacket;
		pMsgPtr->nMsgSize		= nMsgSize + PROTOCOL_NPCMSG_HEADSIZE;
		pMsgPtr->idNpc			= idNpc;
		memcpy(pMsgPtr->pMsg,	pMsg, (size_t)nMsgSize);

		return pMsgPtr->nMsgSize;
	}
	return 0;
}

#endif