#if !defined(AFX_MYINI_H__SFK_4294_INCLUDED_)
#define AFX_MYINI_H__SFK_4294_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"
#include <stdarg.h>
#include <string>
#include <map>
using namespace std;

class CMyString : public string
{
public:
	CMyString()	{}
	CMyString(const string& str) : string(str)	{}
	CMyString(const char* fmt, ...)
	{
		if (!fmt)
			return;
		char szBuf[4096] = "";
		try
		{
			va_list args;
			va_start(args, fmt);
			vsprintf(szBuf, fmt, args);
			va_end(args);
			this->assign(szBuf);
		}
		catch(...)
		{
			::LogMsg("Error: too big size of string in format.");
		}
	}
	virtual ~CMyString()	{}
	int		ToInt() const	{ return atoi(this->c_str()); }
	long	ToLong() const	{ return atol(this->c_str()); }
};

typedef map<CMyString, CMyString>		MAP_INIVALUE;
typedef map<CMyString, MAP_INIVALUE>	MAP_INISECTION;

class CMyIni
{
	friend struct MyIniProbe;
public:
	CMyIni(bool bShowError);
	virtual ~CMyIni();

	static CMyIni*	CreateNew(const char* pszIniFile, bool bShowError);
	BOOL	Load(const char* pszIniFile);
#ifdef GAMEDATA_EXPORTS
	BOOL	Load(const char* buf, DWORD dwSize, const char* pszName);
	static CMyIni*	CreateNew(const char* buf, DWORD dwSize, const char* pszName, bool bShowError);
#endif
	BOOL	SearchSection(const char* pszSection);
	const CMyString&	GetContent(const char* pszIndex);
	BOOL	HasSection(const char* pszSection);
	const char*	GetString(const char* pszSection, const char* pszIndex);
	int		GetInt(const char* pszSection, const char* pszIndex);

private:
	BOOL	IsValidLine(char* szLine);
	BOOL	ParseSection(char* szLine, CMyString& strSection);
	BOOL	ParseKeyValue(char* szLine, CMyString& strKey, CMyString& strValue);

private:
	MAP_INISECTION	m_mapSection;
	MAP_INISECTION::iterator	m_iterSection;
	bool			m_bShowError;
	string			m_strFile;
};

#endif
