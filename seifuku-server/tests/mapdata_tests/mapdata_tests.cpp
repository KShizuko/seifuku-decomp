#include "CppUnitTest.h"
#include <stddef.h>
#include "typedef.h"
#include "MapData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(CCell) == 6, "");
static_assert(sizeof(CLayer) == 6, "");
static_assert(sizeof(PassageInfo) == 12, "");
static_assert(sizeof(NodeInfo) == 0x20, "");
static_assert(sizeof(PASSPATH0) == 0x18, "");
static_assert(offsetof(PASSPATH0, pPath) == 0x14, "");
static_assert(offsetof(NodeInfo, nDir) == 0x14, "");

static const char* Fixture(const char* pszName)
{
	static char buf[MAX_PATH];
	strcpy(buf, __FILE__);
	char* p = strrchr(buf, '\\');
	strcpy(p + 1, "fixtures\\");
	strcat(buf, pszName);
	return buf;
}

class CFixtureDir
{
public:
	CFixtureDir()	{ ::GetCurrentDirectory(MAX_PATH, m_szOld); ::SetCurrentDirectory(Fixture("")); }
	~CFixtureDir()	{ ::SetCurrentDirectory(m_szOld); }
private:
	char	m_szOld[MAX_PATH];
};

static shared_ptr<CLayer> NewLayer(int nAlt, int nMask, int nTerrian)
{
	shared_ptr<CLayer> p = CLayer::CreateNew();
	p->nAltitude = nAlt;
	p->dwMask = nMask;
	p->nTerrian = nTerrian;
	return p;
}

TEST_CLASS(LayerTests)
{
public:
	TEST_METHOD(ANewLayerIsZero)
	{
		shared_ptr<CLayer> p = CLayer::CreateNew();
		Assert::IsTrue(p.get() != NULL);
		Assert::AreEqual(0, (int)p->nTerrian);
		Assert::AreEqual(0, (int)p->nAltitude);
		Assert::AreEqual(0, (int)p->dwMask);
	}
};

TEST_CLASS(CellTests)
{
public:
	TEST_METHOD(CreateKeepsAltitudeAndMask)
	{
		DEQUE2_LAYER set;
		CCell cell;
		Assert::IsTrue(cell.Create(12, 0));
		Assert::AreEqual(12, cell.GetFloorAlt(set));
		Assert::AreEqual(12, cell.GetSurfaceAlt());
		Assert::AreEqual(0u, (unsigned)cell.GetFloorMask(set));
		Assert::AreEqual(DEFAULT_FLOORATTR, cell.GetFloorAttr(set));
		Assert::AreEqual(0, cell.GetRoleAmount());
		Assert::IsFalse(cell.BeSearched() != 0);

		cell.Create(-3, 5);
		Assert::AreEqual(-3, cell.GetFloorAlt(set));
		Assert::AreEqual(1u, (unsigned)cell.GetFloorMask(set));
		cell.FullMask();
		Assert::AreEqual(1u, (unsigned)cell.GetFloorMask(set));
	}

	TEST_METHOD(RoleCountClearsTheSearchFlag)
	{
		CCell cell;
		cell.Create(0, 0);
		cell.IncRole(2);
		cell.SetSearchFlag(true);
		Assert::IsTrue(cell.BeSearched() != 0);
		Assert::AreEqual(2, cell.GetRoleAmount());
		cell.IncRole();
		Assert::AreEqual(3, cell.GetRoleAmount());
		Assert::IsFalse(cell.BeSearched() != 0);
		cell.SetSearchFlag(true);
		cell.DecRole(3);
		Assert::AreEqual(0, cell.GetRoleAmount());
		Assert::IsFalse(cell.BeSearched() != 0);
		cell.SetSearchFlag(true);
		cell.SetSearchFlag(false);
		Assert::IsFalse(cell.BeSearched() != 0);
	}

	TEST_METHOD(TheTopLayerDecides)
	{
		DEQUE2_LAYER set;
		CCell cell;
		cell.Create(1, 0);
		cell.AddLayer(set, NewLayer(7, 1, 2));
		Assert::AreEqual((size_t)1, set.size());
		Assert::AreEqual(7, cell.GetFloorAlt(set));
		Assert::AreEqual(1u, (unsigned)cell.GetFloorMask(set));
		Assert::AreEqual(2, cell.GetFloorAttr(set));
		Assert::AreEqual(1, cell.GetSurfaceAlt());
		cell.AddLayer(set, NewLayer(9, 0, 3));
		Assert::AreEqual((size_t)1, set.size());
		Assert::AreEqual(9, cell.GetFloorAlt(set));
		Assert::AreEqual(0u, (unsigned)cell.GetFloorMask(set));
		Assert::IsTrue(cell.DelLayer(set, 1));
		Assert::AreEqual(9, cell.GetFloorAlt(set));
		Assert::IsTrue(cell.DelLayer(set));
		Assert::AreEqual(1, cell.GetFloorAlt(set));
		Assert::AreEqual(0, cell.GetFloorAttr(set));
	}

	TEST_METHOD(AnEmptySlotIsReusedAndTheLayerFreed)
	{
		DEQUE2_LAYER set;
		CCell a, b, c;
		a.Create(0, 0); b.Create(0, 0); c.Create(0, 0);
		shared_ptr<CLayer> p = NewLayer(4, 0, 0);
		weak_ptr<CLayer> w = p;
		a.AddLayer(set, p);
		p.reset();
		b.AddLayer(set, NewLayer(5, 0, 0));
		Assert::AreEqual((size_t)2, set.size());
		Assert::IsFalse(w.expired());
		Assert::IsTrue(a.DelLayer(set));
		Assert::IsTrue(w.expired());
		Assert::AreEqual(0, a.GetFloorAlt(set));
		c.AddLayer(set, NewLayer(6, 0, 0));
		Assert::AreEqual((size_t)2, set.size());
		Assert::AreEqual(6, c.GetFloorAlt(set));
		Assert::AreEqual(5, b.GetFloorAlt(set));
	}

	TEST_METHOD(ANullLayerIsIgnored)
	{
		DEQUE2_LAYER set;
		CCell cell;
		cell.Create(2, 0);
		cell.AddLayer(set, shared_ptr<CLayer>());
		Assert::AreEqual((size_t)0, set.size());
		Assert::AreEqual(2, cell.GetFloorAlt(set));
	}
};

