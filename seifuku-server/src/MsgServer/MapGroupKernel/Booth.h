#if !defined(AFX_BOOTH_H__8DDAB97F_6B97_43F3_9EA8_8DC3E505968B__INCLUDED_)
#define AFX_BOOTH_H__8DDAB97F_6B97_43F3_9EA8_8DC3E505968B__INCLUDED_

#include "I_Role.h"
#include "NetworkDef.h"
#include <vector>
using namespace std;

#if _MSC_VER > 1000
#pragma once
#endif

class CGameMap;
class CUser;
class IDatabase;
class CBooth : IMapThing
{
protected:
	CBooth();
	virtual ~CBooth();
public:
	static CBooth*	CreateNew(PROCESS_ID idProcess, CUser* pUser, int nPosX, int nPosY, int nDir);
	static bool		InitSysBuyInfo(IDatabase* pDb);
protected:
	bool	Create(PROCESS_ID idProcess, CUser* pUser, int nPosX, int nPosY, int nDir);
	IMapThing*	QueryMapThing()									{ return (IMapThing*)this; }

public:
	bool	AddItem(OBJID idItem, int nMoney, int nEmoney);
	bool 	DelItem(OBJID idItem);
	bool 	GetCost(OBJID idItem, int* pnMoney, int* pnEmoney);
	bool	BuyItem(CUser* pTarget, OBJID idItem);
	void	SendGoods(CUser* pTarget);
	void	EnterMap(int nPosX, int nPosY, int nDir);
	void	LeaveMap();
	void	SetCryOut(LPCTSTR szWords)							{ SafeCopy(m_szCryOut, szWords, _MAX_WORDSSIZE); }
	void	SendCryOut(CUser* pUser);
	void	OnTimer();

public:
	virtual void	ReleaseByOwner()							{ delete this; }
	virtual OBJID	GetID()										{ return m_id; }
	virtual int 	GetObjType()								{ return OBJ_BOOTH; }
	virtual int		GetPosX()									{ return m_nPosX; }
	virtual int		GetPosY()									{ return m_nPosY; }
	virtual void	SendShow(IRole* pRole);

	virtual bool	QueryObj(OBJID idObjType, void** ppv)		{ *ppv = NULL; if(idObjType == OBJ_BOOTH) return *ppv=this,true; return false; }
	virtual bool	QueryRole(void** ppv)						{ *ppv = NULL; return false; }

protected:
	struct	GOODS_ST{
		OBJID	idItem;
		int		nType;
		int		nMoney;
		int		nEmoney;
	};
	typedef	vector<GOODS_ST>	GOODS_SET;
	struct	SYSBUY_ST{
		OBJID	idType;
		int		nInterval;
		DWORD	dwMinPrice;
		DWORD	dwMaxPrice;
	};
	typedef	vector<SYSBUY_ST>	SYSBUY_SET;
	static SYSBUY_SET	s_setSysBuy;
	vector<time_t>	m_setSysBuyTime;
	GOODS_SET		m_setGoods;

protected:
	CUser*			m_pUser;
	OBJID			m_id;
	CGameMap*		m_pMap;
	int				m_nPosX;
	int				m_nPosY;
	int				m_nDir;
	char			m_szCryOut[_MAX_WORDSSIZE];

protected:
	PROCESS_ID		m_idProcess;
	bool			m_bEnterMap;

protected:
	MYHEAP_DECLARATION(s_heap)
};

#endif
