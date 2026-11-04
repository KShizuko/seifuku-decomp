#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "IniFile.h"
#include "StringManager.h"

static void TrimLeft(char* pszStr);
static void TrimRight(char* pszStr);

CStringManager::CStringManager()
{
	char	szIniFile[]	= "shell.ini";
	char	szSection[]	= "Language";
	char	szKey[]		= "StringFile";
	CIniFile	ini(szIniFile, szSection);

	char	szLine[1024]	= "";
	char	szFile[256];
	if(!ini.GetString(szFile, szKey, 256))
	{
		::LogSave("ERROR: CStringManager::CStringManager() find session '[%s]%s=' fail! at %s, %d", szSection, szKey, __FILE__, __LINE__);
		return;
	}

	FILE* pFile = fopen(szFile, "r");
	if(!pFile)
	{
		::LogSave("ERROR: CStringManager::CStringManager() load string resource file %s fail! at %s, %d", szFile, __FILE__, __LINE__);
		return;
	}

	while(fgets(szLine, 1024, pFile))
		LoadLine(szLine);

	fclose(pFile);
}

CStringManager::~CStringManager()
{
	Clear();
}

CStringManager* CStringManager::Instance()
{
	static CStringManager	s_obj;
	return &s_obj;
}

void CStringManager::Clear()
{
	m_mapString.clear();
}

static void Trim(char* pszStr)
{
	TrimLeft(pszStr);
	TrimRight(pszStr);
}

static void TrimLeft(char* pszStr)
{
	if(!pszStr)
		return;

	int nLen = strlen(pszStr);
	while(nLen > 0 && (pszStr[0] == ' ' || pszStr[0] == '\t' || pszStr[0] == '\r' || pszStr[0] == '\n'))
	{
		memmove(pszStr, pszStr+1, nLen-1);
		pszStr[nLen-1] = 0;
		nLen = strlen(pszStr);
	}
}

static void TrimRight(char* pszStr)
{
	if(!pszStr)
		return;

	for(int i = strlen(pszStr); i >= 0; i--)
	{
		char c = pszStr[i-1];
		if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
			pszStr[i-1] = 0;
		else
			break;
	}
}

void CStringManager::LoadLine(const char* pszLine)
{
	const char* pEqual = strchr(pszLine, '=');
	if(!pEqual)
		return;

	char	buf[1024];
	memset(buf, 0, sizeof(buf));
	strncpy(buf, pszLine, pEqual - pszLine);
	Trim(buf);
	if(strlen(buf) == 0 || buf[0] == ';')
		return;

	std::string	strKey(buf);

	memset(buf, 0, sizeof(buf));
	strncpy(buf, pEqual + 1, strlen(pszLine) - (pEqual - pszLine) - 1);
	if(buf[strlen(buf)-1] == '\r' || buf[strlen(buf)-1] == '\n')
		buf[strlen(buf)-1] = 0;
	if(buf[strlen(buf)-1] == '\r' || buf[strlen(buf)-1] == '\n')
		buf[strlen(buf)-1] = 0;

	m_mapString[strKey] = buf;
}

const char* CStringManager::GetStr(const std::string& strKey)
{
	MAP_STRING::iterator iter = m_mapString.find(strKey);
	if(iter != m_mapString.end())
		return iter->second.c_str();

	::LogSave("ERROR: CStringManager::GetStr() Can't find string[%s]!", strKey.c_str());
	return "";
}
