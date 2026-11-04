#ifndef _TERRAINLAYER_H
#define _TERRAINLAYER_H
#include "MapLayer.h"
#include "PuzzleBmp.h"
#include "TqTerrain.h"
#include "Layer.h"
#include "2DMapRegion.h"
#include <vector>
typedef struct
{
	int nPosX;
	int nPosY;
	int nIndex;
}PassageInfo;
#include <deque>
using namespace std;
typedef deque<PassageInfo* >  DEQUE_PASSAGE;
typedef deque<C2DMapRegion* > DEQUE_REGION;

typedef struct
{
	char	cOffsetX;
	char	cOffsetY;
	char	cIndex;
	char	cReserved[5];
}TraceJitter;
typedef struct
{
	short	nPosX;
	short	nPosY;
	DWORD	dwTime;
}TraceInfo;
typedef vector<TraceInfo> VECTOR_TRACE;

class CTerrainPuzzle
{
public:
	CTerrainPuzzle();
	virtual ~CTerrainPuzzle();

	BOOL	Load	(char* pszFile);
	void	Show	(int nPosX, int nPosY);
	int		GetWidth	(void);
	int		GetHeight	(void);
	void	SetGrideSize	(int nSize);
	int		GetGrideSize	(void);
	void	PreLoad	(int nPosX, int nPosY);
	void	Process	(void);
	void	SetPulGrideSize	(int nSize);
	void	AddBlastTrace	(int nPosX, int nPosY);
	void	AddCartridge	(int nPosX, int nPosY);
	void	SetTraceLife	(DWORD dwLife);

	static int	GetRandom	(int nRange);

private:
	void	Reset	(void);
	void	ShowBlastTrace	(int nPosX, int nPosY);
	void	ShowCartridge	(int nPosX, int nPosY);

private:
	BOOL			m_bTqTerrain;
	char			m_szFile[_MAX_PATH];
	CTqTerrain		m_objTerrain;
	CPuzzleBmp		m_bmpPuzzle;
	VECTOR_TRACE*	m_setCartridge;
	TraceJitter		m_setCartridgeJitter[1024];
	VECTOR_TRACE*	m_setBlastTrace;
	TraceJitter		m_setBlastJitter[1024];
	DWORD			m_dwTraceLife;
	IMultiBmp*		m_pMultiBmp;
	friend struct TerrainLayerProbe;
};

class CTerrainLayer : public CMapLayer
{
public:
	CTerrainLayer();
	~CTerrainLayer();

private:
	char m_szPuzzleFile[_MAX_PATH];
	CTerrainPuzzle	m_bmpPuzzle;
	DEQUE_PASSAGE m_setPassage;
	DEQUE_REGION  m_setRegion;
	friend struct TerrainLayerProbe;
public:
	void Show(CMyPos posShow);
	void Process(void* pInfo);
	int  GetType();
	int  GetExitIndex(CMyPos posCell);

public:
	static CTerrainLayer* CreateNew();

public:
	void LoadPuzzle(char* pszFile);
	void ClearPassage();
	void AddPassage(CMyPos posMap, int nIndex);
	void DelPassage(CMyPos posMap);

	void ClearRegion();
	void DelRegion(CMyPos posRegion, int nType);
	void AddRegion(C2DMapRegion* pRegion);
	C2DMapRegion* GetRegion(CMyPos posRegion, int nType);
	void ShowRegion(int nType);
	int  GetRegionAmount();
	C2DMapRegion* GetRegionByINdex(int nIndex);

	void GetPuzzleSize(CMySize& infoSize);
	int GetPuzzleGrideSize();
	char* GetPuzleFileName(){return m_szPuzzleFile;}

	void ProcessPuzzle();

	void LoadDataPassage(FILE* fp);
	void LoadTextPassage(FILE* fp);
	void SaveDataPassage(FILE* fp);
	void SaveTextPassage(FILE* fp);
	void LoadDataRegion(FILE* fp);
	void LoadTextRegion(FILE* fp);
	void SaveDataRegion(FILE* fp);
	void SaveTextRegion(FILE* fp);


	void SetPuzzleGrideSize(int nSize){m_bmpPuzzle.SetGrideSize(nSize);}
};
#endif
