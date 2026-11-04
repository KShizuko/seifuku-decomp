#include "stdafx.h"
#include "MyEditRect.h"
#include "CppUnitTest.h"
#include <cstddef>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(CMyEditRect) - sizeof(CButton) >= 4 && sizeof(CMyEditRect) - sizeof(CButton) <= 8, "");

struct MyEditRectProbe : CMyEditRect
{
    static size_t Parent() { return offsetof(MyEditRectProbe, m_pParentWnd); }
    CWnd* ParentWnd() const { return m_pParentWnd; }
};

TEST_CLASS(MyEditRectLayout)
{
public:
    TEST_METHOD(TheParentFollowsTheButton)
    {
        Assert::IsTrue(MyEditRectProbe::Parent() == sizeof(CButton));
    }

    TEST_METHOD(InitStoresTheParent)
    {
        MyEditRectProbe rect;
        Assert::IsTrue(rect.ParentWnd() == NULL);
        CWnd wnd;
        rect.Init(&wnd);
        Assert::IsTrue(rect.ParentWnd() == &wnd);
        rect.Init(NULL);
        Assert::IsTrue(rect.ParentWnd() == NULL);
    }
};
