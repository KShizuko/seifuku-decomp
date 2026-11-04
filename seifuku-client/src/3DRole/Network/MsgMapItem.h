#if !defined(AFX_MSGMAPITEM_H__010D6614_C45C_48F7_83F5_640FE069D2D0__INCLUDED_)
#define AFX_MSGMAPITEM_H__010D6614_C45C_48F7_83F5_640FE069D2D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

enum
{	
	MSGMAPITEM_CREATE=1,
	MSGMAPITEM_DELETE=2,
	MSGMAPITEM_PICK=3,

	MSGMAPITEM_CAST_TRAP	=10,
	MSGMAPITEM_SYNCHRO_TRAP	=11,
	MSGMAPITEM_DROP_TRAP	=12,
	MSGMAPITEM_SCREENEFFECT	=13,
};

class CMsgMapItem  : public CNetMsg 
{
public:
	CMsgMapItem();
	virtual ~CMsgMapItem();
public:
	BOOL Create(OBJID idMapItem, int nPosX, int nPosY, unsigned char ucAction=MSGMAPITEM_PICK);

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);
	void			Send		(void);

protected:
	typedef struct 
	{
		USHORT			unMsgSize;
		USHORT			unMsgType;

		OBJID			id;
		union{
			OBJID			idType;
			USHORT			usLook;
		};
		unsigned short	usPosX;
		unsigned short	usPosY;
		unsigned char	ucAction;
		char			szBuf[1];
	}MSG_Info;
	
	MSG_Info*	m_pInfo;
};

#endif
