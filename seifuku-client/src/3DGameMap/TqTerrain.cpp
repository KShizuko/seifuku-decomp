#include "TqTerrain.h"
#include "3DGameMap.h"
#include "GameDataSet.h"

static const char s_szShapeName[14][32] =
{
	"Up", "Down", "Left", "Right", "LeftUp2RightDown", "RightUp2LeftDown",
	"MinLeftDown", "MinRightDown", "MinLeftUp", "MinRightUp",
	"FullLeftDown", "FullRightDown", "FullLeftUp", "FullRightUp",
};
static const char* s_pszMiddle = "Middle";

TqLayer::TqLayer()
{
	wMaterial	= 0xFFFF;
	dwAlphaMask	= 0;
	nReserved	= -1;
	byReserved	= 0;
	bHead		= 0;
	pNext		= NULL;
}

void TqLayer::Add(const TqLayer& info)
{
	if (wMaterial == 0xFFFF)
	{
		dwAlphaMask	= info.dwAlphaMask;
		wMaterial	= info.wMaterial;
		return;
	}
	TqLayer* pNew	= new TqLayer;
	TqLayer* pLast	= this;
	if (pNext)
		pLast = pNext->GetLast();
	pLast->pNext		= pNew;
	pNew->dwAlphaMask	= info.dwAlphaMask;
	pNew->wMaterial		= info.wMaterial;
	pNew->pNext			= NULL;
}

TqLayer* TqLayer::GetLast(void)
{
	TqLayer* pLayer = this;
	while (pLayer->pNext)
		pLayer = pLayer->pNext;
	return pLayer;
}

void TqLayer::Destroy(void)
{
	if (pNext)
	{
		pNext->Destroy();
		pNext = NULL;
	}
	delete this;
}

CTqTerrain::CTqTerrain()
{
	m_nVersion		= 1000;
	m_nRows			= 0;
	m_nCols			= 0;
	m_nBlockSize	= 0x100;
	m_pTriangle		= ::PuzzelTriangleCreate();
	m_pBlock		= ::PuzzelBlockCreate(0x100, 0x100, 4, 4);
}

CTqTerrain::~CTqTerrain()
{
	this->Destroy();
	m_pTriangle->Release();
	m_pBlock->Release();
	for (map<__int64, ITargetBmp*>::iterator iter = m_mapTarget.begin(); iter != m_mapTarget.end(); iter++)
	{
		if (iter->second)
			iter->second->Release();
	}
	m_mapTarget.clear();
}

int CTqTerrain::Release()
{
	delete this;
	return 0;
}

BOOL CTqTerrain::Load(const char* pszFile)
{
	this->Destroy();
	if (!pszFile || strlen(pszFile) == 0)
		return false;
	FILE* fp = fopen(pszFile, "rb");
	if (!fp)
		return false;

	char szHeader[16] = "TqTerrain";
	fread(szHeader, 1, 16, fp);
	if (strcmp(szHeader, szHeader) != 0)
		return false;
	int nVersion = 1000;
	fread(&nVersion, 4, 1, fp);
	if (nVersion != 1000)
		return false;
	fread(&m_nCols, 4, 1, fp);
	fread(&m_nRows, 4, 1, fp);
	this->Create(m_nCols, m_nRows);
	this->LoadMaterial(fp);
	this->LoadShape(fp);

	DWORD dwAmount = 0;
	fread(&dwAmount, 4, 1, fp);
	DWORD i;
	for (i = 0; i < dwAmount; i++)
	{
		BYTE byCount = 0;
		fread(&byCount, 1, 1, fp);
		for (DWORD j = 0; j < byCount; j++)
		{
			TqLayer info;
			fread(&info.wMaterial, 2, 1, fp);
			fread(&info.dwAlphaMask, 4, 1, fp);
			m_setBlock[i]->Add(info);
		}
	}

	dwAmount = 0;
	fread(&dwAmount, 4, 1, fp);
	for (i = 0; i < dwAmount; i++)
	{
		DWORD dwKey;
		TqVertex info;
		fread(&dwKey, 4, 1, fp);
		fread(&info.byMaterial, 1, 1, fp);
		fread(&info.byShape, 1, 1, fp);
		fread(&info.byFrame, 1, 1, fp);
		fread(&info.byFlag, 1, 1, fp);
		m_mapVertex[dwKey] = info;
	}
	fclose(fp);
	return true;
}

