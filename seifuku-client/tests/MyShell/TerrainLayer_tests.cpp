#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "TerrainLayer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct TerrainLayerProbe : CTerrainLayer
{
    static size_t File() { return offsetof(TerrainLayerProbe, m_szPuzzleFile); }
    static size_t Puzzle() { return offsetof(TerrainLayerProbe, m_bmpPuzzle); }
    static size_t Passages() { return offsetof(TerrainLayerProbe, m_setPassage); }
    static size_t Regions() { return offsetof(TerrainLayerProbe, m_setRegion); }
    static size_t TqFlag() { return offsetof(CTerrainPuzzle, m_bTqTerrain); }
    static size_t PuzzleFile() { return offsetof(CTerrainPuzzle, m_szFile); }
    static size_t Terrain() { return offsetof(CTerrainPuzzle, m_objTerrain); }
    static size_t Bmp() { return offsetof(CTerrainPuzzle, m_bmpPuzzle); }
    static size_t Cartridges() { return offsetof(CTerrainPuzzle, m_setCartridge); }
    static size_t CartridgeJitter() { return offsetof(CTerrainPuzzle, m_setCartridgeJitter); }
    static size_t Blasts() { return offsetof(CTerrainPuzzle, m_setBlastTrace); }
    static size_t BlastJitter() { return offsetof(CTerrainPuzzle, m_setBlastJitter); }
    static size_t Life() { return offsetof(CTerrainPuzzle, m_dwTraceLife); }
    static size_t MultiBmp() { return offsetof(CTerrainPuzzle, m_pMultiBmp); }
};

static_assert(sizeof(PassageInfo) == 0xC, "");
static_assert(sizeof(TraceJitter) == 8, "");
static_assert(sizeof(TraceInfo) == 8, "");
static_assert(sizeof(CTerrainLayer) == 0x110 + sizeof(CTerrainPuzzle) + sizeof(DEQUE_PASSAGE) + sizeof(DEQUE_REGION), "");
static_assert(std::is_base_of<CMapLayer, CTerrainLayer>::value, "");
static_assert(std::is_polymorphic<CTerrainPuzzle>::value, "");
static_assert(std::is_same<decltype(&CTerrainLayer::CreateNew), CTerrainLayer* (*)()>::value, "");
static_assert(std::is_same<decltype(&CTerrainLayer::LoadPuzzle), void (CTerrainLayer::*)(char*)>::value, "");
static_assert(std::is_same<decltype(&CTerrainLayer::AddPassage), void (CTerrainLayer::*)(CMyPos, int)>::value, "");
static_assert(std::is_same<decltype(&CTerrainLayer::GetExitIndex), int (CTerrainLayer::*)(CMyPos)>::value, "");
static_assert(std::is_same<decltype(&CTerrainLayer::LoadDataPassage), void (CTerrainLayer::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&CTerrainLayer::AddRegion), void (CTerrainLayer::*)(C2DMapRegion*)>::value, "");
static_assert(std::is_same<decltype(&CTerrainLayer::GetRegionByINdex), C2DMapRegion* (CTerrainLayer::*)(int)>::value, "");
static_assert(std::is_same<decltype(&CTerrainPuzzle::Load), BOOL (CTerrainPuzzle::*)(char*)>::value, "");
static_assert(std::is_same<decltype(&CTerrainPuzzle::Show), void (CTerrainPuzzle::*)(int, int)>::value, "");
static_assert(std::is_same<decltype(&CTerrainPuzzle::GetRandom), int (*)(int)>::value, "");

TEST_CLASS(ClientTerrainLayerHeader)
{
public:
    TEST_METHOD(TheNameAndPuzzleFollowTheLayer)
    {
        Assert::IsTrue(TerrainLayerProbe::File() == 0xC);
        Assert::IsTrue(TerrainLayerProbe::Puzzle() == 0x110);
    }
    TEST_METHOD(TheDequesFollowThePuzzle)
    {
        const size_t d = sizeof(DEQUE_PASSAGE);
        Assert::IsTrue(TerrainLayerProbe::Passages() == 0x110 + sizeof(CTerrainPuzzle));
        Assert::IsTrue(TerrainLayerProbe::Regions() == 0x110 + sizeof(CTerrainPuzzle) + d);
    }
    TEST_METHOD(ThePuzzleObjectKeepsItsLayout)
    {
        Assert::IsTrue(TerrainLayerProbe::TqFlag() == 0x4);
        Assert::IsTrue(TerrainLayerProbe::PuzzleFile() == 0x8);
        Assert::IsTrue(TerrainLayerProbe::Terrain() == 0x10C);
        const size_t t = 0x10C + sizeof(CTqTerrain);
        Assert::IsTrue(TerrainLayerProbe::Bmp() == t);
        Assert::IsTrue(TerrainLayerProbe::Cartridges() == t + sizeof(CPuzzleBmp));
        Assert::IsTrue(TerrainLayerProbe::CartridgeJitter() == TerrainLayerProbe::Cartridges() + 4);
        Assert::IsTrue(TerrainLayerProbe::Blasts() == TerrainLayerProbe::CartridgeJitter() + 0x2000);
        Assert::IsTrue(TerrainLayerProbe::BlastJitter() == TerrainLayerProbe::Blasts() + 4);
        Assert::IsTrue(TerrainLayerProbe::Life() == TerrainLayerProbe::BlastJitter() + 0x2000);
        Assert::IsTrue(TerrainLayerProbe::MultiBmp() == TerrainLayerProbe::Life() + 4);
        Assert::IsTrue(sizeof(CTerrainPuzzle) == TerrainLayerProbe::MultiBmp() + 4);
    }
};
