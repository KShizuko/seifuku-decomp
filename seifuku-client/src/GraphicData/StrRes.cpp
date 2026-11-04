#include "StrRes.h"
#include <stdio.h>
#include <string.h>

static CRITICAL_SECTION	s_csStrRes;
static char				s_szStrResMissing[256];

CStrRes::CStrRes()
{
	::InitializeCriticalSection(&s_csStrRes);
}

CStrRes::~CStrRes()
{
	::DeleteCriticalSection(&s_csStrRes);
	this->Destroy();
}
void CStrRes::Destroy()
{
	m_setStr.clear();
}
void CStrRes::Init()
{
	const char	szFile[]="ini/StrRes.ini";

	FILE* fp = fopen(szFile, "r");
	if (!fp)
	{
		::ErrorMsg("can't open file:%s！", szFile);
		return;
	}

	char szLine[1024] = "";
	while(true)
	{
		if (NULL == fgets(szLine, 1024, fp))
			break;
		int nLength = strlen(szLine);
		if (nLength <= 0)
			continue;
		if (szLine[nLength-1] == '\n')
			szLine[nLength-1] = '\0';
		if (szLine[0] == ';')
			continue;
		char* pszValue = strchr(szLine, '=');
		if (!pszValue)
			continue;
		*pszValue = '\0';
		OBJID id = atoi(szLine);
		pszValue ++;
		m_setStr[id] = pszValue;
	}

	fclose(fp);
}
const char*	CStrRes::GetStr(OBJID idRes)
{
	MAP_STRRES::iterator iter = m_setStr.find(idRes);
	if (iter != m_setStr.end())
		return iter->second.c_str();

	::EnterCriticalSection(&s_csStrRes);
	sprintf(s_szStrResMissing, "SE:%u", idRes);
	::LeaveCriticalSection(&s_csStrRes);
	return s_szStrResMissing;
}
