#if !defined(AFX_GAMEMAP_H__7EF1735C_1C98_4A7D_A00A_5A41F4DAC44B__INCLUDED_)
#define AFX_GAMEMAP_H__7EF1735C_1C98_4A7D_A00A_5A41F4DAC44B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "GameData.h"
#include "GameBlock.h"
#include "define.h"

#	define		MAPDATA_VERSION		(LPCTSTR)true
const DWORD	DEFAULT_LIGHTRGB		= 0xFFFFFFFF;

inline int	Block(int nPos)				{ return nPos / CELLS_PER_BLOCK; }

inline int POS2INDEX(int x, int y, int cx, int cy) { return (x + y*cx); }
inline int INDEX2X(int idx, int cx, int cy) { return (idx % cy); }
inline int INDEX2Y(int idx, int cx, int cy) { return (idx / cy); }
inline void POS2LT(POINT* pTarget, const POINT& pos, int nRange) { pTarget->x = pos.x - nRange; pTarget->y = pos.y - nRange; }
inline void POS_ADD(POINT* pTarget, int nOffsetX, int nOffsetY) { pTarget->x += nOffsetX; pTarget->y += nOffsetY; }
inline void POS_SUB(POINT* pTarget, int nOffsetX, int nOffsetY) { pTarget->x -= nOffsetX; pTarget->y -= nOffsetY; }

enum GAMEMAPDATA{

	GAMEMAPDATA_NAME=1,
	GAMEMAPDATA_DESCRIBE,
	GAMEMAPDATA_MAPDOC,
	GAMEMAPDATA_TYPE,
	GAMEMAPDATA_OWNERID,
	GAMEMAPDATA_MAPGROUPID,
	GAMEMAPDATA_IDXSERVER,
	GAMEMAPDATA_WEATHER,
	GAMEMAPDATA_BGMUSIC,
	GAMEMAPDATA_BGMUDIC_SHOW,
	GAMEMAPDATA_PORTAL0_X,
	GAMEMAPDATA_PORTAL0_Y,
	GAMEMAPDATA_REBORN_MAPID,
};

extern char	szMapTable[];
typedef	CGameData<GAMEMAPDATA,szMapTable,szID>	CGameMapData;

class CGameMap;
class IMapData;
class CGameMap : public CGameObj
{
public:
	CGameMap();
	virtual ~CGameMap();
	static CGameMap*	CreateNew()		{ return new CGameMap; }
	virtual bool	Create(IRecordset* pRes);
	virtual ULONG	ReleaseByOwner() { delete this; return 0; }
	IMapData*		QueryMapData()	 { MYASSERT(m_pMapData); return m_pMapData; }

public:
	OBJID	GetID()					{ return m_pData->GetKey(); }
	LPCTSTR	GetName()				{ return m_pData->GetStr(GAMEMAPDATA_NAME); }
	int		GetWidth()				{ return m_pMapData->GetMapWidth(); }
	int		GetHeight()				{ return m_pMapData->GetMapHeight(); }
	int		GetWidthOfBlock()		{ return (m_pMapData->GetMapWidth()-1) / CELLS_PER_BLOCK + 1; }
	int		GetHeightOfBlock()		{ return (m_pMapData->GetMapHeight()-1) / CELLS_PER_BLOCK + 1; }
	int		GetStatus()				{ return m_nStatus; }

public:
	bool	IsStandEnable(int nPosX, int nPosY);
	bool	IsMoveEnable(int x, int y, int nDir, int nSizeAdd, int nClimbCap = 0);
	bool	IsSuperposition(IRole* pRole);
	int		Distance(int x1, int y1, int x2, int y2)	{ return __max(abs(x1-x2), abs(y1-y2)); }
	bool	IsValidPoint(int x, int y)					{ return (x >=0 && x < GetWidth() && y >= 0 && y < GetHeight()); }
	bool	IsWarTime()									{ return m_nStatus == STATUS_WAR; }
	bool	IsJumpEnable(int x, int y)					{ return IsStandEnable(x, y) && m_pMapData->GetRoleAmount(x, y) == 0; }
	void	CollectRole(vector<IRole*>* psetRole, const POINT pos, int nRange);

public:
	void	SetStatus(int nStatus)	{ m_nStatus = nStatus; }
	bool	ChangeMapDoc(OBJID idDoc);
	void	AddTerrainObj(OBJID idOwner, int nPosX, int nPosY, OBJID idTerrainObj)	{ m_pMapData->AddTerrainItem(idOwner, nPosX, nPosY, idTerrainObj); }
	void	DelTerrainObj(OBJID idOwner)				{ m_pMapData->DelTerrainItem(idOwner); }

public:
	void	IncRole(int x, int y, int nSizeAdd = 0);
	void	DecRole(int x, int y, int nSizeAdd = 0);

public:
	bool		SetDormancy(int nPosX, int nPosY);

public:
	void		EnterMap(CUser* pUser);
	void		EnterMap(INpc* pNpc);
	void		MoveTo(CUser* pUser, int nNewPosX, int nNewPosY);
	void		MoveTo(INpc* pNpc, int nNewPosX, int nNewPosY);
	void		LeaveMap(CUser* pUser);
	void		LeaveMap(INpc* pNpc);

public:
	CGameBlock&	BlockSet(int nPosX, int nPosY)		{ return m_setBlock[Block(nPosX)][Block(nPosY)]; }
	CGameBlock&	BlockByIndex(int x, int y)		{ return m_setBlock[x][y]; }

protected:
	IMapData*		m_pMapData;
protected:
	typedef	vector<CGameBlock>		BOOL_SET;
	typedef	vector<BOOL_SET>		BOOLS_SET;
	BOOLS_SET	m_setBlock;

protected:
	CGameMapData*	m_pData;

public:
	enum	{ STATUS_NONE, STATUS_WAR, };
protected:
	void CallWakeMonster(int nPosX, int nPosY);
	int				m_nStatus;
};

#define	FOR_9_BLOCKS(P,X,Y)							\
	for(x = __max(Block(X)-1,0); x <= Block(X)+1 && x < P->GetWidthOfBlock(); x++)		\
		for(y = __max(Block(Y)-1,0); y <= Block(Y)+1 && y < P->GetHeightOfBlock(); y++)

#endif