void CTqTerrain::Destroy(void)
{
	vector<TqLayer*>::iterator iter;
	for (iter = m_setBlock.begin(); iter != m_setBlock.end(); iter++)
	{
		TqLayer* pLayer = *iter;
		if (pLayer)
		{
			if (pLayer->pNext)
			{
				pLayer->pNext->Destroy();
				pLayer->pNext = NULL;
			}
			delete pLayer;
		}
	}
	m_setBlock.clear();

	vector<TqMaterial*>::iterator iterMaterial;
	for (iterMaterial = m_setMaterial.begin(); iterMaterial != m_setMaterial.end(); iterMaterial++)
	{
		if (*iterMaterial)
			delete *iterMaterial;
	}
	m_setMaterial.clear();
	for (iterMaterial = m_setShape.begin(); iterMaterial != m_setShape.end(); iterMaterial++)
	{
		if (*iterMaterial)
			delete *iterMaterial;
	}
	m_setShape.clear();
	m_mapVertex.clear();

	m_nVersion		= 1000;
	m_nRows			= 0;
	m_nCols			= 0;
	m_nBlockSize	= 0x100;
}

void CTqTerrain::Create(int nCols, int nRows)
{
	this->Destroy();
	if (nCols == 0 || nRows == 0)
		return;
	m_nCols = nCols;
	m_nRows = nRows;
	for (DWORD i = 0; i < (DWORD)(m_nCols * m_nRows); i++)
	{
		TqLayer* pLayer = new TqLayer;
		pLayer->bHead = 1;
		m_setBlock.push_back(pLayer);
	}
}

BOOL CTqTerrain::LoadMaterial(FILE* fp)
{
	if (!fp)
		return false;
	vector<TqMaterial*>::iterator iter;
	for (iter = m_setMaterial.begin(); iter != m_setMaterial.end(); iter++)
	{
		if (*iter)
			delete *iter;
	}
	m_setMaterial.clear();

	int nVersion = 1000;
	fread(&nVersion, 4, 1, fp);
	WORD wAmount = 0;
	fread(&wAmount, 2, 1, fp);
	for (int i = 0; (WORD)i < wAmount; i++)
	{
		TqMaterial* pInfo = new TqMaterial;
		char szBuf[1024];
		WORD wLen = 0;
		fread(&wLen, 2, 1, fp);
		fread(szBuf, 1, wLen, fp);
		szBuf[wLen] = 0;
		pInfo->strName = szBuf;
		fread(&wLen, 2, 1, fp);
		fread(szBuf, 1, wLen, fp);
		szBuf[wLen] = 0;
		pInfo->strFile = szBuf;
		fread(&wLen, 2, 1, fp);
		fread(szBuf, 1, wLen, fp);
		szBuf[wLen] = 0;
		pInfo->strTitle = szBuf;
		fread(&pInfo->dwSrcBlend, 1, 4, fp);
		fread(&pInfo->dwDestBlend, 1, 4, fp);
		fread(&pInfo->dwColorOp, 1, 4, fp);
		fread(&pInfo->nType, 1, 4, fp);
		fread(&pInfo->nReserved, 1, 4, fp);
		m_setMaterial.push_back(pInfo);
	}
	return true;
}

BOOL CTqTerrain::LoadShape(FILE* fp)
{
	if (!fp)
		return false;
	vector<TqMaterial*>::iterator iter;
	for (iter = m_setShape.begin(); iter != m_setShape.end(); iter++)
	{
		if (*iter)
			delete *iter;
	}
	m_setShape.clear();

	int nVersion = 1000;
	fread(&nVersion, 4, 1, fp);
	WORD wAmount = 0;
	fread(&wAmount, 2, 1, fp);
	for (int i = 0; (WORD)i < wAmount; i++)
	{
		TqMaterial* pInfo = new TqMaterial;
		char szBuf[1024];
		WORD wLen = 0;
		fread(&wLen, 2, 1, fp);
		fread(szBuf, 1, wLen, fp);
		szBuf[wLen] = 0;
		pInfo->strName = szBuf;
		fread(&wLen, 2, 1, fp);
		fread(szBuf, 1, wLen, fp);
		szBuf[wLen] = 0;
		pInfo->strFile = szBuf;
		fread(&wLen, 2, 1, fp);
		fread(szBuf, 1, wLen, fp);
		szBuf[wLen] = 0;
		pInfo->strTitle = szBuf;
		fread(&pInfo->dwSrcBlend, 1, 4, fp);
		fread(&pInfo->dwDestBlend, 1, 4, fp);
		fread(&pInfo->dwColorOp, 1, 4, fp);
		fread(&pInfo->nType, 1, 4, fp);
		fread(&pInfo->nReserved, 1, 4, fp);
		m_setShape.push_back(pInfo);
	}
	return true;
}

