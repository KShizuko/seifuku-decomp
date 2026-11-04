#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MapItemManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MapItemManagerProbe : CMapItemManager
{
    static size_t Infos() { return offsetof(MapItemManagerProbe, m_setItemInfo); }
};

static_assert(sizeof(MapItemInfo) == 0x14, "");
static_assert(offsetof(MapItemInfo, idMapItem) == 0x0, "");
static_assert(offsetof(MapItemInfo, idItemType) == 0x4, "");
static_assert(offsetof(MapItemInfo, posCell) == 0x8, "");
static_assert(offsetof(MapItemInfo, pMapItem) == 0x10, "");
static_assert(sizeof(CMapItemManager) == 0x4 + sizeof(VECTOR_MAPITEMINFO), "");
static_assert(std::is_polymorphic<CMapItemManager>::value, "");
static_assert(std::is_same<decltype(&CMapItemManager::AddMapItem), void (CMapItemManager::*)(OBJID, OBJID, CMyPos)>::value, "");
static_assert(std::is_same<decltype(&CMapItemManager::DelMapItem), void (CMapItemManager::*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&CMapItemManager::GetMapItem), OBJID (CMapItemManager::*)(CMyPos)>::value, "");
static_assert(std::is_same<decltype(&CMapItemManager::GetFocusItem), OBJID (CMapItemManager::*)(CMyPos&)>::value, "");

TEST_CLASS(ClientMapItemManagerHeader)
{
public:
    TEST_METHOD(TheVectorFollowsTheVtable)
    {
        Assert::IsTrue(MapItemManagerProbe::Infos() == 0x4);
    }
};
