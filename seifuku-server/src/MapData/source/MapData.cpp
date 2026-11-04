#include "typedef.h"
#include "MapData.h"
#include "TerrainObj.h"
#include "common.h"
#include "inifile.h"
#include "I_MapData.h"
#include "LOGFILE.h"
#include <math.h>
#include <map>
#include <memory>

const int	_MAX_TITLESIZE			= 128;

typedef	unique_ptr<FILE, int (__cdecl*)(FILE*)>	FILE_GUARD;

CMapData::CMapData()
{
	m_setPassage.clear();
}
CMapData::~CMapData()
{
	this->ClearNodeSet();
	this->ClearCell();
	this->ClearPassage();
	this->ClearMapObj();
	this->ClearLayerSet();
}
CMapData* CMapData::CreateNew(LPCTSTR pszFileName, LPCTSTR pszVersion, int nBlockSize)
{
	if(!pszFileName)
		return NULL;
	FILE* fp = fopen(pszFileName, "rb");
	if(!fp)
	{
		LOGERROR("Cannot open map file [%s]", pszFileName);
		return NULL;
	}
	FILE_GUARD	guardFile(fp, fclose);

	CMapData* ptr = new CMapData;
	if(!ptr)
	{
		return NULL;
	}

	char szHeader[8];
	fread(&szHeader, sizeof(char), 8, fp);
	if(pszVersion)
		ptr->m_bDDVersion = true;

	char szFileName[_MAX_PATH];
	fread(szFileName, sizeof(char), _MAX_PATH, fp);
	if(	!ptr->LoadSurfaceCellData(fp)
		|| !ptr->LoadDataPassage(fp)
		|| !ptr->LoadTerrainItemData(fp)
		)
	{
		LOGERROR("Error loading map file [%s]!", pszFileName);

		SAFE_DELETE(ptr);
		return NULL;
	}
	if(_stricmp("none", szFileName) != 0 && !ptr->LoadPuzzle(szFileName, nBlockSize))
	{
		LOGERROR("Error loading map file [%s] background file [%s]!", pszFileName, szFileName);
		SAFE_DELETE(ptr);
		return NULL;
	}

	return ptr;
}
bool CMapData::LoadPuzzle(char* pszFile, int nBlockSize)
{
	if(!pszFile)
		return false;

	FILE* fp = fopen(pszFile, "rb");
	if(!fp)
	{
		LOGERROR("Cannot load puzzle file [%s]", pszFile);
		return false;
	}
	FILE_GUARD	guardFile(fp, fclose);
	char szHeader[8]="";
	fread(szHeader, 1, 8, fp);
	char szAni[256];
	fread(szAni, 1, 256, fp);

	SIZE infoSize;
	fread(&infoSize.cx, sizeof(UINT), 1, fp);
	fread(&infoSize.cy, sizeof(UINT), 1, fp);

#define FULL_CELL
#ifdef	FULL_CELL
	MYASSERT(m_sizeMap.cx == m_sizeMap.cy);
	const int _CELL_WIDTH	= 64;
	const int _CELL_HEIGHT	= 32;
	infoSize.cx	= (infoSize.cx * nBlockSize) / _CELL_WIDTH;
	infoSize.cy	= (infoSize.cy * nBlockSize) / _CELL_HEIGHT;

	int nDiagonalCells = m_sizeMap.cx / 2;
	int nTopFulCells	= (infoSize.cx/2) + ((nDiagonalCells-infoSize.cy)/2);
	int nLeftFulCells	= (infoSize.cy/2) + ((nDiagonalCells-infoSize.cx)/2);
	MYASSERT(nTopFulCells + nLeftFulCells == nDiagonalCells);
	for(int y = 0; y < nTopFulCells; y++)
	{
		for(int x = 0; x < nTopFulCells - y; x++)
		{
			CCell* pCell = GetCell(x, y);
			IF_OK(pCell)
			{
				pCell->FullMask();
			}
			 pCell = GetCell((m_sizeMap.cx-1) - x, (m_sizeMap.cy-1) - y);
			IF_OK(pCell)
			{
				pCell->FullMask();
			}
		}
	}
	for(int y = 0; y < nLeftFulCells; y++)	{
		for(int x = 0; x < nLeftFulCells - y; x++)
		{
			CCell* pCell = GetCell(x, (m_sizeMap.cy-1) - y);
			IF_OK(pCell)
			{
				pCell->FullMask();
			}
			 pCell = GetCell((m_sizeMap.cx-1) - x, y);
			IF_OK(pCell)
			{
				pCell->FullMask();
			}
		}
	}
#endif

	return true;
}
bool CMapData::LoadSurfaceCellData(FILE* fp)
{
	if(!fp)
		return false;

	SIZE infoSize = {0, 0};
	fread(&infoSize.cx, sizeof(UINT), 1, fp);
	fread(&infoSize.cy, sizeof(UINT), 1, fp);
	m_sizeMap		= infoSize;
	IF_NOT(m_setCell.Create(infoSize.cx*infoSize.cy))
		return false;

	int i, j;
	int	idx = 0;
	for(i = 0; i < infoSize.cy; i++)
	{
		for(j = 0; j < infoSize.cx; j++)
		{
			SHORT	nAlt	= 0;
			USHORT	dwMask	= 0;
			USHORT	nTerrian	= 0;

			fread(&dwMask, sizeof(unsigned short), 1, fp);
			fread(&nTerrian, sizeof(unsigned short), 1, fp);
			fread(&nAlt, sizeof(short), 1, fp);

			IF_OK(idx < m_setCell.size())
			{
				CCell* pCell = m_setCell[idx++];
				pCell->Create(nAlt, dwMask);
			}
		}
		DWORD dwMapCheckData;
		fread(&dwMapCheckData, sizeof(DWORD), 1, fp);
	}

	return true;
}
bool CMapData::LoadDataPassage(FILE* fp)
{
	if(!fp)
		return false;

	int nAmount = 0;
	fread(&nAmount, sizeof(int), 1, fp);
	for(int i = 0; i < nAmount; i++)
	{
		PassageInfo info;

		fread(&info, sizeof(PassageInfo), 1, fp);
		AddPassage(&info);
	}

	return true;
}
bool CMapData::LoadTerrainItemData(FILE* fp)
{
	if(!fp)
		return false;
	int nAmount = 0;
	fread(&nAmount, sizeof(int), 1, fp);

	for(int i = 0; i < nAmount; i++)
	{
		int nMapObjType = 0;
		fread(&nMapObjType, sizeof(int), 1, fp);

		switch(nMapObjType)
		{
		case MAP_COVER:
			{
				char	data[_MAX_PATH];
				CHECKF(_MAX_TITLESIZE <= _MAX_PATH);
				fread(data, sizeof(char), _MAX_PATH, fp);
				fread(data, sizeof(char), _MAX_TITLESIZE, fp);

				fread(data, sizeof(int), 1, fp);
				fread(data, sizeof(int), 1, fp);

				fread(data, sizeof(UINT), 1, fp);
				fread(data, sizeof(UINT), 1, fp);

				fread(data, sizeof(int), 1, fp);
				fread(data, sizeof(int), 1, fp);

				fread(data, sizeof(DWORD), 1, fp);
			}
			break;
		case MAP_TERRAIN:
			{
				char	szFileName[_MAX_PATH];
				POINT	posCell;
				fread(szFileName, sizeof(char), _MAX_PATH, fp);
				fread(&posCell.x, sizeof(int), 1, fp);
				fread(&posCell.y, sizeof(int), 1, fp);

				CTerrainObj* pTerrainObj = CTerrainObj::CreateNew(szFileName);
				if(!pTerrainObj)
					return false;

				pTerrainObj->SetPos(posCell);
				if(!this->AddMapObj(pTerrainObj))
					return false;
			}
			break;
		case MAP_SCENE:
			{
				char	data[_MAX_PATH];
				CHECKF(_MAX_TITLESIZE <= _MAX_PATH);

				fread(data, sizeof(char), _MAX_PATH, fp);
				fread(data, sizeof(char), _MAX_TITLESIZE, fp);

				fread(data, sizeof(int), 1, fp);
				fread(data, sizeof(int), 1, fp);

				fread(data, sizeof(int), 1, fp);
				fread(data, sizeof(int), 1, fp);

				fread(data, sizeof(DWORD), 1, fp);
				fread(data, sizeof(DWORD), 1, fp);
			}
			break;
		case MAP_3DEFFECT:
			{
				char	data[_MAX_PATH];
				CHECKF(64 <= _MAX_PATH);

				fread(data, sizeof(char), 64, fp);
				fread(data, sizeof(POINT), 1, fp);
			}
			break;
		case MAP_SOUND:
			{
				char	data[_MAX_PATH];
				CHECKF(_MAX_TITLESIZE <= _MAX_PATH);

				fread(data, sizeof(char), _MAX_PATH, fp);
				fread(data, sizeof(POINT), 1, fp);
				fread(data, sizeof(int), 1, fp);
				fread(data, sizeof(int), 1, fp);
			}
			break;
		default:
			return false;
		}
	}
	return true;
}
bool CMapData::PlaceTerrainObj(CTerrainObj* pTerrainObj)
{
	if(!pTerrainObj)
		return false;
	int nAmount = pTerrainObj->GetPartAmount();
	for(int i = 0; i < nAmount; i++)
	{
		CTerrainObjPart* pPart = pTerrainObj->GetPartByIndex(i);
		if(!pPart)
			return false;
		SIZE sizePart = pPart->GetBase();
		for(int j = 0; j < sizePart.cy; j++)
		{
			for(int k = 0; k < sizePart.cx; k++)
			{
				CLayer* pLayer = pPart->GetLayer(k, j);
				if(!pLayer)
					return false;
				shared_ptr<CLayer> pNewLayer = CLayer::CreateNew();
				if(!pNewLayer)
					return false;
				POINT posCell = pPart->GetPos();
				posCell.x -= k;
				posCell.y -= j;
				CCell* pCell = GetCell(posCell.x, posCell.y);
				if(!pCell)
					return false;

				pNewLayer->dwMask = pLayer->dwMask;
				pNewLayer->nTerrian = pLayer->nTerrian;
				pNewLayer->nAltitude = pLayer->nAltitude + pCell->GetFloorAlt(m_set2Layer);
				pCell->AddLayer(m_set2Layer, pNewLayer);
			}
		}
	}

	return true;
}
bool CMapData::DisplaceTerrainObj(CTerrainObj* pTerrainObj)
{
	CHECKF(pTerrainObj);

	int nAmount = pTerrainObj->GetPartAmount();
	for(int i = 0; i < nAmount; i++)
	{
		CTerrainObjPart* pPart = pTerrainObj->GetPartByIndex(i);
		MYASSERT(pPart);
		if(!pPart)
			return false;
		SIZE sizePart = pPart->GetBase();
		for(int j = 0; j < sizePart.cy; j++)
		{
			for(int k = 0; k < sizePart.cx; k++)
			{
				POINT posCell = pPart->GetPos();
				posCell.x -= k;
				posCell.y -= j;
				CCell* pCell = GetCell(posCell.x, posCell.y);

				MYASSERT(pCell);
				if(!pCell)
					return false;

				MYASSERT(pCell->DelLayer(m_set2Layer));
			}
		}
	}

	return true;
}
void CMapData::ClearCell()
{
	m_setCell.clear();
}
void CMapData::ClearPassage()
{
	int nAmount =  m_setPassage.size();
	for(int i = 0; i < nAmount; i++)
	{
		PassageInfo* pInfo = m_setPassage[i];
		if(pInfo)
			delete pInfo;
	}
	m_setPassage.clear();
}
void CMapData::ClearMapObj()
{
	int nAmount =  m_setMapObj.size();
	for(int i = 0; i < nAmount; i++)
	{
		CTerrainObj* pInfo = m_setMapObj[i];
		SAFE_DELETE(pInfo);
	}
	m_setMapObj.clear();
}
void CMapData::ClearLayerSet()
{
	m_set2Layer.clear();
}
CCell*  CMapData::GetCell(int nIndex)
{
	int nAmount = m_setCell.size();
	if((nIndex < 0) || (nIndex >= nAmount))
		return NULL;
	return m_setCell[nIndex];
}
CCell*  CMapData::GetCell(int nCellX, int nCellY)
{
	return GetCell(POS2INDEX(nCellX, nCellY, m_sizeMap.cx, m_sizeMap.cy));
}
const CCell*  CMapData::GetCell(int nCellX, int nCellY) const
{
	return const_cast<CMapData*>(this)->GetCell(POS2INDEX(nCellX, nCellY, m_sizeMap.cx, m_sizeMap.cy));
}
void CMapData::AddPassage(const PassageInfo* pInfo)
{

	CHECK(pInfo);

	POINT	posCell;
	posCell.x	= pInfo->nPosX;
	posCell.y	= pInfo->nPosY;
	AddPassage(posCell, pInfo->nIndex);
}
void CMapData::AddPassage(POINT posMap, int nIndex)
{
	{
		int nAmount =  m_setPassage.size();
		for(int i = 0; i < nAmount; i++)
		{
			PassageInfo* pInfo = m_setPassage[i];
			if(pInfo)
			{
				if((pInfo->nPosX == posMap.x) && (pInfo->nPosY == posMap.y))
				{
					pInfo->nIndex = nIndex;
					return;
				}
			}
		}
	}

	PassageInfo* pInfo = new PassageInfo;
	if(pInfo)
	{
		pInfo->nPosX = posMap.x;
		pInfo->nPosY = posMap.y;
		pInfo->nIndex = nIndex;
		m_setPassage.push_back(pInfo);
	}
}
bool CMapData::AddMapObj(CTerrainObj* pObj)
{
	CHECKF(pObj);

	m_setMapObj.push_back(pObj);

	return PlaceTerrainObj(pObj);
}

