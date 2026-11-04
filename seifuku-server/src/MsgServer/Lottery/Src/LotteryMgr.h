#if !defined(SFK_LOTTERYMGR_H_INCLUDED)
#define SFK_LOTTERYMGR_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>
#include <map>
#include "define.h"
#include "I_mydb.h"
#include "I_Adapter.h"
#include "Lottery.h"
#include "ConfigMgr.h"

const USHORT	actionLottery	= 242;
#define	_TBL_LOTTERY			"cq_lottery"

class CLotteryMgr : public ILotteryMgr
{
public:
	CLotteryMgr()	{}
	static CLotteryMgr*	GetInstance	();

public:
	virtual bool		Lottery			(int nLotteryType, int nColor, ILotteryActor* pActor, bool bTest);
	virtual ~CLotteryMgr();
	virtual bool		GetLotteryRank	(int nLotteryType, ILotteryActor* pActor, int* pRank, int* pBigRank);
	virtual CLottery*	GetLottery		(int nLotteryType, int nRank, int nColor);
	virtual bool		AwardPrize		(CLottery* pLottery, ILotteryActor* pActor);
	virtual CLottery*	QueryLottery	(OBJID idLottery);
	virtual int			SumChance		(int nLotteryType, int nRank);
	virtual CLottery*	GetLotteryByRoll(int nLotteryType, int nRank, int nRoll);

public:
	bool		Create			(IDatabase* pDb);

protected:
	typedef std::map<OBJID, CLottery*>	LOTTERY_SET;
	LOTTERY_SET		m_setLottery;
};

extern "C" __declspec(dllexport) bool ModuleCreate(IRegister* pRegister, IDatabase* pDb);

#endif
