#include "stdafx.h"
#include "MyShell.h"
#include "DlgFrdState.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgFrdState>::value, "");
static_assert(std::is_polymorphic<CDlgFrdState>::value, "");
static_assert(CDlgFrdState::IDD == 209, "");
static_assert(ON_FRDSTATE_SHOW == 96, "");
static_assert(ON_FRDSTATE_DEL == 57, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgFrdStateProbe
{
    static size_t Brush() { return offsetof(CDlgFrdState, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgFrdState, m_Pnt); }
};

TEST_CLASS(ClientDlgFrdStateHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgFrdState, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgFrdStateProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgFrdStateProbe::Pnt() == b + 0x08);
        const size_t face = RoundUp(b + 0x10, __alignof(CMyImage));
        Assert::IsTrue(offsetof(CDlgFrdState, m_FrdStateImaface) == face);
        const size_t a = __alignof(CMyStatic);
        const size_t sta6 = RoundUp(face + sizeof(CMyImage), a);
        Assert::IsTrue(offsetof(CDlgFrdState, m_Sta6) == sta6);
        Assert::IsTrue(offsetof(CDlgFrdState, m_Sta5) == sta6 + 1 * RoundUp(sizeof(CMyStatic), a));
        Assert::IsTrue(offsetof(CDlgFrdState, m_Sta1) == sta6 + 2 * RoundUp(sizeof(CMyStatic), a));
        Assert::IsTrue(offsetof(CDlgFrdState, m_Sta4) == sta6 + 5 * RoundUp(sizeof(CMyStatic), a));
        Assert::IsTrue(offsetof(CDlgFrdState, m_Sta7) == sta6 + 6 * RoundUp(sizeof(CMyStatic), a));
        const size_t del = RoundUp(offsetof(CDlgFrdState, m_Sta7) + sizeof(CMyStatic), __alignof(CMyButton));
        Assert::IsTrue(offsetof(CDlgFrdState, m_FrdStateBtnDel) == del);
        Assert::IsTrue(offsetof(CDlgFrdState, m_FrdStateBtnClose) == RoundUp(del + sizeof(CMyButton), __alignof(CMyButton)));
        Assert::IsTrue(sizeof(CDlgFrdState) == RoundUp(offsetof(CDlgFrdState, m_FrdStateBtnClose) + sizeof(CMyButton), __alignof(CDlgFrdState)));
    }
};
