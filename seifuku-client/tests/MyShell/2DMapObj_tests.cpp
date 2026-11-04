#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "2DMapObj.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MapObj2DProbe : C2DMapObj
{
    static size_t Cell() { return offsetof(MapObj2DProbe, m_posCell); }
    static size_t CellY() { return offsetof(MapObj2DProbe, m_posCell.y); }
};

static_assert(sizeof(C2DMapObj) == 0x194, "");
static_assert(std::is_base_of<CMapObj, C2DMapObj>::value, "");
static_assert(std::has_virtual_destructor<C2DMapObj>::value, "");
static_assert(std::is_same<decltype(&C2DMapObj::SetPos), void (C2DMapObj::*)(CMyPos)>::value, "");
static_assert(std::is_same<decltype(&C2DMapObj::GetPos), void (C2DMapObj::*)(CMyPos&)>::value, "");
static_assert(std::is_same<decltype(&C2DMapObj::GetWorldPos), void (C2DMapObj::*)(CMyPos&)>::value, "");

TEST_CLASS(Client2DMapObjHeader)
{
public:
    TEST_METHOD(CellPositionFollowsCMapObj)
    {
        Assert::IsTrue(MapObj2DProbe::Cell() == 0xC);
        Assert::IsTrue(MapObj2DProbe::CellY() == 0xD0);
    }
};
