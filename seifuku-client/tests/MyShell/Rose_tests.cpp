#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Rose.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct RoseProbe
{
    static size_t ShowPoint() { return offsetof(CRose, m_nShowWorldPoint); }
    static size_t OrigPoint() { return offsetof(CRose, m_nOrigWorldPoint); }
    static size_t TargetPoint() { return offsetof(CRose, m_nTargetPoint); }
    static size_t LastFrame() { return offsetof(CRose, m_nLastFrame); }
    static size_t StayCount() { return offsetof(CRose, m_nStayCount); }
    static size_t LoopFrame() { return offsetof(CRose, m_nLoopFrame); }
    static size_t RiseCount() { return offsetof(CRose, m_nRiseCount); }
    static size_t CurX() { return offsetof(CRose, m_nCurX); }
    static size_t CurY() { return offsetof(CRose, m_nCurY); }
    static size_t YMoved() { return offsetof(CRose, m_iYMoved); }
    static size_t PathLength() { return offsetof(CRose, m_nPathLength); }
    static size_t AniIndex() { return offsetof(CRose, m_bAniIndex); }
    static size_t Alpha() { return offsetof(CRose, m_iAlpha); }
    static size_t FrameTime() { return offsetof(CRose, m_dwFrameTime); }
    static size_t FileName() { return offsetof(CRose, m_szFileName); }
    static size_t AniTitle() { return offsetof(CRose, m_szAniTitle); }
    static size_t RoseFileName() { return offsetof(Rose, m_szFileName); }
    static size_t RoseNum() { return offsetof(Rose, m_nNum); }
    static size_t RoseWorldY() { return offsetof(Rose, m_iWorldY); }
};

static_assert(sizeof(CRose) == 0x248, "");
static_assert(offsetof(CRose, m_iStatus) == 0, "");
static_assert(sizeof(Rose) == sizeof(DequeRose) + 0x4 + 8 * 100 + 0xC, "");
static_assert(offsetof(Rose, m_nStatus) == sizeof(DequeRose), "");
static_assert(sizeof(CRoseSys) == sizeof(CMapObj) + sizeof(DequeRoseSys), "");
static_assert(offsetof(CRoseSys, m_DequeRose) == sizeof(CMapObj), "");
static_assert(std::is_base_of<CMapObj, CRoseSys>::value, "");
static_assert(MAX_ROSEANI == 4 && ROSE_DESTROY == 5, "");
static_assert(std::is_same<decltype(&CRose::Create), BOOL (CRose::*)(int, int, int, int, int)>::value, "");
static_assert(std::is_same<decltype(&Rose::Create), BOOL (Rose::*)(int, int, int, int)>::value, "");
static_assert(std::is_same<decltype(&CRoseSys::Create), BOOL (CRoseSys::*)(int, int, int)>::value, "");

TEST_CLASS(ClientRoseHeader)
{
public:
    TEST_METHOD(CRoseMembersMatchCreate)
    {
        Assert::IsTrue(RoseProbe::ShowPoint() == 0x4);
        Assert::IsTrue(RoseProbe::OrigPoint() == 0xC);
        Assert::IsTrue(RoseProbe::TargetPoint() == 0x14);
        Assert::IsTrue(RoseProbe::LastFrame() == 0x1C);
        Assert::IsTrue(RoseProbe::StayCount() == 0x20);
        Assert::IsTrue(RoseProbe::LoopFrame() == 0x24);
        Assert::IsTrue(RoseProbe::RiseCount() == 0x28);
        Assert::IsTrue(RoseProbe::CurX() == 0x30);
        Assert::IsTrue(RoseProbe::CurY() == 0x34);
        Assert::IsTrue(RoseProbe::YMoved() == 0x38);
        Assert::IsTrue(RoseProbe::PathLength() == 0x40);
        Assert::IsTrue(RoseProbe::AniIndex() == 0x48);
        Assert::IsTrue(RoseProbe::Alpha() == 0x4C);
        Assert::IsTrue(RoseProbe::FrameTime() == 0x50);
        Assert::IsTrue(RoseProbe::FileName() == 0x54);
        Assert::IsTrue(RoseProbe::AniTitle() == 0x1E4);
    }

    TEST_METHOD(RoseMembersFollowTheDeque)
    {
        Assert::IsTrue(RoseProbe::RoseFileName() == sizeof(DequeRose) + 0x4);
        Assert::IsTrue(RoseProbe::RoseNum() == sizeof(DequeRose) + 0x4 + 8 * 100);
        Assert::IsTrue(RoseProbe::RoseWorldY() == sizeof(DequeRose) + 0x4 + 8 * 100 + 8);
    }
};
