#ifndef		MSGPACKAGE_H
#define		MSGPACKAGE_H

#include "NetMsg.h"

enum{
		MSGPACKAGE_QUERYLIST			= 0,
		MSGPACKAGE_CHECKIN				= 1,
		MSGPACKAGE_CHECKOUT				= 2,
		MSGPACKAGE_QUERYLIST2			= 3,
};

enum{
		MSGPACKAGE_TYPE_NONE			= 0,
		MSGPACKAGE_TYPE_STORAGE			= 10,
		MSGPACKAGE_TYPE_TRUNK			= 20,
		MSGPACKAGE_TYPE_CHEST			= 30,
};

typedef struct
{
	OBJID idItem;
	OBJID idItemType;
	UCHAR		ucIdent;
	UCHAR		ucGem1;
	UCHAR		ucGem2;
	UCHAR		ucMagic1;
	UCHAR		ucMagic2;
	UCHAR		ucMagic3;
	UCHAR		ucReduceDmg;
	USHORT		usAddLife;
	USHORT		usAntiMonster;
}MsgPackageItemInfo;

class CMsgPackage : public CNetMsg
{
public:
	CMsgPackage();
	virtual ~CMsgPackage();

public:
	BOOL			Create(int nAction, OBJID id, const MsgPackageItemInfo* buf, int nSize);
	BOOL			Create(int nAction, OBJID id, OBJID idItem);
public:
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

protected:
	typedef struct
	{
		MSGHEAD_DEFINE

		OBJID		id;
		UCHAR		ucAction;
		UCHAR		ucType;
		union {
			struct{
				USHORT		usSize;
				MsgPackageItemInfo	setItemInfo[1];
			};
			OBJID		idItem;
		};
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
