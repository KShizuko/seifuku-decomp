#include "stdafx.h"
#include "MyShell.h"
#include "MyProgress.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CMyProgress) == (sizeof(CProgressCtrl) + 0x7C + __alignof(CProgressCtrl) - 1) / __alignof(CProgressCtrl) * __alignof(CProgressCtrl), "");
static_assert(std::is_base_of<CProgressCtrl, CMyProgress>::value, "");
static_assert(std::is_polymorphic<CMyProgress>::value, "");

struct MyProgressProbe
{
    static size_t Par() { return offsetof(CMyProgress, m_PrgPar); }
    static size_t Id() { return offsetof(CMyProgress, m_PrgPar.m_strProgressID); }
    static size_t Border() { return offsetof(CMyProgress, m_PrgPar.m_bBorder); }
    static size_t BorderOffset() { return offsetof(CMyProgress, m_PrgPar.m_BorderOffset); }
    static size_t IntPoint() { return offsetof(CMyProgress, m_PrgPar.m_IntPoint); }
    static size_t Width() { return offsetof(CMyProgress, m_PrgPar.m_nWidth); }
    static size_t HWidth() { return offsetof(CMyProgress, m_PrgPar.m_nHWidth); }
    static size_t Height() { return offsetof(CMyProgress, m_PrgPar.m_nHeight); }
    static size_t Step() { return offsetof(CMyProgress, m_PrgPar.m_nStep); }
    static size_t AddStep() { return offsetof(CMyProgress, m_PrgPar.m_fAddStep); }
    static size_t Type() { return offsetof(CMyProgress, m_PrgPar.m_btType); }
    static size_t Max() { return offsetof(CMyProgress, m_PrgPar.m_nMax); }
    static size_t Upper() { return offsetof(CMyProgress, m_PrgPar.m_nUpper); }
    static size_t Lower() { return offsetof(CMyProgress, m_PrgPar.m_nLower); }
    static size_t ShowMode() { return offsetof(CMyProgress, m_PrgPar.m_btShowMode); }
    static size_t ParSize() { return sizeof(CMyProgress::MyProgressParameter); }
};

TEST_CLASS(ClientMyProgressHeader)
{
public:
    TEST_METHOD(TheRecordSitsWhereTheBodiesReadIt)
    {
        const size_t b = sizeof(CProgressCtrl);
        Assert::IsTrue(MyProgressProbe::Par() == b);
        Assert::IsTrue(MyProgressProbe::ParSize() == 0x7C);
        Assert::IsTrue(MyProgressProbe::Id() == b + 0x00);
        Assert::IsTrue(MyProgressProbe::Border() == b + 0x40);
        Assert::IsTrue(MyProgressProbe::BorderOffset() == b + 0x44);
        Assert::IsTrue(MyProgressProbe::IntPoint() == b + 0x4C);
        Assert::IsTrue(MyProgressProbe::Width() == b + 0x54);
        Assert::IsTrue(MyProgressProbe::HWidth() == b + 0x58);
        Assert::IsTrue(MyProgressProbe::Height() == b + 0x5C);
        Assert::IsTrue(MyProgressProbe::Step() == b + 0x60);
        Assert::IsTrue(MyProgressProbe::AddStep() == b + 0x64);
        Assert::IsTrue(MyProgressProbe::Type() == b + 0x68);
        Assert::IsTrue(MyProgressProbe::Max() == b + 0x6C);
        Assert::IsTrue(MyProgressProbe::Upper() == b + 0x70);
        Assert::IsTrue(MyProgressProbe::Lower() == b + 0x74);
        Assert::IsTrue(MyProgressProbe::ShowMode() == b + 0x78);
    }
};
