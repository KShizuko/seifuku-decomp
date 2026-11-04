#include "stdafx.h"
#include "MyShell.h"
#include "MyButton.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CMyButton) == (sizeof(CButton) + 0x74 + __alignof(CButton) - 1) / __alignof(CButton) * __alignof(CButton), "");
static_assert(std::is_base_of<CButton, CMyButton>::value, "");
static_assert(std::is_polymorphic<CMyButton>::value, "");

TEST_CLASS(ClientMyButtonHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheHandlersReadThem)
    {
        const size_t b = sizeof(CButton);
        Assert::IsTrue(offsetof(CMyButton, m_BtnPnt) == b + 0x00);
        Assert::IsTrue(offsetof(CMyButton, m_IntPnt) == b + 0x08);
        Assert::IsTrue(offsetof(CMyButton, m_ShowRect) == b + 0x10);
        Assert::IsTrue(offsetof(CMyButton, m_nWidth) == b + 0x20);
        Assert::IsTrue(offsetof(CMyButton, m_nHeight) == b + 0x24);
        Assert::IsTrue(offsetof(CMyButton, m_uFrame) == b + 0x28);
        Assert::IsTrue(offsetof(CMyButton, m_btOwnerID) == b + 0x2C);
        Assert::IsTrue(offsetof(CMyButton, m_nButtonID) == b + 0x30);
        Assert::IsTrue(offsetof(CMyButton, m_strButtonID) == b + 0x34);
    }
};
