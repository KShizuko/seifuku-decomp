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
		INTERACT_DIVORCE,
		INTERACT_PRESENTMONEY,
		INTERACT_PRESENTITEM,
		INTERACT_SENDFLOWERS,
		INTERACT_KILL,
		INTERACT_JOINSYN,
		INTERACT_ACCEPTSYNMEMBER,
		INTERACT_KICKOUTSYNMEMBER,
		INTERACT_PRESENTPOWER,
		INTERACT_QUERYINFO,
		INTERACT_RUSHATK,
		INTERACT_MAGICATTACK, 
		INTERACT_ABORTMAGIC,
		INTERACT_REFLECTWEAPON,
		INTERACT_BUMP,
		INTERACT_SHOOT,
		INTERACT_REFLECTMAGIC,
		INTERACT_CUTMEAT,
		INTERACT_MINE,
		INTERACT_QUARRY,
		INTERACT_CHOP				= 30,
		INTERACT_HUSTLE,
		INTERACT_SOUL,
};

class CMsgInteract : public CNetMsg  
{
	friend struct MsgInteractProbe;
public:
	CMsgInteract();
	virtual ~CMsgInteract();

	BOOL	Create(USHORT usType, OBJID idSender, OBJID idTarget, OBJID idBullet);
	
	BOOL	Create(USHORT usType, OBJID idSender, OBJID idTarget, USHORT usPosX, USHORT usPosY, DWORD dwData=0);
	BOOL	Create(USHORT usType, OBJID idSender, OBJID idTarget, USHORT usPosX, USHORT usPosY, USHORT usData0, USHORT usData1, OBJID idCpoy=0);

public:	
	BOOL		Create		(char* pMsgBuf, DWORD dwSize);
	void		Process		(void* pInfo);
	void		Send		(void);

private:
	typedef struct{
		USHORT	unMsgSize;
		USHORT	unMsgType;
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
			DWORD dwBulletType;
			OBJID idBullet;
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
	}MSG_Info;
	
	MSG_Info*	m_pInfo;
};
#define	ENCODE_MAGICATTACK(idUser,usType,idTarget,usPosX,usPosY) {	\
				usType		= (::ExchangeShortBits((usType - 0x14BE),3) ^ (idUser) ^ 0x915D);	\
				idTarget	= ::ExchangeLongBits(((idTarget - 0x8B90B51A) ^ (idUser) ^ 0x5F2D2463),32-13); \
				usPosX		= (::ExchangeShortBits((usPosX - 0xDD12),1) ^ (idUser) ^ 0x2ED6);	\
usPosY		= (::ExchangeShortBits((usPosY - 0x76DE),5) ^ (idUser) ^ 0xB99B);	}
#define	DECODE_MAGICATTACK(idUser,usType,idTarget,usPosX,usPosY) {	\
				usType		= 0xFFFF&(::ExchangeShortBits(((usType) ^ (idUser) ^ 0x915D),16-3) + 0x14BE);	\
				idTarget	= (::ExchangeLongBits((idTarget),13) ^ (idUser) ^ 0x5F2D2463) + 0x8B90B51A; \
				usPosX		= 0xFFFF&(::ExchangeShortBits(((usPosX) ^ (idUser) ^ 0x2ED6),16-1) + 0xDD12);	\
usPosY		= 0xFFFF&(::ExchangeShortBits(((usPosY) ^ (idUser) ^ 0xB99B),16-5) + 0x76DE);	}

#endif
