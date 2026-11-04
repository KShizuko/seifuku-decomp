#if !defined(AFX_MSGITEMINFO_H__683B9A1E_9C3D_41E1_B90D_8DF5ABE79632__INCLUDED_EX)
#define AFX_MSGITEMINFO_H__683B9A1E_9C3D_41E1_B90D_8DF5ABE79632__INCLUDED_EX

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "Item.h"

enum {	ITEMINFOEX_NONE=0,
		ITEMINFOEX_BOOTH,
		ITEMINFOEX_EQUIPMENT,
		ITEMINFOEX_BOOTH_EMONEY,
};

class CMsgItemInfoEx : public CNetMsg
{
public:
	CMsgItemInfoEx();
	virtual ~CMsgItemInfoEx();

	BOOL	Create		(CItem* pItem, OBJID idOwner, int nCost, int nAction = ITEMINFOEX_BOOTH);
	BOOL	Create		(char* pMsgBuf, DWORD dwSize);
	void	Process		(void* pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		OBJID		id;
		OBJID		idOwner;
		DWORD		dwPrice;
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
