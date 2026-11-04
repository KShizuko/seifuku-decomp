#include "CppUnitTest.h"
#include <windows.h>
#include <cstddef>
#include "3DTarget.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct TargetProbe
{
    static int Dir(C3DTarget* p) { return p->m_nDir; }
    static BOOL Hit(C3DTarget* p) { return p->m_bHit; }
    static int FrameAmount(C3DTarget* p) { return p->m_nFrameAmount; }
    static I3DSimpleObj* Obj(C3DTarget* p) { return p->m_p3DSimpleObj; }
    static size_t OffObj() { return offsetof(C3DTarget, m_p3DSimpleObj); }
    static size_t OffTimer() { return offsetof(C3DTarget, m_dwTimer); }
    static size_t OffName() { return offsetof(C3DTarget, m_szName); }
};

static_assert(sizeof(C3DTarget) == 0x3C, "");

TEST_CLASS(RoleView3DTarget)
{
public:
    TEST_METHOD(TheTargetWithoutALook)
    {
        Assert::IsTrue(TargetProbe::OffName() == 0x1C && TargetProbe::OffObj() == 0x2C && TargetProbe::OffTimer() == 0x34);
        IRoleView* pView = RoleViewCreate(_ROLE_VIEW_TARGET);
        Assert::IsNotNull(pView);
        C3DTarget* p = static_cast<C3DTarget*>(pView);
        Assert::IsTrue(TargetProbe::Dir(p) == -1 && TargetProbe::Hit(p) == FALSE && TargetProbe::FrameAmount(p) == 1 && TargetProbe::Obj(p) == NULL);
        Assert::IsTrue(pView->SetVariable(_VAR_DIR, 2) != 0 && TargetProbe::Dir(p) == 2);
        Assert::IsTrue(pView->SetVariable(_VAR_DIR, 4) != 0 && TargetProbe::Dir(p) == 2);
        Assert::IsFalse(pView->SetVariable(_VAR_HAIR, 1) != 0);
        Assert::IsFalse(pView->SetVariable(_VAR_BLEND, 1) != 0);
        Assert::IsNull(TargetProbe::Obj(p));
        Assert::IsTrue(pView->SetVariable(_VAR_ROTATEX, 30) != 0);
        Assert::IsNotNull(TargetProbe::Obj(p));
        Assert::AreEqual(10, pView->SetAction(100));
        Assert::IsTrue(TargetProbe::Hit(p) == FALSE);
        Assert::AreEqual(10, pView->SetAction(320));
        Assert::IsTrue(TargetProbe::Hit(p) == TRUE);
        pView->SetPos(1.0f, 2.0f, 3.0f, 0, 1.0f);
        pView->SetLightOffset(2, 0, 0, 0, 1, 1, 1, 1);
        pView->SetLightOffset(0, 0, 0, 0, 1, 1, 1, 1);
        pView->SetRGBA(1, 1, 1, 1);
        pView->AddPartEffect(_ROLE_PART_BODY, "x");
        pView->ClearPartEffect(_ROLE_PART_BODY, "x");
        Assert::AreEqual(0u, (unsigned) pView->GetVariable(_VAR_RWEAPON_ID));
        Assert::AreEqual(0u, (unsigned) pView->Release());
    }
};
