#if !defined(SFK_DISDAINDATASET_H__INCLUDED_)
#define SFK_DISDAINDATASET_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#pragma warning(disable:4786)
#include "define.h"
#include "GameData.h"
#include "GameObj.h"
#include <map>

#define	_TBL_DISDAIN			"cq_disdain"

enum DISDAINDATA
{
	DISDAINDATA_ID = 0,
	DISDAINDATA_TYPE,
	DISDAINDATA_DELTA_LEV,
	DISDAINDATA_USR_ATK_MST,
	DISDAINDATA_MST_ATK,
	DISDAINDATA_MAX_ATK,
	DISDAINDATA_MAX_XP_ATK,
	DISDAINDATA_EXP_FACTOR,
	DISDAINDATA_XP_EXP_FACTOR,
	DISDAINDATA_USR_ATK_USR_MIN,
	DISDAINDATA_USR_ATK_USR_MAX,
	DISDAINDATA_USR_ATK_USR_OVERADJ,
	DISDAINDATA_USR_ATK_USRX_MIN,
	DISDAINDATA_USR_ATK_USRX_MAX,
	DISDAINDATA_USR_ATK_USRX_OVERADJ,
	DISDAINDATA_USRX_ATK_USR_MIN,
	DISDAINDATA_USRX_ATK_USR_MAX,
	DISDAINDATA_USRX_ATK_USR_OVERADJ,
	DISDAINDATA_USRX_ATK_USRX_MIN,
	DISDAINDATA_USRX_ATK_USRX_MAX,
	DISDAINDATA_USRX_ATK_USRX_OVERADJ,
};

extern char szDisdainTable[];
typedef	CGameData<DISDAINDATA,szDisdainTable,szID>	CDisdainData;

class IDatabase;
class CDisdainDataSet
{
public:
	CDisdainDataSet();
	virtual ~CDisdainDataSet();

public:
	static CDisdainDataSet*	CreateNew()		{ return new CDisdainDataSet; }
	void		Release()					{ delete this; }
	bool		Init		(IDatabase* pDb);
	CDisdainData*	Query	(int nType, int nDeltaLev);

protected:
	typedef	std::map<__int64, CDisdainData*>	DISDAIN_MAP;
	DISDAIN_MAP		m_map;
};

#endif
