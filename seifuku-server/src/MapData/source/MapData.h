#ifndef MAPDATA_HEAD
#define MAPDATA_HEAD
#include "I_MapData.h"
#include "Cell.h"
#include "TerrainObj.h"
#include "T_CompactObjSet.h"
#include <deque>
using namespace std;
enum{_ASTAR_NONE=0, _ASTAR_CLOSE, _ASTAR_LEAVE};
typedef struct
{
	int nPosX;
	int nPosY;
	int nIndex;
}PassageInfo;
struct  NodeInfo
{
	int nX;
	int nY;
	int nG;
	int nH;
	int nStep;
	int nDir;
	NodeInfo* pNode;
	NodeInfo* pChildNode;
};
#include <vector>
using namespace std;
typedef vector<CCell* >  DEQUE_CELL;
typedef vector<PassageInfo* >  DEQUE_PASSAGE;

inline int POS2INDEX(int x, int y, int cx, int cy) { return (x + y*cx); }
inline int INDEX2X(int idx, int cx, int cy) { return (idx % cy); }
inline int INDEX2Y(int idx, int cx, int cy) { return (idx / cy); }

class CTerrainObj;
class CMapData : public IMapData
{
public:
	CMapData();
	~CMapData();
	static CMapData* CreateNew(LPCTSTR pszFileName, LPCTSTR pszVersion, int nBlockSize);

protected:
	ULONG	Release()	{ delete this; return 0; }
protected:
	bool	AddTerrainItem(OBJID idOwner, int nCellX, int nCellY, OBJID idTerrainItemType);
	bool	DelTerrainItem(OBJID idOwner);
protected:
	int		GetMapWidth() const;
	int		GetMapHeight() const;
	CCell*	QueryCell(int nCellX, int nCellY);
	const CCell*	QueryCell(int nCellX, int nCellY) const;
protected:
	int		GetPassage(int x, int y) const;
	bool	FindPath(PASSPATH0* pPathBuf, int nBufSize, POINT posSource, POINT posTarget, IRoleAttr* pAttr, int nEscapeSteps = 0);
protected:
	virtual int		GetFloorAlt		(int x, int y) const;
	virtual int		GetFloorAttr	(int x, int y) const;
	virtual DWORD	GetFloorMask	(int x, int y) const;
	virtual int		GetSurfaceAlt	(int x, int y) const;
protected:
	virtual void	IncRole			(int x, int y, int nVal = 1);
	virtual void	DecRole			(int x, int y, int nVal = 1);
	virtual int		GetRoleAmount	(int x, int y) const;

private:
	void ClearNodeSet();
	NodeInfo* CreateNewNode();
	void ClearCloseSet()							{ m_nCloseNodeAmount = 0; }
	void ClearOpenSet()								{ m_setOpen.clear(); }
	void ClearPathSet()								{ m_setPath.clear(); }
	void FindPath(POINT posStart, POINT posTarget);
	void InitAStar();
	void GetPath(NodeInfo* pNode);
	int  GetPathAmount();
	NodeInfo* GetNodeByIndex(int nIndex);
	int GetHValue(POINT pos);
	BOOL AddSucNode(NodeInfo* pNode);
	NodeInfo* CheckBestNode();
	NodeInfo* CheckOpenList();
	BOOL CheckNode(POINT posNode, int nHight);
	int GetNodeSetAmount()							{ return m_setNode.size(); }
private:
	typedef deque<NodeInfo*>  DEQUE_NODE;
    DEQUE_NODE		m_setNode;
    int				m_nNodeIndex;
    DEQUE_NODE		m_setOpen;
    DEQUE_NODE		m_setClose;
    int				m_nCloseNodeAmount;
    DEQUE_NODE		m_setPath;
    POINT			m_posStart;
    POINT			m_posTarget;
	POINT			m_posRealTarget;
    DEQUE_CELL		m_setCellAStar;
	int				m_nPathMode;
	int				m_nLeaveDisc;

private:
	SIZE			m_sizeMap;
	typedef	CompactObjSet<CCell>	SELL_SET;
	SELL_SET		m_setCell;
	DEQUE_PASSAGE	m_setPassage;
	DEQUE_TERRAINOBJ	m_setMapObj;
	DEQUE2_LAYER	m_set2Layer;

protected:
	auto_long		m_bDDVersion;

public:

protected:
	void ClearCell();
	void ClearPassage();
	void ClearMapObj();
	void ClearLayerSet();
protected:
	void AddPassage(const PassageInfo* pInfo);
	void AddPassage(POINT posMap, int nIndex);
	CCell* GetCell(int nCellX, int nCellY);
	CCell* GetCell(int nIndex);
	const CCell* GetCell(int nCellX, int nCellY) const;
	bool LoadSurfaceCellData(FILE* fp);
	bool LoadDataPassage(FILE* fp);
	bool LoadTerrainItemData(FILE* fp);
	bool LoadPuzzle(char* pszFile, int nBlockSize);
	bool AddMapObj(CTerrainObj* pObj);
	bool DelMapObj(int idx);
private:
	bool PlaceTerrainObj(CTerrainObj* pTerrainObj);
	bool DisplaceTerrainObj(CTerrainObj* pTerrainObj);
};
#endif
