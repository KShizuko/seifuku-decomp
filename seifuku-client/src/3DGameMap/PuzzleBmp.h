#ifndef _PUZZLEBMP_H
#define _PUZZLEBMP_H
#include "BaseFunc.h"

extern int PUZZLE_GRIDE_SIZE;
const int PUZZLE_SCRX = 800;
const int PUZZLE_SCRY = 600;

const  int		_MAX_DIRTY_RECT = 256;

class CPuzzleBmp
{
public:
	CPuzzleBmp();
	virtual ~CPuzzleBmp();

protected:
	int m_nPuzzleAmount;
	unsigned short* m_usPuzzleIndex;
	BOOL m_bTerrain;

public:
	unsigned short GetGrid(int iPosX, int iPosY);
	void   SetSize(CMySize infoSize);

public:
	void SetAniFile(char* pszFile){if(pszFile) strcpy(m_szAniFile ,pszFile);}
	char* GetAniFile(){return m_szAniFile;}
	
public:
	void Show(int iPosX, int iPosY, BOOL bGrid=false, BOOL bPos=false);

	void GetSizeByGride(CMySize &infoSize){memcpy(&infoSize ,&m_infoSize,sizeof(infoSize));}
	void GetSize(CMySize &infoSize);

	BOOL Load(char* pszFile);
	void SetGrid(int iPosX, int iPosY, unsigned short usIndex);
	unsigned short GetGrid(CMyPos posShow);
	void Bmp2Grid	(CMyPos posBmp, CMyPos& posGrid);
	void Grid2Bmp	(CMyPos posGrid, CMyPos& posBmp);
	void Save(char* pszFile);
	void SetZoomInNum(int nZoom);
	void Unit(CPuzzleBmp* pPuzzle, CMyPos posGrid);
	static BOOL IsDirty(CMyRect rectInfo);
	static void SetNormalGrideSize(int nSize);
	void Destroy();

	void PreLoad(int iPosX, int iPosY);
	int	 GetZoomInNum(){return m_nZoomInNum;}

	void SetTerrainFlag(BOOL bTerrain){m_bTerrain = bTerrain;}
	void SetRollSpeedX(int nSpeedX){m_nRollSpeedX = nSpeedX;}
	void SetRollSpeedY(int nSpeedY){m_nRollSpeedY = nSpeedY;}
	int	 GetRoolSpeedX(){return m_nRollSpeedX;}
	int	 GetRoolSpeedY(){return m_nRollSpeedY;}
	void SetMoveRate(int nMoveRateX, int nMoveRateY){m_nMoveRateX = nMoveRateX; m_nMoveRateY = nMoveRateY;}
	void SetRoll(BOOL bRoll){m_bRoll = bRoll;}
	
	void SetGrideSize(int nSize);
	void SetGrideSizeOnly(int nSize){m_nGrideSize = nSize;}
	int  GetGrideSize(){return m_nGrideSize;}
	static int  GetNormalGrideSize();
protected:
	CMySize m_infoSize;
	char	m_szAniFile[256];
	int		m_nZoomInNum;
	int		m_nGrideSize;
	int		m_nRollSpeedX;
	int		m_nRollSpeedY;
	DWORD	m_dwVersion;
	DWORD	m_dwTimeBegin;
	int		m_nMoveRateX;
	int		m_nMoveRateY;
	BOOL	m_bRoll;
	friend struct PuzzleBmpProbe;
public:
	static CMyRect	s_setRect[_MAX_DIRTY_RECT]; 
	static int		s_nRectAmount;

public:
	static void ClearRectSet(){s_nRectAmount = 0;}
	static BOOL TestRect(CMyRect rectInfo, BOOL bTerrain, BOOL bTransparent);

};
#endif