IAni* CTqTerrain::GetAni(WORD wMaterial)
{
	if (wMaterial >= m_setMaterial.size())
		return NULL;
	TqMaterial* pInfo = m_setMaterial[wMaterial];
	return GameDataSetQuery()->GetDataAni(pInfo->strFile.c_str(), pInfo->strTitle.c_str(), EXIGENCE_IMMEDIATE, 30000);
}

BOOL CTqTerrain::IsGround(WORD wMaterial)
{
	if (wMaterial < m_setMaterial.size())
	{
		TqMaterial* pInfo = m_setMaterial[wMaterial];
		if (pInfo)
			return pInfo->nType == 1;
	}
	return false;
}

void CTqTerrain::SetBlend(WORD wMaterial)
{
	if (wMaterial < m_setMaterial.size())
	{
		TqMaterial* pInfo = m_setMaterial[wMaterial];
		m_pTriangle->SetBlend(pInfo->dwSrcBlend, pInfo->dwDestBlend, pInfo->dwColorOp);
	}
}

TqLayer* CTqTerrain::GetBlock(int nCellX, int nCellY)
{
	return this->GetBlockByIndex((DWORD)nCellX >> 2, (DWORD)nCellY >> 2);
}

TqLayer* CTqTerrain::GetBlockByIndex(int nBlockX, int nBlockY)
{
	if ((DWORD)nBlockX < (DWORD)m_nCols && (DWORD)nBlockY < (DWORD)m_nRows)
	{
		DWORD dwIndex = m_nCols * nBlockY + nBlockX;
		if (dwIndex < m_setBlock.size())
			return m_setBlock[dwIndex];
	}
	return NULL;
}

BOOL CTqTerrain::GetShape(WORD wShape, TqMaterial& info)
{
	if (wShape >= m_setShape.size())
		return false;
	info = *m_setShape[wShape];
	return true;
}

void CTqTerrain::GetVertex(int nX, int nY, TqVertex& info)
{
	info.byMaterial = 0xFF;
	if ((WORD)nX < (DWORD)(m_nCols << 2) && (WORD)nY <= (DWORD)(m_nRows << 2))
	{
		DWORD dwKey = ((DWORD)(WORD)nX << 16) + (WORD)nY;
		map<DWORD, TqVertex>::iterator iter = m_mapVertex.find(dwKey);
		if (iter != m_mapVertex.end())
			info = iter->second;
	}
}

BOOL CTqTerrain::IsFullVertex(int nX, int nY)
{
	if ((WORD)nX < (DWORD)(m_nCols << 2) && (WORD)nY <= (DWORD)(m_nRows << 2))
	{
		DWORD dwKey = ((DWORD)(WORD)nX << 16) + (WORD)nY;
		map<DWORD, TqVertex>::iterator iter = m_mapVertex.find(dwKey);
		if (iter != m_mapVertex.end() && iter->second.byShape <= 0x0E && iter->second.byFlag == 1)
			return true;
	}
	return false;
}

bool CTqTerrain::IsShapeVertex(WORD wX, WORD wY, BYTE byShape)
{
	if (wX >= (DWORD)(m_nCols << 2) || wY > (DWORD)(m_nRows << 2))
		return false;
	DWORD dwKey = ((DWORD)wX << 16) + wY;
	map<DWORD, TqVertex>::iterator iter = m_mapVertex.find(dwKey);
	if (iter != m_mapVertex.end() && iter->second.byShape == byShape && iter->second.byFlag == 1)
		return true;
	return false;
}

