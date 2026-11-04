#ifndef	STRINGMANAGER_H
#define	STRINGMANAGER_H

#pragma warning(disable:4786)
#include <string>
#include <map>

class CStringManager
{
public:
	CStringManager();
	virtual ~CStringManager();

public:
	static CStringManager*	Instance();

	const char*	GetStr(const std::string& strKey);

protected:
	void	Clear();
	void	LoadLine(const char* pszLine);

protected:
	typedef std::map<std::string, std::string>	MAP_STRING;
	MAP_STRING	m_mapString;
};

#endif
