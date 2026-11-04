#include "stdafx.h"
#include "MyShell.h"
#include "DlgEnmState.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgEnmState>::value, "");
static_assert(std::is_polymorphic<CDlgEnmState>::value, "");
static_assert(ON_ENMSTATE_DEL == 94, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgEnmStateProbe
{
    static size_t Show() { return offsetof(CDlgEnmState, m_bShow); }
    static size_t Brush() { return offsetof(CDlgEnmState, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgEnmState, m_Pnt); }
    static int Idd() { return CDlgEnmState::IDD; }
    static size_t Face() { return offsetof(CDlgEnmState, m_EnmStateImaface); }
    static size_t Sta6() { return offsetof(CDlgEnmState, m_Sta6); }
    static size_t Sta1() { return offsetof(CDlgEnmState, m_Sta1); }
    static size_t Sta7() { return offsetof(CDlgEnmState, m_Sta7); }
    static size_t Del() { return offsetof(CDlgEnmState, m_EnmStateBtnDel); }
    static size_t Close() { return offsetof(CDlgEnmState, m_EnmStateBtnClose); }
};

TEST_CLASS(ClientDlgEnmStateHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgEnmStateProbe::Idd() == 270);
        Assert::IsTrue(DlgEnmStateProbe::Show() == b + 0x00);
        Assert::IsTrue(DlgEnmStateProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgEnmStateProbe::Pnt() == b + 0x08);
        const size_t face = RoundUp(b + 0x10, __alignof(CMyImage));
        Assert::IsTrue(DlgEnmStateProbe::Face() == face);
        const size_t a = __alignof(CMyStatic);
        const size_t sta6 = RoundUp(face + sizeof(CMyImage), a);
        Assert::IsTrue(DlgEnmStateProbe::Sta6() == sta6);
        Assert::IsTrue(DlgEnmStateProbe::Sta1() == sta6 + 2 * RoundUp(sizeof(CMyStatic), a));
        Assert::IsTrue(DlgEnmStateProbe::Sta7() == sta6 + 6 * RoundUp(sizeof(CMyStatic), a));
        const size_t del = RoundUp(DlgEnmStateProbe::Sta7() + sizeof(CMyStatic), __alignof(CMyButton));
        Assert::IsTrue(DlgEnmStateProbe::Del() == del);
        Assert::IsTrue(DlgEnmStateProbe::Close() == RoundUp(del + sizeof(CMyButton), __alignof(CMyButton)));
        Assert::IsTrue(sizeof(CDlgEnmState) == RoundUp(DlgEnmStateProbe::Close() + sizeof(CMyButton), __alignof(CDlgEnmState)));
    }
};