bool CMapData::DelMapObj(int idx)
{
	CHECKF(0 <= idx && idx < static_cast<int>(m_setMapObj.size()));

	CTerrainObj* pObj = m_setMapObj[idx];
	if(pObj && DisplaceTerrainObj(pObj))
	{
		SAFE_DELETE(pObj);
		m_setMapObj.erase(m_setMapObj.begin() + idx);
		return true;
	}

	return false;
}

bool CMapData::AddTerrainItem(OBJID idOwner, int nCellX, int nCellY, OBJID idTerrainItemType)
{
	CHECKF(idTerrainItemType != ID_NONE);

	char	szKey[256];
	_snprintf(szKey, 256, "NpcType%u", idTerrainItemType/10);
	CIniFile	ini("ini\\TerrainNpc.ini", szKey);
	char	szField[256];
	_snprintf(szField, 256, "Dir%u", idTerrainItemType%10);
	char	szFileName[256];
	if(!ini.GetString(szFileName, szField, 256))
	{
		LOGERROR("Can't find TerrainNpc[%d] in ini\\TerrainNpc.ini", idTerrainItemType);
		return NULL;
	}

	CTerrainObj* pObj = CTerrainObj::CreateNew(szFileName, idOwner);
	if(!pObj)
		return false;

	POINT	posCell;
	posCell.x	= nCellX;
	posCell.y	= nCellY;
	pObj->SetPos(posCell);

	if(!AddMapObj(pObj))
	{
		SAFE_DELETE(pObj);
		return false;
	}

	return true;
}

