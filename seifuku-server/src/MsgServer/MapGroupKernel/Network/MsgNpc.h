#if !defined(AFX_MSGNPC_H__B662BA21_4E11_41B2_B997_584B4833F266__INCLUDED_)
#define AFX_MSGNPC_H__B662BA21_4E11_41B2_B997_584B4833F266__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

enum {
		EVENT_BEACTIVED = 0,
		EVENT_ADDNPC,
		EVENT_LEAVEMAP,
		EVENT_DELNPC,
		EVENT_CHANGEPOS,
		EVENT_LAYNPC,
};

class CMsgNpc   : public CNetMsg  
{
public:
	CMsgNpc();
	virtual ~CMsgNpc();
	BOOL	Create	(int nAction, OBJID id, int nType = 0, DWORD dwData = 0);

public:	
	void			Process		(void* pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		OBJID id;
		union{
			DWORD			dwData;
			struct{
				USHORT			usPosX;
				USHORT			usPosY;
			};
		};
		unsigned short usAction;
		unsigned short usType;
	}MSG_Info;

	MSG_Info*	m_pInfo;

};

#endif
