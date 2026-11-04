#if !defined(AFX_MSGDICE_H__B3F539EB_1871_4A23_A7BD_72211205A127__INCLUDED_)
#define AFX_MSGDICE_H__B3F539EB_1871_4A23_A7BD_72211205A127__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

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
class CMsgDice : public CNetMsg 
{
	friend struct MsgDiceProbe;
public:
	CMsgDice();
	virtual ~CMsgDice();
	BOOL	Create	();

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);
	BOOL			Create		(OBJID idDice, int nAction, DWORD dwMoney, int nType = 0);	
private:
	typedef struct
	{
		unsigned short	unMsgSize;
		unsigned short	unMsgType;
		unsigned char	ucAction;
		union
		{
			unsigned char	ucTime;
			unsigned char	ucAmount;
		};
		OBJID			idDice;
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
