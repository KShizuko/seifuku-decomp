#include "stdafx.h"
#include "MyShell.h"
#include "DlgInsHelp.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgInsHelp>::value, "");
static_assert(std::is_polymorphic<CDlgInsHelp>::value, "");
static_assert(CDlgInsHelp::IDD == 272, "");
static_assert(ON_INSHELP_SHOW == 100, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgInsHelpProbe
{
    static size_t Show() { return offsetof(CDlgInsHelp, m_bShow); }
    static size_t Brush() { return offsetof(CDlgInsHelp, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgInsHelp, m_Pnt); }
    static size_t Flash() { return offsetof(CDlgInsHelp, m_bFlash); }
    static size_t Tip() { return offsetof(CDlgInsHelp, m_bTipShow); }
    static size_t Index() { return offsetof(CDlgInsHelp, m_nCurTipIndex); }
};

TEST_CLASS(ClientDlgInsHelpHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgInsHelpProbe::Show() == b + 0x00);
        Assert::IsTrue(DlgInsHelpProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgInsHelpProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgInsHelpProbe::Flash() == b + 0x10);
        Assert::IsTrue(DlgInsHelpProbe::Tip() == b + 0x14);
        Assert::IsTrue(DlgInsHelpProbe::Index() == b + 0x18);
        const size_t a = __alignof(CMyButton);
        const size_t closeb = RoundUp(b + 0x1C, a);
        Assert::IsTrue(offsetof(CDlgInsHelp, m_InsHelpBtnCloseB) == closeb);
        Assert::IsTrue(offsetof(CDlgInsHelp, m_InsHelpBtnHelp) == closeb + 4 * RoundUp(sizeof(CMyButton), a));
        Assert::IsTrue(offsetof(CDlgInsHelp, m_BtnClose) == closeb + 6 * RoundUp(sizeof(CMyButton), a));
        Assert::IsTrue(sizeof(CDlgInsHelp) == RoundUp(offsetof(CDlgInsHelp, m_BtnClose) + sizeof(CMyButton), __alignof(CDlgInsHelp)));
    }
};