void CTqTerrain::ShowVertexShape(int nX, int nY, int nShowX, int nShowY, int nWidth, int nHeight)
{
	static TqVertex infoVertex;
	this->GetVertex(nX, nY, infoVertex);
	if (infoVertex.byMaterial != 0xFF && infoVertex.byShape < 14 && infoVertex.byFlag == 1)
	{
		static TqMaterial infoShape;
		if (!this->GetShape(infoVertex.byMaterial, infoShape))
			return;
		IAni* pAni = GameDataSetQuery()->GetDataAni(infoShape.strFile.c_str(), s_szShapeName[infoVertex.byShape], EXIGENCE_IMMEDIATE, 30000);
		if (!pAni)
			return;
		CMyBitmap* pBmp = pAni->GetFrame(infoVertex.byFrame % pAni->GetFrameAmount());
		if (!pBmp)
			return;
		pBmp->ShowEx(nShowX, nShowY, NULL, nWidth, nHeight, 0, 0, 0);
	}
}

void CTqTerrain::ShowVertexCorner(int nX, int nY, int nShowX, int nShowY, int nWidth, int nHeight)
{
	TqVertex infoVertex;
	this->GetVertex(nX, nY, infoVertex);
	if (infoVertex.byMaterial != 0xFF && infoVertex.byShape < 14 && infoVertex.byFlag != 1)
	{
		static TqMaterial infoShape;
		if (!this->GetShape(infoVertex.byMaterial, infoShape))
			return;
		IAni* pAni = GameDataSetQuery()->GetDataAni(infoShape.strFile.c_str(), s_pszMiddle, EXIGENCE_IMMEDIATE, 30000);
		if (!pAni)
			return;
		DWORD dwFlag = infoVertex.byFlag;
		m_pTriangle->SetVertex(0, nShowX, nShowY, ((dwFlag & 2) ? 0xFF000000 : 0) + 0x00FFFFFF, 0.0f, 0.0f);
		m_pTriangle->SetVertex(1, nShowX + nWidth, nShowY, ((dwFlag & 4) ? 0xFF000000 : 0) + 0x00FFFFFF, 1.0f, 0.0f);
		m_pTriangle->SetVertex(2, nShowX, nShowY + nHeight, ((dwFlag & 8) ? 0xFF000000 : 0) + 0x00FFFFFF, 0.0f, 1.0f);
		m_pTriangle->Show(pAni->GetFrame(infoVertex.byFrame % pAni->GetFrameAmount()));
		m_pTriangle->SetVertex(0, nShowX + nWidth, nShowY, ((dwFlag & 4) ? 0xFF000000 : 0) + 0x00FFFFFF, 1.0f, 0.0f);
		m_pTriangle->SetVertex(1, nShowX + nWidth, nShowY + nHeight, ((dwFlag & 0x10) ? 0xFF000000 : 0) + 0x00FFFFFF, 1.0f, 1.0f);
		m_pTriangle->SetVertex(2, nShowX, nShowY + nHeight, ((dwFlag & 8) ? 0xFF000000 : 0) + 0x00FFFFFF, 0.0f, 1.0f);
		m_pTriangle->Show(pAni->GetFrame(infoVertex.byFrame % pAni->GetFrameAmount()));
	}
}

int CTqTerrain::RoundDouble(double fValue)
{
	int nValue	= (int)fValue;
	int nHalf	= (int)(fValue + 0.5);
	return (nHalf > nValue) ? nValue + 1 : nValue;
}

__int64 CTqTerrain::BlockKey(int nBlockX, int nBlockY)
{
	return ((__int64)nBlockY << 32) + (DWORD)nBlockX;
}

int CTqTerrain::GetWidth(void)
{
	return m_nCols * m_nBlockSize;
}

int CTqTerrain::GetHeight(void)
{
	return m_nRows * m_nBlockSize;
}

void CTqTerrain::Process(void)
{
	for (map<__int64, ITargetBmp*>::iterator iter = m_mapTarget.begin(); iter != m_mapTarget.end(); iter++)
	{
		if (iter->second)
			iter->second->Release();
	}
	m_mapTarget.clear();
}

