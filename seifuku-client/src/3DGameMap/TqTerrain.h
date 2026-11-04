#if !defined(AFX_TQTERRAIN_H__SFK_1062__INCLUDED_)
#define AFX_TQTERRAIN_H__SFK_1062__INCLUDED_

#include "BaseFunc.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

class IAni;
class IPuzzleTriangle;
class IPuzzleBlock;
class ITargetBmp;

struct TqLayer
{
	WORD		wMaterial;
	DWORD		dwAlphaMask;
	int			nReserved;
	BYTE		byReserved;
	BYTE		bHead;
	TqLayer*	pNext;

	TqLayer();
	void		Add		(const TqLayer& info);
	TqLayer*	GetLast	(void);
	void		Destroy	(void);
};

struct TqMaterial
{
	string		strName;
	string		strFile;
	string		strTitle;
	DWORD		dwSrcBlend;
	DWORD		dwDestBlend;
	DWORD		dwColorOp;
	int			nType;
	int			nReserved;
};

struct TqVertex
{
	BYTE		byMaterial;
	BYTE		byShape;
	BYTE		byFrame;
	BYTE		byFlag;
	TqVertex()	{ byMaterial = 0xFF; }
};

class CTqTerrain
{
public:
	CTqTerrain();
	virtual ~CTqTerrain();

	BOOL	Load	(const char* pszFile);
	void	Show	(int nPosX, int nPosY);
	int		GetWidth	(void);
	int		GetHeight	(void);
	void	Process	(void);
	void	PreLoad	(int nPosX, int nPosY);
	void	Destroy	(void);
	int		Release	(void);

private:
	void		Create	(int nCols, int nRows);
	BOOL		LoadMaterial	(FILE* fp);
	BOOL		LoadShape		(FILE* fp);
	IAni*		GetAni	(WORD wMaterial);
	BOOL		IsGround	(WORD wMaterial);
	void		SetBlend	(WORD wMaterial);
	TqLayer*	GetBlock	(int nCellX, int nCellY);
	TqLayer*	GetBlockByIndex	(int nBlockX, int nBlockY);
	BOOL		GetShape	(WORD wShape, TqMaterial& info);
	void		GetVertex	(int nX, int nY, TqVertex& info);
	BOOL		IsFullVertex	(int nX, int nY);
	bool		IsShapeVertex	(WORD wX, WORD wY, BYTE byShape);
	void		ShowVertexShape	(int nX, int nY, int nShowX, int nShowY, int nWidth, int nHeight);
	void		ShowVertexCorner	(int nX, int nY, int nShowX, int nShowY, int nWidth, int nHeight);
	void		SetLayerTriangle	(TqLayer* pLayer, int nX, int nY, int nShowX, int nShowY, int nTriangle, int nPass, int* pnLayer);
	void		PrepareAlpha	(ITargetBmp* pTarget, int nBlockX, int nBlockY);
	void		RenderBlock	(ITargetBmp* pTarget, int nBlockX, int nBlockY);
	static int	RoundDouble	(double fValue);
	static __int64	BlockKey	(int nBlockX, int nBlockY);

private:
	int					m_nBlockSize;
	int					m_nCols;
	int					m_nRows;
	int					m_nVersion;
	IPuzzleTriangle*	m_pTriangle;
	IPuzzleBlock*		m_pBlock;
	vector<TqLayer*>		m_setBlock;
	map<DWORD, TqVertex>	m_mapVertex;
	vector<TqMaterial*>		m_setMaterial;
	vector<TqMaterial*>		m_setShape;
	map<DWORD, DWORD>		m_mapReserved;
	BYTE				m_abyAlpha[25];
	BYTE				m_abyAlphaLast[25];
	map<__int64, ITargetBmp*>	m_mapTarget;
	friend struct TqTerrainProbe;
};

#endif
