#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "3DMapSimpleObj.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MapSimpleObjProbe
{
    static size_t Obj() { return offsetof(C3DMapSimpleObj, m_obj3DSimpleObj); }
    static size_t World() { return offsetof(C3DMapSimpleObj, m_posWorld); }
    static size_t Begin() { return offsetof(C3DMapSimpleObj, m_dwTimeBegin); }
    static size_t Interval() { return offsetof(C3DMapSimpleObj, m_dwFrameInterval); }
    static size_t Die() { return offsetof(C3DMapSimpleObj, m_bDie); }
    static size_t Role() { return offsetof(C3DMapSimpleObj, m_bRole); }
    static size_t H() { return offsetof(C3DMapSimpleObj, m_fHorizontal); }
    static size_t V() { return offsetof(C3DMapSimpleObj, m_fVertical); }
};

static_assert(std::is_base_of<C3DMapObj, C3DMapSimpleObj>::value, "");
static_assert(sizeof(C3DMapObj) == 0x14, "");
static_assert(std::is_same<decltype(&C3DMapSimpleObj::Create), BOOL (C3DMapSimpleObj::*)(OBJID, CMyPos, BOOL)>::value, "");
static_assert(std::is_same<decltype(&C3DMapSimpleObj::CreateNew), C3DMapSimpleObj* (*)(OBJID, CMyPos, BOOL)>::value, "");
static_assert(std::is_same<decltype(&C3DMapSimpleObj::SetWorldPos), void (C3DMapSimpleObj::*)(CMyPos)>::value, "");
static_assert(std::is_same<decltype(&C3DMapSimpleObj::AddEffect), void (C3DMapSimpleObj::*)(char*)>::value, "");
static_assert(std::is_same<decltype(&C3DMapSimpleObj::SetFrameInterval), void (C3DMapSimpleObj::*)(DWORD)>::value, "");

TEST_CLASS(ClientMapSimpleObjHeader)
{
public:
    TEST_METHOD(TheSimpleObjectFollowsTheCell)
    {
        Assert::IsTrue(MapSimpleObjProbe::Obj() == 0x14);
    }
    TEST_METHOD(TheScalarsFollowTheSimpleObject)
    {
        const size_t o = sizeof(I3DSimpleObj*);
        Assert::IsTrue(MapSimpleObjProbe::World() == 0x14 + o);
        Assert::IsTrue(MapSimpleObjProbe::Begin() == 0x14 + o + 0x8);
        Assert::IsTrue(MapSimpleObjProbe::Interval() == 0x14 + o + 0xC);
        Assert::IsTrue(MapSimpleObjProbe::Die() == 0x14 + o + 0x10);
        Assert::IsTrue(MapSimpleObjProbe::Role() == 0x14 + o + 0x14);
        Assert::IsTrue(MapSimpleObjProbe::H() == 0x14 + o + 0x18);
        Assert::IsTrue(MapSimpleObjProbe::V() == 0x14 + o + 0x1C);
        Assert::IsTrue(sizeof(C3DMapSimpleObj) == 0x14 + o + 0x20);
    }
    TEST_METHOD(TheTypeIsNine)
    {
        Assert::IsTrue(MAP_3DSIMPLE == 9);
    }
};
