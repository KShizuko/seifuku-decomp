#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "3DMapObj.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MapObj3DProbe : C3DMapObj
{
    static size_t Cell() { return offsetof(MapObj3DProbe, m_posCell); }
};

static_assert(sizeof(C3DMapObj) == 0x14, "");
static_assert(std::is_base_of<CMapObj, C3DMapObj>::value, "");
static_assert(std::is_polymorphic<C3DMapObj>::value, "");
static_assert(MAP_3DOBJ == 13, "");
static_assert(std::is_same<decltype(&C3DMapObj::GetWorldPos), void (C3DMapObj::*)(CMyPos&)>::value, "");
static_assert(std::is_same<decltype(&C3DMapObj::GetPos), void (C3DMapObj::*)(CMyPos&)>::value, "");
static_assert(std::is_same<decltype(&C3DMapObj::SetPos), void (C3DMapObj::*)(CMyPos)>::value, "");

TEST_CLASS(Client3DMapObjHeader)
{
public:
    TEST_METHOD(CellPositionFollowsTheBase)
    {
        Assert::IsTrue(MapObj3DProbe::Cell() == 0xC);
    }
};