bool CMapData::DelTerrainItem(OBJID idOwner)
{
	for(int i = m_setMapObj.size()-1; i >= 0; i--)
	{
		CTerrainObj* pObj = m_setMapObj[i];
		if(pObj && pObj->GetOwnerID() == idOwner)
		{
			if(!DelMapObj(i))
				return false;
		}
	}

	return true;
}

int  CMapData::GetMapWidth() const
{
	return m_sizeMap.cx;
}

int	 CMapData::GetMapHeight() const
{
	return m_sizeMap.cy;
}

CCell* CMapData::QueryCell(int nCellX, int nCellY)
{
	CHECKF(0 <= nCellX && nCellX < m_sizeMap.cx && 0 <= nCellY && nCellY < m_sizeMap.cy);

	return GetCell(nCellX, nCellY);
}

const CCell* CMapData::QueryCell(int nCellX, int nCellY) const
{
	CHECKF(0 <= nCellX && nCellX < m_sizeMap.cx && 0 <= nCellY && nCellY < m_sizeMap.cy);

	return GetCell(nCellX, nCellY);
}
int CMapData::GetPassage(int x, int y) const
{
	for(int i = 0; i < m_setPassage.size(); i++)
	{
		if(m_setPassage[i]->nPosX == x && m_setPassage[i]->nPosY == y)
		{
			return m_setPassage[i]->nIndex;
		}
	}
	return PASSAGE_NONE;
}
struct MAPFILE_INFO
{
	int		nBlockSize;
	char	szFileName[_MAX_PATH];
};
typedef	map<OBJID, MAPFILE_INFO>	MAPFILE_SET;
static MAPFILE_SET	s_setMapFile;
IMapData* IMapData::CreateNew(int nMapDoc, LPCTSTR pszVersion )
{
	CHECKF(nMapDoc != ID_NONE);

	const char*	pszMapFile = "ini/GameMap.dat";
	if(s_setMapFile.size() == 0)
	{
		FILE* fp = fopen(pszMapFile, "rb");
		if(!fp)
		{
			LOGERROR("Map config file %s not found!", pszMapFile);
			return NULL;
		}
		FILE_GUARD	guardFile(fp, fclose);

		int nAmount = 0;
		fread(&nAmount, sizeof(int), 1, fp);
		for(int i = 0; i < nAmount; i++)
		{
			OBJID	idMap = ID_NONE;
			int		nLen = 0;
			MAPFILE_INFO	info;
			fread(&idMap, sizeof(OBJID), 1, fp);
			fread(&nLen, sizeof(int), 1, fp);
			fread(info.szFileName, 1, nLen, fp);
			info.szFileName[nLen] = 0;
			fread(&info.nBlockSize, sizeof(int), 1, fp);
			s_setMapFile[idMap] = info;
		}
	}

	MAPFILE_SET::iterator iter = s_setMapFile.find(nMapDoc);
	if(iter == s_setMapFile.end())
	{
		LOGERROR("Map %d not found in %s!", nMapDoc, pszMapFile);
		return NULL;
	}

	return CMapData::CreateNew(iter->second.szFileName, pszVersion, iter->second.nBlockSize);
}

