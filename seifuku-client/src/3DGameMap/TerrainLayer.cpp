#include "TerrainLayer.h"
#include "3DGameMap.h"
#include "Hero.h"
#include "GameDataSet.h"
CTerrainLayer::CTerrainLayer()
{
	m_setPassage.clear();
	m_setRegion.clear();
}
CTerrainLayer::~CTerrainLayer()
{
	this->ClearPassage();
	this->ClearRegion();
}
CTerrainLayer* CTerrainLayer::CreateNew()
{
	CTerrainLayer* pLayer = new CTerrainLayer;
	return  pLayer;
}
void CTerrainLayer::Show(CMyPos posShow)
{
	m_bmpPuzzle.Show(posShow.x, posShow.y);
}
void CTerrainLayer::Process(void* pInfo)
{
	int nViewportX, nViewportY;
	g_objGameMap.GetBgViewport(nViewportX, nViewportY);
	m_bmpPuzzle.PreLoad(nViewportX, nViewportY);
}
void CTerrainLayer::ProcessPuzzle()
{
	m_bmpPuzzle.Process();
}
int  CTerrainLayer::GetType()
{
	return LAYER_TERRAIN;
}
void CTerrainLayer::LoadPuzzle(char* pszFile)
{
	if(!pszFile)
		return;
	strcpy(m_szPuzzleFile, pszFile);
	m_bmpPuzzle.Load(pszFile);
}
void CTerrainLayer::ClearPassage()
{
	int nAmount =  m_setPassage.size();
	for(int i = 0; i < nAmount; i++)
	{
		PassageInfo* pInfo = m_setPassage[i];
		SAFE_DELETE(pInfo);
	}
	m_setPassage.clear();
}
void CTerrainLayer::AddPassage(CMyPos posMap, int nIndex)
{
	int nAmount = m_setPassage.size();
	for(int i = 0; i < nAmount; i++)
	{
		PassageInfo* pInfo = m_setPassage[i];
		if(pInfo)
		{
			if(pInfo->nPosX == posMap.x && pInfo->nPosY == posMap.y)
			{
				pInfo->nIndex = nIndex;
				return;
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
int  CTerrainLayer::GetExitIndex(CMyPos posCell)
{
	int nAmount = m_setPassage.size();
	for(int i = 0; i < nAmount; i++)
	{
		PassageInfo* pInfo = m_setPassage[i];
		if(pInfo)
		{
			if(pInfo->nPosX == posCell.x && pInfo->nPosY == posCell.y)
				return pInfo->nIndex;
		}
	}
	return -1;
}
void CTerrainLayer::DelPassage(CMyPos posMap)
{
	int nAmount =  m_setPassage.size();
	for(int i = 0; i < nAmount; i++)
	{
		PassageInfo* pInfo = m_setPassage[i];
		if(pInfo)
		{
			if((pInfo->nPosX == posMap.x) && (pInfo->nPosY == posMap.y))
			{
				SAFE_DELETE(pInfo);
				m_setPassage.erase(m_setPassage.begin()+i);
				return;
			}
		}
	}
}
void CTerrainLayer::LoadDataPassage(FILE* fp)
{
	if(!fp)
		return;
	int nAmount;
	fread(&nAmount, sizeof(int), 1, fp);
	for(int i = 0; i < nAmount; i++)
	{
		PassageInfo info;
		fread(&info, sizeof(PassageInfo), 1, fp);
		CMyPos posPassage;
		posPassage.x = info.nPosX;
		posPassage.y = info.nPosY;
		this->AddPassage(posPassage, info.nIndex);
		CMyPos posWorld;
		g_objGameMap.Cell2World(posPassage.x, posPassage.y, posWorld.x, posWorld.y);
		g_objGameMap.Add3DMapEffect(posWorld, "Exit");
	}
}
void CTerrainLayer::LoadTextPassage(FILE* fp)
{
    if(!fp)
        return;
	int nAmount;
	fscanf(fp, "PassageAmount=%d\n", &nAmount);
	for(int i = 0; i < nAmount; i++)
	{
		CMyPos posPassage;
		int nPassageIndex;
		int nIndex;
		fscanf(fp, "Passage%d=[%d,%d,%d]\n", &nIndex, &posPassage.x,
			&posPassage.y, &nPassageIndex);
		this->AddPassage(posPassage, nPassageIndex);
	}
}
void CTerrainLayer::SaveTextPassage(FILE* fp)
{
	if(!fp)
		return;
	int nAmount = m_setPassage.size();
	fprintf(fp, "PassageAmount=%d\n", nAmount);
	for(int i = 0; i < nAmount; i++)
	{
		PassageInfo* pInfo = m_setPassage[i];
		if(pInfo)
		{
			fprintf(fp, "Passage%d=[%d,%d,%d]\n", i, pInfo->nPosX, pInfo->nPosY, pInfo->nIndex);
		}
	}
}
void CTerrainLayer::GetPuzzleSize(CMySize& infoSize)
{
	infoSize.iWidth = m_bmpPuzzle.GetWidth();
	infoSize.iHeight = m_bmpPuzzle.GetHeight();
}
void CTerrainLayer::SaveDataPassage(FILE* fp)
{
	if(!fp)
		return;
	int nAmount = m_setPassage.size();
	fwrite(&nAmount, sizeof(int), 1, fp);
	for(int i = 0; i < nAmount; i++)
	{

		PassageInfo* pInfo = m_setPassage[i];
		if(pInfo)
		{
			fwrite(pInfo, sizeof(PassageInfo), 1, fp);
		}
	}
}
int CTerrainLayer::GetPuzzleGrideSize()
{
	return m_bmpPuzzle.GetGrideSize();
}
void CTerrainLayer::ClearRegion()
{
	int nAmount = m_setRegion.size();
	for(int i = 0; i < nAmount; i++)
	{
		C2DMapRegion* pRegion = m_setRegion[i];
		SAFE_DELETE(pRegion);
	}
	m_setRegion.clear();
}
void CTerrainLayer::AddRegion(C2DMapRegion* pRegion)
{
	m_setRegion.push_back(pRegion);
}
int  CTerrainLayer::GetRegionAmount()
{
	return m_setRegion.size();
}
C2DMapRegion* CTerrainLayer::GetRegionByINdex(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_setRegion.size())
		return NULL;
	return m_setRegion[nIndex];
}
C2DMapRegion* CTerrainLayer::GetRegion(CMyPos posRegion, int nType)
{
	int nAmount = m_setRegion.size();
	for(int i = 0; i < nAmount; i ++)
	{
		C2DMapRegion* pRegion = m_setRegion[i];
		if(pRegion)
		{
			RegionInfo* pInfo = pRegion->GetInfo();
			if(pInfo->m_posCell.x == posRegion.x && pInfo->m_posCell.y == posRegion.y && pInfo->m_dwType == nType)
			{
				return 	pRegion;
			}
		}
	}
	return NULL;
}
void CTerrainLayer::DelRegion(CMyPos posRegion, int nType)
{
	int nAmount = m_setRegion.size();
	for(int i = 0; i < nAmount; i ++)
	{
		C2DMapRegion* pRegion = m_setRegion[i];
		if(pRegion)
		{
			RegionInfo* pInfo = pRegion->GetInfo();
			if(pInfo->m_posCell.x == posRegion.x && pInfo->m_posCell.y == posRegion.y && pInfo->m_dwType == nType)
			{
				SAFE_DELETE(pRegion);
				m_setRegion.erase(m_setRegion.begin()+i);
				return;
			}
		}

	}
}
void CTerrainLayer::ShowRegion(int nType)
{
	int nAmount = m_setRegion.size();
	for(int i = 0; i < nAmount; i ++)
	{
		C2DMapRegion* pRegion = m_setRegion[i];
		RegionInfo* pInfo = pRegion->GetInfo();
		if(pRegion && pInfo && pInfo->m_dwType == nType)
		{
			pRegion->Show(NULL);
		}
	}
}
void CTerrainLayer::LoadDataRegion(FILE* fp)
{
	MYASSERT(fp);
	DWORD dwAmount = 0;
	fread(&dwAmount, sizeof(DWORD), 1, fp);
	for(int i = 0; i < dwAmount; i ++)
	{
		RegionInfo infoRegion;
		fread(&infoRegion, sizeof(RegionInfo), 1, fp);
		C2DMapRegion* pRegion = C2DMapRegion::CreateNew(&infoRegion);
		this->AddRegion(pRegion);
	}
}
void CTerrainLayer::LoadTextRegion(FILE* fp)
{
	MYASSERT(fp);
	int nAmount;
	if( 1 != fscanf(fp, "RegionAmount=%d\n", &nAmount))
		return;
	for(int i = 0; i < nAmount; i++)
	{
		RegionInfo infoRegion;
		fprintf(fp, "Region%d=[%u,%u,%u,%u,%u,%u,%u,%u,%u,%s]\n", i,
			&infoRegion.m_posCell.x, &infoRegion.m_posCell.y,
			&infoRegion.m_dwCx, &infoRegion.m_dwCy, &infoRegion.m_dwType, &infoRegion.m_nNColor,
			&infoRegion.m_nNShowType, &infoRegion.m_nDColor, &infoRegion.m_nDShowPos, infoRegion.m_szString);
		C2DMapRegion* pRegion = C2DMapRegion::CreateNew(&infoRegion);
		this->AddRegion(pRegion);

	}
}
void CTerrainLayer::SaveDataRegion(FILE* fp)
{
	MYASSERT(fp);
	DWORD dwAmount = m_setRegion.size();
	fwrite(&dwAmount, sizeof(DWORD), 1, fp);
	for(int i = 0; i < dwAmount; i ++)
	{
		C2DMapRegion* pRegion = m_setRegion[i];
		if(pRegion)
		{
			RegionInfo* pInfo = pRegion->GetInfo();
			fwrite(pInfo, sizeof(RegionInfo), 1, fp);
		}
	}
}
void CTerrainLayer::SaveTextRegion(FILE* fp)
{
	MYASSERT(fp);
	int nAmount = m_setRegion.size();
	fprintf(fp, "RegionAmount=%d\n", nAmount);
	for(int i = 0; i < nAmount; i++)
	{
		PassageInfo* pInfo = m_setPassage[i];
		C2DMapRegion* pRegion = m_setRegion[i];
		if(pRegion)
		{
			RegionInfo* pInfo = pRegion->GetInfo();
			fprintf(fp, "Region%d=[%u,%u,%u,%u,%u,%u,%u,%u,%u,%s]\n", i,
				pInfo->m_posCell.x, pInfo->m_posCell.y,
					pInfo->m_dwCx, pInfo->m_dwCy, pInfo->m_dwType, pInfo->m_nNColor,
						pInfo->m_nNShowType, pInfo->m_nDColor, pInfo->m_nDShowPos, pInfo->m_szString);
		}
	}
}

CTerrainPuzzle::CTerrainPuzzle()
{
	strcpy(m_szFile, "");
	m_bTqTerrain		= false;
	m_setCartridge		= NULL;
	m_setBlastTrace		= NULL;
	for(int i = 0; i < 1024; i++)
	{
		m_setCartridgeJitter[i].cIndex		= (char) GetRandom(0x10);
		m_setCartridgeJitter[i].cOffsetX	= (char)(GetRandom(0x10) - 0x20);
		m_setCartridgeJitter[i].cOffsetY	= (char)(GetRandom(0x10) - 0x20);
		m_setBlastJitter[i].cIndex		= (char) GetRandom(0x20);
		m_setBlastJitter[i].cOffsetX	= (char)(GetRandom(0x20) - 0x40);
		m_setBlastJitter[i].cOffsetY	= (char)(GetRandom(0x20) - 0x40);
	}
	m_pMultiBmp = ::MultiBmpCreate();
}
CTerrainPuzzle::~CTerrainPuzzle()
{
	if(m_setCartridge)
		delete [] m_setCartridge;
	if(m_setBlastTrace)
		delete [] m_setBlastTrace;
	if(m_pMultiBmp)
		m_pMultiBmp->Release();
}
BOOL CTerrainPuzzle::Load(char* pszFile)
{
	if(!pszFile)
		return false;
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szName[_MAX_FNAME];
	char szExt[_MAX_EXT];
	_splitpath(pszFile, szDrive, szDir, szName, szExt);
	if(_stricmp(szExt, ".pul") != 0)
	{
		m_bTqTerrain = true;
		m_objTerrain.Load(pszFile);
	}
	else
	{
		m_bTqTerrain = false;
		m_bmpPuzzle.Load(pszFile);
	}
	strcpy(m_szFile, pszFile);
	this->Reset();
	return true;
}
void CTerrainPuzzle::Show(int nPosX, int nPosY)
{
	if(m_bTqTerrain)
		m_objTerrain.Show(-nPosX, -nPosY);
	else
		m_bmpPuzzle.Show(-nPosX, -nPosY, false, false);
}
int CTerrainPuzzle::GetWidth()
{
	if(m_bTqTerrain)
		return m_objTerrain.GetWidth();
	CMySize infoSize;
	m_bmpPuzzle.GetSize(infoSize);
	return infoSize.iWidth;
}
int CTerrainPuzzle::GetHeight()
{
	if(m_bTqTerrain)
		return m_objTerrain.GetHeight();
	CMySize infoSize;
	m_bmpPuzzle.GetSize(infoSize);
	return infoSize.iHeight;
}
void CTerrainPuzzle::SetGrideSize(int nSize)
{
	m_bmpPuzzle.SetGrideSize(nSize);
}
void CTerrainPuzzle::SetPulGrideSize(int nSize)
{
	if(!m_bTqTerrain)
		m_bmpPuzzle.SetGrideSizeOnly(nSize);
}
void CTerrainPuzzle::AddBlastTrace(int nPosX, int nPosY)
{
	int nBlockAmountX = ((DWORD)this->GetWidth() >> 9) + 1;
	int nBlockAmountY = ((DWORD)this->GetHeight() >> 9) + 1;
	int nBlockX = (DWORD)nPosX >> 9;
	int nBlockY = (DWORD)nPosY >> 9;
	if(nBlockX < 0 || nBlockX >= nBlockAmountX || nBlockY < 0 || nBlockY >= nBlockAmountY)
		return;
	VECTOR_TRACE& setTrace = m_setBlastTrace[nBlockY * nBlockAmountX + nBlockX];
	if((WORD)setTrace.size() >= 0x400)
		return;
	TraceInfo info;
	info.nPosX = (short)(nPosX - nBlockX * 512);
	info.nPosY = (short)(nPosY - nBlockY * 512);
	if(setTrace.size() > 0)
	{
		TraceInfo infoLast = setTrace[setTrace.size() - 1];
		info.dwTime = ::TimeGet();
		if(info.dwTime <= infoLast.dwTime)
			info.dwTime ++;
	}
	else
		info.dwTime = ::TimeGet();
	setTrace.push_back(info);
}
void CTerrainPuzzle::AddCartridge(int nPosX, int nPosY)
{
	int nBlockAmountX = ((DWORD)this->GetWidth() >> 9) + 1;
	int nBlockAmountY = ((DWORD)this->GetHeight() >> 9) + 1;
	int nBlockX = (DWORD)nPosX >> 9;
	int nBlockY = (DWORD)nPosY >> 9;
	if(nBlockX < 0 || nBlockX >= nBlockAmountX || nBlockY < 0 || nBlockY >= nBlockAmountY)
		return;
	VECTOR_TRACE& setTrace = m_setCartridge[nBlockY * nBlockAmountX + nBlockX];
	if((WORD)setTrace.size() >= 0x400)
		return;
	TraceInfo info;
	info.nPosX = (short)(nPosX - nBlockX * 512);
	info.nPosY = (short)(nPosY - nBlockY * 512);
	if(setTrace.size() > 0)
	{
		TraceInfo infoLast = setTrace[setTrace.size() - 1];
		info.dwTime = ::TimeGet();
		if(info.dwTime <= infoLast.dwTime)
			info.dwTime ++;
	}
	else
		info.dwTime = ::TimeGet();
	setTrace.push_back(info);
}
void CTerrainPuzzle::SetTraceLife(DWORD dwLife)
{
	m_dwTraceLife = dwLife;
}
int CTerrainPuzzle::GetGrideSize()
{
	if(m_bTqTerrain)
		return 0x80;
	return m_bmpPuzzle.GetGrideSize();
}
void CTerrainPuzzle::PreLoad(int nPosX, int nPosY)
{
	if(m_bTqTerrain)
		m_objTerrain.PreLoad(-nPosX, -nPosY);
}
void CTerrainPuzzle::Process()
{
	if(m_bTqTerrain)
		m_objTerrain.Process();
}
void CTerrainPuzzle::Reset()
{
	if(m_setCartridge)
		delete [] m_setCartridge;
	if(m_setBlastTrace)
		delete [] m_setBlastTrace;
	int nBlockAmountX = ((DWORD)this->GetWidth() >> 9) + 1;
	int nBlockAmountY = ((DWORD)this->GetHeight() >> 9) + 1;
	int nAmount = nBlockAmountY * nBlockAmountX;
	m_setCartridge = new VECTOR_TRACE[nAmount];
	m_setBlastTrace = new VECTOR_TRACE[nAmount];
}
int CTerrainPuzzle::GetRandom(int nRange)
{
	static __int64 s_qwSeed = 0x2537;
	s_qwSeed = (s_qwSeed * 0x08088405 + 1) % 4294967296i64;
	return (int)((double)s_qwSeed * (1.0 / 4294967296.0) * nRange);
}
void CTerrainPuzzle::ShowBlastTrace(int nPosX, int nPosY)
{
	if(!m_pMultiBmp)
		return;
	IAni* pAni = GameDataSetQuery()->GetDataAni("ani/common.ani", "BlastTrace", EXIGENCE_NORMAL, 30000);
	if(!pAni)
		return;
	CMyBitmap* pBmp = pAni->GetFrame(0);
	if(!pBmp)
		return;
	if(!m_setBlastTrace)
		return;
	int nBlockAmountX = ((DWORD)this->GetWidth() >> 9) + 1;
	int nBlockAmountY = ((DWORD)this->GetHeight() >> 9) + 1;
	int nBlockX = nPosX / 512;
	int nBlockY = nPosY / 512;
	int nScreenX = ::Double2Int((double)(nBlockX * 512 - nPosX));
	int nScreenY = ::Double2Int((double)(nBlockY * 512 - nPosY));
	for(int nY = nBlockY, nShowY = nScreenY; nShowY < nScreenY + CMyBitmap::GetScreenHeight() + 512; nY++, nShowY += 512)
	{
		for(int nX = nBlockX, nShowX = nScreenX; nShowX < nScreenX + CMyBitmap::GetScreenWidth() + 512; nX++, nShowX += 512)
		{
			if(nX < 0 || nY < 0 || nX >= nBlockAmountX || nY >= nBlockAmountY)
				continue;
			VECTOR_TRACE& setTrace = m_setBlastTrace[nY * nBlockAmountX + nX];
			m_pMultiBmp->Clear();
			int nRunning = 0;
			for(VECTOR_TRACE::iterator it = setTrace.end(); it != setTrace.begin(); )
			{
				nRunning--;
				it--;
				TraceInfo info = *it;
				if(m_dwTraceLife && ::TimeGet() - info.dwTime > m_dwTraceLife)
				{
					it = setTrace.erase(it);
					continue;
				}
				int nWidth, nHeight;
				pBmp->GetSize(nWidth, nHeight);
				if(info.dwTime)
					nRunning = info.dwTime % 1024;
				const TraceJitter& jitter = m_setBlastJitter[nRunning];
				int nCell = jitter.cIndex % 16;
				float fU = (float)(nCell % 4) * 0.25f;
				float fV = (float)(nCell / 4) * 0.25f;
				const TraceJitter& jitterOffset = m_setBlastJitter[info.dwTime % 1024];
				m_pMultiBmp->Add(nShowX + info.nPosX + jitterOffset.cOffsetX, nShowY + info.nPosY + jitterOffset.cOffsetY, nWidth / 4, nHeight / 4,
					fU, fV, fU + 0.25f, fV, fU + 0.25f, fV + 0.25f, fU, fV + 0.25f);
				if((DWORD)nRunning > 0x400)
					break;
			}
			m_pMultiBmp->Show(pAni->GetFrame(0));
		}
	}
}
void CTerrainPuzzle::ShowCartridge(int nPosX, int nPosY)
{
	if(!m_pMultiBmp)
		return;
	IAni* pAni = GameDataSetQuery()->GetDataAni("ani/common.ani", "Cartridge_Case", EXIGENCE_NORMAL, 30000);
	if(!pAni)
		return;
	CMyBitmap* pBmp = pAni->GetFrame(0);
	if(!pBmp)
		return;
	if(!m_setCartridge)
		return;
	int nBlockAmountX = ((DWORD)this->GetWidth() >> 9) + 1;
	int nBlockAmountY = ((DWORD)this->GetHeight() >> 9) + 1;
	int nBlockX = nPosX / 512;
	int nBlockY = nPosY / 512;
	int nScreenX = ::Double2Int((double)(nBlockX * 512 - nPosX));
	int nScreenY = ::Double2Int((double)(nBlockY * 512 - nPosY));
	for(int nY = nBlockY, nShowY = nScreenY; nShowY < nScreenY + CMyBitmap::GetScreenHeight() + 512; nY++, nShowY += 512)
	{
		for(int nX = nBlockX, nShowX = nScreenX; nShowX < nScreenX + CMyBitmap::GetScreenWidth() + 512; nX++, nShowX += 512)
		{
			if(nX < 0 || nY < 0 || nX >= nBlockAmountX || nY >= nBlockAmountY)
				continue;
			VECTOR_TRACE& setTrace = m_setCartridge[nY * nBlockAmountX + nX];
			int nRunning = m_setCartridge[0].size();
			m_pMultiBmp->Clear();
			for(VECTOR_TRACE::iterator it = setTrace.end(); it != setTrace.begin(); )
			{
				nRunning--;
				it--;
				TraceInfo info = *it;
				if(m_dwTraceLife && ::TimeGet() - info.dwTime > m_dwTraceLife)
				{
					it = setTrace.erase(it);
					continue;
				}
				int nWidth, nHeight;
				pBmp->GetSize(nWidth, nHeight);
				if(m_dwTraceLife)
					nRunning = info.dwTime % 1024;
				const TraceJitter& jitter = m_setCartridgeJitter[nRunning];
				int nCell = jitter.cIndex % 16;
				float fU = (float)(nCell % 4) * 0.25f;
				float fV = (float)(nCell / 4) * 0.25f;
				m_pMultiBmp->Add(nShowX + info.nPosX + jitter.cOffsetX, nShowY + info.nPosY + jitter.cOffsetY, nWidth / 4, nHeight / 4,
					fU, fV, fU + 0.25f, fV, fU + 0.25f, fV + 0.25f, fU, fV + 0.25f);
				if((DWORD)nRunning > 0x400)
					break;
			}
			m_pMultiBmp->Show(pAni->GetFrame(0));
		}
	}
}
