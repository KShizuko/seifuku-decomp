#if !defined(AFX_STRRES_H__B92BDD57_47DD_4856_B21B_58FCFC32B256__INCLUDED_)
#define AFX_STRRES_H__B92BDD57_47DD_4856_B21B_58FCFC32B256__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"
#include <map>
#include <string>
using namespace std;
typedef map<OBJID, string>	MAP_STRRES;
class CStrRes
{
public:
	CStrRes();
	virtual ~CStrRes();

public:
	const	char*	GetStr(OBJID idRes);

private:
	MAP_STRRES		m_setStr;

public:
	void	Init();
	void	Destroy();
};

#endif
