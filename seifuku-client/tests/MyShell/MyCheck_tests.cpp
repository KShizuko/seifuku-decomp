#include "stdafx.h"
#include "MyShell.h"
#include "MyCheck.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CMyCheck) == sizeof(CButton) + 0x68, "");
static_assert(std::is_base_of<CButton, CMyCheck>::value, "");
static_assert(std::is_polymorphic<CMyCheck>::value, "");

struct MyCheckProbe
{
    static size_t Par() { return offsetof(CMyCheck, m_ChkPar); }
    static size_t Frame() { return offsetof(CMyCheck, m_ChkPar.m_uFrame); }
    static size_t Id() { return offsetof(CMyCheck, m_ChkPar.m_strCheckID); }
    static size_t Point() { return offsetof(CMyCheck, m_ChkPar.m_IntPoint); }
    static size_t Type() { return offsetof(CMyCheck, m_ChkPar.m_btType); }
    static size_t Current() { return offsetof(CMyCheck, m_ChkPar.m_btCurrent); }
    static size_t Rect() { return offsetof(CMyCheck, m_ChkPar.m_ShowRect); }
    static size_t Width() { return offsetof(CMyCheck, m_ChkPar.m_nWidth); }
    static size_t Height() { return offsetof(CMyCheck, m_ChkPar.m_nHeight); }
};

TEST_CLASS(ClientMyCheckHeader)
{
public:
    TEST_METHOD(TheRecordSitsWhereInitWritesIt)
    {
        const size_t b = sizeof(CButton);
        Assert::IsTrue(MyCheckProbe::Par() == b);
        Assert::IsTrue(MyCheckProbe::Frame() == b + 0x00);
        Assert::IsTrue(MyCheckProbe::Id() == b + 0x04);
        Assert::IsTrue(MyCheckProbe::Point() == b + 0x44);
        Assert::IsTrue(MyCheckProbe::Type() == b + 0x4C);
        Assert::IsTrue(MyCheckProbe::Current() == b + 0x4D);
        Assert::IsTrue(MyCheckProbe::Rect() == b + 0x50);
        Assert::IsTrue(MyCheckProbe::Width() == b + 0x60);
        Assert::IsTrue(MyCheckProbe::Height() == b + 0x64);
    }
};
