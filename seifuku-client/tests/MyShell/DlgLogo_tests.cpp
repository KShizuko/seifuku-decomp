#include "stdafx.h"
#include "MyShell.h"
#include "DlgLogo.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CDlgLogo) == (sizeof(CDialog) + 8 + __alignof(CDialog) - 1) / __alignof(CDialog) * __alignof(CDialog), "");
static_assert(std::is_base_of<CDialog, CDlgLogo>::value, "");
static_assert(std::is_polymorphic<CDlgLogo>::value, "");
static_assert(CDlgLogo::IDD == 273, "");

TEST_CLASS(ClientDlgLogoHeader)
{
public:
    TEST_METHOD(TheBrushFollowsCDialog)
    {
        Assert::IsTrue(offsetof(CDlgLogo, m_Hbr) == sizeof(CDialog));
        Assert::IsTrue(offsetof(CDlgLogo, m_hBmp) == sizeof(CDialog) + 4);
    }
};
