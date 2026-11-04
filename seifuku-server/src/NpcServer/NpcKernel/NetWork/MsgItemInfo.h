#if !defined(AFX_MSGITEMINFO_H__683B9A1E_9C3D_41E1_B90D_8DF5ABE79632__INCLUDED_)
#define AFX_MSGITEMINFO_H__683B9A1E_9C3D_41E1_B90D_8DF5ABE79632__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "Item.h"

enum {	ITEMINFO_NONE=0,
		ITEMINFO_ADDITEM,
		ITEMINFO_TRADE,
		ITEMINFO_UPDATE,
		ITEMINFO_OTHERPLAYER_EQUIPMENT,
};

class CMsgItemInfo : public CNetMsg
{
public:
	CMsgItemInfo();
	virtual ~CMsgItemInfo();

	BOOL	Create		(char* pMsgBuf, DWORD dwSize);
	void	Process		(void* pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

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
		UCHAR		ucReserved14[2];
		int			nData;
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
