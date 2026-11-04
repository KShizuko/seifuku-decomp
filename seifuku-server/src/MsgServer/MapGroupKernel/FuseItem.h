#if !defined(AFX_FUSEITEM_H__SFK_2008_INCLUDED_)
#define AFX_FUSEITEM_H__SFK_2008_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include <map>

class IDatabase;
class CItem;

enum FUSEDATA {
		FUSEDATA_ID_ = 0,
		FUSEDATA_SORT,
		FUSEDATA_TYPE,
		FUSEDATA_LEV,
		FUSEDATA_QUALITY,
		FUSEDATA_CHANCE,
};

const int	FUSESORT_EQUIP			= 1;
const int	FUSEKEY_SORT_BASE		= 100000000;

class CFuseItem
{
public:
	CFuseItem();
	virtual ~CFuseItem();

public:
	bool	Create	(IDatabase* pDb);
	int		GetItemScore	(CItem* pItem);

protected:
	typedef	std::map<int, int>	FUSE_SET;
	FUSE_SET	m_setFuse;
};

extern CFuseItem*	g_pFuseItem;

#endif
