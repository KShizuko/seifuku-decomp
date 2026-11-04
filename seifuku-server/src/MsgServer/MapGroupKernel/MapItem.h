#if !defined(AFX_MAPITEM_H__9FB2A993_BFD6_47A9_B21A_4378262CCC0B__INCLUDED_)
#define AFX_MAPITEM_H__9FB2A993_BFD6_47A9_B21A_4378262CCC0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Gameobj.h"
#include "ItemType.h"
#include "GameBlock.h"
#include "Timeout.h"
#include "Myheap.h"
#include "Item.h"

const int	MAPITEM_ONTIMER_SECS		= 5;
const int	MAPITEM_MONSTER_ALIVESECS	= 3*60;
const int	MAPITEM_USERMAX_ALIVESECS	= 3*60;
const int	MAPITEM_USERMIN_ALIVESECS	= 1*60;
const int	MAPITEM_ALIVESECS_PERPRICE	= 1000/(MAPITEM_USERMAX_ALIVESECS-MAPITEM_USERMIN_ALIVESECS);
const int	MAPITEM_PRIV_SECS			= 30;
const int	DROPMONEY_LIMIT				= 10000000;
const int	PICKMAPITEMDIST_LIMIT		= 0;

class CMapItem : public CGameObj, IMapThing
{
protected:
	CMapItem();
	virtual ~CMapItem();

public:
	static CMapItem* CreateNew()			{ return new CMapItem; }
	void	ReleaseByOwner()				{ delete this; }
	bool	Create(OBJID idNewMapItem, CGameMap* pMap, POINT pos, OBJID idType, OBJID idOwner);
	bool	Create(OBJID idNewMapItem, CGameMap* pMap, POINT pos, ItemInfoStruct* pInfo, OBJID idOwner = ID_NONE);
	bool	CreateMoney(OBJID idNewMapItem, CGameMap* pMap, POINT pos, int nMoney, OBJID idOwner = ID_NONE);

public:
	virtual IMapThing*	QueryMapThing()		{ return (IMapThing*)this; }

public:
	virtual OBJID	GetID()									{ return m_id; }
	virtual int 	GetObjType()							{ return CGameObj::GetObjType(); }
	virtual int		GetPosX()								{ return m_nPosX; }
	virtual int		GetPosY()								{ return m_nPosY; }
	virtual CGameMap*	GetMap()								{ CHECKF(m_pMap); return m_pMap; }
	virtual void	SendShow(IRole* pRole);
protected:
	virtual bool	QueryObj(OBJID idObjType, void** ppv)			{ if(idObjType==OBJ_MAPITEM) return *ppv=this,true; return *ppv=NULL,false; }
	virtual bool	QueryRole(void** ppv)					{ return *ppv=NULL,false; }

public:
	OBJID	GetType()										{ return m_Info.idType; }
	int		GetAmount()										{ return m_Info.nAmount; }
	OBJID	GetPlayerID()									{ return m_Info.idPlayer; }
	ItemInfoStruct*	GetInfo()								{ return &m_Info; }
	bool	IsPriv()										{ return !m_tPriv.IsTimeOut(); }
	bool	IsAlive()										{ return !m_tAlive.IsTimeOut(); }
	bool	IsMoney()										{ return GetType() >= ITEMTYPEMONEY_MIN && GetType() <= ITEMTYPEMONEY_MAX; }
	bool	IsGem()											{ return CItem::IsGhostGem(GetType());}
	void	BroadcastShow();
	int		GetWeight()										{ return CItem::GetWeight(GetType(), m_Info.nAmount); }
	int		GetColor()										{ return m_nColor; }

protected:
	OBJID		m_id;
	ItemInfoStruct	m_Info;
	int			m_nPosX;
	int			m_nPosY;
	int			m_nColor;
	CTimeOut	m_tPriv;
	CTimeOut	m_tAlive;
	CGameMap*	m_pMap;

	MYHEAP_DECLARATION(s_heap)
};

#endif
