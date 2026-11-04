#include "MyIni.h"

CMyIni::CMyIni(bool bShowError)
{
	m_bShowError = bShowError;
}

CMyIni::~CMyIni()
{
}
static CMyString s_strEmpty;
BOOL CMyIni::SearchSection(const char* pszSection)
{
	if (!pszSection)
	{
		m_iterSection = m_mapSection.begin();
		return true;
	}
	CMyString strSection(pszSection);
	m_iterSection = m_mapSection.find(strSection);
	return m_iterSection != m_mapSection.end();
}

const CMyString& CMyIni::GetContent(const char* pszIndex)
{
	IF_NOT(pszIndex)
		return s_strEmpty;
	CMyString strIndex(pszIndex);
	MAP_INIVALUE::iterator itValue = m_iterSection->second.find(strIndex);
	if (itValue == m_iterSection->second.end())
	{
		if (m_bShowError)
			::ErrorMsg("section[%s], index[%s] not found in %s!", m_iterSection->first.c_str(), pszIndex, m_strFile.c_str());
		return s_strEmpty;
	}
	return itValue->second;
}

BOOL CMyIni::HasSection(const char* pszSection)
{
	CMyString strSection(pszSection);
	return m_mapSection.find(strSection) != m_mapSection.end();
}

BOOL CMyIni::IsValidLine(char* szLine)
{
	IF_NOT(szLine)
		return false;
	if (strlen(szLine) <= 2)
		return false;
	switch (szLine[0])
	{
	case '\t':
	case '\n':
	case '\r':
	case ' ':
	case '/':
	case ';':
	case '=':
	case '\\':
		return false;
	}
	return true;
}

BOOL CMyIni::ParseSection(char* szLine, CMyString& strSection)
{
	IF_NOT(szLine)
		return false;
	if (szLine[0] != '[')
		return false;
	int nLen = strlen(szLine);
	int i;
	for (i = 1; i < nLen; i++)
	{
		if (szLine[i] == ']')
			break;
	}
	if (i >= nLen)
		return false;
	szLine[i] = 0;
	if (i >= nLen)
		return false;
	strSection.assign(szLine + 1);
	return true;
}

BOOL CMyIni::ParseKeyValue(char* szLine, CMyString& strKey, CMyString& strValue)
{
	if (!this->IsValidLine(szLine))
		return false;
	int nLen = strlen(szLine);
	int i;
	for (i = 0; i < nLen; i++)
	{
		if (szLine[i] == '=')
			break;
	}
	if (i >= nLen)
		return false;
	szLine[i] = 0;
	for (int j = i; j > 0; j--)
	{
		if (szLine[j-1] == ' ' || szLine[j-1] == '\t')
			szLine[j-1] = 0;
		else
			break;
	}
	strKey.assign(szLine);
	if (i >= nLen)
		return false;
	if (i + 1 >= nLen)
		return true;
	char* pszValue = szLine + i + 1;
	int nValueLen = strlen(pszValue);
	int k;
	for (k = 0; k < nValueLen; k++)
	{
		if (pszValue[k] != ' ' && pszValue[k] != '\t')
			break;
	}
	pszValue += k;
	nValueLen = strlen(pszValue);
	for (k = 0; k < nValueLen; k++)
	{
		if (pszValue[k] == ' ' || pszValue[k] == '\t' || pszValue[k] == ';' || pszValue[k] == '\r' || pszValue[k] == '\n')
		{
			pszValue[k] = 0;
			break;
		}
	}
	strValue.assign(pszValue);
	return true;
}

BOOL CMyIni::Load(const char* pszIniFile)
{
	IF_NOT(pszIniFile)
		return false;
	m_mapSection.clear();
	FILE* fp = fopen(pszIniFile, "r");
	if (!fp)
		return false;
	MAP_INIVALUE mapValue;
	CMyString strSection;
	char szLine[1024] = "";
	while (fgets(szLine, sizeof(szLine), fp))
	{
		int nLen = strlen(szLine);
		if (nLen <= 2)
			continue;
		if (szLine[nLen-1] == '\n')
			szLine[nLen-1] = 0;
		CMyString strName;
		if (this->ParseSection(szLine, strName))
		{
			if (strSection.length() != 0)
			{
				MAP_INISECTION::iterator it = m_mapSection.insert(MAP_INISECTION::value_type(strSection, MAP_INIVALUE())).first;
				it->second = mapValue;
			}
			strSection = strName;
			mapValue.clear();
		}
		else if (strSection.length() != 0)
		{
			CMyString strKey, strValue;
			if (this->ParseKeyValue(szLine, strKey, strValue))
			{
				MAP_INIVALUE::iterator it = mapValue.insert(MAP_INIVALUE::value_type(strKey, CMyString())).first;
				it->second = strValue;
			}
		}
	}
	if (strSection.length() != 0)
	{
		MAP_INISECTION::iterator it = m_mapSection.insert(MAP_INISECTION::value_type(strSection, MAP_INIVALUE())).first;
		it->second = mapValue;
	}
	fclose(fp);
	m_strFile = pszIniFile;
	return true;
}

