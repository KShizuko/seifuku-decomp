#if !defined(AFX_MSGCONNECT_H__F6692EA5_5D32_4E87_AA3C_4B37A6BE6695__INCLUDED_)
#define AFX_MSGCONNECT_H__F6692EA5_5D32_4E87_AA3C_4B37A6BE6695__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include <vector>

const int	MAX_MACADDRSIZE		= 6;
typedef struct tagMAC_ADDRESS
{
	BYTE b1,b2,b3,b4,b5,b6;
}MAC_ADDRESS,*LPMAC_ADDRESS;
void GetMacAddress(std::vector<MAC_ADDRESS>& setMac);

const int	_MAX_LANGUAGESIZE	= 2;

class CMsgConnect : public CNetMsg
{
public:
	CMsgConnect();
	virtual ~CMsgConnect();

	BOOL	Create	(OBJID idAccount, DWORD dwData, const char* pszInfo);
	BOOL	Create	(OBJID idAccount, DWORD dwData, int nVersion, const char* pszLanguage, const MAC_ADDRESS* pMac, DWORD dwRes);

public:
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

protected:
	typedef struct {
		unsigned short	unMsgSize;
		unsigned short	unMsgType;
		OBJID			idAccount;
		DWORD			dwData;
		union {
			char			szInfo[_MAX_NAMESIZE];
			struct {
				USHORT			usVersion;
				char			szLanguage[_MAX_LANGUAGESIZE];
				BYTE			abyMac[MAX_MACADDRSIZE];
				BYTE			abyPad[2];
				DWORD			dwRes;
			} game;
		};
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
