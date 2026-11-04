#include "CppUnitTest.h"
#include <windows.h>
#include <cstddef>
#include "3DStatuary.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(C3DStatuary) == 0xF8, "");
static_assert(offsetof(C3DStatuary, m_nActionType) == 0xE8 && offsetof(C3DStatuary, m_fScale) == 0xEC && offsetof(C3DStatuary, m_nFrame) == 0xF0 && offsetof(C3DStatuary, m_nDir) == 0xF4, "");
static_assert(_VAR_DIR == 19, "");

TEST_CLASS(RoleView3DStatuary)
{
public:
    TEST_METHOD(TheStatueAndItsOneShotVariables)
    {
        IRoleView* pView = RoleViewCreate(_ROLE_VIEW_STATUARY);
        Assert::IsNotNull(pView);
        C3DStatuary* p = static_cast<C3DStatuary*>(pView);
        Assert::IsTrue(p->m_nActionType == -1 && p->m_fScale == -1.0f && p->m_nFrame == -1 && p->m_nDir == -1);
        Assert::IsNotNull(p->m_pRole);
        Assert::IsTrue(pView->SetVariable(_VAR_FRAME, 7) != 0 && p->m_nFrame == 7 && p->m_dwFrame == 7);
        Assert::IsTrue(pView->SetVariable(_VAR_FRAME, 9) != 0 && p->m_nFrame == 7 && p->m_dwFrame == 7);
        Assert::IsTrue(pView->SetVariable(_VAR_DIR, 3) != 0 && p->m_nDir == 3);
        Assert::IsTrue(pView->SetVariable(_VAR_DIR, 5) != 0 && p->m_nDir == 3);
        Assert::IsTrue(pView->SetVariable(_VAR_ROTATEX, 11) != 0 && p->m_nRotateX == 11);
        Assert::IsTrue(pView->SetVariable(12, 1) != 0);
        Assert::IsTrue(pView->SetVariable(_VAR_RWEAPON, 0) != 0);
        Assert::AreEqual(0u, (unsigned) pView->GetVariable(_VAR_RWEAPON_ID));
        Assert::AreEqual(0u, (unsigned) pView->Release());
    }
};
