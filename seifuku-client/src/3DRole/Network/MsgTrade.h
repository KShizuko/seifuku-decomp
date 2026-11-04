#if !defined(AFX_MSGTRADE_H__0967762A_20BA_4F08_8C15_CA80AE0E2BD2__INCLUDED_)
#define AFX_MSGTRADE_H__0967762A_20BA_4F08_8C15_CA80AE0E2BD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

const unsigned short _TRADE_APPLY			= 1;
const unsigned short _TRADE_QUIT			= 2;
const unsigned short _TRADE_OPEN			= 3;
const unsigned short _TRADE_SUCCESS			= 4;
const unsigned short _TRADE_FALSE			= 5;
const unsigned short _TRADE_ADDITEM			= 6;
const unsigned short _TRADE_ADDMONEY		= 7;
const unsigned short _TRADE_PLAYERTOTALMONEY= 8;
const unsigned short _TRADE_HEROTOTALMONEY	= 9;
const unsigned short _TRADE_OK				= 10;
const unsigned short _TRADE_ADDITEM_FALSE	= 11;
const unsigned short _TRADE_PLAYERTOTALEMONEY	= 12;
const unsigned short _TRADE_ADDEMONEY			= 13;
const unsigned short _TRADE_HEROTOTALEMONEY	= 14;

class CMsgTrade : public CNetMsg    
{
public:
	CMsgTrade();
	virtual ~CMsgTrade();

public:
	BOOL	Create(OBJID idPlayer, unsigned short usAction);

	BOOL	Create(char* pbufMsg, DWORD dwMsgSize);
	void	Process(void* pInfo);

private:
	typedef struct {
		unsigned short	unMsgSize;
		unsigned short	unMsgType;

		union
		{
			OBJID			id;
			DWORD			dwData;
		};
		unsigned short	usAction;
	}MSG_Info;
	
	MSG_Info* m_pInfo;
	friend struct MsgTradeProbe;
};

#endif
