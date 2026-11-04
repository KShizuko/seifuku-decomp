#if !defined(SFK_LOTTERY_CONFIGMGR_H_INCLUDED)
#define SFK_LOTTERY_CONFIGMGR_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>
#include <string>
#include <map>
#include "define.h"
#include "I_mydb.h"

struct ST_LOTTERYCONFIG
{
	OBJID		id;
	DWORD		dwType;
	int			nData1;
	int			nData2;
	int			nData3;
	int			nData4;
	int			nData5;
	std::string	strDescribe;

	ST_LOTTERYCONFIG() : id(ID_NONE), dwType(0), nData1(0), nData2(0), nData3(0), nData4(0), nData5(0) {}
};

class CLotteryConfigSet
{
public:
	CLotteryConfigSet()		{}
	~CLotteryConfigSet()	{}
public:
	bool		AddConfig		(ST_LOTTERYCONFIG* pInfo);
	ST_LOTTERYCONFIG*	GetConfig	(DWORD dwType);
	bool		GetGuarantee	(int nLotteryType, DWORD* pInfo);
	DWORD		RollType		(int nLotteryType, int nRoll);
protected:
	typedef std::map<DWORD, ST_LOTTERYCONFIG>	CONFIG_SET;
	CONFIG_SET	m_setConfig;
};

class CConfigMgr
{
public:
	CConfigMgr()	{}
	~CConfigMgr();
	static CConfigMgr*	GetInstance	();
public:
	bool		Create			(IDatabase* pDb);
	CLotteryConfigSet*	GetConfigSet	(int nLotteryType);
protected:
	typedef std::map<int, CLotteryConfigSet*>	LOTTERYCONFIG_SET;
	LOTTERYCONFIG_SET	m_setConfig;
};

#endif
