#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "PuzzleBmp.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct PuzzleBmpProbe : CPuzzleBmp
{
    static size_t Amount() { return offsetof(PuzzleBmpProbe, m_nPuzzleAmount); }
    static size_t Index() { return offsetof(PuzzleBmpProbe, m_usPuzzleIndex); }
    static size_t Terrain() { return offsetof(PuzzleBmpProbe, m_bTerrain); }
    static size_t Size() { return offsetof(PuzzleBmpProbe, m_infoSize); }
    static size_t AniFile() { return offsetof(PuzzleBmpProbe, m_szAniFile); }
    static size_t ZoomIn() { return offsetof(PuzzleBmpProbe, m_nZoomInNum); }
    static size_t Gride() { return offsetof(PuzzleBmpProbe, m_nGrideSize); }
    static size_t SpeedX() { return offsetof(PuzzleBmpProbe, m_nRollSpeedX); }
    static size_t SpeedY() { return offsetof(PuzzleBmpProbe, m_nRollSpeedY); }
    static size_t Version() { return offsetof(PuzzleBmpProbe, m_dwVersion); }
    static size_t TimeBegin() { return offsetof(PuzzleBmpProbe, m_dwTimeBegin); }
    static size_t RateX() { return offsetof(PuzzleBmpProbe, m_nMoveRateX); }
    static size_t RateY() { return offsetof(PuzzleBmpProbe, m_nMoveRateY); }
    static size_t Roll() { return offsetof(PuzzleBmpProbe, m_bRoll); }
};

static_assert(sizeof(CPuzzleBmp) == 0x13C, "");
static_assert(std::is_polymorphic<CPuzzleBmp>::value, "");
static_assert(_MAX_DIRTY_RECT == 256, "");
static_assert(sizeof(CMyRect) == 16, "");
static_assert(std::is_same<decltype(&CPuzzleBmp::s_nRectAmount), int*>::value, "");
static_assert(std::is_same<decltype(&CPuzzleBmp::TestRect), BOOL (*)(CMyRect, BOOL, BOOL)>::value, "");
static_assert(std::is_same<decltype(&CPuzzleBmp::GetNormalGrideSize), int (*)()>::value, "");
static_assert(std::is_same<decltype(&CPuzzleBmp::Show), void (CPuzzleBmp::*)(int, int, BOOL, BOOL)>::value, "");
static_assert(std::is_same<decltype(&CPuzzleBmp::Load), BOOL (CPuzzleBmp::*)(char*)>::value, "");
static_assert(std::is_same<decltype(&CPuzzleBmp::PreLoad), void (CPuzzleBmp::*)(int, int)>::value, "");
static_assert(std::is_same<decltype(&CPuzzleBmp::SetSize), void (CPuzzleBmp::*)(CMySize)>::value, "");
static_assert(std::is_same<decltype(static_cast<unsigned short (CPuzzleBmp::*)(int, int)>(&CPuzzleBmp::GetGrid)), unsigned short (CPuzzleBmp::*)(int, int)>::value, "");
static_assert(std::is_same<decltype(&CPuzzleBmp::SetGrideSize), void (CPuzzleBmp::*)(int)>::value, "");
static_assert(std::is_same<decltype(&CPuzzleBmp::SetGrid), void (CPuzzleBmp::*)(int, int, unsigned short)>::value && std::is_same<decltype(static_cast<unsigned short (CPuzzleBmp::*)(CMyPos)>(&CPuzzleBmp::GetGrid)), unsigned short (CPuzzleBmp::*)(CMyPos)>::value, "");
static_assert(std::is_same<decltype(&CPuzzleBmp::Save), void (CPuzzleBmp::*)(char*)>::value && std::is_same<decltype(&CPuzzleBmp::Unit), void (CPuzzleBmp::*)(CPuzzleBmp*, CMyPos)>::value && std::is_same<decltype(&CPuzzleBmp::SetZoomInNum), void (CPuzzleBmp::*)(int)>::value, "");
static_assert(std::is_same<decltype(&CPuzzleBmp::IsDirty), BOOL (*)(CMyRect)>::value && std::is_same<decltype(&CPuzzleBmp::SetNormalGrideSize), void (*)(int)>::value, "");

TEST_CLASS(ClientPuzzleBmpHeader)
{
public:
    TEST_METHOD(MembersFollowTheVtable)
    {
        Assert::IsTrue(PuzzleBmpProbe::Amount() == 0x4);
        Assert::IsTrue(PuzzleBmpProbe::Index() == 0x8);
        Assert::IsTrue(PuzzleBmpProbe::Terrain() == 0xC);
        Assert::IsTrue(PuzzleBmpProbe::Size() == 0x10);
        Assert::IsTrue(PuzzleBmpProbe::AniFile() == 0x18);
        Assert::IsTrue(PuzzleBmpProbe::ZoomIn() == 0x118);
        Assert::IsTrue(PuzzleBmpProbe::Gride() == 0x11C);
        Assert::IsTrue(PuzzleBmpProbe::SpeedX() == 0x120);
        Assert::IsTrue(PuzzleBmpProbe::SpeedY() == 0x124);
        Assert::IsTrue(PuzzleBmpProbe::Version() == 0x128);
        Assert::IsTrue(PuzzleBmpProbe::TimeBegin() == 0x12C);
        Assert::IsTrue(PuzzleBmpProbe::RateX() == 0x130);
        Assert::IsTrue(PuzzleBmpProbe::RateY() == 0x134);
        Assert::IsTrue(PuzzleBmpProbe::Roll() == 0x138);
    }
};
