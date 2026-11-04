#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include "NetMsgTracker.h"
#include "basefunc.h"

typedef	std::unique_ptr<FILE, int (__cdecl*)(FILE*)>	FILE_GUARD;

CNetMsgTracker::CNetMsgTracker(const char* pszIniFile, const char* pszLogFile)
	: m_strLogFile(pszLogFile)
{
	FILE* fp = fopen(pszIniFile, "r");
	FILE_GUARD	guardFile(fp, fclose);
	if(!fp)
		return;

	int		nSection = 0;
	char	szLine[1024];
	while(fgets(szLine, sizeof(szLine), fp))
	{
		if(szLine[0] == ';' || szLine[0] == '\n')
			continue;
		if(_stricmp(szLine, "[AccountID]\n") == 0)
		{
			nSection = 1;
			continue;
		}
		if(_stricmp(szLine, "[MessageType]\n") == 0)
		{
			nSection = 2;
			continue;
		}

		int	nValue = atoi(szLine);
		if(nValue == 0)
			continue;
		if(nSection == 1)
			m_setAccount.insert(nValue);
		else if(nSection == 2)
			m_setType.insert(nValue);
	}
}

CNetMsgTracker::~CNetMsgTracker() {}

CNetMsgTracker* CNetMsgTracker::Instance()
{
	static CNetMsgTracker	s_obj("NetMsgTracker.ini", "gmlog/NetMsgTrack");
	return &s_obj;
}

bool CNetMsgTracker::Track(OBJID idAccount, OBJID idUser, int nType, int iSize, const char* pBuf)
{
	if(m_setAccount.find(idAccount) == m_setAccount.end() || m_setType.find(nType) == m_setType.end())
		return false;

	MYASSERT(pBuf && iSize >= 4);

	char	szTemp[64];
	_snprintf(szTemp, 64, "RX AccountID:%u, UserID:%u, type %u, size %d: ", idAccount, idUser, nType, iSize);
	std::string	strLine(szTemp);
	for(int i = 0; i < iSize; i++)
	{
		_snprintf(szTemp, 64, "%02X ", (unsigned char)pBuf[i]);
		strLine.append(szTemp, szTemp + strlen(szTemp));
		if((i & 7) == 7 && i + 1 <= iSize)
		{
			const char* pszGap = "- ";
			strLine.append(pszGap, pszGap + strlen(pszGap));
		}
	}
	MyLogSave(m_strLogFile.c_str(), strLine.c_str());
	return true;
}
