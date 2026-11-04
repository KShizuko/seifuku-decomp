#pragma warning(disable:4786)
#include "typedef.h"
#include "TerrainObjPart.h"
#include "common.h"
#include "MapData.h"

CTerrainObjPart::CTerrainObjPart()
{
	SetObjType(TERRAIN_LEAF);

	m_sizeBase.cx			= 2;
	m_sizeBase.cy	 		= 2;
	m_nThickness 			= 40;
	m_dwFrameInterval 		= 200;
	m_posSceneOffset.x 		= 0;
	m_posSceneOffset.y 		= 0;
}

CTerrainObjPart::~CTerrainObjPart()
{
	this->Destroy();
}

void CTerrainObjPart::Destroy()
{
	m_setLayer.clear();
}

CTerrainObjPart* CTerrainObjPart::CreateNew(FILE* fp)
{
	if(!fp)
		return NULL;
	CTerrainObjPart* ptr = new CTerrainObjPart;
	if(!ptr)
		return NULL;

	fread(ptr->m_szAniFile, 1, 256, fp);
	fread(ptr->m_szAniTitle, 1, 64, fp);

	fread(&ptr->m_posOffset.x, sizeof(int), 1, fp);
	fread(&ptr->m_posOffset.y, sizeof(int), 1, fp);

	fread(&ptr->m_dwFrameInterval, sizeof(DWORD), 1, fp);

	fread(&ptr->m_sizeBase.cx, sizeof(UINT), 1, fp);
	fread(&ptr->m_sizeBase.cy, sizeof(UINT), 1, fp);

	fread(&ptr->m_nThickness, sizeof(int), 1, fp);
	fread(&ptr->m_posSceneOffset.x, sizeof(int), 1, fp);
	fread(&ptr->m_posSceneOffset.y, sizeof(int), 1, fp);
	fread(&ptr->m_nHeight, sizeof(int), 1, fp);
	SIZE infoSize = ptr->GetBase();
	for(int i = 0; i < infoSize.cy; i++)
	{
		for(int j = 0; j < infoSize.cx; j++)
		{
			shared_ptr<CLayer> pLayer = CLayer::CreateNew();
			if(!pLayer)
			{
				SAFE_DELETE(ptr);
				return NULL;
			}

			int	nData;
			fread(&nData, sizeof(DWORD), 1, fp);
			pLayer->dwMask = nData;
			fread(&nData, sizeof(int), 1, fp);
			pLayer->nTerrian = nData;
			fread(&nData, sizeof(int), 1, fp);
			pLayer->nAltitude = nData;

			ptr->m_setLayer.push_back(pLayer);
		}
	}

	return ptr;
}

CLayer* CTerrainObjPart::GetLayer(int nX, int nY)
{
	int nAmount = m_setLayer.size();
	int nIndex = POS2INDEX(nX, nY, m_sizeBase.cx, m_sizeBase.cy);
	if((nIndex >= nAmount)||(nIndex < 0))
		return NULL;
	return m_setLayer[nIndex].get();
}
