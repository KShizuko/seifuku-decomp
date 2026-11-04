#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MapLayer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MapLayerProbe : CMapLayer
{
    static size_t RateX() { return offsetof(MapLayerProbe, m_nMoveRateX); }
    static size_t RateY() { return offsetof(MapLayerProbe, m_nMoveRateY); }
};

static_assert(sizeof(CMapLayer) == 0xC, "");
static_assert(CMapLayer::LAYER_NONE == 0 && CMapLayer::LAYER_TERRAIN == 1 && CMapLayer::LAYER_FLOOR == 2 && CMapLayer::LAYER_INTERACTIVE == 3 && CMapLayer::LAYER_SCENE == 4 && CMapLayer::LAYER_SKY == 5 && CMapLayer::LAYER_SURFACE == 6, "");
static_assert(std::is_polymorphic<CMapLayer>::value, "");
static_assert(std::is_same<decltype(&CMapLayer::Show), void (CMapLayer::*)(CMyPos)>::value, "");
static_assert(std::is_same<decltype(&CMapLayer::Process), void (CMapLayer::*)(void*)>::value, "");
static_assert(std::is_same<decltype(&CMapLayer::GetType), int (CMapLayer::*)()>::value, "");

TEST_CLASS(ClientMapLayerHeader)
{
public:
    TEST_METHOD(RatesFollowTheVtable)
    {
        Assert::IsTrue(MapLayerProbe::RateX() == 0x4);
        Assert::IsTrue(MapLayerProbe::RateY() == 0x8);
    }
};
