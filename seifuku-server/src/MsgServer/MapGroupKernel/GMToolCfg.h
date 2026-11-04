#if !defined(AFX_GMTOOLCFG_H__SFK_2008_INCLUDED_)
#define AFX_GMTOOLCFG_H__SFK_2008_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "windows.h"
#include "define.h"
#include <vector>

class IDatabase;

enum GMTOOLCFGDATA {
		GMTOOLCFGDATA_MAPID		= 2,
		GMTOOLCFGDATA_X,
		GMTOOLCFGDATA_Y,
		GMTOOLCFGDATA_CX,
		GMTOOLCFGDATA_CY,
};

struct GMTOOL_REGION
{
	OBJID	idMap;
	RECT	rc;
};

class CGMToolCfg
{
public:
	CGMToolCfg();
	~CGMToolCfg();
	static CGMToolCfg*	CreateNew	(IDatabase* pDb);
	void	Release		();

public:
	bool	Create		(IDatabase* pDb);
	bool	IsInRegion	(OBJID idMap, POINT pos);

protected:
	std::vector<DWORD>			m_setUnknown;
	std::vector<GMTOOL_REGION>	m_setRegion;
};

extern CGMToolCfg*	g_pGMToolCfg;

#endif
