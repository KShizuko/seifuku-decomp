#ifndef	PROTOCOL_H
#define	PROTOCOL_H

#include <string.h>
#ifdef MAPGROUP_KERNEL
#include "UserData.h"
#endif

const int	PROTOCOL_MSG_HEADSIZE		= 4;
struct CProtocolMsgStruct{
	unsigned short	nMsgSize;
	unsigned short	idPacket;
	char			pMsg[1];
};

inline int	SplitPacket(const char* pBuf, int nBufLen, OBJID* pidPacket, char** ppMsg, int* pMsgSize)
{
	if(nBufLen > PROTOCOL_MSG_HEADSIZE)
	{
		CProtocolMsgStruct*	pMsgPtr = (CProtocolMsgStruct*)pBuf;
		if(pMsgPtr->nMsgSize <= nBufLen)
		{
			if(pMsgPtr->nMsgSize < 4 || pMsgPtr->nMsgSize > MAX_PACKETSIZE)
				return 0;

			*pidPacket	= pMsgPtr->idPacket;
			*pMsgSize	= pMsgPtr->nMsgSize - PROTOCOL_MSG_HEADSIZE;
			*ppMsg		= pMsgPtr->pMsg;
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