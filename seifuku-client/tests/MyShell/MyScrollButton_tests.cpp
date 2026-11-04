#include "stdafx.h"
#include "MyShell.h"
#include "MyScrollButton.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CMyScrollButton) == sizeof(CButton) + 0x58, "");
static_assert(std::is_base_of<CButton, CMyScrollButton>::value, "");
static_assert(std::is_polymorphic<CMyScrollButton>::value, "");

struct MyScrollButtonProbe
{
    static size_t Parent() { return offsetof(CMyScrollButton, m_ParentWnd); }
    static size_t Type() { return offsetof(CMyScrollButton, m_btScrBtnType); }
    static size_t Id() { return offsetof(CMyScrollButton, m_strScrButtonID); }
    static size_t Point() { return offsetof(CMyScrollButton, m_ScrBtnPnt); }
    static size_t Drop() { return offsetof(CMyScrollButton, m_bDrpValue); }
    static size_t Frame() { return offsetof(CMyScrollButton, m_uFrame); }
};

TEST_CLASS(ClientMyScrollButtonHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheHandlersReadThem)
    {
        const size_t b = sizeof(CButton);
        Assert::IsTrue(MyScrollButtonProbe::Parent() == b + 0x00);
        Assert::IsTrue(MyScrollButtonProbe::Type() == b + 0x04);
        Assert::IsTrue(MyScrollButtonProbe::Id() == b + 0x05);
        Assert::IsTrue(MyScrollButtonProbe::Point() == b + 0x48);
        Assert::IsTrue(MyScrollButtonProbe::Drop() == b + 0x50);
        Assert::IsTrue(MyScrollButtonProbe::Frame() == b + 0x54);
    }
};
