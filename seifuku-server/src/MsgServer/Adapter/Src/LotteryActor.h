#if !defined(SFK_LOTTERYACTOR_H_INCLUDED)
#define SFK_LOTTERYACTOR_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include "I_Adapter.h"
#include "User.h"

class CLotteryActor : public ILotteryActor
{
public:
	CLotteryActor(CUser* pUser, PROCESS_ID idProcess);
	virtual ~CLotteryActor()		{}

public:
	virtual USHORT	Release				()							{ delete this; return 0; }
	virtual OBJID	GetID				()							{ return m_pUser->GetID(); }
	virtual LPCTSTR	GetName				()							{ return m_pUser->GetName(); }
	virtual OBJID	GetMapID			()							{ return m_pUser->GetMapID(); }
	virtual DWORD	GetLev				()							{ return m_pUser->GetLev(); }
	virtual __int64	GetExp				()							{ return m_pUser->GetExp(); }
	virtual int		GetBigPrizeFails	()							{ return m_pUser->GetBigPrizeFails(); }
	virtual void	SetBigPrizeFails	(int nData, bool bUpdate);
	virtual int		GetSmallPrizeFails	()							{ return m_pUser->GetSmallPrizeFails(); }
	virtual void	SetSmallPrizeFails	(int nData, bool bUpdate);
	virtual ILotteryPrize*	AwardItem	(int nItemType);
	virtual bool	SendSysMsg			(const char* fmt, ...);
	virtual bool	SendMsg				(PLUGIN_MSG* pMsg);
	virtual bool	BroadcastMsg		(const char* fmt, ...);
	virtual bool	BroadcastRoomMsg	(PLUGIN_MSG* pMsg);

protected:
	CUser*			m_pUser;
	PROCESS_ID		m_idProcess;
};

ILotteryActor*	LotteryActorCreate(CUser* pUser, PROCESS_ID idProcess);

#endif
