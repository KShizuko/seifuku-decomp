#if !defined(AFX_MSGREGISTER_H__CBFA01B1_4965_4A7E_B412_2CD9D70D19E6__INCLUDED_)
#define AFX_MSGREGISTER_H__CBFA01B1_4965_4A7E_B412_2CD9D70D19E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

class CMsgRegister : public world_kernel::CNetMsg
{
public:
	CMsgRegister();
	virtual ~CMsgRegister();

public:	
	BOOL		Create		(char* pMsgBuf, DWORD dwSize);
	void		Process		(void *pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		char	szAccount[_MAX_NAMESIZE];
		char	szName[_MAX_NAMESIZE];
		char	szPassword[_MAX_NAMESIZE];
		USHORT	usLook;
		USHORT	usProfession;
		OBJID	idAccount;
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
