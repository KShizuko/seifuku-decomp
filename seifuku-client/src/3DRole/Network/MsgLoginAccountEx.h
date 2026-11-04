#ifndef _MSG_LOGINACCOUNTEX_H
#define _MSG_LOGINACCOUNTEX_H

#include "NetMsg.h"

class CMsgLoginAccountEx : public CNetMsg
{
public:
	CMsgLoginAccountEx();
	virtual ~CMsgLoginAccountEx();

	void	Create	(const char* pszAccount, const char* pszPsw, const char* pszServer);

private:
	typedef struct{
		unsigned short	unMsgSize;
		unsigned short	unMsgType;
		char			szAccount[64];
		char			szPassword[64];
		char			szServer[_MAX_NAMESIZE];
	}MSG_Info;

	MSG_Info*	m_pInfo;
	friend struct MsgLoginAccountExProbe;

};

#endif