int CMyIni::GetInt(const char* pszSection, const char* pszIndex)
{
	IF_NOT(pszSection && pszIndex)
		return 0;
	CMyString strSection(pszSection);
	MAP_INISECTION::iterator itSection = m_mapSection.find(strSection);
	if (itSection == m_mapSection.end())
	{
		if (m_bShowError)
			::ErrorMsg("section[%s] not found in %s!", pszSection, m_strFile.c_str());
		return 0;
	}
	CMyString strIndex(pszIndex);
	MAP_INIVALUE::iterator itValue = itSection->second.find(strIndex);
	if (itValue == itSection->second.end())
	{
		if (m_bShowError)
			::ErrorMsg("section[%s], index[%s] not found in %s!", pszSection, pszIndex, m_strFile.c_str());
		return 0;
	}
	return atoi(itValue->second.c_str());
}

const char* CMyIni::GetString(const char* pszSection, const char* pszIndex)
{
	IF_NOT(pszSection && pszIndex)
		return NULL;
	CMyString strSection(pszSection);
	MAP_INISECTION::iterator itSection = m_mapSection.find(strSection);
	if (itSection == m_mapSection.end())
	{
		if (m_bShowError)
			::ErrorMsg("section[%s] not found in %s!", pszSection, m_strFile.c_str());
		return NULL;
	}
	CMyString strIndex(pszIndex);
	MAP_INIVALUE::iterator itValue = itSection->second.find(strIndex);
	if (itValue == itSection->second.end())
	{
		if (m_bShowError)
			::ErrorMsg("section[%s], index[%s] not found in %s!", pszSection, pszIndex, m_strFile.c_str());
		return NULL;
	}
	return itValue->second.c_str();
}

#ifdef GAMEDATA_EXPORTS
BOOL CMyIni::Load(const char* buf, DWORD dwSize, const char* pszName)
{
	IF_NOT(buf)
		return false;
	m_mapSection.clear();
	MAP_INIVALUE mapValue;
	CMyString strSection;
	DWORD dwPos = 0;
	char szLine[1024] = "";
	while (::ReadLine(buf, dwSize, &dwPos, szLine, sizeof(szLine)))
	{
		int nLen = strlen(szLine);
		if (nLen <= 2)
			continue;
		if (szLine[nLen-1] == '\n')
			szLine[nLen-1] = 0;
		CMyString strName;
		if (this->ParseSection(szLine, strName))
		{
			if (strSection.length() != 0)
			{
				MAP_INISECTION::iterator it = m_mapSection.insert(MAP_INISECTION::value_type(strSection, MAP_INIVALUE())).first;
				it->second = mapValue;
			}
			strSection = strName;
			mapValue.clear();
		}
		else if (strSection.length() != 0)
		{
			CMyString strKey, strValue;
			if (this->ParseKeyValue(szLine, strKey, strValue))
			{
				MAP_INIVALUE::iterator it = mapValue.insert(MAP_INIVALUE::value_type(strKey, CMyString())).first;
				it->second = strValue;
			}
		}
	}
	if (strSection.length() != 0)
	{
		MAP_INISECTION::iterator it = m_mapSection.insert(MAP_INISECTION::value_type(strSection, MAP_INIVALUE())).first;
		it->second = mapValue;
	}
	m_strFile = pszName;
	return true;
}
CMyIni* CMyIni::CreateNew(const char* buf, DWORD dwSize, const char* pszName, bool bShowError)
{
	IF_NOT(buf)
		return NULL;
	CMyIni* pMyIni = new CMyIni(bShowError);
	IF_NOT(pMyIni)
		return NULL;
	if (!pMyIni->Load(buf, dwSize, pszName))
	{
		delete pMyIni;
		return NULL;
	}
	return pMyIni;
}
#endif
CMyIni* CMyIni::CreateNew(const char* pszIniFile, bool bShowError)
{
	IF_NOT(pszIniFile)
		return NULL;
	CMyIni* pMyIni = new CMyIni(bShowError);
	IF_NOT(pMyIni)
		return NULL;
	if (!pMyIni->Load(pszIniFile))
	{
		delete pMyIni;
		return NULL;
	}
	return pMyIni;
}
