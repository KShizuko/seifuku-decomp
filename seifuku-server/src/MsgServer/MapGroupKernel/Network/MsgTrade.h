#if !defined(AFX_MSGTRADE_H__F3900087_7C9F_4D0D_906A_10E4EB5B5E40__INCLUDED_)
#define AFX_MSGTRADE_H__F3900087_7C9F_4D0D_906A_10E4EB5B5E40__INCLUDED_

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
const unsigned short _TRADE_MONEYALL		= 8;
const unsigned short _TRADE_SELF_MONEYALL	= 9;
const unsigned short _TRADE_OK				= 10;
const unsigned short _TRADE_ADDITEMFAIL		= 11;
const unsigned short _TRADE_EMONEYALL		= 12;
const unsigned short _TRADE_ADDEMONEY		= 13;
const unsigned short _TRADE_SELF_EMONEYALL	= 14;
const unsigned short _TRADE_UNFAIR			= 15;
const unsigned short _TRADE_UNFAIR_OK		= 16;

class CMsgTrade : public CNetMsg   
{
public:
	CMsgTrade();
	virtual ~CMsgTrade();

public:
	BOOL	Create		(unsigned short usAction, OBJID id);

	BOOL	Create		(char* pbufMsg, DWORD dwMsgSize);
	void	Process		(void* pInfo);

protected:
	typedef struct 
	{
		MSGHEAD_DEFINE

		union{
			OBJID			id;
			DWORD			nData;
		};
		unsigned short	usAction;
	}MSG_Info;
	
	MSG_Info* m_pInfo;
};

#endif