void CMapData::DecRole(int x, int y, int nVal)
{
	CCell* ptr=QueryCell(x,y); CHECK(ptr); ptr->DecRole(nVal);
}

int CMapData::GetFloorAlt(int x, int y) const
{
	const CCell* ptr=QueryCell(x,y); CHECKF(ptr); return ptr->GetFloorAlt(m_set2Layer);
}

int CMapData::GetFloorAttr(int x, int y) const
{
	const CCell* ptr=QueryCell(x,y); CHECKF(ptr); return ptr->GetFloorAttr(m_set2Layer);
}

DWORD CMapData::GetFloorMask(int x, int y) const
{
	const CCell* ptr=QueryCell(x,y); IF_NOT(ptr) return MASK_MASK; return ptr->GetFloorMask(m_set2Layer);
}

int CMapData::GetRoleAmount(int x, int y) const
{
	const CCell* ptr=QueryCell(x,y); CHECKF(ptr); return ptr->GetRoleAmount();
}

int CMapData::GetSurfaceAlt(int x, int y) const
{
	const CCell* ptr=QueryCell(x,y); CHECKF(ptr); return ptr->GetSurfaceAlt();
}

void CMapData::IncRole(int x, int y, int nVal)
{
	CCell* ptr=QueryCell(x,y); CHECK(ptr); ptr->IncRole(nVal);
}
