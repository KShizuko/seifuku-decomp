#if !defined(AFX_MSGNPC_H__B662BA21_4E11_41B2_B997_584B4833F266__INCLUDED_)
#define AFX_MSGNPC_H__B662BA21_4E11_41B2_B997_584B4833F266__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

enum { 
		MSGNPC_DELNPC = 1,
};

class CMsgNpc   : public CNetMsg  
{
public:
	CMsgNpc();
	virtual ~CMsgNpc();

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		OBJID id;
		union{
			DWORD		   dwData;
			struct{
				unsigned short usCellX;
				unsigned short usCellY;
			};
		};
		unsigned short usAction;
		unsigned short usType;
	}MSG_Info;

	MSG_Info*	m_pInfo;

};

#endif
