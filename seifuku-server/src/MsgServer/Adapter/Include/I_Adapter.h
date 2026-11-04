#if !defined(SFK_I_ADAPTER_H_INCLUDED)
#define SFK_I_ADAPTER_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>
#include "define.h"

class IDatabase;

const int	ADAPTER_MODULE_LOTTERY	= 0x20;

struct PLUGIN_MSG
{
	DWORD		dwVtbl;
	USHORT		unMsgSize;
	USHORT		unMsgType;
	char		bufBody[1];
};

struct PLUGIN_ITEMINFO
{
	OBJID	id;
	OBJID	idType;
	OBJID	idOwner;
	OBJID	idPlayer;
	int		nAmount;
	int		nAmountLimit;
	int		nIdent;
	int		nPosition;
	int		nData;
	int		nGem1;
	int		nGem2;
	int		nMagic1;
	int		nMagic2;
	int		nMagic3;
	int		nReduceDmg;
	int		nAddLife;
	int		nAntiMonster;
	USHORT	usPlunder;
	USHORT	usSpecialFlag;
};

class IAdapter
{
public:
	virtual int		Unknown0		()								{ return 0; }
	virtual void*	CreateActor		(LPCTSTR szName)				= 0;
	virtual void*	Find			(int idModule, int nUnknown)	= 0;
	virtual void*	GetActor		(int idModule, void* pBuf)		= 0;
};

class IRegister
{
public:
	virtual bool	Register		(int idModule, void* pModule)	= 0;
};

typedef bool (*MODULE_CREATE)(IRegister* pRegister, IDatabase* pDb);

class ILotteryPrize;
class ILotteryActor
{
public:
	virtual USHORT	Release				()								= 0;
	virtual OBJID	GetID				()								= 0;
	virtual LPCTSTR	GetName				()								= 0;
	virtual OBJID	GetMapID			()								= 0;
	virtual DWORD	GetLev				()								= 0;
	virtual __int64	GetExp				()								= 0;
	virtual int		GetBigPrizeFails	()								= 0;
	virtual void	SetBigPrizeFails	(int nData, bool bUpdate)		= 0;
	virtual int		GetSmallPrizeFails	()								= 0;
	virtual void	SetSmallPrizeFails	(int nData, bool bUpdate)		= 0;
	virtual ILotteryPrize*	AwardItem	(int nItemType)					= 0;
	virtual bool	SendSysMsg			(const char* fmt, ...)			= 0;
	virtual bool	SendMsg				(PLUGIN_MSG* pMsg)				= 0;
	virtual bool	BroadcastMsg		(const char* fmt, ...)			= 0;
	virtual bool	BroadcastRoomMsg	(PLUGIN_MSG* pMsg)				= 0;
protected:
	virtual ~ILotteryActor()			{}
};

class ILotteryPrize
{
public:
	virtual OBJID	GetID				()								= 0;
	virtual bool	IsEquip				()								= 0;
	virtual bool	SetInt				(int nIdx, int nData)			= 0;
	virtual bool	GetInfo				(PLUGIN_ITEMINFO* pInfo)		= 0;
	virtual USHORT	Release				()								= 0;
protected:
	virtual ~ILotteryPrize()			{}
};

class ILotteryMgr
{
public:
	virtual bool	Lottery				(int nLotteryType, int nColor, ILotteryActor* pActor, bool bTest)	= 0;
};

#endif
