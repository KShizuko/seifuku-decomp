#include "CppUnitTest.h"
#include <windows.h>
#include <cstddef>
#include "NpcXView.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct NpcXViewProbe
{
    static int Dir(CNpcXView* p) { return p->m_nDir; }
    static NpcXTypeInfo* Info(CNpcXView* p) { return p->m_pInfo; }
    static size_t OffDir() { return offsetof(CNpcXView, m_nDir); }
    static size_t OffInfo() { return offsetof(CNpcXView, m_pInfo); }
};

static_assert(sizeof(CNpcXView) == 0xF0, "");
static_assert(_ROLE_VIEW_NPCX == 10, "");

TEST_CLASS(RoleViewNpcXView)
{
public:
    TEST_METHOD(TheNpcWithoutARecord)
    {
        Assert::IsTrue(NpcXViewProbe::OffDir() == 0xE8 && NpcXViewProbe::OffInfo() == 0xEC);
        IRoleView* pView = RoleViewCreate(_ROLE_VIEW_NPCX);
        Assert::IsNotNull(pView);
        CNpcXView* p = static_cast<CNpcXView*>(pView);
        Assert::IsTrue(NpcXViewProbe::Dir(p) == -1 && NpcXViewProbe::Info(p) == NULL && p->m_pRole != NULL);
        Assert::IsFalse(pView->SetVariable(_VAR_DIR, 1) != 0);
        Assert::IsFalse(pView->SetVariable(_VAR_DIRECT_DIR, 1) != 0);
        Assert::IsFalse(pView->SetVariable(_VAR_ROTATEX, 1) != 0);
        Assert::IsFalse(pView->SetVariable(_VAR_BLEND, 1) != 0);
        Assert::IsTrue(NpcXViewProbe::Dir(p) == -1 && p->m_nRotateZ == 0);
        pView->SetPos(1.0f, 2.0f, 3.0f, 90, 1.0f);
        Assert::IsTrue(p->m_nRotateZ == 90 && p->m_fPosX == 1.0f);
        Assert::AreEqual(0u, (unsigned) pView->GetVariable(_VAR_RWEAPON_ID));
        pView->SetRGBA(1, 1, 1, 1);
        pView->SetLightOffset(2, 0, 0, 0, 1, 1, 1, 1);
        Assert::IsTrue(p->m_nShadowType == 2);
        Assert::AreEqual(0u, (unsigned) pView->Release());
    }
};
