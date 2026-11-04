#if !defined(AFX_MSGCHARARRAY_H__9C42BA31_AE9D_493E_9336_BD1BC2615921__INCLUDED_)
#define AFX_MSGCHARARRAY_H__9C42BA31_AE9D_493E_9336_BD1BC2615921__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

enum
{
	_DATAARRAY_ACTION_MEDAL				=	0,
	_DATAARRAY_ACTION_HONORTITLE		=	1,

	_DATAARRAY_ACTION_UPGRADE_QUALITY		=2,
	_DATAARRAY_ACTION_UPGRADE_SUPERADDITION =3,
	_DATAARRAY_ACTION_UPGRADE_EQUIPLEVEL	=4,
	_DATAARRAY_ACTION_EMBEDGEM				=5,
	_DATAARRAY_ACTION_TABLE					=75,
};

#define MAX_LEVEL_SUPERADDITION 9

class CMsgDataArray   : public CNetMsg 
{
public:
	CMsgDataArray();
	virtual ~CMsgDataArray();

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

	BOOL	Create(DWORD dwData0, DWORD dwData1, DWORD dwData2, DWORD dwData3, DWORD dwData4);

protected:
	typedef struct 
	{
		USHORT			unMsgSize;
		USHORT			unMsgType;
		unsigned char	ucAction;
		unsigned char	ucAmount;
		union
		{
			unsigned char	ucChar[1];
			char			cChar[1];
			unsigned short	usShort[1];
			short			sShort[1];
			unsigned long	dwLong[1];
			long			nLong[1];
		};
	}MSG_Info;
	MSG_Info*	m_pInfo;
};

#endif
