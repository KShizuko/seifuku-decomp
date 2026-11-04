#if !defined(AFX_MSGPETINFO_H__5A842CE5_DE96_40B8_8352_F4F1C9E3DD68__INCLUDED_)
#define AFX_MSGPETINFO_H__5A842CE5_DE96_40B8_8352_F4F1C9E3DD68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
class CMsgPetInfo : public CNetMsg
{
public:
	CMsgPetInfo();
	virtual ~CMsgPetInfo();
public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);
public:
	typedef struct 
	{
		unsigned short	unMsgSize;
		unsigned short	unMsgType;
		
		OBJID			id;
		DWORD			dwLookFace;
		int				nAIType;
		unsigned short	usPosX;
		unsigned short	usPosY;
		char			szName[_MAX_NAMESIZE];

	}MSG_Info;
	MSG_Info*	m_pInfo;
};

#endif
