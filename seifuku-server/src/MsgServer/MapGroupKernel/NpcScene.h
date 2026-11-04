#if !defined(AFX_NPCSCENE_H__63C2DB1A_BDEF_4A3A_8379_B30F383C77D0__INCLUDED_)
#define AFX_NPCSCENE_H__63C2DB1A_BDEF_4A3A_8379_B30F383C77D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class CNpcScene  
{
protected:
	CNpcScene();
	virtual ~CNpcScene();

public:
	static CNpcScene* CreateNew()						{ return new CNpcScene; }
	ULONG	Release()									{ delete this; return 0; }

public:
	bool	Create(CNpc* pOwner, CGameMap* pMap);
	void	DelTerrainObj();
	void	ChangeTerrainObj(int nLook);

protected:
	CNpc*		m_pOwner;
	CGameMap*	m_pMap;
	auto_long	m_bTerrainObj;

	MYHEAP_DECLARATION(s_heap)
};

#endif
