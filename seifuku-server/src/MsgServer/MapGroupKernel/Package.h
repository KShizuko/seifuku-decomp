#if !defined(AFX_PACKAGE_H__DBC395BA_5C4A_40B5_944A_329D547BFBE5__INCLUDED_)
#define AFX_PACKAGE_H__DBC395BA_5C4A_40B5_944A_329D547BFBE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Item.h"
#include "GameObjSet.h"
#include "Myheap.h"
#include <vector>
enum {
		_STORE_NONE				=0,
		_STORE_NPCITEM			=ITEMPOSITION_STORAGE,
		_STORE_TRUNKITEM		=ITEMPOSITION_TRUNK,
		_STORE_USERPACK_ITEM	=ITEMPOSITION_CHEST,
};
enum{
	GOODS,
	MONEY,
};
const int	PACKAGE_LIMIT			= 50;
typedef	IGameObjSet<CItem>	IItemSet;
typedef	CGameObjSet<CItem>	CItemSet;

class CPackage
{
protected:
	CPackage();
	virtual ~CPackage();
public:
	static CPackage* CreateNew()							{ return new CPackage; }
	ULONG	Release()										{ delete this; return 0; }

public:
	int GetItemType(int index);
    bool	Create(int nType, OBJID idOwner, OBJID idPlayer, IDatabase* pDb);
	bool	CheckIn(CUser* pUser, CItem* pItem, bool bUpdate = true);
	CItem*	CheckOut(CUser* pUser, OBJID idItem, bool bUpdate);
	bool	SendInfo(CUser* pUser);
	bool	IsPlayerPackage()								{ return m_nType == ITEMPOSITION_STORAGE || m_nType == ITEMPOSITION_CHEST; }
	int		GetAmount()										{ return m_setItem->GetAmount(); }
	CItem*	GetItemByID(OBJID idItem);
protected:
	IItemSet*	m_setItem;
	int			m_nType;
	OBJID		m_idOwner;
	OBJID		m_idPlayer;
protected:
	MYHEAP_DECLARATION(s_heap)
};

#endif
