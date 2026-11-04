#ifndef _NETMSGTRACKER_H
#define _NETMSGTRACKER_H

#pragma	warning(disable:4786)
#include <string>
#include <set>
#include "define.h"

class CNetMsgTracker
{
public:
	CNetMsgTracker(const char* pszIniFile, const char* pszLogFile);
	~CNetMsgTracker();

	static CNetMsgTracker*	Instance();
	bool	Track(OBJID idAccount, OBJID idUser, int nType, int iSize, const char* pBuf);

protected:
	typedef	std::set<OBJID>	ACCOUNT_SET;
	typedef	std::set<int>	TYPE_SET;
	std::string		m_strLogFile;
	ACCOUNT_SET		m_setAccount;
	TYPE_SET		m_setType;
};

#endif
