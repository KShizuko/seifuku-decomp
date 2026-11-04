#include "AllMsg.h"
#include "MapItem.h"
#include "MapGroup.h"
#include "ItemData.h"

MYHEAP_IMPLEMENTATION(CMapItem,s_heap)
CMapItem::CMapItem()
{
	SetObjType(OBJ_MAPITEM);
	memset(&m_Info, 0, sizeof(m_Info));

	m_pMap = NULL;
}

CMapItem::~CMapItem()
{
	if (m_pMap)
	{
		m_pMap->LeaveRoom(QueryMapThing());

		CMsgMapItem	msg;
		if(msg.Create(MSGMAPITEM_DELETE, this))
			m_pMap->BroadcastBlockMsg(QueryMapThing(), &msg);
	}
}

bool CMapItem::Create(OBJID idNewMapItem, CGameMap* pMap, POINT pos, OBJID idType, OBJID idOwner)
{
	CHECKF(pMap);
	CHECKF(idType != ID_NONE);

	CItemTypeData* pType = ItemType()->QueryItemType(idType);
	if(!pType)
		return false;

	m_tAlive.SetInterval(MAPITEM_MONSTER_ALIVESECS);
	m_tAlive.Update();
	m_pMap		= pMap;
	m_nPosX		= pos.x;
	m_nPosY		= pos.y;
	m_nColor	= 3;

	memset(&m_Info, 0, sizeof(m_Info));
	m_id			= idNewMapItem;
	m_Info.id			= ID_NONE;
	m_Info.idType		= pType->GetKey();
	m_Info.idOwner		= ID_NONE;
	m_Info.idPlayer		= idOwner;
	m_Info.nAmount		= pType->GetInt(ITEMTYPEDATA_AMOUNT);
	m_Info.nAmountLimit		= pType->GetInt(ITEMTYPEDATA_AMOUNT_LIMIT);
	m_Info.nIdent		= pType->GetInt(ITEMTYPEDATA_IDENT);
	m_Info.nPosition	= ITEMPOSITION_GROUND;
	m_Info.nData	= 0;

	m_Info.nGem1		= pType->GetInt(ITEMTYPEDATA_GEM1);
	m_Info.nGem2		= pType->GetInt(ITEMTYPEDATA_GEM2);
	m_Info.nMagic1		= pType->GetInt(ITEMTYPEDATA_MAGIC1);
	m_Info.nMagic2		= pType->GetInt(ITEMTYPEDATA_MAGIC2);
	m_Info.nMagic3		= pType->GetInt(ITEMTYPEDATA_MAGIC3);

	if(idOwner != ID_NONE)
	{
		m_tPriv.SetInterval(MAPITEM_PRIV_SECS);
		m_tPriv.Update();
	}

	m_pMap->EnterRoom(QueryMapThing());

	BroadcastShow();

	return true;
}

bool CMapItem::Create(OBJID idNewMapItem, CGameMap* pMap, POINT pos, ItemInfoStruct* pInfo, OBJID idOwner)
{
	CHECKF(pMap);
	CHECKF(pInfo);

	CItemTypeData* pType = ItemType()->QueryItemType(pInfo->idType);
	int nAliveSecs = MAPITEM_USERMAX_ALIVESECS;
	if(pType)
		nAliveSecs = pType->GetInt(ITEMTYPEDATA_PRICE)/MAPITEM_ALIVESECS_PERPRICE + MAPITEM_USERMIN_ALIVESECS;
	if(nAliveSecs > MAPITEM_USERMAX_ALIVESECS)
		nAliveSecs = MAPITEM_USERMAX_ALIVESECS;

	m_tAlive.SetInterval(nAliveSecs);
	m_tAlive.Update();
	m_pMap		= pMap;
	m_nPosX		= pos.x;
	m_nPosY		= pos.y;
	m_nColor	= pInfo->nColor;

	memcpy(&m_Info, pInfo, sizeof(m_Info));
	m_id			= idNewMapItem;
	m_Info.idOwner		= ID_NONE;
	m_Info.idPlayer		= idOwner;
	m_Info.nPosition	= ITEMPOSITION_GROUND;

	if(idOwner != ID_NONE)
	{
		m_tPriv.SetInterval(MAPITEM_PRIV_SECS);
		m_tPriv.Update();
	}

	m_pMap->EnterRoom(QueryMapThing());

	BroadcastShow();

	return true;
}

bool CMapItem::CreateMoney(OBJID idNewMapItem, CGameMap* pMap, POINT pos, int nMoney, OBJID idOwner )
{
	CHECKF(pMap);
	CHECKF(idNewMapItem);
	if(nMoney <= 0 || nMoney > DROPMONEY_LIMIT)
		return false;

	int nAliveSecs = MAPITEM_MONSTER_ALIVESECS;
	if(idOwner == ID_NONE)
	{
		nAliveSecs = nMoney/MAPITEM_ALIVESECS_PERPRICE + MAPITEM_USERMIN_ALIVESECS;
		if(nAliveSecs > MAPITEM_USERMAX_ALIVESECS)
			nAliveSecs = MAPITEM_USERMAX_ALIVESECS;
	}

	m_tAlive.SetInterval(nAliveSecs);
	m_tAlive.Update();
	m_pMap		= pMap;
	m_nPosX		= pos.x;
	m_nPosY		= pos.y;

	OBJID idType = ID_NONE;
	if(nMoney <= SMALLHEAPMONEY_LIMIT)
		idType	= SMALLHEAPMONEY_TYPE;
	else if(nMoney <= MIDDLEHEAPMONEY_LIMIT)
		idType	= MIDDLEHEAPMONEY_TYPE;
	else if(nMoney <= BIGHEAPMONEY_LIMIT)
		idType	= BIGHEAPMONEY_TYPE;
	else if(nMoney <= SMALLHEAPGOLD_LIMIT)
		idType	= SMALLHEAPGOLD_TYPE;
	else if(nMoney <= MIDDLEHEAPGOLD_LIMIT)
		idType	= MIDDLEHEAPGOLD_TYPE;
	else
		idType	= BIGHEAPGOLD_TYPE;

	memset(&m_Info, 0, sizeof(m_Info));
	m_id			= idNewMapItem;
	m_Info.id			= ID_NONE;
	m_Info.idType		= idType;
	m_Info.idOwner		= ID_NONE;
	m_Info.idPlayer		= idOwner;
	m_Info.nAmount		= nMoney;
	m_Info.nAmountLimit		= DROPMONEY_LIMIT;
	m_Info.nIdent		= 0;
	m_Info.nPosition	= ITEMPOSITION_GROUND;
	m_Info.nData		= 0;
	m_Info.nGem1		= 0;
	m_Info.nGem2		= 0;
	m_Info.nMagic1		= 0;
	m_Info.nMagic2		= 0;
	m_Info.nMagic3		= 0;

	if(idOwner != ID_NONE)
	{
		m_tPriv.SetInterval(MAPITEM_PRIV_SECS);
		m_tPriv.Update();
	}

	m_pMap->EnterRoom(QueryMapThing());

	BroadcastShow();

	return true;
}

void CMapItem::SendShow(IRole* pRole)
{
	CHECK(m_pMap);

	CMsgMapItem	msg;
	if(msg.Create(MSGMAPITEM_CREATE, this))
		pRole->SendMsg(&msg);
}

void CMapItem::BroadcastShow()
{
	CHECK(m_pMap);

	CMsgMapItem	msg;
	if(msg.Create(MSGMAPITEM_CREATE, this))
		m_pMap->BroadcastBlockMsg(QueryMapThing(), &msg);
}
