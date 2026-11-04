#if !defined(AFX_2DMAPITEM_H__BEB266A1_58BE_4541_8B61_DC18A5BF9C9A__INCLUDED_)
#define AFX_2DMAPITEM_H__BEB266A1_58BE_4541_8B61_DC18A5BF9C9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "2DMapObj.h"
#include "AniEffect.h"
#include "Game3DEffectEx.h"

class C2DMapItem  : public C2DMapObj
{
	friend struct MapItem2DProbe;
public:
	C2DMapItem(OBJID idItemType);
	C2DMapItem() {}
	virtual ~C2DMapItem();

	OBJID	GetItemType() {return m_idItemType;}
private:
	DWORD	m_dwBegin;
	DWORD	m_dwFrame;
	OBJID	m_idItemType;
	CMyPos	m_posWorld;
	BOOL	m_bShowName;
	CAniEffect m_objAniEffect;
	IGame3DEffect* m_pEffectEx;

public:
	void Show(void* pInfo);
	void Process(void* pInfo);
	int	 GetObjType(){return MAP_2DITEM;}
	void BeShowName(BOOL bShow){m_bShowName = bShow;}

public:
	static C2DMapItem* CreateNew(CMyPos posCell, OBJID idItemType);
};

#endif
