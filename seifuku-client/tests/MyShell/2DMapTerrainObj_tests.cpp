#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "2DMapTerrainObj.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct TerrainObjProbe : C2DMapTerrainObj
{
    static size_t Parts() { return offsetof(TerrainObjProbe, m_setPart); }
    static size_t Syn() { return offsetof(TerrainObjProbe, m_bSyn); }
    static size_t FileName() { return offsetof(TerrainObjProbe, m_szFileName); }
    static size_t Fixed() { return offsetof(TerrainObjProbe, m_bFixed); }
    static size_t Die() { return offsetof(TerrainObjProbe, m_bDie); }
};

static_assert(sizeof(C2DMapTerrainObj) == sizeof(C2DMapObj) + sizeof(DEQUE_SCENEPART) + 4 + _MAX_PATH + 8, "");
static_assert(_MAX_PATH == 260, "");
static_assert(std::is_polymorphic<C2DMapTerrainObj>::value, "");
static_assert(MAP_TERRAIN == 1, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObj::Load), void (C2DMapTerrainObj::*)(char*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObj::LoadData), void (C2DMapTerrainObj::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObj::AddPart), void (C2DMapTerrainObj::*)(C2DMapTerrainObjPart*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObj::GetPartByIndex), C2DMapTerrainObjPart* (C2DMapTerrainObj::*)(int)>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObj::GetPartAmount), int (C2DMapTerrainObj::*)()>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObj::ClearScenePart), void (C2DMapTerrainObj::*)()>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObj::Init), void (C2DMapTerrainObj::*)()>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObj::Destory), void (C2DMapTerrainObj::*)()>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObj::DeletePart), void (C2DMapTerrainObj::*)(int)>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObj::LoadText), void (C2DMapTerrainObj::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObj::GetMouseOffset), BOOL (C2DMapTerrainObj::*)(CMyPos, int&)>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObj::SaveData), void (C2DMapTerrainObj::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObj::SaveText), void (C2DMapTerrainObj::*)(FILE*)>::value, "");

TEST_CLASS(ClientTerrainObjHeader)
{
public:
    TEST_METHOD(ThePartDequeFollowsTheBase)
    {
        Assert::IsTrue(TerrainObjProbe::Parts() == sizeof(C2DMapObj));
    }
    TEST_METHOD(TheTailFollowsTheDeque)
    {
        const size_t d = sizeof(DEQUE_SCENEPART);
        Assert::IsTrue(TerrainObjProbe::Syn() == sizeof(C2DMapObj) + d);
        Assert::IsTrue(TerrainObjProbe::FileName() == sizeof(C2DMapObj) + 4 + d);
        Assert::IsTrue(TerrainObjProbe::Fixed() == sizeof(C2DMapObj) + 0x108 + d);
        Assert::IsTrue(TerrainObjProbe::Die() == sizeof(C2DMapObj) + 0x10C + d);
    }
};
