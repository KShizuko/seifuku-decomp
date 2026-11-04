#include "stdafx.h"
#include "MyShell.h"
#include "DlgSetup.h"
#include "Role.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgSetup>::value, "");
static_assert(std::is_polymorphic<CDlgSetup>::value, "");
static_assert(CDlgSetup::IDD == 138, "");
static_assert(MAX_INTERFACE_NUM == 2, "");
static_assert(ON_EXIT == 0 && ON_SETUP_MODECHANGE == 59 && ON_SETUP_HOTKEY == 60 && ON_SETUP_OTHER == 61 && ON_SETUP_SHIFT == 89 && ON_INTERFACE_CHANGE == 99 && ON_SETUP_KOSHOW == 105, "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_SETUP == 19, "");
static_assert(CRole::SHADOW_NONE == 0 && CRole::SHADOW_SIMPLE == 1 && CRole::SHADOW_REAL == 2, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgSetupProbe
{
    static size_t Brush() { return offsetof(CDlgSetup, m_Hbr); }
    static size_t CtlBrush() { return offsetof(CDlgSetup, m_CtlHbr); }
    static size_t InitSuc() { return offsetof(CDlgSetup, m_bInitSuc); }
    static size_t Pnt() { return offsetof(CDlgSetup, m_Pnt); }
    static size_t ScreenMode() { return offsetof(CDlgSetup, m_nScreenMode); }
};

TEST_CLASS(ClientDlgSetupHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgSetup, m_bShow) == b + 0x00);
        Assert::IsTrue(offsetof(CDlgSetup, m_nInterface) == b + 0x04);
        Assert::IsTrue(DlgSetupProbe::Brush() == b + 0x08);
        Assert::IsTrue(DlgSetupProbe::CtlBrush() == b + 0x0C);
        Assert::IsTrue(DlgSetupProbe::InitSuc() == b + 0x10);
        Assert::IsTrue(DlgSetupProbe::Pnt() == b + 0x14);
        Assert::IsTrue(DlgSetupProbe::ScreenMode() == b + 0x1C);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t right = RoundUp(b + 0x20, ab);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnRight) == right);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnMode) == right + btn);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnLeft) == right + 2 * btn);
        const size_t chk = RoundUp(right + 3 * btn, __alignof(CMyCheck));
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupChkNameopen) == chk);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupChkKoShow) == chk + sizeof(CMyCheck));
        const size_t other = RoundUp(chk + 2 * sizeof(CMyCheck), ab);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnOther) == other);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnCloses) == other + btn);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnHotkey) == other + 2 * btn);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnWinb) == other + 3 * btn);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnFullb) == other + 4 * btn);
        const size_t as = __alignof(CMySlider);
        const size_t sli = RoundUp(sizeof(CMySlider), as);
        const size_t music = RoundUp(other + 5 * btn, as);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupSliMusic) == music);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupSliEffect) == music + sli);
        const size_t win = RoundUp(music + 2 * sli, ab);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnWin) == win);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnFull) == win + btn);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnExit) == win + 2 * btn);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnDelete) == win + 3 * btn);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnReturn) == win + 4 * btn);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnReal) == win + 5 * btn);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnSimple) == win + 6 * btn);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnShadowclose) == win + 7 * btn);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupBtnHelp) == win + 8 * btn);
        const size_t ast = __alignof(CMyStatic);
        const size_t sta = RoundUp(sizeof(CMyStatic), ast);
        const size_t pic1 = RoundUp(win + 9 * btn, ast);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupPic1) == pic1);
        Assert::IsTrue(offsetof(CDlgSetup, m_SetupPic2) == pic1 + sta);
        Assert::IsTrue(sizeof(CDlgSetup) == RoundUp(pic1 + 2 * sta, __alignof(CDlgSetup)));
    }
};
