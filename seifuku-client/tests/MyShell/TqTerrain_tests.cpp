#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "TqTerrain.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct TqTerrainProbe
{
    static size_t BlockSize() { return offsetof(CTqTerrain, m_nBlockSize); }
    static size_t Cols() { return offsetof(CTqTerrain, m_nCols); }
    static size_t Rows() { return offsetof(CTqTerrain, m_nRows); }
    static size_t Version() { return offsetof(CTqTerrain, m_nVersion); }
    static size_t Triangle() { return offsetof(CTqTerrain, m_pTriangle); }
    static size_t Block() { return offsetof(CTqTerrain, m_pBlock); }
    static size_t Blocks() { return offsetof(CTqTerrain, m_setBlock); }
    static size_t Vertices() { return offsetof(CTqTerrain, m_mapVertex); }
    static size_t Materials() { return offsetof(CTqTerrain, m_setMaterial); }
    static size_t Shapes() { return offsetof(CTqTerrain, m_setShape); }
    static size_t Reserved() { return offsetof(CTqTerrain, m_mapReserved); }
    static size_t Alpha() { return offsetof(CTqTerrain, m_abyAlpha); }
    static size_t AlphaLast() { return offsetof(CTqTerrain, m_abyAlphaLast); }
    static size_t Targets() { return offsetof(CTqTerrain, m_mapTarget); }
    static_assert(std::is_same<decltype(&CTqTerrain::Create), void (CTqTerrain::*)(int, int)>::value, "");
    static_assert(std::is_same<decltype(&CTqTerrain::LoadMaterial), BOOL (CTqTerrain::*)(FILE*)>::value, "");
    static_assert(std::is_same<decltype(&CTqTerrain::GetAni), IAni* (CTqTerrain::*)(WORD)>::value, "");
    static_assert(std::is_same<decltype(&CTqTerrain::GetBlockByIndex), TqLayer* (CTqTerrain::*)(int, int)>::value, "");
    static_assert(std::is_same<decltype(&CTqTerrain::GetShape), BOOL (CTqTerrain::*)(WORD, TqMaterial&)>::value, "");
    static_assert(std::is_same<decltype(&CTqTerrain::GetVertex), void (CTqTerrain::*)(int, int, TqVertex&)>::value, "");
    static_assert(std::is_same<decltype(&CTqTerrain::ShowVertexShape), void (CTqTerrain::*)(int, int, int, int, int, int)>::value, "");
    static_assert(std::is_same<decltype(&CTqTerrain::RenderBlock), void (CTqTerrain::*)(ITargetBmp*, int, int)>::value, "");
    static_assert(std::is_same<decltype(&CTqTerrain::RoundDouble), int (*)(double)>::value, "");
};

static_assert(sizeof(TqLayer) == 0x14, "");
static_assert(offsetof(TqLayer, wMaterial) == 0 && offsetof(TqLayer, dwAlphaMask) == 4 && offsetof(TqLayer, nReserved) == 8, "");
static_assert(offsetof(TqLayer, byReserved) == 0xC && offsetof(TqLayer, bHead) == 0xD && offsetof(TqLayer, pNext) == 0x10, "");
static_assert(sizeof(TqVertex) == 4, "");
static_assert(offsetof(TqMaterial, strFile) == sizeof(string) && offsetof(TqMaterial, strTitle) == 2 * sizeof(string), "");
static_assert(offsetof(TqMaterial, dwSrcBlend) == 3 * sizeof(string) && offsetof(TqMaterial, nReserved) == 3 * sizeof(string) + 0x10, "");
static_assert(sizeof(TqMaterial) == 3 * sizeof(string) + 0x14, "");
static_assert(std::is_polymorphic<CTqTerrain>::value && std::has_virtual_destructor<CTqTerrain>::value, "");
static_assert(std::is_same<decltype(&CTqTerrain::Load), BOOL (CTqTerrain::*)(const char*)>::value, "");
static_assert(std::is_same<decltype(&CTqTerrain::Show), void (CTqTerrain::*)(int, int)>::value, "");
static_assert(std::is_same<decltype(&CTqTerrain::PreLoad), void (CTqTerrain::*)(int, int)>::value, "");
static_assert(std::is_same<decltype(&CTqTerrain::Process), void (CTqTerrain::*)()>::value, "");

TEST_CLASS(ClientTqTerrainHeader)
{
public:
    TEST_METHOD(TheNumbersAndThePrimitivesLeadTheObject)
    {
        Assert::IsTrue(TqTerrainProbe::BlockSize() == 0x4);
        Assert::IsTrue(TqTerrainProbe::Cols() == 0x8);
        Assert::IsTrue(TqTerrainProbe::Rows() == 0xC);
        Assert::IsTrue(TqTerrainProbe::Version() == 0x10);
        Assert::IsTrue(TqTerrainProbe::Triangle() == 0x14);
        Assert::IsTrue(TqTerrainProbe::Block() == 0x18);
        Assert::IsTrue(TqTerrainProbe::Blocks() == 0x1C);
    }
    TEST_METHOD(TheContainersFollowInTheBinarysOrder)
    {
        Assert::IsTrue(TqTerrainProbe::Vertices() == 0x1C + sizeof(vector<TqLayer*>));
        Assert::IsTrue(TqTerrainProbe::Materials() == TqTerrainProbe::Vertices() + sizeof(map<DWORD, TqVertex>));
        Assert::IsTrue(TqTerrainProbe::Shapes() == TqTerrainProbe::Materials() + sizeof(vector<TqMaterial*>));
        Assert::IsTrue(TqTerrainProbe::Reserved() == TqTerrainProbe::Shapes() + sizeof(vector<TqMaterial*>));
        Assert::IsTrue(TqTerrainProbe::Alpha() == TqTerrainProbe::Reserved() + sizeof(map<DWORD, DWORD>));
        Assert::IsTrue(TqTerrainProbe::AlphaLast() == TqTerrainProbe::Alpha() + 25);
        Assert::IsTrue(TqTerrainProbe::Targets() == ((TqTerrainProbe::AlphaLast() + 25 + 3) & ~3u));
        Assert::IsTrue(sizeof(CTqTerrain) == TqTerrainProbe::Targets() + sizeof(map<__int64, ITargetBmp*>));
    }
    TEST_METHOD(TheVertexRecordStartsEmpty)
    {
        TqVertex info;
        Assert::IsTrue(info.byMaterial == 0xFF);
    }
};
