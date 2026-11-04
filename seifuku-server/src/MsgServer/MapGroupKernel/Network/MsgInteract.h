#if !defined(AFX_MSGSTEAL_H__05C99CA2_5829_11D5_A212_0000E8E7F527__INCLUDED_)
#define AFX_MSGSTEAL_H__05C99CA2_5829_11D5_A212_0000E8E7F527__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

enum {	INTERACT_NONE =0, 
		INTERACT_STEAL, 
		INTERACT_ATTACK, 
		INTERACT_HEAL, 
		INTERACT_POISON, 
		INTERACT_ASSASSINATE,
		INTERACT_FREEZE,
		INTERACT_UNFREEZE,
		INTERACT_COURT,
		INTERACT_MARRY,
		INTERACT_DIVORCE			= 10,
		INTERACT_PRESENTMONEY,
		INTERACT_PRESENTITEM,
		INTERACT_SENDFLOWERS,
		INTERACT_KILL,
		INTERACT_JOINSYN,
		INTERACT_ACCEPTSYNMEMBER,
		INTERACT_KICKOUTSYNMEMBER,
		INTERACT_PRESENTPOWER,
		INTERACT_QUERYINFO,
		INTERACT_RUSHATK			= 20,
		INTERACT_MAGICATTACK, 
		INTERACT_ABORTMAGIC,
		INTERACT_REFLECTWEAPON,
		INTERACT_BUMP,
		INTERACT_SHOOT				= 25,
		INTERACT_REFLECTMAGIC,
		INTERACT_CUTMEAT,
		INTERACT_MINE,
		INTERACT_QUARRY,
		INTERACT_CHOP				= 30,
		INTERACT_HUSTLE,
};

class CMsgInteract : public CNetMsg  
{
public:
	CMsgInteract();
	virtual ~CMsgInteract();

	BOOL	Create(USHORT unType, OBJID idSender, OBJID idTarget, USHORT unPosX, USHORT unPosY, DWORD dwData = 0, DWORD dwBattleEffect = 0);
	BOOL	Create(USHORT unType, OBJID idSender, OBJID idTarget, USHORT unPosX, USHORT unPosY, USHORT usMagicType, USHORT usMagicLev, DWORD dwBattleEffect);

public:	
	BOOL		Create		(char* pMsgBuf, DWORD dwSize);
	void		Process		(void* pInfo);
	
private:
	typedef struct{
		MSGHEAD_DEFINE

		DWORD	dwTimeStamp;
		
		OBJID	idSender;
		OBJID	idTarget;
		
		union
		{
			struct
			{
				USHORT	unPosX;
				USHORT	unPosY;
			};
		};

		USHORT	unType;
		union 
		{
			OBJID	idItem;
			DWORD	dwData;
			INT		nData;
			struct
			{
				USHORT	usMagicType;
				USHORT	usMagicLevel;
			};

			struct
			{
				USHORT	usData0;
				USHORT	usData1;
			};
		};
		DWORD	dwBattleEffect;
	}MSG_Info;
	
	MSG_Info*	m_pInfo;
};

#endif
