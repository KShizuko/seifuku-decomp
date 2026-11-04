#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "2DMapPuzzleObj.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MapPuzzleObjProbe
{
    static size_t Name() { return offsetof(C2DMapPuzzleObj, m_szFileName); }
};

static_assert(sizeof(C2DMapObj) == 0x194, "");
static_assert(sizeof(CPuzzleBmp) == 0x13C, "");
static_assert(sizeof(C2DMapPuzzleObj) == 0x3D4, "");
static_assert(offsetof(C2DMapPuzzleObj, m_bmpPuzzle) == 0x298, "");
static_assert(std::is_base_of<C2DMapObj, C2DMapPuzzleObj>::value, "");
static_assert(std::is_same<decltype(&C2DMapPuzzleObj::LoadData), void (C2DMapPuzzleObj::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapPuzzleObj::SaveData), void (C2DMapPuzzleObj::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapPuzzleObj::LoadFile), void (C2DMapPuzzleObj::*)(char*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapPuzzleObj::SaveText), void (C2DMapPuzzleObj::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapPuzzleObj::LoadText), void (C2DMapPuzzleObj::*)(FILE*)>::value, "");
static_assert(MAP_PUZZLE == 8, "");

TEST_CLASS(ClientMapPuzzleObjHeader)
{
public:
    TEST_METHOD(TheFileNameFollowsTheCell)
    {
        Assert::IsTrue(MapPuzzleObjProbe::Name() == 0x194);
    }
};
