#if !defined(AFX_MSGACCOUNT_H__5D73358C_1E18_4877_BB14_969CC7CBAAF5__INCLUDED_A)
#define AFX_MSGACCOUNT_H__5D73358C_1E18_4877_BB14_969CC7CBAAF5__INCLUDED_A

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

class CMsgAccount_A : public world_kernel::CNetMsg
{
public:
	CMsgAccount_A();
	virtual ~CMsgAccount_A();

	BOOL	Create	(const char* pszAccount, const char* pszPsw, const char* pszServer, int nServerPort);

public:
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);

	typedef struct{
		unsigned short	unMsgSize;
		unsigned short	unMsgType;

		char			szAccount[_MAX_NAMESIZE];
		char			szPassword[_MAX_NAMESIZE];
		char			szServer[_MAX_NAMESIZE];
		int				nServerPort;
	}MSG_Info;

	MSG_Info*	m_pInfo;

};

#endif
