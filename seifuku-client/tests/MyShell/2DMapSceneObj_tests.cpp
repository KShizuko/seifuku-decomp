#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "2DMapSceneObj.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MapSceneObjProbe
{
    static size_t Name() { return offsetof(C2DMapSceneObj, m_szFileName); }
    static size_t Index() { return offsetof(C2DMapSceneObj, m_szIndex); }
    static size_t Offset() { return offsetof(C2DMapSceneObj, m_posOffset); }
    static size_t Frame() { return offsetof(C2DMapSceneObj, m_nFrameIndex); }
    static size_t Interval() { return offsetof(C2DMapSceneObj, m_dwFrameInterval); }
    static size_t PreFrame() { return offsetof(C2DMapSceneObj, m_dwTimePreFrame); }
    static size_t ShowWay() { return offsetof(C2DMapSceneObj, m_dwShowWay); }
};

static_assert(sizeof(C2DMapObj) == 0x194, "");
static_assert(_MAX_TITLESIZE == 128, "");
static_assert(sizeof(C2DMapSceneObj) == sizeof(C2DMapObj) + 0x19C, "");
static_assert(std::is_base_of<C2DMapObj, C2DMapSceneObj>::value, "");
static_assert(std::is_polymorphic<C2DMapSceneObj>::value, "");
static_assert(MAP_SCENE == 3, "");
static_assert(std::is_same<decltype(&C2DMapSceneObj::LoadDataObj), void (C2DMapSceneObj::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapSceneObj::CreateNew), C2DMapSceneObj* (*)(char*, char*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapSceneObj::Create), void (C2DMapSceneObj::*)(char*, char*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapSceneObj::LoadTextObj), void (C2DMapSceneObj::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapSceneObj::SaveDataObj), void (C2DMapSceneObj::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapSceneObj::SaveTextObj), void (C2DMapSceneObj::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapSceneObj::SetFrameInterval), void (C2DMapSceneObj::*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(&C2DMapSceneObj::GetFrameInterval), DWORD (C2DMapSceneObj::*)()>::value, "");
static_assert(std::is_same<decltype(&C2DMapSceneObj::SetOffset), void (C2DMapSceneObj::*)(CMyPos)>::value, "");
static_assert(std::is_same<decltype(&C2DMapSceneObj::GetOffset), void (C2DMapSceneObj::*)(CMyPos&)>::value, "");

TEST_CLASS(ClientMapSceneObjHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheLoaderReadsThem)
    {
        Assert::IsTrue(MapSceneObjProbe::Name() == sizeof(C2DMapObj));
        Assert::IsTrue(MapSceneObjProbe::Index() == sizeof(C2DMapObj) + 0x104);
        Assert::IsTrue(MapSceneObjProbe::Offset() == sizeof(C2DMapObj) + 0x184);
        Assert::IsTrue(MapSceneObjProbe::Frame() == sizeof(C2DMapObj) + 0x18C);
        Assert::IsTrue(MapSceneObjProbe::Interval() == sizeof(C2DMapObj) + 0x190);
        Assert::IsTrue(MapSceneObjProbe::PreFrame() == sizeof(C2DMapObj) + 0x194);
        Assert::IsTrue(MapSceneObjProbe::ShowWay() == sizeof(C2DMapObj) + 0x198);
    }
};
