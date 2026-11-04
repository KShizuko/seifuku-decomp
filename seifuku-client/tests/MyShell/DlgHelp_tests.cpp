#include "stdafx.h"
#include "MyShell.h"
#include "DlgHelp.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgHelp>::value, "");
static_assert(std::is_polymorphic<CDlgHelp>::value, "");
static_assert(CDlgHelp::IDD == 215, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

TEST_CLASS(ClientDlgHelpHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgHelp, m_pWeb) == b);
        Assert::IsTrue(offsetof(CDlgHelp, m_Hbr) == b + 0x04);
        Assert::IsTrue(offsetof(CDlgHelp, m_hBmpBk) == b + 0x08);
        Assert::IsTrue(offsetof(CDlgHelp, m_hBmpClose) == b + 0x0C);
        Assert::IsTrue(offsetof(CDlgHelp, m_hBmpMore) == b + 0x10);
        const size_t a = __alignof(CButton);
        const size_t more = RoundUp(b + 0x14, a);
        Assert::IsTrue(offsetof(CDlgHelp, m_HelpMore) == more);
        Assert::IsTrue(offsetof(CDlgHelp, m_HelpClose) == RoundUp(more + sizeof(CButton), a));
        Assert::IsTrue(sizeof(CDlgHelp) == RoundUp(offsetof(CDlgHelp, m_HelpClose) + sizeof(CButton), __alignof(CDlgHelp)));
    }
};