void CTqTerrain::Show(int nPosX, int nPosY)
{
	int nBlockX = -(nPosX / 256);
	int nBlockY = -(nPosY / 256);
	if (nPosX > 0)
		nBlockX--;
	if (nPosY > 0)
		nBlockY--;
	int nCols = m_nCols;
	int nRows = m_nRows;
	int nSize = g_objGameMap.GetScale() * 256 / _NORMAL_SCALE;
	int nCountX = CMyBitmap::GetScreenWidth() / nSize + 2;
	int nCountY = CMyBitmap::GetScreenHeight() / nSize + 2;
	int nOffsetX = nBlockX * 256 + nPosX;
	int nOffsetY = nBlockY * 256 + nPosY;
	for (int j = -nCountY; j < nCountY; j++)
	{
		for (int i = -nCountX; i < nCountX; i++)
		{
			int nX = nBlockX + i;
			int nY = nBlockY + j;
			if (nX < 0 || nY < 0 || nX >= nCols || nY >= nRows)
				continue;
			int nShowX = i * 256 + nOffsetX;
			int nShowY = j * 256 + nOffsetY;
			g_objGameMap.MapScaleShowPos(nShowX, nShowY);
			if (nShowX + nSize < 0 || nShowY + nSize < 0)
				continue;
			if (nShowX >= CMyBitmap::GetScreenWidth() || nShowY >= CMyBitmap::GetScreenHeight())
				continue;
			map<__int64, ITargetBmp*>::iterator iter = m_mapTarget.find(BlockKey(nX, nY));
			if (iter != m_mapTarget.end() && iter->second)
				iter->second->ShowAlpha(nShowX, nShowY, nSize, nSize, 0xFFFFFFFF);
		}
	}
}

void CTqTerrain::SetLayerTriangle(TqLayer* pLayer, int nX, int nY, int nShowX, int nShowY, int nTriangle, int nPass, int* pnLayer)
{
	IAni* pAni = this->GetAni(pLayer->wMaterial);
	this->SetBlend(pLayer->wMaterial);
	if (!pAni)
		return;
	CMyBitmap* pBmp = pAni->GetFrame(0);
	if (!pBmp)
		return;
	BOOL bOpaque = false;
	if (pnLayer)
	{
		(*pnLayer)++;
		bOpaque = (*pnLayer == 1 && nPass == 0);
	}

	int nIndex0 = (nX & 3) + (nY & 3) * 5;
	int nIndex1 = nIndex0 + 1;
	int nIndex2 = (nX & 3) + ((nY & 3) + 1) * 5;
	int nIndex3 = nIndex0 + 6;
	int nWidth = 0, nHeight = 0;
	pBmp->GetSize(nWidth, nHeight);
	int nTilesX = (DWORD)nWidth >> 6;
	int nTilesY = (DWORD)nHeight >> 6;
	int nTileX = nX % nTilesX;
	int nTileY = nY % nTilesY;
	float fU0 = (float)nTileX / (float)nTilesX;
	float fV0 = (float)nTileY / (float)nTilesY;
	float fU1 = (float)(nTileX + 1) / (float)nTilesX;
	float fV1 = (float)(nTileY + 1) / (float)nTilesY;

	int nIndex[3], nVertexX[3], nVertexY[3];
	float fU[3], fV[3];
	if (nTriangle == 0)
	{
		nIndex[0] = nIndex0; nVertexX[0] = nShowX;			nVertexY[0] = nShowY;			fU[0] = fU0; fV[0] = fV0;
		nIndex[1] = nIndex1; nVertexX[1] = nShowX + 0x40;	nVertexY[1] = nShowY;			fU[1] = fU1; fV[1] = fV0;
		nIndex[2] = nIndex2; nVertexX[2] = nShowX;			nVertexY[2] = nShowY + 0x40;	fU[2] = fU0; fV[2] = fV1;
	}
	else
	{
		nIndex[0] = nIndex1; nVertexX[0] = nShowX + RoundDouble(64.0);	nVertexY[0] = nShowY;							fU[0] = fU1; fV[0] = fV0;
		nIndex[1] = nIndex3; nVertexX[1] = nShowX + RoundDouble(64.0);	nVertexY[1] = nShowY + RoundDouble(64.0);		fU[1] = fU1; fV[1] = fV1;
		nIndex[2] = nIndex2; nVertexX[2] = nShowX;						nVertexY[2] = nShowY + RoundDouble(64.0);		fU[2] = fU0; fV[2] = fV1;
	}
	for (int i = 0; i < 3; i++)
	{
		BYTE byAlpha = 0;
		if ((BYTE)nIndex[i] < 25)
			byAlpha = (pLayer->dwAlphaMask & (1 << (BYTE)nIndex[i])) ? 0xFF : 0;
		if (byAlpha == 0xFF)
			if (!pnLayer)
				m_abyAlpha[nIndex[i]] = 1;
		if (bOpaque)
			byAlpha = 0xFF;
		m_pTriangle->SetVertex(i, nVertexX[i], nVertexY[i], ((DWORD)byAlpha << 24) + 0x00FFFFFF, fU[i], fV[i]);
	}
	if (pnLayer)
		m_pTriangle->Show(pBmp);
}

