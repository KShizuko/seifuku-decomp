#if !defined(MSGMAPITEM_HEADER)
#define MSGMAPITEM_HEADER

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

enum {	
		MSGMAPITEM_CREATE		=1,
		MSGMAPITEM_DELETE		=2,
		MSGMAPITEM_PICK			=3,

		MSGMAPITEM_CAST_TRAP	=10,
		MSGMAPITEM_SYNCHRO_TRAP	=11,
		MSGMAPITEM_DROP_TRAP	=12,
};

class CMapItem;
class CMapTrap;
class CMsgMapItem : public CNetMsg  
{
public:
	CMsgMapItem();
	virtual ~CMsgMapItem();

	BOOL	Create	(int nAction, CMapItem* pMapItem);
	BOOL	Create	(int nAction, CMapTrap* pTrap);
	BOOL	Create	(OBJID id, int nPosX, int nPosY, int nAction=MSGMAPITEM_PICK);

public:	
	BOOL	Create		(char* pMsgBuf, DWORD dwSize);
	void	Process		(void* pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		OBJID			id;
		union{
			OBJID			idType;
			USHORT			usLook;
		};
		USHORT			nPosX;
		USHORT			nPosY;
		UCHAR			nAction;
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
