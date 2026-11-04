#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "2DMapCoverObj.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MapCoverObjProbe : C2DMapCoverObj
{
    static size_t FileName() { return offsetof(MapCoverObjProbe, m_szFileName); }
    static size_t Index() { return offsetof(MapCoverObjProbe, m_szIndex); }
    static size_t Offset() { return offsetof(MapCoverObjProbe, m_posOffset); }
    static size_t Base() { return offsetof(MapCoverObjProbe, m_sizeBase); }
    static size_t FrameIndex() { return offsetof(MapCoverObjProbe, m_nFrameIndex); }
    static size_t FrameInterval() { return offsetof(MapCoverObjProbe, m_dwFrameInterval); }
    static size_t TimePreFrame() { return offsetof(MapCoverObjProbe, m_dwTimePreFrame); }
    static size_t ShowWay() { return offsetof(MapCoverObjProbe, m_dwShowWay); }
};

static_assert(sizeof(C2DMapCoverObj) == sizeof(C2DMapObj) + 0x1A4, "");
static_assert(sizeof(C2DMapObj) == 0x194, "");
static_assert(_MAX_PATH == 260 && _MAX_TITLESIZE == 128, "");
static_assert(std::is_same<decltype(&C2DMapCoverObj::LoadDataObj), void (C2DMapCoverObj::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapCoverObj::s_bStroke), BOOL*>::value, "");
static_assert(std::is_same<decltype(&C2DMapCoverObj::CreateNew), C2DMapCoverObj* (*)(char*, char*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapCoverObj::Create), void (C2DMapCoverObj::*)(char*, char*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapCoverObj::LoadTextObj), void (C2DMapCoverObj::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapCoverObj::SaveDataObj), void (C2DMapCoverObj::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapCoverObj::SaveTextObj), void (C2DMapCoverObj::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapCoverObj::SetOffset), void (C2DMapCoverObj::*)(CMyPos)>::value, "");
static_assert(std::is_same<decltype(&C2DMapCoverObj::GetOffset), void (C2DMapCoverObj::*)(CMyPos&)>::value, "");
static_assert(std::is_same<decltype(&C2DMapCoverObj::SetFrameInterval), void (C2DMapCoverObj::*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(&C2DMapCoverObj::GetFrameInterval), DWORD (C2DMapCoverObj::*)()>::value, "");
static_assert(std::is_same<decltype(&C2DMapCoverObj::SetShowWay), void (C2DMapCoverObj::*)(DWORD)>::value, "");

TEST_CLASS(ClientMapCoverObjHeader)
{
public:
    TEST_METHOD(MembersFollowTheBase)
    {
        Assert::IsTrue(MapCoverObjProbe::FileName() == sizeof(C2DMapObj));
        Assert::IsTrue(MapCoverObjProbe::Index() == sizeof(C2DMapObj) + 0x104);
        Assert::IsTrue(MapCoverObjProbe::Offset() == sizeof(C2DMapObj) + 0x184);
        Assert::IsTrue(MapCoverObjProbe::Base() == sizeof(C2DMapObj) + 0x18C);
        Assert::IsTrue(MapCoverObjProbe::FrameIndex() == sizeof(C2DMapObj) + 0x194);
        Assert::IsTrue(MapCoverObjProbe::FrameInterval() == sizeof(C2DMapObj) + 0x198);
        Assert::IsTrue(MapCoverObjProbe::TimePreFrame() == sizeof(C2DMapObj) + 0x19C);
        Assert::IsTrue(MapCoverObjProbe::ShowWay() == sizeof(C2DMapObj) + 0x1A0);
    }
};
