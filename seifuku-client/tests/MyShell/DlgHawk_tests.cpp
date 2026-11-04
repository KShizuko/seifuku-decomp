#include "stdafx.h"
#include "MyShell.h"
#include "DlgHawk.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgHawk>::value, "");
static_assert(std::is_polymorphic<CDlgHawk>::value, "");
static_assert(CDlgHawk::IDD == 235, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgHawkProbe
{
    static size_t Show() { return offsetof(CDlgHawk, m_bShow); }
    static size_t Brush() { return offsetof(CDlgHawk, m_Hbr); }
    static size_t Black() { return offsetof(CDlgHawk, m_HbrBlack); }
    static size_t Pnt() { return offsetof(CDlgHawk, m_Pnt); }
};

TEST_CLASS(ClientDlgHawkHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgHawkProbe::Show() == b + 0x00);
        Assert::IsTrue(DlgHawkProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgHawkProbe::Black() == b + 0x08);
        Assert::IsTrue(DlgHawkProbe::Pnt() == b + 0x0C);
        const size_t edit = RoundUp(b + 0x14, __alignof(CMyEdit));
        Assert::IsTrue(offsetof(CDlgHawk, m_EdtData) == edit);
        const size_t a = __alignof(CMyButton);
        Assert::IsTrue(offsetof(CDlgHawk, m_BtnClose) == RoundUp(edit + sizeof(CMyEdit), a));
        Assert::IsTrue(offsetof(CDlgHawk, m_BtnAgree) == RoundUp(offsetof(CDlgHawk, m_BtnClose) + sizeof(CMyButton), a));
        Assert::IsTrue(sizeof(CDlgHawk) == RoundUp(offsetof(CDlgHawk, m_BtnAgree) + sizeof(CMyButton), __alignof(CDlgHawk)));
    }
};
