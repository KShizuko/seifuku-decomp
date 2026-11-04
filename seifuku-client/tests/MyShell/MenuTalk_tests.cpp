#include "stdafx.h"
#include "MyShell.h"
#include "MenuTalk.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CMenuTalk>::value, "");
static_assert(std::is_polymorphic<CMenuTalk>::value, "");
static_assert(MENU_TALK_CLICK == 5000 && G_CLICK == 5001 && H_CLICK == 5005 && GM_CLICK == 5006, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct MenuTalkProbe
{
    static int Idd() { return CMenuTalk::IDD; }
    static size_t Show() { return offsetof(CMenuTalk, m_bShow); }
    static size_t Brush() { return offsetof(CMenuTalk, m_Hbr); }
    static size_t Pnt() { return offsetof(CMenuTalk, m_Pnt); }
    static size_t Parent() { return offsetof(CMenuTalk, m_ParentWnd); }
    static size_t GM() { return offsetof(CMenuTalk, m_TalkBtnGM); }
    static size_t G() { return offsetof(CMenuTalk, m_TalkBtnG); }
    static size_t H() { return offsetof(CMenuTalk, m_TalkBtnH); }
};

TEST_CLASS(ClientMenuTalkHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        Assert::IsTrue(MenuTalkProbe::Idd() == 174);
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(MenuTalkProbe::Show() == b + 0x00);
        Assert::IsTrue(MenuTalkProbe::Brush() == b + 0x04);
        Assert::IsTrue(MenuTalkProbe::Pnt() == b + 0x08);
        Assert::IsTrue(MenuTalkProbe::Parent() == b + 0x10);
        const size_t a = __alignof(CMyButton);
        const size_t gm = RoundUp(b + 0x14, a);
        Assert::IsTrue(MenuTalkProbe::GM() == gm);
        Assert::IsTrue(MenuTalkProbe::G() == RoundUp(gm + sizeof(CMyButton), a));
        Assert::IsTrue(MenuTalkProbe::H() == RoundUp(gm + sizeof(CMyButton), a) + 4 * RoundUp(sizeof(CMyButton), a));
        Assert::IsTrue(sizeof(CMenuTalk) == RoundUp(MenuTalkProbe::H() + sizeof(CMyButton), __alignof(CMenuTalk)));
    }
};