TEST_CLASS(TerrainObjTests)
{
public:
	TEST_METHOD(ReadsThePartsAndTheirLayers)
	{
		CTerrainObj* pObj = CTerrainObj::CreateNew(Fixture("map\\obj.scene"), 5);
		Assert::IsTrue(pObj != NULL);
		Assert::AreEqual(5u, (unsigned)pObj->GetOwnerID());
		Assert::AreEqual(1, pObj->GetPartAmount());
		Assert::IsTrue(pObj->GetPartByIndex(1) == NULL);
		Assert::IsTrue(pObj->GetPartByIndex(-1) == NULL);
		CTerrainObjPart* pPart = pObj->GetPartByIndex(0);
		Assert::AreEqual(2, (int)pPart->GetBase().cx);
		Assert::AreEqual(1, (int)pPart->GetBase().cy);
		Assert::AreEqual(100u, (unsigned)pPart->GetAniInterval());
		Assert::AreEqual(10, pPart->GetThick());
		Assert::AreEqual("obj", (const char*)pPart->GetAniTitle());
		CLayer* pLayer = pPart->GetLayer(0, 0);
		Assert::AreEqual(1, (int)pLayer->dwMask);
		Assert::AreEqual(9, (int)pLayer->nTerrian);
		Assert::AreEqual(3, (int)pLayer->nAltitude);
		pLayer = pPart->GetLayer(1, 0);
		Assert::AreEqual(0, (int)pLayer->dwMask);
		Assert::AreEqual(4, (int)pLayer->nTerrian);
		Assert::AreEqual(6, (int)pLayer->nAltitude);
		Assert::IsTrue(pPart->GetLayer(0, 1) == NULL);
		Assert::IsTrue(pPart->GetLayer(-1, 0) == NULL);

		POINT pos = { 5, 6 };
		pObj->SetPos(pos);
		Assert::AreEqual(5, (int)pPart->GetPos().x);
		Assert::AreEqual(6, (int)pPart->GetPos().y);
		Assert::AreEqual((int)MAP_TERRAIN, pObj->GetObjType());
		Assert::AreEqual((int)MAP_TERRAIN_PART, pPart->GetObjType());
		delete pObj;
	}
};

