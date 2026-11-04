#ifndef	MSGBETOUTSIDE_H
#define MSGBETOUTSIDE_H

#include "NetMsg.h"

enum
{
	_ACTION_CHIPIN				= 0,
	_ACTION_CHIPIN_CONFIRM		= 1,
	_ACTION_CANCELCHIP			= 2,
	_ACTION_CANCELCHIP_CONFIRM	= 3,
	_ACTION_BEGINCHIP			= 4,
	_ACTION_ENDCHIP				= 5,
	_ACTION_DICE				= 6,
};

class CMsgBODice : public CNetMsg
{
public:
	CMsgBODice();
	virtual ~CMsgBODice();

	BOOL	Create (int nAction, OBJID idNpc, int nTime=0);
	BOOL	Append (int nType, DWORD dwData);
	BOOL	Create (int nAction, OBJID idNpc, char aDice[4]);

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		unsigned char	ucAction;
		union
		{
			unsigned char	ucTime;
			unsigned char	ucAmount;
		};
		OBJID			idDiceNpc;
		struct
		{
			unsigned char	ucType;
			union
			{
				DWORD			dwData;
				unsigned char	ucDice[4];
			};
		}Data[1];
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