void CTqTerrain::PrepareAlpha(ITargetBmp* pTarget, int nBlockX, int nBlockY)
{
	memset(m_abyAlpha, 0, sizeof(m_abyAlpha));
	if (!pTarget)
		return;
	for (int nCellY = 0; nCellY < 4; nCellY++)
	{
		for (int nCellX = 0; nCellX < 4; nCellX++)
		{
			int nX = nBlockX * 4 + nCellX;
			int nY = nBlockY * 4 + nCellY;
			int nShowX = nCellX * 0x40;
			int nShowY = nCellY * 0x40;
			for (int nPass = 0; nPass < 3; nPass++)
			{
				if (nPass == 2)
				{
					TqVertex info;
					this->GetVertex(nX, nY, info);
					if (info.byMaterial == 0xFF || info.byShape >= 14 || info.byFlag == 1)
						continue;
					int nIndex = (nX & 3) + (nY & 3) * 5;
					m_abyAlpha[nIndex]		= 1;
					m_abyAlpha[nIndex + 1]	= 1;
					m_abyAlpha[nIndex + 5]	= 1;
					m_abyAlpha[nIndex + 6]	= 1;
				}
				TqLayer* pLayer;
				for (pLayer = this->GetBlock(nX, nY); pLayer; pLayer = pLayer->pNext)
				{
					if (nPass == 0 && !this->IsGround(pLayer->wMaterial))
						continue;
					if (nPass == 1 && this->IsGround(pLayer->wMaterial))
						continue;
					this->SetLayerTriangle(pLayer, nX, nY, nShowX, nShowY, 0, nPass, NULL);
				}
				for (pLayer = this->GetBlock(nX, nY); pLayer; pLayer = pLayer->pNext)
				{
					if (nPass == 0 && !this->IsGround(pLayer->wMaterial))
						continue;
					if (nPass == 1 && this->IsGround(pLayer->wMaterial))
						continue;
					this->SetLayerTriangle(pLayer, nX, nY, nShowX, nShowY, 1, nPass, NULL);
				}
			}
		}
	}
	for (int i = 0; i < 25; i++)
	{
		m_abyAlphaLast[i] = m_abyAlpha[i];
		if (m_abyAlpha[i])
			pTarget->SetAlpha((BYTE)i, 0xFF);
		else
			pTarget->SetAlpha((BYTE)i, 0);
	}
}

