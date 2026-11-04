#if !defined(AFX_GAMEMAP_H__C01C6BBF_DE8E_486F_B369_B6A1E735FF80__INCLUDED_)
#define AFX_GAMEMAP_H__C01C6BBF_DE8E_486F_B369_B6A1E735FF80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "define.h"
#include "GameBlock.h"
#include "GameObj.h"
#include "GameData.h"
#include "I_MapData.h"
#include "Myheap.h"
#include "GameData.h"
#include "EventPack.h"
#include "T_SingleObjSet2.h"
#include <vector>
using namespace std;
inline int	Block(int nPos)				{ return nPos / CELLS_PER_BLOCK; }

inline int POS2INDEX(int x, int y, int cx, int cy) { return (x + y*cx); }
inline int INDEX2X(int idx, int cx, int cy) { return (idx % cy); }
inline int INDEX2Y(int idx, int cx, int cy) { return (idx / cy); }

enum GAMEMAPDATA{
	GAMEMAPDATA_NAME=1,
	GAMEMAPDATA_DESCRIBE,
	GAMEMAPDATA_MAPDOC,
	GAMEMAPDATA_TYPE,
	GAMEMAPDATA_OWNERID_,
	GAMEMAPDATA_MAPGROUP,
	GAMEMAPDATA_IDXSERVER,
	GAMEMAPDATA_WEATHER,
	GAMEMAPDATA_BGMUSIC,
	GAMEMAPDATA_BGMUDIC_SHOW,
	GAMEMAPDATA_PORTAL0_X,
	GAMEMAPDATA_PORTAL0_Y,
	GAMEMAPDATA_REBORN_MAPID,
	GAMEMAPDATA_REBORN_PORTAL,
	GAMEMAPDATA_RESOURCE_LEV,
	GAMEMAPDATA_OWNERTYPE,
	GAMEMAPDATA_LINKMAP,
	GAMEMAPDATA_LINK_X,
	GAMEMAPDATA_LINK_Y,
	GAMEMAPDATA_DELFLAG,
	GAMEMAPDATA_COLOR,
};
extern char	szMapTable[];
typedef	CGameData<GAMEMAPDATA,szMapTable,szID>	CGameMapData;

enum REGIONDATA{
	REGIONDATA_MAPID=1,
	REGIONDATA_TYPE,
	REGIONDATA_BOUND_X,
	REGIONDATA_BOUND_Y,
	REGIONDATA_BOUND_CX,
	REGIONDATA_BOUND_CY,
	REGIONDATA_DATASTR,
	REGIONDATA_DATA0,
	REGIONDATA_DATA1,
	REGIONDATA_DATA2,
	REGIONDATA_DATA3,
};
extern char	szRegionTable[];
typedef	CGameData<REGIONDATA, szRegionTable, szID>	CRegionData;
typedef	ISingleObjSet2<CRegionData>					IRegionSet;
typedef	CSingleObjSet2<CRegionData>					CRegionSet;
const int	REGIONDATA_SIZE		= 4;

enum ENUM_MAPTYPE {
	MAPTYPE_NORMAL				= 0x0000,
	MAPTYPE_PKFIELD				= 0x0001,
	MAPTYPE_CHGMAP_DISABLE		= 0x0002,
	MAPTYPE_RECORD_DISABLE		= 0x0004,
	MAPTYPE_PK_DISABLE			= 0x0008,
	MAPTYPE_BOOTH_ENABLE		= 0x0010,
	MAPTYPE_TEAM_DISABLE		= 0x0020,
	MAPTYPE_TELEPORT_DISABLE	= 0x0040,
	MAPTYPE_SYN_MAP				= 0x0080,
	MAPTYPE_PRISON_MAP			= 0x0100,
	MAPTYPE_WING_DISABLE		= 0x0200,
	MAPTYPE_FAMILY				= 0x0400,
	MAPTYPE_MINEFIELD			= 0x0800,
	MAPTYPE_PKGAME				= 0x1000,
	MAPTYPE_NEVERWOUND			= 0x2000,
	MAPTYPE_DEADISLAND			= 0x4000,
};

