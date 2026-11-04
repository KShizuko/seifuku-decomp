#if !defined(SFK_LOTTERY_MSGITEMINFO_H_INCLUDED)
#define SFK_LOTTERY_MSGITEMINFO_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "I_Adapter.h"

const USHORT	_MSG_ITEMINFO		= 1008;
const int		_ITEM_STATUS_NOT_IDENT	= 1;

enum {	ITEMINFO_NONE=0,
		ITEMINFO_ADDITEM,
		ITEMINFO_TRADE,
		ITEMINFO_UPDATE,
};

class CMsgItemInfo : public CNetMsg
{
public:
	CMsgItemInfo();
	virtual ~CMsgItemInfo()	{}

public:
	bool	Create		(ILotteryPrize* pPrize, int nAction = ITEMINFO_ADDITEM, OBJID idUser = ID_NONE);
	virtual void	Process		(void* pInfo);

protected:
	typedef struct {
		PLUGINMSGHEAD_DEFINE

		OBJID		id;
		DWORD		dwType;
		USHORT		usAmount;
		USHORT		usAmountLimit;
		UCHAR		ucAction;
		UCHAR		ucIdent;
		UCHAR		ucPosition;
		UCHAR		ucGem1;
		UCHAR		ucGem2;
		UCHAR		ucMagic1;
		UCHAR		ucMagic2;
		UCHAR		ucMagic3;
		UCHAR		ucReduceDmg;
		UCHAR		ucAddLife;
		int			nData;
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