void CTqTerrain::RenderBlock(ITargetBmp* pTarget, int nBlockX, int nBlockY)
{
	this->PrepareAlpha(pTarget, nBlockX, nBlockY);
	if (!pTarget)
		return;
	if (!pTarget->Begin())
		return;
	CMyBitmap::ClearBuffer(false, true, 0xFFFFFFFF);
	for (int nCellY = 0; nCellY < 4; nCellY++)
	{
		for (int nCellX = 0; nCellX < 4; nCellX++)
		{
			int nX = nBlockX * 4 + nCellX;
			int nY = nBlockY * 4 + nCellY;
			int nShowX = nCellX * 0x40;
			int nShowY = nCellY * 0x40;
			for (int nPass = 0; nPass < 4; nPass++)
			{
				if (nPass == 1)
				{
					this->ShowVertexShape(nX, nY, nShowX, nShowY, 0x40, 0x40);
					continue;
				}
				if (nPass == 3)
				{
					this->ShowVertexCorner(nX, nY, nShowX, nShowY, 0x40, 0x40);
					continue;
				}
				if (nPass == 2 && this->IsFullVertex(nX, nY))
					continue;
				int nLayer = 0;
				TqLayer* pLayer;
				for (pLayer = this->GetBlock(nX, nY); pLayer; pLayer = pLayer->pNext)
				{
					if (nPass == 0 && !this->IsGround(pLayer->wMaterial))
						continue;
					if (nPass == 2 && this->IsGround(pLayer->wMaterial))
						continue;
					this->SetLayerTriangle(pLayer, nX, nY, nShowX, nShowY, 0, nPass, &nLayer);
				}
				nLayer = 0;
				for (pLayer = this->GetBlock(nX, nY); pLayer; pLayer = pLayer->pNext)
				{
					if (nPass == 0 && !this->IsGround(pLayer->wMaterial))
						continue;
					if (nPass == 2 && this->IsGround(pLayer->wMaterial))
						continue;
					this->SetLayerTriangle(pLayer, nX, nY, nShowX, nShowY, 1, nPass, &nLayer);
				}
			}
		}
	}
	pTarget->End();
}

void CTqTerrain::PreLoad(int nPosX, int nPosY)
{
	if (CMyBitmap::IfDeviceLost())
		return;
	int nBlockX = -(nPosX / 256);
	int nBlockY = -(nPosY / 256);
	if (nPosX > 0)
		nBlockX--;
	if (nPosY > 0)
		nBlockY--;
	int nRows = m_nRows;
	int nCols = m_nCols;
	int nSize = g_objGameMap.GetScale() * 256 / _NORMAL_SCALE;
	int nCountX = CMyBitmap::GetScreenWidth() / nSize + 2;
	int nCountY = CMyBitmap::GetScreenHeight() / nSize + 2;
	vector<__int64> setVisible;
	int nOffsetX = nBlockX * 256 + nPosX;
	int nOffsetY = nBlockY * 256 + nPosY;
	for (int j = -nCountY; j < nCountY; j++)
	{
		for (int i = -nCountX; i < nCountX; i++)
		{
			int nX = nBlockX + i;
			int nY = nBlockY + j;
			if (nX < 0 || nY < 0 || nX >= nCols || nY >= nRows)
				continue;
			int nShowX = i * 256 + nOffsetX;
			int nShowY = j * 256 + nOffsetY;
			g_objGameMap.MapScaleShowPos(nShowX, nShowY);
			if (nShowX + nSize < 0 || nShowY + nSize < 0)
				continue;
			if (nShowX >= CMyBitmap::GetScreenWidth() || nShowY >= CMyBitmap::GetScreenHeight())
				continue;
			setVisible.push_back(BlockKey(nX, nY));
		}
	}

	vector<ITargetBmp*> setRelease;
	map<__int64, ITargetBmp*>::iterator iter;
	for (iter = m_mapTarget.begin(); iter != m_mapTarget.end(); )
	{
		BOOL bVisible = false;
		for (DWORD k = 0; k < setVisible.size(); k++)
		{
			if (setVisible[k] == iter->first)
			{
				bVisible = true;
				break;
			}
		}
		map<__int64, ITargetBmp*>::iterator iterNext = iter;
		iterNext++;
		if (!bVisible)
		{
			setRelease.push_back(iter->second);
			m_mapTarget.erase(iter);
		}
		iter = iterNext;
	}

	for (DWORD k = 0; k < setVisible.size(); k++)
	{
		__int64 i64Key = setVisible[k];
		if (m_mapTarget.find(i64Key) != m_mapTarget.end())
			continue;
		int nX = (int)i64Key;
		int nY = (int)(i64Key >> 32);
		if (setRelease.empty())
		{
			ITargetBmp* pTarget = ::TargetBmpCreate(m_nBlockSize, m_nBlockSize);
			this->RenderBlock(pTarget, nX, nY);
			m_mapTarget[i64Key] = pTarget;
		}
		else
		{
			ITargetBmp* pTarget = setRelease[0];
			this->RenderBlock(pTarget, nX, nY);
			m_mapTarget[i64Key] = pTarget;
			setRelease.erase(setRelease.begin());
		}
	}
	for (DWORD k = 0; k < setRelease.size(); k++)
		setRelease[k]->Release();
}
