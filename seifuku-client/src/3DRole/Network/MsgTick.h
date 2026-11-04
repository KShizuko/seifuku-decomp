#if !defined(AFX_MSGTICK_H__C68B182B_68CB_4D0D_ACDC_8A87BAEB1AA7__INCLUDED_)
#define AFX_MSGTICK_H__C68B182B_68CB_4D0D_ACDC_8A87BAEB1AA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

class CMsgTick : public CNetMsg  
{
public:
	CMsgTick();
	virtual ~CMsgTick();

public:
	BOOL	Create	(char* pbufMsg, DWORD dwMsgSize);
	void	Process	(void* pInfo);
	void	Send	(void);

private:
	typedef struct {
		unsigned short	unMsgSize;
		unsigned short	unMsgType;
		OBJID			idPlayer;
		DWORD			dwData;
		DWORD			dwRandom[4];
		DWORD			dwChkData;
	}MSG_Info;
	
	MSG_Info* m_pInfo;
	friend struct MsgTickProbe;
};

#endif
