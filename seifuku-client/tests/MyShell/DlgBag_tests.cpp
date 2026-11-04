#include "stdafx.h"
#include "MyShell.h"
#include "DlgBag.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgBag>::value, "");
static_assert(std::is_polymorphic<CDlgBag>::value, "");
static_assert(DLG_BAG == 42, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgBagProbe
{
    static int Idd() { return CDlgBag::IDD; }
    static size_t Show() { return offsetof(CDlgBag, m_bShow); }
    static size_t Brush() { return offsetof(CDlgBag, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgBag, m_Pnt); }
    static size_t Frame() { return offsetof(CDlgBag, m_btFrame); }
    static size_t Grid() { return offsetof(CDlgBag, m_Grid); }
    static size_t Close() { return offsetof(CDlgBag, m_BtnClose); }
};

TEST_CLASS(ClientDlgBagHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        Assert::IsTrue(DlgBagProbe::Idd() == 277);
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgBagProbe::Show() == b + 0x00);
        Assert::IsTrue(DlgBagProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgBagProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgBagProbe::Frame() == b + 0x10);
        const size_t grid = RoundUp(b + 0x14, __alignof(CMyGrid));
        Assert::IsTrue(DlgBagProbe::Grid() == grid);
        Assert::IsTrue(DlgBagProbe::Close() == RoundUp(grid + sizeof(CMyGrid), __alignof(CMyButton)));
        Assert::IsTrue(sizeof(CDlgBag) == RoundUp(DlgBagProbe::Close() + sizeof(CMyButton), __alignof(CDlgBag)));
    }
};
