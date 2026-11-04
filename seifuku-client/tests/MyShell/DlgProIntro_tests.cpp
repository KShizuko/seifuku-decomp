#include "stdafx.h"
#include "MyShell.h"
#include "DlgProIntro.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgProIntro>::value, "");
static_assert(std::is_polymorphic<CDlgProIntro>::value, "");
static_assert(CDlgProIntro::IDD == 271, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgProIntroProbe
{
    static size_t Show() { return offsetof(CDlgProIntro, m_bShow); }
    static size_t Brush() { return offsetof(CDlgProIntro, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgProIntro, m_Pnt); }
};

TEST_CLASS(ClientDlgProIntroHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgProIntroProbe::Show() == b + 0x00);
        Assert::IsTrue(DlgProIntroProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgProIntroProbe::Pnt() == b + 0x08);
        const size_t close = RoundUp(b + 0x10, __alignof(CMyButton));
        Assert::IsTrue(offsetof(CDlgProIntro, m_BtnClose) == close);
        const size_t a = __alignof(CMyStatic);
        Assert::IsTrue(offsetof(CDlgProIntro, m_Sta1) == RoundUp(close + sizeof(CMyButton), a));
        Assert::IsTrue(offsetof(CDlgProIntro, m_Sta2) == RoundUp(offsetof(CDlgProIntro, m_Sta1) + sizeof(CMyStatic), a));
        Assert::IsTrue(offsetof(CDlgProIntro, m_Sta3) == RoundUp(offsetof(CDlgProIntro, m_Sta2) + sizeof(CMyStatic), a));
        Assert::IsTrue(sizeof(CDlgProIntro) == RoundUp(offsetof(CDlgProIntro, m_Sta3) + sizeof(CMyStatic), __alignof(CDlgProIntro)));
    }
};
