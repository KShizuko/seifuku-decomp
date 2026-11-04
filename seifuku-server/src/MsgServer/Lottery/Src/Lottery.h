#if !defined(SFK_LOTTERY_H_INCLUDED)
#define SFK_LOTTERY_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>
#include <string>
#include "define.h"

struct ST_LOTTERY
{
	OBJID		id;
	int			nType;
	int			nRank;
	int			nChance;
	std::string	strPrizeName;
	OBJID		idPrizeItem;
	int			nColor;
	int			nHoleNum;
	int			nAdditionLev;

	ST_LOTTERY() : id(ID_NONE), nType(0), nRank(0), nChance(0), idPrizeItem(ID_NONE), nColor(0), nHoleNum(0), nAdditionLev(0) {}
};

class ILottery
{
public:
	virtual OBJID		GetID			()	= 0;
	virtual int			GetType			()	= 0;
	virtual int			GetRank			()	= 0;
	virtual int			GetChance		()	= 0;
	virtual LPCTSTR		GetPrizeName	()	= 0;
	virtual OBJID		GetPrizeItem	()	= 0;
	virtual int			GetColor		()	= 0;
	virtual int			GetHoleNum		()	= 0;
	virtual int			GetAdditionLev	()	= 0;
};

class CLottery : public ILottery
{
protected:
	CLottery()			{}
public:
	static CLottery*	CreateNew	(ST_LOTTERY* pInfo);
	bool		Create			(ST_LOTTERY* pInfo);

public:
	OBJID		GetID			()	{ return m_info.id; }
	int			GetType			()	{ return m_info.nType; }
	int			GetRank			()	{ return m_info.nRank; }
	int			GetChance		()	{ return m_info.nChance; }
	LPCTSTR		GetPrizeName	()	{ return m_info.strPrizeName.c_str(); }
	OBJID		GetPrizeItem	()	{ return m_info.idPrizeItem; }
	int			GetColor		()	{ return m_info.nColor; }
	int			GetHoleNum		()	{ return m_info.nHoleNum; }
	int			GetAdditionLev	()	{ return m_info.nAdditionLev; }

	virtual USHORT	Release		()	{ delete this; return 0; }
protected:
	virtual ~CLottery()	{}

protected:
	ST_LOTTERY	m_info;
};

#endif
