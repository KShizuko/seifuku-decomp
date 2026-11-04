#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "SkyRose.h"
#include "PatternRose.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct SkyRoseProbe : CSkyRose
{
    static size_t Show() { return offsetof(SkyRoseProbe, m_nShowWorldPoint); }
    static size_t Orig() { return offsetof(SkyRoseProbe, m_nOrigWorldPoint); }
    static size_t CircleDir() { return offsetof(SkyRoseProbe, m_iCircleDir); }
    static size_t CurY() { return offsetof(SkyRoseProbe, m_nCurY); }
    static size_t LastFrame() { return offsetof(SkyRoseProbe, m_nLastFrame); }
    static size_t LoopFrame() { return offsetof(SkyRoseProbe, m_nLoopFrame); }
    static size_t RiseCount() { return offsetof(SkyRoseProbe, m_nRiseCount); }
    static size_t CurX() { return offsetof(SkyRoseProbe, m_nCurX); }
    static size_t YMoved() { return offsetof(SkyRoseProbe, m_iYMoved); }
    static size_t RiseStep() { return offsetof(SkyRoseProbe, m_nRiseStep); }
    static size_t AniIndex() { return offsetof(SkyRoseProbe, m_bAniIndex); }
    static size_t Alpha() { return offsetof(SkyRoseProbe, m_iAlpha); }
    static size_t FrameTime() { return offsetof(SkyRoseProbe, m_dwFrameTime); }
    static size_t Title() { return offsetof(SkyRoseProbe, m_szAniTitle); }
    static size_t Status() { return offsetof(SkyRoseProbe, m_iStatus); }
};

static_assert(sizeof(CSkyRose) == 0xB0, "");
static_assert(std::is_base_of<CMapObj, CSkyRose>::value, "");
static_assert(std::is_same<decltype(&CSkyRose::Create), void (CSkyRose::*)(char*, int, int, int, int)>::value, "");
static_assert(PATTERNROSE_RISE == 2 && PATTERNROSE_FALL == 3 && PATTERNROSE_DESTROY == 4, "");

TEST_CLASS(ClientSkyRoseHeader)
{
public:
    TEST_METHOD(MembersSitAtTheirOffsets)
    {
        Assert::IsTrue(SkyRoseProbe::Show() == 0x0C);
        Assert::IsTrue(SkyRoseProbe::Orig() == 0x14);
        Assert::IsTrue(SkyRoseProbe::CircleDir() == 0x1C);
        Assert::IsTrue(SkyRoseProbe::CurY() == 0x20);
        Assert::IsTrue(SkyRoseProbe::LastFrame() == 0x24);
        Assert::IsTrue(SkyRoseProbe::LoopFrame() == 0x28);
        Assert::IsTrue(SkyRoseProbe::RiseCount() == 0x2C);
        Assert::IsTrue(SkyRoseProbe::CurX() == 0x30);
        Assert::IsTrue(SkyRoseProbe::YMoved() == 0x34);
        Assert::IsTrue(SkyRoseProbe::RiseStep() == 0x38);
        Assert::IsTrue(SkyRoseProbe::AniIndex() == 0x3C);
        Assert::IsTrue(SkyRoseProbe::Alpha() == 0x40);
        Assert::IsTrue(SkyRoseProbe::FrameTime() == 0x44);
        Assert::IsTrue(SkyRoseProbe::Title() == 0x48);
        Assert::IsTrue(SkyRoseProbe::Status() == 0xAC);
    }
};
