#if !defined(AFX_MSGPACKAGE_H__739F5FDB_46FD_444B_9ADE_C157E831FBF8__INCLUDED_)
#define AFX_MSGPACKAGE_H__739F5FDB_46FD_444B_9ADE_C157E831FBF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

enum
{	
		MSGPACKAGE_TYPE_NONE		= 0,
		MSGPACKAGE_TYPE_STORAGE		= 10,
		MSGPACKAGE_TYPE_TRUNK		= 20,
		MSGPACKAGE_TYPE_CHEST		= 30,
		MSGPACKAGE_TYPE_EUDEMON_BROODER = 40,
		MSGPACKAGE_TYPE_EUDEMON_STORAGE = 50,
		MSGPACKAGE_TYPE_AUCTION_STORAGE = 60,
};

enum
{
		MSGPACKAGE_QUERYLIST		= 0,
		MSGPACKAGE_CHECKIN			= 1,
		MSGPACKAGE_CHECKOUT			= 2,
		MSGPACKAGE_QUERYLIST2		= 3,
};
typedef struct
{
	OBJID		idItem;
	OBJID		idItemType;
	UCHAR		ucStatus;
	UCHAR		ucGem1;
	UCHAR		ucGem2;
	UCHAR		ucMagic1;
	UCHAR		ucMagic2;
	UCHAR		ucMagic3;
	UCHAR		ucBless;
	UCHAR		ucReserved;
	USHORT		usEnchant;
	USHORT		usRestrain;
}MsgPackageItemInfo;
class CMsgPackage : public CNetMsg  
{
public:
	CMsgPackage();
	virtual ~CMsgPackage();

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);
	BOOL			Create		(int nAction, int nType, OBJID id, OBJID idItem);

protected:
	typedef struct
	{
		USHORT	unMsgSize;
		USHORT	unMsgType;
		OBJID		id;
		unsigned char	ucAction;
		unsigned char	ucType;
		union {
				struct
				{
					USHORT				usItemAmount;	
					MsgPackageItemInfo	setItemInfo[1];
				};
				OBJID		idItem;
		      };
	}MSG_Info;
	
	MSG_Info*	m_pInfo;
};

#endif
