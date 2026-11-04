#include "CppUnitTest.h"
#include <windows.h>
#include <cstddef>
#include "SynStone.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct SynStoneProbe
{
    static int Dir(CSynStone* p) { return p->m_nDir; }
    static int Look(CSynStone* p) { return p->m_nLook; }
    static I3DSimpleObj* Obj(CSynStone* p) { return p->m_p3DSimpleObj; }
    static size_t OffName() { return offsetof(CSynStone, m_szName); }
    static size_t OffObj() { return offsetof(CSynStone, m_p3DSimpleObj); }
    static size_t OffAmount() { return offsetof(CSynStone, m_nAmount); }
    static size_t OffVar() { return offsetof(CSynStone, m_nVarSet); }
    static size_t OffLook() { return offsetof(CSynStone, m_nLookSet); }
    static size_t OffMaxLife() { return offsetof(CSynStone, m_dwMaxLife); }
    static size_t OffLife() { return offsetof(CSynStone, m_dwLife); }
};

static_assert(sizeof(CSynStone) == 0x230, "");
static_assert(_VAR_MAX_LIFE == 21 && _VAR_LIFE == 22, "");

TEST_CLASS(RoleViewSynStone)
{
public:
    TEST_METHOD(TheStoneWithoutALook)
    {
        Assert::IsTrue(SynStoneProbe::OffName() == 0x0C && SynStoneProbe::OffObj() == 0x20 && SynStoneProbe::OffAmount() == 0x24);
        Assert::IsTrue(SynStoneProbe::OffVar() == 0x28 && SynStoneProbe::OffLook() == 0x128 && SynStoneProbe::OffMaxLife() == 0x228 && SynStoneProbe::OffLife() == 0x22C);
        IRoleView* pView = RoleViewCreate(_ROLE_VIEW_SYNSTONE);
        Assert::IsNotNull(pView);
        CSynStone* p = static_cast<CSynStone*>(pView);
        Assert::IsTrue(SynStoneProbe::Look(p) == 0 && SynStoneProbe::Dir(p) == -1 && SynStoneProbe::Obj(p) == NULL);
        Assert::IsFalse(pView->SetVariable(_VAR_HAIR, 1) != 0);
        Assert::IsFalse(pView->SetVariable(_VAR_DIR, 1) != 0);
        Assert::IsFalse(pView->SetVariable(23, 1) != 0);
        Assert::IsFalse(pView->SetVariable(_VAR_BLEND, 1) != 0);
        Assert::AreEqual(5, pView->SetAction(100));
        Assert::AreEqual(0xFFFFFFFFu, (unsigned) pView->GetVariable(_VAR_LOOK));
        Assert::AreEqual(0u, (unsigned) pView->GetVariable(_VAR_DIR));
        Assert::IsTrue(pView->SetVariable(_VAR_ROTATEX, 30) != 0);
        Assert::IsNotNull(SynStoneProbe::Obj(p));
        pView->SetPos(1.0f, 2.0f, 3.0f, 0, 1.0f);
        pView->SetLightOffset(2, 0, 0, 0, 1, 1, 1, 1);
        pView->SetRGBA(1, 1, 1, 1);
        pView->AddPartEffect(_ROLE_PART_BODY, "x");
        pView->ClearPartEffect(_ROLE_PART_BODY, "x");
        Assert::AreEqual(0u, (unsigned) pView->Release());
    }
};
