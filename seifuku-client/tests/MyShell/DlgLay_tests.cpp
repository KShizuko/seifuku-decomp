#include "stdafx.h"
#include "MyShell.h"
#include "DlgLay.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgLay>::value, "");
static_assert(std::is_polymorphic<CDlgLay>::value, "");
static_assert(CDlgLay::IDD == 242, "");
static_assert(DLG_LAY == 34 && ON_LAY_CLOSE == 81 && DLG_FRAME_AMOUNT == 20, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgLayProbe
{
    static size_t Show() { return offsetof(CDlgLay, m_bShow); }
    static size_t Brush() { return offsetof(CDlgLay, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgLay, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgLay, m_nPntX); }
    static size_t Dir() { return offsetof(CDlgLay, m_nDir); }
    static size_t Time() { return offsetof(CDlgLay, m_dwLastShowTime); }
    static size_t Step() { return offsetof(CDlgLay, m_fFrameStep); }
    static size_t Mode() { return offsetof(CDlgLay, m_nFrameMode); }
};

TEST_CLASS(ClientDlgLayHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgLayProbe::Show() == b + 0x00);
        Assert::IsTrue(DlgLayProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgLayProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgLayProbe::PntX() == b + 0x10);
        Assert::IsTrue(DlgLayProbe::Dir() == b + 0x14);
        Assert::IsTrue(DlgLayProbe::Time() == b + 0x18);
        Assert::IsTrue(DlgLayProbe::Step() == b + 0x1C);
        Assert::IsTrue(DlgLayProbe::Mode() == b + 0x20);
        const size_t a = __alignof(CMyButton);
        const size_t right = RoundUp(b + 0x24, a);
        Assert::IsTrue(offsetof(CDlgLay, m_LayRight) == right);
        Assert::IsTrue(offsetof(CDlgLay, m_BtnCancel) == right + 6 * RoundUp(sizeof(CMyButton), a));
        Assert::IsTrue(sizeof(CDlgLay) == RoundUp(offsetof(CDlgLay, m_BtnCancel) + sizeof(CMyButton), __alignof(CDlgLay)));
    }
};
