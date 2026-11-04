#include "stdafx.h"
#include "MyShell.h"
#include "MenuGroup.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CMenuGroup>::value, "");
static_assert(std::is_polymorphic<CMenuGroup>::value, "");
static_assert(MENU_GROUP_CLICK == 5000 && GEAR_CLICK == 5001 && MONEY_CLICK == 5002 && ITEM_CLICK == 5003, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct MenuGroupProbe
{
    static int Idd() { return CMenuGroup::IDD; }
    static size_t Show() { return offsetof(CMenuGroup, m_bShow); }
    static size_t Create() { return offsetof(CMenuGroup, m_bCreate); }
    static size_t Brush() { return offsetof(CMenuGroup, m_Hbr); }
    static size_t Pnt() { return offsetof(CMenuGroup, m_Pnt); }
    static size_t Parent() { return offsetof(CMenuGroup, m_ParentWnd); }
    static size_t Stop() { return offsetof(CMenuGroup, m_GroupChkStop); }
    static size_t Money() { return offsetof(CMenuGroup, m_GroupChkMoney); }
    static size_t Item() { return offsetof(CMenuGroup, m_GroupChkItem); }
};

TEST_CLASS(ClientMenuGroupHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        Assert::IsTrue(MenuGroupProbe::Idd() == 268);
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(MenuGroupProbe::Show() == b + 0x00);
        Assert::IsTrue(MenuGroupProbe::Create() == b + 0x04);
        Assert::IsTrue(MenuGroupProbe::Brush() == b + 0x08);
        Assert::IsTrue(MenuGroupProbe::Pnt() == b + 0x0C);
        Assert::IsTrue(MenuGroupProbe::Parent() == b + 0x14);
        const size_t a = __alignof(CMyCheck);
        const size_t stop = RoundUp(b + 0x18, a);
        Assert::IsTrue(MenuGroupProbe::Stop() == stop);
        Assert::IsTrue(MenuGroupProbe::Money() == RoundUp(stop + sizeof(CMyCheck), a));
        Assert::IsTrue(MenuGroupProbe::Item() == RoundUp(MenuGroupProbe::Money() + sizeof(CMyCheck), a));
        Assert::IsTrue(sizeof(CMenuGroup) == RoundUp(MenuGroupProbe::Item() + sizeof(CMyCheck), __alignof(CMenuGroup)));
    }
};
