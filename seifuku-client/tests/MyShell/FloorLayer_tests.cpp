#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "FloorLayer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct FloorLayerProbe
{
    static constexpr size_t Rose() { return offsetof(CFloorLayer, m_ObjRose); }
    static constexpr size_t Pattern() { return offsetof(CFloorLayer, m_ObjPatternRose); }
    static constexpr size_t Objs() { return offsetof(CFloorLayer, m_dequeMapObj); }
};

static_assert(sizeof(CFloorLayer) == sizeof(CMapLayer) + sizeof(CRoseSys) + sizeof(CPatternRoseSys) + sizeof(DEQUE_MAPOBJ), "");
static_assert(std::is_base_of<CMapLayer, CFloorLayer>::value, "");
static_assert(CMapLayer::LAYER_FLOOR == 2, "");
static_assert(std::is_same<decltype(&CFloorLayer::CreateNew), CFloorLayer* (*)()>::value, "");
static_assert(std::is_same<decltype(static_cast<void (CFloorLayer::*)(int, int, int, int)>(&CFloorLayer::CreateRose)), void (CFloorLayer::*)(int, int, int, int)>::value, "");
static_assert(std::is_same<decltype(&CFloorLayer::AddObj), void (CFloorLayer::*)(CMapObj*)>::value, "");

struct FloorObjProbe
{
    static constexpr size_t Life() { return offsetof(CFloorObj, m_nLifeTime); }
    static constexpr size_t X() { return offsetof(CFloorObj, m_nWorldX); }
    static constexpr size_t Y() { return offsetof(CFloorObj, m_nWorldY); }
    static constexpr size_t Frame() { return offsetof(CFloorObj, m_nCurrentFrame); }
    static constexpr size_t Ani() { return offsetof(CFloorObj, m_pAni); }
};
static_assert(std::is_base_of<CMapObj, CFloorObj>::value, "");
static_assert(sizeof(CFloorObj) == 0x20, "");
static_assert(std::is_same<decltype(&CFloorObj::Create), BOOL (CFloorObj::*)(const char*, const char*, int, int, int)>::value, "");

static_assert(FloorObjProbe::Life() == 0xC && FloorObjProbe::X() == 0x10 && FloorObjProbe::Y() == 0x14, "");
static_assert(FloorObjProbe::Frame() == 0x18 && FloorObjProbe::Ani() == 0x1C, "");
static_assert(FloorLayerProbe::Rose() == sizeof(CMapLayer), "");
static_assert(FloorLayerProbe::Pattern() == FloorLayerProbe::Rose() + sizeof(CRoseSys), "");
static_assert(FloorLayerProbe::Objs() == FloorLayerProbe::Pattern() + sizeof(CPatternRoseSys), "");