enum REGION_TYPE {
	REGION_NONE					= 0,
	REGION_CITY					= 1,
	REGION_WEATHER				= 2,
	REGION_STATUARY				= 3,
	REGION_DESC					= 4,
	REGION_GOBALDESC			= 5,
	REGION_DANCE				= 6,
	REGION_PK_PROTECTED			= 7,
};
CONST OBJID MAP_PRISON			= 6000;
const int	DYNAMIC_MAP_ID		= 1000000;
const int	WHITE_SYN_MAP_ID	= 1038;
const int	BLACK_SYN_MAP_ID	= 1000;
const int	NEWBIE_MAP_ID		= 10100;

enum { RPC_MAP_DELNPC };

struct	NewMapInfo{
		NAMESTR		szName;
		int		nMapDoc;
		int		nType;
		int		nOwnerType;
		OBJID	idOwner;
		int		nMapGroup;
		OBJID	idLikeMap;
		int		nLinkX;
		int		nLinkY;
		int		nPortalX;
		int		nPortalY;
		OBJID	idRebornMap;
		int		nRebornPortal;
		int		nResLev;
};

#include "WeatherRegion.h"
const bool	WITH_BLOCK		= true;
typedef	vector<IMapThing*>	MAPTHING_SET;
class CGameMap : public CGameObj
{
protected:
	CGameMap();
	virtual ~CGameMap();
public:
	static CGameMap*	CreateNew()	{ return new CGameMap(); }
	ULONG	ReleaseByOwner()		{ delete this; return 0; }

public:
	OBJID	GetID() { return m_pData->GetKey(); }
	bool	Create(PROCESS_ID idProcess, IRecordset* pRes);
	OBJID	CreateDynaMap(PROCESS_ID idProcess, const NewMapInfo* pInfo);
	void	OnTimer(DWORD tCurr);
	bool	QueryObjInPos(int nPosX, int nPosY, OBJID idObjType, void** ppObj);
	bool	QueryObj(int nCenterX, int nCenterY, OBJID idObjType, OBJID idObj, void** ppObj);
	IRole*	QueryRole(int nCenterX, int nCenterY, OBJID idObj);
	CWeatherRegion*	QueryWeatherRegion(OBJID id)		{ CHECKF(m_setWeather && id != ID_NONE); return m_setWeather->GetObj(id);}

public:
	CGameBlock&	QueryBlock(int nPosX, int nPosY)		{ return m_setBlock[Block(nPosX)][Block(nPosY)]; }
	IMapThing*	QueryThingByIndex(int x, int y, int z)	{ return BlockByIndex(x,y).QuerySet()->GetObjByIndex(z); }
	CGameBlock&	BlockByIndex(int x, int y)				{ return m_setBlock[x][y]; }
protected:
	IRegionSet*	QueryRegionSet()						{ CHECKF(m_setRegion); return m_setRegion; }

public:
	virtual void	SendBlockInfo(IRole* pRole);
	virtual void	BroadcastBlockMsg(IMapThing* pThing, CNetMsg* pMsg, bool bSendSelf = false);
	virtual void	BroadcastBlockMsg(int nPosX, int nPosY, CNetMsg* pMsg);

public:
	void	EnterRoom(IMapThing* pThing, BOOL bWithBlock=false);
	void	LeaveRoom(IMapThing* pThing, BOOL bWithBlock=false);
	void	MoveTo(IRole* pRole, int nNewPosX, int nNewPosY);
	void	IncRole(int x, int y)		{ m_pMapData->IncRole(x, y); }
	void	DecRole(int x, int y)		{ m_pMapData->DecRole(x, y); }

public:
	void	SendRegionInfo(CUser* pUser);
	void	ClearRegionInfo(CUser* pUser);
	void	ChangeRegion(CUser* pUser, int nNewPosX, int nNewPosY);

public:
	CRegionData*	QueryRegion(int nRegionType, int x, int y);
	bool	IsInRegionType(int nRegionType, int x, int y);
	static bool	IsInRegion(CRegionData* pData, OBJID idMap, int x, int y);

public:
	DWORD	GetLight()						{ return m_dwLightRGB; }
	void	SetLight(DWORD dwRGB)			{ m_dwLightRGB = dwRGB; }
protected:
	DWORD	m_dwLightRGB;

public:
	int		GetOwnerID()					{ return m_pData->GetInt(GAMEMAPDATA_OWNERID_); }
	int		GetOwnerType()					{ return m_pData->GetInt(GAMEMAPDATA_OWNERTYPE); }
	int		GetWidth()						{ return m_pMapData->GetMapWidth(); }
	int		GetHeight()						{ return m_pMapData->GetMapHeight(); }
	int		GetWidthOfBlock()				{ return (m_pMapData->GetMapWidth()-1) / CELLS_PER_BLOCK + 1; }
	int		GetHeightOfBlock()				{ return (m_pMapData->GetMapHeight()-1) / CELLS_PER_BLOCK + 1; }
	int		GetFloorAttr(int x, int y)		{ return m_pMapData->GetFloorAttr(x, y); }
	int		GetFloorAlt(int x, int y)		{ return m_pMapData->GetFloorAlt(x, y); }
	int		GetResLev()						{ return m_pData->GetInt(GAMEMAPDATA_RESOURCE_LEV); }
	OBJID	GetSynID()						{ if(GetOwnerType() == OWNER_SYN) return GetOwnerID(); return ID_NONE; }
	DWORD	GetStatus()						{ return m_nStatus; }
	DWORD	GetType()						{ return m_pData->GetInt(GAMEMAPDATA_TYPE); }
	POINT	GetPortal()						{ POINT pos; pos.x=m_pData->GetInt(GAMEMAPDATA_PORTAL0_X); pos.y=m_pData->GetInt(GAMEMAPDATA_PORTAL0_Y); return pos;}
	OBJID	GetDocID()						{ return m_pData->GetInt(GAMEMAPDATA_MAPDOC); }
	void	SendMapInfo(CUser* pUser);
	const char* GetName()					{ return m_pData->GetStr(GAMEMAPDATA_NAME); }

public:
	bool	IsStandEnable(int nPosX, int nPosY);
	bool	IsMoveEnable(int x, int y);
	bool	IsAltEnable(const POINT& posSour, const POINT& pos, int nAltDiff);
	bool	IsAltOver(const POINT& pos, int nAlt);
	bool	IsLayItemEnable(int x, int y);
	int		Distance(int x1, int y1, int x2, int y2)	{ return __max(abs(x1-x2), abs(y1-y2)); }
	bool	IsValidPoint(POINT pos)			{ return IsValidPoint(pos.x, pos.y); }
	bool	IsValidPoint(int x, int y)		{ return (x >=0 && x < GetWidth() && y >= 0 && y < GetHeight()); }
	bool	IsPkField(void)					{ return ((m_pData->GetInt(GAMEMAPDATA_TYPE)&MAPTYPE_PKFIELD) != 0); }
	bool	IsFlyToDisable(void)			{ return ((m_pData->GetInt(GAMEMAPDATA_TYPE)&MAPTYPE_CHGMAP_DISABLE) != 0); }
	bool	IsNewbieMap()					{ return m_bNewbieMap; }
	bool	IsRecordDisable(void)			{ return ((m_pData->GetInt(GAMEMAPDATA_TYPE)&MAPTYPE_RECORD_DISABLE) != 0); }
	bool	IsPkDisable(void)				{ return ((m_pData->GetInt(GAMEMAPDATA_TYPE)&MAPTYPE_PK_DISABLE) != 0); }
	bool	IsWarTime()						{ return (GetStatus() & STATUS_WAR) != 0; }
	bool	IsTeamDisable()					{ return (m_pData->GetInt(GAMEMAPDATA_TYPE)&MAPTYPE_TEAM_DISABLE) != 0; }
	bool	IsTeleportDisable()				{ return (m_pData->GetInt(GAMEMAPDATA_TYPE)&MAPTYPE_TELEPORT_DISABLE) != 0; }
	bool	IsSynMap()						{ return (m_pData->GetInt(GAMEMAPDATA_TYPE)&MAPTYPE_SYN_MAP) != 0; }
	bool	IsPrisonMap()					{ return (m_pData->GetInt(GAMEMAPDATA_TYPE)&MAPTYPE_PRISON_MAP) != 0; }
	bool	IsTrainMap()					{ return GetID() == 1039; }
	bool	IsDynaMap()						{ return GetID() > DYNAMIC_MAP_ID; }
	bool	IsWingEnable()					{ return (m_pData->GetInt(GAMEMAPDATA_TYPE)&MAPTYPE_WING_DISABLE) == 0; }
	bool	IsMineField()					{ return (m_pData->GetInt(GAMEMAPDATA_TYPE)&MAPTYPE_MINEFIELD) != 0; }
	bool	IsPkGameMap()					{ return (m_pData->GetInt(GAMEMAPDATA_TYPE)&MAPTYPE_PKGAME) != 0; }
	bool	IsFamilyMap()					{ return (m_pData->GetInt(GAMEMAPDATA_TYPE)&MAPTYPE_FAMILY) != 0; }
	bool	IsNeverWoundMap()				{ return (m_pData->GetInt(GAMEMAPDATA_TYPE)&MAPTYPE_NEVERWOUND) != 0;}
	bool	IsDeadIsland()					{ return (m_pData->GetInt(GAMEMAPDATA_TYPE)&MAPTYPE_DEADISLAND) != 0;}
	bool	IsBoothEnable()					{ return (m_pData->GetInt(GAMEMAPDATA_TYPE)&MAPTYPE_BOOTH_ENABLE) != 0;}
public:
	void	CollectMapThing(MAPTHING_SET& psetMapThing, const POINT pos, int nRange, OBJID idObjTypeUnion);
	bool	FindDropItemCell(int nRange, POINT* pos);
	bool	GetPassageMap(OBJID* pidMap, POINT* pposTarget, const POINT& pos);
	bool	GetRebornMap(OBJID* pidMap, POINT* pposTarget);
	bool	SetStatus(int nStatus, bool flag);
	void	SetSynID(OBJID idSyn, bool bWithAllNpc);
	CNpc*	QueryNpcByType(int nPosX, int nPosY, int nType, int nRange);
	CNpc*	QueryNpc(int nPosX, int nPosY, OBJID idNpc, int nRange);
	bool	ChangeMapDoc(OBJID idDoc);
	bool	EraseMap();
	void	SetResLev(int nData, bool bUpdate)			{ m_pData->SetInt(GAMEMAPDATA_RESOURCE_LEV, nData); if(bUpdate) m_pData->Update(); }
	void	SetPortal0X(int nData, bool bUpdate)		{ m_pData->SetInt(GAMEMAPDATA_PORTAL0_X, nData); if(bUpdate) m_pData->Update(); }
	void	SetPortal0Y(int nData, bool bUpdate)		{ m_pData->SetInt(GAMEMAPDATA_PORTAL0_Y, nData); if(bUpdate) m_pData->Update(); }
	bool	AddTerrainObj(OBJID idOwner, int x, int y, OBJID idTerrainObj);
	bool	DelTerrainObj(OBJID idOwner);

public:
	bool	GetDebugData(int* pAttr, int* pMask, int* pAlt, int* pAlt2, int x, int y)
			{ if(!IsValidPoint(x,y)) return false;
				*pAttr=m_pMapData->GetFloorAttr(x,y);
				*pMask=m_pMapData->GetFloorMask(x,y);
				*pAlt=m_pMapData->GetFloorAlt(x,y);
				*pAlt2=m_pMapData->GetSurfaceAlt(x,y);
				return true; }

protected:
	typedef	vector<CGameBlock>	BLOCK_SET;
	typedef	vector<BLOCK_SET>	BLOCKS_SET;
	BLOCKS_SET		m_setBlock;

protected:
	IRegionSet*		m_setRegion;

protected:
	IWeatherSet*	m_setWeather;

protected:
	CGameMapData*	m_pData;
	IMapData*		m_pMapData;

protected:
	PROCESS_ID		m_idProcess;
	MYHEAP_DECLARATION(s_heap)

public:
	void ProcessRpc(CEventPack& pack);
	void DelNpcByType(int nType);
	enum { 
			STATUS_NONE=0, 
			STATUS_WAR=1,
	};
protected:
	DWORD			m_nStatus;
public:
	bool			m_bNewbieMap;
};

#define	FOR_9_BLOCKS(P,X,Y)							\
	for(x = __max(Block(X)-1,0); x <= Block(X)+1 && x < P->GetWidthOfBlock(); x++)		\
		for(y = __max(Block(Y)-1,0); y <= Block(Y)+1 && y < P->GetHeightOfBlock(); y++)
#define FOR_9_BLOCKTHINGS(P,X,Y)	\
	FOR_9_BLOCKS((P),(X),(Y))		\
		for(z = 0; z < P->BlockByIndex(x,y).QuerySet()->GetAmount(); z++)

#endif
