#if !defined(SFK_LOTTERYPRIZE_H_INCLUDED)
#define SFK_LOTTERYPRIZE_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include "I_Adapter.h"
#include "Item.h"

class CLotteryPrize : public ILotteryPrize
{
public:
	CLotteryPrize(CItem* pItem, PROCESS_ID idProcess);
	virtual ~CLotteryPrize()		{}

public:
	virtual OBJID	GetID			()							{ return m_pItem->GetID(); }
	virtual bool	IsEquip			();
	virtual bool	SetInt			(int nIdx, int nData);
	virtual bool	GetInfo			(PLUGIN_ITEMINFO* pInfo);
	virtual USHORT	Release			()							{ delete this; return 0; }

protected:
	CItem*			m_pItem;
	PROCESS_ID		m_idProcess;
};

ILotteryPrize*	LotteryPrizeCreate(CItem* pItem, PROCESS_ID idProcess);

#endif
