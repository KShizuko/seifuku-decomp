#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "PatternRose.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct PatternRoseProbe
{
    static size_t FileName() { return offsetof(CPatternRose, m_szFileName); }
    static size_t ShowPoint() { return offsetof(CPatternRose, m_nShowWorldPoint); }
    static size_t OrigPoint() { return offsetof(CPatternRose, m_nOrigWorldPoint); }
    static size_t CircleDir() { return offsetof(CPatternRose, m_iCircleDir); }
    static size_t CurY() { return offsetof(CPatternRose, m_nCurY); }
    static size_t LastFrame() { return offsetof(CPatternRose, m_nLastFrame); }
    static size_t LoopFrame() { return offsetof(CPatternRose, m_nLoopFrame); }
    static size_t RiseCount() { return offsetof(CPatternRose, m_nRiseCount); }
    static size_t CurX() { return offsetof(CPatternRose, m_nCurX); }
    static size_t YMoved() { return offsetof(CPatternRose, m_iYMoved); }
    static size_t RiseStep() { return offsetof(CPatternRose, m_nRiseStep); }
    static size_t AniIndex() { return offsetof(CPatternRose, m_bAniIndex); }
    static size_t Alpha() { return offsetof(CPatternRose, m_iAlpha); }
    static size_t FrameTime() { return offsetof(CPatternRose, m_dwFrameTime); }
    static size_t AniTitle() { return offsetof(CPatternRose, m_szAniTitle); }
    static size_t SysFileName() { return offsetof(PatternRose, m_szFileName); }
    static size_t Points() { return offsetof(PatternRose, m_nPoint); }
    static size_t Num() { return offsetof(PatternRose, m_nNum); }
    static size_t WorldY() { return offsetof(PatternRose, m_iWorldY); }
};

static_assert(sizeof(CPatternRose) == 0x234, "");
static_assert(offsetof(CPatternRose, m_iStatus) == 0, "");
static_assert(sizeof(PatternRose) == sizeof(DequePatternRose) + 0x4 + 8 * 100 + 35 * 20 * 2 * 4 + 0xC, "");
static_assert(offsetof(PatternRose, m_nStatus) == sizeof(DequePatternRose), "");
static_assert(sizeof(CPatternRoseSys) == sizeof(CMapObj) + sizeof(DequePatternRoseSys), "");
static_assert(offsetof(CPatternRoseSys, m_DequePatternRose) == sizeof(CMapObj), "");
static_assert(std::is_base_of<CMapObj, CPatternRoseSys>::value, "");
static_assert(MAX_PatternRoseANI == 4 && PATTERNROSE_DESTROY == 4, "");
static_assert(std::is_same<decltype(&CPatternRose::Create), BOOL (CPatternRose::*)(int, int)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (PatternRose::*)(int, int, int, int)>(&PatternRose::Create)), BOOL (PatternRose::*)(int, int, int, int)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CPatternRoseSys::*)(int, int, int, int)>(&CPatternRoseSys::Create)), BOOL (CPatternRoseSys::*)(int, int, int, int)>::value, "");

TEST_CLASS(ClientPatternRoseHeader)
{
public:
    TEST_METHOD(CPatternRoseMembersMatchCreate)
    {
        Assert::IsTrue(PatternRoseProbe::FileName() == 0x4);
        Assert::IsTrue(PatternRoseProbe::ShowPoint() == 0x194);
        Assert::IsTrue(PatternRoseProbe::OrigPoint() == 0x19C);
        Assert::IsTrue(PatternRoseProbe::CircleDir() == 0x1A4);
        Assert::IsTrue(PatternRoseProbe::CurY() == 0x1A8);
        Assert::IsTrue(PatternRoseProbe::LastFrame() == 0x1AC);
        Assert::IsTrue(PatternRoseProbe::LoopFrame() == 0x1B0);
        Assert::IsTrue(PatternRoseProbe::RiseCount() == 0x1B4);
        Assert::IsTrue(PatternRoseProbe::CurX() == 0x1B8);
        Assert::IsTrue(PatternRoseProbe::YMoved() == 0x1BC);
        Assert::IsTrue(PatternRoseProbe::RiseStep() == 0x1C0);
        Assert::IsTrue(PatternRoseProbe::AniIndex() == 0x1C4);
        Assert::IsTrue(PatternRoseProbe::Alpha() == 0x1C8);
        Assert::IsTrue(PatternRoseProbe::FrameTime() == 0x1CC);
        Assert::IsTrue(PatternRoseProbe::AniTitle() == 0x1D0);
    }

    TEST_METHOD(PatternRoseMembersFollowTheDeque)
    {
        Assert::IsTrue(PatternRoseProbe::SysFileName() == sizeof(DequePatternRose) + 0x4);
        Assert::IsTrue(PatternRoseProbe::Points() == sizeof(DequePatternRose) + 0x4 + 8 * 100);
        Assert::IsTrue(PatternRoseProbe::Num() == sizeof(DequePatternRose) + 0x4 + 8 * 100 + 35 * 20 * 2 * 4);
        Assert::IsTrue(PatternRoseProbe::WorldY() == sizeof(DequePatternRose) + 0x4 + 8 * 100 + 35 * 20 * 2 * 4 + 8);
    }
};
