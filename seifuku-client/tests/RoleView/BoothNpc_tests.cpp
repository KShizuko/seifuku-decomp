#include "CppUnitTest.h"
#include <windows.h>
#include <cstddef>
#include "BoothNpc.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct BoothNpcProbe
{
    static DWORD Item0(CBoothNpc* p) { return p->m_dwItem0; }
    static DWORD Item1(CBoothNpc* p) { return p->m_dwItem1; }
    static size_t OffItem0() { return offsetof(CBoothNpc, m_dwItem0); }
};

static_assert(sizeof(CBoothNpc) == 0x54, "");
static_assert(_VAR_ITEM0 == 23 && _VAR_ITEM1 == 24, "");

TEST_CLASS(RoleViewBoothNpc)
{
public:
    TEST_METHOD(TheBoothAndItsMask)
    {
        Assert::IsTrue(BoothNpcProbe::OffItem0() == 0x4C);
        IRoleView* pView = RoleViewCreate(_ROLE_VIEW_BOOTH);
        Assert::IsNotNull(pView);
        CBoothNpc* p = static_cast<CBoothNpc*>(pView);
        Assert::IsTrue(BoothNpcProbe::Item0(p) == 0 && BoothNpcProbe::Item1(p) == 0);
        Assert::IsTrue(pView->SetVariable(_VAR_DIR, 1) != 0);
        Assert::IsFalse(pView->SetVariable(_VAR_HAIR, 1) != 0);
        Assert::AreEqual(1u, (unsigned) pView->GetVariable(_VAR_DIR));
        Assert::AreEqual(10, pView->SetAction(100));
        Assert::IsTrue(pView->HitTest(10, 20, 10, 20) != 0);
        Assert::IsTrue(pView->HitTest(9, 20, 10, 20) != 0);
        Assert::IsTrue(pView->HitTest(10, 21, 10, 20) != 0);
        Assert::IsFalse(pView->HitTest(9, 19, 10, 20) != 0);
        Assert::IsFalse(pView->HitTest(12, 20, 10, 20) != 0);
        Assert::AreEqual(0u, (unsigned) pView->Release());
    }
};
