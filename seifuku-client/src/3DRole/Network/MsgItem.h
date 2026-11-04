#if !defined(AFX_MSGITEM_H__9AB1BCEA_F6D1_4413_A39D_8AC35C9FF655__INCLUDED_)
#define AFX_MSGITEM_H__9AB1BCEA_F6D1_4413_A39D_8AC35C9FF655__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

enum
{
		ITEMACT_NONE			=0, 
		ITEMACT_BUY				=1,
		ITEMACT_SELL			=2,
		ITEMACT_DROP			=3,
		ITEMACT_USE				=4,
		ITEMACT_EQUIP			=5,
		ITEMACT_UNEQUIP			=6,
		ITEMACT_SPLITITEM		=7,
		ITEMACT_COMBINEITEM		=8,
		ITEMACT_QUERYMONEYSAVED	=9,
		ITEMACT_SAVEMONEY		=10,
		ITEMACT_DRAWMONEY		=11,
		ITEMACT_DROPMONEY		=12,
		ITEMACT_SPENDMONEY		=13,
		ITEMACT_REPAIR			=14,
		ITEMACT_REPAIRALL		=15,
		ITEMACT_IDENT			=16,
		ITEMACT_DURABILITY		=17,
		ITEMACT_DROPEQUIPMENT	=18,
		ITEMACT_IMPROVE			=19,
		ITEMACT_UPLEV			=20,
		ITEMACT_BOOTH_QUERY		=21,
		ITEMACT_BOOTH_ADD		=22,
		ITEMACT_BOOTH_DEL		=23,
		ITEMACT_BOOTH_BUY		=24,
		ITEMACT_SYNCHRO_AMOUNT	=25,
		ITEMACT_FIREWORKS		=26,
		
		ITEMACT_PING			=27,
		ITEMACT_ENCHANT			=28,
		ITEMACT_BOOTH_ADD_EMONEY	=29,
};	

class CMsgItem : public CNetMsg  
{
public:
	CMsgItem();
	virtual ~CMsgItem();

	BOOL	Create	(OBJID id, DWORD data, int nAction);
	BOOL	Create	(OBJID id, USHORT usPosX, USHORT usPosY, int nAction);

public:	
	BOOL	Create		(char* pMsgBuf, DWORD dwSize);
	void	Process		(void* pInfo);

protected:
	typedef struct {
		unsigned short	unMsgSize;
		unsigned short	unMsgType;

		OBJID		id;
		union {
			DWORD		dwData;
			struct {
				USHORT	usPosY;
				USHORT	usPosX;
			};
		};
		USHORT		usAction;
		DWORD		dwTimeStamp;
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
