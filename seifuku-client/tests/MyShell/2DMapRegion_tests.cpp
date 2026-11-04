#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "2DMapRegion.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MapRegionProbe : C2DMapRegion
{
    static size_t Info() { return offsetof(MapRegionProbe, m_InfoRegion); }
};

static_assert(sizeof(RegionInfo) == 0x170, "");
static_assert(offsetof(RegionInfo, m_idMap) == 0x0, "");
static_assert(offsetof(RegionInfo, m_posCell) == 0x4, "");
static_assert(offsetof(RegionInfo, m_dwType) == 0xC, "");
static_assert(offsetof(RegionInfo, m_dwCx) == 0x10, "");
static_assert(offsetof(RegionInfo, m_dwCy) == 0x14, "");
static_assert(offsetof(RegionInfo, m_nNColor) == 0x18, "");
static_assert(offsetof(RegionInfo, m_nNShowType) == 0x1C, "");
static_assert(offsetof(RegionInfo, m_nDColor) == 0x20, "");
static_assert(offsetof(RegionInfo, m_nDShowPos) == 0x24, "");
static_assert(offsetof(RegionInfo, m_szString) == 0x28, "");
static_assert(sizeof(RegionInfo::m_szString) == 0x40, "");
static_assert(offsetof(RegionInfo, m_dwAccessTime) == 0x68, "");
static_assert(offsetof(RegionInfo, m_bAccess) == 0x6C, "");
static_assert(offsetof(RegionInfo, m_szMedia) == 0x70 && sizeof(RegionInfo::m_szMedia) == 0x100, "");
static_assert(sizeof(C2DMapRegion) == 0x174, "");
static_assert(std::is_polymorphic<C2DMapRegion>::value, "");
static_assert(MAP_2DREGION == 16, "");
static_assert(std::is_same<decltype(static_cast<C2DMapRegion* (*)(RegionInfo*)>(&C2DMapRegion::CreateNew)), C2DMapRegion* (*)(RegionInfo*)>::value, "");
static_assert(std::is_same<decltype(static_cast<C2DMapRegion* (*)(int, int, int, int)>(&C2DMapRegion::CreateNew)), C2DMapRegion* (*)(int, int, int, int)>::value, "");

TEST_CLASS(ClientMapRegionHeader)
{
public:
    TEST_METHOD(TheRecordFollowsTheVtable)
    {
        Assert::IsTrue(MapRegionProbe::Info() == 0x4);
    }
};
