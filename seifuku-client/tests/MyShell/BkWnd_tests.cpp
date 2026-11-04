#include "stdafx.h"
#include "MyShell.h"
#include "BkWnd.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CWnd, CBkWnd>::value, "");
static_assert(std::is_polymorphic<CBkWnd>::value, "");
static_assert(IDR_MAINFRAME == 128 && IDD_MYSHELL_DIALOG == 102, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct BkWndProbe
{
    static size_t Icon() { return offsetof(CBkWnd, m_hIcon); }
};

TEST_CLASS(BkWndTests)
{
public:
    TEST_METHOD(LayoutFollowsTheBinary)
    {
        size_t o = sizeof(CWnd);
        o = RoundUp(o, __alignof(CMyShellDlg));
        Assert::IsTrue(offsetof(CBkWnd, m_MyShellDlg) == o);
        o += sizeof(CMyShellDlg);
        o = RoundUp(o, __alignof(HICON));
        Assert::IsTrue(BkWndProbe::Icon() == o);
        o += sizeof(HICON);
        Assert::IsTrue(sizeof(CBkWnd) == RoundUp(o, __alignof(CBkWnd)));
        Assert::IsTrue(BkWndProbe::Icon() - offsetof(CBkWnd, m_MyShellDlg) == sizeof(CMyShellDlg));
    }
};
