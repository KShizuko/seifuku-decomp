#include "CppUnitTest.h"
#include <windows.h>
#include <cstddef>
#include "TerrainNpcView.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct TerrainNpcViewProbe
{
    static OBJID Id(CTerrainNpcView* p) { return p->m_idNpc; }
    static size_t OffCell() { return offsetof(CTerrainNpcView, m_posCell); }
};

static_assert(sizeof(CTerrainNpcView) == 0x10, "");

TEST_CLASS(RoleViewTerrainNpcView)
{
public:
    TEST_METHOD(TheHusk)
    {
        Assert::IsTrue(TerrainNpcViewProbe::OffCell() == 0x08);
        IRoleView* pView = RoleViewCreate(_ROLE_VIEW_TERRAIN);
        Assert::IsNotNull(pView);
        CTerrainNpcView* p = static_cast<CTerrainNpcView*>(pView);
        Assert::AreEqual(0u, (unsigned) TerrainNpcViewProbe::Id(p));
        Assert::IsTrue(pView->SetVariable(_VAR_LOOK, 1230) != 0);
        Assert::IsFalse(pView->SetVariable(_VAR_DIR, 1) != 0);
        Assert::AreEqual(0u, (unsigned) TerrainNpcViewProbe::Id(p));
        Assert::AreEqual(100, pView->SetAction(100));
        Assert::IsFalse(pView->HitTest(0, 0, 0, 0) != 0);
        Assert::IsTrue(pView->Draw(0, 0) != 0);
        Assert::AreEqual(0u, (unsigned) pView->GetVariable(_VAR_LOOK));
        pView->SetPos(1, 2, 3, 4, 5); pView->SetLightOffset(1, 2, 3, 4, 5, 6, 7, 8); pView->SetRGBA(1, 1, 1, 1);
        pView->AddPartEffect(_ROLE_PART_BODY, "x"); pView->ClearPartEffect(_ROLE_PART_BODY, "x");
        Assert::IsFalse(pView->Reload() != 0);
        Assert::AreEqual(0u, (unsigned) pView->Release());
    }
};
