#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "InteractiveLayer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct InteractiveLayerProbe : CInteractiveLayer
{
    static size_t Objs() { return offsetof(InteractiveLayerProbe, m_setMapObj); }
    static size_t Shown() { return offsetof(InteractiveLayerProbe, m_setMapObjShow); }
};

static_assert(sizeof(CInteractiveLayer) == sizeof(CMapLayer) + 2 * sizeof(DEQUE_MAPOBJ), "");
static_assert(std::is_base_of<CMapLayer, CInteractiveLayer>::value, "");
static_assert(std::is_same<decltype(&CInteractiveLayer::CreateNew), CInteractiveLayer* (*)()>::value, "");
static_assert(std::is_same<decltype(&CInteractiveLayer::LoadDataLayer), void (CInteractiveLayer::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&CInteractiveLayer::Get2DFocusMapTerrainObjPart), C2DMapTerrainObjPart* (CInteractiveLayer::*)()>::value, "");
static_assert(std::is_same<decltype(static_cast<void (CInteractiveLayer::*)(CMapObj*)>(&CInteractiveLayer::ApplyShow)), void (CInteractiveLayer::*)(CMapObj*)>::value, "");
static_assert(std::is_same<decltype(static_cast<void (CInteractiveLayer::*)(CMapObj*, DEQUE_MAPOBJ&)>(&CInteractiveLayer::ApplyShow)), void (CInteractiveLayer::*)(CMapObj*, DEQUE_MAPOBJ&)>::value, "");
static_assert(std::is_same<decltype(&CInteractiveLayer::GetObjByIndex), CMapObj* (CInteractiveLayer::*)(int)>::value, "");
static_assert(std::is_same<decltype(&CInteractiveLayer::SaveDataLayer), void (CInteractiveLayer::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&CInteractiveLayer::SaveTextLayer), void (CInteractiveLayer::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&CInteractiveLayer::LoadTextLayer), void (CInteractiveLayer::*)(FILE*)>::value, "");

TEST_CLASS(ClientInteractiveLayerHeader)
{
public:
    TEST_METHOD(TheDequesFollowTheLayer)
    {
        Assert::IsTrue(InteractiveLayerProbe::Objs() == sizeof(CMapLayer));
        Assert::IsTrue(InteractiveLayerProbe::Shown() == sizeof(CMapLayer) + sizeof(DEQUE_MAPOBJ));
    }
};