TEST_CLASS(MapDataTests)
{
public:
	TEST_METHOD(LoadsTheMapNamedInGameMapDat)
	{
		CFixtureDir dir;
		IMapData* pMap = IMapData::CreateNew(1001);
		Assert::IsTrue(pMap != NULL);
		Assert::AreEqual(8, pMap->GetMapWidth());
		Assert::AreEqual(8, pMap->GetMapHeight());
		Assert::AreEqual(12, pMap->GetFloorAlt(1, 1));
		Assert::AreEqual(12, pMap->GetSurfaceAlt(1, 1));
		Assert::AreEqual(-3, pMap->GetFloorAlt(1, 2));
		Assert::AreEqual(1u, (unsigned)pMap->GetFloorMask(3, 0));
		Assert::AreEqual(1u, (unsigned)pMap->GetFloorMask(3, 5));
		Assert::AreEqual(0u, (unsigned)pMap->GetFloorMask(3, 6));
		Assert::AreEqual(3, pMap->GetPassage(6, 6));
		Assert::AreEqual(PASSAGE_NONE, pMap->GetPassage(1, 1));
		pMap->Release();
	}

	TEST_METHOD(PlacesTheTerrainObjectOfTheMap)
	{
		CFixtureDir dir;
		IMapData* pMap = IMapData::CreateNew(1001);
		Assert::IsTrue(pMap != NULL);
		Assert::AreEqual(1u, (unsigned)pMap->GetFloorMask(5, 5));
		Assert::AreEqual(9, pMap->GetFloorAttr(5, 5));
		Assert::AreEqual(3, pMap->GetFloorAlt(5, 5));
		Assert::AreEqual(0, pMap->GetSurfaceAlt(5, 5));
		Assert::AreEqual(0u, (unsigned)pMap->GetFloorMask(4, 5));
		Assert::AreEqual(4, pMap->GetFloorAttr(4, 5));
		Assert::AreEqual(6, pMap->GetFloorAlt(4, 5));
		Assert::AreEqual(0, pMap->GetFloorAttr(6, 5));
		pMap->Release();
	}

	TEST_METHOD(TerrainItemsStackAndGo)
	{
		CFixtureDir dir;
		IMapData* pMap = IMapData::CreateNew(1001);
		Assert::IsTrue(pMap != NULL);
		Assert::IsTrue(pMap->AddTerrainItem(77, 2, 7, 123));
		Assert::AreEqual(9, pMap->GetFloorAttr(2, 7));
		Assert::AreEqual(1u, (unsigned)pMap->GetFloorMask(2, 7));
		Assert::AreEqual(4, pMap->GetFloorAttr(1, 7));
		Assert::IsTrue(pMap->AddTerrainItem(78, 5, 5, 123));
		Assert::AreEqual(6, pMap->GetFloorAlt(5, 5));
		Assert::AreEqual(12, pMap->GetFloorAlt(4, 5));
		Assert::IsTrue(pMap->DelTerrainItem(78));
		Assert::AreEqual(3, pMap->GetFloorAlt(5, 5));
		Assert::AreEqual(6, pMap->GetFloorAlt(4, 5));
		Assert::IsTrue(pMap->DelTerrainItem(77));
		Assert::AreEqual(0, pMap->GetFloorAttr(2, 7));
		Assert::AreEqual(0u, (unsigned)pMap->GetFloorMask(2, 7));
		Assert::IsTrue(pMap->DelTerrainItem(79));
		pMap->Release();
	}

	TEST_METHOD(RolesAreCountedPerCell)
	{
		CFixtureDir dir;
		IMapData* pMap = IMapData::CreateNew(1001);
		Assert::IsTrue(pMap != NULL);
		pMap->IncRole(2, 2);
		pMap->IncRole(2, 2, 3);
		Assert::AreEqual(4, pMap->GetRoleAmount(2, 2));
		pMap->DecRole(2, 2);
		Assert::AreEqual(3, pMap->GetRoleAmount(2, 2));
		Assert::AreEqual(0, pMap->GetRoleAmount(2, 3));
		Assert::AreEqual(1u, (unsigned)pMap->GetFloorMask(-1, 0));
		Assert::AreEqual(0, pMap->GetFloorAlt(8, 0));
		pMap->Release();
	}

	TEST_METHOD(ThePuzzleMasksTheCorners)
	{
		CFixtureDir dir;
		IMapData* pMap = IMapData::CreateNew(1003);
		Assert::IsTrue(pMap != NULL);
		static const int s_nMasked[][2] = { {0,0}, {7,7}, {0,7}, {1,7}, {2,7}, {7,0}, {6,0}, {5,0}, {0,6}, {1,6}, {7,1}, {6,1},
											{0,5}, {7,2} };
		int nMasked = 0;
		for(int y = 0; y < 8; y++)
			for(int x = 0; x < 8; x++)
				nMasked += pMap->GetFloorMask(x, y) != 0;
		Assert::AreEqual(14, nMasked);
		for(int i = 0; i < 14; i++)
			Assert::AreEqual(1u, (unsigned)pMap->GetFloorMask(s_nMasked[i][0], s_nMasked[i][1]));
		pMap->Release();
	}

	TEST_METHOD(MapsThatAreNotLoaded)
	{
		CFixtureDir dir;
		Assert::IsTrue(IMapData::CreateNew(1002) == NULL);
		Assert::IsTrue(IMapData::CreateNew(1004) == NULL);
		Assert::IsTrue(IMapData::CreateNew(1005) == NULL);
		Assert::IsTrue(IMapData::CreateNew(ID_NONE) == NULL);
	}

	TEST_METHOD(FindPathGoesRoundTheWall)
	{
		CFixtureDir dir;
		IMapData* pMap = IMapData::CreateNew(1001);
		Assert::IsTrue(pMap != NULL);
		static const int nArrayX[8] = {0, -1, -1, -1,  0,  1, 1, 1};
		static const int nArrayY[8] = {1,  1,  0, -1, -1, -1, 0, 1};
		char buf[sizeof(PASSPATH0) + 64];
		PASSPATH0* pPath = (PASSPATH0*)buf;
		POINT posFrom = { 1, 4 }, posTo = { 5, 4 };
		Assert::IsTrue(pMap->FindPath(pPath, sizeof(buf), posFrom, posTo, NULL));
		Assert::AreEqual(1, (int)pPath->posStart.x);
		Assert::AreEqual(4, (int)pPath->posStart.y);
		Assert::AreEqual(5, (int)pPath->posStop.x);
		Assert::AreEqual(4, (int)pPath->posStop.y);
		Assert::IsTrue(pPath->nSize > 0);
		int x = posFrom.x, y = posFrom.y;
		for(int i = 0; i < pPath->nSize; i++)
		{
			int nDir = pPath->pPath[i];
			Assert::IsTrue(nDir >= 0 && nDir < 8);
			x += nArrayX[nDir];
			y += nArrayY[nDir];
			Assert::AreEqual(0u, (unsigned)pMap->GetFloorMask(x, y));
		}
		Assert::AreEqual(5, x);
		Assert::AreEqual(4, y);
		pMap->Release();
	}

	TEST_METHOD(FindPathStopsAtTheBuffer)
	{
		CFixtureDir dir;
		IMapData* pMap = IMapData::CreateNew(1001);
		Assert::IsTrue(pMap != NULL);
		POINT posFrom = { 1, 4 }, posTo = { 5, 4 };
		char buf[sizeof(PASSPATH0) + 2 + 8];
		memset(buf, 0x5A, sizeof(buf));
		PASSPATH0* pPath = (PASSPATH0*)buf;
		Assert::IsTrue(pMap->FindPath(pPath, sizeof(PASSPATH0) + 2, posFrom, posTo, NULL));
		Assert::AreEqual(2, pPath->nSize);
		for(int i = sizeof(PASSPATH0) + 2; i < (int)sizeof(buf); i++)
			Assert::AreEqual(0x5A, (int)(unsigned char)buf[i]);

		memset(buf, 0x5A, sizeof(buf));
		Assert::IsTrue(pMap->FindPath(pPath, 20, posFrom, posTo, NULL));
		Assert::AreEqual(0, pPath->nSize);
		for(int i = 20; i < (int)sizeof(buf); i++)
			Assert::AreEqual(0x5A, (int)(unsigned char)buf[i]);
		pMap->Release();
	}

	TEST_METHOD(FindPathAvoidsASteepCell)
	{
		CFixtureDir dir;
		IMapData* pMap = IMapData::CreateNew(1001);
		Assert::IsTrue(pMap != NULL);
		static const int nArrayX[8] = {0, -1, -1, -1,  0,  1, 1, 1};
		static const int nArrayY[8] = {1,  1,  0, -1, -1, -1, 0, 1};
		char buf[sizeof(PASSPATH0) + 64];
		PASSPATH0* pPath = (PASSPATH0*)buf;
		POINT posFrom = { 5, 1 }, posTo = { 7, 1 };
		Assert::IsTrue(pMap->FindPath(pPath, sizeof(buf), posFrom, posTo, NULL));
		Assert::AreEqual(7, (int)pPath->posStop.x);
		Assert::AreEqual(1, (int)pPath->posStop.y);
		int x = posFrom.x, y = posFrom.y;
		for(int i = 0; i < pPath->nSize; i++)
		{
			x += nArrayX[(int)pPath->pPath[i]];
			y += nArrayY[(int)pPath->pPath[i]];
			Assert::IsFalse(x == 6 && y == 1);
		}
		Assert::AreEqual(7, x);
		Assert::AreEqual(1, y);
		pMap->Release();
	}
};
