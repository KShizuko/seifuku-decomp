#include "stdafx.h"
#include "MyShell.h"
#include "DlgHotkey.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgHotkey>::value, "");
static_assert(std::is_polymorphic<CDlgHotkey>::value, "");
static_assert(HOTKEY_PARENT_AMOUNT * HOTKEY_CHILD_AMOUNT == 60, "");
static_assert(sizeof(g_nHotkeyValue) == 60 * sizeof(int), "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_HOTKEY == 25, "");
static_assert(HOTKEYF_SHIFT == 1 && HOTKEYF_CONTROL == 2 && HOTKEYF_ALT == 4, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgHotkeyProbe
{
    static int Idd() { return CDlgHotkey::IDD; }
    static size_t Brush() { return offsetof(CDlgHotkey, m_Hbr); }
    static size_t Black() { return offsetof(CDlgHotkey, m_HbrBlack); }
    static size_t Pnt() { return offsetof(CDlgHotkey, m_Pnt); }
    static size_t VirtualKey() { return offsetof(CDlgHotkey, m_wVirtualKey); }
    static size_t Modifiers() { return offsetof(CDlgHotkey, m_wModifiers); }
    static size_t HotKeyText() { return offsetof(CDlgHotkey, m_szHotKey); }
    static size_t HotKeyTextSize() { return sizeof(((CDlgHotkey*)0)->m_szHotKey); }
    static size_t BtnDel() { return offsetof(CDlgHotkey, m_BtnDel); }
    static size_t HotKey() { return offsetof(CDlgHotkey, m_HotKey); }
    static size_t StaOld() { return offsetof(CDlgHotkey, m_StaOld); }
    static size_t LstParent() { return offsetof(CDlgHotkey, m_LstParent); }
    static size_t LstChild() { return offsetof(CDlgHotkey, m_LstChild); }
    static size_t BtnCloseB() { return offsetof(CDlgHotkey, m_BtnCloseB); }
    static size_t BtnClose() { return offsetof(CDlgHotkey, m_BtnClose); }
    static size_t BtnRevert() { return offsetof(CDlgHotkey, m_BtnRevert); }
    static size_t BtnChange() { return offsetof(CDlgHotkey, m_BtnChange); }
    static size_t BtnHelp() { return offsetof(CDlgHotkey, m_BtnHelp); }
};

TEST_CLASS(ClientDlgHotkeyHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgHotkeyProbe::Idd() == 231);
        Assert::IsTrue(offsetof(CDlgHotkey, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgHotkeyProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgHotkeyProbe::Black() == b + 0x08);
        Assert::IsTrue(DlgHotkeyProbe::Pnt() == b + 0x0C);
        Assert::IsTrue(DlgHotkeyProbe::VirtualKey() == b + 0x14);
        Assert::IsTrue(DlgHotkeyProbe::Modifiers() == b + 0x16);
        Assert::IsTrue(DlgHotkeyProbe::HotKeyText() == b + 0x18);
        Assert::IsTrue(DlgHotkeyProbe::HotKeyTextSize() == 64);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t del = RoundUp(b + 0x58, ab);
        Assert::IsTrue(DlgHotkeyProbe::BtnDel() == del);
        const size_t hot = RoundUp(del + sizeof(CMyButton), __alignof(CHotKeyCtrl));
        Assert::IsTrue(DlgHotkeyProbe::HotKey() == hot);
        const size_t sta = RoundUp(hot + sizeof(CHotKeyCtrl), __alignof(CMyStatic));
        Assert::IsTrue(DlgHotkeyProbe::StaOld() == sta);
        const size_t al = __alignof(CMyList);
        const size_t lst = RoundUp(sta + sizeof(CMyStatic), al);
        Assert::IsTrue(DlgHotkeyProbe::LstParent() == lst);
        Assert::IsTrue(DlgHotkeyProbe::LstChild() == lst + RoundUp(sizeof(CMyList), al));
        const size_t closeb = RoundUp(lst + 2 * RoundUp(sizeof(CMyList), al), ab);
        Assert::IsTrue(DlgHotkeyProbe::BtnCloseB() == closeb);
        Assert::IsTrue(DlgHotkeyProbe::BtnClose() == closeb + 1 * btn);
        Assert::IsTrue(DlgHotkeyProbe::BtnRevert() == closeb + 2 * btn);
        Assert::IsTrue(DlgHotkeyProbe::BtnChange() == closeb + 3 * btn);
        Assert::IsTrue(DlgHotkeyProbe::BtnHelp() == closeb + 4 * btn);
        Assert::IsTrue(sizeof(CDlgHotkey) == RoundUp(closeb + 5 * btn, __alignof(CDlgHotkey)));
    }
};
