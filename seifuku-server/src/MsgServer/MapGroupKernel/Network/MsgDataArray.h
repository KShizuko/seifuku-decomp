#ifndef		_MSGDATAARRAY_H_
#define		_MSGDATAARRAY_H_

#include "NetMsg.h"

enum
{
	_DATAARRAY_ACTION_MEDAL				=	0,
	_DATAARRAY_ACTION_HONORTITLE		=	1,
	_DATAARRAY_ACTION_UPGRADE_QUALITY		=2,
	_DATAARRAY_ACTION_UPGRADE_SUPERADDITION	=3,
	_DATAARRAY_ACTION_UPGRADE_EQUIPLEVEL	=4,
	_DATAARRAY_ACTION_EMBEDGEM				=5,

	_DATAARRAY_ACTION_SETSYN_DRESS	=50,

	_DATAARRAY_SETITEMREBATE	=60,

	_DATAARRAY_ACTION_COMPOSE		=0,
	_DATAARRAY_ACTION_CRACKURL		=1,
};
#define MAX_LEVEL_SUPERADDITION	9

class CMsgDataArray : public CNetMsg
{
public:
	CMsgDataArray();
	virtual ~CMsgDataArray();

public:
public:
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

private:
	typedef struct {
		MSGHEAD_DEFINE

		unsigned char ucAction;
		unsigned char ucAmount;
		union
		{
			unsigned char ucChar[1];
			char cChar[1];
			unsigned short usShort[1];
			short sShort[1];
			unsigned long dwLong[1];
			long nLong[1];
		};
	}MSG_Info;
	
	MSG_Info*	m_pInfo;
};

#endif