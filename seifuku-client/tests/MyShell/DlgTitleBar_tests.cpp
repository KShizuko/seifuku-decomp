#include "stdafx.h"
#include "MyShell.h"
#include "DlgTitleBar.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgTitleBar>::value, "");
static_assert(std::is_polymorphic<CDlgTitleBar>::value, "");
static_assert(CDlgTitleBar::IDD == 195, "");
static_assert(ON_EXIT == 0 && ON_MIN == 2 && ON_OWNER_MOVE == 64 && ON_OWNER_STOP == 65, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

TEST_CLASS(ClientDlgTitleBarHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheHandlersReadThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgTitleBar, m_bLBtnDown) == b + 0x00);
        Assert::IsTrue(offsetof(CDlgTitleBar, m_InitDlgRect) == b + 0x04);
        Assert::IsTrue(offsetof(CDlgTitleBar, m_InitDlgPnt) == b + 0x14);
        Assert::IsTrue(offsetof(CDlgTitleBar, m_MouseMovePnt) == b + 0x1C);
        Assert::IsTrue(offsetof(CDlgTitleBar, m_Hbr) == b + 0x24);
        Assert::IsTrue(offsetof(CDlgTitleBar, m_hBmp) == b + 0x28);
        Assert::IsTrue(offsetof(CDlgTitleBar, m_pParentWnd) == b + 0x2C);
        const size_t a = __alignof(CButton);
        const size_t five = RoundUp(b + 0x30, a);
        Assert::IsTrue(offsetof(CDlgTitleBar, m_5) == five);
        Assert::IsTrue(offsetof(CDlgTitleBar, m_BtnMin) == RoundUp(five + sizeof(CButton), a));
        Assert::IsTrue(offsetof(CDlgTitleBar, m_BtnClose) == RoundUp(offsetof(CDlgTitleBar, m_BtnMin) + sizeof(CButton), a));
        Assert::IsTrue(sizeof(CDlgTitleBar) == RoundUp(offsetof(CDlgTitleBar, m_BtnClose) + sizeof(CButton), __alignof(CDlgTitleBar)));
    }
};
