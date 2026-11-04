#include "stdafx.h"
#include "MyShell.h"
#include "MyCombo.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CMyCombo) == sizeof(CEdit) + 0x38 + sizeof(CMyEditRect), "");
static_assert(std::is_base_of<CEdit, CMyCombo>::value, "");
static_assert(std::is_polymorphic<CMyCombo>::value, "");

TEST_CLASS(ClientMyComboHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheBodiesReadThem)
    {
        const size_t b = sizeof(CEdit);
        Assert::IsTrue(offsetof(CMyCombo, m_IntPnt) == b + 0x00);
        Assert::IsTrue(offsetof(CMyCombo, m_pBmp) == b + 0x08);
        Assert::IsTrue(offsetof(CMyCombo, m_nWidth) == b + 0x0C);
        Assert::IsTrue(offsetof(CMyCombo, m_nHeight) == b + 0x10);
        Assert::IsTrue(offsetof(CMyCombo, m_bChinese) == b + 0x14);
        Assert::IsTrue(offsetof(CMyCombo, m_nStartChar) == b + 0x18);
        Assert::IsTrue(offsetof(CMyCombo, m_OffsetPoint) == b + 0x1C);
        Assert::IsTrue(offsetof(CMyCombo, m_nFontWidth) == b + 0x24);
        Assert::IsTrue(offsetof(CMyCombo, m_dwFontColor) == b + 0x28);
        Assert::IsTrue(offsetof(CMyCombo, m_dwBackColor) == b + 0x2C);
        Assert::IsTrue(offsetof(CMyCombo, m_nCharNum) == b + 0x30);
        Assert::IsTrue(offsetof(CMyCombo, m_CobLstBox) == b + 0x34);
        Assert::IsTrue(offsetof(CMyCombo, m_EditRect) == b + 0x38);
    }
};
