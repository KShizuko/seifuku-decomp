#include "stdafx.h"
#include "MyShell.h"
#include "DlgDelRole.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgDelRole>::value, "");
static_assert(std::is_polymorphic<CDlgDelRole>::value, "");
static_assert(CDlgDelRole::IDD == 283, "");
static_assert(IDC_DELROLE_STA_TEXT == 1454 && IDC_DELROLE_STA_TITLE == 1452 && IDC_DELROLE_EDT == 1451, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

TEST_CLASS(ClientDlgDelRoleHeader)
{
public:
    TEST_METHOD(LayoutFollowsTheCtor)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgDelRole, m_strCode) == b);
        const size_t sta = RoundUp(b + sizeof(CString), __alignof(CStatic));
        Assert::IsTrue(offsetof(CDlgDelRole, m_StaText) == sta);
        Assert::IsTrue(offsetof(CDlgDelRole, m_StaTitle) == sta + sizeof(CStatic));
        const size_t edt = RoundUp(sta + 2 * sizeof(CStatic), __alignof(CEdit));
        Assert::IsTrue(offsetof(CDlgDelRole, m_Edt) == edt);
        Assert::IsTrue(sizeof(CDlgDelRole) == RoundUp(edt + sizeof(CEdit), __alignof(CDlgDelRole)));
    }
};
